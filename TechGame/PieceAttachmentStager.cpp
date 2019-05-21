#include "PieceAttachmentStager.h"
#include "Piece.h"
#include "PiecePoint.h"
#include "PiecePointRow.h"
#include "PieceManager.h"

#include "NewtonPhysicsWorld.h"

bool PieceAttachmentStager::AddPotentialAttachement(PiecePoint* pointA, PiecePoint* pointB)
{
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

		potentialAttachments_.erase(potentialAttachments_.index_of(pair));

		needsAnalyzed_ = true;

		return true;
	}
	return false;
}

bool PieceAttachmentStager::AttachAll()
{
	if (!IsValid())
		return false;


	ea::vector<Piece*> pieces;

	//un solididify pieces involved in attachement
	for (AttachmentPair* pair : finalAttachments_)
	{
		if(!pieces.contains(pair->pieceA))
			pieces.push_back(pair->pieceA);

		if(!pieces.contains(pair->pieceB))
			pieces.push_back(pair->pieceB);
	}
	for (Piece* piece : pieces) {
		PieceGroup* group = piece->GetNearestPieceGroup();
		if (group)
		{
			group->PushSolidState(false);
		}
	}

	
	//pieces.Front()->GetScene()->GetComponent<PieceManager>()->StripGroups(pieces);


	pieces.front()->GetScene()->GetComponent<NewtonPhysicsWorld>()->ForceBuild();



	bool allAttachSuccess = true;
	for (AttachmentPair* pair : finalAttachments_)
	{
		if(!pair->rowA->AttachedToRow(pair->rowB) && !pair->rowB->AttachedToRow(pair->rowA))
			allAttachSuccess &= PiecePointRow::AttachRows(pair->rowA, pair->rowB, pair->pointA, pair->pointB);
	}


	
	for (Piece* piece : pieces) {
		PieceGroup* group = piece->GetNearestPieceGroup();
		if (group)
		{
			group->PopSolidState();
		}
	}




	return allAttachSuccess;
}

bool PieceAttachmentStager::checkDistances()
{
	for (AttachmentPair* pair : potentialAttachments_)
	{
		Vector3 posA = pair->pointA->GetNode()->GetWorldPosition();

		Vector3 posB = pair->pointB->GetNode()->GetWorldPosition();


		if ((posA - posB).Length() > 0.002f) {
			URHO3D_LOGINFO(ea::to_string((posA - posB).Length()));
			return false;

		}
	}

	return true;
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

bool PieceAttachmentStager::checkEndPointRules()
{

	bool allPass = true;
	for (AttachmentPair* pair : potentialAttachments_) {


		allPass &= endPointRulePass(pair);


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

			//if pointA is an endcap (ie its a smallcap piece)
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