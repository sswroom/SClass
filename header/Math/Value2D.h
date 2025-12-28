#ifndef _SM_MATH_VALUE2D
#define _SM_MATH_VALUE2D
#include "Math/Double2D.h"
#include "Math/Math_C.h"

namespace Math
{
	template <typename T> struct Value2D
	{
		union
		{
			struct
			{
				T x;
				T y;
			};
			T vals[2];
		};
		

		Value2D() = default;
		Value2D(std::nullptr_t)
		{
			this->x = 0;
			this->y = 0;
		}

		Value2D(T x, T y)
		{
			this->x = x;
			this->y = y;
		}

		static Value2D<UOSInt> UOSIntFromDouble(Double2D v)
		{
			return Value2D<UOSInt>((UOSInt)Double2OSInt(v.x), (UOSInt)Double2OSInt(v.y));
		}

		Math::Double2D ToDouble() const
		{
			return Math::Double2D((Double)this->x, (Double)this->y);
		}

		Bool operator!=(Value2D<T> v) const
		{
			return (this->x != v.x) || (this->y != v.y);
		}

		Bool operator==(Value2D<T> v) const
		{
			return (this->x == v.x) && (this->y == v.y);
		}

		Bool operator>=(Value2D<T> v) const
		{
			return (this->x >= v.x) && (this->y >= v.y);
		}

		Bool operator<(Value2D<T> v) const
		{
			return (this->x < v.x) && (this->y < v.y);
		}

		Value2D<T> operator+(Value2D<T> v) const
		{
			return {this->x + v.x, this->y + v.y};
		}

		Value2D<T> operator+(T v) const
		{
			return {this->x + v, this->y + v};
		}

		Value2D<T> operator-(Value2D<T> v) const
		{
			return {this->x - v.x, this->y - v.y};
		}

		Value2D<T> operator-(T v) const
		{
			return {this->x - v, this->y - v};
		}

		Value2D<T> operator*(Value2D<T> v) const
		{
			return {this->x * v.x, this->y * v.y};
		}

		Value2D<T> operator*(T v) const
		{
			return {this->x * v, this->y * v};
		}

		Value2D<T> operator/(Value2D<T> v) const
		{
			return {this->x / v.x, this->y / v.y};
		}

		Value2D<T> operator/(T v) const
		{
			return {this->x / v, this->y / v};
		}

		Value2D<T>& operator+=(Value2D<T> v)
		{
			this->x += v.x;
			this->y += v.y;
			return NNTHIS;
		}

		Math::Value2D<T> SwapXY() const
		{
			return Math::Value2D<T>(this->y, this->x);
		}
	};
}
#endif
