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

NN<Math::Geometry::Vector2D> Math::Geometry::Point::Clone() const
{
	NN<Math::Geometry::Point> pt;
	NEW_CLASSNN(pt, Math::Geometry::Point(this->srid, this->pos));
	return pt;
}

Math::RectAreaDbl Math::Geometry::Point::GetBounds() const
{
	return Math::RectAreaDbl(this->pos, this->pos);
}

Double Math::Geometry::Point::CalBoundarySqrDistance(Math::Coord2DDbl pt, OutParam<Math::Coord2DDbl> nearPt) const
{
	Math::Coord2DDbl diff = pt - this->pos;
	nearPt.Set(this->pos);
	diff = diff * diff;
	return diff.x + diff.y;
}

Double Math::Geometry::Point::CalArea() const
{
	return 0;
}

Bool Math::Geometry::Point::JoinVector(NN<const Math::Geometry::Vector2D> vec)
{
	return false;
}

Bool Math::Geometry::Point::GetZBounds(OutParam<Double> min, OutParam<Double> max) const
{
	return false;
}

Bool Math::Geometry::Point::GetMBounds(OutParam<Double> min, OutParam<Double> max) const
{
	return false;
}

void Math::Geometry::Point::Convert(NN<Math::CoordinateConverter> converter)
{
	this->pos = converter->Convert2D(this->pos);
	this->srid = converter->GetOutputSRID();
}

Bool Math::Geometry::Point::Equals(NN<const Math::Geometry::Vector2D> vec, Bool sameTypeOnly, Bool nearlyVal) const
{
	if (vec->GetSRID() != this->srid)
	{
		return false;
	}
	if (vec->GetVectorType() == VectorType::Point && !vec->HasZ())
	{
		Math::Geometry::Point *pt = (Math::Geometry::Point*)vec.Ptr();
		if (nearlyVal)
			return this->pos.EqualsNearly(pt->pos);
		else
			return this->pos.Equals(pt->pos);
	}
	else
	{
		return false;
	}
}

UOSInt Math::Geometry::Point::GetCoordinates(NN<Data::ArrayListA<Math::Coord2DDbl>> coordList) const
{
	coordList->Add(this->pos);
	return 1;
}

void Math::Geometry::Point::SwapXY()
{
	this->pos = this->pos.SwapXY();
}

void Math::Geometry::Point::MultiplyCoordinatesXY(Double v)
{
	this->pos = this->pos * v;
}

Bool Math::Geometry::Point::InsideOrTouch(Math::Coord2DDbl coord) const
{
	return this->pos == coord;
}

UOSInt Math::Geometry::Point::GetPointCount() const
{
	return 1;
}
