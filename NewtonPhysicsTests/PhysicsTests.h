//
// Copyright (c) 2008-2019 the Urho3D project.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#pragma once

#include "Samples/Sample.h"
#include "NewtonConstraint.h"
#include <Urho3D/MLControl/Gym.h>
#include <Urho3D/SystemUI/Gizmo.h>


#include "MathExtras_DSP/MathExtras_DSP.h"


namespace Urho3D
{
class Node;
class Scene;
class NewtonRevoluteJoint;









}
/// PhysicsTests example.
/// This sample demonstrates different types of physics configurations and provides a testing ground for physics functionality.
class PhysicsTests : public Application
{
    URHO3D_OBJECT(PhysicsTests, Application);

public:
    /// Construct.
	explicit PhysicsTests(Context* context);

	void Setup() override
	{
		// Engine is not initialized yet. Set up all the parameters now.
		engineParameters_[EP_FULL_SCREEN] = false;
		engineParameters_[EP_WINDOW_HEIGHT] = 1080;
		engineParameters_[EP_WINDOW_WIDTH] = 1920;
		engineParameters_[EP_WINDOW_RESIZABLE] = true;
		// Resource prefix path is a list of semicolon-separated paths which will be checked for containing resource directories. They are relative to application executable file.
		engineParameters_[EP_RESOURCE_PREFIX_PATHS] = ".;./../../../bin";
		engineParameters_[EP_WORKER_THREADS] = 4;
		engineParameters_[EP_RENDER_PATH] = "./RenderPaths/PBRDeferred.xml";
	}


    /// Setup after engine initialization and before running the main loop.
	virtual void Start();
	virtual void Start(const ea::vector<ea::string>& args);

protected:
   

private:
    /// Construct the scene content.
    void CreateScene();


    void CreatePyramids(Vector3 position);


    void CreateTowerOfLiar(Vector3 position);
    /// Construct an instruction text to the UI.
    void CreateUI();
    /// Set up a viewport for displaying the scene.
    void SetupViewport();
    /// Subscribe to application-wide logic update and post-render update events.
    void SubscribeToEvents();





    /// Read input and moves the camera.
    void MoveCamera(float timeStep);





    /// Spawn a physics object from the camera position.
    void SpawnObject();
    void SpawnConvexHull(const Vector3& worldPos);
    void SpawnCompound(const Vector3& worldPos);
    void SpawnDecompCompound(const Vector3& worldPos);    
	void SpawnSceneCompoundTest(const Vector3& worldPos, bool oneBody);
    void SpawnNSquaredJointedObject(Vector3 worldPosition);
    void SpawnGlueJointedObject(Vector3 worldPosition);
    void SpawnLinearJointedObject(float size, Vector3 worldPosition);
    void SpawnMaterialsTest(Vector3 worldPosition);
    void SpawnBallSocketTest(Vector3 worldPosition);
    void SpawnCollisionExceptionsTest(Vector3 worldPosition);
    void SpawnSliderTest(Vector3 worldPosition);
	void SpawnGearTest(Vector3 worldPosition);
    void SpawnRandomObjects();
    void SpawnCompoundedRectTest(Vector3 worldPosition);
    void SpawnCompoundedRectTest2(Vector3 worldPosition);
    void SpawnKinematicBodyTest(Vector3 worldPosition, Quaternion worldRotation);
	void SpawnRejointingTest(Vector3 worldPosition);
	void SpawnCollisionOffsetTest(Vector3 worldPosition);
    void SpawnSegway(Vector3 worldPosition);
    void SpawnUnitsTest(Vector3 worldPosition);

    /// Handle the logic update event.
    void HandleUpdate(StringHash eventType, VariantMap& eventData);
    /// Handle the post-render update event.
    void HandlePostRenderUpdate(StringHash eventType, VariantMap& eventData);
	void EvalOrbitGym();
    /// Handle physics pre-step
    void HandlePhysicsPreStep(StringHash eventType, VariantMap& eventData);

    /// Handle physics post-step
    void HandlePhysicsPostStep(StringHash eventType, VariantMap& eventData);

    /// Flag for drawing debug geometry.
    bool enableEditor_ = false;
    void DecomposePhysicsTree();
    void RecomposePhysicsTree();
    void  TransportNode();



    void HandleMouseButtonUp(StringHash eventType, VariantMap& eventData);
    void HandleMouseButtonDown(StringHash eventType, VariantMap& eventData);


