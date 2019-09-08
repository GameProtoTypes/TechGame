#pragma once

#include "Urho3D/Urho3DAll.h"

class ColorPallet : public Object {
	URHO3D_OBJECT(ColorPallet, Object);

public:
	ColorPallet(Context* context);
	~ColorPallet();

	static void RegisterObject(Context* context);

	Color GetColorById(StringHash colorIdentifier);

	void SetColorById(StringHash colorIdentifier, Color color);

	void AutoCalculateFromHue(float hue);

	ea::hash_map<StringHash, Color> pallet_;

};

class ColorPalletManager : public Object {
	URHO3D_OBJECT(ColorPalletManager, Object);
public:

	ColorPalletManager(Context* context);

	~ColorPalletManager();

	static void RegisterObject(Context* context);

	ColorPallet* CreateNewPallet( ea::string id, float baseColorHue );



	ColorPallet* GetPallet(ea::string id);


	ea::hash_map<StringHash, SharedPtr<ColorPallet>> pallets_;
};

