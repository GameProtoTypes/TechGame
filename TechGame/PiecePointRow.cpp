#include "PiecePointRow.h"
#include "PiecePoint.h"
#include "Piece.h"
#include "PieceManager.h"

#include "NewtonPhysicsWorld.h"
#include "NewtonConstraint.h"
#include "NewtonSliderConstraint.h"
#include "NewtonFullyFixedConstraint.h"
#include "NewtonHingeConstraint.h"

#include "MathExtras.h"



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
	if (!points_.contains(SharedPtr<PiecePoint>(point))) {
		points_.push_back(SharedPtr<PiecePoint>(point));
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

bool PiecePointRow::HasAnEndCap()
{
	for (PiecePoint* pnt : points_)
	{
		if (pnt->isEndCap_)
			return true;
	}
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
		if (rowAttachements_[i].rowOther_ == row)
			return true;
	}
	return false;
}

void PiecePointRow::GetAttachedRows(ea::vector<PiecePointRow*>& rows)
{
	for (int i = 0; i < rowAttachements_.size(); i++) {
		rows.push_back(rowAttachements_[i].rowOther_);
	}
}

bool PiecePointRow::DetachFrom(PiecePointRow* otherRow, bool updateOccupiedPoints)
{

	bool detached = false;
	for (int i = 0; i < rowAttachements_.size(); i++) {
		
		if (rowAttachements_[i].rowOther_ == otherRow)
		{
			if(!rowAttachements_[i].constraint_.Expired())
				rowAttachements_[i].constraint_->Remove();
			
			rowAttachements_.erase_at(i);
			detached = true;
			break;
		}
	}
	for (int i = 0; i < otherRow->rowAttachements_.size(); i++) {

		if (otherRow->rowAttachements_[i].rowOther_ == this)
		{
			otherRow->rowAttachements_.erase_at(i);
			detached = true;
			break;
		}
	}


	URHO3D_LOGINFO("PiecePointRow:: Row Detached");
	if (updateOccupiedPoints) {
		UpdatePointOccupancies();
		otherRow->UpdatePointOccupancies();
		UpdateOptimizeFullRow(this);
		UpdateOptimizeFullRow(otherRow);
	}

	
	return detached;
}

bool PiecePointRow::DetachAll()
{
	ea::vector<RowAttachement> rowAttachementsCopy = rowAttachements_;
	for (int i = 0; i < rowAttachementsCopy.size(); i++) {
		if(!rowAttachementsCopy[i].rowOther_.Expired())
			DetachFrom(rowAttachementsCopy[i].rowOther_, true);
	}

	return true;
}

