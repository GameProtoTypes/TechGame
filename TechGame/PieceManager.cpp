#include "PieceManager.h"
#include "Piece.h"
#include "PiecePoint.h"
#include "PiecePointRow.h"


#include "EASTL/sort.h"

PiecePoint* PieceManager::GetClosestPiecePoint(Vector3 worldPosition, Piece* piece)
{
	ea::vector<PiecePoint*> points;
	piece->GetNode()->GetDerivedComponents<PiecePoint>(points, true);
	if (points.size()) {

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

Piece* PieceManager::GetClosestGlobalPiece(Vector3 worldPosition, ea::vector<Piece*> blacklist, float radius)
{
	Octree* octTree = GetScene()->GetComponent<Octree>();

	ea::vector<Drawable*> result;
	SphereOctreeQuery query(result, Sphere(worldPosition, radius));
	octTree->GetDrawables(query);


	Piece* closestPiece = nullptr;
	float closestDist = M_LARGE_VALUE;
	for (Drawable* drawable : result) {


		Piece* piece = drawable->GetNode()->GetParent()->GetComponent<Piece>(false);

		if (piece && !blacklist.contains(piece)) {
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
	ea::vector<Piece*> blacklist;
	Piece* piece = GetClosestGlobalPiece(worldPosition, blacklist, 5);

	if (piece)
	{
		return GetClosestPiecePoint(worldPosition, piece);
	}
	else
		return nullptr;
}

void PieceManager::GetPointsInRadius(ea::vector<PiecePoint*>& points, Vector3 worldPosition, float radius)
{
	Octree* octTree = GetScene()->GetComponent<Octree>();

	ea::vector<Drawable*> result;
	SphereOctreeQuery query(result, Sphere(worldPosition, radius));
	octTree->GetDrawables(query);

	for (Drawable* drawable : result)
	{
		Piece* piece = drawable->GetNode()->GetParent()->GetComponent<Piece>(false);
		if (piece)
		{
			ea::vector<PiecePoint*> pointsInPiece;
			piece->GetPoints(pointsInPiece);

			for (PiecePoint* point : pointsInPiece) {

				float dist = (point->GetNode()->GetWorldPosition() - worldPosition).Length();

				if (dist <= radius) {
					points.push_back(point);
				}
			}
		}
	}

}







void PieceManager::GetPointsAroundPoints(ea::vector<PiecePoint*>& inPieces, ea::vector<PiecePoint*>& outPieces, float radius)
{
	outPieces.clear();

	//Form a list of all potential points that we could attach too.
	ea::vector<PiecePoint*> comparisonPoints;
	for (PiecePoint* point : inPieces) {
		GetPointsInRadius(comparisonPoints, point->GetNode()->GetWorldPosition(), radius);
	}

	ea::vector<PiecePoint*> finalList;
	for (PiecePoint* point : comparisonPoints)
	{
		if(!inPieces.contains(point))
			finalList.push_back(point);
	}

	//remove duplicates

	eastl::sort(finalList.begin(), finalList.end());
	finalList.erase(ea::unique(finalList.begin(), finalList.end()), finalList.end());

	for (PiecePoint* piece : finalList)
	{
		outPieces.push_back(piece);
	}
}





























PieceSolidificationGroup* PieceManager::AddPiecesToNewGroup(ea::vector<Piece*> pieces)
{
	ea::vector<Node*> nodes;
	for (Piece* pc : pieces)
	{
		nodes.push_back(pc->GetNode());
	}

	Node* commonParent = GetCommonParentWithComponent(nodes);

	// create new group as child of scene
	if (commonParent == GetScene())
	{
		Node* newGroupNode = commonParent->CreateChild();

		newGroupNode->SetWorldPosition(GetNodePositionAverage(nodes));


		PieceSolidificationGroup* newGroup = newGroupNode->CreateComponent<PieceSolidificationGroup>();

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
		PieceSolidificationGroup* newGroup = newGroupNode->CreateComponent<PieceSolidificationGroup>();
		commonParent->SetParent(newGroupNode);

		RebuildSolidifies();

		return newGroup;
	}
}

void PieceManager::MovePieceToGroup(Piece* piece, PieceSolidificationGroup* group)
{
	ea::vector<Piece*> list;
	list.push_back(piece);

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

PieceSolidificationGroup* PieceManager::GetCommonGroup(ea::vector<Piece*> pieces)
{
	ea::vector<Node*> nodes;
	for (Piece* piece : pieces)
	{
		nodes.push_back(piece->GetNode());
	}

	Node* commonGroupNode = GetCommonParentWithComponent(nodes, PieceSolidificationGroup::GetTypeStatic());
	if (commonGroupNode) {
		return commonGroupNode->GetComponent<PieceSolidificationGroup>();
	}
	return nullptr;
}

void PieceManager::RemovePiecesFromFirstCommonGroup(ea::vector<Piece*> pieces)
{
	ea::vector<Node*> nodes;
	for (Piece* pc : pieces)
	{
		nodes.push_back(pc->GetNode());
	}
	Node* commonParent = GetCommonParentWithComponent(nodes, PieceSolidificationGroup::GetTypeStatic());

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

void PieceManager::StripGroups(const ea::vector<Piece*>& pieces)
{
	ea::vector<Node*> oldParents;
	for (Piece* pc : pieces)
	{
		oldParents.push_back(pc->GetNode()->GetParent());
		pc->GetNode()->SetParent(GetScene());
		URHO3D_LOGINFO("Piece Moved To Scene.");
	}
	
	//remove dangling group nodes.
	for (Node* oldParent : oldParents)
	{
		Node* curParent = oldParent;
		while (!curParent->GetNumChildren()) {
			Node* rem = curParent;
			curParent = curParent->GetParent();
			rem->Remove();
			URHO3D_LOGINFO("Old PieceGroup-Node removed.");
		}
	}

	RebuildSolidifies();
}

void PieceManager::RemoveUnnecesaryGroup(Piece* piece)
{
	PieceSolidificationGroup* group = piece->GetNearestPieceGroup();

	if (group)
	{
		ea::vector<Piece*> pieces;
		group->GetPieces(pieces, 99);
		if (pieces.size() <= 1)
		{
			StripGroups(pieces);
			URHO3D_LOGINFO("removed not necessary group.");
		}
	}

}

void PieceManager::RemoveGroup(PieceSolidificationGroup* group)
{
	ea::vector<Node*> children;
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

	if (startNode->HasComponent<PieceSolidificationGroup>()) 
	{
		if (!branchSolidified && startNode->GetComponent<PieceSolidificationGroup>()->GetSolidified())
		{

			branchSolidified = true;
			startNode->GetComponent<PieceSolidificationGroup>()->isEffectivelySolidified_ = true;
			NewtonRigidBody* body = startNode->GetOrCreateComponent<NewtonRigidBody>();
			body->SetEnabled(true);
		}
		else
		{
			startNode->GetComponent<PieceSolidificationGroup>()->isEffectivelySolidified_ = false;
			startNode->RemoveComponent<NewtonRigidBody>();
		}
	}


	ea::vector<Node*> children;
	startNode->GetChildren(children);

	for (Node* child : children)
	{
		if (child->GetComponent<Piece>()) {
			child->GetComponent<NewtonRigidBody>()->SetEnabled(!branchSolidified);
			
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

void PieceManager::FindLoops(Piece* piece, ea::vector<ea::vector<Piece*>>& loops) {
	ea::vector<Piece*> traverseList;
	FindLoops(piece, loops, traverseList, 0);
}

void PieceManager::FindLoops(Piece* piece, ea::vector<ea::vector<Piece*>>& loops, ea::vector<Piece*>& traverseList, int depth)
{

	//if the traverse list contains the piece - we may have detected a loop.
	if (traverseList.contains(piece)) {
		int loopSize = traverseList.index_of(piece) + 1;

		if (loopSize >= 3)
		{
			loops.push_back(ea::vector<Piece*>());
			//its a cycle.  count how far back the piece is in the visited list to determine how long the cycle is and what nodes are present.
			for (int i = traverseList.index_of(piece); i >= 0; i--) {
				//URHO3D_LOGINFO(String((int)(void*)traverseList[i]));
				loops.back().push_back(traverseList[i]);
			}

		}
		return;
	}

	traverseList.insert(traverseList.begin(), piece);

	ea::vector<PiecePointRow*> attachedRows;
	piece->GetAttachedRows(attachedRows);

	for (PiecePointRow* row : attachedRows) {
		FindLoops(row->GetPiece(), loops, traverseList, depth + 1);
	}
	traverseList.erase(0);
}


void PieceManager::HandleNodeAdded(StringHash event, VariantMap& eventData)
{
	Node* node = (Node*)eventData[NodeAdded::P_NODE].GetPtr();
	Node* parentNode = (Node*)eventData[NodeAdded::P_PARENT].GetPtr();
	Scene* scene = (Scene*)eventData[NodeAdded::P_SCENE].GetPtr();

	if (scene == GetScene() && parentNode->HasComponent<PieceSolidificationGroup>())
	{
		RebuildSolidifies();
	}

}

void PieceManager::HandleNodeRemoved(StringHash event, VariantMap& eventData)
{
	Node* node = (Node*)eventData[NodeRemoved::P_NODE].GetPtr();
	Node* parentNode = (Node*)eventData[NodeRemoved::P_PARENT].GetPtr();
	Scene* scene = (Scene*)eventData[NodeRemoved::P_SCENE].GetPtr();

	if (scene == GetScene() && parentNode->HasComponent<PieceSolidificationGroup>())
	{
		RebuildSolidifies();
	}
}

