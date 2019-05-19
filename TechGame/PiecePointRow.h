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
		RowType_Hole = 0,
		RowType_HoleTight,
		RowType_RodHard,
		RowType_RodRound
	};

	enum RowTypeGeneral {
		RowTypeGeneral_Hole = 0,
		RowTypeGeneral_Rod
	};

	struct RowAttachement {
		ea::weak_ptr<PiecePoint> pointA_;
		ea::weak_ptr<PiecePoint> pointB ;
		ea::weak_ptr<PiecePointRow> rowA_ ;
		ea::weak_ptr<PiecePointRow> rowB_ ;
		ea::weak_ptr<NewtonConstraint> constraint_ ;
	};


	PiecePointRow(Context* context) : Component(context)
	{
		debugColor_ = Color(Random(1.0f), Random(1.0f), Random(1.0f),0.2f);

		SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(PiecePointRow, HandleUpdate));
	}

	static bool RowsAttachCompatable(PiecePointRow* rowA, PiecePointRow* rowB);

	//attach 2 rows together.  Must be in setup in world configuration first.
	static bool AttachRows(PiecePointRow* rowA, PiecePointRow* rowB, PiecePoint* pointA, PiecePoint* pointB, bool attachAsFullRow = false);

	// checks if the given row is full and if it is, reforms constraints.
	static bool OptimizeFullRow(PiecePointRow* row);

	//distance between 2 points in a row.
	static float RowPointDistance() { return 0.25*0.5f; }

	static void RegisterObject(Context* context) {
		context->RegisterFactory<PiecePointRow>();
	}

	virtual void DrawDebugGeometry(DebugRenderer* debug, bool depthTest) override;




	bool Finalize() {
		if (!CheckValid())
			return false;

		FormPointDirectionsOnEndPoints();

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


	//return direction pointing from pointA to pointB.
	Vector3 GetRowDirectionLocal();

	Vector3 GetRowDirectionWorld();

	bool IsEndPoint(PiecePoint* point);

	int Count() { return points_.size(); }

	///return the next point going inside the row from the given endPoint.
	PiecePoint* GetPointNextToEndPoint(PiecePoint* endPoint);

	const ea::vector<PiecePoint*>& GetPoints() { return points_; }

	Vector3 GetLocalCenter();

	Vector3 GetWorldCenter();

	bool AttachedToRow(PiecePointRow* row);

	bool DetachFrom(PiecePointRow* otherRow);

	bool DetachAll();

	Piece* GetPiece();



	ea::vector<PiecePoint*> points_;

	ea::vector<RowAttachement> attachedRows_;
protected:

	void HandleUpdate(StringHash event, VariantMap& eventData);

	void UpdatePointOccupancies();

	bool isOccupiedOptimized_ = false;
	Vector2 oldSliderLimits_;

	Color debugColor_;

	RowType rowType_ = RowType_Hole;
	RowTypeGeneral rowTypeGeneral_ = RowTypeGeneral_Hole;

	bool isPiecePlaner_ = true;//true if the whole piece geometry is on the plane that is perpendicular to the row.
};


