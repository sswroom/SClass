#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/CoordinateSystem.h"
#include "Math/Point3D.h"

Math::Point3D::Point3D(Int32 srid, Double x, Double y, Double z) : Math::Point(srid, x, y)
{
	this->z = z;
}

Math::Point3D::~Point3D()
{
}

Math::Vector2D *Math::Point3D::Clone()
{
	Math::Point3D *pt;
	NEW_CLASS(pt, Math::Point3D(this->srid, this->x, this->y, this->z));
	return pt;
}

void Math::Point3D::GetCenter3D(Double *x, Double *y, Double *z)
{
	*x = this->x;
	*y = this->y;
	*z = this->z;
}

Bool Math::Point3D::Support3D()
{
	return true;
}

void Math::Point3D::ConvCSys(Math::CoordinateSystem *srcCSys, Math::CoordinateSystem *destCSys)
{
	Math::CoordinateSystem::ConvertXYZ(srcCSys, destCSys, this->x, this->y, this->z, &this->x, &this->y, &this->z);
}
