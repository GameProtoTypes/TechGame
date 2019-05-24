#include "PiecePointRow.h"
#include "PiecePoint.h"
#include "Piece.h"
#include "PieceManager.h"

#include "NewtonPhysicsWorld.h"
#include "NewtonConstraint.h"
#include "NewtonSliderConstraint.h"
#include "NewtonFullyFixedConstraint.h"
#include "NewtonHingeConstraint.h"

bool PiecePointRow::RowsAttachCompatable(PiecePointRow* rowA, PiecePointRow* rowB)
{
	if (rowA->rowType_ == RowType_Hole && rowB->rowType_ == RowType_Hole)
		return false;

	if (rowA->rowType_ == RowType_RodHard && rowB->rowType_ == RowType_RodHard)
		return false;

	if (rowA->rowType_ == RowType_RodHard && rowB->rowType_ == RowType_RodHard)
		return false;



	//check that the row directions agree within tolerance of 45 degrees.
	if (!(rowA->GetRowDirectionWorld().CrossProduct(rowB->GetRowDirectionWorld()).Length() <= 0.25f))
		return false;

	return true;
}

void PiecePointRow::DrawDebugGeometry(DebugRenderer* debug, bool depthTest)
{
	BoundingBox bounds;

	for (int i = 0; i < points_.size(); i++)
	{
		PiecePoint* point = points_[i];

		Color c = Color(float(points_.size() - 1 - i) / (points_.size() - 1), float(i) / (points_.size() - 1), 0.0);

		debug->AddSphere(Sphere(point->GetNode()->GetWorldPosition(), 0.125*0.5f), c, depthTest);
	}

	PiecePoint* pointA;
	PiecePoint* pointB;
	GetEndPoints(pointA, pointB);
	debug->AddLine(pointA->GetNode()->GetWorldPosition(), pointA->GetNode()->GetWorldPosition() + GetRowDirectionWorld(), debugColor_, depthTest);
}

bool PiecePointRow::CheckValid()
{


	for (int i = 0; i < points_.size(); i++) {
		//no endCap flags allowed mid row.
		if (i != 0 && i != points_.size() - 1)
		{
			if (points_[i]->isEndCap_)
				return false;
		}
	}

	return true;
}

void PiecePointRow::FormPointDirectionsOnEndPoints()
{
	if (points_.size() <= 1)
		return;

	//first find center of row in real coords
	Vector3 center = GetLocalCenter();

	//set directions pointing towards center.
	for (PiecePoint* point : points_)
	{
		point->direction_ = (center - point->GetNode()->GetPosition()).Normalized();
	}
}

void PiecePointRow::PushBack(PiecePoint* point)
{
	if (!points_.contains(point)) {
		points_.push_back(point);
		point->row_ = WeakPtr<PiecePointRow>(this);
	}
}

void PiecePointRow::GetEndPoints(PiecePoint*& pointA, PiecePoint*& pointB)
{
	pointA = points_.front();
	pointB = points_.back();
}

Urho3D::Vector3 PiecePointRow::GetRowDirectionLocal()
{
	if (points_.size() <= 1)
	{
		return localDirection_;
	}
		

	PiecePoint* pointA;
	PiecePoint* pointB;
	GetEndPoints(pointA, pointB);

	return (pointB->GetNode()->GetPosition() - pointA->GetNode()->GetPosition()).Normalized();

}

Urho3D::Vector3 PiecePointRow::GetRowDirectionWorld()
{
	return node_->GetWorldRotation().RotationMatrix() * GetRowDirectionLocal();
}

bool PiecePointRow::IsEndPoint(PiecePoint* point)
{
	if (point == points_.front() || point == points_.back())
		return true;
	else
		return false;
}

PiecePoint* PiecePointRow::GetPointNextToEndPoint(PiecePoint* endPoint)
{
	if (!endPoint->isEndCap_)
		return nullptr;

	if (endPoint == points_.front())
	{
		return points_[1];
	}
	else if (endPoint == points_.back())
	{
		return points_[points_.size() - 2];
	}

	return nullptr;
}

