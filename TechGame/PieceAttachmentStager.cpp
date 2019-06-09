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
	URHO3D_LOGINFO("AddPotentialAttachement");


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

bool PieceAttachmentStager::checkDistances()
{
	for (AttachmentPair* pair : potentialAttachments_)
	{
		Vector3 posA = pair->pointA->GetNode()->GetWorldPosition();
		Vector3 posB = pair->pointB->GetNode()->GetWorldPosition();

		float thresh = scene_->GetComponent<PieceManager>()->GetAttachPointThreshold();

		if ((posA - posB).Length() > thresh) {
			URHO3D_LOGINFO(ea::to_string((posA - posB).Length()));
			return false;
		}
	}

	return true;
}

bool PieceAttachmentStager::checkEndPointRules()
{

	bool allPass = true;
	for (AttachmentPair* pair : potentialAttachments_) {
		allPass &= endPointRulePass(pair);
	}

	return allPass;
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

bool PieceAttachmentStager::checkRowBasicCompatability()
{
	bool allPass = true;
	for (AttachmentPair* pair : potentialAttachments_)
	{
		allPass &= PiecePointRow::RowsAttachCompatable(pair->rowA, pair->rowB);
	}
	return allPass;
}

bool PieceAttachmentStager::endPointRulePass(AttachmentPair* attachPair)
{

	PiecePoint* pointA = attachPair->pointA;
	PiecePoint* pointB = attachPair->pointB;

	bool pass = true;

	for (int i = 0; i < 2; i++) {

		if (pointA->isEndCap_) {
			if (pointB->row_) {
				if (!pointB->row_->IsEndPoint(pointB)) {
					pass = false;
					URHO3D_LOGINFO("end cap mid row.");
				}
				else
				{
					if (pointA->GetDirectionWorld().DotProduct(pointB->GetDirectionWorld()) < 0.0f) {
						pass = false;
						URHO3D_LOGINFO("end cap direction does not agree");
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
	}



	return pass;

}
bool PieceAttachmentStager::AttachAll()
{
	if (!IsValid())
		return false;


	ea::vector<Piece*> allPieces;
	for (AttachmentPair* pair : finalAttachments_)
	{
		if(!allPieces.contains(pair->pieceA))
			allPieces.push_back(pair->pieceA);

		if(!allPieces.contains(pair->pieceB))
			allPieces.push_back(pair->pieceB);
	}

	//un solidifying pieces involved in attachment
	ea::vector<PieceSolidificationGroup*> allGroups;
	for (Piece* piece : allPieces) {
		piece->GetPieceGroups(allGroups);

	}
	for (PieceSolidificationGroup* group : allGroups)
	{
		URHO3D_LOGINFO("pushing non solid..");
		group->PushSolidState(false);
		//URHO3D_LOGINFO(ea::to_string(group->GetEffectivelySolidified()));
	}
	
	allPieces.front()->GetScene()->GetComponent<PieceManager>()->RebuildSolidifies();


	allPieces.front()->GetScene()->GetComponent<NewtonPhysicsWorld>()->ForceBuild();



	bool allAttachSuccess = true;
	for (AttachmentPair* pair : finalAttachments_)
	{
		if (!pair->rowA->AttachedToRow(pair->rowB) && !pair->rowB->AttachedToRow(pair->rowA)) {

			//get closest existing groups from both sides.
			PieceSolidificationGroup* groupA = pair->pieceA->GetNearestPieceGroup();
			PieceSolidificationGroup* groupB = pair->pieceB->GetNearestPieceGroup();

			

			allAttachSuccess &= PiecePointRow::AttachRows(pair->rowA, pair->rowB, pair->pointA, pair->pointB);

			if (allAttachSuccess)
			{
				if (!PiecePointRow::RowsHaveDegreeOfFreedom(pair->rowA, pair->rowB))
				{
					URHO3D_LOGINFO("rows have no degree of freedom - merging solid groups..");
					
					if (groupB)
						scene_->GetComponent<PieceManager>()->MovePieceToSolidGroup(pair->pieceA, groupB);
					else if (groupA)
						scene_->GetComponent<PieceManager>()->MovePieceToSolidGroup(pair->pieceB, groupA);
				}
				else
				{
					URHO3D_LOGINFO("rows have degree of freedom - solid group therefore not joined.");
				}
			}
		}
	}

	
	for (PieceSolidificationGroup* group : allGroups) {
			group->PopSolidState();
	}

	scene_->GetComponent<PieceManager>()->RebuildSolidifies();



	return allAttachSuccess;
}
