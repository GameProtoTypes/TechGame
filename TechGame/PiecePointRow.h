#pragma once
#include "Urho3D/Urho3DAll.h"

#include "NewtonConstraint.h"

class PiecePoint;
class Piece;

class PiecePointRow : public Component
{
	URHO3D_OBJECT(PiecePointRow, Component);
public:

	enum RowType {
		RowType_Hole = 0, // a hole
		RowType_HoleTight, // a hole that limits movement (glued hole)
		RowType_RodHard,
		RowType_RodRound
	};

	enum RowTypeGeneral {
		RowTypeGeneral_Hole = 0,
		RowTypeGeneral_Rod
	};

	struct RowAttachement {
		WeakPtr<PiecePoint> pointA_;
		WeakPtr<PiecePoint> pointB ;
		WeakPtr<PiecePointRow> rowA_ ;
		WeakPtr<PiecePointRow> rowB_ ;
		WeakPtr<NewtonConstraint> constraint_ ;
	};


	PiecePointRow(Context* context) : Component(context)
	{
		debugColor_ = Color(Random(1.0f), Random(1.0f), Random(1.0f),0.2f);

		SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(PiecePointRow, HandleUpdate));
	}

	static bool RowsAttachCompatable(PiecePointRow* rowA, PiecePointRow* rowB);

	//attach 2 rows together.  Must be in setup in world configuration first.
	static bool AttachRows(PiecePointRow* rowA, PiecePointRow* rowB, PiecePoint* pointA, PiecePoint* pointB, bool attachAsFullRow = false);

	static bool RowsHaveDegreeOfFreedom(PiecePointRow* rowA, PiecePointRow* rowB);

	// checks if the given row is full and if it is, reforms constraints.
	static bool UpdateOptimizeFullRow(PiecePointRow* row);


	static void RegisterObject(Context* context) {
		context->RegisterFactory<PiecePointRow>();
	}

	virtual void DrawDebugGeometry(DebugRenderer* debug, bool depthTest) override;




	bool Finalize() {


		if (!CheckValid())
			return false;

		FormPointDirectionsOnEndPoints();

		if (GetRowDirectionLocal().Length() <= 0.0f)
		{
			URHO3D_LOGWARNING("PiecePointRow has no direction defined, Use SetLocalDirection(...)");
		}

		return true;
	}

	
	bool CheckValid();

	void FormPointDirectionsOnEndPoints();

	void SetRowType(RowType type) {
		rowType_ = type;

		if (rowType_ == RowType_Hole)
			rowTypeGeneral_ = RowTypeGeneral_Hole;
		
		if (rowType_ == RowType_RodHard)
			rowTypeGeneral_ = RowTypeGeneral_Rod;

		if (rowType_ == RowType_RodRound)
			rowTypeGeneral_ = RowTypeGeneral_Rod;

	}
	RowType GetRowType() const { return rowType_; }
	RowTypeGeneral GetGeneralRowType() const { return rowTypeGeneral_; }

	void SetPiecePlaner(bool planer) { isPiecePlaner_ = planer; }

	bool GetIsPiecePlaner() const { return isPiecePlaner_; }

	void PushBack(PiecePoint* point);

	void GetEndPoints(PiecePoint*& pointA, PiecePoint*& pointB);

	void SetRowDirectionLocal(Vector3 direction) { localDirection_ = direction.Normalized(); }

	//return direction pointing from pointA to pointB. in piece space
	Vector3 GetRowDirectionLocal();


	//return direction pointing from pointA to pointB. in world space
	Vector3 GetRowDirectionWorld();


	bool IsEndPoint(PiecePoint* point);

	int Count() { return points_.size(); }

	///return the next point going inside the row from the given endPoint.
	PiecePoint* GetPointNextToEndPoint(PiecePoint* endPoint);

	const ea::vector<PiecePoint*>& GetPoints() { return points_; }

	Vector3 GetLocalCenter();

	Vector3 GetWorldCenter();

	bool AttachedToRow(PiecePointRow* row);

	///get all rows that are attached to this row.
	void GetAttachedRows(ea::vector<PiecePointRow*>& rows);

	bool DetachFrom(PiecePointRow* otherRow, bool updateOccupiedPoints);

	bool DetachAll();

	Piece* GetPiece();

	ea::vector<PiecePoint*> points_;

	ea::vector<RowAttachement> rowAttachements_;//row attachments where rowA is the "other row" and robB is this row. (same with points)
protected:

	void HandleUpdate(StringHash event, VariantMap& eventData);

	void UpdatePointOccupancies();

	void UpdateDynamicDettachement();

	bool isOccupiedOptimized_ = false;
	int occupiedCountDownCount_ = 10;
	int occupiedCountDown_ = 1;
	Vector2 oldSliderLimits_;

	Color debugColor_;

	Vector3 localDirection_ = Vector3::ZERO;

	RowType rowType_ = RowType_Hole;
	RowTypeGeneral rowTypeGeneral_ = RowTypeGeneral_Hole;

	bool isPiecePlaner_ = true;//true if the whole piece geometry is on the plane that is perpendicular to the row.
};


