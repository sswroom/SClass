#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/CoordinateSystem.h"
#include "Math/Ellipse.h"
#include "Math/Math.h"

Math::Ellipse::Ellipse(UInt32 srid, Double tlx, Double tly, Double w, Double h) : Math::Vector2D(srid)
{
	this->tlx = tlx;
	this->tly = tly;
	this->w = w;
	this->h = h;
}

Math::Ellipse::~Ellipse()
{
}

Math::Vector2D::VectorType Math::Ellipse::GetVectorType()
{
	return Math::Vector2D::VT_ELLIPSE;
}

void Math::Ellipse::GetCenter(Double *x, Double *y)
{
	*x = this->tlx + (this->w * 0.5);
	*y = this->tly + (this->h * 0.5);
}

Math::Vector2D *Math::Ellipse::Clone()
{
	Math::Ellipse *ellipse;
	NEW_CLASS(ellipse, Math::Ellipse(this->srid, this->tlx, this->tly, this->w, this->h));
	return ellipse;
}

void Math::Ellipse::GetBounds(Double *minX, Double *minY, Double *maxX, Double *maxY)
{
	*minX = this->tlx;
	*minY = this->tly;
	*maxX = this->tlx + this->w;
	*maxY = this->tly + this->h;
}

Double Math::Ellipse::CalSqrDistance(Double x, Double y, Double *nearPtX, Double *nearPtY)
{
	//////////////////////////////////////////////////////////
	*nearPtX = x;
	*nearPtY = y;
	return 0;
}

Bool Math::Ellipse::JoinVector(Math::Vector2D *vec)
{
	return false;
}

Bool Math::Ellipse::Support3D()
{
	return false;
}

void Math::Ellipse::ConvCSys(Math::CoordinateSystem *srcCSys, Math::CoordinateSystem *destCSys)
{
	Double x2 = this->tlx + this->w;
	Double y2 = this->tly + this->h;
	Math::CoordinateSystem::ConvertXYZ(srcCSys, destCSys, this->tlx, this->tly, 0, &this->tlx, &this->tly, 0);
	Math::CoordinateSystem::ConvertXYZ(srcCSys, destCSys, x2, y2, 0, &x2, &y2, 0);
	this->w = x2 - this->tlx;
	this->h = y2 - this->tly;
}
