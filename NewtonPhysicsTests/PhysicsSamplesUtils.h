#pragma once
#include "Urho3D/Scene/Node.h"
#include "Urho3D/Math/Vector3.h"

class Urho3D::Node;
class Urho3D::Vector3;
using namespace Urho3D;

#define SAMPLE_MATERIAL "Materials/Metal.xml"
#define SAMPLE_COLOR (Vector4(0.5, 0.5f, 0.5f, 1.0f))


Node* SpawnSamplePhysicsCylinder(Node* parentNode, const Vector3& worldPosition, float radius = 0.5f, float height = 1.0f);
Node* SpawnSamplePhysicsChamferCylinder(Node* parentNode, const Vector3& worldPosition, float radius = 0.5f, float height = 1.0f);
Node* SpawnSamplePhysicsCapsule(Node* parentNode, const Vector3& worldPosition, float radius = 0.5f, float height = 1.0f);
Node* SpawnSamplePhysicsCone(Node* parentNode, const Vector3& worldPosition, float radius = 0.5f, float height = 1.0f);
Node* SpawnSamplePhysicsSphere(Node* parentNode, const Vector3& worldPosition, float radius = 0.5f);
Node* SpawnSamplePhysicsBox(Node* parentNode, const Vector3& worldPosition, const Vector3& size, Color color = Color(SAMPLE_COLOR));




