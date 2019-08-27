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


	float GetTweekScaleFactor() { return 0.5f; };

	//global scale factors and distances.
	float GetScaleFactor() { return 0.25*GetTweekScaleFactor(); }

	float GetAttachPointThreshold() { return 0.08*GetScaleFactor(); }
	//distance between 2 points in a row.
	float RowPointDistance() { return GetScaleFactor()*0.5f; }


	//runtime options
	bool enableDynamicRodDetach_ = true;
	void SetEnableDynamicRodDetachment(bool enable) { enableDynamicRodDetach_ = enable; }
	bool GetEnableDynamicRodDetachment() const { return enableDynamicRodDetach_; }



	//piece creation

	Node* CreatePiece(ea::string name, bool loadExisting);

	ea::vector<Piece*> CreatePieceAssembly(ea::string name, bool loadExisting);




	//contraption utils
	void GetAllPointsInContraption(Piece* pieceInContraption, ea::vector<PiecePoint*>& points);












	Piece* GetClosestGlobalPiece(Vector3 worldPosition, ea::vector<Piece*> blacklist, float radius);

	void GetClosestGlobalPieces(Vector3 worldPosition, ea::vector<Piece*> blacklist, float radius, ea::vector<Piece*>& pieces, int maxPieces = 5);
	
	void GetGlobalPiecesInRadius(Vector3 worldPosition, ea::vector<Piece*> blacklist, float radius, ea::vector<Piece*>& pieces, int maxPieces = 5);



	PiecePoint* GetClosestGlobalPiecePoint(Vector3 worldPosition, ea::vector<Piece*>& blacklist, float radius, int maxPieces);

	PiecePoint* GetClosestPiecePoint(Vector3 worldPosition, Piece* piece);


	void GetPointsInRadius(ea::vector<PiecePoint*>& pieces, Vector3 worldPosition, float radius);

	Piece* GetClosestAimPiece(Vector3& worldPos, Node* lookNode);

	PiecePoint* GetClosestAimPiecePoint(Node* lookNode);
	///given inPieces - returns a list of possible connection pieces using radius searches around each point.  outPieces should contain no duplicates and contain no points from inPieces.
	void GetPointsAroundPoints(ea::vector<PiecePoint*>& inPieces, ea::vector<PiecePoint*>& outPieces, float radius);



	///create a new node with group component as child of parent at worldPosition
	Node* CreateGroupNode(Node* parent, Vector3 worldPosition);



	///move a piece to an existing group potentially changing its position in the group tree. optionally clean the old group.
	void MovePieceToSolidGroup(Piece* piece, PieceSolidificationGroup* group, bool clean = true);
	void MovePiecesToSolidGroup(ea::vector<Piece*>& pieces, PieceSolidificationGroup* group, bool clean = true);

	///return the first common group for the given pieces.
	PieceSolidificationGroup* GetCommonSolidGroup(ea::vector<Piece*> pieces);

	///sets a piece with no grouping. optionally cleanh
	void RemovePieceFromGroup(Piece* piece, bool postClean = true);

	///sets all pieces with no grouping. (moves nodes to scene)  cleans up afterwards.
	void RemovePiecesFromGroups(const ea::vector<Piece*>& pieces, bool postClean = true);


	///Removes the group from the group tree, all children are re-parented to the parent of the given group.
	void RemoveSolidGroup(PieceSolidificationGroup* group);

	///Resolves solidification state for group trees starting at startNode. (Best if scene is used as startNode)
	void RebuildSolidifiesSub(Node* startNode, bool branchSolidified = false);

	///Resolves solidification state for all groups.
	void RebuildSolidifies();

	///removes groups if the node has no piece's on children nodes. continues down the tree.
	void CleanGroups(Node* node);

	void CleanAll();

	///find all pieces that are rigidly connected starting at the startingPiece.
	void GetRigidlyConnectedPieces(Piece* startingPiece, ea::vector<Piece*>& pieces);
	
	///form the largest solid group starting at the given piece.
	PieceSolidificationGroup*  FormSolidGroup(Piece* startingPiece);

	
	void ClearAllGroups();

	void FormSolidGroupsOnContraption(Piece* startingPiece);
	void AutoFormAllGroups();


	void FindLoops(Piece* piece, ea::vector<ea::vector<Piece*>>& loops);
	void FindLoops(Piece* piece, ea::vector<ea::vector<Piece*>>& loops, ea::vector<Piece*>& traverseStack, int depth);

protected:

	void HandleNodeAdded(StringHash event, VariantMap& eventData);
	void HandleNodeRemoved(StringHash event, VariantMap& eventData);

};

