#include "TechGame.h"
#include "Piece.h"
#include "Character.h"
#include "ManipulationTool.h"
#include "PieceManager.h"
#include "PieceAttachmentStager.h"
#include "PiecePointRow.h"

#include "MathExtras.h"

#include "NewtonPhysicsWorld.h"
#include "NewtonRigidBody.h"
#include "NewtonCollisionShape.h"
#include "NewtonCollisionShapesDerived.h"
#include "Newton6DOFConstraint.h"
#include "NewtonPhysicsEvents.h"

#include "VisualDebugger.h"
#include "Urho3D/SystemUI/Console.h"
#include "Urho3D/SystemUI/DebugHud.h"
#include "PieceGear.h"
#include "VR.h"

void TechGame::Setup()
{
	// Engine is not initialized yet. Set up all the parameters now.
	engineParameters_[EP_FULL_SCREEN] = false;
	float scale = 0.8;
	engineParameters_[EP_WINDOW_WIDTH] = int(1920* scale);
	engineParameters_[EP_WINDOW_HEIGHT] = int(1080* scale);
	
	// Resource prefix path is a list of semicolon-separated paths which will be checked for containing resource directories. They are relative to application executable file.
#if _WIN32
	engineParameters_[EP_RESOURCE_PREFIX_PATHS] = "../../../bin";
#else
	engineParameters_[EP_RESOURCE_PREFIX_PATHS] = ".";
#endif
	

	Character::RegisterObject(context_);
	ManipulationTool::RegisterObject(context_);

	PieceManager::RegisterObject(context_);
	PieceAttachmentStager::RegisterObject(context_);
	Piece::RegisterObject(context_);
	PieceSolidificationGroup::RegisterObject(context_);
	PiecePoint::RegisterObject(context_);
	PiecePointRow::RegisterObject(context_);
	PieceGear::RegisterObject(context_);



	RegisterNewtonPhysicsLibrary(context_);
}

void TechGame::Start()
{

	GetSubsystem<Engine>()->SetMaxFps(1000);
	GetSubsystem<Engine>()->SetMinFps(90);

	// Create the scene content
	CreateScene();

	

	CreateGameUI();

	CreateConsoleAndDebugHud();

	// Setup the viewport for displaying the scene
	SetupViewport();

	// Hook up to the frame update and render post-update events
	SubscribeToEvents();

	// Set the mouse mode to use in the sample
	GetSubsystem<Input>()->SetMouseMode(MM_RELATIVE);


}

void TechGame::Stop()
{
	// This step is executed when application is closing. No more frames will be rendered after this method is invoked.
}

TechGame::TechGame(Context* context) : Application(context)
{

}



void TechGame::CreateCharacter()
{
	auto* cache = GetSubsystem<ResourceCache>();

	Node* playerNode = scene_->CreateChild("player");
	playerNode->SetPosition(Vector3(0.0f, 0.0f, 0.0f));


	character_ = playerNode->CreateComponent<Character>();

}


void TechGame::CreateConsoleAndDebugHud()
{
	// Create console
	Console* console = engine_->CreateConsole();

	// Create debug HUD.
	DebugHud* debugHud = engine_->CreateDebugHud();

}

void TechGame::CreateGameUI()
{
	UI* ui = GetSubsystem<UI>();
	Graphics* graphics = GetSubsystem<Graphics>();

	crossHairElement_ = new BorderImage(context_);
	crossHairElement_->SetTexture(GetSubsystem<ResourceCache>()->GetResource<Texture2D>("Textures/UI_Crosshairs.png")); // Set texture
	
	crossHairElement_->SetBlendMode(BLEND_ADD);
	crossHairElement_->SetSize(64, 64);
	crossHairElement_->SetImageRect(IntRect(128, 0, 64, 64));
	crossHairElement_->SetPosition((graphics->GetWidth() - crossHairElement_->GetWidth()) / 2, (graphics->GetHeight() - crossHairElement_->GetHeight()) / 2);
	crossHairElement_->SetName("crossHair");
	crossHairElement_->SetVar("curMode", CrossHairMode_Free);
    ui->GetRoot()->AddChild(crossHairElement_);



	crossHairElementOuter_ = new BorderImage(context_);
	crossHairElementOuter_->SetTexture(GetSubsystem<ResourceCache>()->GetResource<Texture2D>("Textures/UI_Crosshairs.png")); // Set texture
	crossHairElementOuter_->SetBlendMode(BLEND_ADD);
	crossHairElementOuter_->SetSize(64, 64);
	crossHairElementOuter_->SetImageRect(IntRect(0, 3* 64, 64, 64));
	crossHairElementOuter_->SetPosition((graphics->GetWidth() - crossHairElementOuter_->GetWidth()) / 2, (graphics->GetHeight() - crossHairElementOuter_->GetHeight()) / 2);
	crossHairElementOuter_->SetName("crossHair");
	crossHairElementOuter_->SetVar("curMode", CrossHairMode_Free);
	ui->GetRoot()->AddChild(crossHairElementOuter_);



}

