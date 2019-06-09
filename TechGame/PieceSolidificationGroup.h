#pragma once
#include <Urho3D/Urho3DAll.h>


//component that represents a group of pieces.  component is on a root node common to all pieces in the group.
class Piece;
class PieceSolidificationGroup : public Component {
	URHO3D_OBJECT(PieceSolidificationGroup, Component);
public:
	PieceSolidificationGroup(Context* context);

	friend class PieceManager;

	static void RegisterObject(Context* context);

	///Set this group as being solidified or not.  if not - the pieces may still be solidified if a parent group is still solid.
	void SetSolidified(bool solid);


	bool GetSolidified() const { return solidStateStack_.back(); }

	void PushSolidState(bool solid)
	{
		solidStateStack_.push_back(solid);
		SetSolidified(solidStateStack_.back());
	}

	bool PopSolidState()
	{
		if (solidStateStack_.size() > 1) {
			solidStateStack_.pop_back();

			SetSolidified(solidStateStack_.back());
			return true;
		}
		else
			return false;
	}



	bool GetEffectivelySolidified() const;





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

	ea::vector<bool> solidStateStack_;


	void HandleUpdate(StringHash event, VariantMap& eventData);
	void HandleNodeAdded(StringHash event, VariantMap& eventData);
	void HandleNodeRemoved(StringHash event, VariantMap& eventData);



	Color debugColor_;
};