Urho3D::Vector3 PiecePointRow::GetLocalCenter()
{

	//first find center of row in real coords
	Vector3 center;
	for (PiecePoint* point : points_)
	{
		center += point->GetNode()->GetPosition();
	}
	center *= 1.0f / float(points_.size());

	return center;
}

Urho3D::Vector3 PiecePointRow::GetWorldCenter()
{
	return node_->LocalToWorld(GetLocalCenter());
}

bool PiecePointRow::AttachedToRow(PiecePointRow* row)
{
	for (int i = 0; i < rowAttachements_.size(); i++) {
		if (rowAttachements_[i].rowA_ == row)
			return true;
	}
	return false;
}

bool PiecePointRow::DetachFrom(PiecePointRow* otherRow)
{

	bool detached = false;
	for (int i = 0; i < rowAttachements_.size(); i++) {
		
		if (rowAttachements_[i].rowA_ == otherRow)
		{
			if (!rowAttachements_[i].constraint_.Expired())
			{
				rowAttachements_[i].constraint_->Remove();
			}

			rowAttachements_.erase(i);
			detached = true;
		}
	}
	for (int i = 0; i < otherRow->rowAttachements_.size(); i++) {

		if (otherRow->rowAttachements_[i].rowA_ == this)
		{
			otherRow->rowAttachements_.erase(i);
			detached = true;
		}
	}



	
	return detached;
}

bool PiecePointRow::DetachAll()
{
	ea::vector<RowAttachement> rowAttachementsCopy = rowAttachements_;
	for (int i = 0; i < rowAttachementsCopy.size(); i++) {
		DetachFrom(rowAttachementsCopy[i].rowA_);
	}

	rowAttachements_.clear();

	return true;
}

