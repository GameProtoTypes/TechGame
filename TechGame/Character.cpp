

#include <Urho3D/Core/Context.h>
#include <Urho3D/Graphics/AnimationController.h>
#include <Urho3D/IO/MemoryBuffer.h>
#include <Urho3D/Physics/PhysicsEvents.h>
#include <Urho3D/Physics/PhysicsWorld.h>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Scene/SceneEvents.h>


#include "NewtonPhysicsWorld.h"
#include "NewtonPhysicsEvents.h"
#include "NewtonRigidBody.h"

#include "Character.h"
#include "Urho3D/IO/Log.h"
#include "Urho3D/SystemUI/SystemUI.h"
#include "Urho3D/Input/Input.h"

Character::Character(Context* context) :
	LogicComponent(context),
	onGround_(false),
	okToJump_(true),
	inAirTimer_(0.0f)
{
	SetUpdateEventMask(USE_UPDATE);
}

void Character::RegisterObject(Context* context)
{
	context->RegisterFactory<Character>();

	// These macros register the class attributes to the Context for automatic load / save handling.
	// We specify the Default attribute mode which means it will be used both for saving into file, and network replication
	URHO3D_ATTRIBUTE("Controls Yaw", float, controls_.yaw_, 0.0f, AM_DEFAULT);
	URHO3D_ATTRIBUTE("Controls Pitch", float, controls_.pitch_, 0.0f, AM_DEFAULT);
	URHO3D_ATTRIBUTE("On Ground", bool, onGround_, false, AM_DEFAULT);
	URHO3D_ATTRIBUTE("OK To Jump", bool, okToJump_, true, AM_DEFAULT);
	URHO3D_ATTRIBUTE("In Air Timer", float, inAirTimer_, 0.0f, AM_DEFAULT);
}

void Character::Start()
{
	// Component has been inserted into its scene node. Subscribe to events now
	SubscribeToEvent(GetNode(), E_NEWTON_NODECOLLISION, URHO3D_HANDLER(Character, HandleNodeCollision));
	SubscribeToEvent(GetNode(), E_NEWTON_NODECOLLISIONEND, URHO3D_HANDLER(Character, HandleNodeCollisionEnd));
}

void Character::FixedUpdate(float timeStep)
{

}

void Character::Update(float timeStep)
{
	auto* input = GetSubsystem<Input>();

	if (!input->IsMouseGrabbed() || input->IsMouseVisible())
		return;

	// Clear previous controls
	controls_.Set(CTRL_FORWARD | CTRL_BACK | CTRL_LEFT | CTRL_RIGHT | CTRL_JUMP, false);


	// Update controls using keys
	controls_.Set(CTRL_FORWARD, input->GetKeyDown(KEY_W));
	controls_.Set(CTRL_BACK, input->GetKeyDown(KEY_S));
	controls_.Set(CTRL_LEFT, input->GetKeyDown(KEY_A));
	controls_.Set(CTRL_RIGHT, input->GetKeyDown(KEY_D));

	controls_.Set(CTRL_JUMP, input->GetKeyPress(KEY_SPACE));
	
	{
		controls_.yaw_ += (float)input->GetMouseMoveX() * YAW_SENSITIVITY;
		controls_.pitch_ += (float)input->GetMouseMoveY() * YAW_SENSITIVITY;
	}

	// Limit pitch
	controls_.pitch_ = Clamp(controls_.pitch_, -80.0f, 80.0f);

	
	// Set rotation already here so that it's updated every rendering frame instead of every physics frame
	GetNode()->SetRotation(Quaternion(controls_.yaw_, Vector3::UP));
		
	// Get camera lookat dir from character yaw + pitch
	const Quaternion& rot = node_->GetRotation();
	Quaternion dir = rot * Quaternion(controls_.pitch_, Vector3::RIGHT);

	// Turn head to camera pitch, but limit to avoid unnatural animation
	float limitPitch = Clamp(controls_.pitch_, -45.0f, 45.0f);
	Quaternion headDir = rot * Quaternion(limitPitch, Vector3(1.0f, 0.0f, 0.0f));


	cameraNode_->SetPosition(node_->GetWorldPosition() + Vector3(0, 0.7, 0) + rot * Vector3(0.0f, 0.15f, 0.2f));
	cameraNode_->SetRotation(dir);




	updatePhysics(timeStep);


}

