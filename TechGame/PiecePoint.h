#pragma once

#include <Urho3D/Urho3DAll.h>



class Piece;
class PiecePointRow;
class PiecePoint : public Component
{
	URHO3D_OBJECT(PiecePoint, Component);
public:


	PiecePoint(Context* context);

	static void RegisterObject(Context* context);


	virtual void DrawDebugGeometry(DebugRenderer* debug, bool depthTest) override;

	virtual bool OccupancyCompatible(PiecePoint* point);


	bool IsOccupied() { if (occupiedPoint_) return true; else return false; }

	Piece* GetPiece();

	Vector3 GetDirectionWorld() { return GetNode()->GetWorldRotation().RotationMatrix() * direction_; }


	void SetShowBasisIndicator(bool enable);
	bool GetShowBasisIndicator() { return showBasisIndicator_; }

	void SetShowColorIndicator(bool enable, Color color);
	bool GetShowColorIndicator() const { return showColorIndicator_; }



	bool isEndCap_ = false;//if the point is a rod - indicates the point is a limiting end.  if the point is a hole - indicates the hole is a cap and a rod cannot pass through one side. does not need to have row.


	Vector3 direction_ = Vector3::FORWARD;//indicates direction in local space

	bool showBasisIndicator_ = false;
	bool showColorIndicator_ = false;

	WeakPtr<PiecePointRow> row_;//#todo serialize
	unsigned rowId_ = 0;

	WeakPtr<PiecePoint> occupiedPoint_;//other point that is "occupying the space of this point"
	WeakPtr<PiecePoint> occupiedPointPrev_;

	WeakPtr<StaticModel> basisIndicatorStMdl_;
	WeakPtr<StaticModel> colorIndicatorStMdl_;
	WeakPtr<Node> basisIndicatorNode_;
	WeakPtr<Node> colorIndicatorNode_;
	Color colorIndicatorColor_;


	virtual bool SaveXML(XMLElement& dest) const override;
	virtual bool LoadXML(const XMLElement& source) override;


	virtual void ApplyAttributes() override;



protected:
	virtual void OnNodeSet(Node* node) override;
};