bool PiecePointRow::AttachRows(PiecePointRow* rowA, PiecePointRow* rowB, PiecePoint* pointA, PiecePoint* pointB, bool attachAsFullRow)
{
	
	if (!PiecePointRow::RowsAttachCompatable(rowA, rowB)) {
		URHO3D_LOGWARNING("PiecePointRow::AttachRows: Rows Not Compatable");
		return false;
	}

	if (rowA->AttachedToRow(rowB) || rowB->AttachedToRow(rowA)){
		URHO3D_LOGWARNING("PiecePointRow::AttachRows: rows already attached");
		return false;
	}

	if (rowA->GetPiece()->IsEffectivelySolidified() || rowB->GetPiece()->IsEffectivelySolidified()) {
		URHO3D_LOGWARNING("PiecePointRow::AttachRows: Cannot AttachRows while pieces are solidified.");
		return false;
	}


	PiecePointRow* theHoleRow = nullptr;
	PiecePointRow* theRodRow = nullptr;
	PiecePoint* theRodPoint = nullptr;
	PiecePoint* theHolePoint = nullptr;

	if (rowA->GetGeneralRowType() == PiecePointRow::RowTypeGeneral_Hole && rowB->GetGeneralRowType() == PiecePointRow::RowTypeGeneral_Rod) {
		theHoleRow = rowA;
		theRodRow = rowB;
		
		theHolePoint = pointA;
		theRodPoint = pointB;
	}
	else if (rowA->GetGeneralRowType() == PiecePointRow::RowTypeGeneral_Rod && rowB->GetGeneralRowType() == PiecePointRow::RowTypeGeneral_Hole)
	{
		theHoleRow = rowB;
		theRodRow = rowA;

		theHolePoint = pointB;
		theRodPoint = pointA;
	}
	else
		return false;



	Piece* theHolePiece = theHoleRow->GetPiece();
	Piece* theRodPiece = theRodRow->GetPiece();


	NewtonRigidBody* holeBody = theHolePiece->GetComponent<NewtonRigidBody>();
	NewtonRigidBody* rodBody = theRodPiece->GetComponent<NewtonRigidBody>();



		//wait for update finished because we need to do some manual rigidbody moving and hacking.
		holeBody->GetScene()->GetComponent<NewtonPhysicsWorld>()->WaitForUpdateFinished();

		Matrix3x4 origHoldBodyTransform = holeBody->GetWorldTransform();
		Matrix3x4 origRodBodyTransform = rodBody->GetWorldTransform();




		PieceManager* pieceManager = holeBody->GetScene()->GetComponent<PieceManager>();

		//create the constraint
		NewtonConstraint* constraint = nullptr;
		if (theRodRow->GetRowType() == PiecePointRow::RowType_RodHard || theHoleRow->GetRowType() == PiecePointRow::RowType_HoleTight)
		{

			holeBody->SetWorldPosition(-theHolePoint->GetNode()->GetPosition());
			//holeBody->SetWorldRotation(Quaternion::IDENTITY);

			rodBody->SetWorldPosition(-theRodPoint->GetNode()->GetPosition());


			Quaternion diff = (holeBody->GetWorldRotation().Inverse() * rodBody->GetWorldRotation()).Normalized();
			diff = diff.Inverse();
			Quaternion diffSnap90;

			if (!diff.IsNaN() && Abs(diff.Axis().x_) <= 1.0f)
			{
				//URHO3D_LOGINFO("non nan");
				diffSnap90 = Quaternion(RoundToNearestMultiple(diff.Angle(), 45.0f), diff.Axis());
			}
			else {
				//URHO3D_LOGINFO("nan");
				diffSnap90 = Quaternion::IDENTITY;
			}

			holeBody->SetWorldRotation(Quaternion::IDENTITY);
			rodBody->SetWorldRotation(diffSnap90);



			constraint = holeBody->GetNode()->CreateComponent<NewtonFullyFixedConstraint>();


			constraint->SetOtherBody(rodBody);
			constraint->SetOwnPosition(theHolePoint->GetNode()->GetPosition());
			constraint->SetOwnRotation(Quaternion(90, Vector3(0, 1, 0)));
			constraint->SetOtherPosition(theRodPoint->GetNode()->GetPosition());
			constraint->SetOtherRotation(diffSnap90 * Quaternion(90, Vector3(0, 1, 0)));

		}
		else if (theRodRow->GetRowType() == PiecePointRow::RowType_RodRound)
		{
			//move bodies to world 0,0 - around the mid point of rows.



			

			holeBody->SetWorldPosition(-theHoleRow->GetLocalCenter());
			rodBody->SetWorldPosition(-theRodRow->GetLocalCenter());

			Quaternion diff = (holeBody->GetWorldRotation().Inverse() * rodBody->GetWorldRotation()).Normalized();
			diff = diff.Inverse();
			//URHO3D_LOGINFO("Diff Angle: " + String(diff.Angle()));
			//URHO3D_LOGINFO("Diff Axis: "  + String(diff.Axis()));
			
			
			Quaternion diffSnap90; 

			if (!diff.IsNaN() && Abs(diff.Axis().x_) <= 1.0f)
			{
				//URHO3D_LOGINFO("non nan");
				diffSnap90 = Quaternion(RoundToNearestMultiple(diff.Angle(), 45.0f), diff.Axis());
			}
			else {
			
				//URHO3D_LOGINFO("nan");
				diffSnap90 = Quaternion::IDENTITY;

			}



			//URHO3D_LOGINFO("DiffSnapped90: " + String(diffSnap90.Angle()));

			holeBody->SetWorldRotation(Quaternion::IDENTITY);
			rodBody->SetWorldRotation(diffSnap90);

			if (!attachAsFullRow) {

				constraint = holeBody->GetNode()->CreateComponent<NewtonSliderConstraint>();
				static_cast<NewtonSliderConstraint*>(constraint)->SetTwistLowerLimitEnable(false);
				static_cast<NewtonSliderConstraint*>(constraint)->SetTwistUpperLimitEnable(false);
				static_cast<NewtonSliderConstraint*>(constraint)->SetEnableSliderLimits(true, true);
			}
			else
			{
				constraint = holeBody->GetNode()->CreateComponent<NewtonHingeConstraint>();
				static_cast<NewtonHingeConstraint*>(constraint)->SetEnableLimits(false);
			}

				//compute slide limits
				Vector2 slideLimits;
				
				float totalSlideAmount = (float(theHoleRow->Count() + theRodRow->Count() - 2))*RowPointDistance();
				slideLimits.x_ = -totalSlideAmount * 0.5f;
				slideLimits.y_ = totalSlideAmount * 0.5f;

				PiecePoint* rodEndA;
				PiecePoint* rodEndB;
				PiecePoint* holeEndA;
				PiecePoint* holeEndB;

				theRodRow->GetEndPoints(rodEndA, rodEndB);
				theHoleRow->GetEndPoints(holeEndA, holeEndB);

				bool flipped = theRodRow->GetRowDirectionWorld().DotProduct(theHoleRow->GetRowDirectionWorld()) < 0;
				
				
				if (rodEndA->isEndCap_)
				{
					slideLimits.x_ += (float(theHoleRow->Count() - 1))*RowPointDistance();
				}
				else
				{
					slideLimits.x_ = -100.0f;
				}

				if (rodEndB->isEndCap_)
				{
					slideLimits.y_ -= (float(theHoleRow->Count() - 1))*RowPointDistance();
				}
				else
				{
					slideLimits.y_ = 100.0f;
				}




				if (holeEndB->isEndCap_)
				{
					slideLimits.y_ = (theRodRow->Count() / 2.0f)*RowPointDistance() + 0.5f;
					slideLimits.x_ = (theRodRow->Count() / 2.0f)*RowPointDistance() - 0.01f;
				}
				if (holeEndA->isEndCap_)
				{
					slideLimits.y_ = -(theRodRow->Count() / 2.0f)*RowPointDistance() + 0.01f;
					slideLimits.x_ = -(theRodRow->Count() / 2.0f)*RowPointDistance() - 0.5f;
				}



				//Vector2 trueSlideLimits = static_cast<PiecePointRoundRod*>(point)->GetSlideLimits();

				if (flipped)
				{
					URHO3D_LOGINFO("flipped");
					float tmp = slideLimits.x_;
					slideLimits.x_ = -slideLimits.y_;
					slideLimits.y_ = -tmp;
				}
				

				const float slopDist = 0.01f;
				if (!attachAsFullRow) {
					static_cast<NewtonSliderConstraint*>(constraint)->SetSliderLimits(slideLimits.x_ - slopDist, slideLimits.y_ + slopDist);
					static_cast<NewtonSliderConstraint*>(constraint)->SetSliderFriction(0.01f);
				}


				constraint->SetOtherBody(rodBody);
				constraint->SetOwnPosition(theHoleRow->GetLocalCenter());
				constraint->SetOwnRotation(Quaternion(90, Vector3(0, 1, 0)));
				constraint->SetOtherPosition(theRodRow->GetLocalCenter());
				constraint->SetOtherRotation(diffSnap90 * Quaternion(90, Vector3(0, 1, 0)));


		}




		//force build now to ensure the bond is accurate.
		constraint->BuildNow();



		//restore transforms
		rodBody->SetWorldTransform(origRodBodyTransform);
		rodBody->ApplyTransformToNode();
		holeBody->SetWorldTransform(origHoldBodyTransform);
		holeBody->ApplyTransformToNode();



		RowAttachement attachment;
		attachment.pointA_ = pointA;
		attachment.pointB = pointB;
		attachment.rowA_ = theHoleRow;
		attachment.rowB_ = theRodRow;
		attachment.constraint_ = constraint;

		theRodRow->rowAttachements_.push_back(attachment);
		attachment.rowA_ = theRodRow;
		attachment.rowB_ = theHoleRow;
		attachment.pointA_ = pointB;
		attachment.pointB = pointA;
		theHoleRow->rowAttachements_.push_back(attachment);

		rowA->UpdatePointOccupancies();
		rowB->UpdatePointOccupancies();
		OptimizeFullRow(rowA);
		OptimizeFullRow(rowB);

		return true;

}

