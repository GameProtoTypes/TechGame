#pragma once

#include "NewtonModel.h"
#include "Urho3D/MLControl/Gym.h"


using namespace Urho3D;

class GYM_RobotArm : public GYM
{
public:
    explicit GYM_RobotArm(Context* context) : GYM(context)
    {

    }
	virtual void Reset()
	{
		GYM::Reset();

        redBox = SpawnSamplePhysicsBox(scene_, worldPos + Vector3(5, 5, 0), Vector3(0.2f, 0.2f, 0.2f), Color::RED);


        robotRoot = scene_->CreateChild("RobotArm");
        rootNode = robotRoot;
        orbitNode = robotRoot->CreateChild("Orbit");
        Node* root = robotRoot;

        Node* base = SpawnSamplePhysicsCylinder(root, Vector3::ZERO, 2, 0.5);
        base->GetComponent<NewtonCollisionShape_Cylinder>()->SetDensity(100);
        robotBaseBody = base->GetComponent<NewtonRigidBody>();
        base->SetName("RobotBase");

        Node* base2 = SpawnSamplePhysicsCylinder(root, Vector3(0, 1, 0), 1, 1);
        NewtonRevoluteJoint* motor1 = base->CreateComponent<NewtonRevoluteJoint>();
        motor1->SetOtherBody(base2->GetComponent<NewtonRigidBody>());
        motor1->SetRotation(Quaternion(90, Vector3(0, 0, 1)));
        motor1->SetEnableHingeLimits(false);
        robotHinges[0] = motor1;

        Node* arm1 = SpawnSamplePhysicsBox(root, Vector3(0, 2, 0), Vector3(0.5, 3, 0.5));
        NewtonRevoluteJoint* motor2 = base2->CreateComponent<NewtonRevoluteJoint>();
        motor2->SetOtherBody(arm1->GetComponent<NewtonRigidBody>());
        robotHinges[1] = motor2;

        Node* arm2 = SpawnSamplePhysicsBox(root, Vector3(0, 4, 0), Vector3(0.5, 3, 0.5));
        NewtonRevoluteJoint* motor3 = arm1->CreateComponent<NewtonRevoluteJoint>();
        motor3->SetOtherBody(arm2->GetComponent<NewtonRigidBody>());
        motor3->SetPosition(Vector3(0, 1, 0));
        robotHinges[2] = motor3;


        Node* arm3 = SpawnSamplePhysicsBox(root, Vector3(0, 6, 0), Vector3(0.5, 3, 0.5));
        NewtonRevoluteJoint* motor4 = arm2->CreateComponent<NewtonRevoluteJoint>();
        motor4->SetOtherBody(arm3->GetComponent<NewtonRigidBody>());
        motor4->SetPosition(Vector3(0, 1, 0));
        robotHinges[3] = motor4;

        Node* wrist = SpawnSamplePhysicsCylinder(root, Vector3(0, 7.5, 0), 0.5, 0.5);
        NewtonRevoluteJoint* motor5 = arm3->CreateComponent<NewtonRevoluteJoint>();
        motor5->SetOtherBody(wrist->GetComponent<NewtonRigidBody>());
        motor5->SetPosition(Vector3(0, 1, 0));
        motor5->SetRotation(Quaternion(90, Vector3(0, 0, 1)));
        robotHinges[4] = motor5;


        Node* arm4 = SpawnSamplePhysicsBox(root, Vector3(0, 8.5, 0), Vector3(0.5, 1.5, 0.5));
        NewtonRevoluteJoint* motor6 = wrist->CreateComponent<NewtonRevoluteJoint>();
        motor6->SetOtherBody(arm4->GetComponent<NewtonRigidBody>());
        motor6->SetPosition(Vector3(0, 0, 0));
        robotHinges[5] = motor6;

        robotEndBody = arm4->GetComponent<NewtonRigidBody>();


        scene_->GetComponent<NewtonPhysicsWorld>()->BuildAndUpdateNewtonModels();


        for (int i = 0; i < 6; i++)
        {
            robotHinges[i]->SetFrictionCoef(1.0f);
            robotHinges[i]->SetEnableHingeLimits(false);
            //robotHinges[i]->GetOtherBody()->SetNoCollideOverride(true);
        }

        robotPIDControllers[0].PGain = 20000.0f;
        robotPIDControllers[1].PGain = 20000.0f;
        robotPIDControllers[2].PGain = 20000.0f;
        robotPIDControllers[3].PGain = 10000.0f;
        robotPIDControllers[4].PGain = 500.0f;
        robotPIDControllers[5].PGain = 200.0f;


        root->SetWorldPosition(worldPos);
	}


    virtual void ApplyActionVec(float timeStep)
    {
        GYM::ApplyActionVec(timeStep);

        UpdateRobotArm(timeStep);

    }


    WeakPtr<Node> robotRoot;
    WeakPtr<NewtonRigidBody> robotBaseBody;
    WeakPtr<NewtonRigidBody> robotEndBody;
    WeakPtr<NewtonRevoluteJoint> robotHinges[6];//base to end
    MathExtras::PID<float> robotPIDControllers[6];
    MathExtras::SinglePoleLPFilter<float> robotControlFilters[6];
    float robotJointSpeedTargets[6];

