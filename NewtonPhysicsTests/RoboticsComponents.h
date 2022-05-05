#pragma once
#include "Urho3DAll.h"

#include "NewtonRevoluteJoint.h"
using namespace Urho3D;

class BrushlessMotor : public Component
{
public:
	URHO3D_OBJECT(BrushlessMotor, Component);

	struct MotorPerformancePoint {
		float rpm = 0.0f;
		float currentAmps = 0.0f;
		float torque = 0.0f;
	};




	explicit BrushlessMotor(Context* context) : Component(context)
	{

	}



	void SetCurrent(float currentAmps)
	{
		//based off of the motor model, convert amps to torque.
		if (Abs(currentAmps) > maxCurrent)
			currentAmps = maxCurrent * Sign(currentAmps);

		float nMPerAmp = 250.0f / 5.0f;
		joint->SetCommandedTorque(currentAmps * nMPerAmp);
	}


	WeakPtr<NewtonRevoluteJoint> joint;


	virtual void OnNodeSet(Node* node)
	{
		Component::OnNodeSet(node);
		if (node != nullptr)
			joint = node->GetComponent<NewtonRevoluteJoint>(true);
		else
			joint = nullptr;
	}


}


