#include "ColorPallet.h"
#include "Urho3D/Core/Context.h"
#include "Urho3D/Core/Object.h"
#include "Urho3D/Container/Ptr.h"
#include "Urho3D/Math/StringHash.h"
#include "Urho3D/Math/Color.h"



ColorPallet::ColorPallet(Context* context) : Object(context)
{

}

ColorPallet::~ColorPallet()
{

}

void ColorPallet::RegisterObject(Context* context)
{
	context->RegisterFactory<ColorPallet>();
}

Urho3D::Color ColorPallet::GetColorById(StringHash colorIdentifier)
{
	if (pallet_.contains(colorIdentifier))
		return pallet_[colorIdentifier];
	else
		return Color();
}

void ColorPallet::SetColorById(StringHash colorIdentifier, Color color)
{
	pallet_.insert_or_assign(colorIdentifier, color);
}

void ColorPallet::AutoCalculateFromHue(float hue)
{

	Color  primary(0 / 255.0f, 78 / 255.0f, 106 / 255.0f);
	float primaryHue = primary.ToHSL().x_;
	float primarySat = primary.ToHSL().y_;
	float primaryLight = primary.ToHSL().z_;

	Color  secondary = Color(0, 15 / 255.0f, 149 / 255.0f);
	float secondaryHue = secondary.ToHSL().x_;
	float secondarySat = secondary.ToHSL().y_;
	float secondaryLight = secondary.ToHSL().z_;


	secondaryHue = hue;
	primaryHue = hue;


	Color primaryColor;
	primaryColor.FromHSL(primaryHue, primarySat, primaryLight);
	Color secondaryColor;
	secondaryColor.FromHSL(secondaryHue, secondarySat, secondaryLight);



	SetColorById("RoundRods", Color(0 / 255.0f, 0 / 255.0f, 0 / 255.0f));
	SetColorById("HardRods", Color(83 / 255.0f, 83 / 255.0f, 83 / 255.0f));
	SetColorById("ThickHoles", secondaryColor);
	SetColorById("ThinHoles", primaryColor);
	SetColorById("Sleeves", secondaryColor);
	SetColorById("Caps", secondaryColor);
	SetColorById("Gears", Color(40 / 255.0f, 40 / 255.0f, 40 / 255.0f));
	SetColorById("Housings", Color(50 / 255.0f, 50 / 255.0f, 50 / 255.0f));

}

ColorPalletManager::ColorPalletManager(Context* context) : Object(context)
{
	ColorPallet* defaultPallet = CreateNewPallet("default", Color::BLUE.ToHSL().x_);

	


}

ColorPalletManager::~ColorPalletManager()
{

}

void ColorPalletManager::RegisterObject(Context* context)
{
	context->RegisterFactory<ColorPalletManager>();
}

ColorPallet* ColorPalletManager::CreateNewPallet(ea::string id, float baseColorHue)
{
	SharedPtr<ColorPallet> pallet = context_->CreateObject<ColorPallet>();

	pallet->AutoCalculateFromHue(baseColorHue);

	pallets_.insert_or_assign(StringHash(id), pallet);



	return pallet;
}

ColorPallet* ColorPalletManager::GetPallet(ea::string id)
{
	if (pallets_.contains(StringHash(id)))
		return pallets_[StringHash(id)];
	else
		return nullptr;
}
