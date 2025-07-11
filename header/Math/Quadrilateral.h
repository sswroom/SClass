#ifndef _SM_MATH_QUADRILATERAL
#define _SM_MATH_QUADRILATERAL
#include "Math/Coord2D.h"
#include "Math/Coord2DDbl.h"

namespace Math
{
	class RectAreaDbl;
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
		RectAreaDbl GetExterior() const;
		Bool IsRectangle() const;
		Bool IsNonRotateRectangle() const;
		UOSInt CalcIntersactsAtY(UnsafeArray<Double> xArr, Double y) const;
		static Double Sign(Coord2DDbl p1, Coord2DDbl p2, Coord2DDbl p3);
		static Quadrilateral FromPolygon(UnsafeArray<Coord2D<UOSInt>> pg);
		static Quadrilateral FromPolygon(UnsafeArray<Coord2DDbl> pg);

	private:
		static UOSInt CalcIntersactAtY(Math::Coord2DDbl p1, Coord2DDbl p2, Double y, UnsafeArray<Double> xArr);
	};
}
#endif
