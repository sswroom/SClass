#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/CoordinateSystem.h"
#include "Math/Math.h"
#include "Math/Geometry/Ellipse.h"

Math::Geometry::Ellipse::Ellipse(UInt32 srid, Math::Coord2DDbl tl, Math::Size2DDbl size) : Math::Geometry::Vector2D(srid)
{
	this->tl = tl;
	this->size = size;
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
	return this->tl + (this->size * 0.5);
}

NotNullPtr<Math::Geometry::Vector2D> Math::Geometry::Ellipse::Clone() const
{
	NotNullPtr<Math::Geometry::Ellipse> ellipse;
	NEW_CLASSNN(ellipse, Math::Geometry::Ellipse(this->srid, this->tl, this->size));
	return ellipse;
}

Math::RectAreaDbl Math::Geometry::Ellipse::GetBounds() const
{
	return Math::RectAreaDbl(this->tl, this->tl + this->size);
}

Double Math::Geometry::Ellipse::CalBoundarySqrDistance(Math::Coord2DDbl pt, OutParam<Math::Coord2DDbl> nearPt) const
{
	Math::Coord2DDbl cent = this->tl + (this->size * 0.5);
	Double ang = Math_ArcTan2((pt.y - cent.y) * this->size.x / this->size.y, pt.x - cent.x);
	Double sVal = Math_Sin(ang);
	Double cVal = Math_Cos(ang);
	nearPt.Set(Math::Coord2DDbl(cent.x + cVal * this->size.x * 0.5, cent.y + sVal * this->size.y * 0.5));
	return ang * 180 / Math::PI;
}

Bool Math::Geometry::Ellipse::JoinVector(NotNullPtr<const Math::Geometry::Vector2D> vec)
{
	return false;
}

Bool Math::Geometry::Ellipse::HasZ() const
{
	return false;
}

void Math::Geometry::Ellipse::ConvCSys(NotNullPtr<const Math::CoordinateSystem> srcCSys, NotNullPtr<const Math::CoordinateSystem> destCSys)
{
	Math::Coord2DDbl br = this->tl + this->size;
	this->tl = Math::CoordinateSystem::ConvertXYZ(srcCSys, destCSys, Math::Vector3(this->tl, 0)).GetXY();
	br = Math::CoordinateSystem::ConvertXYZ(srcCSys, destCSys, Math::Vector3(br, 0)).GetXY();
	this->size = br - this->tl;
	this->srid = destCSys->GetSRID();
}

Bool Math::Geometry::Ellipse::Equals(NotNullPtr<const Math::Geometry::Vector2D> vec, Bool sameTypeOnly, Bool nearlyVal) const
{
	if (vec->GetVectorType() != VectorType::Ellipse)
	{
		return false;
	}
	const Math::Geometry::Ellipse *ellipse = (const Math::Geometry::Ellipse*)vec.Ptr();
	if (nearlyVal)
		return this->srid == ellipse->srid &&
			Math::NearlyEqualsDbl(this->size.x, ellipse->size.x) &&
			Math::NearlyEqualsDbl(this->size.y, ellipse->size.y) &&
			Math::NearlyEqualsDbl(this->tl.x, ellipse->tl.x) &&
			Math::NearlyEqualsDbl(this->tl.y, ellipse->tl.y);
	else
		return this->srid == ellipse->srid &&
			this->size == ellipse->size &&
			this->tl == ellipse->tl;
}

UOSInt Math::Geometry::Ellipse::GetCoordinates(NotNullPtr<Data::ArrayListA<Math::Coord2DDbl>> coordList) const
{
	coordList->Add(Math::Coord2DDbl(this->tl.x + this->size.x * 0.5, this->tl.y));
	coordList->Add(Math::Coord2DDbl(this->tl.x + this->size.x, this->tl.y + this->size.y * 0.5));
	coordList->Add(Math::Coord2DDbl(this->tl.x + this->size.x * 0.5, this->tl.y + this->size.y));
	coordList->Add(Math::Coord2DDbl(this->tl.x, this->tl.y + this->size.y * 0.5));
	return 4;
}

Bool Math::Geometry::Ellipse::InsideVector(Math::Coord2DDbl coord) const
{
	Math::Coord2DDbl cent = this->tl + (this->size * 0.5);
	Double yDiff = (coord.y - cent.y) * this->size.x / this->size.y;
	Double xDiff = (coord.x - cent.x);
	return Math_Sqrt(xDiff * xDiff + yDiff * yDiff) <= this->size.x * 0.5;
}

void Math::Geometry::Ellipse::SwapXY()
{
	this->tl = this->tl.SwapXY();
	this->size = this->size.SwapXY();
}

void Math::Geometry::Ellipse::MultiplyCoordinatesXY(Double v)
{
	this->tl = this->tl * v;
	this->size = this->size * v;
}

Math::Coord2DDbl Math::Geometry::Ellipse::GetTL()
{
	return this->tl;
}

Math::Coord2DDbl Math::Geometry::Ellipse::GetBR()
{
	return this->tl + this->size;
}

Double Math::Geometry::Ellipse::GetLeft()
{
	return this->tl.x;
}

Double Math::Geometry::Ellipse::GetTop()
{
	return this->tl.y;
}

Double Math::Geometry::Ellipse::GetWidth()
{
	return this->size.x;
}

Double Math::Geometry::Ellipse::GetHeight()
{
	return this->size.y;
}
