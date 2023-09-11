#ifndef _SM_MATH_VECTOR3
#define _SM_MATH_VECTOR3
#include "Math/Double2D.h"

namespace Math
{
	struct Vector3
	{
		Double val[3];
		
		Vector3() = default;
		Vector3(Double x, Double y, Double z)
		{
			this->val[0] = x;
			this->val[1] = y;
			this->val[2] = z;
		}

		Vector3(Double2D xy, Double z)
		{
			this->val[0] = xy.x;
			this->val[1] = xy.y;
			this->val[2] = z;
		}

		Double DotProduct(const Vector3 &val) const
		{
			return this->val[0] * val.val[0] + this->val[1] * val.val[1] + this->val[2] * val.val[2];
		}

		Vector3 CrossProduct(const Vector3 &val) const
		{
			return Math::Vector3(
				this->val[1] * val.val[2] - this->val[2] * val.val[1],
				this->val[2] * val.val[0] - this->val[0] * val.val[2],
				this->val[0] * val.val[1] - this->val[1] * val.val[0]
			);
		}

		Vector3 operator+(const Vector3 &val) const
		{
			return Math::Vector3(this->val[0] + val.val[0], this->val[1] + val.val[1], this->val[2] + val.val[2]);
		}

		Vector3 operator-(const Vector3 &val) const
		{
			return Math::Vector3(this->val[0] - val.val[0], this->val[1] - val.val[1], this->val[2] - val.val[2]);
		}

		Vector3 operator+(Double val) const
		{
			return Math::Vector3(this->val[0] + val, this->val[1] + val, this->val[2] + val);
		}

		Vector3 operator-(Double val) const
		{
			return Math::Vector3(this->val[0] - val, this->val[1] - val, this->val[2] - val);
		}

		Vector3 operator*(Double val) const
		{
			return Math::Vector3(this->val[0] * val, this->val[1] * val, this->val[2] * val);
		}

		Vector3 operator/(Double val) const
		{
			return Math::Vector3(this->val[0] / val, this->val[1] / val, this->val[2] / val);
		}

		Double GetLength() const
		{
			return Math_Sqrt(this->val[0] * this->val[0] + this->val[1] * this->val[1] + this->val[2] * this->val[2]);
		}

		void Set(Double x, Double y, Double z)
		{
			this->val[0] = x;
			this->val[1] = y;
			this->val[2] = z;
		}

		void Set(Double2D xy, Double z)
		{
			this->val[0] = xy.x;
			this->val[1] = xy.y;
			this->val[2] = z;
		}

		Double2D GetXY() const
		{
			return Math::Double2D(this->val[0], this->val[1]);
		}

		Double GetX() const
		{
			return this->val[0];
		}

		Double GetY() const
		{
			return this->val[1];
		}

		Double GetZ() const
		{
			return this->val[2];
		}

		Double GetLat() const
		{
			return this->val[1];
		}

		Double GetLon() const
		{
			return this->val[0];
		}

		Double GetH() const
		{
			return this->val[2];
		}

		Math::Vector3 MulXY(Double v) const
		{
			return Math::Vector3(this->val[0] * v, this->val[1] * v, this->val[2]);
		}

		Bool IsZero() const
		{
			return this->val[0] == 0 && this->val[1] == 0 && this->val[2] == 0;
		}

		Math::Vector3 Abs() const
		{
			return Math::Vector3((this->val[0] < 0)?-this->val[0]:this->val[0],
				(this->val[1] < 0)?-this->val[1]:this->val[1],
				(this->val[2] < 0)?-this->val[2]:this->val[2]);
		}
	};
}
#endif
