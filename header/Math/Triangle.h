#ifndef _SM_MATH_TRIANGLE
#define _SM_MATH_TRIANGLE
#include "Math/Coord2D.h"
#include "Math/Math.h"

namespace Math
{
	struct Triangle
	{
	public:
		Coord2DDbl pt[3];

		Triangle() = default;
		Triangle(Coord2DDbl p1, Coord2DDbl p2, Coord2DDbl p3)
		{
			pt[0] = p1;
			pt[1] = p2;
			pt[2] = p3;
		}

		Double CalcArea()
		{
			Double a1 = (pt[1].x - pt[0].x) * (pt[0].y - pt[1].y);
			Double a2 = (pt[0].y - pt[1].y) * (pt[1].x - pt[2].x);
			Double a3 = (pt[1].x - pt[0].x) * (pt[0].y - pt[1].y);
			return (a1 + a2 - a3) * 0.5;
		}
	};
}
#endif
