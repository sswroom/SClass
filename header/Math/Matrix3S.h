#ifndef _SM_MATH_MATRIX3S
#define _SM_MATH_MATRIX3S
#include "Math/Vector3S.h"

namespace Math
{
	class Matrix3S
	{
	public:
		Vector3S vec[3];

	public:
		Matrix3S() = default;
		Matrix3S(Vector3S row1, Vector3S row2, Vector3S row3);
		Matrix3S(Double *val);

		void Inverse();
		void Transposition();
		void Set(const Matrix3S &matrix);
		Matrix3S Multiply(const Matrix3S &matrix) const;
		void MultiplyInplace(const Matrix3S &matrix);
		void MultiplyInplaceBA(const Matrix3S &matrix);
		void Multiply(Double x, Double y, Double z, OutParam<Double> outX, OutParam<Double> outY, OutParam<Double> outZ);
		Vector3S Multiply(const Vector3S &srcVec) const;
		void SetIdentity();
		static Matrix3S Identity();
	};
}
#endif