    void HandleCollisionStart(StringHash eventType, VariantMap& eventData);
    void HandleCollision(StringHash eventType, VariantMap& eventData);
    void HandleCollisionEnd(StringHash eventType, VariantMap& eventData);


    Node* pickPullNode = nullptr;
	WeakPtr<Node> noseHoverNode;
    WeakPtr<Node> mouseHoverNode;
    WeakPtr<Node> selectedNode;
    WeakPtr<Node> selectedNode_ManipNodeTarget;
    WeakPtr<Node> selectedNode_ManipNodeChild;
    WeakPtr<Node> noseNode;
    Quaternion pickPullCameraStartOrientation;
    Vector3 pickPullStartPositionWorld;
    Vector3 pickPullEndPositionWorld;
    NewtonRevoluteJoint* hingeActuatorTest = nullptr;

    enum GizmoPhysicsMode {
        GizmoPhysicsMode_KinJoint = 0,
        GizmoPhysicsMode_DirectTransform
    };
    GizmoPhysicsMode gizmoPhysMode_ = GizmoPhysicsMode_DirectTransform;

    enum GizmoSelectionLocationMode {
        GizmoSelectionLocationMode_Surface = 0,
        GizmoSelectionLocationMode_NodeCenter,
        GizmoSelectionLocationMode_COM
    };
    GizmoSelectionLocationMode gizmoSelLocMode_ = GizmoSelectionLocationMode_NodeCenter;


    float timeAccum = 0.0f;

    bool gizmoManip = false;
    bool gizmoManip_1 = false;



	Node* reJointRoot = nullptr;
	Node* reJointA = nullptr;
	Node* reJointB = nullptr;
	void ToggleRejointTest();

   


    void CreatePickTargetNodeOnPhysics();
    void ReleasePickTargetOnPhysics();
    void UpdatePickPull();
    RayQueryResult GetCameraPickNode();
    RayQueryResult GetCameraMousePickNode();
    bool pickPullFreeze = false;

    //temp variable to tracking world position across updates.
    ea::vector<float> worldPosHistory_;
	ea::vector<float> worldPosHistory2_;


	ea::vector<NewtonConstraint*> forceCalculationConstraints_;

    WeakPtr<Node> kinematicNode_;

    SharedPtr<Gizmo> gizmo_;

    void CreateScenery(Vector3 worldPosition);
    void RemovePickNode(bool removeRigidBodyOnly = false);
   
   
	void CreateResetGYMs();
	ea::vector<SharedPtr<GYM>> gyms;


	protected:

		/// Initialize mouse mode on non-web platform.
		void InitMouseMode(MouseMode mode);
		/// Control logo visibility.
		void SetLogoVisible(bool enable);
		///
		void CloseSample();

		/// Logo sprite.
		SharedPtr<Sprite> logoSprite_;
		/// Scene.
		SharedPtr<Scene> scene_;
		/// Camera scene node.
		SharedPtr<Node> cameraNode_;
		/// Camera yaw angle.
		float yaw_;
		/// Camera pitch angle.
		float pitch_;
		/// Mouse mode option to use in the sample.
		MouseMode useMouseMode_;

private:

	/// Set custom window Title & Icon
	void SetWindowTitleAndIcon();
	/// Create console and debug HUD.
	void CreateConsoleAndDebugHud();
	/// Handle request for mouse mode on web platform.
	void HandleMouseModeRequest(StringHash eventType, VariantMap& eventData);
	/// Handle request for mouse mode change on web platform.
	void HandleMouseModeChange(StringHash eventType, VariantMap& eventData);
	/// Handle key down event to process key controls common to all samples.
	void HandleKeyDown(StringHash eventType, VariantMap& eventData);
	/// Handle key up event to process key controls common to all samples.
	void HandleKeyUp(StringHash eventType, VariantMap& eventData);
	/// Handle scene update event to control camera's pitch and yaw for all samples.
	void HandleSceneUpdate(StringHash eventType, VariantMap& eventData);
	/// Handle touch begin event to initialize touch input on desktop platform.
	void HandleTouchBegin(StringHash eventType, VariantMap& eventData);


	/// Pause flag.
	bool paused_;




	bool orbitGYM = true;


};

// A helper macro which defines main function. Forgetting it will result in linker errors complaining about missing `_main` or `_WinMain@16`.
URHO3D_DEFINE_APPLICATION_MAIN(PhysicsTests);