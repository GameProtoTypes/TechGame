#include "ManipulationTool.h"
#include "PieceManager.h"
#include "PieceSolidificationGroup.h"
#include <vector>
#include <algorithm>
#include "PieceAttachmentStager.h"
#include "MathExtras.h"
#include "PieceGear.h"
#include "VisualDebugger.h"


ManipulationTool::ManipulationTool(Context* context) : HandTool(context)
{
	SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(ManipulationTool, HandleUpdate));
}

void ManipulationTool::RegisterObject(Context* context)
{
	context->RegisterFactory<ManipulationTool>();
}

bool ManipulationTool::BeginDrag()
{
	if (IsGathering())
		return false;


	Vector3 worldHitPos;
	Piece* aimPiece = node_->GetScene()->GetComponent<PieceManager>()->GetClosestAimPiece(worldHitPos, GetEffectiveLookNode());

	if (!aimPiece) {
		URHO3D_LOGINFO("No aim piece");
		return false;
	}

	dragPiece_ = aimPiece;

	dragPoint_ = aimPiece->GetNode()->CreateChild("DragPoint");
	dragPoint_->SetWorldPosition(worldHitPos);

	if (dragPiece_->GetEffectiveRigidBody()->GetMassScale() <= 0.0f) {
		dragPiece_->GetEffectiveRigidBody()->SetMassScale(1.0);
		node_->GetScene()->GetComponent<NewtonPhysicsWorld>()->ForceBuild();
	}
	

	URHO3D_LOGINFO("beginning drag.");

	isDragging_ = true;

	ea::vector<Piece*> pieces;
	aimPiece->GetAttachedPieces(pieces, true);
	pieces.push_back(aimPiece);
	dragMassTotal_ = 0.0f;

	for (Piece* pc : pieces) {
		dragMassTotal_ += pc->GetRigidBody()->GetEffectiveMass();
	}


	{
		ea::vector<Piece*> allContraptionPieces;
		dragPiece_->GetAttachedPieces(allContraptionPieces, true);
		allContraptionPieces.push_back(dragPiece_);
		for (Piece* pc : allContraptionPieces)
		{
			pieceDynDetachSaves_.push_back(pc->GetEnableDynamicDetachment());
			pc->SetEnableDynamicDetachment(false);
		}
	}

	if (dragUseKinematicJoint_)
	{

		kinamaticConstriant_ = dragPiece_->GetEffectiveRigidBody()->GetNode()->CreateComponent<NewtonKinematicsControllerConstraint>();
		kinamaticConstriant_->SetLimitRotationalVelocity(false);

		float acceleration = Clamp<float>(dragMassTotal_*10000.0f, 100.0f, 1000000.0f);

		kinamaticConstriant_->SetLinearFrictionalAcceleration(acceleration);
		kinamaticConstriant_->SetAngularFrictionalAcceleration(acceleration);
		
	}

	//set gatherNode position to dragpoint position so reference distance is maintained.
	gatherNode_->SetWorldPosition(dragPoint_->GetWorldPosition());


	return true;
}

void ManipulationTool::EndDrag(bool freeze)
{
	URHO3D_LOGINFO("ending drag.");
	//kinamaticConstriant_->Remove();

	dragPiece_->GetEffectiveRigidBody()->ResetForces();
	dragPoint_->Remove();

	if (freeze) {
		node_->GetScene()->GetComponent<NewtonPhysicsWorld>()->WaitForUpdateFinished();
		dragPiece_->GetEffectiveRigidBody()->SetMassScale(0);
		
	}
	if (dragUseKinematicJoint_)
	{
		kinamaticConstriant_->Remove();
	}


	{
		ea::vector<Piece*> allContrationPieces;
		dragPiece_->GetAttachedPieces(allContrationPieces, true);
		allContrationPieces.push_back(dragPiece_);
		for (int i = 0; i < allContrationPieces.size(); i++)
		{
			Piece* pc = allContrationPieces[i];
			pc->SetEnableDynamicDetachment(pieceDynDetachSaves_[i]);
		}
	}



	isDragging_ = false;
}

