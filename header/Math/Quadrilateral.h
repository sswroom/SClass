#ifndef _SM_MATH_QUADRILATERAL
#define _SM_MATH_QUADRILATERAL
#include "Math/Coord2D.h"

namespace Math
{
	struct Quadrilateral
	{
	public:
		Coord2D<Double> tl;
		Coord2D<Double> tr;
		Coord2D<Double> br;
		Coord2D<Double> bl;

		Double CalcMaxTiltAngle();
		Double CalcArea();
		Double CalcLenLeft();
		Double CalcLenTop();
		Double CalcLenRight();
		Double CalcLenBottom();
		static Quadrilateral FromPolygon(Coord2D<UOSInt> *pg);
		static Quadrilateral FromPolygon(Coord2D<Double> *pg);
	};
}
#endif
