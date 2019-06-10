#pragma once
#include <Urho3D/Urho3DAll.h>
#include "Piece.h"
#include "PieceSolidificationGroup.h"
#include "PiecePoint.h"
#include "PieceManager.h"

#include "NewtonKinematicsJoint.h"

//tool to be attached to Camera Node (or arm node)
class Tool : public Component {
	URHO3D_OBJECT(Tool, Component);
public:
	Tool(Context* context) : Component(context)
	{

	}

	static void RegisterObject(Context* context)
	{
		context->RegisterFactory<Tool>();
	}

};




class ManipulationTool : public Tool {
	URHO3D_OBJECT(ManipulationTool, Tool);
public:
	ManipulationTool(Context* context);

	static void RegisterObject(Context* context);




	bool Gather(bool grabOne = false);
	bool IsGathering();
	void UnGather(bool freeze);

	void AdvanceGatherPoint(bool forward = true);

	void RotateGatherNode(Quaternion rotation)
	{
		gatherNode_->Rotate(rotation, TS_PARENT);
	}


	//rotates the gather node to next nearest rotation within the current contraption.
	void RotateNextNearest();








	PiecePoint* GetClosestAimPiecePoint();


	Piece* GetClosestAimPiece(Vector3& worldPos);

protected:


	void HandleUpdate(StringHash eventType, VariantMap& eventData);


	virtual void OnNodeSet(Node* node) override;

	void drop(bool freeze);

	void formGatherContraption(bool onlyOne = false);

	void updateKinematicsControllerPos()
	{
		if (gatherPiecePoint_) {
			kinamaticConstriant_->SetOwnWorldPosition(gatherPiecePoint_->GetNode()->GetWorldPosition());
			kinamaticConstriant_->SetOwnWorldRotation(gatherPiecePoint_->GetNode()->GetWorldRotation());
		}
	}

	WeakPtr<Node> gatherNode_;
	WeakPtr<PieceSolidificationGroup> gatheredPieceGroup_;
	bool gatheredPieceGroupFromExisting = false;
	WeakPtr<Piece> gatheredPiece_;
	WeakPtr<PiecePoint> gatherPiecePoint_;
	ea::vector<PiecePoint*> allGatherPiecePoints_;
	ea::vector<Piece*> allGatherPieces_;
	
	Quaternion gatherRotation_;


	WeakPtr<NewtonKinematicsControllerConstraint> kinamaticConstriant_;



	WeakPtr<Piece> otherPiece_;
	WeakPtr<PiecePoint> otherPiecePoint_;

	SharedPtr<PieceManager> pieceManager_;
};

