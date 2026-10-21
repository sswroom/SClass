#ifndef _SM_MATH_MATRIX3D
#define _SM_MATH_MATRIX3D
#include "Math/Vector3D.h"

namespace Math
{
	class Matrix3D
	{
	public:
		Vector3D vec[3];

	public:
		Matrix3D() = default;
		Matrix3D(Vector3D row1, Vector3D row2, Vector3D row3);
		Matrix3D(UnsafeArray<Double> val);

		void Inverse();
		void Transposition();
		void Set(const Matrix3D &matrix);
		Matrix3D Multiply(const Matrix3D &matrix) const;
		void MultiplyInplace(const Matrix3D &matrix);
		void MultiplyInplaceBA(const Matrix3D &matrix);
		void Multiply(Double x, Double y, Double z, OutParam<Double> outX, OutParam<Double> outY, OutParam<Double> outZ);
		Vector3D Multiply(const Vector3D &srcVec) const;
		void SetIdentity();
		static Matrix3D Identity();
	};
}
#endif
