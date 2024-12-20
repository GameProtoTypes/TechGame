#pragma once
#include <Urho3D/MLControl/Gym.h>



using namespace Urho3D;

class GYM_TrialBike : public GYM
{
public:
	URHO3D_OBJECT(GYM_TrialBike, GYM);

	explicit GYM_TrialBike(Context* context) : GYM(context)
	{

	}



	virtual void Reset()
	{
		GYM::Reset();

		timeLimit = 15.0f;
		rootNode = scene_->CreateChild("TrialBike");
		rootNode->AddTag("TrialBike");


		bool enableGyroOnWheels = true;

		//A (Engine Body)
		bodyNode = SpawnSamplePhysicsBox(rootNode, Vector3::ZERO, Vector3(1, 1, 0.5f));
		orbitNode = bodyNode;

		Node* B = SpawnSamplePhysicsBox(bodyNode, Vector3::ZERO + Vector3(-1, 0.7, 0), Vector3(2, 0.3, 0.5));
		B->RemoveComponent<NewtonRigidBody>();
		B->SetWorldRotation(Quaternion(0, 0, -30));

		Node* C = SpawnSamplePhysicsBox(rootNode, Vector3::ZERO + Vector3(-1, -0.5, 0), Vector3(2, 0.3, 0.5));
		C->SetWorldRotation(Quaternion(0, 0, 0));

		C->GetComponent<NewtonRigidBody>()->SetCollisionOverride(bodyNode->GetComponent<NewtonRigidBody>(), false);

		//make back spring.
		backSuspension = bodyNode->CreateComponent<NewtonRevoluteJoint>();
		backSuspension->SetOtherBody(C->GetComponent<NewtonRigidBody>());
		backSuspension->SetWorldRotation(Quaternion(90, 0, 90));
		backSuspension->SetWorldPosition(bodyNode->GetWorldPosition() + Vector3(0, -0.5, 0));
		//hingeConstraint->SetSolveMode(SOLVE_MODE_EXACT);


		Node* D = SpawnSamplePhysicsBox(bodyNode, Vector3::ZERO + Vector3(0.7, 0.5, 0), Vector3(1, 0.5, 0.5));
		D->RemoveComponent<NewtonRigidBody>();
		D->SetWorldRotation(Quaternion(0, 0, 45));


		Node* E = SpawnSamplePhysicsBox(rootNode, Vector3::ZERO + Vector3(1.5, 0, 0), Vector3(0.2, 2.5, 0.5));
		E->GetComponent<NewtonRigidBody>()->SetCollisionOverride(bodyNode->GetComponent<NewtonRigidBody>(), false);
		E->SetWorldRotation(Quaternion(0, 0, 20));


		NewtonRevoluteJoint* hinge = E->CreateComponent<NewtonRevoluteJoint>();
		hinge->SetOtherBody(bodyNode->GetComponent<NewtonRigidBody>());
		hinge->SetWorldPosition(Vector3::ZERO + Vector3(1.2, 0.8, 0));
		hinge->SetWorldRotation(Quaternion(0, 0, -90 + 20));
		//hinge->SetEnableHingeLimits(true);
	//	hinge->SetMaxAngle(30);
	//	hinge->SetMinAngle(-30);
		//hinge->SetSolveMode(SOLVE_MODE_EXACT);



		Node* F = SpawnSamplePhysicsBox(rootNode, Vector3::ZERO + Vector3(1.5, 0, 0), Vector3(0.2, 2.5, 0.5));
		F->SetWorldRotation(Quaternion(0, 0, 20));
		F->GetComponent<NewtonRigidBody>()->SetCollisionOverride(E->GetComponent<NewtonRigidBody>(), false);
		F->GetComponent<NewtonRigidBody>()->SetCollisionOverride(bodyNode->GetComponent<NewtonRigidBody>(), false);

		frontSuspension = F->CreateComponent<NewtonSliderConstraint>();
		frontSuspension->SetOtherBody(E->GetComponent<NewtonRigidBody>());
		frontSuspension->SetWorldRotation(F->GetWorldRotation() * Quaternion(0, 0, 90));
		frontSuspension->SetSliderLimits(-0.5, 0.5);

		//frontSuspension->SetSolveMode(SOLVE_MODE_EXACT);


		float wheelFriction = 20.0f;

		//backwheel
		Vector3 backWheelOffset = Vector3(-2.0, -0.5, 0);
		Node* backWheel = SpawnSamplePhysicsChamferCylinder(rootNode, Vector3::ZERO + backWheelOffset, 0.8f, 0.2f);
		backWheel->SetWorldRotation(Quaternion(90, 0, 0));
		backWheel->GetComponent<NewtonRigidBody>()->SetCollisionOverride(C->GetComponent<NewtonRigidBody>(), false);
		//backWheel->GetComponent<NewtonRigidBody>()->SetUseGyroscopicTorque(enableGyroOnWheels);
		backWheel->GetDerivedComponent<NewtonCollisionShape>()->SetFriction(wheelFriction);


		NewtonRevoluteJoint* motor = backWheel->CreateComponent<NewtonRevoluteJoint>();
		motor->SetOtherBody(C->GetComponent<NewtonRigidBody>());
		motor->SetWorldPosition(Vector3::ZERO + backWheelOffset);
		motor->SetWorldRotation(Quaternion(0, 90, 0));
		motor->SetEnableHingeLimits(false);


		Vector3 frontWheelOffset = Vector3(1.8, -1, 0);
		Node* frontWheel = SpawnSamplePhysicsChamferCylinder(rootNode, Vector3::ZERO + frontWheelOffset, 0.8f, 0.2f);
		frontWheel->SetWorldRotation(Quaternion(90, 0, 0));
		frontWheel->GetComponent<NewtonRigidBody>()->SetCollisionOverride(E->GetComponent<NewtonRigidBody>(), false);
		frontWheel->GetComponent<NewtonRigidBody>()->SetCollisionOverride(F->GetComponent<NewtonRigidBody>(), false);
		//frontWheel->GetComponent<NewtonRigidBody>()->SetUseGyroscopicTorque(enableGyroOnWheels);
		frontWheel->GetDerivedComponent<NewtonCollisionShape>()->SetFriction(wheelFriction);


		NewtonRevoluteJoint* frontAxle = frontWheel->CreateComponent<NewtonRevoluteJoint>();
		//frontAxle->SetPowerMode(NewtonRevoluteJoint::MOTOR_TORQUE);
		frontAxle->SetOtherBody(F->GetComponent<NewtonRigidBody>());
		frontAxle->SetWorldPosition(Vector3::ZERO + frontWheelOffset);
		frontAxle->SetWorldRotation(Quaternion(0, 90, 0));
		frontAxle->SetEnableHingeLimits(false);
		//frontAxle->SetSolveMode(SOLVE_MODE_EXACT);
		//frontAxle->SetMotorTargetAngularRate(10);

		motors.clear();
		
		motors.push_back(hinge);
		motors.push_back(motor);

		rootNode->SetWorldPosition(worldPos - Vector3(0,1.8,0));
		//rootNode->Rotate(Quaternion(Random(-1.0f,1.0f)*20, Vector3(1, 0, 0)));


		targetAngularVel.y_ = 0.0f;
		targetVel= 0.0f;
		motorTorques.clear();
		frontHingeTorques.clear();
		stat3.clear();
		stat4.clear();

	}


