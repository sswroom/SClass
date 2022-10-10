#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/DataComparer.h"
#include "Math/CoordinateSystem.h"
#include "Math/Geometry/Point.h"

Math::Geometry::Point::Point(UInt32 srid, Double x, Double y) : Vector2D(srid)
{
	this->pos = Math::Coord2DDbl(x, y);
}

Math::Geometry::Point::Point(UInt32 srid, Math::Coord2DDbl pos) : Vector2D(srid)
{
	this->pos = pos;
}

Math::Geometry::Point::~Point()
{
}

Math::Geometry::Vector2D::VectorType Math::Geometry::Point::GetVectorType() const
{
	return Math::Geometry::Vector2D::VectorType::Point;
}

Math::Coord2DDbl Math::Geometry::Point::GetCenter() const
{
	return this->pos;
}

Math::Geometry::Vector2D *Math::Geometry::Point::Clone() const
{
	Math::Geometry::Point *pt;
	NEW_CLASS(pt, Math::Geometry::Point(this->srid, this->pos));
	return pt;
}

void Math::Geometry::Point::GetBounds(Math::RectAreaDbl *bounds) const
{
	*bounds = Math::RectAreaDbl(this->pos, this->pos);
}

Double Math::Geometry::Point::CalBoundarySqrDistance(Math::Coord2DDbl pt, Math::Coord2DDbl *nearPt) const
{
	Math::Coord2DDbl diff = pt - this->pos;
	if (nearPt)
	{
		*nearPt = this->pos;
	}
	diff = diff * diff;
	return diff.x + diff.y;
}

Bool Math::Geometry::Point::JoinVector(Math::Geometry::Vector2D *vec)
{
	return false;
}

void Math::Geometry::Point::ConvCSys(Math::CoordinateSystem *srcCSys, Math::CoordinateSystem *destCSys)
{
	Math::CoordinateSystem::ConvertXYZ(srcCSys, destCSys, this->pos.x, this->pos.y, 0, &this->pos.x, &this->pos.y, 0);
	this->srid = destCSys->GetSRID();
}

Bool Math::Geometry::Point::Equals(Math::Geometry::Vector2D *vec) const
{
	if (vec == 0)
		return false;
	if (vec->GetSRID() != this->srid)
	{
		return false;
	}
	if (vec->GetVectorType() == VectorType::Point && !vec->HasZ())
	{
		Math::Geometry::Point *pt = (Math::Geometry::Point*)vec;
		return Data::DataComparer::NearlyEquals(this->pos.x, pt->pos.x) && Data::DataComparer::NearlyEquals(this->pos.y, pt->pos.y);
	}
	else
	{
		return false;
	}
}

UOSInt Math::Geometry::Point::GetCoordinates(Data::ArrayListA<Math::Coord2DDbl> *coordList) const
{
	coordList->Add(this->pos);
	return 1;
}
