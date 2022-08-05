#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Geometry/MultiSurface.h"

Math::Geometry::MultiSurface::MultiSurface(UInt32 srid, Bool hasZ, Bool hasM) : Math::Geometry::MultiGeometry<Math::Geometry::Vector2D>(srid, hasZ, hasM)
{
}

Math::Geometry::MultiSurface::~MultiSurface()
{
}

void Math::Geometry::MultiSurface::AddGeometry(Vector2D *geometry)
{
	VectorType t = geometry->GetVectorType();
	if (t == VectorType::CurvePolygon)// || t == VectorType::LineString)
	{
		this->geometries.Add(geometry);
	}
	else
	{
		DEL_CLASS(geometry);
	}
}

Math::Geometry::Vector2D::VectorType Math::Geometry::MultiSurface::GetVectorType() const
{
	return Math::Geometry::Vector2D::VectorType::MultiSurface;
}

Math::Geometry::Vector2D *Math::Geometry::MultiSurface::Clone() const
{
	Math::Geometry::MultiSurface *newObj;
	NEW_CLASS(newObj, Math::Geometry::MultiSurface(this->srid, this->hasZ, this->hasM));
	UOSInt i = 0;
	UOSInt j = this->geometries.GetCount();
	while (i < j)
	{
		newObj->AddGeometry(this->geometries.GetItem(i)->Clone());
		i++;
	}
	return newObj;
}
