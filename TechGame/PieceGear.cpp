#include "PieceGear.h"
#include "Piece.h"
#include "NewtonGearConstraint.h"



PieceGear::PieceGear(Context* context) : Component(context)
{
	SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(PieceGear, HandleUpdate));
}

void PieceGear::RegisterObject(Context* context)
{
	context->RegisterFactory<PieceGear>();
}

void PieceGear::HandleUpdate(StringHash event, VariantMap& eventData)
{
	refreshCounter_--;
	if (refreshCounter_ <= 0) {


		ReEvalConstraints();

		refreshCounter_ = PIECEGEAR_REFRESH_CNT + Random(0, PIECEGEAR_REFRESH_CNT);
	}
}

void PieceGear::ReEvalConstraints()
{
	//check for proximity gears.
	PieceManager* pieceManager = GetScene()->GetComponent<PieceManager>();


	ea::vector<Piece*> blacklist;
	blacklist.push_back(node_->GetComponent<Piece>());

	ea::vector<Piece*> proximityPieces;
	pieceManager->GetGlobalPiecesInRadius(node_->GetWorldPosition(), blacklist, 5.0f, proximityPieces, 999);

	ea::vector<PieceGear*> proximityPieceGears;
	for (Piece* pc : proximityPieces) {
		if (pc->GetNode()->HasComponent<PieceGear>())
			proximityPieceGears.push_back(pc->GetNode()->GetComponent<PieceGear>());
	}



	ea::vector<NewtonGearConstraint*> ownGearConstraints;
	node_->GetComponents<NewtonGearConstraint>(ownGearConstraints);
	
	for (PieceGear* otherGear : proximityPieceGears) {

		Vector3 delta = otherGear->node_->GetWorldPosition() - node_->GetWorldPosition();

		float connectionDist = otherGear->GetRadius() + GetRadius();
		
		
		//search for existing constraint
		bool constraintAlreadyExists = false;
		NewtonGearConstraint* constraintOfInterest = nullptr;
		for (NewtonGearConstraint* gr : ownGearConstraints) {
			if (gr->GetOtherBody(false) == otherGear->node_->GetComponent<Piece>()->GetRigidBody()) {
				constraintAlreadyExists = true;
				constraintOfInterest = gr;
			}
		}

		ea::vector<NewtonGearConstraint*> otherGearConstraints;
		otherGear->node_->GetComponents<NewtonGearConstraint>(otherGearConstraints);
		for (NewtonGearConstraint* otherGrCnstraint : otherGearConstraints) {
			if (otherGrCnstraint->GetOtherBody(false) == node_->GetComponent<Piece>()->GetRigidBody()) {
				constraintAlreadyExists = true;
				constraintOfInterest = otherGrCnstraint;
			}
		}

		const float epsilon = 0.05f;
		bool alignmentCheck = true;

		//gears must be correct distance apart

		bool distanceCheck = (delta.Length() <= connectionDist + epsilon && delta.Length() >= connectionDist - epsilon);
		alignmentCheck &= distanceCheck;
		
		//URHO3D_LOGINFO(ea::to_string(alignmentCheck));
		
		
		//alignmentCheck &= (delta.Normalized().CrossProduct(GetWorldNormal()).Length() >= (1.0f - epsilon));
		



		
		//gears must also have the correct angle with each other
			//URHO3D_LOGINFO(ea::to_string(alignmentCheck));
		float angle = GetWorldNormal().Angle(otherGear->GetWorldNormal());

		while (angle >= 90)
			angle -= 180;
		while (angle <= -90)
			angle += 180;

		//URHO3D_LOGINFO(ea::to_string(angle));
		bool angleCheck = (Abs<float>(angle) < 10.0f);
		alignmentCheck &= angleCheck;

		//URHO3D_LOGINFO(ea::to_string(alignmentCheck));
		

		if (alignmentCheck) {

			if (!constraintAlreadyExists) {
				URHO3D_LOGINFO("building gear link...");
				NewtonGearConstraint* constraint = node_->CreateComponent<NewtonGearConstraint>();
				
				constraint->SetOtherBody(otherGear->node_->GetComponent<Piece>()->GetRigidBody());
				constraint->SetOwnPosition(Vector3::ZERO);
				constraint->SetOwnRotation(Quaternion(0, 90, 0));

				constraint->SetOtherRotation(Quaternion(0, 90, 0));


				Vector3 dir1 = GetWorldNormal();
				Vector3 dir2 = otherGear->GetWorldNormal();
				
				
				float ratio = (radius_) / (otherGear->radius_);
				
				//URHO3D_LOGINFO(ea::to_string(dir1.Angle(dir2)));
				if (Abs(dir1.Angle(dir2)) > 170.0f)
				{
					//URHO3D_LOGINFO("gear orientations opposite - using inverse ratio..");
					constraint->SetGearRatio(-ratio);
				}
				else
				{
					constraint->SetGearRatio(ratio);
				}

			}
		}
		else
		{
			if (constraintAlreadyExists)
			{
				URHO3D_LOGINFO("removing gear link.. distanceCheck: " + ea::to_string(distanceCheck) 
					+ " angleCheck: " + ea::to_string(angleCheck) + " angle: " + ea::to_string(angle));
				node_->RemoveComponent(constraintOfInterest);
			}


		}

	}





	//URHO3D_LOGINFO("found " + ea::to_string(proximityPieceGears.size()) + " piece gears in proximity");
}

void PieceGear::DrawDebugGeometry(DebugRenderer* debug, bool depthTest)
{
	//draw normal direction
	debug->AddLine(node_->GetWorldPosition(), node_->GetWorldPosition() + node_->GetWorldRotation() * normal_*0.25f, Color::RED, depthTest);




}

void PieceGear::OnNodeSet(Node* node)
{
	if (node)
	{


	}
	else
	{



	}
}
