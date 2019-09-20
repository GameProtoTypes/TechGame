#pragma once
#include "Urho3D/Urho3DAll.h"

#include "NewtonConstraint.h"
#include "PieceManager.h"

class PiecePoint;
class Piece;



static const char* RowTypeNames[] =
{
	"RowType_Hole",
	"RowType_HoleTight",
	"RowType_RodHard",
	"RowType_RodRound",
	nullptr
};
static const char* RowTypeGeneralNames[] =
{
	"RowTypeGeneral_Hole",
	"RowTypeGeneral_Rod",
	nullptr
};


class PiecePointRow : public LogicComponent
{
	URHO3D_OBJECT(PiecePointRow, LogicComponent);
public:

	enum RowType {
		RowType_Hole = 0, // a hole
		RowType_RodHard,
		RowType_RodRound
	};

	enum RowTypeGeneral {
		RowTypeGeneral_Hole = 0,
		RowTypeGeneral_Rod
	};


	static const RowType DefaultRowType = RowType_Hole;
	static const RowTypeGeneral DefaultGeneralRowType = RowTypeGeneral_Hole;


	struct RowAttachement {
		unsigned pointOtherId_;
		unsigned pointId;
		unsigned rowOtherId_;
		unsigned rowId_;
		unsigned constraintId_;

		WeakPtr<PiecePoint> pointOther_;
		WeakPtr<PiecePoint> point;
		WeakPtr<PiecePointRow> rowOther_ ;
		WeakPtr<PiecePointRow> row_ ;
		WeakPtr<NewtonConstraint> constraint_ ;
	};


	PiecePointRow(Context* context) : LogicComponent(context)
	{
		debugColor_ = Color(Random(1.0f), Random(1.0f), Random(1.0f), 0.2f);

		SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(PiecePointRow, HandleUpdate));
	}

	static bool RowsAttachCompatable(PiecePointRow* rowA, PiecePointRow* rowB);

	//attach 2 rows together.  Must be in setup in world configuration first.
	static bool AttachRows(PiecePointRow* rowA, PiecePointRow* rowB, PiecePoint* pointA, PiecePoint* pointB, bool attachAsFullRow = false, bool updateOptimizations = true);

	static void ComputeSlideLimits(PiecePointRow* theHoleRow, PiecePointRow* theRodRow, PieceManager* pieceManager, NewtonConstraint* constraint, NewtonRigidBody* rodBody, Quaternion diffSnap45);

	static bool RowsHaveDegreeOfFreedom(PiecePointRow* rowA, PiecePointRow* rowB);

	// checks if the given row is full and if it is, reforms constraints.
	static bool UpdateOptimizeFullRow(PiecePointRow* row);


	static void RegisterObject(Context* context);

	virtual void DrawDebugGeometry(DebugRenderer* debug, bool depthTest) override;


	bool Finalize() {

		if (!CheckValid())
			return false;

		FormPointDirections();

		if (GetRowDirectionLocal().Length() <= 0.0f)
		{
			URHO3D_LOGWARNING("PiecePointRow has no direction defined, Use SetLocalDirection(...)");
		}

		return true;
	}

	
	bool CheckValid();

	void FormPointDirections();

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

	//returns true if there is at least one endcap in the row.
	bool HasAnEndCap();

	int Count() { return points_.size(); }

	///return the next point going inside the row from the given endPoint.
	PiecePoint* GetPointNextToEndPoint(PiecePoint* endPoint);

	const ea::vector<SharedPtr<PiecePoint>>& GetPoints() { return points_; }

	Vector3 GetLocalCenter();

	Vector3 GetWorldCenter();

	bool AttachedToRow(PiecePointRow* row);

	///get all rows that are attached to this row.
	void GetAttachedRows(ea::vector<PiecePointRow*>& rows);

	bool DetachFrom(PiecePointRow* otherRow, bool updateOccupiedPoints);

	bool DetachAll();

	bool ReAttachTo(PiecePointRow* otherRow);

	bool ReAttachAll();

	void ResetOccupiedPointsCountDown() {
		occupiedCountDown_ = occupiedCountDownCount_;
	}

	Piece* GetPiece();

	ea::vector<SharedPtr<PiecePoint>> points_;
	ea::vector<unsigned> pointIds_;

	ea::vector<RowAttachement> rowAttachements_;//row attachments where rowA is the "other row" and robB is this row. (same with points)





	virtual bool SaveXML(XMLElement& dest) const override;
	virtual bool LoadXML(const XMLElement& source) override;



	virtual void ApplyAttributes() override;




protected:

	void HandleUpdate(StringHash event, VariantMap& eventData);

	void UpdatePointOccupancies();

	void UpdateDynamicDettachement();

	bool isFullRowOptimized_ = false;

	int numOccupiedPoints_ = 0;
	int occupiedCountDownCount_ = 50;
	int occupiedCountDown_ = 1;
	Vector2 oldSliderLimits_;

	Color debugColor_;

	Vector3 localDirection_ = Vector3::FORWARD;

	RowType rowType_ = RowType_Hole;
	RowTypeGeneral rowTypeGeneral_ = RowTypeGeneral_Hole;

	bool isPiecePlaner_ = true;//true if the whole piece geometry is on the plane that is perpendicular to the row.

	SharedPtr<PieceManager> pieceManager_;

	virtual void OnNodeSet(Node* node) override;
	virtual void DelayedStart() override;

};


