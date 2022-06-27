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

Math::Vector2D::VectorType Math::Ellipse::GetVectorType() const
{
	return Math::Vector2D::VectorType::Ellipse;
}

Math::Coord2DDbl Math::Ellipse::GetCenter() const
{
	return Math::Coord2DDbl(this->tlx + (this->w * 0.5), this->tly + (this->h * 0.5));
}

Math::Vector2D *Math::Ellipse::Clone() const
{
	Math::Ellipse *ellipse;
	NEW_CLASS(ellipse, Math::Ellipse(this->srid, this->tlx, this->tly, this->w, this->h));
	return ellipse;
}

void Math::Ellipse::GetBounds(Math::RectAreaDbl *bounds) const
{
	*bounds = Math::RectAreaDbl(this->tlx, this->tly, this->w, this->h);
}

Double Math::Ellipse::CalSqrDistance(Math::Coord2DDbl pt, Math::Coord2DDbl *nearPt) const
{
	//////////////////////////////////////////////////////////
	*nearPt = pt;
	return 0;
}

Bool Math::Ellipse::JoinVector(Math::Vector2D *vec)
{
	return false;
}

Bool Math::Ellipse::Support3D() const
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

Bool Math::Ellipse::Equals(Math::Vector2D *vec) const
{
	if (vec == 0 || vec->GetVectorType() != VectorType::Ellipse)
	{
		return false;
	}
	Math::Ellipse *ellipse = (Math::Ellipse*)vec;
	return this->srid == ellipse->srid &&
		this->w == ellipse->w &&
		this->h == ellipse->h &&
		this->tlx == ellipse->tlx &&
		this->tly == ellipse->tly;
}
