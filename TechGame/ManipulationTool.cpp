#include "ManipulationTool.h"
#include "PieceManager.h"
#include "PieceGroup.h"
#include <vector>
#include <algorithm>
#include "PieceAttachmentStager.h"


ManipulationTool::ManipulationTool(Context* context) : Tool(context)
{
	SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(ManipulationTool, HandleUpdate));
}

void ManipulationTool::RegisterObject(Context* context)
{
	context->RegisterFactory<ManipulationTool>();
}

void ManipulationTool::Gather()
{
	PiecePoint* piecePoint = GetClosestAimPiecePoint();
	if (!piecePoint)
		return;


	Piece* piece = piecePoint->GetPiece();

	gatheredPiece_ = piece;
	gatherPiecePoint_ = piecePoint;

	formGatherContraption();
	

	NewtonRigidBody* rigBody;

	//re parent contraption to single body

	PieceGroup* group = (pieceManager_->GetCommonGroup(allGatherPieces_));
	if (!group) {
		gatheredPieceGroup_ = pieceManager_->AddPiecesToNewGroup(allGatherPieces_);
	}
	else
		gatheredPieceGroup_ = group;

	gatheredPieceGroup_->SetSolidified(true);

	gatheredContraptionNode_ = gatheredPieceGroup_->GetNode();

	rigBody = gatheredContraptionNode_->GetComponent<NewtonRigidBody>();
	rigBody->SetIsKinematic(false);
	rigBody->SetNoCollideOverride(true);
	rigBody->SetMassScale(1.0f);






	kinamaticConstriant_ = rigBody->GetNode()->CreateComponent<NewtonKinematicsControllerConstraint>();




	updateKinematicsControllerPos();
	

	for (Piece* gatheredPiece : allGatherPieces_)
	{
		gatheredPiece->GetNode()->GetComponent<NewtonRigidBody>()->SetNoCollideOverride(true);
	}

}


bool ManipulationTool::IsGathering()
{
	if (gatheredPiece_)
		return true;
	else
		return false;
}

void ManipulationTool::UnGather(bool freeze)
{

	bool goodToDrop = true;
	if (otherPiece_ && otherPiecePoint_) {


		PieceManager* pieceManager = GetScene()->GetComponent<PieceManager>();

		ea::vector<PiecePoint*> comparisonPoints;
		pieceManager->GetPointsAroundPoints(allGatherPiecePoints_, comparisonPoints, 0.2f);



		//check that all other points are within attach tolerance.
		bool attachmentPotential = false;
		ea::vector<PiecePoint*> goodPoints;
		ea::vector<PiecePoint*> closestPoints;
		closestPoints.resize(comparisonPoints.size());
		for(int i = 0; i < allGatherPiecePoints_.size(); i++) 
		{
			PiecePoint* point = allGatherPiecePoints_[i];


			//find closest comparison point to point.
			PiecePoint* closest = nullptr;
			float closestDist = M_LARGE_VALUE;
			for (int i = 0; i < comparisonPoints.size(); i++) {
				
				PiecePoint* cp = comparisonPoints[i];

				if (cp->GetPiece() == gatheredPiece_)
					continue;

				float dist = (cp->GetNode()->GetWorldPosition() - point->GetNode()->GetWorldPosition()).Length();

				if (dist < closestDist)
				{
					closestDist = dist;
					closest = cp;
				}
			}

			if (closest && closestDist < 0.002f) {
				closestPoints[i] = closest;
				attachmentPotential = true;
			}
		}

		


		if (attachmentPotential)
		{
			ea::shared_ptr<PieceAttachmentStager> attachStager = context_->CreateObject<PieceAttachmentStager>();
			for (int i = 0; i < allGatherPiecePoints_.size(); i++) {

				if (closestPoints[i]) {
					attachStager->AddPotentialAttachement(allGatherPiecePoints_[i], closestPoints[i]);
				}
			}

			attachStager->AnalyzeAndFix();

			if (attachStager->IsValid())
				attachStager->AttachAll();
			else
				goodToDrop = false;
		}
	}



	if (goodToDrop) {

		//check collisions

		drop(freeze);
	}
}