void TechGame::UpdateGameUI()
{
	//update main crosshair
	CrossHairMode curCrossHairMode = (CrossHairMode)crossHairElement_->GetVar("curMode").GetInt();
	int xOffset = 0;
	if (curCrossHairMode == CrossHairMode_Free)
	{
		xOffset = 128;
	}
	else if (curCrossHairMode == CrossHairMode_Busy)
	{
		xOffset = 128 + 64;
	}
	crossHairElement_->SetImageRect(IntRect(xOffset, 0, 64, 64));

	//update outer crosshair
	CrossHairMode curCrossHairModeOuter = (CrossHairMode)crossHairElementOuter_->GetVar("curMode").GetInt();
	xOffset = 0;
	if (curCrossHairModeOuter == CrossHairMode_Free)
	{
		crossHairElementOuter_->SetVisible(false);
	}
	else if (curCrossHairModeOuter == CrossHairMode_Busy)
	{
		crossHairElementOuter_->SetVisible(true);
	}
	crossHairElementOuter_->SetImageRect(IntRect(xOffset, 3 * 64, 64, 64));



}

void TechGame::SubscribeToEvents()
{
	// Subscribe HandleUpdate() function for processing update events
	SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(TechGame, HandleUpdate));

	SubscribeToEvent(E_POSTUPDATE, URHO3D_HANDLER(TechGame, HandlePostUpdate));

	// Subscribe HandlePostRenderUpdate() function for processing the post-render update event, during which we request
	// debug geometry
	SubscribeToEvent(E_POSTRENDERUPDATE, URHO3D_HANDLER(TechGame, HandlePostRenderUpdate));


	SubscribeToEvent(E_NEWTON_NODECOLLISIONSTART, URHO3D_HANDLER(TechGame, HandleNodeCollisionStart));
	SubscribeToEvent(E_NEWTON_NODECOLLISIONEND, URHO3D_HANDLER(TechGame, HandleNodeCollisionEnd));



}

void TechGame::SetupViewport()
{
	auto* renderer = GetSubsystem<Renderer>();
	XMLFile* file = GetSubsystem<ResourceCache>()->GetResource<XMLFile>("RenderPaths/Deferred.xml");
	SharedPtr<RenderPath> renderPath = SharedPtr<RenderPath>(new RenderPath());
	renderPath->Load(file);
	renderer->SetDefaultRenderPath(renderPath);

	// Set up a viewport to the Renderer subsystem so that the 3D scene can be seen
	SharedPtr<Viewport> viewport(new Viewport(context_, scene_, character_->headNode_->GetComponent<Camera>()));
	renderer->SetViewport(0, viewport);
}






