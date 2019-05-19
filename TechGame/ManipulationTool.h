#pragma once
#include <Urho3D/Urho3DAll.h>
#include "Piece.h"
#include "PieceGroup.h"
#include "PiecePoint.h"
#include "PieceManager.h"

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




	void Gather();
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

	void formGatherContraption();

	void updateKinematicsControllerPos()
	{
		if (gatherPiecePoint_) {
			kinamaticConstriant_->SetOwnWorldPosition(gatherPiecePoint_->GetNode()->GetWorldPosition());
			kinamaticConstriant_->SetOwnWorldRotation(gatherPiecePoint_->GetNode()->GetWorldRotation());
		}
	}

	ea::weak_ptr<Node> gatherNode_;
	ea::weak_ptr<PieceGroup> gatheredPieceGroup_;
	ea::weak_ptr<Node> gatheredContraptionNode_;
	ea::weak_ptr<Piece> gatheredPiece_;
	ea::weak_ptr<PiecePoint> gatherPiecePoint_;
	ea::vector<PiecePoint*> allGatherPiecePoints_;
	ea::vector<Piece*> allGatherPieces_;
	
	Quaternion gatherRotation_;


	NewtonKinematicsControllerConstraint* kinamaticConstriant_;



	ea::weak_ptr<Piece> otherPiece_;
	ea::weak_ptr<PiecePoint> otherPiecePoint_;

	ea::shared_ptr<PieceManager> pieceManager_;
};