	virtual void ResizeVectors()
	{
		actionVec.resize(2);
		actionVec_1 = actionVec;

		FormResponses(0.0f);
		stateVec_1 = stateVec;
	}

	virtual void FormResponses(float timeStep)
	{
		GYM::FormResponses(timeStep);
		
		
		float angularY = bodyNode->GetComponent<NewtonRigidBody>()->GetAngularVelocity().y_;
		float forwardVel = bodyNode->GetComponent<NewtonRigidBody>()->GetLinearVelocity(TS_LOCAL).x_;
		SetNextState(targetAngularVel.y_);
		SetNextState(targetVel*0.1f);

		SetNextState(bodyNode->GetWorldDirection().y_);//side to side tilt indicator
		SetNextState(motors[0]->GetAngle()/(M_PI));
		SetNextState(forwardVel*0.1f);
		SetNextState(angularY);



		BuildStateDerivatives(timeStep);

		//reward for turning with target angular vel
		float turnAgreement = targetAngularVel.y_*angularY;
		SetNextRewardPart(10.0f*turnAgreement);

		//reward for velocity 
		float reward = targetVel*forwardVel;
		if (reward < 0.1f)
			reward = -100;

		SetNextRewardPart(10.0f*reward);


		//reward staying vertical
		SetNextRewardPart(10.0f*bodyNode->GetWorldUp().y_);
		
		//reward hint for steering into the lean
		SetNextRewardPart(-10.0f*motors[0]->GetAngle()*bodyNode->GetWorldDirection().y_);

		//punish hard turn angle
		SetNextRewardPart(-10.0f*Abs(motors[0]->GetAngle()));

		//energy save backwheel
		SetNextRewardPart(-actionVec_1[0]*actionVec_1[0] - actionVec_1[1]*actionVec_1[1]);

		//guide the bike to lean towards into direction of rotation
		SetNextRewardPart(10*bodyNode->GetWorldDirection().y_*targetAngularVel.y_);

		if (bodyNode->GetWorldUp().y_ <= 0.1f)
		{
			end = 1;
		}
	}


