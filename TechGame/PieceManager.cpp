#include "PieceManager.h"
#include "Piece.h"
#include "PiecePoint.h"
#include "PiecePointRow.h"




PiecePoint* PieceManager::GetClosestPiecePoint(Vector3 worldPosition, Piece* piece)
{
	PODVector<PiecePoint*> points;
	piece->GetNode()->GetDerivedComponents<PiecePoint>(points, true);
	if (points.Size()) {

		PiecePoint* closest = nullptr;
		float closestDist = M_LARGE_VALUE;
		for (PiecePoint* point : points) {

			if ((point->GetNode()->GetWorldPosition() - worldPosition).Length() < closestDist)
			{
				closestDist = (point->GetNode()->GetWorldPosition() - worldPosition).Length();
				closest = point;
			}
		}
		return closest;
	}
	else
	{
		return nullptr;
	}
}

Piece* PieceManager::GetClosestGlobalPiece(Vector3 worldPosition, PODVector<Piece*> blacklist, float radius)
{
	Octree* octTree = GetScene()->GetComponent<Octree>();

	PODVector<Drawable*> result;
	SphereOctreeQuery query(result, Sphere(worldPosition, radius));
	octTree->GetDrawables(query);


	Piece* closestPiece = nullptr;
	float closestDist = M_LARGE_VALUE;
	for (Drawable* drawable : result) {


		Piece* piece = drawable->GetNode()->GetParent()->GetComponent<Piece>(false);

		if (piece && !blacklist.Contains(piece)) {
			float delta = (piece->GetNode()->GetWorldPosition() - worldPosition).Length();
			if (delta < closestDist)
			{
				closestDist = delta;
				closestPiece = piece;
			}
		}
	}

	return closestPiece;
}

PiecePoint* PieceManager::GetClosestGlobalPiecePoint(Vector3 worldPosition)
{
	PODVector<Piece*> blacklist;
	Piece* piece = GetClosestGlobalPiece(worldPosition, blacklist, 5);

	if (piece)
	{
		return GetClosestPiecePoint(worldPosition, piece);
	}
	else
		return nullptr;
}

void PieceManager::GetPointsInRadius(PODVector<PiecePoint*>& points, Vector3 worldPosition, float radius)
{
	Octree* octTree = GetScene()->GetComponent<Octree>();

	PODVector<Drawable*> result;
	SphereOctreeQuery query(result, Sphere(worldPosition, radius));
	octTree->GetDrawables(query);

	for (Drawable* drawable : result)
	{
		Piece* piece = drawable->GetNode()->GetParent()->GetComponent<Piece>(false);
		if (piece)
		{
			PODVector<PiecePoint*> pointsInPiece;
			piece->GetPoints(pointsInPiece);

			for (PiecePoint* point : pointsInPiece) {

				float dist = (point->GetNode()->GetWorldPosition() - worldPosition).Length();

				if (dist <= radius) {
					points += point;
				}
			}
		}
	}

}







void PieceManager::GetPointsAroundPoints(PODVector<PiecePoint*>& inPieces, PODVector<PiecePoint*>& outPieces, float radius)
{
	outPieces.Clear();

	//Form a list of all potential points that we could attach too.
	PODVector<PiecePoint*> comparisonPoints;
	for (PiecePoint* point : inPieces) {
		GetPointsInRadius(comparisonPoints, point->GetNode()->GetWorldPosition(), radius);
	}

	std::vector<PiecePoint*> finalList;
	for (PiecePoint* point : comparisonPoints)
	{
		if(!inPieces.Contains(point))
			finalList.push_back(point);
	}

	//remove duplicates
	std::sort(finalList.begin(), finalList.end());
	finalList.erase(unique(finalList.begin(), finalList.end()), finalList.end());

	for (PiecePoint* piece : finalList)
	{
		outPieces += piece;
	}
}





























