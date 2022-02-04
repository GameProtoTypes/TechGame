#include "PiecePoint.h"
#include "Piece.h"
#include "PiecePointRow.h"

#include "NewtonCollisionShapesDerived.h"
#include "PieceManager.h"
#include "PieceGear.h"
#include "NewtonHingeConstraint.h"


Node* PieceManager::CreatePiece(ea::string name, bool loadExisting)
{
	Node* root;

	if (loadExisting)
	{
		SharedPtr<File> file = SharedPtr<File>(new File(context_));
		file->Open(name + ".xml", FILE_READ);
		root = GetScene()->InstantiateXML(*file, Vector3::ZERO, Quaternion::IDENTITY);
	}
	else {
		root = GetScene()->CreateChild();
		root->SetVar("PieceName", name);
		float scaleFactor = GetScene()->GetComponent<PieceManager>()->GetScaleFactor();

		auto* body = root->CreateComponent<NewtonRigidBody>();
		Node* visualNode = root->CreateChild("visualNode");
		StaticModel* staticMdl = visualNode->CreateComponent<StaticModel>();
		visualNode->SetScale(scaleFactor/0.25f);
		Color color;
		ea::string palletId = "";

		if (name == "8_piece_Cshape") {

			Model* pieceModel = GetSubsystem<ResourceCache>()->GetResource<Model>("Models/" + name + ".mdl");
			Vector3 offset(0, -0.25, 0);
			palletId = "ThickHoles";
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

			Model* pieceModel = GetSubsystem<ResourceCache>()->GetResource<Model>("Models/" + name + ".mdl");
			palletId = "Sleeves";
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

			Model* pieceModel = GetSubsystem<ResourceCache>()->GetResource<Model>("Models/" + name + ".mdl");
			Vector3 offset(0, 0, 0);
			palletId = "Caps";

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
			
			row->SetRowType(PiecePointRow::RowType_Hole);
			row->SetRowDirectionLocal(Vector3(0, 0, 1));
			row->Finalize();
			staticMdl->SetModel(pieceModel);
		}


		if (name == "gear_large") {

			Model* pieceModel = GetSubsystem<ResourceCache>()->GetResource<Model>("Models/" + name + ".mdl");
			Vector3 offset(0, 0, 0);
			palletId = "Gears";

			const float thickness = 0.5f;

			//make shapes
			auto* shape1 = root->CreateComponent<NewtonCollisionShape_Cylinder>();
			shape1->SetLength(thickness*scaleFactor);
			shape1->SetRadius(1.5f*scaleFactor);
			shape1->SetRotationOffset(Quaternion(0, 90, 0));
			shape1->SetPositionOffset((Vector3(0, 0, 0) + offset)*scaleFactor);


			//make hole nodes.
			Node* prevHole = nullptr;

			Node* holeCenter = root->CreateChild();
			holeCenter->SetPosition(Vector3(0, 0, 0)*scaleFactor);
			PiecePointRow* rowCenter = root->CreateComponent<PiecePointRow>();
			PiecePoint* pointCenter = holeCenter->CreateComponent<PiecePoint>();
			pointCenter->direction_ = -Vector3::FORWARD;


			const float diagOffset = 0.5;

			Node* hole0 = root->CreateChild();
			hole0->SetPosition(Vector3(-diagOffset, -diagOffset, 0)*scaleFactor);
			PiecePointRow* row0 = root->CreateComponent<PiecePointRow>();
			PiecePoint* point0 = hole0->CreateComponent<PiecePoint>();
			point0->direction_ = -Vector3::FORWARD;

			Node* hole1 = root->CreateChild();
			hole1->SetPosition(Vector3(diagOffset, diagOffset, 0)*scaleFactor);
			PiecePointRow* row1 = root->CreateComponent<PiecePointRow>();
			PiecePoint* point1 = hole1->CreateComponent<PiecePoint>();
			point1->direction_ = -Vector3::FORWARD;

			Node* hole2 = root->CreateChild();
			hole2->SetPosition(Vector3(-diagOffset, diagOffset, 0)*scaleFactor);
			PiecePointRow* row2 = root->CreateComponent<PiecePointRow>();
			PiecePoint* point2 = hole2->CreateComponent<PiecePoint>();
			point2->direction_ = -Vector3::FORWARD;

			Node* hole3 = root->CreateChild();
			hole3->SetPosition(Vector3(diagOffset, -diagOffset, 0)*scaleFactor);
			PiecePointRow* row3 = root->CreateComponent<PiecePointRow>();
			PiecePoint* point3 = hole3->CreateComponent<PiecePoint>();
			point3->direction_ = -Vector3::FORWARD;



			rowCenter->PushBack(pointCenter);
			row0->PushBack(point0);
			row1->PushBack(point1);
			row2->PushBack(point2);
			row3->PushBack(point3);

			rowCenter->SetRowType(PiecePointRow::RowType_Hole);
			rowCenter->SetRowDirectionLocal(Vector3(0, 0, 1));
			rowCenter->Finalize();


			row0->SetRowType(PiecePointRow::RowType_Hole);
			row0->SetRowDirectionLocal(Vector3(0, 0, 1));
			row0->Finalize();

			row1->SetRowType(PiecePointRow::RowType_Hole);
			row1->SetRowDirectionLocal(Vector3(0, 0, 1));
			row1->Finalize();

			row2->SetRowType(PiecePointRow::RowType_Hole);
			row2->SetRowDirectionLocal(Vector3(0, 0, 1));
			row2->Finalize();
			
			row3->SetRowType(PiecePointRow::RowType_Hole);
			row3->SetRowDirectionLocal(Vector3(0, 0, 1));
			row3->Finalize();

			//add gear component
			PieceGear* gear = root->CreateComponent<PieceGear>();
			gear->SetRadius(1.5f * scaleFactor);

			staticMdl->SetModel(pieceModel);
		}

		if (name == "gear_extra_large") {

			Model* pieceModel = GetSubsystem<ResourceCache>()->GetResource<Model>("Models/" + name + ".mdl");
			Vector3 offset(0, 0, 0);
			palletId = "Gears";

			const float thickness = 0.5f;

			//make shapes
			auto* shape1 = root->CreateComponent<NewtonCollisionShape_Cylinder>();
			shape1->SetLength(thickness*scaleFactor);
			shape1->SetRadius(2.0f*scaleFactor);
			shape1->SetRotationOffset(Quaternion(0, 90, 0));
			shape1->SetPositionOffset((Vector3(0, 0, 0) + offset)*scaleFactor);


			//make hole nodes.
			Node* prevHole = nullptr;

			Node* holeCenter = root->CreateChild();
			holeCenter->SetPosition(Vector3(0, 0, 0)*scaleFactor);
			PiecePointRow* rowCenter = root->CreateComponent<PiecePointRow>();
			PiecePoint* pointCenter = holeCenter->CreateComponent<PiecePoint>();
			pointCenter->direction_ = -Vector3::FORWARD;


			

			Node* hole0 = root->CreateChild();
			hole0->SetPosition(Vector3(-1, 0.0, 0.0)*scaleFactor);
			PiecePointRow* row0 = root->CreateComponent<PiecePointRow>();
			PiecePoint* point0 = hole0->CreateComponent<PiecePoint>();
			point0->direction_ = -Vector3::FORWARD;

			Node* hole1 = root->CreateChild();
			hole1->SetPosition(Vector3(1, 0, 0)*scaleFactor);
			PiecePointRow* row1 = root->CreateComponent<PiecePointRow>();
			PiecePoint* point1 = hole1->CreateComponent<PiecePoint>();
			point1->direction_ = -Vector3::FORWARD;

			Node* hole2 = root->CreateChild();
			hole2->SetPosition(Vector3(0, 1, 0)*scaleFactor);
			PiecePointRow* row2 = root->CreateComponent<PiecePointRow>();
			PiecePoint* point2 = hole2->CreateComponent<PiecePoint>();
			point2->direction_ = -Vector3::FORWARD;

			Node* hole3 = root->CreateChild();
			hole3->SetPosition(Vector3(0, -1, 0)*scaleFactor);
			PiecePointRow* row3 = root->CreateComponent<PiecePointRow>();
			PiecePoint* point3 = hole3->CreateComponent<PiecePoint>();
			point3->direction_ = -Vector3::FORWARD;



			rowCenter->PushBack(pointCenter);
			row0->PushBack(point0);
			row1->PushBack(point1);
			row2->PushBack(point2);
			row3->PushBack(point3);

			rowCenter->SetRowType(PiecePointRow::RowType_Hole);
			rowCenter->SetRowDirectionLocal(Vector3(0, 0, 1));
			rowCenter->Finalize();


			row0->SetRowType(PiecePointRow::RowType_Hole);
			row0->SetRowDirectionLocal(Vector3(0, 0, 1));
			row0->Finalize();

			row1->SetRowType(PiecePointRow::RowType_Hole);
			row1->SetRowDirectionLocal(Vector3(0, 0, 1));
			row1->Finalize();

			row2->SetRowType(PiecePointRow::RowType_Hole);
			row2->SetRowDirectionLocal(Vector3(0, 0, 1));
			row2->Finalize();

			row3->SetRowType(PiecePointRow::RowType_Hole);
			row3->SetRowDirectionLocal(Vector3(0, 0, 1));
			row3->Finalize();

			//add gear component
			PieceGear* gear = root->CreateComponent<PieceGear>();
			gear->SetRadius(2.0f * scaleFactor);

			staticMdl->SetModel(pieceModel);
		}





		if (name == "gear_medium") {

			Model* pieceModel = GetSubsystem<ResourceCache>()->GetResource<Model>("Models/" + name + ".mdl");
			Vector3 offset(0, 0, 0);
			palletId = "Gears";

			const float thickness = 0.5f;

			//make shapes
			auto* shape1 = root->CreateComponent<NewtonCollisionShape_Cylinder>();
			shape1->SetLength(thickness*scaleFactor);
			shape1->SetRadius(0.9f*scaleFactor);
			shape1->SetRotationOffset(Quaternion(0, 90, 0));
			shape1->SetPositionOffset((Vector3(0, 0, 0) + offset)*scaleFactor);


			//make hole nodes.
			Node* prevHole = nullptr;

			Node* holeCenter = root->CreateChild();
			holeCenter->SetPosition(Vector3(0, 0, 0)*scaleFactor);
			PiecePointRow* rowCenter = root->CreateComponent<PiecePointRow>();
			PiecePoint* pointCenter = holeCenter->CreateComponent<PiecePoint>();
			pointCenter->direction_ = -Vector3::FORWARD;





			rowCenter->PushBack(pointCenter);

			rowCenter->SetRowType(PiecePointRow::RowType_Hole);
			rowCenter->SetRowDirectionLocal(Vector3(0, 0, 1));
			rowCenter->Finalize();

			//add gear component
			PieceGear* gear = root->CreateComponent<PieceGear>();
			gear->SetRadius(1.0f * scaleFactor);

			staticMdl->SetModel(pieceModel);
		}

		if (name == "gear_small") {

			Model* pieceModel = GetSubsystem<ResourceCache>()->GetResource<Model>("Models/" + name + ".mdl");
			Vector3 offset(0, 0, 0);
			palletId = "Gears";
			const float thickness = 0.5f;

			//make shapes
			auto* shape1 = root->CreateComponent<NewtonCollisionShape_Cylinder>();
			shape1->SetLength(thickness*scaleFactor);
			shape1->SetRadius(0.4f*scaleFactor);
			shape1->SetRotationOffset(Quaternion(0, 90, 0));
			shape1->SetPositionOffset((Vector3(0, 0, 0) + offset)*scaleFactor);


			//make hole nodes.
			Node* prevHole = nullptr;

			Node* holeCenter = root->CreateChild();
			holeCenter->SetPosition(Vector3(0, 0, 0)*scaleFactor);
			PiecePointRow* rowCenter = root->CreateComponent<PiecePointRow>();
			PiecePoint* pointCenter = holeCenter->CreateComponent<PiecePoint>();
			pointCenter->direction_ = -Vector3::FORWARD;





			rowCenter->PushBack(pointCenter);

			rowCenter->SetRowType(PiecePointRow::RowType_Hole);
			rowCenter->SetRowDirectionLocal(Vector3(0, 0, 1));
			rowCenter->Finalize();

			//add gear component
			PieceGear* gear = root->CreateComponent<PieceGear>();
			gear->SetRadius(0.5f * scaleFactor);

			staticMdl->SetModel(pieceModel);
		}




		if (name == "4x4_piece_thin") {

			Model* pieceModel = GetSubsystem<ResourceCache>()->GetResource<Model>("Models/" + name + ".mdl");
			Vector3 offset(0, 0, 0);
			palletId = "ThinHoles";

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

		if (name == "4x8_piece_thin") {

			Model* pieceModel = GetSubsystem<ResourceCache>()->GetResource<Model>("Models/" + name + ".mdl");
			Vector3 offset(0, 0, 0);
			palletId = "ThinHoles";
			const int length = 4;
			const int width = 8;
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

			Model* pieceModel = GetSubsystem<ResourceCache>()->GetResource<Model>("Models/" + name + ".mdl");
			Vector3 offset(0, 0, 0);
			palletId = "ThinHoles";

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

			Model* pieceModel = GetSubsystem<ResourceCache>()->GetResource<Model>("Models/" + name + ".mdl");
			Vector3 offset(0, 0, 0);
			palletId = "ThickHoles";

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

			Model* pieceModel = GetSubsystem<ResourceCache>()->GetResource<Model>("Models/" + name + ".mdl");
			Vector3 offset(0, 0, 0);
			palletId = "ThickHoles";

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

			Model* pieceModel = GetSubsystem<ResourceCache>()->GetResource<Model>("Models/" + name + ".mdl");
			Vector3 offset(0, 0, 0);
			palletId = "ThickHoles";

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

			Model* pieceModel = GetSubsystem<ResourceCache>()->GetResource<Model>("Models/" + name + ".mdl");
			Vector3 offset(0, 0, 0);
			palletId = "ThickHoles";

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

			Model* pieceModel = GetSubsystem<ResourceCache>()->GetResource<Model>("Models/" + name + ".mdl");
			Vector3 offset(0, 0, 0);
			palletId = "ThickHoles";

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

		if (name == "rod_round_1")
		{
			Model* pieceModel = GetSubsystem<ResourceCache>()->GetResource<Model>("Models/" + name + ".mdl");
			Vector3 offset(0, 0, 0);
			palletId = "RoundRods";


			//make shapes
			auto* shape1 = root->CreateComponent<NewtonCollisionShape_Cylinder>();
			shape1->SetScaleFactor(Vector3(0.5, 0.5, 0.5)*scaleFactor);

			//shape1->SetRadius(0.25*scaleFactor);
			//shape1->SetLength(4 * scaleFactor);

			shape1->SetRotationOffset(Quaternion(90, Vector3(0, 1, 0)));

			PiecePointRow* pointRow = root->CreateComponent<PiecePointRow>();

			for (int p = 0; p < 2; p++)
			{
				Node* point = root->CreateChild();
				point->SetPosition(Vector3(0, 0, (p*0.5f - 0.5*0.5f)*scaleFactor));
				PiecePoint* piecePoint = point->CreateComponent<PiecePoint>();

				pointRow->PushBack(piecePoint);



				if (p == 0 || p == 1)
					piecePoint->isEndCap_ = true;

			}
			pointRow->Finalize();
			pointRow->SetRowType(PiecePointRow::RowType_RodRound);

			//body->SetUseInertiaHack(true);

			staticMdl->SetModel(pieceModel);


		}
		if (name == "rod_hard_1")
		{
			Model* pieceModel = GetSubsystem<ResourceCache>()->GetResource<Model>("Models/" + name + ".mdl");
			Vector3 offset(0, 0, 0);
			palletId = "HardRods";

			//make shapes
			auto* shape1 = root->CreateComponent<NewtonCollisionShape_Cylinder>();
			shape1->SetScaleFactor(Vector3(0.5, 0.5, 0.5)*scaleFactor);

			//shape1->SetRadius(0.25*scaleFactor);
			//shape1->SetLength(4 * scaleFactor);

			shape1->SetRotationOffset(Quaternion(90, Vector3(0, 1, 0)));

			PiecePointRow* pointRow = root->CreateComponent<PiecePointRow>();

			for (int p = 0; p < 2; p++)
			{
				Node* point = root->CreateChild();
				point->SetPosition(Vector3(0, 0, (p*0.5f - 0.5*0.5f)*scaleFactor));
				PiecePoint* piecePoint = point->CreateComponent<PiecePoint>();

				pointRow->PushBack(piecePoint);



				if (p == 0 || p == 1)
					piecePoint->isEndCap_ = true;

			}
			pointRow->Finalize();
			pointRow->SetRowType(PiecePointRow::RowType_RodHard);

			//body->SetUseInertiaHack(true);

			staticMdl->SetModel(pieceModel);


		}

		if (name == "corner_hard_1")
		{
			Model* pieceModel = GetSubsystem<ResourceCache>()->GetResource<Model>("Models/" + name + ".mdl");
			Vector3 offset(0, 0, 0);
			palletId = "HardRods";

			//make shapes
			auto* shape1 = root->CreateComponent<NewtonCollisionShape_Box>();
			shape1->SetScaleFactor(Vector3(0.5, 0.5, 0.5)*scaleFactor);

			auto* shape2 = root->CreateComponent<NewtonCollisionShape_Box>();
			shape2->SetScaleFactor(Vector3(1.0, 0.5, 0.5)*scaleFactor);

			shape1->SetPositionOffset(Vector3(0, 0, 0.5f)*scaleFactor);
			shape2->SetPositionOffset(Vector3(0.25, 0 , 0)*scaleFactor);
			//shape1->SetRadius(0.25*scaleFactor);
			//shape1->SetLength(4 * scaleFactor);

			

			PiecePointRow* pointRow1 = root->CreateComponent<PiecePointRow>();

			for (int p = 1; p < 2; p++)
			{
				Node* point = root->CreateChild();
				point->SetPosition(Vector3(0, 0, (p*0.5f + 0.25)*scaleFactor));
				PiecePoint* piecePoint = point->CreateComponent<PiecePoint>();

				pointRow1->PushBack(piecePoint);

				if (p == 0 || p == 1)
					piecePoint->isEndCap_ = true;

			}
			pointRow1->SetRowDirectionLocal(Vector3(0, 0, 1));
			pointRow1->Finalize();
			pointRow1->SetRowType(PiecePointRow::RowType_RodHard);



			PiecePointRow* pointRow2 = root->CreateComponent<PiecePointRow>();

			for (int p = 1; p < 2; p++)
			{
				Node* point = root->CreateChild();
				point->SetPosition(Vector3((p*0.5f + 0.25)*scaleFactor, 0, 0));
				point->SetRotation(Quaternion(0, 90, 0));
				PiecePoint* piecePoint = point->CreateComponent<PiecePoint>();
				
				pointRow2->PushBack(piecePoint);

				if (p == 0 || p == 1)
					piecePoint->isEndCap_ = true;

			}
			pointRow2->SetRowDirectionLocal(Vector3(0, 0, 1));
			pointRow2->Finalize();
			pointRow2->SetRowType(PiecePointRow::RowType_RodHard);



			//body->SetUseInertiaHack(true);

			staticMdl->SetModel(pieceModel);


		}







		if (name == "rod_round_4")
		{
			Model* pieceModel = GetSubsystem<ResourceCache>()->GetResource<Model>("Models/" + name + ".mdl");
			Vector3 offset(0, 0, 0);
			palletId = "RoundRods";


			//make shapes
			auto* shape1 = root->CreateComponent<NewtonCollisionShape_Cylinder>();
			shape1->SetScaleFactor(Vector3(4, 0.5, 0.5)*scaleFactor);

			//shape1->SetRadius(0.25*scaleFactor);
			//shape1->SetLength(4 * scaleFactor);

			float length = 4.0f;

			shape1->SetRotationOffset(Quaternion(90, Vector3(0, 1, 0)));

			PiecePointRow* pointRow = root->CreateComponent<PiecePointRow>();

			for (int p = 0; p < 8; p++)
			{
				Node* point = root->CreateChild();
				point->SetPosition(Vector3(0, 0, (p*0.5f - (length - 0.5)*0.5f)*scaleFactor));
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
			Model* pieceModel = GetSubsystem<ResourceCache>()->GetResource<Model>("Models/" + name + ".mdl");
			Vector3 offset(0, 0, 0);
			palletId = "RoundRods";

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
			Model* pieceModel = GetSubsystem<ResourceCache>()->GetResource<Model>("Models/" + name + ".mdl");
			Vector3 offset(0, 0, 0);
			palletId = "HardRods";

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
		piece->SetColorPalletId(palletId);


		//SharedPtr<File> file = SharedPtr<File>(new File(context_));
		//file->Open(name + ".xml", FILE_WRITE);
		//root->SaveXML(*file);
	}



	



	return root;
}

///Creates a piece assembly as child of single root node.  remove children from root node when "unpacking" the assembly.
Urho3D::Node* PieceManager::CreatePieceAssembly(ea::string name, bool loadExisting)
{
	ea::vector<Piece*> pieces;
	
	Node* superRoot = GetScene()->CreateChild(name);
	superRoot->AddTag("PieceAssembly");


	if (loadExisting)
	{
		SharedPtr<File> file = SharedPtr<File>(new File(context_));
		file->Open(name + ".xml", FILE_READ);
		superRoot = GetScene()->InstantiateXML(*file, Vector3::ZERO, Quaternion::IDENTITY);
	}
	else
	{
		

		if (name == "Motor") {

			NewtonRigidBody* outerHousingBody;
			NewtonRigidBody* innerHousingBody;
			Piece* outerPiece;
			Piece* innerPiece;
			{
				Node* HousingNode;
				HousingNode = superRoot->CreateChild();

				HousingNode->SetVar("PieceName", "motorhousing");
				HousingNode->SetVar("AssemblyName", name);
				float scaleFactor = GetScene()->GetComponent<PieceManager>()->GetScaleFactor();

				outerHousingBody = HousingNode->CreateComponent<NewtonRigidBody>();
				Node* visualNode = HousingNode->CreateChild("visualNode");
				StaticModel* staticMdl = visualNode->CreateComponent<StaticModel>();
				staticMdl->SetCastShadows(true);
				visualNode->SetScale(scaleFactor / 0.25f);


				Color color;


				Model* pieceModel = GetSubsystem<ResourceCache>()->GetResource<Model>("Models/outerhousing.mdl");
				Vector3 offset(0, 0, 0);

				//make shapes
				auto* shape1 = HousingNode->CreateComponent<NewtonCollisionShape_Box>();
				shape1->SetScaleFactor(Vector3(2, 2 - 2 * 0.25f, 3) * scaleFactor);
				shape1->SetRotationOffset(Quaternion(90, Vector3(1, 0, 0)));

				auto* shape2 = HousingNode->CreateComponent<NewtonCollisionShape_Box>();
				shape2->SetScaleFactor(Vector3(2, 2 - 2 * 0.25f, 3) * scaleFactor);
				//shape2->SetRotationOffset(Quaternion(0, Vector3(0, 1, 0)));


				for (int i = 0; i < 4; i++) {
					PiecePointRow* pointRow = HousingNode->CreateComponent<PiecePointRow>();

					float x, y = 0;
					if (i == 0 || i == 1) {
						if (i == 0) {
							x = 1; y = 0;
						}
						if (i == 1) {
							x = -1; y = 0;
						}
					}
					if (i == 2 || i == 3) {
						if (i == 2) {
							x = 0; y = -1;
						}
						if (i == 3) {
							x = 0; y = 1;
						}
					}

					for (int p = 0; p < 4; p++)
					{

						Node* point = HousingNode->CreateChild();
						point->SetPosition(Vector3((p*0.5f - 1.5*0.5f)*scaleFactor, x*scaleFactor, y*scaleFactor));
						PiecePoint* piecePoint = point->CreateComponent<PiecePoint>();
						pointRow->PushBack(piecePoint);
					}

					pointRow->Finalize();
					pointRow->SetRowType(PiecePointRow::RowType_Hole);

				}


				//body->SetUseInertiaHack(true);


				staticMdl->SetModel(pieceModel);

				outerPiece = HousingNode->CreateComponent<Piece>();
				

				pieces.push_back(outerPiece);

			}
			//create inner rotational piece
			{
				Node* RotationalNode;
				RotationalNode = superRoot->CreateChild();

				RotationalNode->SetVar("PieceName", "motorrotatioaldrive");
				RotationalNode->SetVar("AssemblyName", name);

				float scaleFactor = GetScene()->GetComponent<PieceManager>()->GetScaleFactor();

				innerHousingBody = RotationalNode->CreateComponent<NewtonRigidBody>();
				Node* visualNode = RotationalNode->CreateChild("visualNode");
				StaticModel* staticMdl = visualNode->CreateComponent<StaticModel>();
				staticMdl->SetCastShadows(true);
				visualNode->SetScale(scaleFactor / 0.25f);

				//make shapes
				auto* shape1 = RotationalNode->CreateComponent<NewtonCollisionShape_Cylinder>();
				shape1->SetScaleFactor(Vector3(2, 1, 1) * scaleFactor);
				shape1->SetRotationOffset(Quaternion(90, Vector3(1, 0, 0)));

				Color color;

				PiecePointRow* pointRow = RotationalNode->CreateComponent<PiecePointRow>();


				for (int p = 0; p < 4; p++)
				{

					Node* point = RotationalNode->CreateChild();
					point->SetPosition(Vector3((p*0.5f - 1.5*0.5f)*scaleFactor, 0, 0));
					PiecePoint* piecePoint = point->CreateComponent<PiecePoint>();
					
					pointRow->PushBack(piecePoint);
				}
				pointRow->SetRowType(PiecePointRow::RowType_Hole);
				pointRow->Finalize();

				Model* pieceModel = GetSubsystem<ResourceCache>()->GetResource<Model>("Models/InnerRotationalDrive.mdl");
				Vector3 offset(0, 0, 0);

				staticMdl->SetModel(pieceModel);
				innerPiece = RotationalNode->CreateComponent<Piece>();

				pieces.push_back(innerPiece);

			}


			//make motor constraint between the 2 bodies
			NewtonHingeConstraint* constraint = outerHousingBody->GetNode()->CreateComponent<NewtonHingeConstraint>();

			constraint->SetOtherBody(innerHousingBody);
			
			

			outerPiece->AddAssemblyPiece(innerPiece);
			innerPiece->AddAssemblyPiece(outerPiece);

			innerPiece->SetColorPalletId("Sleeves");
			outerPiece->SetColorPalletId("Housings");











		}
		else
		{
			URHO3D_LOGINFO("no piece with name: " + name + " found");
		}

		//SharedPtr<File> file = SharedPtr<File>(new File(context_));
		//file->Open(name + ".xml", FILE_WRITE);
		//root->SaveXML(*file);
	}


	return superRoot;
}