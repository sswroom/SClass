#ifndef _SM_MATH_SIZE2DDBL
#define _SM_MATH_SIZE2DDBL
#include "Math/Coord2DDbl.h"

namespace Math
{
	struct Size2DDbl : public Coord2DDbl
	{
		Size2DDbl() = default;
		Size2DDbl(Coord2DDbl v) : Coord2DDbl(v)
		{
		}

		Size2DDbl(Double w, Double h) : Coord2DDbl(w, h)
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
