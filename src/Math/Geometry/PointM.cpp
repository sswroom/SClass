#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Geometry/PointM.h"

Math::Geometry::PointM::PointM(UInt32 srid, Double x, Double y, Double m) : Math::Geometry::Point(srid, x, y)
{
	this->m = m;
}

Math::Geometry::PointM::~PointM()
{
}

NN<Math::Geometry::Vector2D> Math::Geometry::PointM::Clone() const
{
	NN<Math::Geometry::PointM> pt;
	NEW_CLASSNN(pt, Math::Geometry::PointM(this->srid, this->pos.x, this->pos.y, this->m));
	return pt;
}

Double Math::Geometry::PointM::GetM() const
{
	return this->m;
}

Bool Math::Geometry::PointM::HasM() const
{
	return true;
}

Bool Math::Geometry::PointM::GetMBounds(OutParam<Double> min, OutParam<Double> max) const
{
	min.Set(this->m);
	max.Set(this->m);
	return true;
}

Bool Math::Geometry::PointM::Equals(NN<const Math::Geometry::Vector2D> vec, Bool sameTypeOnly, Bool nearlyVal, Bool no3DGeometry) const
{
	if (vec->GetSRID() != this->srid)
	{
		return false;
	}
	if (vec->GetVectorType() == VectorType::Point && (no3DGeometry || (!vec->HasZ() && vec->HasM())))
	{
		const Math::Geometry::PointM *pt = (const Math::Geometry::PointM*)vec.Ptr();
		if (no3DGeometry)
		{
			if (nearlyVal)
				return this->pos.EqualsNearly(pt->pos);
			else
				return this->pos == pt->pos;
		}
		else
		{
			if (nearlyVal)
				return this->pos.EqualsNearly(pt->pos) &&
					Math::NearlyEqualsDbl(this->m, pt->m);
			else
				return this->pos == pt->pos && this->m == pt->m;
		}
	}
	else
	{
		return false;
	}
}
