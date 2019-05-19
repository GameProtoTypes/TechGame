#pragma once
#include <Urho3D/Urho3DAll.h>


//component that represents a group of pieces.  component is on a root node common to all pieces in the group.
class Piece;
class PieceGroup : public Component {
	URHO3D_OBJECT(PieceGroup, Component);
public:
	PieceGroup(Context* context);

	friend class PieceManager;

	static void RegisterObject(Context* context);

	///Set this group as being solidified or not.  if not - the pieces may still be solidified if a parent group is still solid.
	void SetSolidified(bool solid);

	bool GetSolidified() const { return solidify_; }

	void PushSolidState(bool solid)
	{
		if (isSolidStatePushed_)
			return;

		solidifyPrev_ = solidify_;
		SetSolidified(solid);
		isSolidStatePushed_ = true;
	}

	void PopSolidState()
	{
		if (!isSolidStatePushed_)
			return;

		SetSolidified(solidifyPrev_);
		isSolidStatePushed_ = false;
	}



	bool GetEffectivelySolidified() const { return isEffectivelySolidified_; }





	///Get all pieces part of this group.  only pieces up to "levels" away from the PieceGroup Node are included.  
	///levels == 1 would include only Pieces that are immediate children of the group node.
	///levels == 2 would include immediate children + children from groups that are a child of this group (grandchildren)
	///etc..
	///if singleLevel == true, levels indiates a single level to include pieces from.  so levels == 2 would only return pieces that grandchildren.
	void GetPieces(ea::vector<Piece*>& pieces, int levels = 1, bool singleLevel = false);
	
	

	virtual void DrawDebugGeometry(DebugRenderer* debug, bool depthTest) override;

	void Update();

protected:



	virtual void OnNodeSet(Node* node) override;


	bool solidify_ = false;
	bool solidifyPrev_ = false;
	bool isSolidStatePushed_ = false;

	bool isEffectivelySolidified_ = false;



	void HandleUpdate(StringHash event, VariantMap& eventData);
	void HandleNodeAdded(StringHash event, VariantMap& eventData);
	void HandleNodeRemoved(StringHash event, VariantMap& eventData);



	Color debugColor_;
};