PieceGroup* PieceManager::AddPiecesToNewGroup(PODVector<Piece*> pieces)
{
	PODVector<Node*> nodes;
	for (Piece* pc : pieces)
	{
		nodes += pc->GetNode();
	}

	Node* commonParent = GetCommonParentWithComponent(nodes);

	// create new group as child of scene
	if (commonParent == GetScene())
	{
		Node* newGroupNode = commonParent->CreateChild();

		newGroupNode->SetWorldPosition(GetNodePositionAverage(nodes));


		PieceGroup* newGroup = newGroupNode->CreateComponent<PieceGroup>();

		for (Piece* pc : pieces)
		{
			pc->GetNode()->SetParent(newGroupNode);
		}

		RebuildSolidifies();

		return newGroup;
	}
	else
	{
		//group the common group.
		Node* newGroupNode = commonParent->GetParent()->CreateChild();
		PieceGroup* newGroup = newGroupNode->CreateComponent<PieceGroup>();
		commonParent->SetParent(newGroupNode);

		RebuildSolidifies();

		return newGroup;
	}


}

void PieceManager::MovePieceToGroup(Piece* piece, PieceGroup* group)
{
	PODVector<Piece*> list;
	list += piece;

	Node* oldParent = piece->GetNode()->GetParent();
	piece->GetNode()->SetParent(group->GetNode());


	//clean old tree.
	Node* curParent = oldParent;
	while (!curParent->GetNumChildren()) {
		Node* rem = curParent;
		curParent = curParent->GetParent();
		rem->Remove();
	}

	RebuildSolidifies();
}

PieceGroup* PieceManager::GetCommonGroup(PODVector<Piece*> pieces)
{
	PODVector<Node*> nodes;
	for (Piece* piece : pieces)
	{
		nodes += piece->GetNode();
	}

	Node* commonGroupNode = GetCommonParentWithComponent(nodes, PieceGroup::GetTypeStatic());
	if (commonGroupNode) {
		return commonGroupNode->GetComponent<PieceGroup>();
	}
	return nullptr;
}

void PieceManager::RemovePiecesFromFirstCommonGroup(PODVector<Piece*> pieces)
{
	PODVector<Node*> nodes;
	for (Piece* pc : pieces)
	{
		nodes += pc->GetNode();
	}
	Node* commonParent = GetCommonParentWithComponent(nodes, PieceGroup::GetTypeStatic());

	if (commonParent)
	{
		for (Node* node : nodes)
		{
			node->SetParent(commonParent->GetParent());
		}
		commonParent->Remove();
	}

	RebuildSolidifies();
}

void PieceManager::StripGroups(PODVector<Piece*> pieces)
{
	PODVector<Node*> oldParents;
	for (Piece* pc : pieces)
	{
		oldParents += pc->GetNode()->GetParent();
		pc->GetNode()->SetParent(GetScene());
	}

	//remove dangling group nodes.
	for (Node* oldParent : oldParents)
	{
		Node* curParent = oldParent;
		while (!curParent->GetNumChildren()) {
			Node* rem = curParent;
			curParent = curParent->GetParent();
			rem->Remove();
		}
	}

	RebuildSolidifies();
}

void PieceManager::RemoveGroup(PieceGroup* group)
{
	PODVector<Node*> children;
	group->GetNode()->GetChildren(children);

	Node* parent = group->GetNode()->GetParent();
	for (Node* node : children)
	{
		node->SetParent(parent);
	}

	group->GetNode()->Remove();


	RebuildSolidifies();
}

