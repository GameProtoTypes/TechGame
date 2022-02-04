#pragma once

#define URHOMATHEXTRAS_EXPORT_API __declspec(dllexport)


#ifdef URHOMATHEXTRAS_STATIC
#  define URHOMATHEXTRAS_API
#  define URHOMATHEXTRAS_NO_EXPORT
#else
#  ifndef URHOMATHEXTRAS_API
#    ifdef URHOMATHEXTRAS_EXPORTS
/* We are building this library */
#      define URHOMATHEXTRAS_API URHOMATHEXTRAS_EXPORT_API
#    else
/* We are using this library */
#      define URHOMATHEXTRAS_API __declspec(dllimport)
#    endif
#  endif

#  ifndef URHOMATHEXTRAS_NO_EXPORT
#    define URHOMATHEXTRAS_NO_EXPORT 
#  endif
#endif



#include "Urho3D/IO/Log.h"
#include "Urho3D/Math/Plane.h"
#include "Urho3D/Math/Vector3.h"
#include "EASTL/vector.h"

namespace Urho3D {

	URHOMATHEXTRAS_API void TestFunc();

	URHOMATHEXTRAS_API void TestEigen();

	URHOMATHEXTRAS_API Plane BestFitPlaneSVD(ea::vector<Vector3> points);

	///Basic Algorithm for finding min and max points in the values array. (no filtering)
	URHOMATHEXTRAS_API void FindInflectionPoints(ea::vector<Vector2> & values, ea::vector<ea::pair<Vector2, int>> & t);

	///Given an orientation - converts to Euler angles and rounds each axis independently to a multiple of the given angle. then converts back to quaternion
	template <typename T>
	URHOMATHEXTRAS_API Quaternion SnapOrientationEuler(const Quaternion& orientation, T angle)
	{
		Vector3 Eulers = orientation.EulerAngles();
		Vector3 RoundedEulers = Vector3(RoundToNearestMultiple<T>(Eulers.x_, angle), RoundToNearestMultiple<T>(Eulers.y_, angle), RoundToNearestMultiple<T>(Eulers.z_, angle));

		

		Quaternion roundedQuaternion;
		roundedQuaternion.FromEulerAngles(RoundedEulers.x_, RoundedEulers.y_, RoundedEulers.z_);
		return roundedQuaternion;
	}

	///Given an orientation - snaps its current Angle to the nearest multiple of angle.  
	template <typename T>
	URHOMATHEXTRAS_API Quaternion SnapOrientationAngle(const Quaternion& orientation, T angle)
	{
		Quaternion orientationNormalized = orientation.Normalized();

		if (orientationNormalized.Axis().IsNaN() || orientationNormalized.Axis().IsInf()) {
			return Quaternion::IDENTITY;
		}
		else
		{
			return Quaternion(RoundToNearestMultiple(orientationNormalized.Angle(), angle), orientationNormalized.Axis()).Normalized();
		}
	}
}
