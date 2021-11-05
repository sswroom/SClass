#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/CoordinateSystem.h"
#include "Math/Math.h"
#include "Math/PieArea.h"

Math::PieArea::PieArea(Double cx, Double cy, Double r, Double arcAngle1, Double arcAngle2)
{
	this->cx = cx;
	this->cy = cy;
	this->r = r;
	this->arcAngle1 = arcAngle1;
	this->arcAngle2 = arcAngle2;
}

Math::PieArea::~PieArea()
{
}

Math::Vector2D::VectorType Math::PieArea::GetVectorType()
{
	return Math::Vector2D::VectorType::PIEAREA;
}

void Math::PieArea::GetCenter(Double *x, Double *y)
{
	Double a = (this->arcAngle1 + this->arcAngle2) * 0.5;
	*x = this->cx + r * Math::Sin(a) * 0.5;
	*y = this->cy - r * Math::Cos(a) * 0.5;
}

Math::Vector2D *Math::PieArea::Clone()
{
	Math::PieArea *pie;
	NEW_CLASS(pie, Math::PieArea(this->cx, this->cy, this->r, this->arcAngle1, this->arcAngle2));
	return pie;
}

void Math::PieArea::GetBounds(Double *minX, Double *minY, Double *maxX, Double *maxY)
{
	//////////////////////////////////////////////////////////
	*minX = this->cx - this->r;
	*minY = this->cy - this->r;
	*maxX = this->cx + this->r;
	*maxY = this->cy + this->r;
}

Double Math::PieArea::CalSqrDistance(Double x, Double y, Double *nearPtX, Double *nearPtY)
{
	//////////////////////////////////////////////////////////
	*nearPtX = x;
	*nearPtY = y;
	return 0;
}

Bool Math::PieArea::JoinVector(Math::Vector2D *vec)
{
	return false;
}

Bool Math::PieArea::Support3D()
{
	return false;
}

void Math::PieArea::ConvCSys(Math::CoordinateSystem *srcCSys, Math::CoordinateSystem *destCSys)
{
	Math::CoordinateSystem::ConvertXYZ(srcCSys, destCSys, this->cx, this->cy, 0, &this->cx, &this->cy, 0);
}

Double Math::PieArea::GetCX()
{
	return this->cx;
}

Double Math::PieArea::GetCY()
{
	return this->cy;
}

Double Math::PieArea::GetR()
{
	return this->r;
}

Double Math::PieArea::GetArcAngle1()
{
	return this->arcAngle1;
}

Double Math::PieArea::GetArcAngle2()
{
	return this->arcAngle2;
}

