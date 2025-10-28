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
		Optional<Math::ProjectedCoordinateSystem> csys;

	public:
		DistanceMeasurement(Double instX, Double instY, Double instZ, Double hAngleAdj, Optional<Math::ProjectedCoordinateSystem> csys);
		~DistanceMeasurement();

		void FromXYZ(Double x, Double y, Double z, OutParam<Double> hAngle, OutParam<Double> vAngle, OutParam<Double> dist);
		void ToXYZ(Double hAngle, Double vAngle, Double dist, OutParam<Double> x, OutParam<Double> y, OutParam<Double> z);
	};
}
#endif