#pragma once

// This include pulls in all engine headers. It is used for simplicity here, but overusing this may slow down your builds. Use with care!
#include <Urho3D/Urho3DAll.h>

// This is probably always OK.
using namespace Urho3D;
class Character;

class URHO3D_EXPORT_API TechGame : public Application
{
	URHO3D_OBJECT(TechGame, Application);
public:
	explicit TechGame(Context* context);
	void Setup() override;
	void Start() override;
	void Stop() override;


	void CreateScene();

	void CreateCamera();

	void SetupViewport();
	void SubscribeToEvents();

	/// Read input and moves the camera.
	void UpdateUIInput(float timestep);

	/// Handle the logic update event.
	void HandleUpdate(StringHash eventType, VariantMap& eventData);
	
	void HandlePostUpdate(StringHash eventType, VariantMap& eventData);
	/// Handle the post-render update event.
	void HandlePostRenderUpdate(StringHash eventType, VariantMap& eventData);
	
	void HandleNodeCollisionStart(StringHash eventType, VariantMap& eventData);

	void HandleNodeCollisionEnd(StringHash eventType, VariantMap& eventData);


	void SpawnObject();
	void CreateCharacter();

	Node* CreatePiece(Node* parent, ea::string name, bool loadExisting);

protected:
	/// Scene.
	SharedPtr<Scene> scene_;
	/// Camera scene node.
	SharedPtr<Node> cameraNode_;
	/// The controllable character component.
	WeakPtr<Character> character_;

	/// Camera yaw angle.
	float yaw_ = 0.0f;
	/// Camera pitch angle.
	float pitch_ = 0.0f;
	
	bool crashSoundPlaying = false;
	
	/// Mouse mode option to use 
	MouseMode useMouseMode_;

	/// Create console and debug HUD.
	void CreateConsoleAndDebugHud();



	bool drawDebug_ = false;
	bool drawDebugPhysicsWorld = false;
	bool drawDebugPhysicsWorldConstraints = true;
	bool drawDebugPhysicsWorldContacts = true;
	bool drawDebugPhysicsWorldRigidBodies = true;
	bool drawDebugPhysicsWorldDepthTest = true;

	bool drawDebugVisualDebugger = false;
	bool drawDebugPiecePoints = false;
	bool drawDebugPiecePointRows = false;
	bool drawDebugPieceGears = false;
	bool drawDebugPiecePointsDepthTest = false;

	bool drawDebugPieceGroups = false;

	bool drawDebugHud = false;


	void CreateGameUI();

	//updates all game ui
	void UpdateGameUI();

	BorderImage* crossHairElement_ = nullptr;
	BorderImage* crossHairElementOuter_ = nullptr;
	enum CrossHairMode {
		CrossHairMode_Free = 0,
		CrossHairMode_Busy
	};




private:

};