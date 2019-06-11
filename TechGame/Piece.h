#pragma once

#include <Urho3D/Urho3DAll.h>
#include "Urho3D/IO/Log.h"

#include "NewtonPhysicsWorld.h"
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

	//returns the rigid body that is attached to the same node.
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


	PieceSolidificationGroup* GetPieceGroup()
	{
		return node_->GetParent()->GetComponent<PieceSolidificationGroup>();
	}

	//returns the rigid body that is enabled and is actually controlling this rigid body
	NewtonRigidBody* GetEffectiveRigidBody()
	{
		ea::vector<NewtonRigidBody*> bodies;
		GetRootRigidBodies(bodies, node_, false);
		for (int i = 0; i < bodies.size(); i++)
		{
			if (bodies[i]->IsEnabledEffective())
				return bodies[i];
		}
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







