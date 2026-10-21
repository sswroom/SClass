#ifndef _SM_MATH_SINGLE4
#define _SM_MATH_SINGLE4
#include "SIMD.h"
#include "Math/Math_C.h"

namespace Math
{
	struct Single4
	{
		union
		{
			struct
			{
				Single x;
				Single y;
				Single z;
				Single w;
			};
			Singlex4 vals;
		};
		

		Single4() = default;
		Single4(std::nullptr_t)
		{
			this->vals = PSinglex4SetA(0);
		}

		Single4(Single x, Single y, Single z, Single w)
		{
			this->vals = PSinglex4Set(x, y, z, w);
		}

		Single4(const Singlex4 vals)
		{
			this->vals = vals;
		}

		Bool operator!=(Single4 v) const
		{
			return (this->x != v.x) || (this->y != v.y) || (this->z != v.z) || (this->w != v.w);
		}

		Bool operator==(Single4 v) const
		{
			return (this->x == v.x) && (this->y == v.y) && (this->z == v.z) && (this->w == v.w);
		}

		Bool operator>=(Single4 v) const
		{
			return (this->x >= v.x) && (this->y >= v.y) && (this->z >= v.z) && (this->w >= v.w);
		}

		Bool operator<(Single4 v) const
		{
			return (this->x < v.x) && (this->y < v.y) && (this->z < v.z) && (this->w < v.w);
		}

		Bool Equals(const Single4 &v) const
		{
			return this->x == v.x && this->y == v.y && this->z == v.z && this->w == v.w;
		}

		Bool EqualsNearly(const Single4 &v) const
		{
			return Math::NearlyEqualsSng(this->x, v.x) && Math::NearlyEqualsSng(this->y, v.y) && Math::NearlyEqualsSng(this->z, v.z) && Math::NearlyEqualsSng(this->w, v.w);
		}

		Single4 Max(Single4 v) const
		{
			return PMAXPS(this->vals, v.vals);
		}

		Single4 Min(Single4 v) const
		{
			return PMINPS(this->vals, v.vals);
		}

		Single4 Abs() const
		{
			return Math::Single4((this->x < 0)?-this->x:this->x, (this->y < 0)?-this->y:this->y, (this->z < 0)?-this->z:this->z, (this->w < 0)?-this->w:this->w);
		}
		
		Single4 operator+(Single4 v) const
		{
			return PADDPS(this->vals, v.vals);
		}

		Single4 operator+(Single v) const
		{
			return PADDPS(this->vals, PSinglex4SetA(v));
		}

		Single4 operator-() const
		{
			return Single4(-this->x, -this->y, -this->z, -this->w);
		}

		Single4 operator-(Single4 v) const
		{
			return PSUBPS(this->vals, v.vals);
		}

		Single4 operator-(Single v) const
		{
			return PSUBPS(this->vals, PSinglex4SetA(v));
		}

		Single4 operator*(Single4 v) const
		{
			return PMULPS(this->vals, v.vals);
		}

		Single4 operator*(Single v) const
		{
			return PMULPS(this->vals, PSinglex4SetA(v));
		}

		Single4 operator/(Single4 v) const
		{
			return PDIVPS(this->vals, v.vals);
		}

		Single4 operator/(Single v) const
		{
			return PDIVPS(this->vals, PSinglex4SetA(v));
		}

		Single4& operator+=(Single4 v)
		{
			this->vals = PADDPS(this->vals, v.vals);
			return NNTHIS;
		}

		Single4& operator-=(Single4 v)
		{
			this->vals = PSUBPS(this->vals, v.vals);
			return NNTHIS;
		}

		Single SumSqr() const
		{
			Single4 sqr = NNTHIS * this->vals;
			return sqr.x + sqr.y + sqr.z + sqr.w;
		}

		Bool IsZero() const
		{
			return this->x == 0 && this->y == 0 && this->z == 0 && this->w == 0;
		}
	};
}
#endif
