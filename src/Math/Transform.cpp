#include "Stdafx.h"
#include "Math/Transform.h"

Math::Transform::Transform(Matrix3 axis, Vector3 origin)
{
	this->axis = axis;
	this->origin = origin;
}

Math::Vector3 Math::Transform::TransformPoint(const Vector3 &point) const
{
	return this->axis.Multiply(point) + this->origin;
}

Math::Vector3 Math::Transform::TransformVector(const Vector3 &vector) const
{
	return this->axis.Multiply(vector);
}

void Math::Transform::SetIdentity()
{
	this->axis.SetIdentity();
	this->origin.Set(0.0, 0.0, 0.0);
}

Math::Transform Math::Transform::Identity()
{
	return Transform(Matrix3::Identity(), Vector3(0.0, 0.0, 0.0));
}

Math::Transform Math::Transform::Compose(const Transform &other) const
{
	Matrix3 newAxis = this->axis;
	newAxis.MultiplyInplace(other.axis);
	return Transform(
		newAxis,
		this->axis.Multiply(other.origin) + this->origin
	);
}

Math::Transform Math::Transform::InversePlacement() const
{
	Matrix3 invAxis;
	invAxis.vec[0] = Math::Vector3(this->axis.vec[0].GetX(), this->axis.vec[1].GetX(), this->axis.vec[2].GetX());
	invAxis.vec[1] = Math::Vector3(this->axis.vec[0].GetY(), this->axis.vec[1].GetY(), this->axis.vec[2].GetY());
	invAxis.vec[2] = Math::Vector3(this->axis.vec[0].GetZ(), this->axis.vec[1].GetZ(), this->axis.vec[2].GetZ());
	Vector3 invOrigin = invAxis.Multiply(this->origin) * -1.0;
	return Transform(invAxis, invOrigin);
}
