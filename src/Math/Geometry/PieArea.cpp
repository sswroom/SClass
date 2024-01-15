#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/CoordinateSystem.h"
#include "Math/Math.h"
#include "Math/Geometry/PieArea.h"

Math::Geometry::PieArea::PieArea(UInt32 srid, Math::Coord2DDbl center, Double r, Double arcAngle1, Double arcAngle2) : Math::Geometry::Vector2D(srid)
{
	this->center = center;
	this->r = r;
	this->arcAngle1 = arcAngle1;
	this->arcAngle2 = arcAngle2;
}

Math::Geometry::PieArea::~PieArea()
{
}

Math::Geometry::Vector2D::VectorType Math::Geometry::PieArea::GetVectorType() const
{
	return Math::Geometry::Vector2D::VectorType::PieArea;
}

Math::Coord2DDbl Math::Geometry::PieArea::GetCenter() const
{
	Double a = (this->arcAngle1 + this->arcAngle2) * 0.5;
	return Math::Coord2DDbl(this->center.x + r * Math_Sin(a) * 0.5, this->center.y - r * Math_Cos(a) * 0.5);
}

NotNullPtr<Math::Geometry::Vector2D> Math::Geometry::PieArea::Clone() const
{
	NotNullPtr<Math::Geometry::PieArea> pie;
	NEW_CLASSNN(pie, Math::Geometry::PieArea(this->srid, this->center, this->r, this->arcAngle1, this->arcAngle2));
	return pie;
}

Math::RectAreaDbl Math::Geometry::PieArea::GetBounds() const
{
	//////////////////////////////////////////////////////////
	return Math::RectAreaDbl(this->center - this->r, this->center + this->r);
}

Double Math::Geometry::PieArea::CalBoundarySqrDistance(Math::Coord2DDbl pt, OutParam<Math::Coord2DDbl> nearPt) const
{
	//////////////////////////////////////////////////////////
	nearPt.Set(pt);
	return 0;
}

Double Math::Geometry::PieArea::CalArea() const
{
	return 0;
}

Bool Math::Geometry::PieArea::JoinVector(NotNullPtr<const Math::Geometry::Vector2D> vec)
{
	return false;
}

Bool Math::Geometry::PieArea::HasZ() const
{
	return false;
}

Bool Math::Geometry::PieArea::GetZBounds(OutParam<Double> min, OutParam<Double> max) const
{
	return false;
}

Bool Math::Geometry::PieArea::GetMBounds(OutParam<Double> min, OutParam<Double> max) const
{
	return false;
}

void Math::Geometry::PieArea::Convert(NotNullPtr<Math::CoordinateConverter> converter)
{
	this->center = converter->Convert2D(this->center);
	this->srid = converter->GetOutputSRID();
}

Bool Math::Geometry::PieArea::Equals(NotNullPtr<const Vector2D> vec, Bool sameTypeOnly, Bool nearlyVal) const
{
	if (vec->GetVectorType() != Math::Geometry::Vector2D::VectorType::PieArea)
		return false;
	const Math::Geometry::PieArea* pa = (const Math::Geometry::PieArea*)vec.Ptr();
	if (nearlyVal)
	{
		return this->center.EqualsNearly(pa->center) &&
			Math::NearlyEqualsDbl(this->r, pa->r) &&
			Math::NearlyEqualsDbl(this->arcAngle1, pa->arcAngle1) &&
			Math::NearlyEqualsDbl(this->arcAngle2, pa->arcAngle2);
	}
	else
	{
		return this->center == pa->center &&
			this->r == pa->r &&
			this->arcAngle1 == pa->arcAngle1 &&
			this->arcAngle2 == pa->arcAngle2;
	}
}

UOSInt Math::Geometry::PieArea::GetCoordinates(NotNullPtr<Data::ArrayListA<Math::Coord2DDbl>> coordList) const
{
	coordList->Add(this->center);
	coordList->Add(Math::Coord2DDbl(this->center.x + Math_Cos(this->arcAngle1) * r, this->center.y + Math_Sin(this->arcAngle1) * r));
	coordList->Add(Math::Coord2DDbl(this->center.x + Math_Cos(this->arcAngle2) * r, this->center.y + Math_Sin(this->arcAngle2) * r));
	return 3;
}

Bool Math::Geometry::PieArea::InsideOrTouch(Math::Coord2DDbl coord) const
{
	return false;
}

void Math::Geometry::PieArea::SwapXY()
{
	this->center = this->center.SwapXY();
}

void Math::Geometry::PieArea::MultiplyCoordinatesXY(Double v)
{
	this->center = this->center * v;
	this->r = this->r * v;
}

UOSInt Math::Geometry::PieArea::GetPointCount() const
{
	return 3;
}

Double Math::Geometry::PieArea::GetCX() const
{
	return this->center.x;
}

Double Math::Geometry::PieArea::GetCY() const
{
	return this->center.y;
}

Double Math::Geometry::PieArea::GetR() const
{
	return this->r;
}

Double Math::Geometry::PieArea::GetArcAngle1() const
{
	return this->arcAngle1;
}

Double Math::Geometry::PieArea::GetArcAngle2() const
{
	return this->arcAngle2;
}

