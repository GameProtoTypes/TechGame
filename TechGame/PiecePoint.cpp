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

void PiecePoint::OnNodeSet(Node* node)
{
}