bool ManipulationTool::Gather(bool grabOne)
{
	if (IsDragging())
		return false;

	URHO3D_LOGINFO("Gathering:");

	//Get the PiecePoint we are aiming at, else return.
	PiecePoint* piecePoint = nullptr;
	

	piecePoint = pieceManager_->GetClosestAimPiecePoint(GetEffectiveLookNode());



	if (!piecePoint)
		return false;

	//resolve piece and store both at currently gathered.
	Piece* piece = piecePoint->GetPiece();


	gatheredPiece_ = piece;
	gatherPiecePoint_ = piecePoint;
	gatherPiecePoint_->SetShowBasisIndicator(true);


	if (grabOne)
	{
		allGatherPieces_.clear();
		allGatherPieces_.push_back(gatheredPiece_);
		allGatherPiecePoints_.clear();
		gatheredPiece_->GetPoints(allGatherPiecePoints_);

		PieceSolidificationGroup* existingGroup = gatheredPiece_->GetPieceGroup();
		ea::vector<Piece*> piecesInGroup;
		if (existingGroup) {
			existingGroup->GetPieces(piecesInGroup);
			piecesInGroup.erase_at(piecesInGroup.index_of(gatheredPiece_));
			pieceManager_->RemoveSolidGroup(existingGroup);
		}

		//detach from its contraption.
		gatheredPiece_->DetachAll();
		
		
		for (Piece* pc : piecesInGroup) {
			pieceManager_->FormSolidGroup(pc);
		}
	}
	else
	{
		//form list of all connected pieces to the gathered piece and store in allGatherPieces_
		formGatherContraption(grabOne);
	}


	//apply ghosting effect to contraption
	for (Piece* pc : allGatherPieces_) {
		pc->SetGhostingEffectEnabled(true);
		
		if (pc->GetNode()->HasComponent<PieceGear>()) {
			pc->GetNode()->GetComponent<PieceGear>()->SetEnabled(false);

		}

	}



	//re parent contraption to single body
	{
		//see if there is a common group already existing for all gatheredPieces
		PieceSolidificationGroup* commonGroup = pieceManager_->GetCommonSolidGroup(allGatherPieces_);

		if (commonGroup) {

			//assign gatheredPieceGroup to the existing common grouping and flag.
			gatheredPieceGroup_ = commonGroup;
			gatheredPieceGroupFromExisting = true;
			URHO3D_LOGINFO("gatheredPieceGroup_ assigned to existing common group");

		}
		else
		{
			//create a new group and move all pieces to the newGroup.
			PieceSolidificationGroup* newGroup = pieceManager_->CreateGroupNode(GetScene(), gatheredPiece_->GetNode()->GetWorldPosition())->GetComponent<PieceSolidificationGroup>();


			pieceManager_->MovePiecesToSolidGroup(allGatherPieces_, newGroup);
			gatheredPieceGroup_ = newGroup;
			gatheredPieceGroupFromExisting = false;
			URHO3D_LOGINFO("gatheredPieceGroup_ group created.");
		}
	}


	//set the rigid body of the group to have no collide and attach kinematics controller.
	NewtonRigidBody* rigBody = gatheredPieceGroup_->GetRigidBody();
	rigBody->SetNoCollideOverride(true);
	rigBody->SetMassScale(1.0f);

	kinamaticConstriant_ = rigBody->GetNode()->CreateComponent<NewtonKinematicsControllerConstraint>();
	kinamaticConstriant_->SetLimitRotationalVelocity(false);
	kinamaticConstriant_->SetLinearFrictionalAcceleration(10000.0f);
	kinamaticConstriant_->SetAngularFrictionalAcceleration(10000.0f);

	kinamaticConstraintUpdateTimer_.Reset();

	//update the kinematic constraint's position on the body.
	updateKinematicsControllerPos(true);
	
	//make a piece attachment stager.
	attachStager_ = context_->CreateObject<PieceAttachmentStager>();
	attachStager_->SetScene(GetScene());


	return true;
}


void ManipulationTool::UnGather(bool freeze)
{
	bool goodToDrop = true;
	bool hasAttachement = false;
	if (attachStager_->IsValid()) {
		//attach
		hasAttachement = attachStager_->AttachAll();
	}
	else
		goodToDrop = false;
	




	if (goodToDrop) {

		//bool attachingToFrozenObject = false;
		//for (PieceAttachmentStager::AttachmentPair* attachment : attachStager_->GetFinalAttachments())
		//{
		//	if (attachment->pieceB->GetEffectiveRigidBody()->GetMassScale() <= 0.0f)
		//		attachingToFrozenObject = true;
		//}



		//check collisions
		drop(freeze, hasAttachement);
	}
}

