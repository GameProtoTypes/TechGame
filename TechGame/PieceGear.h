#pragma once

#include <Urho3D/Urho3DAll.h>
#include "PieceManager.h"

#define PIECEGEAR_REFRESH_CNT 100

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


	void HandleUpdate(StringHash event, VariantMap& eventData);

	void ReEvalConstraints();

protected:

	int refreshCounter_ = PIECEGEAR_REFRESH_CNT;
	float radius_ = 1.0f;

	virtual void OnNodeSet(Node* node) override;

};