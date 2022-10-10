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

Math::Geometry::Vector2D *Math::Geometry::PieArea::Clone() const
{
	Math::Geometry::PieArea *pie;
	NEW_CLASS(pie, Math::Geometry::PieArea(this->srid, this->center, this->r, this->arcAngle1, this->arcAngle2));
	return pie;
}

void Math::Geometry::PieArea::GetBounds(Math::RectAreaDbl* bounds) const
{
	//////////////////////////////////////////////////////////
	bounds->tl = this->center - this->r;
	bounds->br = this->center + this->r;
}

Double Math::Geometry::PieArea::CalBoundarySqrDistance(Math::Coord2DDbl pt, Math::Coord2DDbl* nearPt) const
{
	//////////////////////////////////////////////////////////
	*nearPt = pt;
	return 0;
}

Bool Math::Geometry::PieArea::JoinVector(Math::Geometry::Vector2D *vec)
{
	return false;
}

Bool Math::Geometry::PieArea::HasZ() const
{
	return false;
}

void Math::Geometry::PieArea::ConvCSys(Math::CoordinateSystem *srcCSys, Math::CoordinateSystem *destCSys)
{
	Math::CoordinateSystem::ConvertXYZ(srcCSys, destCSys, this->center.x, this->center.y, 0, &this->center.x, &this->center.y, 0);
	this->srid = destCSys->GetSRID();
}

Bool Math::Geometry::PieArea::Equals(Vector2D *vec) const
{
	if (vec == 0 || vec->GetVectorType() != Math::Geometry::Vector2D::VectorType::PieArea)
		return false;
	Math::Geometry::PieArea *pa = (Math::Geometry::PieArea*)vec;
	return this->center == pa->center &&
		this->r == pa->r &&
		this->arcAngle1 == pa->arcAngle1 &&
		this->arcAngle2 == pa->arcAngle2;
}

UOSInt Math::Geometry::PieArea::GetCoordinates(Data::ArrayListA<Math::Coord2DDbl> *coordList) const
{
	coordList->Add(this->center);
	coordList->Add(Math::Coord2DDbl(this->center.x + Math_Cos(this->arcAngle1) * r, this->center.y + Math_Sin(this->arcAngle1) * r));
	coordList->Add(Math::Coord2DDbl(this->center.x + Math_Cos(this->arcAngle2) * r, this->center.y + Math_Sin(this->arcAngle2) * r));
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