void TechGame::UpdateUIInput(float timestep)
{
	
	Input* input = GetSubsystem<Input>();

	if (GetSubsystem<Input>()->GetKeyPress(KEY_TAB))
	{
		URHO3D_LOGINFO("TAB PRESSED");
		GetSubsystem<Input>()->SetMouseGrabbed(!GetSubsystem<Input>()->IsMouseGrabbed());
		GetSubsystem<Input>()->SetMouseVisible(!GetSubsystem<Input>()->IsMouseVisible());
		GetSubsystem<Input>()->SetMouseMode(MM_ABSOLUTE);
	}
	if (GetSubsystem<Input>()->GetKeyPress(KEY_F1)) {
		drawDebug_ = !drawDebug_;
	}


	if (!input->IsMouseGrabbed() || input->IsMouseVisible())
		return;


	if (input->GetMouseButtonPress(Urho3D::MOUSEB_LEFT)) {

		if (character_->rightHandNode_->GetComponent<ManipulationTool>()->IsGathering())
		{
			character_->rightHandNode_->GetComponent<ManipulationTool>()->UnGather(input->GetKeyDown(KEY_SHIFT));
			crossHairElement_->SetVar("curMode", (int)CrossHairMode::CrossHairMode_Free);
		}
		else
		{
			bool gatherSuccess = character_->rightHandNode_->GetComponent<ManipulationTool>()->Gather(input->GetKeyDown(KEY_SHIFT));
			
			if(gatherSuccess)
				crossHairElement_->SetVar("curMode", (int)CrossHairMode::CrossHairMode_Busy);
		}
	}

	if (input->GetMouseButtonPress(Urho3D::MOUSEB_RIGHT))
	{
		character_->rightHandNode_->GetComponent<ManipulationTool>()->AimPointForce();
	}

	if (input->GetKeyPress(KEY_R)) {
		character_->rightHandNode_->GetComponent<ManipulationTool>()->ResetGatherNodeRotation();
	}

	if (input->GetKeyPress(KEY_M)) {
		//toggle move modes
		ManipulationTool::MoveMode curMode = character_->rightHandNode_->GetComponent<ManipulationTool>()->GetMoveMode();
		if (curMode == ManipulationTool::MoveMode_Camera)
			curMode = ManipulationTool::MoveMode_Global;
		else if (curMode == ManipulationTool::MoveMode_Global)
			curMode = ManipulationTool::MoveMode_Camera;

		character_->rightHandNode_->GetComponent<ManipulationTool>()->SetMoveMode(curMode);
	}


	if (input->GetKeyDown(KEY_SHIFT))
	{
		crossHairElementOuter_->SetVar("curMode", (int)CrossHairMode::CrossHairMode_Busy);
	}
	else
	{
		crossHairElementOuter_->SetVar("curMode", (int)CrossHairMode::CrossHairMode_Free);
	}

	if (input->GetKeyPress(KEY_G))
	{
		character_->rightHandNode_->GetComponent<ManipulationTool>()->ToggleUseGrid();
	}


	int mouseWheelMove = input->GetMouseMoveWheel();
	if (mouseWheelMove > 0)
		character_->rightHandNode_->GetComponent<ManipulationTool>()->AdvanceGatherPoint(true);
	if(mouseWheelMove < 0)
		character_->rightHandNode_->GetComponent<ManipulationTool>()->AdvanceGatherPoint(false);



	Vector3 rotationAxis;
	if (!input->GetKeyDown(KEY_SHIFT))
		rotationAxis = Vector3(0, 0, 1);
	else
		rotationAxis = Vector3(0, 1, 0);

	if (input->GetKeyPress(KEY_E))
		character_->rightHandNode_->GetComponent<ManipulationTool>()->RotateGatherNode(Quaternion(-45, rotationAxis));
	if (input->GetKeyPress(KEY_Q))
		character_->rightHandNode_->GetComponent<ManipulationTool>()->RotateGatherNode(Quaternion(45, rotationAxis));
	
	
	if (input->GetKeyPress(KEY_TAB))
		character_->rightHandNode_->GetComponent<ManipulationTool>()->RotateNextNearest();







}

