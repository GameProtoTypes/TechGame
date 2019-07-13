#include "Piece.h"
#include "PiecePoint.h"
#include "PiecePointRow.h"

#include "NewtonPhysicsEvents.h"

#include "VisualDebugger.h"


Piece::Piece(Context* context) : Component(context)
{
	SubscribeToEvent(E_NEWTON_PHYSICSPOSTSTEP, URHO3D_HANDLER(Piece, HandlePhysicsPostStep));
}

void Piece::RegisterObject(Context* context)
{
	context->RegisterFactory<Piece>();
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

			rows.push_back(attachedRow.rowA_);
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

void Piece::SetGhostingEffect(bool enable)
{
	if (enable != ghostingEffectOn_)
	{
		ghostingEffectOn_ = enable;

		RefreshVisualMaterial();
	}
}

void Piece::RefreshVisualMaterial()
{
	Material* ghostMat = GetVisualNode()->GetComponent<StaticModel>(false)->GetMaterial();

	//make sure there is a unique material object for the static model:
	if (!ghostMat) {
		SharedPtr<Material> mat = GetSubsystem<ResourceCache>()->GetResource<Material>("Materials/Piece.xml")->Clone();
		GetVisualNode()->GetComponent<StaticModel>(false)->SetMaterial(mat);
		ghostMat = mat;
	}




	
	if (ghostingEffectOn_) {
		ghostMat->SetTechnique(0, GetSubsystem<ResourceCache>()->GetResource<Technique>("Techniques/DiffEmissiveAlpha.xml"));
		ghostMat->SetShaderParameter("MatDiffColor", Color(1.0f,1.0f,1.0f,0.7f));
	}
	else
	{
		ghostMat->SetTechnique(0, GetSubsystem<ResourceCache>()->GetResource<Technique>("Techniques/Diff.xml"));
		ghostMat->SetShaderParameter("MatDiffColor", primaryColor_.ToVector4());
	}

	
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
				
				attachedPiece = attachedRow.rowA_->GetPiece();

				if (!pieces.contains(attachedPiece))
				{
					pieces.push_back(attachedPiece);
					if (recursive)
						attachedRow.rowA_->GetPiece()->GetAttachedPiecesRec(pieces, recursive);
				}
			}
		}
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


bool Piece::IsEffectivelySolidified()
{
	PieceSolidificationGroup* nearestGroup = GetPieceGroup();
	
	if(nearestGroup)
		return nearestGroup->GetEffectivelySolidified();


	return false;
}


bool Piece::IsPartOfPieceGroup(PieceSolidificationGroup* group)
{
	if (GetPieceGroup() == group && group != nullptr)
		return true;

	return false;
}

void Piece::OnNodeSet(Node* node)
{
	if (node) {


	}
	else
	{

	}
}