bool PiecePointRow::AttachRows(PiecePointRow* rowA, PiecePointRow* rowB, PiecePoint* pointA, PiecePoint* pointB, bool attachAsFullRow /*= false*/, bool updateOptimizations /*= true*/)
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

	if (theHolePiece->GetEffectiveRigidBody() != theHolePiece->GetRigidBody() || theRodPiece->GetEffectiveRigidBody() != theRodPiece->GetRigidBody())
	{
		URHO3D_LOGWARNING("PiecePointRow::AttachRows: Cannot AttachRows pieces have abstracted rigid bodies.");
		return false;
	}
	
	NewtonRigidBody* holeBody = theHolePiece->GetRigidBody();
	NewtonRigidBody* rodBody = theRodPiece->GetRigidBody();






	URHO3D_LOGINFO("PiecePointRow::AttachRows holebody enabled: " + ea::to_string(holeBody->IsEnabledEffective()));
	URHO3D_LOGINFO("PiecePointRow::AttachRows rodbody enabled: " + ea::to_string(rodBody->IsEnabledEffective()));


		//wait for update finished because we need to do some manual rigidbody moving and hacking.
		holeBody->GetScene()->GetComponent<NewtonPhysicsWorld>()->WaitForUpdateFinished();

		Matrix3x4 origHoleBodyTransform = holeBody->GetWorldTransform();
		Matrix3x4 origRodBodyTransform = rodBody->GetWorldTransform();


		PieceManager* pieceManager = holeBody->GetScene()->GetComponent<PieceManager>();

		//create the constraint
		NewtonConstraint* constraint = nullptr;
		if (theRodRow->GetRowType() == PiecePointRow::RowType_RodHard || theHoleRow->GetRowType() == PiecePointRow::RowType_HoleTight)
		{

			URHO3D_LOGINFO("PiecePointRow::AttachRows running hard attachment case.");


			holeBody->SetWorldPosition(-theHolePoint->GetNode()->GetPosition() );
			rodBody->SetWorldPosition(-theRodPoint->GetNode()->GetPosition() );


			Quaternion diff = (holeBody->GetWorldRotation().Inverse() * rodBody->GetWorldRotation()).Normalized();
			diff = diff.Inverse();
			diff.Normalize();
			Quaternion diffSnap45 = SnapOrientationAngle(diff, 45.0f);

			holeBody->SetWorldRotation(Quaternion::IDENTITY);
			rodBody->SetWorldRotation(diffSnap45);



			constraint = holeBody->GetNode()->CreateComponent<NewtonFullyFixedConstraint>();


			constraint->SetOtherBody(rodBody);
			constraint->SetOwnPosition(theHolePoint->GetNode()->GetPosition());
			constraint->SetOwnRotation(Quaternion(90, Vector3(0, 1, 0)));
			constraint->SetOtherPosition(theRodPoint->GetNode()->GetPosition());
			constraint->SetOtherRotation(diffSnap45 * Quaternion(90, Vector3(0, 1, 0)));
			

		}
		else if (theRodRow->GetRowType() == PiecePointRow::RowType_RodRound)
		{
			//move bodies to world 0,0 - around the mid point of rows.



			

			holeBody->SetWorldPosition(-theHoleRow->GetLocalCenter());
			rodBody->SetWorldPosition(-theRodRow->GetLocalCenter());

			Quaternion diff = (holeBody->GetWorldRotation().Inverse() * rodBody->GetWorldRotation()).Normalized();
			diff = diff.Inverse();
			diff.Normalize();
			//URHO3D_LOGINFO("Diff Angle: " + String(diff.Angle()));
			//URHO3D_LOGINFO("Diff Axis: "  + String(diff.Axis()));
			
			
			Quaternion diffSnap45 = SnapOrientationAngle(diff, 45.0f);

			//URHO3D_LOGINFO("DiffSnapped90: " + String(diffSnap90.Angle()));

			holeBody->SetWorldRotation(Quaternion::IDENTITY);
			rodBody->SetWorldRotation(diffSnap45);

			const float twistFriction = 0.001f;
			//if (!attachAsFullRow) {

				constraint = holeBody->GetNode()->CreateComponent<NewtonSliderConstraint>();
				static_cast<NewtonSliderConstraint*>(constraint)->SetTwistLowerLimitEnable(false);
				static_cast<NewtonSliderConstraint*>(constraint)->SetTwistUpperLimitEnable(false);
				static_cast<NewtonSliderConstraint*>(constraint)->SetEnableSliderLimits(true, true);
				static_cast<NewtonSliderConstraint*>(constraint)->SetTwistFriction(twistFriction);
				//constraint->SetSolveMode(SOLVE_MODE_ITERATIVE);
			//}
			//else
			//{
			//	constraint = holeBody->GetNode()->CreateComponent<NewtonHingeConstraint>();
			//	static_cast<NewtonHingeConstraint*>(constraint)->SetEnableLimits(false);
			//	static_cast<NewtonHingeConstraint*>(constraint)->SetFriction(twistFriction);
			//}

				//compute slide limits
				Vector2 slideLimits;
				
				float totalSlideAmount = (float(theHoleRow->Count() + theRodRow->Count() - 2))*pieceManager->RowPointDistance();

				//if (attachAsFullRow)
				//	totalSlideAmount = pieceManager->RowPointDistance()*0.5f;

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
					slideLimits.x_ += (float(theHoleRow->Count() - 1))*pieceManager->RowPointDistance();
				}
				else
				{
					slideLimits.x_ = -100.0f;
				}

				if (rodEndB->isEndCap_)
				{
					slideLimits.y_ -= (float(theHoleRow->Count() - 1))*pieceManager->RowPointDistance();
				}
				else
				{
					slideLimits.y_ = 100.0f;
				}




				if (holeEndB->isEndCap_)
				{
					slideLimits.y_ = (theRodRow->Count() / 2.0f)*pieceManager->RowPointDistance() + 0.5f;
					slideLimits.x_ = (theRodRow->Count() / 2.0f)*pieceManager->RowPointDistance() - 0.01f;
				}
				if (holeEndA->isEndCap_)
				{
					slideLimits.y_ = -(theRodRow->Count() / 2.0f)*pieceManager->RowPointDistance() + 0.01f;
					slideLimits.x_ = -(theRodRow->Count() / 2.0f)*pieceManager->RowPointDistance() - 0.5f;
				}



				//Vector2 trueSlideLimits = static_cast<PiecePointRoundRod*>(point)->GetSlideLimits();

				if (flipped)
				{
					URHO3D_LOGINFO("PiecePointRow::AttachRows flipped");
					float tmp = slideLimits.x_;
					slideLimits.x_ = -slideLimits.y_;
					slideLimits.y_ = -tmp;
				}
				

				const float slopDist = 0.005f;
				//if (!attachAsFullRow) {
					static_cast<NewtonSliderConstraint*>(constraint)->SetSliderLimits(slideLimits.x_ - slopDist, slideLimits.y_ + slopDist);
					static_cast<NewtonSliderConstraint*>(constraint)->SetSliderFriction(0.001f);
				//}



				constraint->SetOtherBody(rodBody);
				constraint->SetOwnPosition(theHoleRow->GetLocalCenter());
				constraint->SetOwnRotation(Quaternion(90, Vector3(0, 1, 0)));
				constraint->SetOtherPosition(theRodRow->GetLocalCenter());
				constraint->SetOtherRotation(diffSnap45 * Quaternion(90, Vector3(0, 1, 0)));


		}
		else
		{
			URHO3D_LOGINFO("PiecePointRow::AttachRows No Constraint Created.");
		}



		//force build now to ensure the bond is accurate.
		constraint->BuildNow();




		//restore both bodies to original state before this function call.
		holeBody->SetWorldTransform(origHoleBodyTransform);
		holeBody->ApplyTransformToNode();

		rodBody->SetWorldTransform(origRodBodyTransform);
		rodBody->ApplyTransformToNode();
		
		//explicitly disable collisions.
		rodBody->SetCollisionOverride(holeBody, false);



		RowAttachement attachment;
		attachment.pointOther_ = pointB;
		attachment.point = pointA;
		attachment.rowOther_ = rowB;
		attachment.row_ = rowA;
		attachment.constraint_ = constraint;

		rowA->rowAttachements_.push_back(attachment);

		attachment.pointOther_ = pointA;
		attachment.point = pointB;
		attachment.rowOther_ = rowA;
		attachment.row_ = rowB;
		attachment.constraint_ = constraint;

		rowB->rowAttachements_.push_back(attachment);



		rowA->UpdatePointOccupancies();
		rowB->UpdatePointOccupancies();

		if (updateOptimizations) {
			UpdateOptimizeFullRow(rowA);
			UpdateOptimizeFullRow(rowB);
		}
		URHO3D_LOGINFO("row attached");
		return true;

}

