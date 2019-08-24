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
		WeakPtr<PiecePoint> pointA = nullptr;
		WeakPtr<PiecePoint> pointB = nullptr;


		WeakPtr<PiecePointRow> rowA = nullptr;
		WeakPtr<PiecePointRow> rowB = nullptr;

		WeakPtr<Piece> pieceA = nullptr;
		WeakPtr<Piece> pieceB = nullptr;

		bool goodAttachment_ = true;
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


	
	void Analyze()
	{
		if (scene_ == nullptr) {
			URHO3D_LOGERROR("PieceAttachementStager: Set the active scene (SetScene) before AnalyzeAndFix!");
			return;
		}

		if (!needsAnalyzed_)
			return;

		isValid_ = false;


		checkDistances();

		collectRows();

		checkRowBasicCompatability();

		checkAllEndPointRules();

		checkPointDirections();


		for (AttachmentPair* pair : potentialAttachments_) {

			if (pair->goodAttachment_)
				goodAttachments_.push_back(pair);
			else
				badAttachments_.push_back(pair);

		}

		//URHO3D_LOGINFO("final attachment size " + ea::to_string(finalAttachments_.size()));

		needsAnalyzed_ = false;
		isValid_ = (badAttachments_.size() == 0);
	}

	

	bool IsValid()
	{
		//its valid if nothing is staged.
		if (potentialAttachments_.size() == 0)
			return true;

		if (needsAnalyzed_)
			return false;

		if (goodAttachments_.size() == 0 && badAttachments_.size() == 0)
			return true;


		return isValid_;
	}


	bool AttachAll();


	void Reset()
	{
		potentialAttachmentMapA_.clear();
		potentialAttachmentMapB_.clear();

		potentialAttachments_.clear();
		rowsA.clear();
		rowsB.clear();

		goodAttachments_.clear();
		badAttachments_.clear();

		needsAnalyzed_ = true;
		isValid_ = false;
	}

	ea::vector<AttachmentPair*>& GetPotentialAttachments() { return potentialAttachments_; }
	ea::vector<AttachmentPair*>& GetGoodAttachments() { return goodAttachments_; }
	ea::vector<AttachmentPair*>& GetBadAttachments() { return badAttachments_; }

	unsigned GetCurrentAttachSignature() { 

		unsigned hash = 0;//NOT A PERFECT HASH! (but should do the job)
		for (AttachmentPair* pair : goodAttachments_) {
			hash += (unsigned)(void*)pair->pointA + (unsigned)(void*)pair->pointB;
		}

		for (AttachmentPair* pair : badAttachments_) {
			hash += (unsigned)(void*)pair->pointA + (unsigned)(void*)pair->pointB;
		}


		return hash;
	
	}

protected:

	void checkDistances();
	bool collectRows();

	void checkRowBasicCompatability();
	void checkAllEndPointRules();
	void checkEndPointRules(AttachmentPair* attachPair);

	void checkPointDirections();
	
	ea::hash_map<PiecePoint*, AttachmentPair*> potentialAttachmentMapA_;
	ea::hash_map<PiecePoint*, AttachmentPair*> potentialAttachmentMapB_;
	ea::vector<AttachmentPair*> potentialAttachments_;


	ea::vector<PiecePointRow*> rowsA;
	ea::vector<PiecePointRow*> rowsB;

	ea::vector<AttachmentPair*> goodAttachments_;
	ea::vector<AttachmentPair*> badAttachments_;



	bool needsAnalyzed_ = true;
	bool isValid_ = false;

	WeakPtr<Scene> scene_ = nullptr;
};



