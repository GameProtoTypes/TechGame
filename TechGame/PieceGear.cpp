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

		refreshCounter_ = PIECEGEAR_REFRESH_CNT + Random(0, 100);
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
		const float epsilon = 0.01f;
		
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



		if (delta.Length() <= connectionDist + epsilon && delta.Length() >= connectionDist - epsilon) {
			//URHO3D_LOGINFO("Gear within proximity");


			if (!constraintAlreadyExists) {
				URHO3D_LOGINFO("building gear link...");
				NewtonGearConstraint* constraint = node_->CreateComponent<NewtonGearConstraint>();
				constraint->SetOtherBody(otherGear->node_->GetComponent<Piece>()->GetRigidBody());
			}
		}
		else
		{
			if (constraintAlreadyExists)
			{
				URHO3D_LOGINFO("removing gear link..");
				node_->RemoveComponent(constraintOfInterest);
			}


		}

	}





	//URHO3D_LOGINFO("found " + ea::to_string(proximityPieceGears.size()) + " piece gears in proximity");
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