void PieceManager::RebuildSolidifiesSub(Node* startNode, bool branchSolidified)
{

	if (startNode->HasComponent<PieceGroup>()) 
	{
		if (!branchSolidified && startNode->GetComponent<PieceGroup>()->GetSolidified())
		{

			branchSolidified = true;
			startNode->GetComponent<PieceGroup>()->isEffectivelySolidified_ = true;
			RigidBody* body = startNode->GetOrCreateComponent<RigidBody>();
			body->SetEnabled(true);
		}
		else
		{
			startNode->GetComponent<PieceGroup>()->isEffectivelySolidified_ = false;
			startNode->RemoveComponent<RigidBody>();
		}
	}


	PODVector<Node*> children;
	startNode->GetChildren(children);

	for (Node* child : children)
	{
		if (child->GetComponent<Piece>()) {
			child->GetComponent<RigidBody>()->SetEnabled(!branchSolidified);
			
		}
		else
		{
			RebuildSolidifiesSub(child, branchSolidified);
		}
	}
}

void PieceManager::RebuildSolidifies()
{
	RebuildSolidifiesSub(GetScene(), false);
}

void PieceManager::FormGroups(Piece* startingPiece)
{
	//Vector<Vector<Piece*>> loops;
	//FindLoops(startingPiece, loops);

	//for (int l = 0; l < loops.Size(); l++)
	//{

	//	PieceGroup* newGroup = GetScene()->CreateComponent<PieceGroup>();
	//	for (int p = 0; p < loops[l].Size(); p++)
	//	{

	//		PODVector<PieceGroup*> existingGroups;
	//		loops[l][p]->GetPieceGroups(existingGroups);
	//		if (existingGroups.Size()) {
	//			//already part of a piece group.
	//		}
	//		else
	//		{
	//			newGroup->AddPiece(loops[l][p]);
	//		}
	//	}

	//	if (newGroup->AllPieces().Size())
	//	{
	//		newGroup->Solidify();
	//	}
	//	else
	//	{
	//		newGroup->Remove();
	//	}
	//}
}

void PieceManager::FindLoops(Piece* piece, Vector<Vector<Piece*>>& loops) {
	Vector<Piece*> traverseList;
	FindLoops(piece, loops, traverseList, 0);
}

void PieceManager::FindLoops(Piece* piece, Vector<Vector<Piece*>>& loops, Vector<Piece*>& traverseList, int depth)
{

	//if the traverse list contains the piece - we may have detected a loop.
	if (traverseList.Contains(piece)) {
		int loopSize = traverseList.IndexOf(piece) + 1;

		if (loopSize >= 3)
		{
			loops.Push(Vector<Piece*>());
			//its a cycle.  count how far back the piece is in the visited list to determine how long the cycle is and what nodes are present.
			for (int i = traverseList.IndexOf(piece); i >= 0; i--) {
				//URHO3D_LOGINFO(String((int)(void*)traverseList[i]));
				loops.Back().Push(traverseList[i]);
			}

		}
		return;
	}

	traverseList.Insert(0, piece);

	PODVector<PiecePointRow*> attachedRows;
	piece->GetAttachedRows(attachedRows);

	for (PiecePointRow* row : attachedRows) {
		FindLoops(row->GetPiece(), loops, traverseList, depth + 1);
	}
	traverseList.Erase(0);
}


void PieceManager::HandleNodeAdded(StringHash event, VariantMap& eventData)
{
	Node* node = (Node*)eventData[NodeAdded::P_NODE].GetPtr();
	Node* parentNode = (Node*)eventData[NodeAdded::P_PARENT].GetPtr();
	Scene* scene = (Scene*)eventData[NodeAdded::P_SCENE].GetPtr();

	if (scene == GetScene() && parentNode->HasComponent<PieceGroup>())
	{
		RebuildSolidifies();
	}

}

void PieceManager::HandleNodeRemoved(StringHash event, VariantMap& eventData)
{
	Node* node = (Node*)eventData[NodeRemoved::P_NODE].GetPtr();
	Node* parentNode = (Node*)eventData[NodeRemoved::P_PARENT].GetPtr();
	Scene* scene = (Scene*)eventData[NodeRemoved::P_SCENE].GetPtr();

	if (scene == GetScene() && parentNode->HasComponent<PieceGroup>())
	{
		RebuildSolidifies();
	}
}