void ManipulationTool::InstantDuplicatePiece()
{
	if (IsDragging() || IsGathering())
		return;

	Vector3 worldHitPos;
	Piece* aimPiece = node_->GetScene()->GetComponent<PieceManager>()->GetClosestAimPiece(worldHitPos, GetEffectiveLookNode());

	if (!aimPiece)
		return;

	ea::string pieceName = aimPiece->GetNode()->GetVar("PieceName").ToString();

	Node* pieceNode = node_->GetScene()->GetComponent<PieceManager>()->CreatePiece(pieceName, false);

	pieceNode->SetWorldPosition(aimPiece->GetNode()->GetWorldPosition() + Vector3(0, 1.0f, 0));
	pieceNode->GetComponent<Piece>()->SetPrimaryColor(aimPiece->GetPrimaryColor());

}

void ManipulationTool::InstantRemovePiece()
{
	if (IsDragging() || IsGathering())
		return;

	Vector3 worldHitPos;
	Piece* aimPiece = node_->GetScene()->GetComponent<PieceManager>()->GetClosestAimPiece(worldHitPos, GetEffectiveLookNode());

	if (!aimPiece)
		return;

	
	Node* pieceNode = aimPiece->GetNode();

	aimPiece->DetachAll();

	pieceNode->Remove();

}

void ManipulationTool::drop(bool freeze, bool hadAttachement)
{

	GetScene()->GetComponent<PieceManager>()->RemoveSolidGroup(gatheredPieceGroup_);
		


	gatheredPieceGroup_ = nullptr;

	for (Piece* gatheredPiece : allGatherPieces_)
	{
		gatheredPiece->GetEffectiveRigidBody()->SetNoCollideOverride(false);
		gatheredPiece->SetGhostingEffectEnabled(false);
		gatheredPiece->GetRigidBody()->SetMassScale(1.0f*float(!freeze));


		if (gatheredPiece->GetNode()->HasComponent<PieceGear>()) {
			gatheredPiece->GetNode()->GetComponent<PieceGear>()->SetEnabled(true);
		}
	}

	if (!kinamaticConstriant_.Expired()) {
		kinamaticConstriant_->Remove();
		kinamaticConstriant_ = nullptr;
	}

	//form new groupings
	GetScene()->GetComponent<PieceManager>()->FormSolidGroupsOnContraption(gatheredPiece_);


	if (hadAttachement) {

		PieceSolidificationGroup* group = gatheredPiece_->GetPieceGroup();
		
		if ((freeze) && group)
		{
			group->GetRigidBody()->SetMassScale(0);
		}
		
	}

	gatheredPiece_ = nullptr;
	gatherPiecePoint_->SetShowBasisIndicator(false);
	gatherPiecePoint_ = nullptr;

	GetScene()->GetComponent<PieceManager>()->CleanAll();
	GetScene()->GetComponent<PieceManager>()->RebuildSolidifies();


	//restore move mode to camera so that the next picked up piece will get picked up in camera mode.
	SetMoveMode(MoveMode_Camera);
	
}

bool ManipulationTool::IsGathering()
{
	if (gatheredPiece_)
		return true;
	else
		return false;
}

void ManipulationTool::AdvanceGatherPoint(bool forward /*= true*/)
{
	if(gatherPiecePoint_)
		gatherPiecePoint_->SetShowBasisIndicator(false);

	

	for (int i = 0; i < allGatherPiecePoints_.size(); i++) {

		if (allGatherPiecePoints_[i] == gatherPiecePoint_) {

			if (forward) {
				if (i < allGatherPiecePoints_.size() - 1)
					gatherPiecePoint_ = allGatherPiecePoints_[i + 1];
				else
					gatherPiecePoint_ = allGatherPiecePoints_[0];
			}
			else
			{
				if (i > 0)
					gatherPiecePoint_ = allGatherPiecePoints_[i - 1];
				else
					gatherPiecePoint_ = allGatherPiecePoints_[allGatherPiecePoints_.size() - 1];
			}

			gatherPiecePoint_->SetShowBasisIndicator(true);
			gatheredPiece_ = gatherPiecePoint_->GetPiece();

			break;
		}
	}

	updateKinematicsControllerPos(true);
}

