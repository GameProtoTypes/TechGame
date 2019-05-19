
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

	URHOMATHEXTRAS_API void TestFunc() {
		URHO3D_LOGINFO("Hello From TestFunc");
	 }


	URHOMATHEXTRAS_API void TestEigen();

	URHOMATHEXTRAS_API Plane BestFitPlaneSVD(ea::vector<Vector3> points);




}