


#include "VisualDebugger.h"
#include "Urho3D/Graphics/DebugRenderer.h"
#include "Urho3D/Graphics/Camera.h"
#include "Urho3D/Graphics/Graphics.h"
#include "Urho3D/Core/Context.h"
#include "Urho3D/Core/Timer.h"

#include "Urho3D/UI/UI.h"
#include "Urho3D/UI/Text.h"
#include "Urho3D/IO/Log.h"
#include "Urho3D/UI/Font.h"
#include "Urho3D/Resource/ResourceCache.h"



namespace Urho3D
{

VisualDebugger::VisualDebugger(Context* context) : Object(context)
{
	mTimer.Reset();
}

void VisualDebugger::RegisterObject(Context* context)
{
	context->RegisterFactory<VisualDebugger>();
}

VisualDebugger::VisualDebuggerCircle* VisualDebugger::AddCircle(const Vector3& center, const Vector3& normal, float radius, const Color& color, int steps /*= 64*/, bool depthTest /*= true*/)
{
	VisualDebuggerCircle* newDbgObject = new VisualDebuggerCircle(this, context_);
	newDbgObject->SetCenter(center);
	newDbgObject->SetNormal(normal);
	newDbgObject->SetRadius(radius);
	newDbgObject->SetSteps(steps);
	SetupAndAddObjectToList(newDbgObject, depthTest, color);
	return newDbgObject;
}

VisualDebugger::VisualDebuggerLine* VisualDebugger::AddLine(const Vector3& start, const Vector3& end, const Color& color, bool depthTest /*= true*/)
{
	VisualDebuggerLine* newDbgObject = new VisualDebuggerLine(this, context_);
	newDbgObject->SetStart(start);
	newDbgObject->SetEnd(end);
	SetupAndAddObjectToList(newDbgObject, depthTest, color);
	return newDbgObject;
}

Urho3D::VisualDebugger::VisualDebuggerRay* VisualDebugger::AddRay(const Ray& ray, const Color& color, bool depthTest /*= true*/)
{
	VisualDebuggerRay* newDbgObject = new VisualDebuggerRay(this, context_);
	newDbgObject->SetRay(ray);
	SetupAndAddObjectToList(newDbgObject, depthTest, color);
	return newDbgObject;
}

VisualDebugger::VisualDebuggerOrb* VisualDebugger::AddOrb(const Vector3& center, const float& radius, const Color& color, int circleSteps /*= 32*/, int numCircles /*= 10*/, bool depthTest /*= true*/)
{
	VisualDebuggerOrb* newDbgObject = new VisualDebuggerOrb(this, context_);
	newDbgObject->SetCenter(center);
	newDbgObject->SetRadius(radius);
	newDbgObject->SetSteps(circleSteps);
	newDbgObject->SetNumCircles(numCircles);
	SetupAndAddObjectToList(newDbgObject, depthTest, color);
	return newDbgObject;
}

VisualDebugger::VisualDebuggerUILabel* VisualDebugger::AddLabel(const Vector3& center, ea::string text, Color color /*= Color::WHITE*/)
{
	VisualDebuggerUILabel* newDbgObject = new VisualDebuggerUILabel(this, context_);
	newDbgObject->SetCenter(center);
	newDbgObject->SetText(text);
	SetupAndAddObjectToList(newDbgObject, true, color);
	return newDbgObject;
}

VisualDebugger::VisualDebuggerNode* VisualDebugger::AddNode(Node* node, const float& scale, bool depthTest)
{
	VisualDebuggerNode* newDbgObject = new VisualDebuggerNode(this, context_);
	newDbgObject->SetNode(WeakPtr<Node>(node));
	newDbgObject->SetScale(scale);
	SetupAndAddObjectToList(newDbgObject, depthTest, Color::WHITE);
	return newDbgObject;
}

VisualDebugger::VisualDebuggerCross* VisualDebugger::AddCross(const Vector3& center, const float& size, Color color, bool depthTest)
{
	VisualDebuggerCross* newDbgObject = new VisualDebuggerCross(this, context_);
	newDbgObject->SetCenter(center);
	newDbgObject->SetSize(size);
	SetupAndAddObjectToList(newDbgObject, depthTest, color);
	return newDbgObject;
}

VisualDebugger::VisualDebuggerFrame* VisualDebugger::AddFrame(const Matrix3x4& frame, const float& size, bool depthTest)
{
    VisualDebuggerFrame* newDbgObject = new VisualDebuggerFrame(this, context_);
    newDbgObject->SetFrame(frame);
    newDbgObject->SetScale(size);
    SetupAndAddObjectToList(newDbgObject, depthTest, Color::RED);
    return newDbgObject;
}


VisualDebugger::VisualDebuggerTriangle* VisualDebugger::AddTriangle(Vector3 v1, Vector3 v2, Vector3 v3, Color color, bool depthTest)
{
	VisualDebuggerTriangle* newDbgObject = new VisualDebuggerTriangle(this, context_);
	newDbgObject->v1_ = v1;
	newDbgObject->v2_ = v2;
	newDbgObject->v3_ = v3;
	SetupAndAddObjectToList(newDbgObject, depthTest, color);
	return newDbgObject;
}

VisualDebugger::VisualDebuggerBoundingBox* VisualDebugger::AddBoundingBox(BoundingBox boundingBox, Color color, bool depthTest)
{
	VisualDebuggerBoundingBox* newDbgObject = new VisualDebuggerBoundingBox(this, context_);
	newDbgObject->SetBox(boundingBox);
	SetupAndAddObjectToList(newDbgObject, depthTest, color);
	return newDbgObject;
}

VisualDebugger::VisualDebuggerPolygon* VisualDebugger::AddPolygon(Vector3 v1, Vector3 v2, Vector3 v3, Vector3 v4, Color color, bool depthTest)
{
	VisualDebuggerPolygon* newDbgObject = new VisualDebuggerPolygon(this, context_);
	newDbgObject->v1_ = v1;
	newDbgObject->v2_ = v2;
	newDbgObject->v3_ = v3;
	newDbgObject->v4_ = v4;
	SetupAndAddObjectToList(newDbgObject, depthTest, color);
	return newDbgObject;
}

VisualDebugger::VisualDebuggerPolyhedron* VisualDebugger::AddPolyhedron(Polyhedron polyhedron, Color color, bool depthTest)
{
	VisualDebuggerPolyhedron* newDbgObject = new VisualDebuggerPolyhedron(this, context_);
	newDbgObject->SetPolyhedron(polyhedron);
	SetupAndAddObjectToList(newDbgObject, depthTest, color);
	return newDbgObject;
}

VisualDebugger::VisualDebuggerCylinder* VisualDebugger::AddCylinder(Vector3 position, float radius, float height, Color color, bool depthTest)
{
	VisualDebuggerCylinder* newDbgObject = new VisualDebuggerCylinder(this, context_);
	newDbgObject->SetPosition(position);
	newDbgObject->SetRadius(radius);
	newDbgObject->SetHeight(height);
	SetupAndAddObjectToList(newDbgObject, depthTest, color);
	return newDbgObject;
}

VisualDebugger::VisualDebuggerFrustum* VisualDebugger::AddFrustum(Frustum frustum, Color color, bool depthTest)
{
	VisualDebuggerFrustum* newDbgObject = new VisualDebuggerFrustum(this, context_);
	newDbgObject->SetFrustum(frustum);
	SetupAndAddObjectToList(newDbgObject, depthTest, color);
	return newDbgObject;
}

VisualDebugger::VisualDebuggerQuad* VisualDebugger::AddQuad(Vector3 center, float width, float height, Color color, bool depthTest)
{
	VisualDebuggerQuad* newDbgObject = new VisualDebuggerQuad(this, context_);
	newDbgObject->SetCenter(center);
	newDbgObject->SetWidth(width);
	newDbgObject->SetHeight(height);
	SetupAndAddObjectToList(newDbgObject, depthTest, color);
	return newDbgObject;
}

VisualDebugger::VisualDebuggerSphere* VisualDebugger::AddSphere(Sphere sphere, Color color, bool depthTest)
{
	VisualDebuggerSphere* newDbgObject = new VisualDebuggerSphere(this, context_);
	newDbgObject->SetSphere(sphere);
	SetupAndAddObjectToList(newDbgObject, depthTest, color);
	return newDbgObject;
}

VisualDebugger::VisualDebuggerSphereSector* VisualDebugger::AddSphereSector(Sphere sphere, Quaternion rotation, float angle, bool drawLines, Color color, bool depthTest)
{
	VisualDebuggerSphereSector* newDbgObject = new VisualDebuggerSphereSector(this, context_);
	newDbgObject->SetSphere(sphere);
	newDbgObject->SetRotation(rotation);
	newDbgObject->SetAngle(angle);
	newDbgObject->SetDrawLines(drawLines);
	SetupAndAddObjectToList(newDbgObject, depthTest, color);
	return newDbgObject;
}

void VisualDebugger::DrawDebugGeometry(DebugRenderer* debugRenderer, unsigned int maxTimeMs)
{
	if (!mEnabled)
		return;

	Timer timer;
	unsigned int startTimeMs = timer.GetMSec(false);
	auto i = mDebuggerObjects.begin();
	unsigned int drawCount = 0;
	for (int i = 0; i < mDebuggerObjects.size(); i++)
	{
		SharedPtr<VisualDebuggerObject> debObject = mDebuggerObjects[i];

		if (timer.GetMSec(false) >= (startTimeMs + maxTimeMs))
			return;

		if (drawCount >= mMaxRenderObjects)
			return;

		debObject->DrawDebugGeometry(debugRenderer);
		drawCount++;

		//check if the object has exceeded its lifetime and if so remove.
		if ((debObject->creationTimeMS_ + debObject->lifetimeMS_) <= mTimer.GetMSec(false)) {
			debObject->TearDown();
			mDebuggerObjects.erase_at(i);
		}

		i++;
	}
}

void VisualDebugger::SetEnabled(bool enabled)
{
	if (mEnabled == enabled)
		return;

	for(SharedPtr<VisualDebuggerObject> obj : mDebuggerObjects) {
		obj->SetEnabled(enabled);
	}
	mEnabled = enabled;
}

void VisualDebugger::SetObjectLifeTimeMs(unsigned int lifeTimeMs)
{
	mDefaultLifetimeMs = lifeTimeMs;
}

void VisualDebugger::SetPrimaryCamera(Camera* camera)
{
	mCamera = camera;
}

void VisualDebugger::SetMaxRenderObjects(unsigned int maxObjects /*= UINT_MAX*/)
{
	mMaxRenderObjects = maxObjects;
}

void VisualDebugger::SetupAndAddObjectToList(VisualDebuggerObject* object, bool depthTest, Color color)
{
	mDebuggerObjects.push_front(SharedPtr<VisualDebuggerObject>(object));
	object->creationTimeMS_ = mTimer.GetMSec(false);
	object->depthTest_ = depthTest;
	object->color_ = color;
	object->lifetimeMS_ = mDefaultLifetimeMs;
	object->Setup();
}



VisualDebugger::VisualDebuggerUILabel::VisualDebuggerUILabel(VisualDebugger* visDebugger, Context* context_) : VisualDebuggerObject(visDebugger, context_)
{

}

VisualDebugger::VisualDebuggerUILabel::~VisualDebuggerUILabel()
{

}

void VisualDebugger::VisualDebuggerUILabel::DrawDebugGeometry(DebugRenderer* debugRenderer)
{
	VisualDebuggerObject::DrawDebugGeometry(debugRenderer);
	UpdatePosition();
}

void VisualDebugger::VisualDebuggerUILabel::Setup()
{
	VisualDebuggerObject::Setup();
	mUIText = context_->GetSubsystem<UI>()->GetRoot()->CreateChild<Text>();
	mUIText->SetText(mText);
	mUIText->SetFont(GetSubsystem<ResourceCache>()->GetResource<Font>("Fonts/BlueHighway.ttf"));
	mUIText->SetColor(color_);
	mUIText->SetEnabled(enabled_);
	UpdatePosition();

	if (visDebugger_->mCamera == nullptr) {
		URHO3D_LOGWARNING("VisualDebugger: No Active Camera Set! (Needed for UILabel)");
	}
		
}

void VisualDebugger::VisualDebuggerUILabel::TearDown()
{
	mUIText->Remove();
	mUIText.Detach();
}

void VisualDebugger::VisualDebuggerUILabel::SetEnabled(bool enabled)
{
	VisualDebuggerObject::SetEnabled(enabled);
	mUIText->SetVisible(enabled_);
}

void VisualDebugger::VisualDebuggerUILabel::UpdatePosition()
{
	//default to screen middle.
	Vector2 screenPoint = Vector2(GetSubsystem<Graphics>()->GetSize())*0.5f;
	
	if (!visDebugger_->mCamera.Expired()) {
		screenPoint = visDebugger_->mCamera->WorldToScreenPoint(GetCenter());
		//screen point has range of 0-1. - convert back to pixels
		screenPoint *= Vector2(GetSubsystem<Graphics>()->GetSize());
	}


	mUIText->SetPosition(IntVector2(screenPoint.x_, screenPoint.y_));
}


VisualDebugger::VisualDebuggerNode::VisualDebuggerNode(VisualDebugger* visDebugger, Context* context_) : VisualDebuggerObject(visDebugger, context_)
{

}

void VisualDebugger::VisualDebuggerNode::DrawDebugGeometry(DebugRenderer* debugRenderer)
{
	VisualDebuggerObject::DrawDebugGeometry(debugRenderer);
	debugRenderer->AddFrame(node_->GetWorldTransform(), 1.0f, Color::RED, Color::GREEN, Color::BLUE, depthTest_);
}

VisualDebugger::VisualDebuggerOrb::VisualDebuggerOrb(VisualDebugger* visDebugger, Context* context_) : VisualDebuggerObject(visDebugger, context_)
{

}

void VisualDebugger::VisualDebuggerOrb::DrawDebugGeometry(DebugRenderer* debugRenderer)
{
	VisualDebuggerObject::DrawDebugGeometry(debugRenderer);
	for (auto i = 0; i < GetNumCircles(); i++) {
		debugRenderer->AddCircle(mCenter, Vector3(Random(-1, 1), Random(-1, 1), Random(-1, 1)).Normalized(), GetRadius(), color_, GetSteps(), depthTest_);
	}
}


VisualDebugger::VisualDebuggerSphereSector::VisualDebuggerSphereSector(VisualDebugger* visDebugger, Context* context_) : VisualDebuggerObject(visDebugger, context_)
{

}

void VisualDebugger::VisualDebuggerSphereSector::DrawDebugGeometry(DebugRenderer* debugRenderer)
{
	VisualDebuggerObject::DrawDebugGeometry(debugRenderer);
	debugRenderer->AddSphereSector(GetSphere(), GetRotation(), GetAngle(), GetDrawLines(), color_, depthTest_);
}

VisualDebugger::VisualDebuggerSphere::VisualDebuggerSphere(VisualDebugger* visDebugger, Context* context_) : VisualDebuggerObject(visDebugger, context_)
{

}

void VisualDebugger::VisualDebuggerSphere::DrawDebugGeometry(DebugRenderer* debugRenderer)
{
	VisualDebuggerObject::DrawDebugGeometry(debugRenderer);
	debugRenderer->AddSphere(GetSphere(), color_, depthTest_);
}

VisualDebugger::VisualDebuggerQuad::VisualDebuggerQuad(VisualDebugger* visDebugger, Context* context_) : VisualDebuggerObject(visDebugger, context_)
{

}

void VisualDebugger::VisualDebuggerQuad::DrawDebugGeometry(DebugRenderer* debugRenderer)
{
	VisualDebuggerObject::DrawDebugGeometry(debugRenderer);
	debugRenderer->AddQuad(GetCenter(), GetWidth(), GetHeight(), color_, depthTest_);
}

VisualDebugger::VisualDebuggerFrustum::VisualDebuggerFrustum(VisualDebugger* visDebugger, Context* context_) : VisualDebuggerObject(visDebugger, context_)
{

}

void VisualDebugger::VisualDebuggerFrustum::DrawDebugGeometry(DebugRenderer* debugRenderer)
{
	VisualDebuggerObject::DrawDebugGeometry(debugRenderer);
	debugRenderer->AddFrustum(GetFrustum(), color_, depthTest_);
}

VisualDebugger::VisualDebuggerCylinder::VisualDebuggerCylinder(VisualDebugger* visDebugger, Context* context_) : VisualDebuggerObject(visDebugger, context_)
{

}

void VisualDebugger::VisualDebuggerCylinder::DrawDebugGeometry(DebugRenderer* debugRenderer)
{
	VisualDebuggerObject::DrawDebugGeometry(debugRenderer);
	debugRenderer->AddCylinder(GetPosition(), GetRadius(), GetHeight(), color_, depthTest_);
}


VisualDebugger::VisualDebuggerPolyhedron::VisualDebuggerPolyhedron(VisualDebugger* visDebugger, Context* context_) : VisualDebuggerObject(visDebugger, context_)
{

}

void VisualDebugger::VisualDebuggerPolyhedron::DrawDebugGeometry(DebugRenderer* debugRenderer)
{
	VisualDebuggerObject::DrawDebugGeometry(debugRenderer);
	debugRenderer->AddPolyhedron(GetPolyhedron(), color_, depthTest_);
}


VisualDebugger::VisualDebuggerPolygon::VisualDebuggerPolygon(VisualDebugger* visDebugger, Context* context_) : VisualDebuggerObject(visDebugger, context_)
{

}

void VisualDebugger::VisualDebuggerPolygon::DrawDebugGeometry(DebugRenderer* debugRenderer)
{
	VisualDebuggerObject::DrawDebugGeometry(debugRenderer);
	debugRenderer->AddPolygon(v1_, v2_, v3_, v4_, color_, depthTest_);
}


VisualDebugger::VisualDebuggerCross::VisualDebuggerCross(VisualDebugger* visDebugger, Context* context_) : VisualDebuggerObject(visDebugger, context_)
{

}

void VisualDebugger::VisualDebuggerCross::DrawDebugGeometry(DebugRenderer* debugRenderer)
{
	VisualDebuggerObject::DrawDebugGeometry(debugRenderer);
	debugRenderer->AddCross(GetCenter(), GetSize(), color_, depthTest_);
}


VisualDebugger::VisualDebuggerTriangle::VisualDebuggerTriangle(VisualDebugger* visDebugger, Context* context_) : VisualDebuggerObject(visDebugger, context_)
{

}

void VisualDebugger::VisualDebuggerTriangle::DrawDebugGeometry(DebugRenderer* debugRenderer)
{
	VisualDebuggerObject::DrawDebugGeometry(debugRenderer);
	debugRenderer->AddTriangle(v1_, v2_, v3_, color_, depthTest_);
}


VisualDebugger::VisualDebuggerBoundingBox::VisualDebuggerBoundingBox(VisualDebugger* visDebugger, Context* context_) : VisualDebuggerObject(visDebugger, context_)
{

}

void VisualDebugger::VisualDebuggerBoundingBox::DrawDebugGeometry(DebugRenderer* debugRenderer)
{
	VisualDebuggerObject::DrawDebugGeometry(debugRenderer);
	debugRenderer->AddBoundingBox(GetBox(), color_, depthTest_, GetSolid());
}

VisualDebugger::VisualDebuggerLine::VisualDebuggerLine(VisualDebugger* visDebugger, Context* context_) : VisualDebuggerObject(visDebugger, context_)
{

}

void VisualDebugger::VisualDebuggerLine::DrawDebugGeometry(DebugRenderer* debugRenderer)
{
	VisualDebuggerObject::DrawDebugGeometry(debugRenderer);
	debugRenderer->AddLine(GetStart(), GetEnd(), color_, depthTest_);
}


VisualDebugger::VisualDebuggerCircle::VisualDebuggerCircle(VisualDebugger* visDebugger, Context* context_) : VisualDebuggerObject(visDebugger, context_)
{

}

void VisualDebugger::VisualDebuggerCircle::DrawDebugGeometry(DebugRenderer* debugRenderer)
{
	VisualDebuggerObject::DrawDebugGeometry(debugRenderer);
	debugRenderer->AddCircle(GetCenter(), GetNormal(), GetRadius(), color_, GetSteps(), depthTest_);
}

VisualDebugger::VisualDebuggerObject::VisualDebuggerObject(VisualDebugger* visDebugger, Context* context_) : Object(context_), visDebugger_(visDebugger)
{
}

void VisualDebugger::VisualDebuggerObject::DrawDebugGeometry(DebugRenderer* debugRenderer)
{

}

void VisualDebugger::VisualDebuggerObject::Setup()
{

}

void VisualDebugger::VisualDebuggerObject::TearDown()
{

}

void VisualDebugger::VisualDebuggerObject::SetEnabled(bool enabled)
{
	enabled_ = enabled;
}

void VisualDebugger::VisualDebuggerObject::SetLifeTimeMs(unsigned int lifeTimeMs)
{
	lifetimeMS_ = lifeTimeMs;
}

void VisualDebugger::VisualDebuggerObject::SetColor(Color color)
{
	color_ = color;
}

VisualDebugger::VisualDebuggerRay::VisualDebuggerRay(VisualDebugger* visDebugger, Context* context_) : VisualDebuggerObject(visDebugger, context_)
{

}

void VisualDebugger::VisualDebuggerRay::DrawDebugGeometry(DebugRenderer* debugRenderer)
{
	VisualDebuggerObject::DrawDebugGeometry(debugRenderer);
	debugRenderer->AddLine(GetRay().origin_, GetRay().origin_ + GetRay().direction_, color_, depthTest_);
}

VisualDebugger::VisualDebuggerFrame::VisualDebuggerFrame(VisualDebugger* visDebugger, Context* context_) : VisualDebuggerObject(visDebugger, context_)
{

}

void VisualDebugger::VisualDebuggerFrame::DrawDebugGeometry(DebugRenderer* debugRenderer)
{
    VisualDebuggerObject::DrawDebugGeometry(debugRenderer);
    debugRenderer->AddFrame(frame_, scale_, Color::RED, Color::GREEN, Color::BLUE, depthTest_);
}


}
