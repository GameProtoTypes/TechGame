#pragma once
#include "Urho3D/Urho3DAll.h"

/// Returns common parents to all nodes.
void GetCommonParentsWithComponent(ea::vector<Node*>& nodes, ea::vector<Node*>& parents, StringHash componentType = StringHash::ZERO);

/// Return the closest common parent to all nodes.
Node* GetCommonParentWithComponent(ea::vector<Node*>& nodes, StringHash componentType = StringHash::ZERO);

/// Return the average world position of all nodes.
Vector3 GetNodePositionAverage(ea::vector<Node*>& nodes);