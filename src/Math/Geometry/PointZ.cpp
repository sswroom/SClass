#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/CoordinateSystem.h"
#include "Math/Geometry/PointZ.h"

Math::Geometry::PointZ::PointZ(UInt32 srid, Double x, Double y, Double z) : Math::Geometry::Point(srid, x, y)
{
	this->z = z;
}

Math::Geometry::PointZ::~PointZ()
{
}

Math::Geometry::Vector2D *Math::Geometry::PointZ::Clone() const
{
	Math::Geometry::PointZ *pt;
	NEW_CLASS(pt, Math::Geometry::PointZ(this->srid, this->pos.x, this->pos.y, this->z));
	return pt;
}

Double Math::Geometry::PointZ::GetZ() const
{
	return this->z;
}

void Math::Geometry::PointZ::GetPos3D(Double *x, Double *y, Double *z) const
{
	*x = this->pos.x;
	*y = this->pos.y;
	*z = this->z;
}

Bool Math::Geometry::PointZ::HasZ() const
{
	return true;
}

void Math::Geometry::PointZ::ConvCSys(Math::CoordinateSystem *srcCSys, Math::CoordinateSystem *destCSys)
{
	Math::CoordinateSystem::ConvertXYZ(srcCSys, destCSys, this->pos.x, this->pos.y, this->z, &this->pos.x, &this->pos.y, &this->z);
	this->srid = destCSys->GetSRID();
}

Bool Math::Geometry::PointZ::Equals(Math::Geometry::Vector2D *vec) const
{
	if (vec == 0)
		return false;
	if (vec->GetSRID() != this->srid)
	{
		return false;
	}
	if (vec->GetVectorType() == VectorType::Point && vec->HasZ())
	{
		Math::Geometry::PointZ *pt = (Math::Geometry::PointZ*)vec;
		return this->pos == pt->pos && this->z == pt->z;
	}
	else
	{
		return false;
	}
}

Bool Math::Geometry::PointZ::EqualsNearly(Math::Geometry::Vector2D *vec) const
{
	if (vec == 0)
		return false;
	if (vec->GetSRID() != this->srid)
	{
		return false;
	}
	if (vec->GetVectorType() == VectorType::Point && vec->HasZ() && !vec->HasM())
	{
		Math::Geometry::PointZ *pt = (Math::Geometry::PointZ*)vec;
		return this->pos.EqualsNearly(pt->pos) &&
				Math::NearlyEqualsDbl(this->z, pt->z);
	}
	else
	{
		return false;
	}
}
