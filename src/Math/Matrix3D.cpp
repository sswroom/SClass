#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math_C.h"
#include "Math/Matrix3D.h"

Math::Matrix3D::Matrix3D(Vector3D row1, Vector3D row2, Vector3D row3)
{
	this->vec[0] = row1;
	this->vec[1] = row2;
	this->vec[2] = row3;
}

Math::Matrix3D::Matrix3D(UnsafeArray<Double> val)
{
	this->vec[0] = Math::Vector3D(val[0], val[1], val[2]);
	this->vec[1] = Math::Vector3D(val[3], val[4], val[5]);
	this->vec[2] = Math::Vector3D(val[6], val[7], val[8]);
}

void Math::Matrix3D::Inverse()
{
	Vector3D outVal[3];
	Double det;
	outVal[0] = Vector3D( (vec[1].xy.y * vec[2].zn.x - vec[1].zn.x * vec[2].xy.y),
						 -(vec[0].xy.y * vec[2].zn.x - vec[0].zn.x * vec[2].xy.y),
						 (vec[0].xy.y * vec[1].zn.x - vec[0].zn.x * vec[1].xy.y));
	outVal[1] = Vector3D( -(vec[1].xy.x * vec[2].zn.x - vec[1].zn.x * vec[2].xy.x),
						 (vec[0].xy.x * vec[2].zn.x - vec[0].zn.x * vec[2].xy.x),
						 -(vec[0].xy.x * vec[1].zn.x - vec[0].zn.x * vec[1].xy.x));
	outVal[2] = Vector3D( (vec[1].xy.x * vec[2].xy.y - vec[1].xy.y * vec[2].xy.x),
						 -(vec[0].xy.x * vec[2].xy.y - vec[0].xy.y * vec[2].xy.x),
						 (vec[0].xy.x * vec[1].xy.y - vec[0].xy.y * vec[1].xy.x));
	det = 1 / (vec[0].xy.x * outVal[0].xy.x + vec[0].xy.y * outVal[1].xy.x + vec[0].zn.x * outVal[2].xy.x);
	this->vec[0] = outVal[0] * det;
	this->vec[1] = outVal[1] * det;
	this->vec[2] = outVal[2] * det;
}

void Math::Matrix3D::Transposition()
{
	Double t;
	t = this->vec[0].xy.y;
	this->vec[0].xy.y = this->vec[1].xy.x;
	this->vec[1].xy.x = t;
	t = this->vec[0].zn.x;
	this->vec[0].zn.x = this->vec[2].xy.x;
	this->vec[2].xy.x = t;
	t = this->vec[1].zn.x;
	this->vec[1].zn.x = this->vec[2].xy.y;
	this->vec[2].xy.y = t;
}


void Math::Matrix3D::Set(const Matrix3D &matrix)
{
	this->vec[0] = matrix.vec[0];
	this->vec[1] = matrix.vec[1];
	this->vec[2] = matrix.vec[2];
}

Math::Matrix3D Math::Matrix3D::Multiply(const Matrix3D &matrix) const
{
	Matrix3D result;
	result.Set(*this);
	result.MultiplyInplace(matrix);
	return result;
}

void Math::Matrix3D::MultiplyInplace(const Matrix3D &matrix)
{
	Vector3D tmpVec[3];
	tmpVec[0] = this->vec[0];
	tmpVec[1] = this->vec[1];
	tmpVec[2] = this->vec[2];
	this->vec[0].xy.x = tmpVec[0].xy.x * matrix.vec[0].xy.x + tmpVec[0].xy.y * matrix.vec[1].xy.x + tmpVec[0].zn.x * matrix.vec[2].xy.x;
	this->vec[0].xy.y = tmpVec[0].xy.x * matrix.vec[0].xy.y + tmpVec[0].xy.y * matrix.vec[1].xy.y + tmpVec[0].zn.x * matrix.vec[2].xy.y;
	this->vec[0].zn.x = tmpVec[0].xy.x * matrix.vec[0].zn.x + tmpVec[0].xy.y * matrix.vec[1].zn.x + tmpVec[0].zn.x * matrix.vec[2].zn.x;
	this->vec[1].xy.x = tmpVec[1].xy.x * matrix.vec[0].xy.x + tmpVec[1].xy.y * matrix.vec[1].xy.x + tmpVec[1].zn.x * matrix.vec[2].xy.x;
	this->vec[1].xy.y = tmpVec[1].xy.x * matrix.vec[0].xy.y + tmpVec[1].xy.y * matrix.vec[1].xy.y + tmpVec[1].zn.x * matrix.vec[2].xy.y;
	this->vec[1].zn.x = tmpVec[1].xy.x * matrix.vec[0].zn.x + tmpVec[1].xy.y * matrix.vec[1].zn.x + tmpVec[1].zn.x * matrix.vec[2].zn.x;
	this->vec[2].xy.x = tmpVec[2].xy.x * matrix.vec[0].xy.x + tmpVec[2].xy.y * matrix.vec[1].xy.x + tmpVec[2].zn.x * matrix.vec[2].xy.x;
	this->vec[2].xy.y = tmpVec[2].xy.x * matrix.vec[0].xy.y + tmpVec[2].xy.y * matrix.vec[1].xy.y + tmpVec[2].zn.x * matrix.vec[2].xy.y;
	this->vec[2].zn.x = tmpVec[2].xy.x * matrix.vec[0].zn.x + tmpVec[2].xy.y * matrix.vec[1].zn.x + tmpVec[2].zn.x * matrix.vec[2].zn.x;
}

