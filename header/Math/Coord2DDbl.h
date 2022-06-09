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

		static Coord2DDbl FromLatLon(Double lat, Double lon)
		{
			return Coord2DDbl(lon, lat);
		}

		Double CalcLengTo(Coord2DDbl coord)
		{
			Math::Coord2DDbl diff = *this - coord;
			diff = diff * diff;
			return Math_Sqrt(diff.x + diff.y);
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

		Coord2DDbl Max(Coord2DDbl v)
		{
			return PMAXPD(this->vals, v.vals);
		}

		Coord2DDbl Min(Coord2DDbl v)
		{
			return PMINPD(this->vals, v.vals);
		}

		Coord2DDbl Abs()
		{
			return Math::Coord2DDbl((this->x < 0)?-x:x, (this->y < 0)?-y:y);
		}
		
		Coord2DDbl operator+(Coord2DDbl v) const
		{
			return PADDPD(this->vals, v.vals);
		}

		Coord2DDbl operator+(Double v)
		{
			return PADDPD(this->vals, PDoublex2SetA(v));
		}

		Coord2DDbl operator-(Coord2DDbl v)
		{
			return PSUBPD(this->vals, v.vals);
		}

		Coord2DDbl operator-(Double v)
		{
			return PSUBPD(this->vals, PDoublex2SetA(v));
		}

		Coord2DDbl operator*(Coord2DDbl v)
		{
			return PMULPD(this->vals, v.vals);
		}

		Coord2DDbl operator*(Double v)
		{
			return {this->x * v, this->y * v};
		}

		Coord2DDbl operator/(Coord2DDbl v)
		{
			return PDIVPD(this->vals, v.vals);
		}

		Coord2DDbl operator/(Double v)
		{
			return {this->x / v, this->y / v};
		}

		Coord2DDbl& operator+=(Coord2DDbl v)
		{
			this->vals = PADDPD(this->vals, v.vals);
			return *this;
		}

		Bool IsZero()
		{
			return this->x == 0 && this->y == 0;
		}
	};
}
#endif
