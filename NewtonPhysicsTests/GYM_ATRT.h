#pragma once

#include "NewtonModel.h"
#include "Urho3D/MLControl/Gym.h"



using namespace Urho3D;

class GYM_ATRT : public GYM
{
public:
	URHO3D_OBJECT(GYM_ATRT, GYM);

	explicit GYM_ATRT(Context* context) : GYM(context)
	{

	}


	void Reset() override
	{
		GYM::Reset();

		targetWorldVel = Vector3(-1,0,0);
		rootNode = scene_->CreateChild("ATRT");
		rootNode->AddTag("ATRT");

		orbitNode = rootNode->CreateChild("Orbit");
		motors.clear();
		leftHinges.clear();
		rightHinges.clear();
		//Body
		bodyNode = SpawnSamplePhysicsBox(rootNode, Vector3::ZERO, Vector3(1, 1, 1));
		

		//LEFT LEG
		Node* HIP_LEFT = SpawnSamplePhysicsCylinder(rootNode, Vector3(0.0, -0.5, -0.5), 0.5, 0.25);
		HIP_LEFT->Rotate(Quaternion(90, Vector3(1, 0, 0)));

		NewtonHingeConstraint* HIPBODYJOINT_LEFT = bodyNode->CreateComponent<NewtonHingeConstraint>();
		HIPBODYJOINT_LEFT->SetRotation(Quaternion(90, Vector3(0, 1, 0)));
		HIPBODYJOINT_LEFT->SetPosition(Vector3(0.0, -0.5, -0.5));
		HIPBODYJOINT_LEFT->SetOtherBody(HIP_LEFT->GetComponent<NewtonRigidBody>());
		leftHinges.push_back(HIPBODYJOINT_LEFT);



		Node* KNEE_LEFT = SpawnSamplePhysicsCylinder(HIP_LEFT, Vector3(0.5, -1.5, -0.5), 0.3, 0.25);
		KNEE_LEFT->RemoveComponent<NewtonRigidBody>();

		Node* KNEE2_LEFT = SpawnSamplePhysicsCylinder(rootNode, Vector3(0.5, -1.5, -0.75), 0.3, 0.25);
		KNEE2_LEFT->Rotate(Quaternion(90, Vector3(1, 0, 0)));

		NewtonHingeConstraint* KNEEJOINT_LEFT = HIP_LEFT->CreateComponent<NewtonHingeConstraint>();
		KNEEJOINT_LEFT->SetRotation(Quaternion(90, Vector3(0, 0, 1)));
		KNEEJOINT_LEFT->SetWorldPosition(KNEE_LEFT->GetWorldPosition());
		KNEEJOINT_LEFT->SetOtherBody(KNEE2_LEFT->GetComponent<NewtonRigidBody>());
		leftHinges.push_back(KNEEJOINT_LEFT);



		Node* KNEE3_LEFT = SpawnSamplePhysicsCylinder(KNEE2_LEFT, Vector3(0.5, -2.5, -0.5), 0.3, 0.25);
		KNEE3_LEFT->RemoveComponent<NewtonRigidBody>();

		KNEE_LOWER_LEFT = SpawnSamplePhysicsCylinder(rootNode, Vector3(0.5, -2.5, -0.75), 0.3, 0.25);
		KNEE_LOWER_LEFT->Rotate(Quaternion(90, Vector3(1, 0, 0)));

		NewtonHingeConstraint* KNEEJOINT2_LEFT = KNEE2_LEFT->CreateComponent<NewtonHingeConstraint>();
		KNEEJOINT2_LEFT->SetRotation(Quaternion(90, Vector3(0, 0, 1)));
		KNEEJOINT2_LEFT->SetWorldPosition(KNEE3_LEFT->GetWorldPosition());
		KNEEJOINT2_LEFT->SetOtherBody(KNEE_LOWER_LEFT->GetComponent<NewtonRigidBody>());
		leftHinges.push_back(KNEEJOINT2_LEFT);



		FOOT_LEFT = SpawnSamplePhysicsCylinder(KNEE_LOWER_LEFT, Vector3(0, -3, -0.5), 0.2, 0.25);
		FOOT_LEFT->RemoveComponent<NewtonRigidBody>();
		FOOT_LEFT->GetDerivedComponent<NewtonCollisionShape>()->SetFriction(10.0f);



		Node* FOOT_LEFT2 = SpawnSamplePhysicsCylinder(KNEE_LOWER_LEFT, Vector3(0, -3, -0.75), 0.2, 0.25);
		FOOT_LEFT2->GetDerivedComponent<NewtonCollisionShape>()->SetFriction(10.0f);

		NewtonHingeConstraint* ENDJOINT_LEFT = KNEE_LOWER_LEFT->CreateComponent<NewtonHingeConstraint>();
		ENDJOINT_LEFT->SetWorldPosition(FOOT_LEFT->GetWorldPosition());
		ENDJOINT_LEFT->SetRotation(Quaternion(90, Vector3(0, 0, 1)));
		ENDJOINT_LEFT->SetOtherBody(FOOT_LEFT2->GetComponent<NewtonRigidBody>());
		leftHinges.push_back(ENDJOINT_LEFT);








		//RIGHT LEG
		Node* HIP_RIGHT = SpawnSamplePhysicsCylinder(rootNode, Vector3(0.0, -0.5, 0.5), 0.5, 0.25);
		HIP_RIGHT->Rotate(Quaternion(90, Vector3(1, 0, 0)));

		NewtonHingeConstraint* HIPBODYJOINT_RIGHT = bodyNode->CreateComponent<NewtonHingeConstraint>();
		HIPBODYJOINT_RIGHT->SetRotation(Quaternion(90, Vector3(0, 1, 0)));
		HIPBODYJOINT_RIGHT->SetPosition(Vector3(0.0, -0.5, 0.5));
		HIPBODYJOINT_RIGHT->SetOtherBody(HIP_RIGHT->GetComponent<NewtonRigidBody>());
		rightHinges.push_back(HIPBODYJOINT_RIGHT);



		Node* KNEE_RIGHT = SpawnSamplePhysicsCylinder(HIP_RIGHT, Vector3(0.5, -1.5, 0.75), 0.3, 0.25);
		KNEE_RIGHT->RemoveComponent<NewtonRigidBody>();

		Node* KNEE2_RIGHT = SpawnSamplePhysicsCylinder(rootNode, Vector3(0.5, -1.5, 0.5), 0.3, 0.25);
		KNEE2_RIGHT->Rotate(Quaternion(90, Vector3(1, 0, 0)));

		NewtonHingeConstraint* KNEEJOINT_RIGHT = HIP_RIGHT->CreateComponent<NewtonHingeConstraint>();
		KNEEJOINT_RIGHT->SetRotation(Quaternion(90, Vector3(0, 0, 1)));
		KNEEJOINT_RIGHT->SetWorldPosition(KNEE_RIGHT->GetWorldPosition());
		KNEEJOINT_RIGHT->SetOtherBody(KNEE2_RIGHT->GetComponent<NewtonRigidBody>());
		rightHinges.push_back(KNEEJOINT_RIGHT);


		Node* KNEE3_RIGHT = SpawnSamplePhysicsCylinder(KNEE2_RIGHT, Vector3(0.5, -2.5, 0.75), 0.3, 0.25);
		KNEE3_RIGHT->RemoveComponent<NewtonRigidBody>();

		KNEE_LOWER_RIGHT = SpawnSamplePhysicsCylinder(rootNode, Vector3(0.5, -2.5, 0.5), 0.3, 0.25);
		KNEE_LOWER_RIGHT->Rotate(Quaternion(90, Vector3(1, 0, 0)));

		NewtonHingeConstraint* KNEEJOINT2_RIGHT = KNEE2_RIGHT->CreateComponent<NewtonHingeConstraint>();
		KNEEJOINT2_RIGHT->SetRotation(Quaternion(90, Vector3(0, 0, 1)));
		KNEEJOINT2_RIGHT->SetWorldPosition(KNEE3_RIGHT->GetWorldPosition());
		KNEEJOINT2_RIGHT->SetOtherBody(KNEE_LOWER_RIGHT->GetComponent<NewtonRigidBody>());
		rightHinges.push_back(KNEEJOINT2_RIGHT);

		FOOT_RIGHT = SpawnSamplePhysicsCylinder(KNEE_LOWER_RIGHT, Vector3(0, -3, 0.5), 0.2, 0.25);
		FOOT_RIGHT->RemoveComponent<NewtonRigidBody>();
		FOOT_RIGHT->GetDerivedComponent<NewtonCollisionShape>()->SetFriction(10.0f);


		Node* FOOT_RIGHT2 = SpawnSamplePhysicsCylinder(KNEE_LOWER_RIGHT, Vector3(0, -3, 0.75), 0.2, 0.25);
		FOOT_RIGHT2->GetDerivedComponent<NewtonCollisionShape>()->SetFriction(10.0f);

		NewtonHingeConstraint* ENDJOINT_RIGHT = KNEE_LOWER_RIGHT->CreateComponent<NewtonHingeConstraint>();
		ENDJOINT_RIGHT->SetWorldPosition(FOOT_RIGHT->GetWorldPosition());
		ENDJOINT_RIGHT->SetRotation(Quaternion(90, Vector3(0, 0, 1)));
		ENDJOINT_RIGHT->SetOtherBody(FOOT_RIGHT2->GetComponent<NewtonRigidBody>());
		rightHinges.push_back(ENDJOINT_RIGHT);




		motors.push_back(HIPBODYJOINT_LEFT);
		motors.push_back(HIPBODYJOINT_RIGHT);
		motors.push_back(KNEEJOINT_LEFT);
		motors.push_back(KNEEJOINT_RIGHT);
		motors.push_back(KNEEJOINT2_LEFT);
		motors.push_back(KNEEJOINT2_RIGHT);


		rootNode->SetWorldPosition(worldPos);
		//rootNode->Rotate(Quaternion(RandomNormal(0, 45), Vector3(0, 0, 1)));
	}