void Math::Matrix3D::MultiplyInplaceBA(const Matrix3D &matrix)
{
	Vector3D tmpVec[3];
	tmpVec[0] = this->vec[0];
	tmpVec[1] = this->vec[1];
	tmpVec[2] = this->vec[2];
	this->vec[0].xy.x = tmpVec[0].xy.x * matrix.vec[0].xy.x + tmpVec[1].xy.x * matrix.vec[0].xy.y + tmpVec[2].xy.x * matrix.vec[0].zn.x;
	this->vec[0].xy.y = tmpVec[0].xy.y * matrix.vec[0].xy.x + tmpVec[1].xy.y * matrix.vec[0].xy.y + tmpVec[2].xy.y * matrix.vec[0].zn.x;
	this->vec[0].zn.x = tmpVec[0].zn.x * matrix.vec[0].xy.x + tmpVec[1].zn.x * matrix.vec[0].xy.y + tmpVec[2].zn.x * matrix.vec[0].zn.x;
	this->vec[1].xy.x = tmpVec[0].xy.x * matrix.vec[1].xy.x + tmpVec[1].xy.x * matrix.vec[1].xy.y + tmpVec[2].xy.x * matrix.vec[1].zn.x;
	this->vec[1].xy.y = tmpVec[0].xy.y * matrix.vec[1].xy.x + tmpVec[1].xy.y * matrix.vec[1].xy.y + tmpVec[2].xy.y * matrix.vec[1].zn.x;
	this->vec[1].zn.x = tmpVec[0].zn.x * matrix.vec[1].xy.x + tmpVec[1].zn.x * matrix.vec[1].xy.y + tmpVec[2].zn.x * matrix.vec[1].zn.x;
	this->vec[2].xy.x = tmpVec[0].xy.x * matrix.vec[2].xy.x + tmpVec[1].xy.x * matrix.vec[2].xy.y + tmpVec[2].xy.x * matrix.vec[2].zn.x;
	this->vec[2].xy.y = tmpVec[0].xy.y * matrix.vec[2].xy.x + tmpVec[1].xy.y * matrix.vec[2].xy.y + tmpVec[2].xy.y * matrix.vec[2].zn.x;
	this->vec[2].zn.x = tmpVec[0].zn.x * matrix.vec[2].xy.x + tmpVec[1].zn.x * matrix.vec[2].xy.y + tmpVec[2].zn.x * matrix.vec[2].zn.x;
}

void Math::Matrix3D::Multiply(Double x, Double y, Double z, OutParam<Double> outX, OutParam<Double> outY, OutParam<Double> outZ)
{
	outX.Set(x * this->vec[0].xy.x + y * this->vec[0].xy.y + z * this->vec[0].zn.x);
	outY.Set(x * this->vec[1].xy.x + y * this->vec[1].xy.y + z * this->vec[1].zn.x);
	outZ.Set(x * this->vec[2].xy.x + y * this->vec[2].xy.y + z * this->vec[2].zn.x);
}

Math::Vector3D Math::Matrix3D::Multiply(const Vector3D &srcVec) const
{
	return Math::Vector3D(this->vec[0].DotProduct(srcVec), this->vec[1].DotProduct(srcVec), this->vec[2].DotProduct(srcVec));
}

void Math::Matrix3D::SetIdentity()
{
	this->vec[0].Set(1.0, 0.0, 0.0);
	this->vec[1].Set(0.0, 1.0, 0.0);
	this->vec[2].Set(0.0, 0.0, 1.0);
}

Math::Matrix3D Math::Matrix3D::Identity()
{
	Matrix3D mat;
	mat.SetIdentity();
	return mat;
}

