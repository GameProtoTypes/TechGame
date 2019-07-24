

#pragma once

#include <Urho3D/Input/Controls.h>
#include <Urho3D/Scene/LogicComponent.h>

using namespace Urho3D;
class Urho3D::Node;

const unsigned CTRL_FORWARD = 1;
const unsigned CTRL_BACK = 2;
const unsigned CTRL_LEFT = 4;
const unsigned CTRL_RIGHT = 8;
const unsigned CTRL_JUMP = 16;

const float MOVE_SPEED = 3.0f;
const float INAIR_MOVE_SPEED = 1.5f;
const float BRAKE_FORCE = 0.2f;
const float JUMP_FORCE = 3.5f;
const float YAW_SENSITIVITY = 0.1f;
const float INAIR_THRESHOLD_TIME = 0.1f;

/// Character component, responsible for physical movement according to controls, as well as animation.
class Character : public LogicComponent
{
	URHO3D_OBJECT(Character, LogicComponent);

public:
	/// Construct.
	explicit Character(Context* context);

	/// Register object factory and attributes.
	static void RegisterObject(Context* context);

	/// Handle startup. Called by LogicComponent base class.
	void Start() override;
	/// Handle physics world update. Called by LogicComponent base class.
	void FixedUpdate(float timeStep) override;

	void Update(float timeStep) override;

	
	void SetIsVRCharacter(bool enable) { isVR_ = enable; }
	bool GetIsVRCharacter() const {
		return isVR_;
	}


	/// Movement controls. Assigned by the main program each frame.
	Controls controls_;


	///nodes that are part of the character.
	SharedPtr<Node> groundNode_;
	SharedPtr<Node> headNode_;
	SharedPtr<Node> rightHandNode_;
	SharedPtr<Node> leftHandNode_;


private:
	/// Handle physics collision event.
	void HandleNodeCollision(StringHash eventType, VariantMap& eventData);
	void HandleNodeCollisionEnd(StringHash eventType, VariantMap& eventData);

	void updatePhysics(float timeStep);


	/// Grounded flag for movement.
	bool onGround_;
	/// Jump flag.
	bool okToJump_;
	/// In air timer. Due to possible physics inaccuracy, character can be off ground for max. 1/10 second and still be allowed to move.
	float inAirTimer_;



	bool isVR_ = false;

protected:
	virtual void OnNodeSet(Node* node) override;
};
