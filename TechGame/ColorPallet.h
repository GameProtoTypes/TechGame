#pragma once
#include "Urho3D/Core/Context.h"
#include "Urho3D/Core/Object.h"
#include "Urho3D/Math/Color.h"
#include "Urho3D/Math/StringHash.h"
#include "Urho3D/Container/Ptr.h"

class ColorPallet : public Object {
	URHO3D_OBJECT(ColorPallet, Object);

public:
	ColorPallet(Context* context) : Object(context) {

	}
	~ColorPallet() {}

	static void RegisterObject(Context* context) { context->RegisterFactory<ColorPallet>(); }

	Color GetColorById(unsigned colorIdentifier) { 
		if (pallet_.contains(colorIdentifier))
			return pallet_[colorIdentifier];
		else
			return Color();
	}

	void SetColorById(unsigned colorIdentifier, Color color) {
		pallet_.insert_or_assign(colorIdentifier, color);
	}

	ea::hash_map<unsigned, Color> pallet_;

};

class ColorPalletManager : public Object {
	URHO3D_OBJECT(ColorPalletManager, Object);
public:

	ColorPalletManager(Context* context) : Object(context) {



		ColorPallet* defaultPallet = CreateNewPallet("default");

		for (int i = 0; i < 16; i++) {
			Color color;
			float colorBaseHue = Color::BLUE.Hue();
			float colorRange = 1.0f;
			color.FromHSL(Wrap<float>(colorBaseHue + Random(-0.5f, 0.5f)*colorRange, 0.0f, 1.0f), 0.75f, 0.3f);

			defaultPallet->SetColorById(i, color);

		}
		


	}

	~ColorPalletManager() {}

	static void RegisterObject(Context* context) { context->RegisterFactory<ColorPalletManager>(); }

	ColorPallet* CreateNewPallet( ea::string id ) {
		SharedPtr<ColorPallet> pallet = context_->CreateObject<ColorPallet>();

		pallets_.insert_or_assign(StringHash(id), pallet);
		return pallet;
	}
	ColorPallet* GetPallet(ea::string id) {
		if(pallets_.contains(StringHash(id)))
			return pallets_[StringHash(id)];
		else
			return nullptr;
	}


	ea::hash_map<StringHash, SharedPtr<ColorPallet>> pallets_;
};

