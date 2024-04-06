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

NotNullPtr<Math::Geometry::Vector2D> Math::Geometry::PointZM::Clone() const
{
	NotNullPtr<Math::Geometry::PointZM> pt;
	NEW_CLASSNN(pt, Math::Geometry::PointZM(this->srid, this->pos.x, this->pos.y, this->z, this->m));
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

Bool Math::Geometry::PointZM::GetMBounds(OutParam<Double> min, OutParam<Double> max) const
{
	min.Set(this->m);
	max.Set(this->m);
	return true;
}

Bool Math::Geometry::PointZM::Equals(NotNullPtr<const Math::Geometry::Vector2D> vec, Bool sameTypeOnly, Bool nearlyVal) const
{
	if (vec->GetSRID() != this->srid)
	{
		return false;
	}
	if (vec->GetVectorType() == VectorType::Point && vec->HasZ() && vec->HasM())
	{
		const Math::Geometry::PointZM *pt = (const Math::Geometry::PointZM*)vec.Ptr();
		if (nearlyVal)
			return this->pos.EqualsNearly(pt->pos) &&
					Math::NearlyEqualsDbl(this->z, pt->z) &&
					Math::NearlyEqualsDbl(this->m, pt->m);
		else
			return this->pos == pt->pos && this->z == pt->z && this->m == pt->m;
	}
	else
	{
		return false;
	}
}
