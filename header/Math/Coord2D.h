#ifndef _SM_MATH_COORD2D
#define _SM_MATH_COORD2D
#include "Math/Value2D.h"

namespace Math
{
	template <typename T> struct Coord2D : public Value2D<T>
	{
		Coord2D() = default;
		Coord2D(void *v) : Value2D<T>(v)
		{
		}

		Coord2D(T x, T y) : Value2D<T>(x, y)
		{
		}

		Coord2D(Value2D<T> v) : Value2D<T>(v)
		{
		}

		Double CalcLengTo(Coord2D<T> coord) const
		{
			return Math_Sqrt(Math_Sqr((Double)(this->x - coord.x)) + Math_Sqr((Double)(this->y - coord.y)));
		}
	};
}
#endif