bool PiecePointRow::OptimizeFullRow(PiecePointRow* row)
{
	ea::vector<PiecePoint*> points = row->GetPoints();

	bool fullyOccupied = true;
	for (PiecePoint* point : points)
	{
		fullyOccupied &= (bool)point->occupiedPoint_;
	}
	if ((row->GetGeneralRowType() == RowTypeGeneral_Rod))
	{
		if (fullyOccupied && !row->isOccupiedOptimized_)
		{
			URHO3D_LOGINFO("Optimizing Rod..");
			for (RowAttachement attachment : row->rowAttachements_)
			{

				if (attachment.rowB_->GetGeneralRowType() != RowTypeGeneral_Hole)
					continue;

				float curSliderPos = static_cast<NewtonSliderConstraint*>(attachment.constraint_.Get())->GetSliderPosition();

				curSliderPos = RoundToNearestMultiple(curSliderPos, RowPointDistance());

				row->DetachFrom(attachment.rowB_);
				AttachRows(row, attachment.rowB_, attachment.pointB, attachment.pointA_, true);


				bool allPlaner = true;
				for (RowAttachement attachment2 : row->rowAttachements_)
				{
					allPlaner &= attachment2.rowA_->GetIsPiecePlaner();
				}

				//if all pieces are planer - dissable collisions between them.
				if (allPlaner) {
					for (RowAttachement attachment2 : row->rowAttachements_)
					{
						attachment.rowA_->GetPiece()->GetRigidBody()->SetCollisionOverride(attachment2.rowA_->GetPiece()->GetRigidBody(), false);
					}
				}

			}

			row->isOccupiedOptimized_ = true;
		}
		else if (!fullyOccupied && row->isOccupiedOptimized_)
		{
			URHO3D_LOGINFO("Un Optimizing Rod..");
			for (RowAttachement attachment : row->rowAttachements_)
			{
				if (attachment.rowA_->GetGeneralRowType() != RowTypeGeneral_Hole)
					continue;


				for (RowAttachement attachment2 : row->rowAttachements_)
				{
					if (attachment.rowA_->GetGeneralRowType() != RowTypeGeneral_Hole)
						continue;

					row->DetachFrom(attachment.rowB_);
					AttachRows(row, attachment.rowB_, attachment.pointB, attachment.pointA_);


					attachment.rowA_->GetPiece()->GetRigidBody()->RemoveCollisionOverride(attachment2.rowA_->GetPiece()->GetRigidBody());
				}


			}
			row->isOccupiedOptimized_ = false;
		}
	}


	return true;
}

