#include "Piece.h"
#include "PieceSolidificationGroup.h"
#include "PieceManager.h"
#include "Urho3D/Core/Context.h"
#include "Urho3D/Scene/Component.h"






PieceSolidificationGroup::PieceSolidificationGroup(Context* context) : Component(context)
{
	solidStateStack_.push_back(true);

	SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(PieceSolidificationGroup, HandleUpdate));
	//SubscribeToEvent(E_NODEADDED, URHO3D_HANDLER(PieceGroup, HandleNodeAdded));
	//SubscribeToEvent(E_NODEREMOVED, URHO3D_HANDLER(PieceGroup, HandleNodeRemoved));

	debugColor_.FromHSL(Random(1.0f), Random(1.0f), Random(1.0f), 0.2);
}

void PieceSolidificationGroup::RegisterObject(Context* context)
{
	context->RegisterFactory<PieceSolidificationGroup>();
}





void PieceSolidificationGroup::SetSolidified(bool solid)
{
	if (solidStateStack_.back() != solid) {
		solidStateStack_.back() = solid;
		GetScene()->GetComponent<PieceManager>()->RebuildSolidifies();
	}
}

bool PieceSolidificationGroup::GetEffectivelySolidified() const
{
	//walk down the tree
	bool solid = solidStateStack_.back();
	bool hasSolid = solid;
	Node* curNode = node_;
	while (curNode)
	{
		if (curNode->GetComponent<PieceSolidificationGroup>())
		{
			solid = curNode->GetComponent<PieceSolidificationGroup>()->solidStateStack_.back();
			if (solid)
				hasSolid = true;
		}
		curNode = curNode->GetParent();
	}

	return hasSolid;
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
	Vector3 colorHSV = debugColor_.ToHSV();

	if (GetEffectivelySolidified())
		colorHSV.z_ = 1.0f;
	else
		colorHSV.z_ = 0.1f;

	Color c;
	c.FromHSV(colorHSV.x_, colorHSV.y_, colorHSV.z_);

	//draw sphere at the group node.
	debug->AddSphere(Sphere(node_->GetWorldPosition(), 0.2f),c, depthTest);
	
	//draw lines to subgroups.
	ea::vector<Node*> childrenGroupNodes;
	node_->GetChildren(childrenGroupNodes, false);
	for (Node* node : childrenGroupNodes)
	{

		Vector3 occlusionOffset = Vector3(0.01, 0.01, 0.01);

		debug->AddLine(node_->GetWorldPosition(), node->GetWorldPosition() + occlusionOffset, c, depthTest);
	}

	//draw lines to sub-pieces.
	ea::vector<Piece*> pieces;
	GetPieces(pieces);
	for (Piece* piece : pieces) {

		debug->AddLine(node_->GetWorldPosition(), piece->GetNode()->GetWorldPosition(), c, depthTest);
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



