#pragma once

// This include pulls in all engine headers. It is used for simplicity here, but overusing this may slow down your builds. Use with care!
#include <Urho3D/Urho3DAll.h>
#include "Character.h"

// This is probably always OK.
using namespace Urho3D;
class Character;

class URHO3D_EXPORT_API TechGame : public Application
{
	URHO3D_OBJECT(TechGame, Application);
public:
	explicit TechGame(Context* context);
	~TechGame();
	void Setup() override;
	void Start() override;
	void Stop() override;


	void DefaultCreateScene();

	void ResolveTools(Character* character);

	void SetupSceneAfterLoad();


	void SetupViewport();
	void SubscribeToEvents();

	void CreateInstructions()
	{
		auto* cache = GetSubsystem<ResourceCache>();
		auto* ui = GetSubsystem<UI>();

		// Construct new Text object, set string to display and font to use
		instructionText_ = ui->GetRoot()->CreateChild<Text>();
		instructionText_->SetFont(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 15);
		instructionText_->SetTextAlignment(HA_CENTER);

		// Position the text relative to the screen center
		instructionText_->SetHorizontalAlignment(HA_CENTER);
		instructionText_->SetVerticalAlignment(VA_CENTER);
		instructionText_->SetPosition(0, ui->GetRoot()->GetHeight() / 4);
	}



	/// Read input and moves the camera.
	void UpdateUIInput(float timestep);

	/// Handle the logic update event.
	void HandleUpdate(StringHash eventType, VariantMap& eventData);
	
	void HandlePostUpdate(StringHash eventType, VariantMap& eventData);
	/// Handle the post-render update event.
	void HandlePostRenderUpdate(StringHash eventType, VariantMap& eventData);
	
	void HandleNodeCollisionStart(StringHash eventType, VariantMap& eventData);

	void HandleNodeCollisionEnd(StringHash eventType, VariantMap& eventData);


	void CreateCharacter();

protected:
	/// Scene.
	SharedPtr<Scene> scene_;


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
	bool debugDepthTest = true;

	bool drawDebugVisualDebugger = false;
	bool drawDebugHandTools = false;
	bool drawDebugPiecePoints = false;
	bool drawDebugPiecePointRows = false;
	bool drawDebugPieceGears = false;
	bool drawDebugPiecePointsDepthTest = false;

	bool drawDebugPieceGroups = false;

	bool drawDebugHud = false;


	void CreateGameUI();

	//updates all game ui
	void UpdateGameUI();


	WeakPtr<Text> instructionText_;

	BorderImage* crossHairElement_ = nullptr;
	BorderImage* crossHairElementOuter_ = nullptr;
	enum CrossHairMode {
		CrossHairMode_Free = 0,
		CrossHairMode_Busy
	};




private:

};