bool PiecePointRow::RowsHaveDegreeOfFreedom(PiecePointRow* rowA, PiecePointRow* rowB)
{
	if (!RowsAttachCompatable(rowA, rowB))
		return false;

	PiecePointRow* rodRow;
	PiecePointRow* holeRow;



	if (rowA->GetGeneralRowType() == RowTypeGeneral_Hole)
	{
		holeRow = rowA;
		rodRow = rowB;
	}
	else
	{
		holeRow = rowB;
		rodRow = rowA;
	}


	if (holeRow->GetRowType() == RowType_HoleTight)
	{
		return false;
	}
	else
	{
		if (rodRow->GetRowType() == RowType_RodHard)
			return false;
		else
			return true;
	}
}

bool PiecePointRow::UpdateOptimizeFullRow(PiecePointRow* row)
{
	return false;//#TODO!

	ea::vector<SharedPtr<PiecePoint>> points = row->GetPoints();
	PieceManager* pieceManager = row->GetScene()->GetComponent<PieceManager>();

	bool fullyOccupied = (row->numOccupiedPoints_ == row->points_.size());



	if ((row->GetGeneralRowType() == RowTypeGeneral_Rod))
	{

		//URHO3D_LOGINFO("is fully occupied: " + ea::to_string(fullyOccupied));

		if (fullyOccupied && !row->isFullRowOptimized_)
		{
			URHO3D_LOGINFO("Optimizing Rod..");
			for (auto attachment : row->rowAttachements_)
			{
				if (!attachment.rowOther_)
					continue;

				row->DetachFrom(attachment.rowOther_, false);
				AttachRows(row, attachment.rowOther_, attachment.point, attachment.pointOther_, true, false);


				bool allPlaner = true;
				for (auto attachment2 : row->rowAttachements_)
				{
					allPlaner &= attachment2.rowOther_->GetIsPiecePlaner();
				}

				//if all pieces are planer - disable collisions between them.
				if (allPlaner) {
					URHO3D_LOGINFO("all pieces planer, disabling collisions.");
					for (auto attachment2 : row->rowAttachements_)
					{
						attachment.row_->GetPiece()->GetRigidBody()->SetCollisionOverride(attachment2.rowOther_->GetPiece()->GetRigidBody(), false);
					}
				}
			}

			row->isFullRowOptimized_ = true;
		}
		else if (!fullyOccupied && row->isFullRowOptimized_)
		{
			URHO3D_LOGINFO("Un Optimizing Rod..");
			for (auto attachment : row->rowAttachements_)
			{


				for (auto attachment2 : row->rowAttachements_)
				{
					attachment.row_->GetPiece()->GetRigidBody()->RemoveCollisionOverride(attachment2.rowOther_->GetPiece()->GetRigidBody());


					row->DetachFrom(attachment.rowOther_, false);


					AttachRows(row, attachment.rowOther_, attachment.point, attachment.pointOther_, false, false);


					
				}


			}
			row->isFullRowOptimized_ = false;
		}
	}


	return true;
}

