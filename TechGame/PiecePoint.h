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




	bool isEndCap_ = false;//if the point is a rod - indicates the point is a limiting end.  if the point is a hole - indicates the hole is a cap and a rod cannot pass through one side. does not need to have row.


	Vector3 direction_;//indicates direction in local space



	ea::weak_ptr<PiecePointRow> row_;

	ea::weak_ptr<PiecePoint> occupiedPoint_;//other point that is "occupying the space of this point"
	ea::weak_ptr<PiecePoint> occupiedPointPrev_;


protected:
	virtual void OnNodeSet(Node* node) override;
};
