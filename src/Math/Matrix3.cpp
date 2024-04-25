#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Math/Matrix3.h"

Math::Matrix3::Matrix3(Vector3 row1, Vector3 row2, Vector3 row3)
{
	this->vec[0] = row1;
	this->vec[1] = row2;
	this->vec[2] = row3;
}

Math::Matrix3::Matrix3(Double *val)
{
	this->vec[0].val[0] = val[0];
	this->vec[0].val[1] = val[1];
	this->vec[0].val[2] = val[2];
	this->vec[1].val[0] = val[3];
	this->vec[1].val[1] = val[4];
	this->vec[1].val[2] = val[5];
	this->vec[2].val[0] = val[6];
	this->vec[2].val[1] = val[7];
	this->vec[2].val[2] = val[8];
}

void Math::Matrix3::Inverse()
{
	Double outVal[9];
	Double det;
	outVal[0] =  (vec[1].val[1] * vec[2].val[2] - vec[1].val[2] * vec[2].val[1]);
	outVal[1] = -(vec[0].val[1] * vec[2].val[2] - vec[0].val[2] * vec[2].val[1]);
	outVal[2] =  (vec[0].val[1] * vec[1].val[2] - vec[0].val[2] * vec[1].val[1]);
	outVal[3] = -(vec[1].val[0] * vec[2].val[2] - vec[1].val[2] * vec[2].val[0]);
	outVal[4] =  (vec[0].val[0] * vec[2].val[2] - vec[0].val[2] * vec[2].val[0]);
	outVal[5] = -(vec[0].val[0] * vec[1].val[2] - vec[0].val[2] * vec[1].val[0]);
	outVal[6] =  (vec[1].val[0] * vec[2].val[1] - vec[1].val[1] * vec[2].val[0]);
	outVal[7] = -(vec[0].val[0] * vec[2].val[1] - vec[0].val[1] * vec[2].val[0]);
	outVal[8] =  (vec[0].val[0] * vec[1].val[1] - vec[0].val[1] * vec[1].val[0]);
	det = 1 / (vec[0].val[0] * outVal[0] + vec[0].val[1] * outVal[3] + vec[0].val[2] * outVal[6]);
	this->vec[0].val[0] = outVal[0] * det;
	this->vec[0].val[1] = outVal[1] * det;
	this->vec[0].val[2] = outVal[2] * det;
	this->vec[1].val[0] = outVal[3] * det;
	this->vec[1].val[1] = outVal[4] * det;
	this->vec[1].val[2] = outVal[5] * det;
	this->vec[2].val[0] = outVal[6] * det;
	this->vec[2].val[1] = outVal[7] * det;
	this->vec[2].val[2] = outVal[8] * det;
}

void Math::Matrix3::Transposition()
{
	Double t;
	t = this->vec[0].val[1];
	this->vec[0].val[1] = this->vec[1].val[0];
	this->vec[1].val[0] = t;
	t = this->vec[0].val[2];
	this->vec[0].val[2] = this->vec[2].val[0];
	this->vec[2].val[0] = t;
	t = this->vec[1].val[2];
	this->vec[1].val[2] = this->vec[2].val[1];
	this->vec[2].val[1] = t;
}


void Math::Matrix3::Set(NN<const Matrix3> matrix)
{
	this->vec[0] = matrix->vec[0];
	this->vec[1] = matrix->vec[1];
	this->vec[2] = matrix->vec[2];
}

void Math::Matrix3::Multiply(NN<const Matrix3> matrix)
{
	Vector3 tmpVec[3];
	tmpVec[0] = this->vec[0];
	tmpVec[1] = this->vec[1];
	tmpVec[2] = this->vec[2];
	this->vec[0].val[0] = tmpVec[0].val[0] * matrix->vec[0].val[0] + tmpVec[0].val[1] * matrix->vec[1].val[0] + tmpVec[0].val[2] * matrix->vec[2].val[0];
	this->vec[0].val[1] = tmpVec[0].val[0] * matrix->vec[0].val[1] + tmpVec[0].val[1] * matrix->vec[1].val[1] + tmpVec[0].val[2] * matrix->vec[2].val[1];
	this->vec[0].val[2] = tmpVec[0].val[0] * matrix->vec[0].val[2] + tmpVec[0].val[1] * matrix->vec[1].val[2] + tmpVec[0].val[2] * matrix->vec[2].val[2];
	this->vec[1].val[0] = tmpVec[1].val[0] * matrix->vec[0].val[0] + tmpVec[1].val[1] * matrix->vec[1].val[0] + tmpVec[1].val[2] * matrix->vec[2].val[0];
	this->vec[1].val[1] = tmpVec[1].val[0] * matrix->vec[0].val[1] + tmpVec[1].val[1] * matrix->vec[1].val[1] + tmpVec[1].val[2] * matrix->vec[2].val[1];
	this->vec[1].val[2] = tmpVec[1].val[0] * matrix->vec[0].val[2] + tmpVec[1].val[1] * matrix->vec[1].val[2] + tmpVec[1].val[2] * matrix->vec[2].val[2];
	this->vec[2].val[0] = tmpVec[2].val[0] * matrix->vec[0].val[0] + tmpVec[2].val[1] * matrix->vec[1].val[0] + tmpVec[2].val[2] * matrix->vec[2].val[0];
	this->vec[2].val[1] = tmpVec[2].val[0] * matrix->vec[0].val[1] + tmpVec[2].val[1] * matrix->vec[1].val[1] + tmpVec[2].val[2] * matrix->vec[2].val[1];
	this->vec[2].val[2] = tmpVec[2].val[0] * matrix->vec[0].val[2] + tmpVec[2].val[1] * matrix->vec[1].val[2] + tmpVec[2].val[2] * matrix->vec[2].val[2];
}

