#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Geometry/CompoundCurve.h"

Math::Geometry::CompoundCurve::CompoundCurve(UInt32 srid, Bool hasZ, Bool hasM) : Math::Geometry::MultiGeometry<Math::Geometry::LineString>(srid, hasZ, hasM)
{
}

Math::Geometry::CompoundCurve::~CompoundCurve()
{
}

void Math::Geometry::CompoundCurve::AddGeometry(LineString *geometry)
{
	VectorType t = geometry->GetVectorType();
	if (t == VectorType::CircularString || t == VectorType::LineString)
	{
		this->geometries.Add(geometry);
	}
	else
	{
		DEL_CLASS(geometry);
	}
}

Math::Geometry::Vector2D::VectorType Math::Geometry::CompoundCurve::GetVectorType() const
{
	return Math::Geometry::Vector2D::VectorType::CompoundCurve;
}

Math::Geometry::Vector2D *Math::Geometry::CompoundCurve::Clone() const
{
	Math::Geometry::CompoundCurve *newObj;
	NEW_CLASS(newObj, Math::Geometry::CompoundCurve(this->srid, this->hasZ, this->hasM));
	UOSInt i = 0;
	UOSInt j = this->geometries.GetCount();
	while (i < j)
	{
		newObj->AddGeometry((LineString*)this->geometries.GetItem(i)->Clone());
		i++;
	}
	return newObj;
}
