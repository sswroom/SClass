#include "stdafx.h"
#include "MyMemory.h"
#include "Math/MultiPoint.h"
#include <memory.h>

Math::MultiPoint::MultiPoint(OSInt nPoints)
{
	this->points = MemAlloc(Double, nPoints << 1);
	this->nPoints = nPoints;
	MemClear(this->points, sizeof(Double) * (nPoints << 1));
	this->part = 0;
}

Math::MultiPoint::~MultiPoint()
{
	MemFree(points);
}

Math::Vector2D::VectorType Math::MultiPoint::GetVectorType()
{
	return Math::Vector2D::VectorType::MULTIPOINT;
}

Int32 *Math::MultiPoint::GetPartList(OSInt *nParts)
{
	*nParts = 1;
	return &this->part;
}

Double *Math::MultiPoint::GetPointList(OSInt *nPoints)
{
	*nPoints = this->nPoints;
	return this->points;
}
