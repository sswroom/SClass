#ifndef _SM_MATH_DISTANCEMEASUREMENT
#define _SM_MATH_DISTANCEMEASUREMENT
#include "Math/ProjectedCoordinateSystem.h"

namespace Math
{
	class DistanceMeasurement
	{
	private:
		Double instX;
		Double instY;
		Double instZ;
		Double hAngleAdj;
		Math::ProjectedCoordinateSystem *csys;

	public:
		DistanceMeasurement(Double instX, Double instY, Double instZ, Double hAngleAdj, Math::ProjectedCoordinateSystem *csys);
		~DistanceMeasurement();

		void FromXYZ(Double x, Double y, Double z, Double *hAngle, Double *vAngle, Double *dist);
		void ToXYZ(Double hAngle, Double vAngle, Double dist, Double *x, Double *y, Double *z);
	};
}
#endif