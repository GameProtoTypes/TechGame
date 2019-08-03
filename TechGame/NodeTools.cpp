#include "NodeTools.h"

void GetCommonParentsWithComponent(ea::vector<Node*>& nodes, ea::vector<Node*>& parents, StringHash componentType)
{
	ea::vector<ea::vector<Node*>> parentStacks;
	for (Node* node : nodes)
	{
		parentStacks.push_back(ea::vector<Node*>());

		Node* curParent = node->GetParent();
		while (curParent) {
			
			if(curParent->HasComponent(componentType) || componentType == StringHash::ZERO)
				parentStacks.back().insert(parentStacks.back().begin(), curParent);
			
			curParent = curParent->GetParent();
		}
	}

	int curIdx = 0;
	bool keepGoing = true;
	Node* curCompareNode = nullptr;
	while (keepGoing) {

		for (int i = 0; i < parentStacks.size(); i++) {

			if (parentStacks[i].size() > curIdx)
			{
				if (curCompareNode == nullptr) {
					curCompareNode = parentStacks[i][curIdx];
				}

				if (parentStacks[i][curIdx] != curCompareNode) {
					keepGoing = false;
				}
			}
			else
			{
				keepGoing = false;
			}
		}

		if (keepGoing)
		{
			parents.insert(parents.begin(), curCompareNode);
		}
		curIdx++;
	}
}

Node* GetCommonParentWithComponent(ea::vector<Node*>& nodes, StringHash componentType)
{
	ea::vector<Node*> parents;
	GetCommonParentsWithComponent(nodes, parents, componentType);

	if (parents.size())
	{
		return parents.back();
	}
	return nullptr;
}

Urho3D::Vector3 GetNodePositionAverage(ea::vector<Node*>& nodes)
{
	Vector3 pos;
	for (Node* node : nodes)
	{
		pos += node->GetWorldPosition();
	}

	pos /= float(nodes.size());
	return pos;
}

void SetNodeHeirarchyTemporary(Node* rootNode, bool enable, bool recursive/* = true*/)
{
	ea::vector<Node*> children;
	rootNode->GetChildren(children, true);
	children.push_back(rootNode);

	for (Node* node : children) {
		node->SetTemporary(enable);
	}
}

Node* GetOrCreateChildNode(Node* parent, const ea::string& name)
{
	Node* node = parent->GetChild(name);
	if (!node)
	{
		return parent->CreateChild(name);
	}
	else
	{
		return node;
	}
}