	void Update(float timestep) override
	{
		GYM::Update(timestep);

		ChainJacobian chainJac;
		NewtonModel* model = rightHinges[0]->GetModel();
		Matrix3x4 rootTransform = rightHinges[0]->GetOwnWorldFrame();
		Matrix3x4 endEffectorWorld = rightHinges.back()->GetOwnWorldFrame();
		Matrix3x4 endEffectorRelRoot = rootTransform.Inverse() * endEffectorWorld;

		static float vertical = 1.0f;
		ui::SliderFloat("vertical", &vertical, 0.0f, 10.0f);

		Vector3 targetRightFootPosWorld = rightHinges[0]->GetOwnWorldFrame().Translation() - Vector3(0, vertical, 0);

		Vector3 targetPos = rootTransform.Inverse() * targetRightFootPosWorld;
		Vector3 delta = (targetPos - endEffectorRelRoot.Translation());



		static float gain = 3.0f;
		static float damping = 1.0f;
		ui::SliderFloat("Gain", &gain, 1.0f, 100.0f);
		ui::SliderFloat("Damping", &damping, 0.0f, 10.0f);


		Vector3 rightFootForce = gain * delta - damping * (rootTransform.RotationMatrix().Inverse() * rightHinges.back()->GetOwnWorldFrameVel());
		Vector3 rightFootTorque(0, 0, 0);



		model->CalculateChainJabobian(rightHinges, chainJac);
		ea::vector<float> jointTorques;
		model->SolveForJointTorques(chainJac, rightHinges, rightFootForce, rightFootTorque, jointTorques);

		for(int i = 0; i < rightHinges.size(); i++)
		{
			float torque = jointTorques[i];
			torque = Clamp(torque, -1000.0f, 1000.0f);

			rightHinges[i]->SetCommandedTorque(torque);
		}

	}

