#pragma once
#include "Urho3D/Scene/Component.h"
#include "Urho3D/Core/Context.h"
#include "Urho3D/Scene/Node.h"
#include "Urho3D/Core/CoreEvents.h"
#include "Character.h"



//tool to be attached to hand node.

class HandTool : public Component {
	URHO3D_OBJECT(HandTool, Component);
public:
	HandTool(Context* context) : Component(context)
	{
		SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(HandTool, HandleUpdate));
	}

	static void RegisterObject(Context* context)
	{
		context->RegisterFactory<HandTool>();
	}

	//if SetPointAtNode is set, the tool will always make node_ face the given node.
	void SetPointAtNode(Node* node) { pointAtNode_ = node; }

	Node* GetPointAtNode() const {
		return pointAtNode_;
	}

	//returns the node used for aiming - in vr this is the tool's node itself. in desktop this is the headnode of the character.
	Node* GetEffectiveLookNode()
	{
		Character* character = node_->GetParentComponent<Character>(true);
		if (character->GetIsVRCharacter())
		{
			return node_;
		}
		else
		{
			return character->headNode_;
		}

	}

	virtual void DrawDebugGeometry(DebugRenderer* debug, bool depthTest) override;

protected:

	void HandleUpdate(StringHash eventType, VariantMap& eventData);
	WeakPtr<Node> pointAtNode_;
};