void TechGame::CreateScene()
{
	auto* cache = GetSubsystem<ResourceCache>();

	scene_ = new Scene(context_);	
	scene_->CreateComponent<DebugRenderer>();
	scene_->CreateComponent<PieceManager>();

	scene_->CreateComponent<Octree>();
	NewtonPhysicsWorld* physicsWorld = scene_->CreateComponent<NewtonPhysicsWorld>();	
	physicsWorld->SetGravity(Vector3(0, -9.81, 0));
	
	

	context_->RegisterSubsystem<VisualDebugger>();


	CreateCharacter();

	

	VR::RegisterObject(context_);

	VR* vr = context_->GetSubsystem<VR>();


	
	bool vrInitialized = vr->InitializeVR(character_->GetNode());








	ManipulationTool* manipTool = character_->rightHandNode_->CreateComponent<ManipulationTool>();
	
	if (vrInitialized) {
		manipTool->SetMoveMode(ManipulationTool::MoveMode_VR);
	}




	Node* zoneNode = scene_->CreateChild("Zone");
	auto* zone = zoneNode->CreateComponent<Zone>();
	zone->SetBoundingBox(BoundingBox(-1000.0f, 1000.0f));
	zone->SetAmbientColor(Color(0.15f, 0.15f, 0.15f));
	zone->SetFogColor(Color(1.0f, 1.0f, 1.0f));
	zone->SetFogStart(300.0f);
	zone->SetFogEnd(500.0f);


	GetSubsystem<Renderer>()->SetShadowMapSize(2048);

	Node* lightNode = scene_->CreateChild("DirectionalLight");
	lightNode->SetDirection(Vector3(0.6f, -1.0f, 0.8f));
	auto* light = lightNode->CreateComponent<Light>();
	light->SetLightType(LIGHT_DIRECTIONAL);
	light->SetCastShadows(true);
	light->SetShadowBias(BiasParameters(0.00025f, 0.5f));
	// Set cascade splits at 10, 50 and 200 world units, fade shadows out at 80% of maximum shadow distance
	light->SetShadowCascade(CascadeParameters(10.0f, 50.0f, 200.0f, 0.0f, 0.8f));

	// Create skybox. The Skybox component is used like StaticModel, but it will be always located at the camera, giving the
	// illusion of the box planes being far away. Use just the ordinary Box model and a suitable material, whose shader will
	// generate the necessary 3D texture coordinates for cube mapping
	Node* skyNode = scene_->CreateChild("Sky");
	skyNode->SetScale(500.0f); // The scale actually does not matter
	auto* skybox = skyNode->CreateComponent<Skybox>();
	skybox->SetModel(cache->GetResource<Model>("Models/Box.mdl"));
	skybox->SetMaterial(cache->GetResource<Material>("Materials/Skybox.xml"));





	//for (int x = -50; x <= 50; x++)
	//{
	//	for (int z = -50; z <= 50; z++)
	//	{
			Node* floorPiece = scene_->CreateChild();
			StaticModel* stmdl = floorPiece->CreateComponent<StaticModel>();
			stmdl->SetModel(cache->GetResource<Model>("Models/Box.mdl"));
			floorPiece->Rotate(Quaternion(90, Vector3(1, 0, 0)));
			//float scaleFactor = 0.025 / 0.1;


			floorPiece->SetWorldPosition(Vector3(0 , -0.5, 0 ));
			floorPiece->SetScale(Vector3(100, 100, 1.0));


			Material* mat = GetSubsystem<ResourceCache>()->GetResource<Material>("Materials/Piece2.xml");
			SharedPtr<Material> clonedMat = mat->Clone();
			//clonedMat->SetShaderParameter("MatDiffColor", Vector4(0.3f + Random() / 8, 0.3f + Random() / 8, 0.3f + Random() / 8, 0.0f));
			clonedMat->SetShaderParameter("UOffset", Vector4(100.0f, 0.0f, 1.0f, 1.0f));
			clonedMat->SetShaderParameter("VOffset", Vector4(0.0f, 100.0f, 1.0f, 1.0f));
			stmdl->SetMaterial(clonedMat);

		//}
	//}
	// Create a floor object, 1000 x 1000 world units. Adjust position so that the ground is at zero Y
	Node* floorNode = scene_->CreateChild("Floor");
	floorNode->SetPosition(Vector3(0.0f, -5, 0.0f));
	floorNode->SetScale(Vector3(1000.0f, 10.0f, 1000.0f));
	//auto* floorObject = floorNode->CreateComponent<StaticModel>();
	//floorObject->SetModel(cache->GetResource<Model>("Models/Box.mdl"));
	//floorObject->SetMaterial(cache->GetResource<Material>("Materials/StoneTiled.xml"));

	// Make the floor physical by adding NewtonRigidBody and CollisionShape components. The NewtonRigidBody's default
	// parameters make the object static (zero mass.) Note that a CollisionShape by itself will not participate
	// in the physics simulation
	NewtonRigidBody* body = floorNode->CreateComponent<NewtonRigidBody>();
	body->SetMassScale(0.0f);

	// Set a box shape of size 1 x 1 x 1 for collision. The shape will be scaled with the scene node scale, so the
	// rendering and physics representation sizes should match (the box model is also 1 x 1 x 1.)
	auto* shape = floorNode->CreateComponent<NewtonCollisionShape_Box>();

	{
		CreatePiece(scene_, "gear_medium", false)->SetWorldPosition(Vector3(-1,0,0));
		CreatePiece(scene_, "8_piece_Cshape", false)->SetWorldPosition(Vector3(1, 0, 0));

		Node* prevPiece = nullptr;

		ea::vector<Piece*> pieces;
		int numDiffPieces = 19;
		for (int y = 0; y < numDiffPieces*20; y += 1) {

			Node* pieceNode;

			int rnd = y % numDiffPieces;// Random(0, numDiffPieces);
			if (rnd == 0)
				pieceNode = CreatePiece(scene_, "8_piece_Cshape", false);
			else if (rnd == 1)
				pieceNode = CreatePiece(scene_, "rod_hard_4", false);
			else if (rnd == 2)
				pieceNode = CreatePiece(scene_, "rod_round_4", false);
			else if (rnd == 3)
				pieceNode = CreatePiece(scene_, "6_piece_thin", false);
			else if (rnd == 4)
				pieceNode = CreatePiece(scene_, "2_sleeve", false);
			else if (rnd == 5)
				pieceNode = CreatePiece(scene_, "1_cap_small", false);
			else if (rnd == 6)
				pieceNode = CreatePiece(scene_, "rod_round_no_caps_4", false);
			else if (rnd == 7)
				pieceNode = CreatePiece(scene_, "6_piece_thick", false);
			else if (rnd == 8)
				pieceNode = CreatePiece(scene_, "5_piece_thick", false);
			else if (rnd == 9)
				pieceNode = CreatePiece(scene_, "4_piece_thick", false);
			else if (rnd == 10)
				pieceNode = CreatePiece(scene_, "3_piece_thick", false);
			else if (rnd == 11)
				pieceNode = CreatePiece(scene_, "2_piece_thick", false);
			else if (rnd == 12)
				pieceNode = CreatePiece(scene_, "4x4_piece_thin", false);
			else if (rnd == 13)
				pieceNode = CreatePiece(scene_, "4x8_piece_thin", false);
			else if (rnd == 14)
				pieceNode = CreatePiece(scene_, "gear_large", false);
			else if (rnd == 15)
				pieceNode = CreatePiece(scene_, "gear_medium", false);
			else if (rnd == 16)
				pieceNode = CreatePiece(scene_, "gear_small", false);
			else if (rnd == 17)
				pieceNode = CreatePiece(scene_, "gear_extra_large", false);
			else if (rnd == 18)
				pieceNode = CreatePiece(scene_, "rod_round_1", false);



			pieceNode->SetWorldPosition(Vector3(Random(-10,10), y * .05, Random(-10,10)));

			Color color;
			float colorBaseHue = Color::BLUE.Hue();
			float colorRange = 1.0f;
			color.FromHSL(Wrap<float>(colorBaseHue + Random(-0.5f ,0.5f)*colorRange, 0.0f, 1.0f), 0.3f, 0.5f + Random(-0.4f, 0.0f));
			


			pieceNode->GetComponent<Piece>()->SetPrimaryColor(color);



			ea::vector<Piece*> singlePiece;
			singlePiece.push_back(pieceNode->GetComponent<Piece>());



			pieces.push_back(pieceNode->GetComponent<Piece>());


			prevPiece = pieceNode;
		}

		//PieceSolidificationGroup* group = scene_->GetComponent<PieceManager>()->AddPiecesToNewSolidGroup(pieces);
		//group->PushSolidState(false);

	}





}



