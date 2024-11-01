#include "Urho3D/Scene/Node.h"
#include "Urho3D/Scene/Scene.h"

#include "PieceAttachmentStager.h"
#include "Piece.h"
#include "PiecePoint.h"
#include "PiecePointRow.h"
#include "PieceManager.h"

#include "NewtonPhysicsWorld.h"

bool PieceAttachmentStager::AddPotentialAttachement(PiecePoint* pointA, PiecePoint* pointB)
{
	//URHO3D_LOGINFO("AddPotentialAttachement");


	if (pointA == nullptr || pointB == nullptr)
		return false;

	if (potentialAttachmentMapA_.contains(pointA) || potentialAttachmentMapB_.contains(pointB))
	{
		//already a potential attachment.
		return false;
	}
	else
	{
		AttachmentPair* pair = new AttachmentPair();
		pair->pointA = pointA;
		pair->pointB = pointB;
		pair->rowA = pointA->row_;
		pair->rowB = pointB->row_;
		pair->pieceA = pointA->GetPiece();
		pair->pieceB = pointB->GetPiece();

		potentialAttachments_.push_back(pair);

		potentialAttachmentMapA_.insert_or_assign(pointA, pair);
		potentialAttachmentMapB_.insert_or_assign(pointB, pair);

		needsAnalyzed_ = true;

		return true;
	}
}

bool PieceAttachmentStager::RemovePotentialAttachment(PiecePoint* pointA, PiecePoint* pointB)
{
	if (potentialAttachmentMapA_.contains(pointA) || potentialAttachmentMapB_.contains(pointB))
	{
		AttachmentPair* pair = potentialAttachmentMapA_[pointA];
		delete pair;

		potentialAttachmentMapA_.erase(pointA);
		potentialAttachmentMapB_.erase(pointB);

		potentialAttachments_.erase_at(potentialAttachments_.index_of(pair));

		needsAnalyzed_ = true;

		return true;
	}
	return false;
}

void PieceAttachmentStager::checkPointDistances()
{
	for (AttachmentPair* pair : potentialAttachments_)
	{
		Vector3 posA = pair->pointA->GetNode()->GetWorldPosition();
		Vector3 posB = pair->pointB->GetNode()->GetWorldPosition();

		float thresh = scene_->GetComponent<PieceManager>()->GetAttachPointThreshold();

		if ((posA - posB).Length() > thresh) {
			pair->goodAttachment_ = false;
			//URHO3D_LOGINFO("checkPointDistances fail");
		}
		pair->distDiff_ = (posA - posB).Length();

		//URHO3D_LOGINFO("Dist Diff: " + ea::to_string(pair->distDiff_));
	}
}

void PieceAttachmentStager::checkPointDirections()
{
	for (AttachmentPair* pair : potentialAttachments_)
	{

		pair->angleDiff_ = pair->pointA->GetDirectionWorld().Angle(pair->pointB->GetDirectionWorld());
		
		float nearestMultiple = RoundToNearestMultiple(pair->angleDiff_, 90.0f);

		float deltaAbs = Abs(nearestMultiple - pair->angleDiff_);

		//URHO3D_LOGINFO("Angle Diff: " + ea::to_string(deltaAbs));
		//URHO3D_LOGINFO("nearestMultiple: " + ea::to_string(nearestMultiple));

		if (deltaAbs > 0.1f || (Abs(nearestMultiple) == 90.0f))
		{
			pair->goodAttachment_ = false;
			//URHO3D_LOGINFO("checkPointDirections fail");
		}
	}
}

void PieceAttachmentStager::checkAllEndPointRules()
{
	for (AttachmentPair* pair : potentialAttachments_) {
		checkEndPointRules(pair);
	}
}

bool PieceAttachmentStager::collectRows()
{
	rowsA.clear();
	rowsB.clear();

	for (AttachmentPair* pair : potentialAttachments_)
	{
		PiecePointRow* rowA = pair->pointA->GetPiece()->GetNode()->GetComponent<PiecePointRow>();
		if (rowA)
		{
			if (!rowsA.contains(rowA))
				rowsA.push_back(rowA);
		}		
		
		PiecePointRow* rowB = pair->pointB->GetPiece()->GetNode()->GetComponent<PiecePointRow>();
		if (rowB)
		{
			if (!rowsB.contains(rowB))
				rowsB.push_back(rowB);
		}
	}

	return true;
}

void PieceAttachmentStager::checkRowBasicCompatability()
{
	for (AttachmentPair* pair : potentialAttachments_)
	{
		if (!PiecePointRow::RowsAttachCompatable(pair->rowA, pair->rowB))
		{
			pair->goodAttachment_ = false;
			//URHO3D_LOGINFO("checkRowBasicCompatability fail");
		}
	}
}

