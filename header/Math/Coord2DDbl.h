#ifndef _SM_MATH_COORD2DDBL
#define _SM_MATH_COORD2DDBL
#include "SIMD.h"
#include "Math/Math.h"

namespace Math
{
	struct Coord2DDbl
	{
		union
		{
			struct
			{
				Double x;
				Double y;
			};
			struct
			{
				Double lon;
				Double lat;
			};
			Doublex2 vals;
		};
		

		Coord2DDbl() = default;
		Coord2DDbl(void *)
		{
			this->x = 0;
			this->y = 0;
		}

		Coord2DDbl(Double x, Double y)
		{
			this->x = x;
			this->y = y;
		}

		Coord2DDbl(Doublex2 vals)
		{
			this->vals = vals;
		}

		Double CalcLengTo(Coord2DDbl coord)
		{
			return Math_Sqrt(Math_Sqr((Double)(this->x - coord.x)) + Math_Sqr((Double)(this->y - coord.y)));
		}

		Bool operator!=(Coord2DDbl v)
		{
			return (this->x != v.x) || (this->y != v.y);
		}

		Bool operator==(Coord2DDbl v)
		{
			return (this->x == v.x) && (this->y == v.y);
		}

		Bool operator>=(Coord2DDbl v)
		{
			return (this->x >= v.x) && (this->y >= v.y);
		}

		Bool operator<(Coord2DDbl v)
		{
			return (this->x < v.x) && (this->y < v.y);
		}

		Coord2DDbl operator+(Coord2DDbl v)
		{
			return this->vals + v.vals;
		}

		Coord2DDbl operator-(Coord2DDbl v)
		{
			return this->vals - v.vals;
		}

		Coord2DDbl operator*(Coord2DDbl v)
		{
			return this->vals * v.vals;
		}

		Coord2DDbl operator*(Double v)
		{
			return {this->x * v, this->y * v};
		}

		Coord2DDbl operator/(Coord2DDbl v)
		{
			return this->vals / v.vals;
		}

		Coord2DDbl operator/(Double v)
		{
			return {this->x / v, this->y / v};
		}

		Coord2DDbl& operator+=(Coord2DDbl v)
		{
			this->vals += v.vals;
			return *this;
		}
	};
}
#endif
