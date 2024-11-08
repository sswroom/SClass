#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Geometry/Vector2D.h"

Double Math::Geometry::Vector2D::CalSqrDistance(Math::Coord2DDbl pt, OutParam<Math::Coord2DDbl> nearPt) const
{
	if (this->InsideOrTouch(pt))
	{
		nearPt.Set(pt);
		return 0;
	}
	return this->CalBoundarySqrDistance(pt, nearPt);
}

Bool Math::Geometry::Vector2D::Contains(NN<Math::Geometry::Vector2D> vec) const
{
	Data::ArrayListA<Math::Coord2DDbl> ptList;
	this->GetCoordinates(ptList);
	UOSInt i = ptList.GetCount();
	if (i == 0)
		return false;
	while (i-- > 0)
	{
		if (!this->InsideOrTouch(ptList.GetItem(i)))
			return false;
	}
	return true;
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
	case VectorType::LinearRing:
		return CSTR("LinearRing");
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