Piece* PiecePointRow::GetPiece()
{
	return node_->GetComponent<Piece>();
}

void PiecePointRow::HandleUpdate(StringHash event, VariantMap& eventData)
{
	UpdatePointOccupancies();
	return;
}

void PiecePointRow::UpdatePointOccupancies()
{
	if (!rowAttachements_.size())
		return;

	if (rowType_ == RowType_RodHard || rowType_ == RowType_HoleTight)
		return;


	//clear occupancies on points
	for (PiecePoint* point : points_)
	{
		point->occupiedPointPrev_ = point->occupiedPoint_;
		point->occupiedPoint_ = nullptr;
	}

	ea::vector<PiecePoint*> otherPoints;

	for (RowAttachement& row : rowAttachements_)
	{
		otherPoints.push_back(row.rowA_->points_);	
	}

	bool aPointIsStillOccupied = false;
	bool allPointsOccupied = true;
	for (PiecePoint* point : points_)
	{
		bool matchFound = false;
		for (PiecePoint* otherPoint : otherPoints)
		{
			float dist = (point->GetNode()->GetWorldPosition() - otherPoint->GetNode()->GetWorldPosition()).Length();
			if (dist < RowPointDistance()) {
				point->occupiedPoint_ = otherPoint;
				aPointIsStillOccupied = true;
				matchFound = true;
			}
		}

		if (!matchFound)
			allPointsOccupied = false;
	}

	if (!aPointIsStillOccupied)
	{
		DetachAll();
	}

	if (allPointsOccupied)
	{
		//rebuild with hinge constraints....
	}
}