void ManipulationTool::AdvanceGatherPoint(bool forward /*= true*/)
{
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


			gatheredPiece_ = gatherPiecePoint_->GetPiece();

			break;
		}
	}

	updateKinematicsControllerPos();
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



PiecePoint* ManipulationTool::GetClosestAimPiecePoint()
{

	PieceManager* pieceManager = GetScene()->GetComponent<PieceManager>();

	Vector3 worldPos;
	Piece* piece = GetClosestAimPiece(worldPos);
	if (!piece)
		return nullptr;


	return pieceManager->GetClosestPiecePoint(worldPos, piece);
}



Piece* ManipulationTool::GetClosestAimPiece(Vector3& worldPos)
{

	Camera* camera = node_->GetComponent<Camera>();

	
	Octree* octree = GetScene()->GetComponent<Octree>();
	RayOctreeQuery querry(Ray(node_->GetWorldPosition(), node_->GetDirection()));
	octree->Raycast(querry);
	
	if (querry.result_.size() > 1)
	{
		if (querry.result_[1].node_->GetName() == "visualNode")
		{
			Piece* piece = querry.result_[1].node_->GetParent()->GetComponent<Piece>();
			if (piece) {
				worldPos = querry.result_[1].position_;
				return piece;
			}
		}
	}


	return nullptr;
}


void ManipulationTool::HandleUpdate(StringHash eventType, VariantMap& eventData)
{
	
	if (IsGathering())
	{

		kinamaticConstriant_->SetOtherWorldPosition(gatherNode_->GetWorldPosition());
		kinamaticConstriant_->SetOtherWorldRotation(gatherNode_->GetWorldRotation());



		ea::vector<Piece*> blackList;
		blackList.push_back(gatheredPiece_);
		Piece* otherPiece = pieceManager_->GetClosestGlobalPiece(gatherNode_->GetWorldTransform().Translation(), blackList, 0.1f);
		if (otherPiece && otherPiece != gatheredPiece_)
		{

			otherPiece_ = otherPiece;


			PiecePoint* otherPoint = pieceManager_->GetClosestPiecePoint(gatherNode_->GetWorldTransform().Translation(), otherPiece);


			if (!allGatherPiecePoints_.contains(otherPoint) &&
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

void ManipulationTool::OnNodeSet(Node* node)
{
	if (node) {
		pieceManager_ = GetScene()->GetComponent<PieceManager>();


		gatherNode_ = node_->CreateChild();
		gatherNode_->Translate(Vector3(0, 0, 2));

	}
	else
	{
		UnGather(false);
	}
}

void ManipulationTool::drop(bool freeze)
{

	if (!freeze) {
		
		GetScene()->GetComponent<PieceManager>()->RemoveGroup(gatheredPieceGroup_);

	}
	else
	{
		gatheredContraptionNode_->GetComponent<NewtonRigidBody>()->SetMassScale(0);

	}
	
	gatheredPieceGroup_ = nullptr;





	for (Piece* gatheredPiece : allGatherPieces_)
	{
		gatheredPiece->GetNode()->GetComponent<NewtonRigidBody>()->SetNoCollideOverride(false);
	}
	gatheredContraptionNode_ = nullptr;

	kinamaticConstriant_->Remove();

	gatheredPiece_ = nullptr;
	gatherPiecePoint_ = nullptr;
}

void ManipulationTool::formGatherContraption()
{
	allGatherPieces_.clear();
	allGatherPiecePoints_.clear();

	gatheredPiece_->GetNode()->GetDerivedComponents<PiecePoint>(allGatherPiecePoints_, true);
	allGatherPieces_.push_back( gatheredPiece_ );

	ea::vector<Piece*> attachedPieces;
	gatheredPiece_->GetAttachedPieces(attachedPieces, true);

	for (Piece* piece : attachedPieces)
	{
		allGatherPieces_.push_back( piece );
		piece->GetNode()->GetDerivedComponents<PiecePoint>(allGatherPiecePoints_, true, false);
	}
}