	void ResizeVectors() override
	{
		actionVec.resize(6);
		actionVec_1 = actionVec;

		FormResponses(0.0f);
		stateVec_1 = stateVec;
	}

	void FormResponses(float timeStep) override
	{
		GYM::FormResponses(timeStep);
	

		int vertState = SetNextState((bodyNode->GetWorldPosition().y_ - 2.0f)*0.5f);//Vertical Translation

		//linear velocity
		//Vector3 worldVel = bodyNode->GetComponent<NewtonRigidBody>()->GetLinearVelocity();
		//SetNextState(worldVel.x_ / 10.0f);                     
		//SetNextState(worldVel.y_ / 10.0f);
		//SetNextState(worldVel.z_ / 10.0f);

		//local linear velocity
		Vector3 vel = bodyNode->GetComponent<NewtonRigidBody>()->GetLinearVelocity(Urho3D::TS_LOCAL);
		SetNextState(vel.x_ / 10.0f);
		SetNextState(vel.y_ / 10.0f);
		SetNextState(vel.z_ / 10.0f);



		int fbtilt = SetNextState(bodyNode->GetWorldRight().y_);    //forward-back tilt indicator
		int rolltilt = SetNextState(bodyNode->GetWorldDirection().y_);//roll indicator


		SetNextState(FOOT_LEFT->GetWorldPosition().y_);
		SetNextState(FOOT_RIGHT->GetWorldPosition().y_);
		


		//SetNextState(targetWorldVel.x_);
		//SetNextState(targetWorldVel.z_);

		for (int i = 0; i < motors.size(); i++)
		{
			//SetNextState(motors[i]->GetOwnBody()->GetNode()->GetPosition().x_);
			//SetNextState(motors[i]->GetOwnBody()->GetNode()->GetPosition().y_);

			SetNextState(sin(motors[i]->GetAngle()));
			SetNextState(cos(motors[i]->GetAngle()));

			SetNextState(motors[i]->GetWorldAngularRate().Length()/10.0f);
		}


		//BuildStateDerivatives(timeStep);



		SetNextRewardPart(Pow(2.0f, stateVec[vertState]));//vertical displacement


		float discountFeetAboveKnee = 0.0f;
		if (FOOT_LEFT->GetWorldPosition().y_ >= KNEE_LOWER_LEFT->GetWorldPosition().y_)
		{
			discountFeetAboveKnee = -10000;
		}
		if (FOOT_RIGHT->GetWorldPosition().y_ >= KNEE_LOWER_RIGHT->GetWorldPosition().y_)
		{
			discountFeetAboveKnee = -10000;
		}

		SetNextRewardPart(discountFeetAboveKnee);


//SetNextRewardPart(-0.1*Abs(stateVec[fbtilt]));
//SetNextRewardPart(-0.1*Abs(stateVec[rolltilt]));




		//float velocityAgreement = worldVel.DotProduct(targetWorldVel.Normalized()) / targetWorldVel.Length();
		//SetNextRewardPart(10*velocityAgreement);
		//SetNextRewardPart(0.1*timeStep);
		//SetNextRewardPart(-(Abs(actionVec_1[0])+
		//	Abs(actionVec_1[1]) +
		//		Abs(actionVec_1[2]) +
		//			Abs(actionVec_1[3]) +
		//				Abs(actionVec_1[4]) +
		//					Abs(actionVec_1[5])));//prev torques



		if (bodyNode->GetWorldPosition().y_ < 1.0f)
		{
			end = 1;
		}


	}

	void ApplyActionVec(float timeStep) override
	{
		GYM::ApplyActionVec(timeStep);

		//for(int m = 0; m < motors.size(); m++ )
			//motors[m]->SetMotorTorque(actionVec[m] * 10);

	}

	void DrawDebugGeometry(DebugRenderer* debugRenderer) override
	{
		debugRenderer->AddLine(bodyNode->GetWorldPosition(), bodyNode->GetWorldPosition() + targetWorldVel * 5.0f, Color::RED);
	}

	ea::vector<NewtonHingeConstraint*> motors;

	ea::vector<NewtonHingeConstraint*> leftHinges;
	ea::vector<NewtonHingeConstraint*> rightHinges;


	WeakPtr<Node> bodyNode;
	WeakPtr<Node> FOOT_RIGHT;
	WeakPtr<Node> FOOT_LEFT;
	WeakPtr<Node> KNEE_LOWER_RIGHT;
	WeakPtr<Node> KNEE_LOWER_LEFT;



	Vector3 targetWorldVel;
};




