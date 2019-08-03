#pragma once
#include "Urho3D/Urho3DAll.h"

/// Returns common parents to all nodes.
void GetCommonParentsWithComponent(ea::vector<Node*>& nodes, ea::vector<Node*>& parents, StringHash componentType = StringHash::ZERO);

/// Return the closest common parent to all nodes.
Node* GetCommonParentWithComponent(ea::vector<Node*>& nodes, StringHash componentType = StringHash::ZERO);

/// Return the average world position of all nodes.
Vector3 GetNodePositionAverage(ea::vector<Node*>& nodes);

/// Sets the temporary flag on rootNode and all of its children.
void SetNodeHeirarchyTemporary(Node* rootNode, bool enable, bool recursive = true);

/// Gets or Creates a child node with name under the given parent.
Node* GetOrCreateChildNode(Node* parent, const ea::string& name);