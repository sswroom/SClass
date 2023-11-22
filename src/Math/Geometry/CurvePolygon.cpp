#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Geometry/CompoundCurve.h"
#include "Math/Geometry/CurvePolygon.h"

Math::Geometry::CurvePolygon::CurvePolygon(UInt32 srid, Bool hasZ, Bool hasM) : Math::Geometry::MultiGeometry<Math::Geometry::Vector2D>(srid, hasZ, hasM)
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
	NEW_CLASSNN(newObj, Math::Geometry::CurvePolygon(this->srid, this->hasZ, this->hasM));
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
			ptOfst.Add((UInt32)ptList.GetCount());
			((Math::Geometry::CompoundCurve*)vec)->GetDrawPoints(ptList);
		}
		else if (vec->GetVectorType() == Math::Geometry::Vector2D::VectorType::LineString)
		{
			ptOfst.Add((UInt32)ptList.GetCount());
			const Math::Coord2DDbl *ptArr = ((Math::Geometry::LineString*)vec)->GetPointListRead(nPoint);
			ptList.AddRange(ptArr, nPoint);
		}
		else
		{
			printf("CurvePolygon: CurveToLine unexpected type: %s\r\n", Math::Geometry::Vector2D::VectorTypeGetName(vec->GetVectorType()).v);
		}
		i++;
	}
	NotNullPtr<Math::Geometry::Polygon> pg;
	NEW_CLASSNN(pg, Math::Geometry::Polygon(this->srid, ptOfst.GetCount(), ptList.GetCount(), false, false));
	MemCopyNO(pg->GetPtOfstList(i), ptOfst.Ptr(), ptOfst.GetCount() * sizeof(UInt32));
	MemCopyNO(pg->GetPointList(i), ptList.Ptr(), ptList.GetCount() * sizeof(Math::Coord2DDbl));
	return pg;
}

Bool Math::Geometry::CurvePolygon::InsideVector(Math::Coord2DDbl coord) const
{
	NotNullPtr<Vector2D> vec = this->CurveToLine();
	Bool inside = vec->InsideVector(coord);
	vec.Delete();
	return inside;
}
