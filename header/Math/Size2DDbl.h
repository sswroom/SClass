#ifndef _SM_MATH_SIZE2DDBL
#define _SM_MATH_SIZE2DDBL
#include "Math/Double2D.h"

namespace Math
{
	struct Size2DDbl : public Double2D
	{
		Size2DDbl() = default;
		Size2DDbl(Double2D v) : Double2D(v)
		{
		}

		Size2DDbl(Double w, Double h) : Double2D(w, h)
		{
		}

		Double GetWidth() const
		{
			return this->x;
		}

		Double GetHeight() const
		{
			return this->y;
		}

		Double CalcArea() const
		{
			return this->x * this->y;
		}

		Bool HasArea() const
		{
			return this->x > 0 && this->y > 0;
		}
	};
}
#endif
