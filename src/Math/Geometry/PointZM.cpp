#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Geometry/PointZM.h"

Math::Geometry::PointZM::PointZM(UInt32 srid, Double x, Double y, Double z, Double m) : Math::Geometry::PointZ(srid, x, y, z)
{
	this->m = m;
}

Math::Geometry::PointZM::~PointZM()
{
}

Math::Geometry::Vector2D *Math::Geometry::PointZM::Clone() const
{
	Math::Geometry::PointZM *pt;
	NEW_CLASS(pt, Math::Geometry::PointZM(this->srid, this->pos.x, this->pos.y, this->z, this->m));
	return pt;
}

Double Math::Geometry::PointZM::GetM() const
{
	return this->m;
}

Bool Math::Geometry::PointZM::HasM() const
{
	return true;
}

Bool Math::Geometry::PointZM::Equals(Math::Geometry::Vector2D *vec) const
{
	if (vec == 0)
		return false;
	if (vec->GetSRID() != this->srid)
	{
		return false;
	}
	if (vec->GetVectorType() == VectorType::Point && vec->HasZ() && vec->HasM())
	{
		Math::Geometry::PointZM *pt = (Math::Geometry::PointZM*)vec;
		return this->pos == pt->pos && this->z == pt->z && this->m == pt->m;
	}
	else
	{
		return false;
	}
}

Bool Math::Geometry::PointZM::EqualsNearly(Math::Geometry::Vector2D *vec) const
{
	if (vec == 0)
		return false;
	if (vec->GetSRID() != this->srid)
	{
		return false;
	}
	if (vec->GetVectorType() == VectorType::Point && vec->HasZ() && vec->HasM())
	{
		Math::Geometry::PointZM *pt = (Math::Geometry::PointZM*)vec;
		return Math::NearlyEqualsDbl(this->pos.x, pt->pos.x) &&
				Math::NearlyEqualsDbl(this->pos.y, pt->pos.y) &&
				Math::NearlyEqualsDbl(this->z, pt->z) &&
				Math::NearlyEqualsDbl(this->m, pt->m);
	}
	else
	{
		return false;
	}
}
