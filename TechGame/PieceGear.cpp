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

		//ReEvalConstraints();

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
				break;
			}
		}

		if (!constraintAlreadyExists) {

			//continue searching on other gears.
			ea::vector<NewtonGearConstraint*> otherGearConstraints;
			otherGear->node_->GetComponents<NewtonGearConstraint>(otherGearConstraints);
			for (NewtonGearConstraint* otherGrCnstraint : otherGearConstraints) {
				if (otherGrCnstraint->GetOtherBody(false) == node_->GetComponent<Piece>()->GetRigidBody()) {
					constraintAlreadyExists = true;
					constraintOfInterest = otherGrCnstraint;
					break;
				}
			}
		}

		//At this point we know if there is a connection or not.



		const float epsilon = 0.02f;
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
		

		if (!constraintAlreadyExists && alignmentCheck && IsEnabledEffective()) {
			URHO3D_LOGINFO("building gear link...");
			NewtonGearConstraint* constraint = node_->CreateComponent<NewtonGearConstraint>();

			constraint->SetOtherBody(otherGear->node_->GetComponent<Piece>()->GetRigidBody());
			constraint->SetOwnPosition(Vector3::ZERO);

			Vector3 dir1 = GetNormal();
			Vector3 dir2 = otherGear->GetNormal();

			
			Quaternion localRotOwn;
			localRotOwn.FromRotationTo(Vector3::RIGHT, dir1);
			Quaternion localRotOther;
			localRotOther.FromRotationTo(Vector3::RIGHT, dir2);

			constraint->SetOwnRotation(localRotOwn);
			constraint->SetOtherRotation(localRotOther);


			


			float ratio = (radius_) / (otherGear->radius_);
			URHO3D_LOGINFO("Gear Ratio: " + ea::to_string(ratio));

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
		else if (constraintAlreadyExists && (!alignmentCheck || !IsEnabledEffective()))
		{
			URHO3D_LOGINFO("removing gear link.. distanceCheck: " + ea::to_string(distanceCheck)
				+ " angleCheck: " + ea::to_string(angleCheck) + " angle: " + ea::to_string(angle));

			//remove the constraint from either this gear or the other gear, whichever was found in the search.
			constraintOfInterest->Remove();
		}
	}

}

void PieceGear::DrawDebugGeometry(DebugRenderer* debug, bool depthTest)
{
	//draw normal direction
	debug->AddLine(node_->GetWorldPosition(), node_->GetWorldPosition() + node_->GetWorldRotation() * normal_*0.25f, Color::RED, depthTest);




}

void PieceGear::OnSetEnabled()
{
	if (IsEnabledEffective()) {
		URHO3D_LOGINFO("gear enabled");
	}
	else
	{
		URHO3D_LOGINFO("gear disabled");
	}

	ReEvalConstraints();
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
