#include "PhysicsSamplesUtils.h"
#include "Urho3D/Graphics/Material.h"
#include "Urho3D/Graphics/Model.h"
#include "Urho3D/Graphics/StaticModel.h"
#include "Urho3D/Physics/RigidBody.h"
#include "Urho3D/Resource/ResourceCache.h"
#include "NewtonRigidBody.h"
#include "NewtonCollisionShapesDerived.h"
#include "Urho3D/Graphics/Geometry.h"
#include "Urho3D/Replica/NetworkObject.h"
#include "Urho3D/Replica/StaticNetworkObject.h"


void SampleCommonNodeSetup(Node* node)
{
}

Node* SpawnSamplePhysicsSphere(Node* parentNode, const Vector3& worldPosition, float radius)
{
        Node* sphere1 = parentNode->CreateChild("SamplePhysicsSphere");
        SampleCommonNodeSetup(sphere1);

        Node* sphereVis = sphere1->CreateChild();;
        sphereVis->SetScale(Vector3(radius, radius, radius)*2.0f);

        Model* sphereMdl = parentNode->GetSubsystem<ResourceCache>()->GetResource<Model>("Models/Sphere.mdl");
		SharedPtr<Material> sphereMat = SharedPtr<Material>(parentNode->GetSubsystem<ResourceCache>()->GetResource<Material>(SAMPLE_MATERIAL));
		sphereMat->SetShaderParameter("MatDiffColor", SAMPLE_COLOR);
		sphereMat->SetUVTransform(Vector2(), 0.0f, 1.0f/radius);

        StaticModel* sphere1StMdl = sphereVis->CreateComponent<StaticModel>();
        sphere1StMdl->SetCastShadows(true);
        sphere1StMdl->SetModel(sphereMdl);
        sphere1StMdl->SetMaterial(sphereMat);
        NewtonRigidBody* s1RigBody = sphere1->CreateComponent<NewtonRigidBody>();

        NewtonCollisionShape_Sphere* s1ColShape = sphere1->CreateComponent<NewtonCollisionShape_Sphere>();
        s1ColShape->SetScaleFactor(Vector3(radius*2.0f, radius*2.0f, radius*2.0f));

        sphere1->SetWorldPosition(worldPosition);

        s1RigBody->SetMassScale(1.0f);

        

        return sphere1;
}

Node* SpawnSamplePhysicsCylinder(Node* parentNode, const Vector3& worldPosition, float radius, float height)
{
    Node* sphere1 = parentNode->CreateChild("SamplePhysicsCylinder");
    SampleCommonNodeSetup(sphere1);
    Node* sphereVis = sphere1->CreateChild();
    sphereVis->SetScale(Vector3(radius*2.0f, height, radius*2.0f));

    Model* sphereMdl = parentNode->GetSubsystem<ResourceCache>()->GetResource<Model>("Models/Cylinder.mdl");
	SharedPtr<Material> sphereMat = SharedPtr<Material>(parentNode->GetSubsystem<ResourceCache>()->GetResource<Material>(SAMPLE_MATERIAL));
	sphereMat->SetShaderParameter("MatDiffColor", SAMPLE_COLOR);
	sphereMat->SetUVTransform(Vector2(), 0.0f, 1.0f / height);

    StaticModel* sphere1StMdl = sphereVis->CreateComponent<StaticModel>();
    sphere1StMdl->SetCastShadows(true);
    sphere1StMdl->SetModel(sphereMdl);
    sphere1StMdl->SetMaterial(sphereMat);

    NewtonRigidBody* s1RigBody = sphere1->CreateComponent<NewtonRigidBody>();

    NewtonCollisionShape_Cylinder* s1ColShape = sphere1->CreateComponent<NewtonCollisionShape_Cylinder>();
    s1ColShape->SetRadius1(radius);
    s1ColShape->SetRadius2(radius);
    s1ColShape->SetLength(height);

    //s1ColShape->SetScaleFactor(Vector3(height, radius*2, radius*2)*0.25f);

    s1ColShape->SetRotationOffset(Quaternion(0, 0, 90));
    sphere1->SetWorldPosition(worldPosition);

    s1RigBody->SetMassScale(1.0f);


    return sphere1;
}

