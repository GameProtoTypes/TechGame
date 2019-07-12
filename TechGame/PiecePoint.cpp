#include "PiecePoint.h"
#include "Piece.h"
#include "PieceManager.h"










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


		if (showColorIndicator_)
		{
			SharedPtr<Material> mat = GetSubsystem<ResourceCache>()->GetResource<Material>("Materials/DefaultMaterial.xml")->Clone();

			if (colorIndicatorNode_.Expired()) {
				colorIndicatorNode_ = node_->CreateChild();

				colorIndicatorNode_->SetScale(0.02f);
				colorIndicatorStMdl_ = colorIndicatorNode_->GetOrCreateComponent<StaticModel>();
				colorIndicatorStMdl_->SetModel(GetSubsystem<ResourceCache>()->GetResource<Model>("Models/Sphere.mdl"));

				mat->SetTechnique(0, GetSubsystem<ResourceCache>()->GetResource<Technique>("Techniques/DiffOverlay.xml"));
				colorIndicatorStMdl_->SetMaterial(mat);
				URHO3D_LOGINFO("making " + ea::to_string((int)(void*)this));
			}
			mat->SetShaderParameter("MatDiffColor", color.ToVector4());
		}
		else
		{
			if (colorIndicatorNode_) {
				colorIndicatorStMdl_->Remove();
				colorIndicatorNode_->Remove();
				colorIndicatorNode_ = nullptr;
				URHO3D_LOGINFO("removing " + ea::to_string((int)(void*)this));
			}
		}
	}
}

void PiecePoint::OnNodeSet(Node* node)
{
	if (node)
	{
	}
	else
	{

	}
}

