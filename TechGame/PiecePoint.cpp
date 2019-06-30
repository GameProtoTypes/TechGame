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

void PiecePoint::SetShowIndicator(bool enable)
{
	if (showIndicator_ != enable)
	{
		showIndicator_ = enable;
		if (showIndicator_)
		{
			visNode = node_->CreateChild("visNode");
			visNode->SetScale(0.1f);
			StaticModel* stMdl = visNode->CreateComponent<StaticModel>();
			stMdl->SetModel(GetSubsystem<ResourceCache>()->GetResource<Model>("Models/LinePrimitives/Basis.mdl"));
		}
		else
		{
			visNode->RemoveComponent<StaticModel>();
			
			if (visNode)
				visNode->Remove();
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

