#ifndef _SM_MATH_VECTOR3D
#define _SM_MATH_VECTOR3D
#include "Math/Double2D.h"

namespace Math
{
	struct Vector3D
	{
		Double2D xy;
		Double2D zn;
		
		Vector3D() = default;
		Vector3D(std::nullptr_t)
		{
			this->xy = Double2D(0, 0);
			this->zn = Double2D(0, 0);
		}

		Vector3D(Double x, Double y, Double z)
		{
			this->xy = Double2D(x, y);
			this->zn = Double2D(z, 0);
		}

		Vector3D(Double2D xy, Double z)
		{
			this->xy = xy;
			this->zn = Double2D(z, 0);
		}

		Vector3D(Double2D xy, Double2D zn)
		{
			this->xy = xy;
			this->zn = zn;
		}

		Double DotProduct(const Vector3D &val) const
		{
			return this->xy.x * val.xy.x + this->xy.y * val.xy.y + this->zn.x * val.zn.x;
		}

		Vector3D CrossProduct(const Vector3D &val) const
		{
			return Math::Vector3D(
				this->xy.y * val.zn.x - this->zn.x * val.xy.y,
				this->zn.x * val.xy.x - this->xy.x * val.zn.x,
				this->xy.x * val.xy.y - this->xy.y * val.xy.x
			);
		}

		Vector3D operator+(const Vector3D &val) const
		{
			return Math::Vector3D(this->xy + val.xy, this->zn + val.zn);
		}

		Vector3D operator-(const Vector3D &val) const
		{
			return Math::Vector3D(this->xy - val.xy, this->zn - val.zn);
		}

		Vector3D operator+(Double val) const
		{
			return Math::Vector3D(this->xy + val, this->zn + val);
		}

		Vector3D operator-(Double val) const
		{
			return Math::Vector3D(this->xy - val, this->zn - val);
		}

		Vector3D operator*(Double val) const
		{
			return Math::Vector3D(this->xy * val, this->zn * val);
		}

		Vector3D operator/(Double val) const
		{
			return Math::Vector3D(this->xy / val, this->zn / val);
		}

		Bool operator==(const Vector3D &val) const
		{
			return this->xy == val.xy && this->zn.x == val.zn.x;
		}

		Bool operator!=(const Vector3D &val) const
		{
			return !(*this == val);
		}

		Double GetLength() const
		{
			return Math_Sqrt(this->xy.x * this->xy.x + this->xy.y * this->xy.y + this->zn.x * this->zn.x);
		}

		void Set(Double x, Double y, Double z)
		{
			this->xy = Double2D(x, y);
			this->zn = Double2D(z, 0);
		}

		void Set(Double2D xy, Double z)
		{
			this->xy = xy;
			this->zn = Double2D(z, 0);
		}

		Double2D GetXY() const
		{
			return this->xy;
		}

		Double GetX() const
		{
			return this->xy.x;
		}

		Double GetY() const
		{
			return this->xy.y;
		}

		Double GetZ() const
		{
			return this->zn.x;
		}

		Double GetLat() const
		{
			return this->xy.y;
		}

		Double GetLon() const
		{
			return this->xy.x;
		}

		Double GetH() const
		{
			return this->zn.x;
		}

		Math::Vector3D MulXY(Double v) const
		{
			return Math::Vector3D(this->xy * v, this->zn * v);
		}

		Bool IsZero() const
		{
			return this->xy.x == 0 && this->xy.y == 0 && this->zn.x == 0;
		}

		Math::Vector3D Normalized() const
		{
			Double len = this->GetLength();
			if (len <= 1e-20) return Math::Vector3D(0, 0, 0);
			return *this / len;
		}

		Math::Vector3D Abs() const
		{
			return Math::Vector3D((this->xy.x < 0)?-this->xy.x:this->xy.x,
				(this->xy.y < 0)?-this->xy.y:this->xy.y,
				(this->zn.x < 0)?-this->zn.x:this->zn.x);
		}

		Math::Vector3D Min(const Math::Vector3D &val) const
		{
			return Math::Vector3D(this->xy.Min(val.xy), this->zn.Min(val.zn));
		}

		Math::Vector3D Max(const Math::Vector3D &val) const
		{
			return Math::Vector3D(this->xy.Max(val.xy), this->zn.Max(val.zn));
		}
	};
}
#endif