void PiecePointRow::RegisterObject(Context* context)
{
	context->RegisterFactory<PiecePointRow>();
	URHO3D_COPY_BASE_ATTRIBUTES(Component);

	URHO3D_ATTRIBUTE("localDirection", Vector3, localDirection_, Vector3::ZERO, AM_DEFAULT);
	URHO3D_ATTRIBUTE("isPiecePlaner_", bool, isPiecePlaner_, false, AM_DEFAULT);

	URHO3D_ATTRIBUTE("isFullRowOptimized_", bool, isFullRowOptimized_, false, AM_FILE);



	URHO3D_ENUM_ATTRIBUTE("rowtype", rowType_, RowTypeNames, DefaultRowType, AM_DEFAULT);
	URHO3D_ENUM_ATTRIBUTE("rowtypegeneral", rowTypeGeneral_, RowTypeGeneralNames, DefaultGeneralRowType, AM_DEFAULT);



}

Piece* PiecePointRow::GetPiece()
{
	return node_->GetComponent<Piece>();
}















bool PiecePointRow::SaveXML(XMLElement& dest) const
{
	Component::SaveXML(dest);

	{
		//points data
		XMLElement el = dest.CreateChild("PointsData");

		VariantVector vec;
		for (PiecePoint* point : points_)
			vec.push_back(point->GetID());

		el.SetVariantVector(vec);
	}

	{
		//attachment data
		
		for (auto attachment : rowAttachements_)
		{
			XMLElement el2 = dest.CreateChild("AttachmentData");
			VariantVector vec;
			vec.push_back(attachment.constraint_->GetID());
			vec.push_back(attachment.point->GetID());
			vec.push_back(attachment.pointOther_->GetID());
			vec.push_back(attachment.row_->GetID());
			vec.push_back(attachment.rowOther_->GetID());
			el2.SetVariantVector(vec);
		}
		
	}






	return true;
}


bool PiecePointRow::LoadXML(const XMLElement& source)
{
	Component::LoadXML(source);

	{
		//points data
		XMLElement el = source.GetChild("PointsData");
		VariantVector vec = el.GetVariantVector();
		for (Variant variant : vec)
		{
			unsigned componentId = variant.GetUInt();
			pointIds_.push_back(componentId);
		}
	}

	{

		rowAttachements_.clear();

		//store attachment data node ids for later resolving.
		XMLElement el2 = source.GetChild("AttachmentData");
		while (!el2.IsNull())
		{
			VariantVector vec = el2.GetVariantVector();


			RowAttachement attachment;

			attachment.constraintId_ = vec[0].GetUInt();
			attachment.pointId = vec[1].GetUInt();
			attachment.pointOtherId_ = vec[2].GetUInt();
			attachment.rowId_ = vec[3].GetUInt();
			attachment.rowOtherId_= vec[4].GetUInt();

			rowAttachements_.push_back(attachment);
			el2 = el2.GetNext("AttachmentData");
		}

	}

	return true;
}
























