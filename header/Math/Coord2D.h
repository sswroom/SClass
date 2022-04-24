#ifndef _SM_MATH_COORD2D
#define _SM_MATH_COORD2D

namespace Math
{
	template <typename T> struct Coord2D
	{
		T x;
		T y;

		Coord2D() = default;

		Coord2D(T x, T y)
		{
			this->x = x;
			this->y = y;
		}

		Bool operator!=(Coord2D<T> v)
		{
			return (this->x != v.x) || (this->y != v.y);
		}

		Bool operator==(Coord2D<T> v)
		{
			return (this->x == v.x) && (this->y == v.y);
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
	};
}
#endif
