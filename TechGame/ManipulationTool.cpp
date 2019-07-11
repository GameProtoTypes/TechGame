#include "ManipulationTool.h"
#include "PieceManager.h"
#include "PieceSolidificationGroup.h"
#include <vector>
#include <algorithm>
#include "PieceAttachmentStager.h"
#include "MathExtras.h"


ManipulationTool::ManipulationTool(Context* context) : Tool(context)
{
	SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(ManipulationTool, HandleUpdate));
}

void ManipulationTool::RegisterObject(Context* context)
{
	context->RegisterFactory<ManipulationTool>();
}

bool ManipulationTool::Gather(bool grabOne)
{
	URHO3D_LOGINFO("Gathering:");

	//Get the PiecePoint we are aiming at, else return.
	PiecePoint* piecePoint = pieceManager_->GetClosestAimPiecePoint(node_->GetComponent<Camera>());

	if (!piecePoint)
		return false;

	//resolve piece and store both at currently gathered.
	Piece* piece = piecePoint->GetPiece();


	

	gatheredPiece_ = piece;
	gatherPiecePoint_ = piecePoint;
	gatherPiecePoint_->SetShowIndicator(true);


	if (grabOne)
	{
		allGatherPieces_.clear();
		allGatherPieces_.push_back(gatheredPiece_);
		allGatherPiecePoints_.clear();
		allGatherPiecePoints_.push_back(gatherPiecePoint_);

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
		pc->SetGhostingEffect(true);
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
			PieceSolidificationGroup* newGroup = pieceManager_->CreateGroupNode(GetScene())->GetComponent<PieceSolidificationGroup>();

			pieceManager_->MovePiecesToSolidGroup(allGatherPieces_, newGroup);
			gatheredPieceGroup_ = newGroup;
			gatheredPieceGroupFromExisting = false;
			URHO3D_LOGINFO("gatheredPieceGroup_ group created.");
		}
	}


	//set the rigid body of the group to have no collide and attach kinematics constroller.
	NewtonRigidBody* rigBody = gatheredPieceGroup_->GetRigidBody();
	rigBody->SetNoCollideOverride(true);
	rigBody->SetMassScale(1.0f);

	kinamaticConstriant_ = rigBody->GetNode()->CreateComponent<NewtonKinematicsControllerConstraint>();
	kinamaticConstraintUpdateTimer_.Reset();

	//update the kinematic constraint's position on the body.
	updateKinematicsControllerPos(true);
	

	return true;
}


void ManipulationTool::UnGather(bool freeze)
{


	bool goodToDrop = true;
	bool hadAttachement = false;
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


		for(int i = 0; i < allGatherPiecePoints_.size(); i++) 
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
			SharedPtr<PieceAttachmentStager> attachStager = context_->CreateObject<PieceAttachmentStager>();
			attachStager->SetScene(GetScene());
			for (int i = 0; i < allGatherPiecePoints_.size(); i++) {

				if (closestPoints[i]) {
					attachStager->AddPotentialAttachement(allGatherPiecePoints_[i], closestPoints[i]);
				}
			}

			attachStager->AnalyzeAndFix();

			if (attachStager->IsValid()) {
			
				//attach
				hadAttachement = attachStager->AttachAll();
			}
			else
				goodToDrop = false;
		}
	}



	if (goodToDrop) {

		//check collisions
		drop(freeze, hadAttachement);
	}
}

