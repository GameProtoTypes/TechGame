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


		colorIndicatorStMdl_->GetMaterial()->SetShaderParameter("MatDiffColor", color.ToVector4());
		colorIndicatorStMdl_->SetEnabled(showColorIndicator_);
	}
}

void PiecePoint::OnNodeSet(Node* node)
{
	if (node)
	{

		//make color indicator
		SharedPtr<Material> mat = GetSubsystem<ResourceCache>()->GetResource<Material>("Materials/DefaultMaterial.xml")->Clone();
\
		colorIndicatorNode_ = node_->CreateChild();

		colorIndicatorNode_->SetScale(0.02f);
		colorIndicatorStMdl_ = colorIndicatorNode_->CreateComponent<StaticModel>();
		colorIndicatorStMdl_->SetModel(GetSubsystem<ResourceCache>()->GetResource<Model>("Models/Sphere.mdl"));

		mat->SetTechnique(0, GetSubsystem<ResourceCache>()->GetResource<Technique>("Techniques/DiffOverlay.xml"));
		colorIndicatorStMdl_->SetMaterial(mat);
		colorIndicatorStMdl_->SetEnabled(false);
	}
	else
	{

	}
}