void ManipulationTool::SetUseGrid(bool enable)
{
	useGrid_ = enable;
}

bool ManipulationTool::GetUseGrid() const
{
	return useGrid_;
}

void ManipulationTool::ToggleUseGrid()
{
	useGrid_ = !useGrid_;
}




void ManipulationTool::SetMoveMode(MoveMode mode)
{
		moveMode_ = mode;

		//make sure gatherNode is created
		CreateGatherNode();

		if (mode == MoveMode_Camera)
		{
			//set gather node to child and in front of tool
			gatherNode_->SetParent(GetEffectiveLookNode());
			gatherNode_->SetTransform(Matrix3x4::IDENTITY);
			if (mode == MoveMode_Camera) {
				gatherNode_->Translate(gatherNodeRefOffset_);
			}
		}
		if (mode == MoveMode_Global)
		{
			//set gather node to child of scene. keep existing world transform.
			gatherNode_->SetParent(GetScene());
			gatherNode_->SetWorldRotation(SnapOrientationEuler(gatherNode_->GetWorldRotation(), 45.0f));
		}
}




ManipulationTool::MoveMode ManipulationTool::GetMoveMode()
{
	return moveMode_;
}

void ManipulationTool::AimPointForce()
{
	//Get the PiecePoint we are aiming at, else return.
	PiecePoint* piecePoint = pieceManager_->GetClosestAimPiecePoint(GetEffectiveLookNode());

	if (!piecePoint) {
		return;
	}

	if (allGatherPiecePoints_.contains(piecePoint)) {

		if (gatherPiecePoint_)
		{
			gatherPiecePoint_->SetShowBasisIndicator(false);
		}


		gatherPiecePoint_ = piecePoint;
		gatheredPiece_ = gatherPiecePoint_->GetPiece();
		updateKinematicsControllerPos(true);
	}
	else
	{

		gatherNode_->SetWorldTransform(piecePoint->GetNode()->GetWorldPosition(), Quaternion::IDENTITY, 1.0f);

	}
}

void ManipulationTool::RotateNextNearest()
{
	if (otherPiece_ && otherPiecePoint_)
	{
		//get pieces from contraption.
		ea::vector<Piece*> attachedPieces;
		otherPiece_->GetAttachedPieces(attachedPieces, true);
		attachedPieces.push_back(otherPiece_);

		float smallestAngle = M_LARGE_VALUE;
		Piece* smallestAnglePiece = nullptr;
		for (Piece* piece : attachedPieces) {


			Quaternion pieceRotation = piece->GetNode()->GetWorldRotation();


			Quaternion deltaRotation = gatherRotation_.Inverse() * pieceRotation;

	
		}

	}
}







void ManipulationTool::UpdateGatherIndicators()
{

	
}

void ManipulationTool::DrawDebugGeometry(DebugRenderer* debug, bool depthTest)
{
	if (IsDragging())
	{

		debug->AddLine(gatherNode_->GetWorldPosition(), dragPoint_->GetWorldPosition(), Color::YELLOW, depthTest);

	}
}


