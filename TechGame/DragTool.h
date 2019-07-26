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








//class DragTool : public HandTool {
//	URHO3D_OBJECT(DragTool, HandTool);
//public:
//
//
//
//	DragTool(Context* context) : HandTool(context)
//	{
//		SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(ManipulationTool, HandleUpdate));
//	}
//
//	static void RegisterObject(Context* context)
//	{
//		context->RemoveFactory<DragTool>();
//	}
//
//
//	virtual void DrawDebugGeometry(DebugRenderer* debug, bool depthTest) override
//	{
//		
//	}
//
//	void BeginDrag()
//	{
//		Vector3 worldHitPos;
//		Piece* aimPiece = node_->GetScene()->GetComponent<PieceManager>()->GetClosestAimPiece(worldHitPos, GetEffectiveLookNode());
//
//
//		kinamaticConstriant_ = aimPiece->GetEffectiveRigidBody()->GetNode()->CreateComponent<NewtonKinematicsControllerConstraint>();
//		
//		
//		
//
//	}
//
//	void EndDrag()
//	{
//
//		kinamaticConstriant_->Remove();
//	}
//
//
//
//
//protected:
//
//
//	void HandleUpdate(StringHash eventType, VariantMap& eventData)
//	{
//
//
//
//	}
//	virtual void OnNodeSet(Node* node) override
//	{
//		if (node)
//		{
//
//		}
//		else
//		{
//
//		}
//	}
//
//	WeakPtr<NewtonKinematicsControllerConstraint> kinamaticConstriant_;
//
//};