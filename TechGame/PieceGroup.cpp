#include "Piece.h"
#include "PieceGroup.h"
#include "PieceManager.h"
#include "Urho3D/Core/Context.h"
#include "Urho3D/Scene/Component.h"






PieceGroup::PieceGroup(Context* context) : Component(context)
{
	SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(PieceGroup, HandleUpdate));
	//SubscribeToEvent(E_NODEADDED, URHO3D_HANDLER(PieceGroup, HandleNodeAdded));
	//SubscribeToEvent(E_NODEREMOVED, URHO3D_HANDLER(PieceGroup, HandleNodeRemoved));

	debugColor_.FromHSL(Random(1.0f), 1.0f, 0.5f, 0.2);
}

void PieceGroup::RegisterObject(Context* context)
{
	context->RegisterFactory<PieceGroup>();
}





void PieceGroup::SetSolidified(bool solid)
{
	if (solid != solidify_) {
		solidify_ = solid;
		GetScene()->GetComponent<PieceManager>()->RebuildSolidifies();
	}
}

void PieceGroup::GetPieces(PODVector<Piece*>& pieces, int levels /*= 1*/, bool singleLevel /*= false*/)
{
	if (levels < 1)
		return;

	//resolve which root nodes to consider.
	PODVector<Node*> nodes;
	if (!singleLevel || (singleLevel && levels == 1))
	{
		//add immediate children.
		node_->GetChildrenWithComponent<Piece>(nodes, false);
		for (Node* node : nodes) {
			pieces += node->GetComponent<Piece>();
		}
		nodes.Clear();
	}

	
	if (levels > 1) {
		node_->GetChildrenWithComponent<PieceGroup>(nodes, false);
		for (Node* node : nodes) {
			node->GetComponent<PieceGroup>()->GetPieces(pieces, levels - 1, singleLevel);
		}
	}

}


void PieceGroup::DrawDebugGeometry(DebugRenderer* debug, bool depthTest)
{
	PODVector<Piece*> pieces;
	GetPieces(pieces);
	for (Piece* piece : pieces) {

		debug->AddLine(node_->GetWorldPosition(), piece->GetNode()->GetWorldPosition(), debugColor_, depthTest);
	}


	PODVector<Node*> childGroups;
	node_->GetChildrenWithComponent<PieceGroup>(childGroups);
	for (Node* childGroupNode : childGroups)
	{
		debug->AddLine(node_->GetWorldPosition(), childGroupNode->GetWorldPosition(), debugColor_, depthTest);
	}
}

void PieceGroup::Update()
{
}

void PieceGroup::OnNodeSet(Node* node)
{
	if (node)
	{

	}
	else
	{

	}
}


void PieceGroup::HandleUpdate(StringHash event, VariantMap& eventData)
{

}

void PieceGroup::HandleNodeAdded(StringHash event, VariantMap& eventData)
{

}

void PieceGroup::HandleNodeRemoved(StringHash event, VariantMap& eventData)
{

}



