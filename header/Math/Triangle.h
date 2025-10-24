#ifndef _SM_MATH_TRIANGLE
#define _SM_MATH_TRIANGLE
#include "Math/Coord2DDbl.h"
#include "Math/Math_C.h"

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
			Double a1 = pt[0].x * (pt[1].y - pt[2].y);
			Double a2 = pt[1].x * (pt[2].y - pt[0].y);
			Double a3 = pt[2].x * (pt[0].y - pt[1].y);
			return (a1 + a2 + a3) * 0.5;
		}

		Coord2DDbl Remap(Coord2DDbl p, Triangle destTri)
		{
			Triangle bcp = Triangle(this->pt[1], this->pt[2], p);
			Triangle cap = Triangle(this->pt[2], this->pt[0], p);
			Triangle abp = Triangle(this->pt[0], this->pt[1], p);
			Double u = bcp.CalcArea();
			Double v = cap.CalcArea();
			Double w = abp.CalcArea();
			Double ratio = 1 / (u + v + w);
			u *= ratio;
			v *= ratio;
			w *= ratio;
			return destTri.pt[0] * u + destTri.pt[1] * v + destTri.pt[2] * w;
		}

		static Double Sign(Coord2DDbl p1, Coord2DDbl p2, Coord2DDbl p3)
		{
			Coord2DDbl diff13 = p1 - p3;
			Coord2DDbl diff23 = p2 - p3;
			return diff13.x * diff23.y - diff23.x * diff13.y;
		}

		Bool InsideOrTouch(Math::Coord2DDbl pt) const
		{
			Double d1 = Sign(pt, this->pt[0], this->pt[1]);
			Double d2 = Sign(pt, this->pt[1], this->pt[2]);
			Double d3 = Sign(pt, this->pt[2], this->pt[0]);
			if (d1 <= 0 && d2 <= 0 && d3 <= 0)
			{
				return true;
			}
			if (d1 >= 0 && d2 >= 0 && d3 >= 0)
			{
				return true;
			}
			return false;
		}
	};
}
#endif
