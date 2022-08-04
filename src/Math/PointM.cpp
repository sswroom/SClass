#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/PointM.h"

Math::PointM::PointM(UInt32 srid, Double x, Double y, Double m) : Math::Point(srid, x, y)
{
	this->m = m;
}

Math::PointM::~PointM()
{
}

Math::Vector2D *Math::PointM::Clone() const
{
	Math::PointM *pt;
	NEW_CLASS(pt, Math::PointM(this->srid, this->pos.x, this->pos.y, this->m));
	return pt;
}

Double Math::PointM::GetM() const
{
	return this->m;
}

Bool Math::PointM::HasM() const
{
	return true;
}

Bool Math::PointM::Equals(Math::Vector2D *vec) const
{
	if (vec == 0)
		return false;
	if (vec->GetSRID() != this->srid)
	{
		return false;
	}
	if (vec->GetVectorType() == VectorType::Point && !vec->HasZ() && vec->HasM())
	{
		Math::PointM *pt = (Math::PointM*)vec;
		return this->pos == pt->pos && this->m == pt->m;
	}
	else
	{
		return false;
	}
}
