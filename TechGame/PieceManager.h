#pragma once
#include "Urho3D/Urho3DAll.h"


//class to manage pieces on a scene level. (attach to scene node)
class Piece;
class PieceSolidificationGroup;
class PiecePoint;
class PieceManager : public Component
{
	URHO3D_OBJECT(PieceManager, Component);

public:

	PieceManager(Context* context) : Component(context)
	{
		SubscribeToEvent(E_NODEADDED, URHO3D_HANDLER(PieceManager, HandleNodeAdded));
		SubscribeToEvent(E_NODEREMOVED, URHO3D_HANDLER(PieceManager, HandleNodeRemoved));
	}

	static void RegisterObject(Context* context)
	{
		context->RegisterFactory<PieceManager>();
	}


	float GetScaleFactor() { return 0.025 / 0.1; }

	float GetAttachPointThreshold() { return 0.02f; }


	Piece* GetClosestGlobalPiece(Vector3 worldPosition, ea::vector<Piece*> blacklist, float radius);

	PiecePoint* GetClosestGlobalPiecePoint(Vector3 worldPosition);

	PiecePoint* GetClosestPiecePoint(Vector3 worldPosition, Piece* piece);


	void GetPointsInRadius(ea::vector<PiecePoint*>& pieces, Vector3 worldPosition, float radius);

	///given inPieces - returns a list of possible connection pieces using radius searches around each point.  outPieces should contain no duplicates and contain no points from inPieces.
	void GetPointsAroundPoints(ea::vector<PiecePoint*>& inPieces, ea::vector<PiecePoint*>& outPieces, float radius);


	


	///create a new node with group component
	Node* CreateGroupNode(Node* parent);

	///Creates a new common group surrounding all pieces. 
	PieceSolidificationGroup* AddPiecesToNewSolidGroup(ea::vector<Piece*> pieces);

	///creates a new group around the piece (inside outer existing groups)
	PieceSolidificationGroup* CreateSolidGroupAroundPiece(Piece* piece);

	///create a new group around an existing group (inside outer existing groups)
	PieceSolidificationGroup* CreateSolidGroupAroundGroup(PieceSolidificationGroup* group);


	///move a piece to an existing group potentially changing its position in the group tree. optionally clean the old group.
	void MovePieceToSolidGroup(Piece* piece, PieceSolidificationGroup* group, bool clean = true);

	///return the first common group for the given pieces.
	//PieceSolidificationGroup* GetCommonSolidGroup(ea::vector<Piece*> pieces);

	///finds the most child-like common group and removes all pieces from it.
	//void RemovePiecesFromFirstCommonSolidGroup(ea::vector<Piece*> pieces);

	///sets a piece with no grouping. optionally clean
	void RemovePieceFromGroups(Piece* piece, bool postClean = true);

	///sets all pieces with no grouping. (moves nodes to scene)  cleans up afterwards.
	void RemovePiecesFromGroups(const ea::vector<Piece*>& pieces, bool postClean = true);

	///removes the group if the piece is the only member of the group.
	//void RemoveUnnecesarySolidGroup(Piece* piece);
	
	///Removes the group from the group tree, all children are re-parented to the parent of the given group.
	void RemoveSolidGroup(PieceSolidificationGroup* group);

	///Resolves solidification state for group trees starting at startNode. (Best if scene is used as startNode)
	void RebuildSolidifiesSub(Node* startNode, bool branchSolidified = false);

	///Resolves solidification state for all groups.
	void RebuildSolidifies();

	///removes groups if the node has no piece's on children nodes. continues down the tree.
	void CleanGroups(Node* node);


	//void FormGroups(Piece* startingPiece);











	void FindLoops(Piece* piece, ea::vector<ea::vector<Piece*>>& loops);
	void FindLoops(Piece* piece, ea::vector<ea::vector<Piece*>>& loops, ea::vector<Piece*>& traverseStack, int depth);

protected:

	void HandleNodeAdded(StringHash event, VariantMap& eventData);
	void HandleNodeRemoved(StringHash event, VariantMap& eventData);

};

