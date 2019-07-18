#pragma once

#include <Urho3D/Urho3DAll.h>
#include "PieceManager.h"

#define PIECEGEAR_REFRESH_CNT 20

class Piece;
class PieceGear : public Component
{
	URHO3D_OBJECT(PieceGear, Component);

public:


	PieceGear(Context* context);

	static void RegisterObject(Context* context);

	void SetRadius(float radius) {
		radius_ = radius;
	}
	float GetRadius() const { return radius_; }

	void SetNormal(Vector3 normal) { normal_ = normal; }
	Vector3 GetNormal() const { return normal_; }

	Vector3 GetWorldNormal() const { return node_->GetWorldRotation() * normal_; }

	void HandleUpdate(StringHash event, VariantMap& eventData);

	void ReEvalConstraints();


	virtual void DrawDebugGeometry(DebugRenderer* debug, bool depthTest) override;


protected:

	int refreshCounter_ = PIECEGEAR_REFRESH_CNT;
	float radius_ = 1.0f;

	Vector3 normal_ = Vector3::FORWARD;

	virtual void OnNodeSet(Node* node) override;

};