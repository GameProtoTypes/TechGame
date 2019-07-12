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

	enum MoveMode {
		MoveMode_Global = 0,
		MoveMode_Camera = 1
	};


	bool Gather(bool grabOne = false);
	bool IsGathering();
	void UnGather(bool freeze);

	void AdvanceGatherPoint(bool forward = true);

	void RotateGatherNode(Quaternion rotation)
	{
		gatherNode_->Rotate(rotation, TS_PARENT);
	}
	void ResetGatherNodeRotation()
	{
		if (MoveMode_Camera)
			gatherNode_->SetRotation(Quaternion::IDENTITY);
		else if(MoveMode_Global)
			gatherNode_->SetWorldRotation(Quaternion::IDENTITY);
	}

	void SetUseGrid(bool enable);
	bool GetUseGrid() const;
	void ToggleUseGrid();

	void SetMoveMode(MoveMode mode);
	MoveMode GetMoveMode();

	//resets the gather node on the gathered contraptions to where the camera is pointing.
	void AimPointForce();

	//rotates the gather node to next nearest rotation within the current contraption.
	void RotateNextNearest();

	void UpdateGatherIndicators();


protected:


	void HandleUpdate(StringHash eventType, VariantMap& eventData);


	virtual void OnNodeSet(Node* node) override;

	void drop(bool freeze, bool hadAttachement);

	void formGatherContraption(bool onlyOne = false);

	void updateKinematicsControllerPos(bool forceUpdate)
	{
		if (!gatherPiecePoint_)
			return;

		if (forceUpdate)
		{
			kinamaticConstraintTimerFireCount_ = 10;
			kinamaticConstraintUpdateTimer_.Reset();
		}


		bool doUpdate = false;
		if(forceUpdate)
			doUpdate = true;

		if (kinamaticConstraintUpdateTimer_.GetMSec(false) >= kinamaticConstraintUpdateTimerTimeout_)
		{
			if (kinamaticConstraintTimerFireCount_ > 0) {
				doUpdate = true;
				kinamaticConstraintUpdateTimer_.Reset();
				kinamaticConstraintTimerFireCount_--;
			}
			else
				kinamaticConstraintTimerFireCount_ = 0;
		}

		if (doUpdate) {

			kinamaticConstriant_->SetOwnWorldPosition(gatherPiecePoint_->GetNode()->GetWorldPosition());
			kinamaticConstriant_->SetOwnWorldRotation(gatherPiecePoint_->GetNode()->GetWorldRotation());


		}
	}

	WeakPtr<Node> gatherNode_;
	MoveMode moveMode_ = MoveMode_Camera;
	WeakPtr<PieceSolidificationGroup> gatheredPieceGroup_;
	bool gatheredPieceGroupFromExisting = false;
	WeakPtr<Piece> gatheredPiece_;
	WeakPtr<PiecePoint> gatherPiecePoint_;
	ea::vector<PiecePoint*> allGatherPiecePoints_;
	ea::vector<Piece*> allGatherPieces_;
	
	Quaternion gatherRotation_;

	bool goodToDrop_ = false;
	bool hasAttachement_ = false;

	SharedPtr<NewtonKinematicsControllerConstraint> kinamaticConstriant_;
	Timer kinamaticConstraintUpdateTimer_;
	int kinamaticConstraintUpdateTimerTimeout_ = 6000;
	int kinamaticConstraintTimerFireCount_ = false;

	bool useGrid_ = false;

	WeakPtr<Piece> otherPiece_;
	WeakPtr<PiecePoint> otherPiecePoint_;

	SharedPtr<PieceManager> pieceManager_;

	
};