Node* SpawnSamplePhysicsChamferCylinder(Node* parentNode, const Vector3& worldPosition, float radius, float height)
{
    Node* sphere1 = parentNode->CreateChild("SamplePhysicsCylinder");
    SampleCommonNodeSetup(sphere1);
    Node* sphereVis = sphere1->CreateChild();
    sphereVis->SetScale(Vector3(radius*2.0f, height, radius*2.0f));

    Model* sphereMdl = parentNode->GetSubsystem<ResourceCache>()->GetResource<Model>("Models/Cylinder.mdl");
	SharedPtr<Material> sphereMat = SharedPtr<Material>(parentNode->GetSubsystem<ResourceCache>()->GetResource<Material>(SAMPLE_MATERIAL));;
	sphereMat->SetShaderParameter("MatDiffColor", SAMPLE_COLOR);
	sphereMat->SetUVTransform(Vector2(), 0.0f, 1.0f / height);


    StaticModel* sphere1StMdl = sphereVis->CreateComponent<StaticModel>();
    sphere1StMdl->SetCastShadows(true);
    sphere1StMdl->SetModel(sphereMdl);
    sphere1StMdl->SetMaterial(sphereMat);

    NewtonRigidBody* s1RigBody = sphere1->CreateComponent<NewtonRigidBody>();

    NewtonCollisionShape_ChamferCylinder* s1ColShape = sphere1->CreateComponent<NewtonCollisionShape_ChamferCylinder>();
    s1ColShape->SetRadius(radius);
    s1ColShape->SetLength(height);
    s1ColShape->SetRotationOffset(Quaternion(0, 0, 90));
    sphere1->SetWorldPosition(worldPosition);

    s1RigBody->SetMassScale(1.0f);


    return sphere1;
}

Node* SpawnSamplePhysicsCapsule(Node* parentNode, const Vector3& worldPosition, float radius, float height)
{
    Node* sphere1 = parentNode->CreateChild("SamplePhysicsCylinder");
    SampleCommonNodeSetup(sphere1);
    Node* sphereVis = sphere1->CreateChild();
    sphereVis->SetScale(Vector3(radius*2.0f, height, radius*2.0f));

    Model* sphereMdl = parentNode->GetSubsystem<ResourceCache>()->GetResource<Model>("Models/Cylinder.mdl");
	SharedPtr<Material> sphereMat = SharedPtr<Material>(parentNode->GetSubsystem<ResourceCache>()->GetResource<Material>(SAMPLE_MATERIAL));
	sphereMat->SetShaderParameter("MatDiffColor", SAMPLE_COLOR);
	sphereMat->SetUVTransform(Vector2(), 0.0f, 1.0f / height);

    StaticModel* sphere1StMdl = sphereVis->CreateComponent<StaticModel>();
    sphere1StMdl->SetCastShadows(true);
    sphere1StMdl->SetModel(sphereMdl);
    sphere1StMdl->SetMaterial(sphereMat);

    NewtonRigidBody* s1RigBody = sphere1->CreateComponent<NewtonRigidBody>();

    NewtonCollisionShape_Capsule* s1ColShape = sphere1->CreateComponent<NewtonCollisionShape_Capsule>();
    s1ColShape->SetRadius1(radius);
    s1ColShape->SetRadius2(radius);
    s1ColShape->SetLength(height);
    s1ColShape->SetRotationOffset(Quaternion(0, 0, 90));
    sphere1->SetWorldPosition(worldPosition);

    s1RigBody->SetMassScale(1.0f);


    return sphere1;
}


