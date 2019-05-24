#include "Piece.h"
#include "PieceSolidificationGroup.h"
#include "PieceManager.h"
#include "Urho3D/Core/Context.h"
#include "Urho3D/Scene/Component.h"






PieceSolidificationGroup::PieceSolidificationGroup(Context* context) : Component(context)
{
	SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(PieceSolidificationGroup, HandleUpdate));
	//SubscribeToEvent(E_NODEADDED, URHO3D_HANDLER(PieceGroup, HandleNodeAdded));
	//SubscribeToEvent(E_NODEREMOVED, URHO3D_HANDLER(PieceGroup, HandleNodeRemoved));

	debugColor_.FromHSL(Random(1.0f), 1.0f, 0.5f, 0.2);
}

void PieceSolidificationGroup::RegisterObject(Context* context)
{
	context->RegisterFactory<PieceSolidificationGroup>();
}





void PieceSolidificationGroup::SetSolidified(bool solid)
{
	if (solid != solidify_) {
		solidify_ = solid;
		GetScene()->GetComponent<PieceManager>()->RebuildSolidifies();
	}
}

void PieceSolidificationGroup::GetPieces(ea::vector<Piece*>& pieces, int levels /*= 1*/, bool singleLevel /*= false*/)
{
	if (levels < 1)
		return;

	//resolve which root nodes to consider.
	ea::vector<Node*> nodes;
	if (!singleLevel || (singleLevel && levels == 1))
	{
		//add immediate children.
		node_->GetChildrenWithComponent<Piece>(nodes, false);
		for (Node* node : nodes) {
			pieces.push_back( node->GetComponent<Piece>() );
		}
		nodes.clear();
	}

	
	if (levels > 1) {
		node_->GetChildrenWithComponent<PieceSolidificationGroup>(nodes, false);
		for (Node* node : nodes) {
			node->GetComponent<PieceSolidificationGroup>()->GetPieces(pieces, levels - 1, singleLevel);
		}
	}

}


void PieceSolidificationGroup::DrawDebugGeometry(DebugRenderer* debug, bool depthTest)
{
	ea::vector<Piece*> pieces;
	GetPieces(pieces);
	for (Piece* piece : pieces) {

		debug->AddLine(node_->GetWorldPosition() + Vector3(debugColor_.r_, debugColor_.g_, debugColor_.b_)*0.2f, piece->GetNode()->GetWorldPosition(), debugColor_, depthTest);
	}
}

void PieceSolidificationGroup::Update()
{
}

void PieceSolidificationGroup::OnNodeSet(Node* node)
{
	if (node)
	{

	}
	else
	{

	}
}


void PieceSolidificationGroup::HandleUpdate(StringHash event, VariantMap& eventData)
{

}

void PieceSolidificationGroup::HandleNodeAdded(StringHash event, VariantMap& eventData)
{

}

void PieceSolidificationGroup::HandleNodeRemoved(StringHash event, VariantMap& eventData)
{

}



