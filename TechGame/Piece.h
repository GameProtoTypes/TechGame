#pragma once

#include <Urho3D/Urho3DAll.h>
#include "Urho3D/IO/Log.h"

#include "NewtonRigidBody.h"


#include "PieceSolidificationGroup.h"
#include "NodeTools.h"



class PiecePoint;
class PiecePointRow;
class PieceSolidificationGroup;
class Piece : public Component {
	URHO3D_OBJECT(Piece, Component);
public:
	friend class PieceSolidificationGroup;

	Piece(Context* context);

	static void RegisterObject(Context* context);

	NewtonRigidBody* GetRigidBody() {
		return node_->GetComponent<NewtonRigidBody>();
	}

	StaticModel* GetStaticModel() {
		return node_->GetComponent<StaticModel>(true);
	}

	void GetPointRows(ea::vector<PiecePointRow*>& rows);

	void GetAttachedRows(ea::vector<PiecePointRow*>& rows);


	void GetPoints(ea::vector<PiecePoint*>& points);

	void GetAttachedPieces(ea::vector<Piece*>& pieces, bool recursive);


	void DetachAll();

	bool IsEffectivelySolidified();


	///return the piece groups the piece is part of.  the first PieceGroup in vector is the parent node's PieceGroup,
	///Followed by ancestors further and further up the tree.
	///The Last PieceGroup is the most root Group.
	void GetPieceGroups(ea::vector<PieceSolidificationGroup*>& pieceGroups);

	PieceSolidificationGroup* GetNearestPieceGroup()
	{
		ea::vector<PieceSolidificationGroup*> pieceGroups;
		GetPieceGroups(pieceGroups);
		if (pieceGroups.size())
			return pieceGroups.front();

		return nullptr;
	}

	bool IsPartOfPieceGroup(PieceSolidificationGroup* group);

	Color primaryColor_;

protected:

	Matrix3x4 lastRigBodyTransform_;

	virtual void OnNodeSet(Node* node) override;

	void GetAttachedPiecesRec(ea::vector<Piece*>& pieces, bool recursive);

	void HandlePhysicsPostStep(StringHash eventType, VariantMap& eventData);

	void HandleNodeCollisionStart(StringHash eventType, VariantMap& eventData);
	void HandleNodeCollisionEnd(StringHash eventType, VariantMap& eventData);
};







