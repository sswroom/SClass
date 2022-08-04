#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/CoordinateSystem.h"
#include "Math/PointZ.h"

Math::PointZ::PointZ(UInt32 srid, Double x, Double y, Double z) : Math::Point(srid, x, y)
{
	this->z = z;
}

Math::PointZ::~PointZ()
{
}

Math::Vector2D *Math::PointZ::Clone() const
{
	Math::PointZ *pt;
	NEW_CLASS(pt, Math::PointZ(this->srid, this->pos.x, this->pos.y, this->z));
	return pt;
}

Double Math::PointZ::GetZ() const
{
	return this->z;
}

void Math::PointZ::GetPos3D(Double *x, Double *y, Double *z) const
{
	*x = this->pos.x;
	*y = this->pos.y;
	*z = this->z;
}

Bool Math::PointZ::HasZ() const
{
	return true;
}

void Math::PointZ::ConvCSys(Math::CoordinateSystem *srcCSys, Math::CoordinateSystem *destCSys)
{
	Math::CoordinateSystem::ConvertXYZ(srcCSys, destCSys, this->pos.x, this->pos.y, this->z, &this->pos.x, &this->pos.y, &this->z);
}

Bool Math::PointZ::Equals(Math::Vector2D *vec) const
{
	if (vec == 0)
		return false;
	if (vec->GetSRID() != this->srid)
	{
		return false;
	}
	if (vec->GetVectorType() == VectorType::Point && vec->HasZ())
	{
		Math::PointZ *pt = (Math::PointZ*)vec;
		return this->pos == pt->pos && this->z == pt->z;
	}
	else
	{
		return false;
	}
}
