#ifndef _SM_MATH_SIZE2D
#define _SM_MATH_SIZE2D
#include "Math/Coord2D.h"

namespace Math
{
	template <typename T> struct Size2D
	{
		T width;
		T height;

		Size2D() = default;
		Size2D(void *)
		{
			this->width = 0;
			this->height = 0;
		}
		Size2D(T w, T h)
		{
			this->width = w;
			this->height = h;
		}

		T CalcArea()
		{
			return this->width * this->height;
		}

		Size2D<Double> ToDouble() const
		{
			return Size2D<Double>((Double)this->width, (Double)this->height);
		}

		Math::Coord2DDbl ToCoord() const
		{
			return Math::Coord2DDbl((Double)this->width, (Double)this->height);
		}
	};
}
#endif
