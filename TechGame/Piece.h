#pragma once

#include <Urho3D/Urho3DAll.h>
#include "Urho3D/IO/Log.h"

#include "NewtonPhysicsWorld.h"
#include "NewtonRigidBody.h"


#include "PieceSolidificationGroup.h"
#include "NodeTools.h"



class PiecePoint;
class PiecePointRow;
class PieceSolidificationGroup;

#define PIECE_ATTRIB_PRIMARY_COLOR "Primary Color"
#define PIECE_ATTRIB_PRIMARY_GHOST "Ghosting Effect"
#define PIECE_ATTRIB_PRIMARY_DYNAMIC_ATTACH "Dynamic Detachement"



class Piece : public Component {
	URHO3D_OBJECT(Piece, Component);
public:
	friend class PieceSolidificationGroup;

	Piece(Context* context);

	static void RegisterObject(Context* context);

	//returns the rigid body that is attached to the same node.
	NewtonRigidBody* GetRigidBody() {
		return node_->GetComponent<NewtonRigidBody>();
	}

	StaticModel* GetStaticModel() {
		return node_->GetComponent<StaticModel>(true);
	}

	void GetPointRows(ea::vector<PiecePointRow*>& rows);

	void GetAttachedRows(ea::vector<PiecePointRow*>& rows);

	/// Get Points, Does not clear points vector
	void GetPoints(ea::vector<PiecePoint*>& points);

	///get attached pieces, not including this piece.
	void GetAttachedPieces(ea::vector<Piece*>& pieces, bool recursive);

	Node* GetVisualNode() { return node_->GetChild("visualNode"); }


	bool IsPartOfAssembly() const { return assemblyPieces_.size(); }

	void GetAssemblyPieces(ea::vector<Piece*>& pieces, bool includeThisPiece = true);

	bool AddAssemblyPiece(Piece* piece);

	void SetPrimaryColor(Color color);
	Color GetPrimaryColor() const { return primaryColor_; }

	void SetColorPalletId(unsigned id) { 
		if (id != colorPalletId_) {
			colorPalletId_ = id;
			RefreshVisualMaterial();
			useColorPallet_ = true;
		}
	}

	void SetEnableDynamicDetachmentAttrib(bool enable);
	void SetEnableDynamicDetachment(bool enable);
	bool GetEnableDynamicDetachment() const { return enableDynamicDetachment_; }

	void SetGhostingEffectEnabled(bool enable);
	bool GetGhostingEffectEnabled() const { return ghostingEffectOn_; }


	void RefreshVisualMaterial();

	void DetachAll();

	bool IsEffectivelySolidified();


	PieceSolidificationGroup* GetPieceGroup()
	{
		return node_->GetParent()->GetComponent<PieceSolidificationGroup>();
	}

	//returns the rigid body that is enabled and is actually controlling this rigid body
	NewtonRigidBody* GetEffectiveRigidBody();

	bool IsPartOfPieceGroup(PieceSolidificationGroup* group);

	


	virtual void ApplyAttributes() override;



protected:



	bool ghostingEffectOn_ = false;

	Color primaryColor_;
	unsigned colorPalletId_ = 0;
	bool useColorPallet_ = true;
	bool visualsDirty_ = false;

	Matrix3x4 lastRigBodyTransform_;

	bool enableDynamicDetachment_ = true;



	ea::vector<Piece*> assemblyPieces_;


	virtual void OnNodeSet(Node* node) override;

	void GetAttachedPiecesRec(ea::vector<Piece*>& pieces, bool recursive);

	void HandleUpdate(StringHash eventType, VariantMap& eventData);
	void HandlePhysicsPostStep(StringHash eventType, VariantMap& eventData);

	void HandleNodeCollisionStart(StringHash eventType, VariantMap& eventData);
	void HandleNodeCollisionEnd(StringHash eventType, VariantMap& eventData);
};







