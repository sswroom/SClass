#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/PointZM.h"

Math::PointZM::PointZM(UInt32 srid, Double x, Double y, Double z, Double m) : Math::PointZ(srid, x, y, z)
{
	this->m = m;
}

Math::PointZM::~PointZM()
{
}

Math::Vector2D *Math::PointZM::Clone() const
{
	Math::PointZM *pt;
	NEW_CLASS(pt, Math::PointZM(this->srid, this->pos.x, this->pos.y, this->z, this->m));
	return pt;
}

Double Math::PointZM::GetM() const
{
	return this->m;
}

Bool Math::PointZM::HasM() const
{
	return true;
}

Bool Math::PointZM::Equals(Math::Vector2D *vec) const
{
	if (vec == 0)
		return false;
	if (vec->GetSRID() != this->srid)
	{
		return false;
	}
	if (vec->GetVectorType() == VectorType::Point && vec->HasZ() && vec->HasM())
	{
		Math::PointZM *pt = (Math::PointZM*)vec;
		return this->pos == pt->pos && this->z == pt->z && this->m == pt->m;
	}
	else
	{
		return false;
	}
}
