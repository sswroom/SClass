#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/CoordinateSystem.h"
#include "Math/Point3D.h"

Math::Point3D::Point3D(UInt32 srid, Double x, Double y, Double z) : Math::Point(srid, x, y)
{
	this->z = z;
}

Math::Point3D::~Point3D()
{
}

Math::Vector2D *Math::Point3D::Clone() const
{
	Math::Point3D *pt;
	NEW_CLASS(pt, Math::Point3D(this->srid, this->pos.x, this->pos.y, this->z));
	return pt;
}

void Math::Point3D::GetCenter3D(Double *x, Double *y, Double *z) const
{
	*x = this->pos.x;
	*y = this->pos.y;
	*z = this->z;
}

Bool Math::Point3D::Support3D() const
{
	return true;
}

void Math::Point3D::ConvCSys(Math::CoordinateSystem *srcCSys, Math::CoordinateSystem *destCSys)
{
	Math::CoordinateSystem::ConvertXYZ(srcCSys, destCSys, this->pos.x, this->pos.y, this->z, &this->pos.x, &this->pos.y, &this->z);
}

Bool Math::Point3D::Equals(Math::Vector2D *vec) const
{
	if (vec == 0)
		return false;
	if (vec->GetSRID() != this->srid)
	{
		return false;
	}
	if (vec->GetVectorType() == VectorType::Point && vec->Support3D())
	{
		Math::Point3D *pt = (Math::Point3D*)vec;
		return this->pos == pt->pos && this->z == pt->z;
	}
	else
	{
		return false;
	}
}