	virtual void Update(float timeStep)
	{
		GYM::Update(timeStep);


		//Front Spring Dynamics
		float k = 100;
		float d = 10;
		frontSuspension->SetCommandedForce(-k * frontSuspension->GetDisplacement() - d*frontSuspension->GetVel());

		//Back Spring Dynamics
		k = 10;
		d = 10;
		ui::Text("backagnel: %f", backSuspension->GetAngle());
		backSuspension->SetCommandedTorque(k * (backSuspension->GetAngle()) + d*backSuspension->GetHingeAngularVelocity());

		


		float curVel = bodyNode->GetComponent<NewtonRigidBody>()->GetLinearVelocity(TS_LOCAL).x_;
		stat5.push_back(curVel);

		int numJoysticks = GetSubsystem<Input>()->GetNumJoysticks();

		if (numJoysticks) {
			JoystickState* joyState = GetSubsystem<Input>()->GetJoystickByIndex(numJoysticks-1);
			targetAngularVel.y_ += 1.0f*timeStep*joyState->GetAxisPosition(0);
			targetAngularVel.y_ = Clamp(targetAngularVel.y_, -0.5f, 0.5f);


			throttleTorque = 20.0f*joyState->GetAxisPosition(5) - 105.0f*joyState->GetAxisPosition(4);

			if(joyState->GetButtonPress(0))
			{
				Reset();
			}
		}



		
		float curAngularVel = bodyNode->GetComponent<NewtonRigidBody>()->GetAngularVelocity().y_;
		float curForwardTilt = bodyNode->GetWorldRight().y_;
		float angVelError = (targetAngularVel.y_ - curAngularVel);


		//targetTilt += 5.0f*timeStep*(targetAngularVel.y_ - targetTilt);
		targetTilt = targetAngularVel.y_;

		//targetVel -= 100.0f*timeStep*angVelError*angVelError;
		//if (targetVel < baseVelFactorParam)
		//	targetVel = baseVelFactorParam;



		float curTilt = bodyNode->GetWorldDirection().y_;
		float tiltError = targetTilt - curTilt;


		float angleSharpFactor;
		if (curVel <= baseVelFactorParam)
			angleSharpFactor = 1.0f;
		else
			angleSharpFactor = (baseVelFactorParam / curVel);

		float dTerm = 0.1f*bodyNode->GetComponent<NewtonRigidBody>()->GetAngularVelocity(TS_LOCAL).x_;
		stat3.push_back(dTerm);
		float targetSteerAngle = angleSharpFactor*tiltError + dTerm;
		float curSteerAngle = motors[0]->GetAngle()/360.0f;


		float steerError = targetSteerAngle - curSteerAngle;
		stat4.push_back(steerError);
		float hingeTorque = hingeTorquePParam * steerError - 0.0f*motors[0]->GetWorldAngularRate().Length();
		motors[0]->SetCommandedTorque(-hingeTorque);
		frontHingeTorques.push_back(hingeTorque);


		float velError = targetVel - curVel;
		float forwardAngVel = bodyNode->GetComponent<NewtonRigidBody>()->GetAngularVelocity(TS_LOCAL).z_;
		float targetForwardTilt = 0.0f;
		float forwardTiltError = targetForwardTilt - curForwardTilt;

		motors[1]->SetCommandedTorque(-throttleTorque);
		stat6.push_back(motors[1]->GetWorldAngularRate().Length());
		motorTorques.push_back(throttleTorque);

		if (1) {
			ui::Begin("Control Model");


			ui::Text("angleSharpFactor: %f", angleSharpFactor);
			ui::Text("curAngularVel: %f", curAngularVel);
			ui::Separator();

			ui::DragFloat("Target Angular Vel", &targetAngularVel.y_, 0.01f, -0.5f, 0.5f);
			ui::DragFloat("Target Tilt", &targetTilt, 0.01f, -0.5f, 0.5f);
			ui::DragFloat("Target Vel", &targetVel, 0.01f, 5.0f, 1000.0f);
			if (numJoysticks == 0)
				ui::DragFloat("Back Wheel Torque", &throttleTorque, 0.01f, -20.0f, 20.0f);


			ui::Separator();


			ui::DragFloat("baseVelFactorParam", &baseVelFactorParam, 0.01f, 1.0f, 50.0f);
			ui::DragFloat("hingeTorquePParam", &hingeTorquePParam, 0.01f, 10.0f, 1000.0f);

			ui::End();

			ui::Begin("Stats");

			if (ImPlot::BeginPlot("Stats"))
			{
				ImPlot::SetupAxes("time", "torque", ImPlotAxisFlags_AutoFit, ImPlotAxisFlags_AutoFit);
				ImPlot::PlotLine("Back Motor Torque", &motorTorques[0], motorTorques.size());
				ImPlot::PlotLine("Front Hinge Motor Torque", &frontHingeTorques[0], frontHingeTorques.size());
				ImPlot::PlotLine("dTerm", &stat3[0], stat3.size());
				ImPlot::PlotLine("steerError", &stat4[0], stat4.size());
				ImPlot::PlotLine("forward Vel", &stat5[0], stat5.size());
				ImPlot::PlotLine("Back Omega", &stat6[0], stat6.size());
				ImPlot::EndPlot();
			}
			ui::End();
		}
	}

