#ifndef _SM_MATH_MATRIX3
#define _SM_MATH_MATRIX3
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

		Double operator*(const Vector3 &val) const
		{
			return this->val[0] * val.val[0] + this->val[1] * val.val[1] + this->val[2] * val.val[2];
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
	};

	class Matrix3
	{
	public:
		Vector3 vec[3];

	public:
		Matrix3() = default;
		Matrix3(Double *val);

		void Inverse();
		void Transposition();
		void Set(NotNullPtr<const Matrix3> matrix);
		void Multiply(NotNullPtr<const Matrix3> matrix);
		void MyMultiply(NotNullPtr<const Matrix3> matrix);
		void Multiply(Double x, Double y, Double z, OutParam<Double> outX, OutParam<Double> outY, OutParam<Double> outZ);
		Vector3 Multiply(const Vector3 &srcVec) const;
		void SetIdentity();
	};
}
#endif