void ManipulationTool::drop(bool freeze, bool hadAttachement)
{
	if (!gatheredPieceGroup_.Expired() && hadAttachement) {
		GetScene()->GetComponent<PieceManager>()->RemoveSolidGroup(gatheredPieceGroup_);
	}

	GetScene()->GetComponent<PieceManager>()->RemoveSolidGroup(gatheredPieceGroup_);
		

	gatheredPieceGroup_ = nullptr;

	for (Piece* gatheredPiece : allGatherPieces_)
	{
		gatheredPiece->GetEffectiveRigidBody()->SetNoCollideOverride(false);
		gatheredPiece->SetGhostingEffect(false);
		gatheredPiece->GetRigidBody()->SetMassScale(1.0f*float(!freeze));
	}

	if (!kinamaticConstriant_.Expired()) {
		kinamaticConstriant_->Remove();
		kinamaticConstriant_ = nullptr;
	}

	//form new groupings
	GetScene()->GetComponent<PieceManager>()->FormSolidGroupsOnContraption(gatheredPiece_);


	if (hadAttachement) {

		PieceSolidificationGroup* group = gatheredPiece_->GetPieceGroup();
		
		if (freeze && group)
		{
			group->GetRigidBody()->SetMassScale(0);
		}
		
	}

	gatheredPiece_ = nullptr;
	gatherPiecePoint_->SetShowIndicator(false);
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
		gatherPiecePoint_->SetShowIndicator(false);


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

			gatherPiecePoint_->SetShowIndicator(true);
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
		if (mode == MoveMode_Camera)
		{
			//set gather node to child and in front of tool
			gatherNode_->SetParent(node_);
			gatherNode_->SetTransform(Matrix3x4::IDENTITY);
			gatherNode_->Translate(Vector3(0, 0, 2));
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
	PiecePoint* piecePoint = pieceManager_->GetClosestAimPiecePoint(node_->GetComponent<Camera>());

	if (!piecePoint) {
		return;
	}

	if (allGatherPiecePoints_.contains(piecePoint)) {

		if (gatherPiecePoint_)
		{
			gatherPiecePoint_->SetShowIndicator(false);
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
		translation = SnapOrientationEuler(node_->GetWorldRotation(), 90.0f) * translation;


		if (input->GetKeyDown(KEY_SHIFT)) {
			translation *= 2.0f;
		}
		else if (input->GetKeyDown(KEY_CTRL)) {
			translation *= 0.5f;
		}


		gatherNode_->Translate(translation, TS_WORLD);
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
		
		

		kinamaticConstriant_->SetOtherWorldPosition(constraintPosition);
		kinamaticConstriant_->SetOtherWorldRotation(constraintOrientation);



		ea::vector<Piece*> blackList;
		blackList.push_back(gatheredPiece_);
		Piece* otherPiece = pieceManager_->GetClosestGlobalPiece(gatherNode_->GetWorldTransform().Translation(), blackList, 0.1f);
		if (otherPiece && !allGatherPieces_.contains(otherPiece))
		{

			otherPiece_ = otherPiece;


			PiecePoint* otherPoint = pieceManager_->GetClosestPiecePoint(gatherNode_->GetWorldTransform().Translation(), otherPiece);


			if (otherPoint && !allGatherPiecePoints_.contains(otherPoint) &&
				otherPoint->OccupancyCompatible(gatherPiecePoint_) && 
				gatherPiecePoint_->OccupancyCompatible(otherPoint)) {

				otherPiecePoint_ = otherPoint;
				
				Vector3 worldPos = otherPoint->GetNode()->GetWorldPosition();
				Quaternion worldRot = otherPoint->GetNode()->GetWorldRotation() * gatherNode_->GetRotation();
				Matrix3x4 finalTransform(worldPos, worldRot, 1.0f);
				
				kinamaticConstriant_->SetOtherWorldPosition(finalTransform.Translation());
				kinamaticConstriant_->SetOtherWorldRotation(finalTransform.Rotation());

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
	else
	{
		//URHO3D_LOGINFO("4");
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

void ManipulationTool::OnNodeSet(Node* node)
{
	if (node) {
		pieceManager_ = GetScene()->GetComponent<PieceManager>();
		
		gatherNode_ = GetScene()->CreateChild();
		Node* gatherNodeVis = gatherNode_->CreateChild();
		gatherNodeVis->SetScale(0.1f);

		StaticModel* stMdl = gatherNodeVis->CreateComponent<StaticModel>();
		stMdl->SetModel(GetSubsystem<ResourceCache>()->GetResource<Model>("Models/LinePrimitives/Basis.mdl"));
		/*Material* mat = GetSubsystem<ResourceCache>()->GetResource<Material>("Models/DefaultMaterial.xml");
		stMdl->SetMaterial(mat);
		mat->SetTechnique(0, GetSubsystem<ResourceCache>()->GetResource<Technique>("Techniques/Diff.xml"));
		mat->SetShaderParameter("MatDiffColor", Vector4(1.0f, 0.0f, 0.0f, 0.0f));*/
		
		SetMoveMode(moveMode_);
	}
	else
	{
		//UnGather(false);
	}
}