	virtual void ApplyActionVec(float timeStep)
	{
		GYM::ApplyActionVec(timeStep);


		//motors[0]->SetMotorTorque(actionVec[0]*3);
		//motors[1]->SetMotorTorque(actionVec[1]*10);
	}

	virtual void DrawDebugGeometry(DebugRenderer* debugRenderer)
	{
		debugRenderer->AddLine(bodyNode->GetWorldPosition(), bodyNode->GetWorldPosition() + targetAngularVel*10.0f, Color::RED);
		debugRenderer->AddLine(bodyNode->GetWorldPosition(), bodyNode->GetWorldPosition() + bodyNode->GetComponent<NewtonRigidBody>()->GetAngularVelocity(), Color::BLUE);
	}



	virtual void DrawUIStats(float timeStep)
	{
		GYM::DrawUIStats(timeStep);


	}

	ea::vector<NewtonRevoluteJoint*> motors;
	WeakPtr<Node> bodyNode;

	Vector3 targetAngularVel;

	ea::vector<float> motorTorques;
	ea::vector<float> frontHingeTorques;
	ea::vector<float> stat3;
	ea::vector<float> stat4;
	ea::vector<float> stat5;
	ea::vector<float> stat6;

	float targetTilt = 0.0f;
	float targetVel = 0.0f;

	float throttleTorque = 0.0f;

	float baseVelFactorParam = 10.0f;
	float hingeTorquePParam = 10.0f;

	WeakPtr<NewtonRevoluteJoint> backSuspension;
	WeakPtr<NewtonSliderConstraint> frontSuspension;
};