void ManipulationTool::HandleUpdate(StringHash eventType, VariantMap& eventData)
{
	if (moveMode_ == MoveMode_Global) {
		//move gather node
		Input* input = GetSubsystem<Input>();
		Vector3 translation;
		float moveSpeed = 0.01f;
		if (input->GetKeyDown(KEY_SEMICOLON))
		{
			translation += Vector3(moveSpeed, 0.0f, 0.0f);
		}
		if (input->GetKeyDown(KEY_K))
		{
			translation += Vector3(-moveSpeed, 0.0f, 0.0f);
		}
		if (input->GetKeyDown(KEY_O))
		{
			translation += Vector3(0.0f, 0.0f, moveSpeed);
		}
		if (input->GetKeyDown(KEY_PERIOD))
		{
			translation += Vector3(0.0f, 0.0f, -moveSpeed);
		}
		if (input->GetKeyDown(KEY_P))
		{
			translation += Vector3(0.0f, moveSpeed, 0.0f);
		}
		if (input->GetKeyDown(KEY_SLASH))
		{
			translation += Vector3(0.0f, -moveSpeed, 0.0f);
		}

		//rotate translation by current camera angle snapped.
		translation = SnapOrientationEuler(node_->GetWorldRotation(), 45.0f) * translation;


		if (input->GetKeyDown(KEY_SHIFT)) {
			translation *= 2.0f;
		}
		else if (input->GetKeyDown(KEY_CTRL)) {
			translation *= 0.5f;
		}


		gatherNode_->Translate(translation, TS_WORLD);
	}
	else if (moveMode_ == MoveMode_Camera)
	{

		if (IsGathering()) {
			//cast the gather node forward until it hits something.

			Octree* octree = GetScene()->GetComponent<Octree>();
			RayOctreeQuery query(Ray(GetEffectiveLookNode()->GetWorldPosition(), GetEffectiveLookNode()->GetWorldDirection()));

			//GetSubsystem<VisualDebugger>()->AddLine(GetEffectiveLookNode()->GetWorldPosition(), GetEffectiveLookNode()->GetWorldPosition() + )

			octree->Raycast(query);
			Vector3 worldPos;
			bool foundPos = false;
			for (int i = 0; i < query.result_.size(); i++)
			{

				if (query.result_[i].distance_ <= (gatherNodeMaxCastDist_) && query.result_[i].node_->GetName() == "visualNode")
				{
					//its a piece..
					Piece* piece = query.result_[i].node_->GetParent()->GetComponent<Piece>();

					if (piece && !allGatherPieces_.contains(piece))
					{
						worldPos = query.result_[i].position_;
						foundPos = true;
						break;
					}

				}

			}

			if (foundPos) {
				gatherNode_->SetWorldPosition(worldPos);
			}
			else
			{
				gatherNode_->SetPosition(gatherNodeRefOffset_);
			}
		}

	}








	if (IsGathering())
	{

		float gridSeperation = GetScene()->GetComponent<PieceManager>()->RowPointDistance()/2.0f;
		Vector3 constraintPosition;
		Quaternion constraintOrientation;
		
		if (useGrid_) {
			constraintPosition = Vector3(RoundToNearestMultiple(gatherNode_->GetWorldPosition().x_, gridSeperation),
				RoundToNearestMultiple(gatherNode_->GetWorldPosition().y_, gridSeperation),
				RoundToNearestMultiple(gatherNode_->GetWorldPosition().z_, gridSeperation));
			constraintOrientation = SnapOrientationEuler(gatherNode_->GetWorldRotation(), 45.0f);
		}
		else
		{
			constraintPosition = gatherNode_->GetWorldPosition();
			constraintOrientation = gatherNode_->GetWorldRotation();
		}

		updateKinematicsControllerPos(false);
		
		if (!kinamaticConstriant_.Expired()) {
			kinamaticConstriant_->SetOtherWorldPosition(constraintPosition);
			kinamaticConstriant_->SetOtherWorldRotation(constraintOrientation);
		}


		//reset all recent colored indicators
		for (WeakPtr<PiecePoint> point : recentPointList_) {
			if(!point.Expired())
				point->SetShowColorIndicator(false, Color::BLUE);
		}
		recentPointList_.clear();


		ea::vector<Piece*> blackList;
		blackList.push_back(gatheredPiece_);
		
	
		
		PiecePoint* otherPoint = pieceManager_->GetClosestGlobalPiecePoint(gatherNode_->GetWorldTransform().Translation(), blackList, 0.1f, 5);

		if (otherPoint && !allGatherPieces_.contains(otherPoint->GetPiece()))
		{

			otherPiece_ = otherPoint->GetPiece();


			float dist = (otherPoint->GetNode()->GetWorldPosition() - gatherNode_->GetWorldPosition()).Length();

			if (otherPoint && (dist < 0.1f) && !allGatherPiecePoints_.contains(otherPoint)) {

				otherPiecePoint_ = otherPoint;
				
				Vector3 worldPos = otherPoint->GetNode()->GetWorldPosition();
				Quaternion worldRot = otherPoint->GetNode()->GetWorldRotation() * gatherNode_->GetRotation();
				Matrix3x4 finalTransform(worldPos, worldRot, 1.0f);
				
				kinamaticConstriant_->SetOtherWorldPosition(finalTransform.Translation());
				kinamaticConstriant_->SetOtherWorldRotation(finalTransform.Rotation());

				

				if (otherPiece_ && otherPiecePoint_) {

					PieceManager* pieceManager = GetScene()->GetComponent<PieceManager>();

					ea::vector<PiecePoint*> comparisonPoints;
					pieceManager->GetPointsAroundPoints(allGatherPiecePoints_, comparisonPoints, 0.2f);

					//URHO3D_LOGINFO("comparisonPoints Size: " + ea::to_string(comparisonPoints.size()));

					//check that all other points are within attach tolerance.
					bool attachmentPotential = false;

					ea::vector<PiecePoint*> goodPoints;
					ea::vector<PiecePoint*> closestPoints;//corresponds the allGatherPiecePoints
					closestPoints.resize(allGatherPiecePoints_.size());


					for (int i = 0; i < allGatherPiecePoints_.size(); i++)
					{
						//URHO3D_LOGINFO("looking at allGatherPiecePoints[" + ea::to_string(i) + "]");

						PiecePoint* point = allGatherPiecePoints_[i];

						//find closest comparison point to point.
						PiecePoint* closest = nullptr;
						float closestDist = M_LARGE_VALUE;

						for (int j = 0; j < comparisonPoints.size(); j++) {

							//URHO3D_LOGINFO("looking at comparisonPoints[" + ea::to_string(j) + "]");

							PiecePoint* cp = comparisonPoints[j];

							if (cp->GetPiece() == gatheredPiece_)
								continue;

							float dist = (cp->GetNode()->GetWorldPosition() - point->GetNode()->GetWorldPosition()).Length();

							if (dist < closestDist)
							{
								closestDist = dist;
								closest = cp;
							}
						}

						if (closest && closestDist <= pieceManager->GetAttachPointThreshold()) {
							closestPoints[i] = closest;
							attachmentPotential = true;
							//URHO3D_LOGINFO("closest updated");
						}
					}


					if (attachmentPotential)
					{
						
						attachStager_->Reset();
						for (int i = 0; i < allGatherPiecePoints_.size(); i++) {

							if (closestPoints[i]) {
								attachStager_->AddPotentialAttachement(allGatherPiecePoints_[i], closestPoints[i]);
							}
						}



						attachStager_->Analyze();



						//update colors on good and bad attachments.
						ea::vector<PieceAttachmentStager::AttachmentPair*>& attachments = attachStager_->GetBadAttachments();
						for (auto* pair : attachments) {
							pair->pointB->SetShowColorIndicator(true, Color::RED);
							recentPointList_.push_back(pair->pointB);
						}
						
						attachments = attachStager_->GetGoodAttachments();
						for (auto* pair : attachments) {
							pair->pointB->SetShowColorIndicator(true, Color::GREEN);
							recentPointList_.push_back(pair->pointB);
						}
					}
				}


			}
			else
			{
				otherPiecePoint_ = nullptr;
			}
		}
		else
		{
			otherPiece_ = nullptr;
			otherPiecePoint_ = nullptr;
		}



	}
	else if(IsDragging())
	{
		
	UpdateDragging();


	}






}

