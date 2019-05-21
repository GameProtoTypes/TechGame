#pragma once


#include "Urho3D/Urho3DAll.h"

class PiecePoint;
class PiecePointRow;
class Piece;
class ContraptionAttachmentMonitor : public Object
{
	URHO3D_OBJECT(ContraptionAttachmentMonitor, Object);

	ContraptionAttachmentMonitor(Context* context) : Object(context)
	{

	}
	static void RegisterObject(Context* context)
	{
		context->RegisterFactory<ContraptionAttachmentMonitor>();
	}

	//sets the contraption that the monitor will start comparing to other pieces in the scene.
	void SetActiveContraption(Piece* piece) {}

	void SetActiveContraption(ea::vector<Piece*>& pieces) {}

	//return true if there are no collisions between the contraption and other pieces in the scene. There must also be at least one attachment point.
	bool PollAttachable() { return false; }

	bool AttemptAttachement() { return false; }




};