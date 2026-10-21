#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math_C.h"
#include "Math/Matrix3S.h"

Math::Matrix3S::Matrix3S(Vector3S row1, Vector3S row2, Vector3S row3)
{
	this->vec[0] = row1;
	this->vec[1] = row2;
	this->vec[2] = row3;
}

Math::Matrix3S::Matrix3S(Double *val)
{
	this->vec[0] = Math::Vector3S(val[0], val[1], val[2]);
	this->vec[1] = Math::Vector3S(val[3], val[4], val[5]);
	this->vec[2] = Math::Vector3S(val[6], val[7], val[8]);
}

void Math::Matrix3S::Inverse()
{
	Vector3S outVal[3];
	Double det;
	outVal[0] = Vector3S( (vec[1].v.y * vec[2].v.z - vec[1].v.z * vec[2].v.y),
						 -(vec[0].v.y * vec[2].v.z - vec[0].v.z * vec[2].v.y),
						 (vec[0].v.y * vec[1].v.z - vec[0].v.z * vec[1].v.y));
	outVal[1] = Vector3S( -(vec[1].v.x * vec[2].v.z - vec[1].v.z * vec[2].v.x),
						 (vec[0].v.x * vec[2].v.z - vec[0].v.z * vec[2].v.x),
						 -(vec[0].v.x * vec[1].v.z - vec[0].v.z * vec[1].v.x));
	outVal[2] = Vector3S( (vec[1].v.x * vec[2].v.y - vec[1].v.y * vec[2].v.x),
						 -(vec[0].v.x * vec[2].v.y - vec[0].v.y * vec[2].v.x),
						 (vec[0].v.x * vec[1].v.y - vec[0].v.y * vec[1].v.x));
	det = 1 / (vec[0].v.x * outVal[0].v.x + vec[0].v.y * outVal[1].v.x + vec[0].v.z * outVal[2].v.x);
	this->vec[0] = outVal[0] * det;
	this->vec[1] = outVal[1] * det;
	this->vec[2] = outVal[2] * det;
}

void Math::Matrix3S::Transposition()
{
	Double t;
	t = this->vec[0].v.y;
	this->vec[0].v.y = this->vec[1].v.x;
	this->vec[1].v.x = t;
	t = this->vec[0].v.z;
	this->vec[0].v.z = this->vec[2].v.x;
	this->vec[2].v.x = t;
	t = this->vec[1].v.z;
	this->vec[1].v.z = this->vec[2].v.y;
	this->vec[2].v.y = t;
}


void Math::Matrix3S::Set(const Matrix3S &matrix)
{
	this->vec[0] = matrix.vec[0];
	this->vec[1] = matrix.vec[1];
	this->vec[2] = matrix.vec[2];
}

Math::Matrix3S Math::Matrix3S::Multiply(const Matrix3S &matrix) const
{
	Matrix3S result;
	result.Set(*this);
	result.MultiplyInplace(matrix);
	return result;
}

void Math::Matrix3S::MultiplyInplace(const Matrix3S &matrix)
{
	Vector3S tmpVec[3];
	tmpVec[0] = this->vec[0];
	tmpVec[1] = this->vec[1];
	tmpVec[2] = this->vec[2];
	this->vec[0].v.x = tmpVec[0].v.x * matrix.vec[0].v.x + tmpVec[0].v.y * matrix.vec[1].v.x + tmpVec[0].v.z * matrix.vec[2].v.x;
	this->vec[0].v.y = tmpVec[0].v.x * matrix.vec[0].v.y + tmpVec[0].v.y * matrix.vec[1].v.y + tmpVec[0].v.z * matrix.vec[2].v.y;
	this->vec[0].v.z = tmpVec[0].v.x * matrix.vec[0].v.z + tmpVec[0].v.y * matrix.vec[1].v.z + tmpVec[0].v.z * matrix.vec[2].v.z;
	this->vec[1].v.x = tmpVec[1].v.x * matrix.vec[0].v.x + tmpVec[1].v.y * matrix.vec[1].v.x + tmpVec[1].v.z * matrix.vec[2].v.x;
	this->vec[1].v.y = tmpVec[1].v.x * matrix.vec[0].v.y + tmpVec[1].v.y * matrix.vec[1].v.y + tmpVec[1].v.z * matrix.vec[2].v.y;
	this->vec[1].v.z = tmpVec[1].v.x * matrix.vec[0].v.z + tmpVec[1].v.y * matrix.vec[1].v.z + tmpVec[1].v.z * matrix.vec[2].v.z;
	this->vec[2].v.x = tmpVec[2].v.x * matrix.vec[0].v.x + tmpVec[2].v.y * matrix.vec[1].v.x + tmpVec[2].v.z * matrix.vec[2].v.x;
	this->vec[2].v.y = tmpVec[2].v.x * matrix.vec[0].v.y + tmpVec[2].v.y * matrix.vec[1].v.y + tmpVec[2].v.z * matrix.vec[2].v.y;
	this->vec[2].v.z = tmpVec[2].v.x * matrix.vec[0].v.z + tmpVec[2].v.y * matrix.vec[1].v.z + tmpVec[2].v.z * matrix.vec[2].v.z;
}

