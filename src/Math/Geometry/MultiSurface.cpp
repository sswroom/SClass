#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Geometry/MultiSurface.h"

Math::Geometry::MultiSurface::MultiSurface(UInt32 srid) : Math::Geometry::MultiGeometry<Math::Geometry::Vector2D>(srid)
{
}

Math::Geometry::MultiSurface::~MultiSurface()
{
}

void Math::Geometry::MultiSurface::AddGeometry(NotNullPtr<Vector2D> geometry)
{
	VectorType t = geometry->GetVectorType();
	if (t == VectorType::CurvePolygon)// || t == VectorType::LineString)
	{
		this->geometries.Add(geometry);
	}
	else
	{
		geometry.Delete();
	}
}

Math::Geometry::Vector2D::VectorType Math::Geometry::MultiSurface::GetVectorType() const
{
	return Math::Geometry::Vector2D::VectorType::MultiSurface;
}

NotNullPtr<Math::Geometry::Vector2D> Math::Geometry::MultiSurface::Clone() const
{
	NotNullPtr<Math::Geometry::MultiSurface> newObj;
	NEW_CLASSNN(newObj, Math::Geometry::MultiSurface(this->srid));
	Data::ArrayIterator<NotNullPtr<Vector2D>> it = this->Iterator();
	while (it.HasNext())
	{
		newObj->AddGeometry(it.Next()->Clone());
	}
	return newObj;
}
