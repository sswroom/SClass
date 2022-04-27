#ifndef _SM_MATH_COORD2D
#define _SM_MATH_COORD2D
#include "Math/Coord2DDbl.h"
#include "Math/Math.h"

namespace Math
{
	template <typename T> struct Coord2D
	{
		union
		{
			struct
			{
				T x;
				T y;
			};
			struct
			{
				T lon;
				T lat;
			};
			T vals[2];
		};
		

		Coord2D() = default;
		Coord2D(void *)
		{
			this->x = 0;
			this->y = 0;
		}
		Coord2D(T x, T y)
		{
			this->x = x;
			this->y = y;
		}

		Double CalcLengTo(Coord2D<T> coord)
		{
			return Math_Sqrt(Math_Sqr((Double)(this->x - coord.x)) + Math_Sqr((Double)(this->y - coord.y)));
		}

		Math::Coord2DDbl ToDouble()
		{
			return Math::Coord2DDbl((Double)this->x, (Double)this->y);
		}

		Bool operator!=(Coord2D<T> v)
		{
			return (this->x != v.x) || (this->y != v.y);
		}

		Bool operator==(Coord2D<T> v)
		{
			return (this->x == v.x) && (this->y == v.y);
		}

		Bool operator>=(Coord2D<T> v)
		{
			return (this->x >= v.x) && (this->y >= v.y);
		}

		Bool operator<(Coord2D<T> v)
		{
			return (this->x < v.x) && (this->y < v.y);
		}

		Coord2D<T> operator+(Coord2D<T> v)
		{
			return {this->x + v.x, this->y + v.y};
		}

		Coord2D<T> operator-(Coord2D<T> v)
		{
			return {this->x - v.x, this->y - v.y};
		}

		Coord2D<T> operator*(Coord2D<T> v)
		{
			return {this->x * v.x, this->y * v.y};
		}

		Coord2D<T> operator*(T v)
		{
			return {this->x * v, this->y * v};
		}

		Coord2D<T> operator/(Coord2D<T> v)
		{
			return {this->x / v.x, this->y / v.y};
		}

		Coord2D<T> operator/(T v)
		{
			return {this->x / v, this->y / v};
		}

		Coord2D<T>& operator+=(Coord2D<T> v)
		{
			this->x += v.x;
			this->y += v.y;
			return *this;
		}
	};
}
#endif
