#pragma once

#include "Urho3D/Urho3DAll.h"

class PiecePoint;
class PiecePointRow;
class Piece;
class PieceAttachmentStager : public Object
{
	URHO3D_OBJECT(PieceAttachmentStager, Object);

public:

	struct AttachmentPair
	{
		PiecePoint* pointA = nullptr;
		PiecePoint* pointB = nullptr;


		PiecePointRow* rowA = nullptr;
		PiecePointRow* rowB = nullptr;

		Piece* pieceA = nullptr;
		Piece* pieceB = nullptr;
	};




	PieceAttachmentStager(Context* context) : Object(context)
	{}

	static void RegisterObject(Context* context)
	{
		context->RegisterFactory<PieceAttachmentStager>();
	}

	bool AddPotentialAttachement(PiecePoint* pointA, PiecePoint* pointB);

	bool RemovePotentialAttachment(PiecePoint* pointA, PiecePoint* pointB);


	
	void AnalyzeAndFix()
	{
		if (!needsAnalyzed_)
			return;

		isValid_ = false;


		if (!checkDistances()) {
			URHO3D_LOGINFO("PieceAttachmentStager: Distance Check Failed");
			return;
		}

		collectRows();

		
		if (!checkEndPointRules()) {
			URHO3D_LOGINFO("PieceAttachmentStager: End Point Check Failed");
			return;
		}





		finalAttachments_ = potentialAttachments_;



		needsAnalyzed_ = false;
		isValid_ = true;
	}

	

	bool IsValid()
	{
		if (needsAnalyzed_)
			return false;

		return isValid_;
	}


	bool AttachAll();






protected:

	bool checkDistances();
	bool collectRows();

	bool checkEndPointRules();
	bool endPointRulePass(AttachmentPair* attachPair);

	
	HashMap<PiecePoint*, AttachmentPair*> potentialAttachmentMapA_;
	HashMap<PiecePoint*, AttachmentPair*> potentialAttachmentMapB_;
	Vector<AttachmentPair*> potentialAttachments_;


	Vector<PiecePointRow*> rowsA;
	Vector<PiecePointRow*> rowsB;

	Vector<AttachmentPair*> finalAttachments_;

	bool needsAnalyzed_ = true;
	bool isValid_ = false;
};



