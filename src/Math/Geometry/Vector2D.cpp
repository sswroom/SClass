#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Geometry/Vector2D.h"

Math::Geometry::Vector2D::Vector2D(UInt32 srid)
{
	this->srid = srid;
}

Math::Geometry::Vector2D::~Vector2D()
{

}

Double Math::Geometry::Vector2D::CalSqrDistance(Math::Coord2DDbl pt, OutParam<Math::Coord2DDbl> nearPt) const
{
	if (this->InsideVector(pt))
	{
		nearPt.Set(pt);
		return 0;
	}
	return this->CalBoundarySqrDistance(pt, nearPt);
}

Bool Math::Geometry::Vector2D::InsideVector(Math::Coord2DDbl coord) const
{
	return false;
}

UInt32 Math::Geometry::Vector2D::GetSRID() const
{
	return this->srid;
}

void Math::Geometry::Vector2D::SetSRID(UInt32 srid)
{
	this->srid = srid;
}

Math::Coord2DDbl Math::Geometry::Vector2D::GetCentroid() const
{
	Math::Coord2DDbl sum = Math::Coord2DDbl(0, 0);
	Data::ArrayListA<Math::Coord2DDbl> coordList;
	UOSInt i = this->GetCoordinates(coordList);
	while (i-- > 0)
	{
		sum += coordList.GetItem(i);
	}
	return sum / UOSInt2Double(coordList.GetCount());
}

Math::Coord2DDbl Math::Geometry::Vector2D::GetDistanceCenter() const
{
	return GetCentroid();
}

Bool Math::Geometry::Vector2D::VectorTypeIsPoint(VectorType vecType)
{
	return vecType == VectorType::Point || vecType == VectorType::MultiPoint;
}

Text::CStringNN Math::Geometry::Vector2D::VectorTypeGetName(VectorType vecType)
{
	switch (vecType)
	{
	case VectorType::Point:
		return CSTR("Point");
	case VectorType::LineString:
		return CSTR("LineString");
	case VectorType::Polygon:
		return CSTR("Polygon");
	case VectorType::MultiPoint:
		return CSTR("MultiPoint");
	case VectorType::Polyline: //MultiLineString
		return CSTR("MultiLineString");
	case VectorType::MultiPolygon:
		return CSTR("MultiPolygon");
	case VectorType::GeometryCollection:
		return CSTR("GeometryCollection");
	case VectorType::CircularString:
		return CSTR("CircularString");
	case VectorType::CompoundCurve:
		return CSTR("CompoundCurve");
	case VectorType::CurvePolygon:
		return CSTR("CurvePolygon");
	case VectorType::MultiCurve:
		return CSTR("MultiCurve");
	case VectorType::MultiSurface:
		return CSTR("MultiSurface");
	case VectorType::Curve:
		return CSTR("Curve");
	case VectorType::Surface:
		return CSTR("Surface");
	case VectorType::PolyhedralSurface:
		return CSTR("PolyhedralSurface");
	case VectorType::Tin:
		return CSTR("Tin");
	case VectorType::Triangle:
		return CSTR("Triangle");
	case VectorType::Image:
		return CSTR("Image");
	case VectorType::String:
		return CSTR("String");
	case VectorType::Ellipse:
		return CSTR("Ellipse");
	case VectorType::PieArea:
		return CSTR("PieArea");
	case VectorType::Unknown:
	default:
		return CSTR("Unknown");
	}
}
