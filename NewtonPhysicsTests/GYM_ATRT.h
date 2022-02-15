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


		motors.clear();
		leftHinges.clear();
		rightHinges.clear();

		//Body
		bodyNode = SpawnSamplePhysicsBox(rootNode, Vector3::ZERO, Vector3(1, 1, 1));
		bodyNode->GetComponent<NewtonRigidBody>()->SetMassScale(10);
		orbitNode = bodyNode->CreateChild("Orbit");
		//LEFT LEG
		Node* HIP_LEFT = SpawnSamplePhysicsCylinder(rootNode, Vector3(0.0, -0.5, -0.5), 0.5, 0.25);
		HIP_LEFT->Rotate(Quaternion(90, Vector3(1, 0, 0)));

		NewtonRevoluteJoint* HIPBODYJOINT_LEFT = bodyNode->CreateComponent<NewtonRevoluteJoint>();
		HIPBODYJOINT_LEFT->SetRotation(Quaternion(90, Vector3(0, 1, 0)));
		HIPBODYJOINT_LEFT->SetPosition(Vector3(0.0, -0.5, -0.5));
		HIPBODYJOINT_LEFT->SetOtherBody(HIP_LEFT->GetComponent<NewtonRigidBody>());
		leftHinges.push_back(HIPBODYJOINT_LEFT);



		Node* KNEE_LEFT = SpawnSamplePhysicsCylinder(HIP_LEFT, Vector3(0.5, -1.5, -0.5), 0.3, 0.25);
		KNEE_LEFT->RemoveComponent<NewtonRigidBody>();

		Node* KNEE2_LEFT = SpawnSamplePhysicsCylinder(rootNode, Vector3(0.5, -1.5, -0.75), 0.3, 0.25);
		KNEE2_LEFT->Rotate(Quaternion(90, Vector3(1, 0, 0)));

		NewtonRevoluteJoint* KNEEJOINT_LEFT = HIP_LEFT->CreateComponent<NewtonRevoluteJoint>();
		KNEEJOINT_LEFT->SetRotation(Quaternion(90, Vector3(0, 0, 1)));
		KNEEJOINT_LEFT->SetWorldPosition(KNEE_LEFT->GetWorldPosition());
		KNEEJOINT_LEFT->SetOtherBody(KNEE2_LEFT->GetComponent<NewtonRigidBody>());
		leftHinges.push_back(KNEEJOINT_LEFT);



		Node* KNEE3_LEFT = SpawnSamplePhysicsCylinder(KNEE2_LEFT, Vector3(0.5, -2.5, -0.5), 0.3, 0.25);
		KNEE3_LEFT->RemoveComponent<NewtonRigidBody>();

		KNEE_LOWER_LEFT = SpawnSamplePhysicsCylinder(rootNode, Vector3(0.5, -2.5, -0.75), 0.3, 0.25);
		KNEE_LOWER_LEFT->Rotate(Quaternion(90, Vector3(1, 0, 0)));

		NewtonRevoluteJoint* KNEEJOINT2_LEFT = KNEE2_LEFT->CreateComponent<NewtonRevoluteJoint>();
		KNEEJOINT2_LEFT->SetRotation(Quaternion(90, Vector3(0, 0, 1)));
		KNEEJOINT2_LEFT->SetWorldPosition(KNEE3_LEFT->GetWorldPosition());
		KNEEJOINT2_LEFT->SetOtherBody(KNEE_LOWER_LEFT->GetComponent<NewtonRigidBody>());
		leftHinges.push_back(KNEEJOINT2_LEFT);



		FOOT_LEFT = SpawnSamplePhysicsCylinder(KNEE_LOWER_LEFT, Vector3(0, -3, -0.5), 0.2, 0.25);
		FOOT_LEFT->RemoveComponent<NewtonRigidBody>();
		FOOT_LEFT->GetDerivedComponent<NewtonCollisionShape>()->SetFriction(10.0f);



		FOOT_LEFT2 = SpawnSamplePhysicsBox(KNEE_LOWER_LEFT, Vector3(-0.2, -3, -0.6), Vector3(0.1, 0.3, 0.75));
		FOOT_LEFT2->GetDerivedComponent<NewtonCollisionShape>()->SetFriction(10.0f);

		NewtonRevoluteJoint* ENDJOINT_LEFT = KNEE_LOWER_LEFT->CreateComponent<NewtonRevoluteJoint>();
		ENDJOINT_LEFT->SetWorldPosition(FOOT_LEFT->GetWorldPosition());
		ENDJOINT_LEFT->SetRotation(Quaternion(90, Vector3(0, 0, 1)));
		ENDJOINT_LEFT->SetOtherBody(FOOT_LEFT2->GetComponent<NewtonRigidBody>());
		ENDJOINT_LEFT->SetMinAngle(-80.0f);
		ENDJOINT_LEFT->SetMaxAngle(20.0f);
		leftHinges.push_back(ENDJOINT_LEFT);








		//RIGHT LEG
		Node* HIP_RIGHT = SpawnSamplePhysicsCylinder(rootNode, Vector3(0.0, -0.5, 0.5), 0.5, 0.25);
		HIP_RIGHT->Rotate(Quaternion(90, Vector3(1, 0, 0)));

		NewtonRevoluteJoint* HIPBODYJOINT_RIGHT = bodyNode->CreateComponent<NewtonRevoluteJoint>();
		HIPBODYJOINT_RIGHT->SetRotation(Quaternion(90, Vector3(0, 1, 0)));
		HIPBODYJOINT_RIGHT->SetPosition(Vector3(0.0, -0.5, 0.5));
		HIPBODYJOINT_RIGHT->SetOtherBody(HIP_RIGHT->GetComponent<NewtonRigidBody>());
		rightHinges.push_back(HIPBODYJOINT_RIGHT);



		Node* KNEE_RIGHT = SpawnSamplePhysicsCylinder(HIP_RIGHT, Vector3(0.5, -1.5, 0.75), 0.3, 0.25);
		KNEE_RIGHT->RemoveComponent<NewtonRigidBody>();

		Node* KNEE2_RIGHT = SpawnSamplePhysicsCylinder(rootNode, Vector3(0.5, -1.5, 0.5), 0.3, 0.25);
		KNEE2_RIGHT->Rotate(Quaternion(90, Vector3(1, 0, 0)));

		NewtonRevoluteJoint* KNEEJOINT_RIGHT = HIP_RIGHT->CreateComponent<NewtonRevoluteJoint>();
		KNEEJOINT_RIGHT->SetRotation(Quaternion(90, Vector3(0, 0, 1)));
		KNEEJOINT_RIGHT->SetWorldPosition(KNEE_RIGHT->GetWorldPosition());
		KNEEJOINT_RIGHT->SetOtherBody(KNEE2_RIGHT->GetComponent<NewtonRigidBody>());
		rightHinges.push_back(KNEEJOINT_RIGHT);


		Node* KNEE3_RIGHT = SpawnSamplePhysicsCylinder(KNEE2_RIGHT, Vector3(0.5, -2.5, 0.75), 0.3, 0.25);
		KNEE3_RIGHT->RemoveComponent<NewtonRigidBody>();

		KNEE_LOWER_RIGHT = SpawnSamplePhysicsCylinder(rootNode, Vector3(0.5, -2.5, 0.5), 0.3, 0.25);
		KNEE_LOWER_RIGHT->Rotate(Quaternion(90, Vector3(1, 0, 0)));

		NewtonRevoluteJoint* KNEEJOINT2_RIGHT = KNEE2_RIGHT->CreateComponent<NewtonRevoluteJoint>();
		KNEEJOINT2_RIGHT->SetRotation(Quaternion(90, Vector3(0, 0, 1)));
		KNEEJOINT2_RIGHT->SetWorldPosition(KNEE3_RIGHT->GetWorldPosition());
		KNEEJOINT2_RIGHT->SetOtherBody(KNEE_LOWER_RIGHT->GetComponent<NewtonRigidBody>());
		rightHinges.push_back(KNEEJOINT2_RIGHT);

		FOOT_RIGHT = SpawnSamplePhysicsCylinder(KNEE_LOWER_RIGHT, Vector3(0, -3, 0.5), 0.2, 0.25);
		FOOT_RIGHT->RemoveComponent<NewtonRigidBody>();
		FOOT_RIGHT->GetDerivedComponent<NewtonCollisionShape>()->SetFriction(10.0f);


		FOOT_RIGHT2 = SpawnSamplePhysicsBox(KNEE_LOWER_RIGHT, Vector3(-0.2, -3, 0.6), Vector3(0.1,0.3,0.75));
		FOOT_RIGHT2->GetDerivedComponent<NewtonCollisionShape>()->SetFriction(10.0f);

		NewtonRevoluteJoint* ENDJOINT_RIGHT = KNEE_LOWER_RIGHT->CreateComponent<NewtonRevoluteJoint>();
		ENDJOINT_RIGHT->SetWorldPosition(FOOT_RIGHT->GetWorldPosition());
		ENDJOINT_RIGHT->SetRotation(Quaternion(90, Vector3(0, 0, 1)));
		ENDJOINT_RIGHT->SetOtherBody(FOOT_RIGHT2->GetComponent<NewtonRigidBody>());
		ENDJOINT_RIGHT->SetMinAngle(-80.0f);
		ENDJOINT_RIGHT->SetMaxAngle(20.0f);
		rightHinges.push_back(ENDJOINT_RIGHT);





		motors.push_back(HIPBODYJOINT_LEFT);
		motors.push_back(HIPBODYJOINT_RIGHT);
		motors.push_back(KNEEJOINT_LEFT);
		motors.push_back(KNEEJOINT_RIGHT);
		motors.push_back(KNEEJOINT2_LEFT);
		motors.push_back(KNEEJOINT2_RIGHT);


		rootNode->SetWorldPosition(worldPos);
		//rootNode->Rotate(Quaternion(RandomNormal(0, 45), Vector3(0, 0, 1)));

		scene_->GetComponent<NewtonPhysicsWorld>()->BuildAndUpdateNewtonModels();

	}

	void Update(float timestep) override
	{
		GYM::Update(timestep);

		static float minAngle = -25.0f;
		//ui::SliderFloat("minAngles", &minAngle, -45.0f, 0.0f);
		static float maxAngle = 45.0f;
		//ui::SliderFloat("maxAngles", &maxAngle, 0, 45.0f);
		for (int i = 0; i < rightHinges.size(); i++)
		{
			rightHinges[i]->SetMinAngle(minAngle);
			leftHinges[i]->SetMinAngle(minAngle);
			rightHinges[i]->SetMaxAngle(maxAngle);
			leftHinges[i]->SetMaxAngle(maxAngle);
		}

		leftHinges.back()->SetEnableHingeLimits(false);
		rightHinges.back()->SetEnableHingeLimits(false);


		leftHinges[1]->SetMinAngle(0.0f);
		rightHinges[1]->SetMinAngle(0.0f);


		NewtonModel* model = rightHinges[0]->GetModel();

		Matrix3x4 rootTransformRight = rightHinges[0]->GetOwnWorldFrame();
		Matrix3x4 endEffectorRightWorld = rightHinges.back()->GetOwnWorldFrame();
		Matrix3x4 endEffectorRightRelRoot = rootTransformRight.Inverse() * endEffectorRightWorld;

		Matrix3x4 rootTransformLeft = leftHinges[0]->GetOwnWorldFrame();
		Matrix3x4 endEffectorLeftWorld = leftHinges.back()->GetOwnWorldFrame();
		Matrix3x4 endEffectorLeftRelRoot = rootTransformLeft.Inverse() * endEffectorLeftWorld;

		
		static float timeFactor = 50.0f;
		static float footRaiseAmplitude = -0.1f;
		static float footStrideAmplitude = -0.2f;
		static float footStrideOffset = -0.2f;
		//ui::SliderFloat("Vertical", &verticalFeetOffset, 0.0f, 5.0f);
		//ui::SliderFloat("Time Factor", &timeFactor, 0.0f, 500.0f);
		//ui::SliderFloat("footRaiseAmplitude", &footRaiseAmplitude, -1.0f, 1.0f);
		//ui::SliderFloat("footStrideAmplitude", &footStrideAmplitude, -1.0f, 1.0f);
		//ui::SliderFloat("footStrideOffset", &footStrideOffset, -1.0f, 1.0f);


		footPhaseDeg += timeFactor*timestep;
		if (footPhaseDeg > 360.0)
			footPhaseDeg = 360.0 - footPhaseDeg;

		//rightFootLocalOffset = Vector3(0,-verticalFeetOffset + footRaiseAmplitude * Sin(footPhaseDeg), Sin(footPhaseDeg + 90.0f) * footStrideAmplitude + footStrideOffset);
		targetRightFootPosWorld = (rootTransformRight * rightFootLocalOffset);

		//leftFootLocalOffset = Vector3(0, -verticalFeetOffset - footRaiseAmplitude * Sin(footPhaseDeg), -Sin(footPhaseDeg + 90.0f) * footStrideAmplitude + footStrideOffset);
		targetLeftFootPosWorld = (rootTransformLeft * leftFootLocalOffset);




		Vector3 targetRightPos = rootTransformRight.Inverse() * targetRightFootPosWorld;
		Vector3 deltaRight = (targetRightPos - endEffectorRightRelRoot.Translation());

		Vector3 targetLeftPos = rootTransformLeft.Inverse() * targetLeftFootPosWorld;
		Vector3 deltaLeft = (targetLeftPos - endEffectorLeftRelRoot.Translation());



		static float gain = 51.0f;
		static float damping = 1.0f;
		//ui::SliderFloat("Gain", &gain, 1.0f, 100.0f);
		//ui::SliderFloat("Damping", &damping, 0.0f, 10.0f);

		float leftFootLevelingTorque = FOOT_LEFT2->GetWorldDirection().y_;
		float rightFootLevelingTorque = FOOT_RIGHT2->GetWorldDirection().y_;

		Vector3 rightFootForce = gain * deltaRight - damping * (rootTransformRight.RotationMatrix().Inverse() * rightHinges.back()->GetOwnWorldFrameVel());
		Vector3 rightFootTorque(rightFootLevelingTorque, 0, 0);
		Vector3 leftFootForce = gain * deltaLeft - damping * (rootTransformLeft.RotationMatrix().Inverse() * leftHinges.back()->GetOwnWorldFrameVel());
		Vector3 leftFootTorque(leftFootLevelingTorque, 0, 0 );

		ChainJacobian chainJacRight;
		ChainJacobian chainJacLeft;

		model->CalculateChainJabobian(rightHinges, chainJacRight);
		model->CalculateChainJabobian(leftHinges, chainJacLeft);
		ea::vector<float> jointTorquesRight;
		ea::vector<float> jointTorquesLeft;
		model->SolveForJointTorques(chainJacRight, rightHinges, rightFootForce, rightFootTorque, jointTorquesRight);
		model->SolveForJointTorques(chainJacLeft, leftHinges, leftFootForce, leftFootTorque, jointTorquesLeft);



		for(int i = 0; i < rightHinges.size(); i++)
		{
			float torque = jointTorquesRight[i];
			torque = Clamp(torque, -1000.0f, 1000.0f);

			rightHinges[i]->SetCommandedTorque(torque);
		}

		for (int i = 0; i < leftHinges.size(); i++)
		{
			float torque = jointTorquesLeft[i];
			torque = Clamp(torque, -1000.0f, 1000.0f);

			leftHinges[i]->SetCommandedTorque(torque);
		}

		//add to torque on hips to keep body level.
		//float curLeftHipTrq = leftHinges[0]->GetCommandedTorque();
		//float curRightHipTrq = rightHinges[0]->GetCommandedTorque();

		//float bodyLevelTorque = 0.5f*bodyNode->GetWorldDirection().y_;
		//leftHinges[0]->SetCommandedTorque(curLeftHipTrq + bodyLevelTorque);
		//rightHinges[0]->SetCommandedTorque(curRightHipTrq + bodyLevelTorque);


	}

	void ResizeVectors() override
	{
		actionVec.resize(4);
		actionVec_1 = actionVec;

		FormResponses(0.0f);
		stateVec_1 = stateVec;
	}

	void FormResponses(float timeStep) override
	{
		GYM::FormResponses(timeStep);
	

		int vertState = SetNextState((bodyNode->GetWorldPosition().y_ - 3.2f)*0.5f);//Vertical Translation


		//local linear velocity
		Vector3 vel = bodyNode->GetComponent<NewtonRigidBody>()->GetLinearVelocity(Urho3D::TS_LOCAL);
		SetNextState(vel.x_ );
		SetNextState(vel.y_ );
		SetNextState(vel.z_ );



		int fbtilt = SetNextState(bodyNode->GetWorldRight().y_);    //forward-back tilt indicator
		int rolltilt = SetNextState(bodyNode->GetWorldDirection().y_);//roll indicator



		//SetNextRewardPart(stateVec[vertState]);//vertical displacement
		SetNextRewardPart(1.0f);//vertical displacement


		if (bodyNode->GetWorldPosition().y_ < 1.5f)
		{
			end = 1;
		}

	}

	void ApplyActionVec(float timeStep) override
	{
		GYM::ApplyActionVec(timeStep);

		rightFootLocalOffset.y_ = -verticalFeetOffset + actionVec[0] ;
		rightFootLocalOffset.z_ = actionVec[1] ;

		leftFootLocalOffset.y_ = - verticalFeetOffset + actionVec[2] ;
		leftFootLocalOffset.z_ = actionVec[3] ;

	}

	void DrawDebugGeometry(DebugRenderer* debugRenderer) override
	{
		debugRenderer->AddLine(bodyNode->GetWorldPosition(), bodyNode->GetWorldPosition() + targetWorldVel * 5.0f, Color::RED);
		debugRenderer->AddFrame(Matrix3x4(targetLeftFootPosWorld, Quaternion::IDENTITY, 1.0f));
		debugRenderer->AddFrame(Matrix3x4(targetRightFootPosWorld, Quaternion::IDENTITY, 1.0f));

		if(end )
		{
			debugRenderer->AddBoundingBox(bodyNode->GetComponent<NewtonRigidBody>()->GetModel()->GetBoundingBox(), Matrix3x4::IDENTITY, Color::BLACK, true, false);
		}
	}

	ea::vector<NewtonRevoluteJoint*> motors;

	ea::vector<NewtonRevoluteJoint*> leftHinges;
	ea::vector<NewtonRevoluteJoint*> rightHinges;


	WeakPtr<Node> bodyNode;
	WeakPtr<Node> FOOT_RIGHT2;
	WeakPtr<Node> FOOT_LEFT2;
	WeakPtr<Node> FOOT_RIGHT;
	WeakPtr<Node> FOOT_LEFT;
	WeakPtr<Node> KNEE_LOWER_RIGHT;
	WeakPtr<Node> KNEE_LOWER_LEFT;

	float verticalFeetOffset = 2.4f;

	Vector3 rightFootLocalOffset;
	Vector3 leftFootLocalOffset;

	Vector3 targetRightFootPosWorld;
	Vector3 targetLeftFootPosWorld;

	float footPhaseDeg = 0.0f;

	Vector3 targetWorldVel;
};