void Math::Matrix3S::MultiplyInplaceBA(const Matrix3S &matrix)
{
	Vector3S tmpVec[3];
	tmpVec[0] = this->vec[0];
	tmpVec[1] = this->vec[1];
	tmpVec[2] = this->vec[2];
	this->vec[0].v.x = tmpVec[0].v.x * matrix.vec[0].v.x + tmpVec[1].v.x * matrix.vec[0].v.y + tmpVec[2].v.x * matrix.vec[0].v.z;
	this->vec[0].v.y = tmpVec[0].v.y * matrix.vec[0].v.x + tmpVec[1].v.y * matrix.vec[0].v.y + tmpVec[2].v.y * matrix.vec[0].v.z;
	this->vec[0].v.z = tmpVec[0].v.z * matrix.vec[0].v.x + tmpVec[1].v.z * matrix.vec[0].v.y + tmpVec[2].v.z * matrix.vec[0].v.z;
	this->vec[1].v.x = tmpVec[0].v.x * matrix.vec[1].v.x + tmpVec[1].v.x * matrix.vec[1].v.y + tmpVec[2].v.x * matrix.vec[1].v.z;
	this->vec[1].v.y = tmpVec[0].v.y * matrix.vec[1].v.x + tmpVec[1].v.y * matrix.vec[1].v.y + tmpVec[2].v.y * matrix.vec[1].v.z;
	this->vec[1].v.z = tmpVec[0].v.z * matrix.vec[1].v.x + tmpVec[1].v.z * matrix.vec[1].v.y + tmpVec[2].v.z * matrix.vec[1].v.z;
	this->vec[2].v.x = tmpVec[0].v.x * matrix.vec[2].v.x + tmpVec[1].v.x * matrix.vec[2].v.y + tmpVec[2].v.x * matrix.vec[2].v.z;
	this->vec[2].v.y = tmpVec[0].v.y * matrix.vec[2].v.x + tmpVec[1].v.y * matrix.vec[2].v.y + tmpVec[2].v.y * matrix.vec[2].v.z;
	this->vec[2].v.z = tmpVec[0].v.z * matrix.vec[2].v.x + tmpVec[1].v.z * matrix.vec[2].v.y + tmpVec[2].v.z * matrix.vec[2].v.z;
}

void Math::Matrix3S::Multiply(Double x, Double y, Double z, OutParam<Double> outX, OutParam<Double> outY, OutParam<Double> outZ)
{
	outX.Set(x * this->vec[0].v.x + y * this->vec[0].v.y + z * this->vec[0].v.z);
	outY.Set(x * this->vec[1].v.x + y * this->vec[1].v.y + z * this->vec[1].v.z);
	outZ.Set(x * this->vec[2].v.x + y * this->vec[2].v.y + z * this->vec[2].v.z);
}

Math::Vector3S Math::Matrix3S::Multiply(const Vector3S &srcVec) const
{
	return Math::Vector3S(this->vec[0].DotProduct(srcVec), this->vec[1].DotProduct(srcVec), this->vec[2].DotProduct(srcVec));
}

void Math::Matrix3S::SetIdentity()
{
	this->vec[0].Set(1.0, 0.0, 0.0);
	this->vec[1].Set(0.0, 1.0, 0.0);
	this->vec[2].Set(0.0, 0.0, 1.0);
}

Math::Matrix3S Math::Matrix3S::Identity()
{
	Matrix3S mat;
	mat.SetIdentity();
	return mat;
}

