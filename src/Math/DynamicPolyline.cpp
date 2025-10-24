#include "stdafx.h"
#include "MyMemory.h"
#include "Math/DynamicPolyline.h"
#include "Math/Math_C.h"
#include <memory.h>

Math::DynamicPolyline::DynamicPolyline(Double x, Double y) : Math::Polyline(1, 10)
{
	this->pointCapacity = 10;
	this->nPoints = 1;
	this->points[0] = x;
	this->points[1] = y;
}

Math::DynamicPolyline::~DynamicPolyline()
{
}

void Math::DynamicPolyline::AddPoint(Double x, Double y)
{
	if (this->pointCapacity <= this->nPoints)
	{
		this->pointCapacity = this->pointCapacity << 1;
		Double *newPoints = MemAlloc(Double, this->pointCapacity << 1);
		MemCopy(newPoints, this->points, sizeof(Double) * this->pointCapacity);
		MemFree(this->points);
		this->points = newPoints;
	}
	this->points[(this->nPoints << 1) + 0] = x;
	this->points[(this->nPoints << 1) + 1] = y;
	this->nPoints++;
}

void Math::DynamicPolyline::ChangeLastPoint(Double x, Double y)
{
	this->points[(this->nPoints << 1) - 2] = x;
	this->points[(this->nPoints << 1) - 1] = y;
}

void Math::DynamicPolyline::GetLastPoint(Double *x, Double *y)
{
	*x = this->points[(this->nPoints << 1) - 2];
	*y = this->points[(this->nPoints << 1) - 1];
}

Bool Math::DynamicPolyline::RemoveLastPoint()
{
	if (this->nPoints > 1)
	{
		this->nPoints--;
		return true;
	}
	return false;
}
