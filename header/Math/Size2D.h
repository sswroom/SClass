#ifndef _SM_MATH_SIZE2D
#define _SM_MATH_SIZE2D
#include "Math/Value2D.h"

namespace Math
{
	template <typename T> struct Size2D : public Value2D<T>
	{
		Size2D() = default;
		Size2D(Value2D<T> v) : Value2D<T>(v)
		{
		}

		Size2D(T w, T h) : Value2D<T>(w, h)
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

		Bool HasArea() const
		{
			return this->x > 0 && this->y > 0;
		}
	};
}
#endif
