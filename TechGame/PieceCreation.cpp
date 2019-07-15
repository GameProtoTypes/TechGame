#include "TechGame.h"
#include "PiecePoint.h"
#include "Piece.h"
#include "PiecePointRow.h"

#include "NewtonCollisionShapesDerived.h"
#include "PieceManager.h"


Node* TechGame::CreatePiece(Node* parent, ea::string name, bool loadExisting)
{
	Node* root;

	if (loadExisting)
	{
		SharedPtr<File> file = SharedPtr<File>(new File(context_));
		file->Open(name + ".xml", FILE_READ);
		root = scene_->InstantiateXML(*file, Vector3::ZERO, Quaternion::IDENTITY);
	}
	else {
		root = parent->CreateChild();
		
		float scaleFactor = scene_->GetComponent<PieceManager>()->GetScaleFactor();

		URHO3D_LOGINFO("scale factor: " + ea::to_string(scaleFactor));

		auto* body = root->CreateComponent<NewtonRigidBody>();
		Node* visualNode = root->CreateChild("visualNode");
		StaticModel* staticMdl = visualNode->CreateComponent<StaticModel>();
		visualNode->SetScale(scaleFactor/0.25f);
		Color color;


		if (name == "8_piece_Cshape") {

			Model* pieceModel = GetSubsystem<ResourceCache>()->GetResource<Model>("Models/8_piece_Cshape.mdl");
			Vector3 offset(0, -0.25, 0);

			//make shapes
			auto* shape1 = root->CreateComponent<NewtonCollisionShape_Box>();
			shape1->SetScaleFactor(Vector3(2, 1, 1)*scaleFactor);
			shape1->SetPositionOffset((Vector3(0, 1, 0) + offset)*scaleFactor);

			auto* shape2 = root->CreateComponent<NewtonCollisionShape_Box>();
			shape2->SetScaleFactor(Vector3(1, 3, 1)*scaleFactor);
			shape2->SetPositionOffset((Vector3(1.5, 0, 0) + offset)*scaleFactor);

			auto* shape3 = root->CreateComponent<NewtonCollisionShape_Box>();
			shape3->SetScaleFactor(Vector3(1, 3, 1)*scaleFactor);
			shape3->SetPositionOffset((Vector3(-1.5, 0, 0) + offset)*scaleFactor);



			ea::vector<Vector3> positions;
			positions.push_back(Vector3(-1.5, -0.25, 0));
			positions.push_back(Vector3(-0.5, 0.75,  0));
			positions.push_back(Vector3(0.5, 0.75,   0));
			positions.push_back(Vector3(-1.5, 0.75,  0));
			positions.push_back(Vector3(1.5, 0.75,   0));
			positions.push_back(Vector3(1.5, -0.25,  0));
			positions.push_back(Vector3(1.5, -1.25,  0));
			positions.push_back(Vector3(-1.5, -1.25, 0));


			
			for (Vector3& position : positions)
			{
				PiecePointRow* pointRow = root->CreateComponent<PiecePointRow>();
				//make hole nodes.
				Node* hole0 = root->CreateChild();
				hole0->SetPosition((position + Vector3(0,0,-0.25))*scaleFactor);
				PiecePoint* point0 = hole0->CreateComponent<PiecePoint>();

				Node* hole1 = root->CreateChild();
				hole1->SetPosition((position + Vector3(0, 0, 0.25))*scaleFactor);
				PiecePoint* point1 = hole1->CreateComponent<PiecePoint>();
				
				pointRow->PushBack(point1);
				pointRow->PushBack(point0);
				pointRow->SetRowType(PiecePointRow::RowType_Hole);
				pointRow->Finalize();
			}


			staticMdl->SetModel(pieceModel);
		}

		if (name == "2_sleeve") {

			Model* pieceModel = GetSubsystem<ResourceCache>()->GetResource<Model>("Models/2_sleeve.mdl");
			
			Vector3 offset(0, 0, 0);

			
			const float thickness = 2.0f;

			//make shapes
			auto* shape1 = root->CreateComponent<NewtonCollisionShape_Box>();
			shape1->SetScaleFactor(Vector3(1, 1, thickness)*scaleFactor);
			shape1->SetPositionOffset((Vector3(0, 0, 0) + offset)*scaleFactor);


			//make hole nodes.
			PiecePointRow* pointRow = root->CreateComponent<PiecePointRow>();

			
			Node* prevHole = nullptr;
			for (float z = -thickness/2.0f + 0.25f; z <= thickness/2.0f - 0.25f; z += 0.5f) {

				Node* hole0 = root->CreateChild();
				hole0->SetPosition(Vector3(0, 0, z)*scaleFactor);
				PiecePoint* point = hole0->CreateComponent<PiecePoint>();

				pointRow->SetRowType(PiecePointRow::RowType_Hole);
				pointRow->PushBack(point);
			}
			pointRow->Finalize();
			staticMdl->SetModel(pieceModel);
		}

		if (name == "1_cap_small") {

			Model* pieceModel = GetSubsystem<ResourceCache>()->GetResource<Model>("Models/1_cap_small.mdl");
			Vector3 offset(0, 0, 0);


			const float thickness = 0.5f;

			//make shapes
			auto* shape1 = root->CreateComponent<NewtonCollisionShape_Box>();
			shape1->SetScaleFactor(Vector3(1, 1, thickness)*scaleFactor);
			shape1->SetPositionOffset((Vector3(0, 0, 0) + offset)*scaleFactor);


			//make hole nodes.

			Node* prevHole = nullptr;

			Node* hole0 = root->CreateChild();
			hole0->SetPosition(Vector3(0, 0, 0)*scaleFactor);
			PiecePointRow* row = root->CreateComponent<PiecePointRow>();
			PiecePoint* point = hole0->CreateComponent<PiecePoint>();
			point->isEndCap_ = true;
			point->direction_ = -Vector3::FORWARD;
			
			row->PushBack(point);
			
			row->SetRowType(PiecePointRow::RowType_HoleTight);
			row->SetRowDirectionLocal(Vector3(0, 0, 1));
			row->Finalize();
			staticMdl->SetModel(pieceModel);
		}



		if (name == "4x4_piece_thin") {

			Model* pieceModel = GetSubsystem<ResourceCache>()->GetResource<Model>("Models/4x4_piece_thin.mdl");
			Vector3 offset(0, 0, 0);

			const int length = 4;
			const int width = 4;
			const float thickness = 0.5f;

			//make shapes
			auto* shape1 = root->CreateComponent<NewtonCollisionShape_Box>();
			shape1->SetScaleFactor(Vector3(length, width, thickness)*scaleFactor);
			shape1->SetPositionOffset((Vector3(0, 0, 0) + offset)*scaleFactor);


			//make hole nodes.
			for (int xi = 0; xi < length; xi++) 
			{
				for (int yi = 0; yi < width; yi++) 
				{

					float x = (float(xi) + 0.5f) - length / 2.0f;
					float y = (float(yi) + 0.5f) - width / 2.0f;

					Node* hole0 = root->CreateChild();
					hole0->SetPosition(Vector3(x, y, 0)*scaleFactor);
					PiecePoint* point = hole0->CreateComponent<PiecePoint>();

					PiecePointRow* row = root->CreateComponent<PiecePointRow>();
					row->PushBack(point);
					row->SetRowType(PiecePointRow::RowType_Hole);

					row->SetRowDirectionLocal(Vector3(0, 0, 1));
					row->Finalize();
				}
			}

			staticMdl->SetModel(pieceModel);

			//body->SetUseInertiaHack(true);
		}





		if (name == "6_piece_thin") {

			Model* pieceModel = GetSubsystem<ResourceCache>()->GetResource<Model>("Models/6_piece_thin.mdl");
			Vector3 offset(0, 0, 0);

			const int length = 6;
			const float thickness = 0.5f;

			//make shapes
			auto* shape1 = root->CreateComponent<NewtonCollisionShape_Box>();
			shape1->SetScaleFactor(Vector3(length, 1, thickness)*scaleFactor);
			shape1->SetPositionOffset((Vector3(0, 0, 0) + offset)*scaleFactor);


			//make hole nodes.
			for (int i = 0; i < length; i++) {

				float x = (float(i) + 0.5f) - length / 2.0f;

				Node* hole0 = root->CreateChild();
				hole0->SetPosition(Vector3(x, 0, 0)*scaleFactor);
				PiecePoint* point = hole0->CreateComponent<PiecePoint>();

				PiecePointRow* row = root->CreateComponent<PiecePointRow>();
				row->PushBack(point);
				row->SetRowType(PiecePointRow::RowType_Hole);

				row->SetRowDirectionLocal(Vector3(0, 0, 1));
				row->Finalize();
			}

			staticMdl->SetModel(pieceModel);

			//body->SetUseInertiaHack(true);
		}

		if (name == "6_piece_thick") {

			Model* pieceModel = GetSubsystem<ResourceCache>()->GetResource<Model>("Models/6_piece_thick.mdl");
			Vector3 offset(0, 0, 0);

			const int length = 6;
			const float thickness = 1.0f;

			//make shapes
			auto* shape1 = root->CreateComponent<NewtonCollisionShape_Box>();
			shape1->SetScaleFactor(Vector3(length, 1, thickness)*scaleFactor);
			shape1->SetPositionOffset((Vector3(0, 0, 0) + offset)*scaleFactor);


			//make hole nodes.
			for (int i = 0; i < length; i++) {

				float x = (float(i) + 0.5f) - length / 2.0f;


				Node* hole0 = root->CreateChild();
				hole0->SetPosition((Vector3(x, 0, -0.25))*scaleFactor);
				PiecePoint* point0 = hole0->CreateComponent<PiecePoint>();

				Node* hole1 = root->CreateChild();
				hole1->SetPosition((Vector3(x, 0, 0.25))*scaleFactor);
				PiecePoint* point1 = hole1->CreateComponent<PiecePoint>();



				PiecePointRow* row = root->CreateComponent<PiecePointRow>();
				row->PushBack(point0);
				row->PushBack(point1);
				row->SetRowType(PiecePointRow::RowType_Hole);

				row->SetRowDirectionLocal(Vector3(0, 0, 1));
				row->Finalize();
			}

			staticMdl->SetModel(pieceModel);

			//body->SetUseInertiaHack(true);
		}




		if (name == "5_piece_thick") {

			Model* pieceModel = GetSubsystem<ResourceCache>()->GetResource<Model>("Models/5_piece_thick.mdl");
			Vector3 offset(0, 0, 0);

			const int length = 5;
			const float thickness = 1.0f;

			//make shapes
			auto* shape1 = root->CreateComponent<NewtonCollisionShape_Box>();
			shape1->SetScaleFactor(Vector3(length, 1, thickness)*scaleFactor);
			shape1->SetPositionOffset((Vector3(0, 0, 0) + offset)*scaleFactor);


			//make hole nodes.
			for (int i = 0; i < length; i++) {

				float x = (float(i) + 0.5f) - length / 2.0f;


				Node* hole0 = root->CreateChild();
				hole0->SetPosition((Vector3(x, 0, -0.25))*scaleFactor);
				PiecePoint* point0 = hole0->CreateComponent<PiecePoint>();

				Node* hole1 = root->CreateChild();
				hole1->SetPosition((Vector3(x, 0, 0.25))*scaleFactor);
				PiecePoint* point1 = hole1->CreateComponent<PiecePoint>();



				PiecePointRow* row = root->CreateComponent<PiecePointRow>();
				row->PushBack(point0);
				row->PushBack(point1);
				row->SetRowType(PiecePointRow::RowType_Hole);

				row->SetRowDirectionLocal(Vector3(0, 0, 1));
				row->Finalize();
			}

			staticMdl->SetModel(pieceModel);

			//body->SetUseInertiaHack(true);
		}



		if (name == "4_piece_thick") {

			Model* pieceModel = GetSubsystem<ResourceCache>()->GetResource<Model>("Models/4_piece_thick.mdl");
			Vector3 offset(0, 0, 0);

			const int length = 4;
			const float thickness = 1.0f;

			//make shapes
			auto* shape1 = root->CreateComponent<NewtonCollisionShape_Box>();
			shape1->SetScaleFactor(Vector3(length, 1, thickness)*scaleFactor);
			shape1->SetPositionOffset((Vector3(0, 0, 0) + offset)*scaleFactor);


			//make hole nodes.
			for (int i = 0; i < length; i++) {

				float x = (float(i) + 0.5f) - length / 2.0f;


				Node* hole0 = root->CreateChild();
				hole0->SetPosition((Vector3(x, 0, -0.25))*scaleFactor);
				PiecePoint* point0 = hole0->CreateComponent<PiecePoint>();

				Node* hole1 = root->CreateChild();
				hole1->SetPosition((Vector3(x, 0, 0.25))*scaleFactor);
				PiecePoint* point1 = hole1->CreateComponent<PiecePoint>();



				PiecePointRow* row = root->CreateComponent<PiecePointRow>();
				row->PushBack(point0);
				row->PushBack(point1);
				row->SetRowType(PiecePointRow::RowType_Hole);

				row->SetRowDirectionLocal(Vector3(0, 0, 1));
				row->Finalize();
			}

			staticMdl->SetModel(pieceModel);

			//body->SetUseInertiaHack(true);
		}

		if (name == "3_piece_thick") {

			Model* pieceModel = GetSubsystem<ResourceCache>()->GetResource<Model>("Models/3_piece_thick.mdl");
			Vector3 offset(0, 0, 0);

			const int length = 3;
			const float thickness = 1.0f;

			//make shapes
			auto* shape1 = root->CreateComponent<NewtonCollisionShape_Box>();
			shape1->SetScaleFactor(Vector3(length, 1, thickness)*scaleFactor);
			shape1->SetPositionOffset((Vector3(0, 0, 0) + offset)*scaleFactor);


			//make hole nodes.
			for (int i = 0; i < length; i++) {

				float x = (float(i) + 0.5f) - length / 2.0f;


				Node* hole0 = root->CreateChild();
				hole0->SetPosition((Vector3(x, 0, -0.25))*scaleFactor);
				PiecePoint* point0 = hole0->CreateComponent<PiecePoint>();

				Node* hole1 = root->CreateChild();
				hole1->SetPosition((Vector3(x, 0, 0.25))*scaleFactor);
				PiecePoint* point1 = hole1->CreateComponent<PiecePoint>();



				PiecePointRow* row = root->CreateComponent<PiecePointRow>();
				row->PushBack(point0);
				row->PushBack(point1);
				row->SetRowType(PiecePointRow::RowType_Hole);

				row->SetRowDirectionLocal(Vector3(0, 0, 1));
				row->Finalize();
			}

			staticMdl->SetModel(pieceModel);

			//body->SetUseInertiaHack(true);
		}

		if (name == "2_piece_thick") {

			Model* pieceModel = GetSubsystem<ResourceCache>()->GetResource<Model>("Models/2_piece_thick.mdl");
			Vector3 offset(0, 0, 0);

			const int length = 2;
			const float thickness = 1.0f;

			//make shapes
			auto* shape1 = root->CreateComponent<NewtonCollisionShape_Box>();
			shape1->SetScaleFactor(Vector3(length, 1, thickness)*scaleFactor);
			shape1->SetPositionOffset((Vector3(0, 0, 0) + offset)*scaleFactor);


			//make hole nodes.
			for (int i = 0; i < length; i++) {

				float x = (float(i) + 0.5f) - length / 2.0f;


				Node* hole0 = root->CreateChild();
				hole0->SetPosition((Vector3(x, 0, -0.25))*scaleFactor);
				PiecePoint* point0 = hole0->CreateComponent<PiecePoint>();

				Node* hole1 = root->CreateChild();
				hole1->SetPosition((Vector3(x, 0, 0.25))*scaleFactor);
				PiecePoint* point1 = hole1->CreateComponent<PiecePoint>();



				PiecePointRow* row = root->CreateComponent<PiecePointRow>();
				row->PushBack(point0);
				row->PushBack(point1);
				row->SetRowType(PiecePointRow::RowType_Hole);

				row->SetRowDirectionLocal(Vector3(0, 0, 1));
				row->Finalize();
			}

			staticMdl->SetModel(pieceModel);

			//body->SetUseInertiaHack(true);
		}




		if (name == "rod_round_4")
		{
			Model* pieceModel = GetSubsystem<ResourceCache>()->GetResource<Model>("Models/rod_round_4.mdl");
			Vector3 offset(0, 0, 0);

			//make shapes
			auto* shape1 = root->CreateComponent<NewtonCollisionShape_Cylinder>();
			shape1->SetScaleFactor(Vector3(4, 0.5, 0.5)*scaleFactor);

			//shape1->SetRadius(0.25*scaleFactor);
			//shape1->SetLength(4 * scaleFactor);

			shape1->SetRotationOffset(Quaternion(90, Vector3(0, 1, 0)));

			PiecePointRow* pointRow = root->CreateComponent<PiecePointRow>();

			for (int p = 0; p < 8; p++)
			{
				Node* point = root->CreateChild();
				point->SetPosition(Vector3(0, 0, (p*0.5f - 3.5*0.5f)*scaleFactor));
				PiecePoint* piecePoint = point->CreateComponent<PiecePoint>();
				
				pointRow->PushBack(piecePoint);
				
				

				if (p == 0 || p == 7)
					piecePoint->isEndCap_ = true;

			}
			pointRow->Finalize();
			pointRow->SetRowType(PiecePointRow::RowType_RodRound);

			//body->SetUseInertiaHack(true);

			staticMdl->SetModel(pieceModel);
			
			
		}

		if (name == "rod_round_no_caps_4")
		{
			Model* pieceModel = GetSubsystem<ResourceCache>()->GetResource<Model>("Models/rod_round_no_caps_4.mdl");
			Vector3 offset(0, 0, 0);

			//make shapes
			auto* shape1 = root->CreateComponent<NewtonCollisionShape_Cylinder>();
			shape1->SetScaleFactor(Vector3(4, 0.5, 0.5)*scaleFactor);
			shape1->SetRotationOffset(Quaternion(90, Vector3(0, 1, 0)));

			PiecePointRow* pointRow = root->CreateComponent<PiecePointRow>();

			for (int p = 0; p < 8; p++)
			{
				Node* point = root->CreateChild();
				point->SetPosition(Vector3(0, 0, (p*0.5f - 3.5*0.5f)*scaleFactor));
				PiecePoint* piecePoint = point->CreateComponent<PiecePoint>();

				pointRow->PushBack(piecePoint);
			}
			//body->SetUseInertiaHack(true);
			pointRow->Finalize();
			pointRow->SetRowType(PiecePointRow::RowType_RodRound);
			staticMdl->SetModel(pieceModel);
		}

		if (name == "rod_hard_4")
		{
			Model* pieceModel = GetSubsystem<ResourceCache>()->GetResource<Model>("Models/rod_hard_4.mdl");
			Vector3 offset(0, 0, 0);

			//make shapes
			auto* shape1 = root->CreateComponent<NewtonCollisionShape_Cylinder>();
			shape1->SetScaleFactor(Vector3(4, 0.5, 0.5)*scaleFactor);
			shape1->SetRotationOffset(Quaternion(90, Vector3(0, 1, 0)));

			PiecePointRow* pointRow = root->CreateComponent<PiecePointRow>();


			for (int p = 0; p < 8; p++)
			{
				Node* point = root->CreateChild();
				point->SetPosition(Vector3(0, 0, (p*0.5f - 3.5*0.5f)*scaleFactor));
				PiecePoint* piecePoint = point->CreateComponent<PiecePoint>();



				pointRow->PushBack(piecePoint);

				if (p == 0 || p == 7)
					piecePoint->isEndCap_ = true;
			}
			//body->SetUseInertiaHack(true);
			pointRow->Finalize();
			pointRow->SetRowType(PiecePointRow::RowType_RodHard);

			staticMdl->SetModel(pieceModel);

		}



		staticMdl->SetCastShadows(true);

		Piece* piece = root->CreateComponent<Piece>();
		


		SharedPtr<File> file = SharedPtr<File>(new File(context_));
		file->Open(name + ".xml", FILE_WRITE);
		root->SaveXML(*file);
	}



	
	root->GetComponent<Piece>()->SetPrimaryColor(Color(Random(), Random(), Random()));


	return root;
}

