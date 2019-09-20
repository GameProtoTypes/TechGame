#include "Piece.h"
#include "PiecePoint.h"
#include "PiecePointRow.h"
#include "ColorPallet.h"

#include "NewtonPhysicsEvents.h"

#include "VisualDebugger.h"


Piece::Piece(Context* context) : Component(context)
{
	SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(Piece, HandleUpdate));
	SubscribeToEvent(E_NEWTON_PHYSICSPOSTSTEP, URHO3D_HANDLER(Piece, HandlePhysicsPostStep));
}

void Piece::RegisterObject(Context* context)
{
	context->RegisterFactory<Piece>();

	URHO3D_COPY_BASE_ATTRIBUTES(Component);
	URHO3D_ACCESSOR_ATTRIBUTE(PIECE_ATTRIB_PRIMARY_GHOST, GetGhostingEffectEnabled, SetGhostingEffectEnabled, bool, false, AM_DEFAULT);
	URHO3D_ACCESSOR_ATTRIBUTE(PIECE_ATTRIB_PRIMARY_COLOR, GetPrimaryColor, SetPrimaryColor, Color, Color::BLUE, AM_DEFAULT);
	URHO3D_ACCESSOR_ATTRIBUTE(PIECE_ATTRIB_PRIMARY_DYNAMIC_ATTACH, GetEnableDynamicDetachment, SetEnableDynamicDetachmentAttrib, bool, true, AM_DEFAULT);



}

void Piece::GetPointRows(ea::vector<PiecePointRow*>& rows)
{
	node_->GetDerivedComponents<PiecePointRow>(rows, true);
}

void Piece::GetAttachedRows(ea::vector<PiecePointRow*>& rows)
{
	ea::vector<PiecePointRow*> ownRows;
	GetPointRows(ownRows);

	for (PiecePointRow* row : ownRows) {
		for (PiecePointRow::RowAttachement& attachedRow : row->rowAttachements_) {

			rows.push_back(attachedRow.rowOther_);
		}
	}
}

void Piece::GetPoints(ea::vector<PiecePoint*>& points)
{
	node_->GetDerivedComponents<PiecePoint>(points, true, false);
}




void Piece::GetAttachedPieces(ea::vector<Piece*>& pieces, bool recursive)
{
	GetAttachedPiecesRec(pieces, recursive);
	pieces.erase_at(pieces.index_of(this));
}

void Piece::GetAssemblyPieces(ea::vector<Piece*>& pieces, bool includeThisPiece /*= true*/)
{
	pieces = assemblyPieces_;
	if (includeThisPiece)
		pieces.push_back(this);
}

bool Piece::AddAssemblyPiece(Piece* piece)
{
	if (piece != this) {
		assemblyPieces_.push_back(piece);
		return true;
	}
	else
		return false;
}

void Piece::SetPrimaryColor(Color color)
{
	if (primaryColor_ != color) {
		primaryColor_ = color;
		visualsDirty_ = true;
		useColorPallet_ = false;
	}
}

void Piece::SetEnableDynamicDetachmentAttrib(bool enable)
{
	enableDynamicDetachment_ = enable;
}

void Piece::SetEnableDynamicDetachment(bool enable)
{
	enableDynamicDetachment_ = enable;


	if (enable == true) {
		ea::vector<PiecePointRow*> rows;
		GetPointRows(rows);
		for (PiecePointRow* row : rows)
		{
			row->ResetOccupiedPointsCountDown();
		}
	}
}

void Piece::SetGhostingEffectEnabled(bool enable)
{
	if (ghostingEffectOn_ != enable) {
		ghostingEffectOn_ = enable;
		visualsDirty_ = true;
	}
}

void Piece::RefreshVisualMaterial()
{
	SharedPtr<Material> resolvedMaterial = SharedPtr<Material>(GetVisualNode()->GetComponent<StaticModel>(false)->GetMaterial());

	//make sure there is a unique material object for the static model:
	if (!resolvedMaterial) {
		SharedPtr<Material> mat = GetSubsystem<ResourceCache>()->GetResource<Material>("Materials/Piece.xml")->Clone();
		GetVisualNode()->GetComponent<StaticModel>(false)->SetMaterial(mat);
		resolvedMaterial = mat;
	}

	if (ghostingEffectOn_) {
		resolvedMaterial->SetTechnique(0, GetSubsystem<ResourceCache>()->GetResource<Technique>("Techniques/DiffEmissiveAlpha.xml"));
		resolvedMaterial->SetShaderParameter("MatDiffColor", Color(1.0f,1.0f,1.0f,0.7f));
	}
	else
	{
		resolvedMaterial->SetTechnique(0, GetSubsystem<ResourceCache>()->GetResource<Technique>("Techniques/Diff.xml"));

		Color resolvedColor;
		if (useColorPallet_) {
			resolvedColor = GetScene()->GetComponent<PieceManager>()->colorPalletManager_->GetPallet("default")->GetColorById(colorPalletId_);
		}
		else {
			resolvedColor = primaryColor_;
		}

	
		//URHO3D_LOGINFO("Resolved Color: " + resolvedColor.ToString());
		resolvedMaterial->SetShaderParameter("MatDiffColor", resolvedColor.ToVector4());
	}


	resolvedMaterial->SetShaderParameter("UOffset", Vector4(0.5, 0.0f, 1.0f, 1.0f));
	resolvedMaterial->SetShaderParameter("VOffset", Vector4(0.0f, 0.5f, 1.0f, 1.0f));
	
}