void PieceAttachmentStager::checkEndPointRules(AttachmentPair* attachPair)
{

	PiecePoint* pointA = attachPair->pointA;
	PiecePoint* pointB = attachPair->pointB;

	bool overallPass = true;

	for (int i = 0; i < 2; i++) {

		bool pass = true;

		if (pointA->isEndCap_) {
			if (pointB->row_) {
				if (!pointB->row_->IsEndPoint(pointB)) {
					pass = false;
					URHO3D_LOGINFO("end cap mid row.");
				}
				else
				{
					if (pointB->row_->Count() > 1) {
						if (pointA->GetDirectionWorld().DotProduct(pointB->GetDirectionWorld()) < 0.0f) {
							pass = false;
							URHO3D_LOGINFO("end cap direction does not agree");
						}
					}

				}
			}
		}


		//if pointA has no row..
		if (!pointA->row_) {

			//if pointA is an end cap (ie its a small cap piece)
			if (pointA->isEndCap_)
			{
				if (pointB->row_)
				{
					if (!pointB->row_->IsEndPoint(pointB))
						pass = false;
					else
					{
						if (!(pointA->GetDirectionWorld().DotProduct(pointB->GetDirectionWorld()) > 0.0f)) {
							pass = false;
							URHO3D_LOGINFO("direction disagreement");
						}
					}
				}
				else
				{
					if (pointB->isEndCap_) {
						if (!(pointA->GetDirectionWorld().DotProduct(pointB->GetDirectionWorld()) > 0.0f)) {
							pass = false;
							URHO3D_LOGINFO("direction disagreement");
						}
					}
				}
			}
		}



		//swap and try from other point of view.
		PiecePoint* tmp;
		tmp = pointB;
		pointB = pointA;
		pointA = tmp;

		overallPass &= pass;
	}

	if (!overallPass)
	{
		attachPair->goodAttachment_ = false;
		//URHO3D_LOGINFO("checkEndPointRules fail");
	}

}

bool PieceAttachmentStager::AttachAll()
{
	if (!IsValid())
		return false;

	if(goodAttachments_.size() == 0)
		return false;


	ea::vector<Piece*> allPieces;
	for (AttachmentPair* pair : goodAttachments_)
	{
		if(!allPieces.contains(pair->pieceA))
			allPieces.push_back(pair->pieceA);

		if(!allPieces.contains(pair->pieceB))
			allPieces.push_back(pair->pieceB);
	}

	//un solidifying pieces involved in attachment
	ea::vector<PieceSolidificationGroup*> allGroups;
	for (Piece* pc : allPieces) {
		if (!allGroups.contains(pc->GetPieceGroup()) && (pc->GetPieceGroup() != nullptr))
			allGroups.push_back(pc->GetPieceGroup());
	}

	for (PieceSolidificationGroup* gp : allGroups) {
		scene_->GetComponent<PieceManager>()->RemoveSolidGroup(gp);
	}
	
	allPieces.front()->GetScene()->GetComponent<PieceManager>()->RebuildSolidifies();


	allPieces.front()->GetScene()->GetComponent<NewtonPhysicsWorld>()->ForceBuild();



	bool allAttachSuccess = true;
	for (AttachmentPair* pair : goodAttachments_)
	{
		if (!pair->rowA->AttachedToRow(pair->rowB) && !pair->rowB->AttachedToRow(pair->rowA)) {

			//get closest existing groups from both sides.
			//PieceSolidificationGroup* groupA = pair->pieceA->GetNearestPieceGroup();
			//PieceSolidificationGroup* groupB = pair->pieceB->GetNearestPieceGroup();

			
			allAttachSuccess &= PiecePointRow::AttachRows(pair->rowA, pair->rowB, pair->pointA, pair->pointB);

			
		}
	}


	scene_->GetComponent<PieceManager>()->CleanAll();
	scene_->GetComponent<PieceManager>()->RebuildSolidifies();



	return allAttachSuccess;
}

float PieceAttachmentStager::GetCurrentAttachMetric()
{
	if (badAttachments_.size())
		return 0.0f;

	if (goodAttachments_.size() == 0)
		return 0.0f;

	float totalMetric = 0.0f;
	for (AttachmentPair* pair : goodAttachments_) {

		float angleMetric = pair->angleDiff_ / 180.0f;
		float distMetric = pair->angleDiff_ / scene_->GetComponent<PieceManager>()->GetAttachPointThreshold();

		float pairMetric = (angleMetric + distMetric)*0.5f;
		totalMetric += pairMetric;
	}

	totalMetric /= float(goodAttachments_.size());
	return totalMetric;
}