void ManipulationTool::UpdateDragging()
{
	if (dragUseKinematicJoint_) {
		kinamaticConstriant_->SetOtherWorldPosition(gatherNode_->GetWorldPosition());
		kinamaticConstriant_->SetOtherWorldRotation(gatherNode_->GetWorldRotation());
	}
	else
	{
		//URHO3D_LOGINFO(gatherNode_->GetWorldPosition().ToString());
		Vector3 displacement = (gatherNode_->GetWorldPosition() - dragPoint_->GetWorldPosition());

		float dragPieceMass = dragPiece_->GetEffectiveRigidBody()->GetEffectiveMass();

		Vector3 worldVel = dragPiece_->GetEffectiveRigidBody()->GetLinearVelocity(TS_WORLD);
		Vector3 worldAngVel = dragPiece_->GetEffectiveRigidBody()->GetAngularVelocity(TS_WORLD);

		dragIntegralAccum_ += displacement;

		Vector3 finalWorldForce = (Urho3D::Ln<float>(displacement.Length() + 1) * 1.0f * displacement.Normalized()) + worldVel * -0.1f + dragIntegralAccum_ * 0.01f;

		Vector3 finalWorldTorque = -worldAngVel * 0.07f*dragPieceMass;



		dragPiece_->GetEffectiveRigidBody()->ResetForces();


		Vector3 netForceOnAllJoints;
		for (NewtonConstraint* c : dragPiece_->GetEffectiveRigidBody()->GetConnectedContraints())
		{
			netForceOnAllJoints += c->GetOwnForce();
		}

		Vector3 calculatedForce = finalWorldForce * dragMassTotal_ * 100.0f;

		if (calculatedForce.Length() > 10.0f)
		{
			calculatedForce = calculatedForce.Normalized()*10.0f;
		}

		dragPiece_->GetEffectiveRigidBody()->AddWorldForce(calculatedForce, dragPoint_->GetWorldPosition());
		dragPiece_->GetEffectiveRigidBody()->AddWorldTorque(finalWorldTorque);

		//limit rotational velocity on drag piece

		Vector3 worldRotVel = dragPiece_->GetEffectiveRigidBody()->GetAngularVelocity(TS_WORLD);
		Vector3 worldLinearVel = dragPiece_->GetEffectiveRigidBody()->GetLinearVelocity(TS_WORLD);

		//dragPiece_->GetEffectiveRigidBody()->AddWorldTorque(-worldRotVel.Normalized() * (worldRotVel.LengthSquared() * 0.1f * dragPieceMass));
	}
}