Node* SpawnSamplePhysicsCone(Node* parentNode, const Vector3& worldPosition, float radius, float height)
{
    Node* sphere1 = parentNode->CreateChild("SamplePhysicsCone");
    SampleCommonNodeSetup(sphere1);
    Node* sphereVis = sphere1->CreateChild();
    sphereVis->SetScale(Vector3(radius*2.0f, height, radius*2.0f));

    Model* sphereMdl = parentNode->GetSubsystem<ResourceCache>()->GetResource<Model>("Models/Cone.mdl");
	SharedPtr<Material> sphereMat = SharedPtr<Material>(parentNode->GetSubsystem<ResourceCache>()->GetResource<Material>(SAMPLE_MATERIAL));
	sphereMat->SetShaderParameter("MatDiffColor", SAMPLE_COLOR);
	sphereMat->SetUVTransform(Vector2(), 0.0f, 1.0f / height);

    StaticModel* sphere1StMdl = sphereVis->CreateComponent<StaticModel>();
    sphere1StMdl->SetCastShadows(true);
    sphere1StMdl->SetModel(sphereMdl);
    sphere1StMdl->SetMaterial(sphereMat);

    NewtonRigidBody* s1RigBody = sphere1->CreateComponent<NewtonRigidBody>();

    NewtonCollisionShape_Cone* s1ColShape = sphere1->CreateComponent<NewtonCollisionShape_Cone>();
    s1ColShape->SetRadius(radius);
    s1ColShape->SetLength(height);
    s1ColShape->SetRotationOffset(Quaternion(0, 0, 90));
    sphere1->SetWorldPosition(worldPosition);

    s1RigBody->SetMassScale(1.0f);


    return sphere1;
}



Node* SpawnSamplePhysicsBox(Node* parentNode, const Vector3& worldPosition, const Vector3& size, Color color)
{
    Node* box = parentNode->CreateChild();
    Node* boxVis = box->CreateChild();
    boxVis->SetScale(size);

    Model* sphereMdl = parentNode->GetSubsystem<ResourceCache>()->GetResource<Model>("Models/Box.mdl");





    SharedPtr<Material> sphereMat = SharedPtr<Material>(parentNode->GetSubsystem<ResourceCache>()->GetResource<Material>(SAMPLE_MATERIAL));
    color.a_ = 0.0f;
	sphereMat->SetShaderParameter("MatDiffColor", color);
   
    auto params = sphereMat->GetShaderParameters();

	sphereMat->SetUVTransform(Vector2(), 0.0f, 1.0f / size.z_);

    StaticModel* sphere1StMdl = boxVis->CreateComponent<StaticModel>();
    sphere1StMdl->SetCastShadows(true);
    sphere1StMdl->SetModel(sphereMdl);
    sphere1StMdl->SetMaterial(sphereMat);



    NewtonRigidBody* s1RigBody = box->CreateComponent<NewtonRigidBody>();

    NewtonCollisionShape_Box* s1ColShape = box->CreateComponent<NewtonCollisionShape_Box>();
    s1ColShape->SetScaleFactor(size);

    box->SetWorldPosition(worldPosition);

    s1RigBody->SetMassScale(1.0f);


    return box;
}