void TechGame::HandleUpdate(StringHash eventType, VariantMap& eventData)
{
	using namespace Update;

	// Take the frame time step, which is stored as a float
	float timeStep = eventData[P_TIMESTEP].GetFloat();

	// Move the camera, scale movement with time step
	//MoveCamera(timeStep);

	UpdateUIInput(timeStep);

	UpdateGameUI();

}





void TechGame::HandlePostUpdate(StringHash eventType, VariantMap& eventData)
{


}

void TechGame::HandlePostRenderUpdate(StringHash eventType, VariantMap& eventData)
{
	// If draw debug mode is enabled, draw physics debug geometry. Use depth test to make the result easier to interpret
	if (drawDebug_) {


		ui::Begin("Debug Drawing");

		ui::Checkbox("DepthTest", &debugDepthTest);

		ui::Checkbox("PhysicsWorld", &drawDebugPhysicsWorld);

		if (drawDebugPhysicsWorld) {

			ui::BeginGroup();
			ui::Indent(10.0f);
			ui::Checkbox("Constraints", &drawDebugPhysicsWorldConstraints);
			ui::Checkbox("Contacts", &drawDebugPhysicsWorldContacts);
			ui::Checkbox("RigidBodies", &drawDebugPhysicsWorldRigidBodies);
			ui::EndGroup();

			scene_->GetComponent<NewtonPhysicsWorld>()->DrawDebugGeometry(scene_->GetComponent<DebugRenderer>(),
				drawDebugPhysicsWorldConstraints, drawDebugPhysicsWorldContacts, drawDebugPhysicsWorldRigidBodies, debugDepthTest);


		}

		ui::Checkbox("VisualDebugger", &drawDebugVisualDebugger);
		if (drawDebugVisualDebugger)
			GetSubsystem<VisualDebugger>()->DrawDebugGeometry(scene_->GetComponent<DebugRenderer>());


		ui::Checkbox("Hand Tools", &drawDebugHandTools);
		if (drawDebugHandTools)
		{
			ea::vector<Tool*> components;
			scene_->GetDerivedComponents<Tool>(components, true);

			for (Tool* comp : components)
			{
				comp->DrawDebugGeometry(scene_->GetComponent<DebugRenderer>(), debugDepthTest);
			}


		}



		ui::Checkbox("PiecePoints", &drawDebugPiecePoints);
		if (drawDebugPiecePoints) {

			ui::BeginGroup();
			ui::Indent(10.0f);
			ui::Checkbox("DepthTest", &debugDepthTest);
			


			ea::vector<PiecePoint*> components;
			scene_->GetDerivedComponents<PiecePoint>(components, true);

			for (PiecePoint* comp : components)
			{
				comp->DrawDebugGeometry(scene_->GetComponent<DebugRenderer>(), debugDepthTest);
			}


			ui::Checkbox("PiecePointRows", &drawDebugPiecePointRows);
			if (drawDebugPiecePointRows) {

				ea::vector<PiecePointRow*> components;
				scene_->GetDerivedComponents<PiecePointRow>(components, true);

				for (PiecePointRow* comp : components)
				{
					comp->DrawDebugGeometry(scene_->GetComponent<DebugRenderer>(), debugDepthTest);
				}
			}
			ui::EndGroup();

		}


		ui::Checkbox("PieceGears", &drawDebugPieceGears);
		if (drawDebugPieceGears)
		{

			ea::vector<PieceGear*> components;
			scene_->GetDerivedComponents<PieceGear>(components, true);

			for (PieceGear* comp : components)
			{
				comp->DrawDebugGeometry(scene_->GetComponent<DebugRenderer>(), debugDepthTest);
			}


		}





		ui::Checkbox("PieceGroups", &drawDebugPieceGroups);
		if (drawDebugPieceGroups) {
			ea::vector<PieceSolidificationGroup*> groups;
			scene_->GetDerivedComponents<PieceSolidificationGroup>(groups, true);

			for (PieceSolidificationGroup* comp : groups)
			{
				comp->DrawDebugGeometry(scene_->GetComponent<DebugRenderer>(), false);
			}
		}

		ui::End();




		ui::Begin("Utils");

		if (ui::Button("Save Scene..."))
		{
			ea::string filePath = "sceneSave.xml";

			GetSubsystem<FileSystem>()->Delete(filePath);

			SharedPtr<File> outFile = SharedPtr<File>(new File(context_, "sceneSave.xml", Urho3D::FILE_WRITE));
			
			bool saveSuccess = scene_->SaveXML(*outFile);
		}
		ui::Checkbox("DebugHud", &drawDebugHud);
		if (drawDebugHud)
		{
			GetSubsystem<DebugHud>()->SetMode(DebugHudMode::DEBUGHUD_SHOW_ALL);
		}
		else
		{
			GetSubsystem<DebugHud>()->SetMode(DebugHudMode::DEBUGHUD_SHOW_NONE);
		}


		if (ui::Button("Form PieceSolidificationGroup"))
		{
			Vector3 worldPos;
			Piece* piece = scene_->GetComponent<PieceManager>()->GetClosestAimPiece(worldPos, character_->headNode_);
			if (piece) {

				URHO3D_LOGINFO("FORMING MANUAL GROUP...");
				scene_->GetComponent<PieceManager>()->FormSolidGroup(piece);
			}
		}
		if (ui::Button("Remove PieceSolidificationGroup"))
		{
			Vector3 worldPos;
			Piece* piece = scene_->GetComponent<PieceManager>()->GetClosestAimPiece(worldPos, character_->headNode_);
			if (piece) {
				scene_->GetComponent<PieceManager>()->RemoveSolidGroup(piece->GetPieceGroup());
			}
		}

		ui::Checkbox("DynamicRodDetachment", &scene_->GetComponent<PieceManager>()->enableDynamicRodDetach_);





		ui::End();


		ui::Begin("Stats");

		ui::Text(("Character Head Position (Camera): " + character_->headNode_->GetWorldPosition().ToString()).c_str());
		ui::Text(("Character RightHand Position: " + character_->rightHandNode_->GetWorldPosition().ToString()).c_str());
		ui::Text(("Character LeftHand Position: " + character_->leftHandNode_->GetWorldPosition().ToString()).c_str());
		ui::Text(("Character Node Position: " + character_->GetNode()->GetWorldPosition().ToString()).c_str());

		ui::End();

	}

	
}

