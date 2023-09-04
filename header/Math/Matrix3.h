#ifndef _SM_MATH_MATRIX3
#define _SM_MATH_MATRIX3

namespace Math
{
	struct Vector3
	{
		Double val[3];
		
		Double operator*(const Vector3 &val);
		void Set(Double val1, Double val2, Double val3);
	};

	class Matrix3
	{
	public:
		Vector3 vec[3];

	public:
		Matrix3();
		Matrix3(Double *val);

		void Inverse();
		void Transposition();
		void Set(Matrix3 *matrix);
		void Multiply(Matrix3 *matrix);
		void MyMultiply(Matrix3 *matrix);
		void Multiply(Double x, Double y, Double z, OutParam<Double> outX, OutParam<Double> outY, OutParam<Double> outZ);
		void Multiply(Vector3 *srcVec, Vector3 *outVec);
		void SetIdentity();
	};
}
#endif
