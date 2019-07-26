#include "HandTool.h"
#include "Urho3D/Graphics/DebugRenderer.h"
#include "Urho3D/Math/StringHash.h"
#include "Urho3D/Math/Vector3.h"
#include "Urho3D/Scene/Component.h"





void HandTool::DrawDebugGeometry(DebugRenderer* debug, bool depthTest)
{
	//draw forward vector
	debug->AddLine(node_->GetWorldPosition(), node_->GetWorldPosition() + node_->GetWorldDirection() * 10.0f, Color::GREEN, depthTest);
}

void HandTool::HandleUpdate(StringHash eventType, VariantMap& eventData)
{
	if (pointAtNode_) {
		node_->LookAt(pointAtNode_->GetWorldPosition(), Vector3::UP, TS_WORLD);
	}
}
