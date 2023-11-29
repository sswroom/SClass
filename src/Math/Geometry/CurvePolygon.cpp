#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayList.h"
#include "Math/Geometry/CompoundCurve.h"
#include "Math/Geometry/CurvePolygon.h"
#include "Math/Geometry/Polygon.h"

Math::Geometry::CurvePolygon::CurvePolygon(UInt32 srid) : Math::Geometry::MultiGeometry<Math::Geometry::Vector2D>(srid)
{
}

Math::Geometry::CurvePolygon::~CurvePolygon()
{
}

void Math::Geometry::CurvePolygon::AddGeometry(NotNullPtr<Vector2D> geometry)
{
	VectorType t = geometry->GetVectorType();
	if (t == VectorType::CircularString || t == VectorType::CompoundCurve || t == VectorType::LineString)
	{
		this->geometries.Add(geometry);
	}
	else
	{
		geometry.Delete();
	}
}

Math::Geometry::Vector2D::VectorType Math::Geometry::CurvePolygon::GetVectorType() const
{
	return Math::Geometry::Vector2D::VectorType::CurvePolygon;
}

NotNullPtr<Math::Geometry::Vector2D> Math::Geometry::CurvePolygon::Clone() const
{
	NotNullPtr<Math::Geometry::CurvePolygon> newObj;
	NEW_CLASSNN(newObj, Math::Geometry::CurvePolygon(this->srid));
	UOSInt i = 0;
	UOSInt j = this->geometries.GetCount();
	while (i < j)
	{
		newObj->AddGeometry(this->geometries.GetItem(i)->Clone());
		i++;
	}
	return newObj;
}

NotNullPtr<Math::Geometry::Vector2D> Math::Geometry::CurvePolygon::CurveToLine() const
{
	NotNullPtr<Math::Geometry::Polygon> pg;
	NEW_CLASSNN(pg, Math::Geometry::Polygon(this->srid));
	NotNullPtr<Math::Geometry::LinearRing> lr;
	Data::ArrayList<UInt32> ptOfst;
	Data::ArrayListA<Math::Coord2DDbl> ptList;
	UOSInt nPoint;
	Math::Geometry::Vector2D *vec;
	UOSInt i = 0;
	UOSInt j = this->GetCount();
	while (i < j)
	{
		vec = this->GetItem(i);
		if (vec->GetVectorType() == Math::Geometry::Vector2D::VectorType::CompoundCurve)
		{
			ptList.Clear();
			((Math::Geometry::CompoundCurve*)vec)->GetDrawPoints(ptList);
			NEW_CLASSNN(lr, LinearRing(this->srid, ptList.Ptr(), ptList.GetCount(), 0, 0));
			pg->AddGeometry(lr);
		}
		else if (vec->GetVectorType() == Math::Geometry::Vector2D::VectorType::LineString)
		{
			const Math::Coord2DDbl *ptArr = ((Math::Geometry::LineString*)vec)->GetPointListRead(nPoint);
			NEW_CLASSNN(lr, LinearRing(this->srid, ptArr, nPoint, 0, 0));
			pg->AddGeometry(lr);
		}
		else
		{
			printf("CurvePolygon: CurveToLine unexpected type: %s\r\n", Math::Geometry::Vector2D::VectorTypeGetName(vec->GetVectorType()).v);
		}
		i++;
	}
	return pg;
}

Bool Math::Geometry::CurvePolygon::InsideOrTouch(Math::Coord2DDbl coord) const
{
	NotNullPtr<Vector2D> vec = this->CurveToLine();
	Bool inside = vec->InsideOrTouch(coord);
	vec.Delete();
	return inside;
}
