#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/CoordinateSystem.h"
#include "Math/Math.h"
#include "Math/Geometry/Ellipse.h"

Math::Geometry::Ellipse::Ellipse(UInt32 srid, Double tlx, Double tly, Double w, Double h) : Math::Geometry::Vector2D(srid)
{
	this->tlx = tlx;
	this->tly = tly;
	this->w = w;
	this->h = h;
}

Math::Geometry::Ellipse::~Ellipse()
{
}

Math::Geometry::Vector2D::VectorType Math::Geometry::Ellipse::GetVectorType() const
{
	return Math::Geometry::Vector2D::VectorType::Ellipse;
}

Math::Coord2DDbl Math::Geometry::Ellipse::GetCenter() const
{
	return Math::Coord2DDbl(this->tlx + (this->w * 0.5), this->tly + (this->h * 0.5));
}

Math::Geometry::Vector2D *Math::Geometry::Ellipse::Clone() const
{
	Math::Geometry::Ellipse *ellipse;
	NEW_CLASS(ellipse, Math::Geometry::Ellipse(this->srid, this->tlx, this->tly, this->w, this->h));
	return ellipse;
}

void Math::Geometry::Ellipse::GetBounds(Math::RectAreaDbl *bounds) const
{
	*bounds = Math::RectAreaDbl(this->tlx, this->tly, this->w, this->h);
}

Double Math::Geometry::Ellipse::CalBoundarySqrDistance(Math::Coord2DDbl pt, Math::Coord2DDbl *nearPt) const
{
	Math::Coord2DDbl cent = Math::Coord2DDbl(this->tlx - this->w * 0.5, this->tly + this->h * 0.5);
	Double ang = Math_ArcTan2(pt.y - cent.y, pt.x - cent.x);
	Double sVal = Math_Sin(ang);
	Double cVal = Math_Cos(ang);
	*nearPt = Math::Coord2DDbl(cent.x + cVal * this->w * 0.5, cent.y - sVal * this->h * 0.5);
	return ang * 180 / Math::PI;
}

Bool Math::Geometry::Ellipse::JoinVector(Math::Geometry::Vector2D *vec)
{
	return false;
}

Bool Math::Geometry::Ellipse::HasZ() const
{
	return false;
}

void Math::Geometry::Ellipse::ConvCSys(Math::CoordinateSystem *srcCSys, Math::CoordinateSystem *destCSys)
{
	Double x2 = this->tlx + this->w;
	Double y2 = this->tly + this->h;
	Math::CoordinateSystem::ConvertXYZ(srcCSys, destCSys, this->tlx, this->tly, 0, &this->tlx, &this->tly, 0);
	Math::CoordinateSystem::ConvertXYZ(srcCSys, destCSys, x2, y2, 0, &x2, &y2, 0);
	this->w = x2 - this->tlx;
	this->h = y2 - this->tly;
	this->srid = destCSys->GetSRID();
}

Bool Math::Geometry::Ellipse::Equals(Math::Geometry::Vector2D *vec) const
{
	if (vec == 0 || vec->GetVectorType() != VectorType::Ellipse)
	{
		return false;
	}
	Math::Geometry::Ellipse *ellipse = (Math::Geometry::Ellipse*)vec;
	return this->srid == ellipse->srid &&
		this->w == ellipse->w &&
		this->h == ellipse->h &&
		this->tlx == ellipse->tlx &&
		this->tly == ellipse->tly;
}

UOSInt Math::Geometry::Ellipse::GetCoordinates(Data::ArrayListA<Math::Coord2DDbl> *coordList) const
{
	coordList->Add(Math::Coord2DDbl(this->tlx + this->w * 0.5, this->tly));
	coordList->Add(Math::Coord2DDbl(this->tlx + this->w, this->tly + this->h * 0.5));
	coordList->Add(Math::Coord2DDbl(this->tlx + this->w * 0.5, this->tly + this->h));
	coordList->Add(Math::Coord2DDbl(this->tlx, this->tly + this->h * 0.5));
	return 4;
}

Double Math::Geometry::Ellipse::GetLeft()
{
	return this->tlx;
}

Double Math::Geometry::Ellipse::GetTop()
{
	return this->tly;
}

Double Math::Geometry::Ellipse::GetWidth()
{
	return this->w;
}

Double Math::Geometry::Ellipse::GetHeight()
{
	return this->h;
}