void TechGame::HandleNodeCollisionStart(StringHash eventType, VariantMap& eventData)
{
	NewtonRigidBody* bodyA = static_cast<NewtonRigidBody*>(eventData[NewtonPhysicsCollisionStart::P_BODYA].GetPtr());
	NewtonRigidBody* bodyB = static_cast<NewtonRigidBody*>(eventData[NewtonPhysicsCollisionStart::P_BODYB].GetPtr());

	NewtonRigidBodyContactEntry* contactData = static_cast<NewtonRigidBodyContactEntry*>(eventData[NewtonPhysicsCollisionStart::P_CONTACT_DATA].GetPtr());

	float largestForce = 0.0f;
	Vector3 position;
	for (int i = 0; i < contactData->numContacts; i++) {
		if (contactData->contactForces[i].Length() > largestForce) {
			largestForce = contactData->contactForces[i].Length();
			position = contactData->contactPositions[i];
		}
	}

	Vector3 velA = bodyA->GetLinearVelocity();
	Vector3 velB = bodyB->GetLinearVelocity();
	float impactVelocity = velA.Length() + velB.Length();

	//URHO3D_LOGINFO(String(impactVelocity));

	if (impactVelocity > 0.1 && !crashSoundPlaying) {

		Node* tempSoundNode = bodyA->GetNode()->CreateChild();
		tempSoundNode->SetTemporary(true);

		SoundSource3D* soundSource = tempSoundNode->CreateComponent<SoundSource3D>();
		soundSource->SetTemporary(true);
		soundSource->SetAutoRemoveMode(REMOVE_NODE);
		soundSource->SetFarDistance(50);
		soundSource->SetNearDistance(1);




		int randSound = Random(1, 4);
		Sound* impactSound;
		//if(randSound == 0)
		//	impactSound = GetSubsystem<ResourceCache>()->GetResource<Sound>("Sounds/Metal Sounds/metal_sound_89.wav");
		if (randSound == 1)
			impactSound = GetSubsystem<ResourceCache>()->GetResource<Sound>("Sounds/Metal Sounds/metal_sound_82.wav");
		if (randSound == 2)
			impactSound = GetSubsystem<ResourceCache>()->GetResource<Sound>("Sounds/Metal Sounds/metal_sound_83.wav");
		if (randSound == 3)
			impactSound = GetSubsystem<ResourceCache>()->GetResource<Sound>("Sounds/Metal Sounds/metal_sound_81.wav");
		//if (randSound == 4)
		//	impactSound = GetSubsystem<ResourceCache>()->GetResource<Sound>("Sounds/Metal Sounds/metal_sound_69.wav");


		//soundSource->Play(impactSound, Random(44100 * 0.9f, 44100 * 1.1f), Clamp(impactVelocity, 0.0f, 0.25f));

	}
}

void TechGame::HandleNodeCollisionEnd(StringHash eventType, VariantMap& eventData)
{

}
