#include "PiecePoint.h"
#include "Piece.h"
#include "PieceManager.h"
#include "PiecePointRow.h"









PiecePoint::PiecePoint(Context* context) : Component(context)
{

}

void PiecePoint::RegisterObject(Context* context)
{
	context->RegisterFactory<PiecePoint>();
}

void PiecePoint::DrawDebugGeometry(DebugRenderer* debug, bool depthTest)
{
	debug->AddFrame(node_->GetWorldTransform(), 0.125f*0.25f, Color::RED, Color::RED, Color::RED, depthTest);
	debug->AddLine(node_->GetWorldPosition(), node_->GetWorldPosition() + GetDirectionWorld() * 0.125f*0.25f, Color::GREEN, depthTest);

}

bool PiecePoint::OccupancyCompatible(PiecePoint* point)
{
	return true;
}



Piece* PiecePoint::GetPiece()
{
	return node_->GetParent()->GetComponent<Piece>();
}

void PiecePoint::SetShowBasisIndicator(bool enable)
{
	if (showBasisIndicator_ != enable)
	{
		showBasisIndicator_ = enable;
		if (showBasisIndicator_)
		{
			if(basisIndicatorNode_.Expired())
				basisIndicatorNode_ = node_->CreateChild();
			
			basisIndicatorNode_->SetScale(0.1f);
			basisIndicatorStMdl_ = basisIndicatorNode_->CreateComponent<StaticModel>();
			basisIndicatorStMdl_->SetModel(GetSubsystem<ResourceCache>()->GetResource<Model>("Models/LinePrimitives/Basis.mdl"));
		}
		else
		{
			basisIndicatorNode_->RemoveComponent(basisIndicatorStMdl_);
			
			if (basisIndicatorNode_->GetNumComponents() <= 0)
				basisIndicatorNode_->Remove();
		}
	}
}


void PiecePoint::SetShowColorIndicator(bool enable, Color color)
{
	if ((showColorIndicator_ != enable) || (colorIndicatorColor_ != color))
	{
		showColorIndicator_ = enable;
		colorIndicatorColor_ = color;


		URHO3D_LOGINFO("color update.");
		colorIndicatorStMdl_->GetMaterial()->SetShaderParameter("MatDiffColor", color.ToVector4());
		colorIndicatorStMdl_->SetEnabled(showColorIndicator_);
	}
}

bool PiecePoint::Weld()
{
	if (occupiedPoint_) {

		isWelded = true;
		occupiedPoint_->isWelded = true;
		URHO3D_LOGINFO("Welding..");
		GetScene()->GetComponent<PieceManager>()->FormSolidGroup(GetPiece());
		GetScene()->GetComponent<PieceManager>()->FormSolidGroup(occupiedPoint_->GetPiece());
	}
	else
		return false;
}

bool PiecePoint::UnWeld()
{
	if (occupiedPoint_ && isWelded) {
		URHO3D_LOGINFO("UnWelding.");
		isWelded = false;
		occupiedPoint_->isWelded = false;

		GetScene()->GetComponent<PieceManager>()->RemovePieceFromGroup(GetPiece());
		GetScene()->GetComponent<PieceManager>()->RemovePieceFromGroup(occupiedPoint_->GetPiece());




		GetScene()->GetComponent<PieceManager>()->FormSolidGroup(GetPiece());
		GetScene()->GetComponent<PieceManager>()->FormSolidGroup(occupiedPoint_->GetPiece());
		return true;
	}
	return false;
}

bool PiecePoint::SaveXML(XMLElement& dest) const
{
	bool s = Component::SaveXML(dest);
	s &= dest.SetUInt("ParentRowId", row_->GetID());
	return s;
}

bool PiecePoint::LoadXML(const XMLElement& source)
{
	bool s = Component::LoadXML(source);

	rowId_ = source.GetUInt("ParentRowId");
	if (rowId_ == 0)
		s = false;
	
	
	return s;
}

void PiecePoint::ApplyAttributes()
{
	SceneResolver sceneResolver;
	Component* comp = GetScene()->GetComponent(rowId_);
	sceneResolver.AddComponent(rowId_, comp);
	sceneResolver.Resolve();
	row_ = dynamic_cast<PiecePointRow*>(GetScene()->GetComponent(comp->GetID()));
}



void PiecePoint::OnNodeSet(Node* node)
{
	if (node)
	{

		//make color indicator
		SharedPtr<Material> mat = GetSubsystem<ResourceCache>()->GetResource<Material>("Materials/DefaultMaterial.xml")->Clone();

		colorIndicatorNode_ = node_->CreateChild();

		colorIndicatorNode_->SetScale(0.02f);
		colorIndicatorStMdl_ = colorIndicatorNode_->CreateComponent<StaticModel>();
		colorIndicatorStMdl_->SetModel(GetSubsystem<ResourceCache>()->GetResource<Model>("Models/Sphere.mdl"));


		mat->SetTechnique(0, GetSubsystem<ResourceCache>()->GetResource<Technique>("Techniques/NoTextureOverlay.xml"));
		colorIndicatorStMdl_->SetMaterial(mat);
		colorIndicatorStMdl_->SetEnabled(false);
	}
	else
	{

	}
}

