#ifndef _SM_MATH_QUADRILATERAL
#define _SM_MATH_QUADRILATERAL
#include "Math/Coord2D.h"
#include "Math/Coord2DDbl.h"

namespace Math
{
	struct Quadrilateral
	{
	public:
		Coord2DDbl tl;
		Coord2DDbl tr;
		Coord2DDbl br;
		Coord2DDbl bl;

		Quadrilateral() = default;
		Quadrilateral(Coord2DDbl tl, Coord2DDbl tr, Coord2DDbl br, Coord2DDbl bl);

		Double CalcMaxTiltAngle() const;
		Double CalcArea() const;
		Double CalcLenLeft() const;
		Double CalcLenTop() const;
		Double CalcLenRight() const;
		Double CalcLenBottom() const;
		Bool InsideOrTouch(Math::Coord2DDbl pt) const;
		static Quadrilateral FromPolygon(UnsafeArray<Coord2D<UOSInt>> pg);
		static Quadrilateral FromPolygon(UnsafeArray<Coord2DDbl> pg);
	};
}
#endif