void ManipulationTool::formGatherContraption(bool onlyOne)
{
	allGatherPieces_.clear();
	allGatherPiecePoints_.clear();


	gatheredPiece_->GetNode()->GetDerivedComponents<PiecePoint>(allGatherPiecePoints_, !onlyOne);
	allGatherPieces_.push_back( gatheredPiece_ );

	if (!onlyOne) {
		ea::vector<Piece*> attachedPieces;
		gatheredPiece_->GetAttachedPieces(attachedPieces, true);

		for (Piece* piece : attachedPieces)
		{
			allGatherPieces_.push_back(piece);
			piece->GetNode()->GetDerivedComponents<PiecePoint>(allGatherPiecePoints_, true, false);
		}
	}
}

void ManipulationTool::updateKinematicsControllerPos(bool forceUpdate)
{
	if (!gatherPiecePoint_)
		return;

	if (forceUpdate)
	{
		kinamaticConstraintTimerFireCount_ = 10;
		kinamaticConstraintUpdateTimer_.Reset();
	}


	bool doUpdate = false;
	if (forceUpdate)
		doUpdate = true;

	if (kinamaticConstraintUpdateTimer_.GetMSec(false) >= (unsigned)kinamaticConstraintUpdateTimerTimeout_)
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

		if (!kinamaticConstriant_.Expired()) {

			kinamaticConstriant_->SetOwnWorldPosition(gatherPiecePoint_->GetNode()->GetWorldPosition());
			kinamaticConstriant_->SetOwnWorldRotation(gatherPiecePoint_->GetNode()->GetWorldRotation());
		}
		else
		{
		}

	}
}

void ManipulationTool::DelayedStart()
{

	CreateGatherNode();

	SetMoveMode(moveMode_);

}

void ManipulationTool::CreateGatherNode()
{
	if (gatherNode_)
		return;

	pieceManager_ = GetScene()->GetComponent<PieceManager>();

	gatherNode_ = GetScene()->CreateChild("gatherNode");
	gatherNode_->SetTemporary(true);

	Node* gatherNodeVis = gatherNode_->CreateChild();
	gatherNodeVis->SetTemporary(true);
	gatherNodeVis->SetScale(0.1f);

	StaticModel* stMdl = gatherNodeVis->CreateComponent<StaticModel>();
	stMdl->SetModel(GetSubsystem<ResourceCache>()->GetResource<Model>("Models/LinePrimitives/Basis.mdl"));
}

void ManipulationTool::OnNodeSet(Node* node)
{
	if (node) {

	}
	else
	{
		//UnGather(false);
	}
}























