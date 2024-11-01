#pragma once
#include <Urho3D/Urho3DAll.h>
#include "Piece.h"
#include "PieceSolidificationGroup.h"
#include "PiecePoint.h"
#include "PieceManager.h"

#include "NewtonKinematicsJoint.h"
#include "PieceAttachmentStager.h"
#include "Character.h"
#include "HandTool.h"








class ManipulationTool : public HandTool {
	URHO3D_OBJECT(ManipulationTool, HandTool);
public:
	ManipulationTool(Context* context);

	static void RegisterObject(Context* context);

	enum MoveMode {
		MoveMode_Global = 0,
		MoveMode_Camera = 1
	};


	bool BeginDrag();
	void EndDrag(bool freeze);
	bool IsDragging() { return isDragging_; }

	bool Gather(bool grabOne = false);
	bool IsGathering();
	void UnGather(bool freeze);


	void InstantDuplicatePiece();

	void InstantRemovePiece();

	void AdvanceGatherPoint(bool forward = true);

	void RotateGatherNode(Quaternion rotation)
	{
		gatherNode_->Rotate(rotation, TS_PARENT);
	}

	void StartGatherNodeRotation(Quaternion rotation) {
		gatherTargetRotation_ = gatherTargetRotation_ * rotation;
		gatherStartRotation_ = gatherNode_->GetRotation();
		gatherNodeIsRotating_ = true;
	}

	void StopGatherNodeRotation() {
		gatherNodeIsRotating_ = false;
	}

	void ResetGatherNodeRotation();

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


	void WeldPoint() {

		URHO3D_LOGINFO("Attempting Weld..");

		PiecePoint* piecePoint = nullptr;

		piecePoint = pieceManager_->GetClosestAimPiecePoint(GetEffectiveLookNode());

		if (piecePoint) {
			if (!piecePoint->IsWelded())
				piecePoint->Weld();
		}
	}

	void UnWeldPoint() {
		PiecePoint* piecePoint = nullptr;

		piecePoint = pieceManager_->GetClosestAimPiecePoint(GetEffectiveLookNode());

		if (piecePoint) {
			if (piecePoint->IsWelded())
				piecePoint->UnWeld();

		}
	}

	void OilPiece() {
		Vector3 worldPos;
		Piece* piece = pieceManager_->GetClosestAimPiece(worldPos, GetEffectiveLookNode());
		if (piece) {

			piece->SetOiled(true);
			pieceManager_->RemovePieceFromGroup(piece);
			piece->ReAttachAll();


		}

	}
	void UnOilPiece() {
		Vector3 worldPos;
		Piece* piece = pieceManager_->GetClosestAimPiece(worldPos, GetEffectiveLookNode());
		if (piece) {

			piece->SetOiled(false);
			pieceManager_->FormSolidGroup(piece);



		}
	}



	virtual void DrawDebugGeometry(DebugRenderer* debug, bool depthTest) override;




protected:

	void HandleUpdate(StringHash eventType, VariantMap& eventData);

	void UpdateHoverPointIndicators();



	void UpdateMoveGatherNode();

	void UpdateDragging();

	void UpdateGatherNodeRotation();

	void SetGatherRotationViaMode(Quaternion rotation);

	virtual void OnNodeSet(Node* node) override;

	void drop(bool freeze, bool hadAttachement);

	void formGatherContraption(bool oneAssembly);

	void updateKinematicsControllerPos(bool forceUpdate);

	bool isDragging_ = false;
	WeakPtr<Node> dragPoint_;
	WeakPtr<Piece> dragPiece_;
	ea::vector<bool> pieceDynDetachSaves_;
	float dragMassTotal_ = 0.0f;
	Vector3 dragIntegralAccum_;
	bool dragUseKinematicJoint_ = false;

	WeakPtr<Node> gatherNode_;
	MoveMode moveMode_ = MoveMode_Camera;
	WeakPtr<PieceSolidificationGroup> gatheredPieceGroup_;
	bool gatheredPieceGroupFromExisting = false;
	WeakPtr<Piece> gatheredPiece_;
	WeakPtr<PiecePoint> gatherPiecePoint_;
	ea::vector<PiecePoint*> allGatherPiecePoints_;
	ea::vector<Piece*> allGatherPieces_;
	
	Quaternion gatherRotationalVel_;
	Quaternion gatherStartRotation_;
	Quaternion gatherTargetRotation_;
	bool gatherNodeIsRotating_ = false;
	float gatherSlerpParam_ = 0.0f;
	ea::vector<Vector2> gatherNodeRotationMetrics_;
	unsigned lastAttachSignature_ = false;
	unsigned lastNumGoodAttachments_ = 0;
	unsigned lastNumBadAttachmnets_ = 0;

	Vector3 gatherNodeRefOffset_ = Vector3(0,0,2);
	float gatherNodeMaxCastDist_ = 5.0f;

	WeakPtr<NewtonKinematicsControllerConstraint> kinamaticConstriant_;

	Timer kinamaticConstraintUpdateTimer_;
	int kinamaticConstraintUpdateTimerTimeout_ = 60;
	int kinamaticConstraintTimerFireCount_ = false;


	ea::vector<WeakPtr<PiecePoint>> recentPointList_;
	ea::vector<WeakPtr<PiecePoint>> hoverPointList_;


	bool useGrid_ = false;

	WeakPtr<Piece> otherPiece_;
	WeakPtr<PiecePoint> otherPiecePoint_;

	SharedPtr<PieceManager> pieceManager_;
	SharedPtr<PieceAttachmentStager> attachStager_;
	
	virtual void DelayedStart() override;

	void CreateGatherNode();

};