void Character::SetCameraNode(Node* cameraNode)
{
	cameraNode_ = cameraNode;
}

void Character::HandleNodeCollision(StringHash eventType, VariantMap& eventData)
{
	// Check collision contacts and see if character is standing on ground (look for a contact that has near vertical normal)
	using namespace NewtonNodeCollision;

	NewtonRigidBodyContactEntry* contactData = static_cast<NewtonRigidBodyContactEntry*>((eventData[NewtonNodeCollision::P_CONTACT_DATA].GetPtr()));

	for (int i = 0; i < contactData->numContacts; i++)
	{
		Vector3 contactPosition = contactData->contactPositions[i];
		Vector3 contactNormal = contactData->contactNormals[i];

		// If contact is below node center and pointing up, assume it's a ground contact
		if (contactPosition.y_ < (node_->GetPosition().y_ + 1.0f))
		{
			float level = contactNormal.y_;
			if (level > 0.75) {
				onGround_ = true;
			}
			else
			{
				onGround_ = false;
			}
		}
	}
}

void Character::HandleNodeCollisionEnd(StringHash eventType, VariantMap& eventData)
{

}

void Character::updatePhysics(float timeStep)
{

	if (cameraNode_ == nullptr)
		return;

	/// \todo Could cache the components for faster access instead of finding them each frame
	auto* body = GetComponent<NewtonRigidBody>();


	// Update the in air timer. Reset if grounded
	if (!onGround_)
		inAirTimer_ += timeStep;
	else
		inAirTimer_ = 0.0f;
	// When character has been in air less than 1/10 second, it's still interpreted as being on ground
	bool softGrounded = inAirTimer_ < INAIR_THRESHOLD_TIME;

	// Update movement & animation
	const Quaternion& rot = cameraNode_->GetWorldRotation();
	Vector3 moveDir = Vector3::ZERO;
	const Vector3& velocity = body->GetLinearVelocity();
	// Velocity on the XZ plane
	Vector3 planeVelocity(velocity.x_, 0.0f, velocity.z_);
	Vector3 verticalVelocity(0.0f, velocity.y_, 0.0f);

	if (controls_.IsDown(CTRL_FORWARD))
		moveDir += Vector3::FORWARD;
	if (controls_.IsDown(CTRL_BACK))
		moveDir += Vector3::BACK;
	if (controls_.IsDown(CTRL_LEFT))
		moveDir += Vector3::LEFT;
	if (controls_.IsDown(CTRL_RIGHT))
		moveDir += Vector3::RIGHT;

	// Normalize move vector so that diagonal strafing is not faster
	if (moveDir.LengthSquared() > 0.0f)
		moveDir.Normalize();




	Vector3 forwardVectorWorld = (rot * moveDir);
	forwardVectorWorld.y_ = 0;
	forwardVectorWorld.Normalize();

	body->SetLinearVelocity(forwardVectorWorld * (softGrounded ? MOVE_SPEED : INAIR_MOVE_SPEED) + Vector3(0, velocity.y_, 0));

	//ui::Checkbox("onGround", &onGround_);
	if (onGround_ && softGrounded)
	{
		// Jump. Must release jump control between jumps
		if (controls_.IsDown(CTRL_JUMP))
		{

			body->SetLinearVelocity(velocity + Vector3::UP * JUMP_FORCE*2.0f);

		}
	}


	// Reset grounded flag for next frame
	onGround_ = false;


}
