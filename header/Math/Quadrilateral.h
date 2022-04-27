#ifndef _SM_MATH_QUADRILATERAL
#define _SM_MATH_QUADRILATERAL
#include "Math/Coord2D.h"

namespace Math
{
	struct Quadrilateral
	{
	public:
		Coord2DDbl tl;
		Coord2DDbl tr;
		Coord2DDbl br;
		Coord2DDbl bl;

		Double CalcMaxTiltAngle();
		Double CalcArea();
		Double CalcLenLeft();
		Double CalcLenTop();
		Double CalcLenRight();
		Double CalcLenBottom();
		static Quadrilateral FromPolygon(Coord2D<UOSInt> *pg);
		static Quadrilateral FromPolygon(Coord2DDbl *pg);
	};
}
#endif