void Math::Matrix3::MultiplyBA(NN<const Matrix3> matrix)
{
	Vector3 tmpVec[3];
	tmpVec[0] = this->vec[0];
	tmpVec[1] = this->vec[1];
	tmpVec[2] = this->vec[2];
	this->vec[0].val[0] = tmpVec[0].val[0] * matrix->vec[0].val[0] + tmpVec[1].val[0] * matrix->vec[0].val[1] + tmpVec[2].val[0] * matrix->vec[0].val[2];
	this->vec[0].val[1] = tmpVec[0].val[1] * matrix->vec[0].val[0] + tmpVec[1].val[1] * matrix->vec[0].val[1] + tmpVec[2].val[1] * matrix->vec[0].val[2];
	this->vec[0].val[2] = tmpVec[0].val[2] * matrix->vec[0].val[0] + tmpVec[1].val[2] * matrix->vec[0].val[1] + tmpVec[2].val[2] * matrix->vec[0].val[2];
	this->vec[1].val[0] = tmpVec[0].val[0] * matrix->vec[1].val[0] + tmpVec[1].val[0] * matrix->vec[1].val[1] + tmpVec[2].val[0] * matrix->vec[1].val[2];
	this->vec[1].val[1] = tmpVec[0].val[1] * matrix->vec[1].val[0] + tmpVec[1].val[1] * matrix->vec[1].val[1] + tmpVec[2].val[1] * matrix->vec[1].val[2];
	this->vec[1].val[2] = tmpVec[0].val[2] * matrix->vec[1].val[0] + tmpVec[1].val[2] * matrix->vec[1].val[1] + tmpVec[2].val[2] * matrix->vec[1].val[2];
	this->vec[2].val[0] = tmpVec[0].val[0] * matrix->vec[2].val[0] + tmpVec[1].val[0] * matrix->vec[2].val[1] + tmpVec[2].val[0] * matrix->vec[2].val[2];
	this->vec[2].val[1] = tmpVec[0].val[1] * matrix->vec[2].val[0] + tmpVec[1].val[1] * matrix->vec[2].val[1] + tmpVec[2].val[1] * matrix->vec[2].val[2];
	this->vec[2].val[2] = tmpVec[0].val[2] * matrix->vec[2].val[0] + tmpVec[1].val[2] * matrix->vec[2].val[1] + tmpVec[2].val[2] * matrix->vec[2].val[2];
}

void Math::Matrix3::Multiply(Double x, Double y, Double z, OutParam<Double> outX, OutParam<Double> outY, OutParam<Double> outZ)
{
	outX.Set(x * this->vec[0].val[0] + y * this->vec[0].val[1] + z * this->vec[0].val[2]);
	outY.Set(x * this->vec[1].val[0] + y * this->vec[1].val[1] + z * this->vec[1].val[2]);
	outZ.Set(x * this->vec[2].val[0] + y * this->vec[2].val[1] + z * this->vec[2].val[2]);
}

Math::Vector3 Math::Matrix3::Multiply(const Vector3 &srcVec) const
{
	return Math::Vector3(this->vec[0].DotProduct(srcVec), this->vec[1].DotProduct(srcVec), this->vec[2].DotProduct(srcVec));
}

void Math::Matrix3::SetIdentity()
{
	this->vec[0].Set(1.0, 0.0, 0.0);
	this->vec[1].Set(0.0, 1.0, 0.0);
	this->vec[2].Set(0.0, 0.0, 1.0);
}
