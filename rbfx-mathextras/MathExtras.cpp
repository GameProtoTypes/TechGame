#include "MathExtras.h"

#include <iostream>


#include "eigen/Eigen/Dense"
#include "Urho3D/IO/Log.h"
#include "Urho3D/Math/Plane.h"
#include "Urho3D/Math/Vector3.h"
#include "EASTL/vector.h"


using namespace Eigen;

namespace Urho3D {

	URHOMATHEXTRAS_API void TestFunc()
	{
		URHO3D_LOGINFO("Hello From TestFunc");
	}

	void TestEigen()
	{
		MatrixXd m(2, 2);
		m(0, 0) = 3;
		m(1, 0) = 2.5;
		m(0, 1) = -1;
		m(1, 1) = m(1, 0) + m(0, 1);
		std::cout << m << std::endl;
	}


	URHOMATHEXTRAS_API Plane BestFitPlaneSVD(ea::vector<Vector3> points)
	{
		Vector3 centriod;

		for (Vector3& point : points)
		{
			centriod += point;
		}
		centriod /= float(points.size());


		//form the matrix M
		MatrixXf M(3, points.size());
		for (int r = 0; r < points.size(); r++)
		{
			M(0, r) = points[r].x_ - centriod.x_;
			M(1, r) = points[r].y_ - centriod.y_;
			M(2, r) = points[r].z_ - centriod.z_;
		}


		Eigen::JacobiSVD<MatrixXf> svd(M, ComputeThinU | ComputeThinV);
		
		auto singularValues = svd.singularValues();

		//std::cout << "singularValues: " << singularValues << std::endl;
		//std::cout << "U: " << svd.matrixU() << std::endl;
		//std::cout << "V: " << svd.matrixV() << std::endl;

		int i = 0;
		int smallestIdx = 0;
		while (i < singularValues.size())
		{
			if (singularValues[i] < singularValues[smallestIdx]) {
				smallestIdx = i;
			}
			i++;
		}

		Eigen::MatrixXf normal = svd.matrixU().row(smallestIdx);


		return Plane(Vector3(normal(0), normal(1), normal(2)), centriod);
	}

	enum Filtering {
		Filtering_None = 0,
		Filtering_Bilinear
	};



	URHOMATHEXTRAS_API void FindInflectionPoints(ea::vector<Vector2> & values, ea::vector<ea::pair<Vector2,int>> & t)
	{

		Vector2 lastHighest = { 0.0, -M_LARGE_VALUE };
		Vector2 lastLowest = { 0.0, M_LARGE_VALUE };
		int dir = 1;
		for (Vector2& val : values) {

			if (dir == 1) {
				if (val.y_ > lastHighest.y_) {
					lastHighest = val;
					dir = 1;
				}
				else 
				{
					//found a high point
					t.push_back({lastHighest , dir });
					lastHighest.y_ = -M_LARGE_VALUE;
					lastLowest = val;
					dir = -1;
				}
			}
			else if (dir == -1) {

				if (val.y_ < lastLowest.y_) {
					lastLowest = val;
					dir = -1;
				}
				else
				{
					//found a low point
					t.push_back({ lastLowest, dir });
					lastLowest.y_ = M_LARGE_VALUE;
					lastHighest = val;
					dir = -1;
				}
			}
		}
	}



}