void RebuildToWorldUV(StaticModel* staticModel, float scale)
{
    SharedPtr<Model> newModel = SharedPtr<Model>(staticModel->GetModel()->Clone());
    SharedPtr<Material> material = SharedPtr<Material>(staticModel->GetMaterial());
    //staticModel->SetModel(nullptr);
    int numGeoms = newModel->GetNumGeometries();

    for (int i = 0; i < numGeoms; i++)
    {

        SharedPtr<Geometry> geom = SharedPtr<Geometry>(newModel->GetGeometry(i, 0));

        

        const unsigned char* vertexData;
        const unsigned char* indexData;
        unsigned elementSize, indexSize;
        const  ea::vector<VertexElement>* elements;

        geom->GetRawData(vertexData, elementSize, indexData, indexSize, elements);

        bool hasPosition = VertexBuffer::HasElement(*elements, TYPE_VECTOR3, SEM_POSITION);
        bool hasUV = VertexBuffer::HasElement(*elements, TYPE_VECTOR2, SEM_TEXCOORD);
        bool hasNormal = VertexBuffer::HasElement(*elements, TYPE_VECTOR3, SEM_NORMAL);

        if (vertexData && indexData && hasPosition && hasUV && hasNormal)
        {
            unsigned vertexStart = geom->GetVertexStart();
            unsigned vertexCount = geom->GetVertexCount();
            unsigned indexStart = geom->GetIndexStart();
            unsigned indexCount = geom->GetIndexCount();



            unsigned positionOffset = VertexBuffer::GetElementOffset(*elements, TYPE_VECTOR3, SEM_POSITION);
            unsigned uvOffset = VertexBuffer::GetElementOffset(*elements, TYPE_VECTOR2, SEM_TEXCOORD);
            unsigned normalOffset = VertexBuffer::GetElementOffset(*elements, TYPE_VECTOR3, SEM_NORMAL);


            geom->GetVertexBuffer(i)->Lock(vertexStart, vertexCount);
            //geom->GetIndexBuffer()->Lock(indexStart, indexCount);
           
            Matrix3x4 nodeWorldTransform = staticModel->GetNode()->GetWorldTransform();

            for (unsigned curIdx = indexStart; curIdx < indexStart + indexCount; curIdx += 3)
            {
                //get indexes
                unsigned i1, i2, i3;
                if (indexSize == 2) {
                    i1 = *reinterpret_cast<const unsigned short*>(indexData + curIdx * indexSize);
                    i2 = *reinterpret_cast<const unsigned short*>(indexData + (curIdx + 1) * indexSize);
                    i3 = *reinterpret_cast<const unsigned short*>(indexData + (curIdx + 2) * indexSize);
                }
                else if (indexSize == 4)
                {
                    i1 = *reinterpret_cast<const unsigned*>(indexData + curIdx * indexSize);
                    i2 = *reinterpret_cast<const unsigned*>(indexData + (curIdx + 1) * indexSize);
                    i3 = *reinterpret_cast<const unsigned*>(indexData + (curIdx + 2) * indexSize);
                }

                //lookup triangle using indexes.
                Vector3* v1 = reinterpret_cast<Vector3*>((unsigned char*)vertexData + i1 * elementSize + positionOffset);
                Vector3* v2 = reinterpret_cast<Vector3*>((unsigned char*)vertexData + i2 * elementSize + positionOffset);
                Vector3* v3 = reinterpret_cast<Vector3*>((unsigned char*)vertexData + i3 * elementSize + positionOffset);

                //lookup UV using indexes.
                Vector2* uv1 = reinterpret_cast<Vector2*>((unsigned char*)vertexData + i1 * elementSize + uvOffset);
                Vector2* uv2 = reinterpret_cast<Vector2*>((unsigned char*)vertexData + i2 * elementSize + uvOffset);
                Vector2* uv3 = reinterpret_cast<Vector2*>((unsigned char*)vertexData + i3 * elementSize + uvOffset);

                //lookup normal using indexes.
                Vector3* norm1 = reinterpret_cast<Vector3*>((unsigned char*)vertexData + i1 * elementSize + normalOffset);
                Vector3* norm2 = reinterpret_cast<Vector3*>((unsigned char*)vertexData + i2 * elementSize + normalOffset);
                Vector3* norm3 = reinterpret_cast<Vector3*>((unsigned char*)vertexData + i3 * elementSize + normalOffset);

               
               

                float uscale = scale;
                float vscale = scale;

                Vector3 v1World = nodeWorldTransform * *v1;
                Vector3 v2World = nodeWorldTransform * *v2;
                Vector3 v3World = nodeWorldTransform * *v3;
               
                Vector3 norm1World = nodeWorldTransform.RotationMatrix() * (*norm1);
                Vector3 norm2World = nodeWorldTransform.RotationMatrix() * (*norm2);
                Vector3 norm3World = nodeWorldTransform.RotationMatrix() * (*norm3);

                Quaternion delta;
                delta.FromRotationTo(norm1World, Vector3::UP);
                delta.RotationMatrix();

                Vector3 v1UpPlane = delta.RotationMatrix() * v1World;
                Vector3 v2UpPlane = delta.RotationMatrix() * v2World;
                Vector3 v3UpPlane = delta.RotationMatrix() * v3World;
                

                *uv1 = Vector2(v1UpPlane.x_, v1UpPlane.z_)* scale;
                *uv2 = Vector2(v2UpPlane.x_, v2UpPlane.z_)* scale;
                *uv3 = Vector2(v3UpPlane.x_, v3UpPlane.z_)* scale;

            }
        }
        geom->GetVertexBuffer(i)->Unlock();
        //geom->GetIndexBuffer()->Unlock();
    }
    
    staticModel->SetModel(newModel);
    staticModel->SetMaterial(material);

}




