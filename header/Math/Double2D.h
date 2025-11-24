#ifndef _SM_MATH_DOUBLE2D
#define _SM_MATH_DOUBLE2D
#include "SIMD.h"
#include "Math/Math_C.h"

namespace Math
{
	struct Double2D
	{
		union
		{
			struct
			{
				Double x;
				Double y;
			};
			Doublex2 vals;
		};
		

		Double2D() = default;
		Double2D(std::nullptr_t)
		{
			this->x = 0;
			this->y = 0;
		}

		Double2D(Double x, Double y)
		{
			this->x = x;
			this->y = y;
		}

		Double2D(const Doublex2 vals)
		{
			this->vals = vals;
		}

		Bool operator!=(Double2D v) const
		{
			return (this->x != v.x) || (this->y != v.y);
		}

		Bool operator==(Double2D v) const
		{
			return (this->x == v.x) && (this->y == v.y);
		}

		Bool operator>=(Double2D v) const
		{
			return (this->x >= v.x) && (this->y >= v.y);
		}

		Bool operator<(Double2D v) const
		{
			return (this->x < v.x) && (this->y < v.y);
		}

		Bool Equals(const Double2D &v) const
		{
			return this->x == v.x && this->y == v.y;
		}

		Bool EqualsNearly(const Double2D &v) const
		{
			return Math::NearlyEqualsDbl(this->x, v.x) && Math::NearlyEqualsDbl(this->y, v.y);
		}

		Double2D Max(Double2D v) const
		{
			return PMAXPD(this->vals, v.vals);
		}

		Double2D Min(Double2D v) const
		{
			return PMINPD(this->vals, v.vals);
		}

		Double2D Abs() const
		{
			return Math::Double2D((this->x < 0)?-x:x, (this->y < 0)?-y:y);
		}
		
		Double2D operator+(Double2D v) const
		{
			return PADDPD(this->vals, v.vals);
		}

		Double2D operator+(Double v) const
		{
			return PADDPD(this->vals, PDoublex2SetA(v));
		}

		Double2D operator-(Double2D v) const
		{
			return PSUBPD(this->vals, v.vals);
		}

		Double2D operator-(Double v) const
		{
			return PSUBPD(this->vals, PDoublex2SetA(v));
		}

		Double2D operator*(Double2D v) const
		{
			return PMULPD(this->vals, v.vals);
		}

		Double2D operator*(Double v) const
		{
			return {this->x * v, this->y * v};
		}

		Double2D operator/(Double2D v) const
		{
			return PDIVPD(this->vals, v.vals);
		}

		Double2D operator/(Double v) const
		{
			return {this->x / v, this->y / v};
		}

		Double2D& operator+=(Double2D v)
		{
			this->vals = PADDPD(this->vals, v.vals);
			return NNTHIS;
		}

		Double2D& operator-=(Double2D v)
		{
			this->vals = PSUBPD(this->vals, v.vals);
			return NNTHIS;
		}

		Double SumSqr() const
		{
			Double2D sqr = NNTHIS * this->vals;
			return sqr.x + sqr.y;
		}

		Bool IsZero() const
		{
			return this->x == 0 && this->y == 0;
		}

		Double2D SwapXY() const
		{
			return Double2D(this->y, this->x);
		}
	};
}
#endif
