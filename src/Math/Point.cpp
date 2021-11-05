#include "Stdafx.h"
#include "MyMemory.h"
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

void Math::Point::GetCenter(Double *x, Double *y)
{
	*x = this->x;
	*y = this->y;
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