void PiecePointRow::ApplyAttributes()
{

	{
		SceneResolver sceneResolver;
		sceneResolver.Resolve();
		for (unsigned id : pointIds_)
		{
			Component* comp = GetScene()->GetComponent(id);
			sceneResolver.AddComponent(id, comp);
			sceneResolver.Resolve();
			points_.push_back(SharedPtr<PiecePoint>(dynamic_cast<PiecePoint*>(comp)));
		}
	}
}



void PiecePointRow::HandleUpdate(StringHash event, VariantMap& eventData)
{

	UpdatePointOccupancies();
	UpdateOptimizeFullRow(this);
	UpdateDynamicDettachement();
	return;
}

void PiecePointRow::UpdatePointOccupancies()
{

	if (!rowAttachements_.size())
		return;


	//clear occupancies on points
	for (SharedPtr<PiecePoint> point : points_)
	{
		point->occupiedPointPrev_ = point->occupiedPoint_;
		point->occupiedPoint_ = nullptr;
	}

	ea::vector<SharedPtr<PiecePoint>> otherPoints;
	for (RowAttachement& row : rowAttachements_)
	{
		otherPoints.push_back(row.rowOther_->points_);	
	}

	numOccupiedPoints_ = 0;
	for (PiecePoint* point : points_)
	{
		int numDuplicatePoints = 0;

		for (PiecePoint* otherPoint : otherPoints)
		{
			
			float dist = (point->GetNode()->GetWorldPosition() - otherPoint->GetNode()->GetWorldPosition()).Length();
			
			//URHO3D_LOGINFO("dist: " + ea::to_string(dist) + " vs " + ea::to_string(pieceManager->RowPointDistance()*0.5f));

			if (dist < pieceManager_->RowPointDistance()*0.5f) {

				point->occupiedPoint_ = otherPoint;
				numOccupiedPoints_++;
			}

		}
	}
	//URHO3D_LOGINFO("num occupied: " + ea::to_string(numPointsOccupied));
}

void PiecePointRow::UpdateDynamicDettachement()
{

	if (numOccupiedPoints_ <= 0)
	{
		if (occupiedCountDown_ <= 0) {
			occupiedCountDown_ = 0;
			if (GetScene()->GetComponent<PieceManager>()->GetEnableDynamicRodDetachment())
			{
				if (rowAttachements_.size())
				{
					ea::vector<RowAttachement> rowAttachementsCopy = rowAttachements_;
					for (int i = 0; i < rowAttachementsCopy.size(); i++) 
					{
						
						if (rowAttachementsCopy[i].pointOther_->GetPiece()->GetEnableDynamicDetachment() && GetPiece()->GetEnableDynamicDetachment()) {
							
							
							if (!rowAttachementsCopy[i].row_->HasAnEndCap() && !rowAttachementsCopy[i].rowOther_->HasAnEndCap())
							{
								DetachFrom(rowAttachementsCopy[i].rowOther_, true);

							}

						}
					}

					
				}

			}
		}
		occupiedCountDown_--;
	}
	else
	{
		occupiedCountDown_ = occupiedCountDownCount_;
	}

}

void PiecePointRow::OnNodeSet(Node* node)
{
	if (node)
	{
		pieceManager_ = GetScene()->GetComponent<PieceManager>();
	}
	else
	{
		points_.clear();
	}
}

void PiecePointRow::DelayedStart()
{
	//resolve attachment data
	{
		for (RowAttachement& att : rowAttachements_)
		{
			att.constraint_ = dynamic_cast<NewtonConstraint*>(GetScene()->GetComponent(att.constraintId_));
			att.point = dynamic_cast<PiecePoint*>(GetScene()->GetComponent(att.pointId));
			att.pointOther_ = dynamic_cast<PiecePoint*>(GetScene()->GetComponent(att.pointOtherId_));
			att.row_ = dynamic_cast<PiecePointRow*>(GetScene()->GetComponent(att.rowId_));
			att.rowOther_ = dynamic_cast<PiecePointRow*>(GetScene()->GetComponent(att.rowOtherId_));
		}
	
	}
}
