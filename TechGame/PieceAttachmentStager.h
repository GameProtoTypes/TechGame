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
	{

	}

	static void RegisterObject(Context* context)
	{
		context->RegisterFactory<PieceAttachmentStager>();
	}

	void SetScene(Scene* scene){ scene_ = scene; }


	bool AddPotentialAttachement(PiecePoint* pointA, PiecePoint* pointB);

	bool RemovePotentialAttachment(PiecePoint* pointA, PiecePoint* pointB);


	
	void AnalyzeAndFix()
	{
		if (scene_ == nullptr) {
			URHO3D_LOGERROR("PieceAttachementStager: Set the active scene (SetScene) before AnalyzeAndFix!");
			return;
		}

		if (!needsAnalyzed_)
			return;

		isValid_ = false;


		if (!checkDistances()) {
			URHO3D_LOGINFO("PieceAttachmentStager: Distance Check Failed");
			return;
		}

		collectRows();

		if (!checkRowBasicCompatability())
		{
			URHO3D_LOGINFO("PieceAttachmentStager: Rows did not pass basic compatability test.");
			return;
		}

		if (!checkEndPointRules()) {
			URHO3D_LOGINFO("PieceAttachmentStager: End Point Check Failed");
			return;
		}

		


		finalAttachments_ = potentialAttachments_;

		URHO3D_LOGINFO("final attachment size " + ea::to_string(finalAttachments_.size()));

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

	bool checkRowBasicCompatability();
	bool checkEndPointRules();
	bool endPointRulePass(AttachmentPair* attachPair);

	
	ea::hash_map<PiecePoint*, AttachmentPair*> potentialAttachmentMapA_;
	ea::hash_map<PiecePoint*, AttachmentPair*> potentialAttachmentMapB_;
	ea::vector<AttachmentPair*> potentialAttachments_;


	ea::vector<PiecePointRow*> rowsA;
	ea::vector<PiecePointRow*> rowsB;

	ea::vector<AttachmentPair*> finalAttachments_;

	bool needsAnalyzed_ = true;
	bool isValid_ = false;

	WeakPtr<Scene> scene_ = nullptr;
};



