#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/DataComparer.h"
#include "Math/CoordinateSystem.h"
#include "Math/Point.h"

Math::Point::Point(UInt32 srid, Double x, Double y) : Vector2D(srid)
{
	this->x = x;
	this->y = y;
}

Math::Point::~Point()
{
}

Math::Vector2D::VectorType Math::Point::GetVectorType()
{
	return Math::Vector2D::VectorType::Point;
}

Math::Coord2DDbl Math::Point::GetCenter()
{
	return Math::Coord2DDbl(this->x, this->y);
}

Math::Vector2D *Math::Point::Clone()
{
	Math::Point *pt;
	NEW_CLASS(pt, Math::Point(this->srid, this->x, this->y));
	return pt;
}

void Math::Point::GetBounds(Double *minX, Double *minY, Double *maxX, Double *maxY)
{
	*minX = this->x;
	*minY = this->y;
	*maxX = this->x;
	*maxY = this->y;
}

Double Math::Point::CalSqrDistance(Double x, Double y, Double *nearPtX, Double *nearPtY)
{
	Double xDiff = x - this->x;
	Double yDiff = y - this->y;
	if (nearPtX && nearPtY)
	{
		*nearPtX = this->x;
		*nearPtY = this->y;
	}
	return xDiff * xDiff + yDiff * yDiff;
}

Bool Math::Point::JoinVector(Math::Vector2D *vec)
{
	return false;
}

void Math::Point::ConvCSys(Math::CoordinateSystem *srcCSys, Math::CoordinateSystem *destCSys)
{
	Math::CoordinateSystem::ConvertXYZ(srcCSys, destCSys, this->x, this->y, 0, &this->x, &this->y, 0);
}

Bool Math::Point::Equals(Math::Vector2D *vec)
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
		return Data::DataComparer::NearlyEquals(this->x, pt->x) && Data::DataComparer::NearlyEquals(this->y, pt->y);
	}
	else
	{
		return false;
	}
}
