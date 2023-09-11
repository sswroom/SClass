#include "Stdafx.h"
#include "MyMemory.h"
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