    WeakPtr<Node> endEffectorTarget;
    ndModel newtonRobotArmModel;
    WeakPtr<Node> redBox;

    Vector3 tipManipV1, tipManipV2, tipManipV3;
    Vector3 tipManipSize;


    void UpdateRobotArm(float timestep)
    {

        //compute the Jacobian From end to base
        Matrix3x4 rootTransform = robotHinges[0]->GetOwnWorldFrame();
        Vector3 rootWorldVel = robotHinges[0]->GetOwnWorldFrameVel();
        Vector3 rootWorldOmega = robotHinges[0]->GetOwnWorldFrameOmega();
        Matrix3x4 endEffectorWorld = robotHinges[5]->GetOwnWorldFrame();
        Matrix3x4 endEffectorRelRoot = rootTransform.Inverse() * endEffectorWorld;
        Vector3 hingeLocalRotationAxis = NewtonRevoluteJoint::LocalHingeAxis();
        Vector3 d_n_0 = endEffectorRelRoot.Translation();


        ea::vector<NewtonRevoluteJoint*> constraintChain;
        for (int i = 0; i < 6; i++)
            constraintChain.push_back(robotHinges[i]);

        ChainJacobian J_;
        robotHinges[0]->GetModel()->CalculateChainJabobian(constraintChain, J_);
        



        //compute manupulability elipse
        robotHinges[0]->GetModel()->ComputeEndEffectorManipulabilityElipse(J_, tipManipV1, tipManipV2, tipManipV3, tipManipSize);
        //rotate tipManip vectors to world space.
        tipManipV1 = rootTransform.RotationMatrix() * tipManipV1;
        tipManipV2 = rootTransform.RotationMatrix() * tipManipV2;
        tipManipV3 = rootTransform.RotationMatrix() * tipManipV3;

        static Vector3 targetVel;
        Vector3 targetPos = (rootTransform.Inverse() * redBox->GetWorldTransform()).Translation();
        Vector3 delta = (targetPos - endEffectorRelRoot.Translation());


        static float gain = 300.0f;
        static float damping = 100.0f;
        ui::Begin("Gains and Damping");
        ui::SliderFloat("Gain", &gain, 1.0f, 1000.0f);
        ui::SliderFloat("Damping", &damping, 0.0f, 100.0f);
        ui::End();

        Vector3 endForceRobotSpace = gain * delta - damping * (rootTransform.RotationMatrix().Inverse() * robotHinges[5]->GetOwnWorldFrameVel());
        Vector3 endTorqueRobotSpace = Vector3::ZERO;
 
        ea::vector<float> jointTorquesAntiGravity;
        ea::vector<float> jointTorquesControl;
        robotHinges[0]->GetModel()->ComputeJointTorquesForEndEffector(J_, constraintChain, endForceRobotSpace, endTorqueRobotSpace, jointTorquesControl);
        robotHinges[0]->GetModel()->ComputeCounterGravitationalTorque(constraintChain, jointTorquesAntiGravity);


        ui::Begin("Torques");



        for (int i = 0; i < 6; i++)
        {
            float torque = jointTorquesAntiGravity[i] + jointTorquesControl[i];
            torque = Clamp(torque, -1000.0f, 1000.0f);
            robotHinges[i]->SetCommandedTorque(torque);
            //robotHinges[i]->SetFrictionCoef(100.0f);    


            ui::Text("AntiGravity: %f,    Control: %f", jointTorquesAntiGravity[i], jointTorquesControl[i]);

        }
        ui::End();
    }

    virtual void DrawDebugGeometry(DebugRenderer* debugRenderer)
    {
        Vector3 tipEnd = robotHinges[5]->GetOwnWorldFrame().Translation();



        debugRenderer->AddLine(tipEnd, tipEnd + tipManipV1  * tipManipSize.x_ * 0.5f, Color::RED);
        debugRenderer->AddLine(tipEnd, tipEnd + tipManipV2  * tipManipSize.y_ * 0.5f, Color::GREEN);
        debugRenderer->AddLine(tipEnd, tipEnd + tipManipV3  * tipManipSize.z_ * 0.5f, Color::BLUE);

        debugRenderer->AddLine(tipEnd, tipEnd - tipManipV1 * tipManipSize.x_ * 0.5f, Color::RED);
        debugRenderer->AddLine(tipEnd, tipEnd - tipManipV2 * tipManipSize.y_ * 0.5f, Color::GREEN);
        debugRenderer->AddLine(tipEnd, tipEnd - tipManipV3 * tipManipSize.z_ * 0.5f, Color::BLUE);

        debugRenderer->AddElipse(tipEnd, tipManipV1, tipManipV2, tipManipSize.y_ * 0.5f, tipManipSize.z_ * 0.5f, Color::WHITE);//yz
        debugRenderer->AddElipse(tipEnd, tipManipV2, tipManipV3, tipManipSize.z_ * 0.5f, tipManipSize.x_ * 0.5f, Color::WHITE);//yz

      }
};