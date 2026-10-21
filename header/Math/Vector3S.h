#ifndef _SM_MATH_VECTOR3S
#define _SM_MATH_VECTOR3S
#include "Math/Single4.h"

namespace Math
{
	struct Vector3S
	{
		Single4 v;
		
		Vector3S() = default;
		Vector3S(std::nullptr_t)
		{
			this->v = Single4(0, 0, 0, 0);
		}

		Vector3S(Single x, Single y, Single z)
		{
			this->v = Single4(x, y, z, 0);
		}

		Vector3S(Single4 v)
		{
			this->v = v;
		}
		
		Single DotProduct(const Vector3S &val) const
		{
			Single4 prod = this->v * val.v;
			return prod.x + prod.y + prod.z;
		}

		Vector3S CrossProduct(const Vector3S &val) const
		{
			return Math::Vector3S(
				this->v.y * val.v.z - this->v.z * val.v.y,
				this->v.z * val.v.x - this->v.x * val.v.z,
				this->v.x * val.v.y - this->v.y * val.v.x
			);
		}

		Vector3S operator+(const Vector3S&val) const
		{
			return Math::Vector3S(this->v + val.v);
		}

		Vector3S operator-(const Vector3S &val) const
		{
			return Math::Vector3S(this->v - val.v);
		}

		Vector3S operator+(Single val) const
		{
			return Math::Vector3S(this->v + Single4(val, val, val, 0));
		}

		Vector3S operator-(Single val) const
		{
			return Math::Vector3S(this->v - PSinglex4SetA(val));
		}

		Vector3S operator*(Single val) const
		{
			return Math::Vector3S(this->v * val);
		}

		Vector3S operator/(Single val) const
		{
			return Math::Vector3S(this->v / val);
		}

		Bool operator==(const Vector3S &val) const
		{
			return this->v == val.v;
		}

		Bool operator!=(const Vector3S &val) const
		{
			return !(*this == val);
		}

		Double GetLength() const
		{
			return Math_Sqrt(this->v.x * this->v.x + this->v.y * this->v.y + this->v.z * this->v.z);
		}

		void Set(Double x, Double y, Double z)
		{
			this->v.x = x;
			this->v.y = y;
			this->v.z = z;
		}


		Double GetX() const
		{
			return this->v.x;
		}

		Double GetY() const
		{
			return this->v.y;
		}

		Double GetZ() const
		{
			return this->v.z;
		}

		Double GetLat() const
		{
			return this->v.y;
		}

		Double GetLon() const
		{
			return this->v.x;
		}

		Double GetH() const
		{
			return this->v.z;
		}

		Math::Vector3S MulXY(Double v) const
		{
			return Math::Vector3S(this->v * v);
		}

		Bool IsZero() const
		{
			return this->v.x == 0 && this->v.y == 0 && this->v.z == 0;
		}

		Math::Vector3S Normalized() const
		{
			Double len = this->GetLength();
			if (len <= 1e-20) return Math::Vector3S(0, 0, 0);
			return *this / len;
		}

		Math::Vector3S Abs() const
		{
			return Math::Vector3S((this->v.x < 0)?-this->v.x:this->v.x,
				(this->v.y < 0)?-this->v.y:this->v.y,
				(this->v.z < 0)?-this->v.z:this->v.z);
		}

		Math::Vector3S Min(const Math::Vector3S &val) const
		{
			return Math::Vector3S(this->v.Min(val.v));
		}

		Math::Vector3S Max(const Math::Vector3S &val) const
		{
			return Math::Vector3S(this->v.Max(val.v));
		}
	};
}
#endif
