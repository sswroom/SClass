#ifndef _SM_MATH_SIZE2D
#define _SM_MATH_SIZE2D
#include "Math/Coord2D.h"
#include "Math/Size2DDbl.h"

namespace Math
{
	template <typename T> struct Size2D : public Coord2D<T>
	{
		Size2D() = default;
		Size2D(Coord2D<T> v) : Coord2D<T>(v)
		{
		}

		Size2D(T w, T h) : Coord2D<T>(w, h)
		{
		}

		T GetWidth() const
		{
			return this->x;
		}

		T GetHeight() const
		{
			return this->y;
		}

		T CalcArea() const
		{
			return this->x * this->y;
		}

		Size2DDbl ToDouble() const
		{
			return this->Coord2D<T>::ToDouble();
		}

		Bool HasArea() const
		{
			return this->x > 0 && this->y > 0;
		}
	};
}
#endif
