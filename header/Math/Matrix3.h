#ifndef _SM_MATH_MATRIX3
#define _SM_MATH_MATRIX3
#include "Math/Vector3.h"

namespace Math
{
	class Matrix3
	{
	public:
		Vector3 vec[3];

	public:
		Matrix3() = default;
		Matrix3(Vector3 row1, Vector3 row2, Vector3 row3);
		Matrix3(Double *val);

		void Inverse();
		void Transposition();
		void Set(NotNullPtr<const Matrix3> matrix);
		void Multiply(NotNullPtr<const Matrix3> matrix);
		void MultiplyBA(NotNullPtr<const Matrix3> matrix);
		void Multiply(Double x, Double y, Double z, OutParam<Double> outX, OutParam<Double> outY, OutParam<Double> outZ);
		Vector3 Multiply(const Vector3 &srcVec) const;
		void SetIdentity();
	};
}
#endif
