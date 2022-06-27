#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/DataComparer.h"
#include "Math/CoordinateSystem.h"
#include "Math/Point.h"

Math::Point::Point(UInt32 srid, Double x, Double y) : Vector2D(srid)
{
	this->pos = Math::Coord2DDbl(x, y);
}

Math::Point::Point(UInt32 srid, Math::Coord2DDbl pos) : Vector2D(srid)
{
	this->pos = pos;
}

Math::Point::~Point()
{
}

Math::Vector2D::VectorType Math::Point::GetVectorType() const
{
	return Math::Vector2D::VectorType::Point;
}

Math::Coord2DDbl Math::Point::GetCenter() const
{
	return this->pos;
}

Math::Vector2D *Math::Point::Clone() const
{
	Math::Point *pt;
	NEW_CLASS(pt, Math::Point(this->srid, this->pos));
	return pt;
}

void Math::Point::GetBounds(Math::RectAreaDbl *bounds) const
{
	*bounds = Math::RectAreaDbl(this->pos, this->pos);
}

Double Math::Point::CalSqrDistance(Math::Coord2DDbl pt, Math::Coord2DDbl *nearPt) const
{
	Math::Coord2DDbl diff = pt - this->pos;
	if (nearPt)
	{
		*nearPt = this->pos;
	}
	diff = diff * diff;
	return diff.x + diff.y;
}

Bool Math::Point::JoinVector(Math::Vector2D *vec)
{
	return false;
}

void Math::Point::ConvCSys(Math::CoordinateSystem *srcCSys, Math::CoordinateSystem *destCSys)
{
	Math::CoordinateSystem::ConvertXYZ(srcCSys, destCSys, this->pos.x, this->pos.y, 0, &this->pos.x, &this->pos.y, 0);
}

Bool Math::Point::Equals(Math::Vector2D *vec) const
{
	if (vec == 0)
		return false;
	if (vec->GetSRID() != this->srid)
	{
		return false;
	}
	if (vec->GetVectorType() == VectorType::Point && !vec->Support3D())
	{
		Math::Point *pt = (Math::Point*)vec;
		return Data::DataComparer::NearlyEquals(this->pos.x, pt->pos.x) && Data::DataComparer::NearlyEquals(this->pos.y, pt->pos.y);
	}
	else
	{
		return false;
	}
}
