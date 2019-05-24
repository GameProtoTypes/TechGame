#pragma once
#include "Urho3D/Urho3DAll.h"


//class to manage pieces on a scene level. (attach to scene node)
class Piece;
class PieceGroup;
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


	




	///Creates a new common group surrounding all pieces. 
	PieceGroup* AddPiecesToNewGroup(ea::vector<Piece*> pieces);

	///move a piece to an existing group potentially changing its position in the group tree.
	void MovePieceToGroup(Piece* piece, PieceGroup* group);

	///return the first common group for the given pieces.
	PieceGroup* GetCommonGroup(ea::vector<Piece*> pieces);

	///finds the most child-like common group and removes all pieces from it.
	void RemovePiecesFromFirstCommonGroup(ea::vector<Piece*> pieces);

	///sets all pieces with no grouping. (moves nodes to scene)  cleans up afterwards.
	void StripGroups(const ea::vector<Piece*>& pieces);

	///removes the group if the piece is the only member of the group.
	void RemoveUnnecesaryGroup(Piece* piece);
	///
	void RemoveGroup(PieceGroup* group);

	///Resolves solidification state for group trees starting at startNode. (Best if scene is used as startNode)
	void RebuildSolidifiesSub(Node* startNode, bool branchSolidified = false);
	void RebuildSolidifies();



	void FormGroups(Piece* startingPiece);











	void FindLoops(Piece* piece, ea::vector<ea::vector<Piece*>>& loops);
	void FindLoops(Piece* piece, ea::vector<ea::vector<Piece*>>& loops, ea::vector<Piece*>& traverseStack, int depth);

protected:

	void HandleNodeAdded(StringHash event, VariantMap& eventData);
	void HandleNodeRemoved(StringHash event, VariantMap& eventData);

};

