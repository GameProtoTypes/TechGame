#pragma once

#include "Urho3D/Urho3DAll.h"

class AppVersion : public Object {
	URHO3D_OBJECT(AppVersion, Object);
public:

	AppVersion(Context* context);

	~AppVersion();

	static void RegisterObject(Context* context);

	void SetVersion(int major, int beta, int alpha) { major_ = major; beta_ = beta; alpha_ = alpha; }

	int GetMajor() const { return major_; }
	int GetBeta() const { return beta_; }
	int GetAlpha() const { return alpha_; }

	ea::string GetVersionString() const { return ea::to_string(major_) + "." + ea::to_string(beta_) + "." + ea::to_string(alpha_); }

protected:

	int major_ = 0;
	int beta_ = 0;
	int alpha_ = 3;

};


