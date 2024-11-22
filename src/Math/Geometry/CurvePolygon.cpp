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

void Math::Geometry::CurvePolygon::AddGeometry(NN<Vector2D> geometry)
{
	VectorType t = geometry->GetVectorType();
	if (t == VectorType::CircularString || t == VectorType::CompoundCurve || t == VectorType::LineString || t == VectorType::LinearRing)
	{
		this->geometries.Add(geometry);
	}
	else
	{
		printf("Error: Adding %s to CurvePolygon\r\n", Math::Geometry::Vector2D::VectorTypeGetName(t).v.Ptr());
		geometry.Delete();
	}
}

Math::Geometry::Vector2D::VectorType Math::Geometry::CurvePolygon::GetVectorType() const
{
	return Math::Geometry::Vector2D::VectorType::CurvePolygon;
}

NN<Math::Geometry::Vector2D> Math::Geometry::CurvePolygon::Clone() const
{
	NN<Math::Geometry::CurvePolygon> newObj;
	NEW_CLASSNN(newObj, Math::Geometry::CurvePolygon(this->srid));
	Data::ArrayIterator<NN<Math::Geometry::Vector2D>> it = this->geometries.Iterator();
	while (it.HasNext())
	{
		newObj->AddGeometry(it.Next()->Clone());
	}
	return newObj;
}

NN<Math::Geometry::Vector2D> Math::Geometry::CurvePolygon::CurveToLine() const
{
	NN<Math::Geometry::Polygon> pg;
	NEW_CLASSNN(pg, Math::Geometry::Polygon(this->srid));
	NN<Math::Geometry::LinearRing> lr;
	Data::ArrayList<UInt32> ptOfst;
	Data::ArrayListA<Math::Coord2DDbl> ptList;
	UOSInt nPoint;
	NN<Math::Geometry::Vector2D> vec;
	Data::ArrayIterator<NN<Math::Geometry::Vector2D>> it = this->Iterator();
	while (it.HasNext())
	{
		vec = it.Next();
		if (vec->GetVectorType() == Math::Geometry::Vector2D::VectorType::CompoundCurve)
		{
			ptList.Clear();
			NN<Math::Geometry::CompoundCurve>::ConvertFrom(vec)->GetDrawPoints(ptList);
			NEW_CLASSNN(lr, LinearRing(this->srid, ptList.Arr(), ptList.GetCount(), 0, 0));
			pg->AddGeometry(lr);
		}
		else if (vec->GetVectorType() == Math::Geometry::Vector2D::VectorType::LineString)
		{
			UnsafeArray<const Math::Coord2DDbl> ptArr = NN<Math::Geometry::LineString>::ConvertFrom(vec)->GetPointListRead(nPoint);
			NEW_CLASSNN(lr, LinearRing(this->srid, ptArr, nPoint, 0, 0));
			pg->AddGeometry(lr);
		}
		else if (vec->GetVectorType() == Math::Geometry::Vector2D::VectorType::LinearRing)
		{
			pg->AddGeometry(NN<LinearRing>::ConvertFrom(vec->Clone()));
		}
		else
		{
			printf("CurvePolygon: CurveToLine unexpected type: %s\r\n", Math::Geometry::Vector2D::VectorTypeGetName(vec->GetVectorType()).v.Ptr());
		}
	}
	return pg;
}

Bool Math::Geometry::CurvePolygon::InsideOrTouch(Math::Coord2DDbl coord) const
{
	NN<Vector2D> vec = this->CurveToLine();
	Bool inside = vec->InsideOrTouch(coord);
	vec.Delete();
	return inside;
}