void Piece::GetAttachedPiecesRec(ea::vector<Piece*>& pieces, bool recursive)
{
	ea::vector<PiecePointRow*> rows;
	GetPointRows(rows);

	for (PiecePointRow* row : rows)
	{
		Piece* attachedPiece = nullptr;

		if (row->rowAttachements_.size()) {
			
			for (PiecePointRow::RowAttachement& attachedRow : row->rowAttachements_) {
				
				attachedPiece = attachedRow.rowOther_->GetPiece();

				if (!pieces.contains(attachedPiece))
				{
					pieces.push_back(attachedPiece);
					if (recursive)
						attachedRow.rowOther_->GetPiece()->GetAttachedPiecesRec(pieces, recursive);
				}
			}
		}
	}

	//also get pieces that are attached via normal constraints
	ea::vector<NewtonConstraint*> constraints;
	constraints = GetNode()->GetComponent<NewtonRigidBody>()->GetConnectedContraints();

	for (NewtonConstraint* cn : constraints) 
	{
		Piece* piece = nullptr;

		if (cn->GetOwnBody(false) == GetNode()->GetComponent<NewtonRigidBody>()) {
			NewtonRigidBody* otherRigidBody = cn->GetOtherBody();
			if (otherRigidBody) {
				Node* otherNode = otherRigidBody->GetNode();
				piece = otherNode->GetComponent<Piece>();
			}
		}
		else if (cn->GetOtherBody(false) == GetNode()->GetComponent<NewtonRigidBody>()) {
			NewtonRigidBody* otherRigidBody = cn->GetOwnBody();
			if (otherRigidBody) {
				Node* otherNode = otherRigidBody->GetNode();
				piece = otherNode->GetComponent<Piece>();
			}
		}

		bool s = (!pieces.contains(piece));
		
		if (piece != nullptr && s)
		{
			pieces.push_back(piece);
			if (recursive)
				piece->GetAttachedPiecesRec(pieces, recursive);
		}

	}



}


void Piece::HandleUpdate(StringHash eventType, VariantMap& eventData)
{
	if (visualsDirty_) {
		RefreshVisualMaterial();
		visualsDirty_ = false;
	}
}
void Piece::HandlePhysicsPostStep(StringHash eventType, VariantMap& eventData)
{

	//NewtonRigidBody* rigBody = GetRigidBody();
	//if ((rigBody->GetWorldPosition() - lastRigBodyTransform_.Translation()).Length() > 10.0f) {
	//	URHO3D_LOGINFO("Jumpy Rigid Body!");
	//	rigBody->SetWorldTransform(lastRigBodyTransform_);
	//	rigBody->SetLinearVelocity(Vector3::ZERO);
	//	rigBody->SetAngularVelocity(Vector3::ZERO);
	//}



	//lastRigBodyTransform_ = GetRigidBody()->GetWorldTransform();


}

void Piece::DetachAll()
{
	ea::vector<PiecePointRow*> rows;
	GetPointRows(rows);

	for (PiecePointRow* row : rows) {
		row->DetachAll();
	}

	//restore rigid body properties:
	GetRigidBody()->ClearCollisionExceptions();
}


void Piece::ReAttachAll()
{
	ea::vector<Piece*> assemblyPieces_;
	GetAttachedPieces(assemblyPieces_, false);
	for (Piece* pc : assemblyPieces_) {
		GetScene()->GetComponent<PieceManager>()->RemovePieceFromGroup(pc);
	}

	ea::vector<PiecePointRow*> rows;
	GetPointRows(rows);

	for (PiecePointRow* row : rows) {
		
		row->ReAttachAll();
	}

	GetScene()->GetComponent<PieceManager>()->AutoFormAllGroups();
}

bool Piece::IsEffectivelySolidified()
{
	PieceSolidificationGroup* nearestGroup = GetPieceGroup();
	
	if(nearestGroup)
		return nearestGroup->GetEffectivelySolidified();


	return false;
}


Urho3D::NewtonRigidBody* Piece::GetEffectiveRigidBody()
{
	if (GetPieceGroup())
	{
		return GetPieceGroup()->GetRigidBody();
	}

	ea::vector<NewtonRigidBody*> bodies;
	GetRootRigidBodies(bodies, node_, false);
	for (int i = 0; i < bodies.size(); i++)
	{
		if (bodies[i]->IsEnabledEffective())
			return bodies[i];
	}
	return nullptr;
}

bool Piece::IsPartOfPieceGroup(PieceSolidificationGroup* group)
{
	if (GetPieceGroup() == group && group != nullptr)
		return true;

	return false;
}

void Piece::ApplyAttributes()
{

}

void Piece::OnNodeSet(Node* node)
{
	if (node) {


	}
	else
	{

	}
}

