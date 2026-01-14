#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Geometry/MultiPolygon.h"
#include "Math/Geometry/MultiSurface.h"

Math::Geometry::MultiSurface::MultiSurface(UInt32 srid) : Math::Geometry::MultiGeometry<Math::Geometry::Vector2D>(srid)
{
}

Math::Geometry::MultiSurface::~MultiSurface()
{
}

void Math::Geometry::MultiSurface::AddGeometry(NN<Vector2D> geometry)
{
	VectorType t = geometry->GetVectorType();
	if (t == VectorType::CurvePolygon)// || t == VectorType::LineString)
	{
		this->geometries.Add(geometry);
	}
	else
	{
		printf("Error: Adding %s to MultiSurface\r\n", Math::Geometry::Vector2D::VectorTypeGetName(geometry->GetVectorType()).v.Ptr());
		geometry.Delete();
	}
}

Math::Geometry::Vector2D::VectorType Math::Geometry::MultiSurface::GetVectorType() const
{
	return Math::Geometry::Vector2D::VectorType::MultiSurface;
}

NN<Math::Geometry::Vector2D> Math::Geometry::MultiSurface::Clone() const
{
	NN<Math::Geometry::MultiSurface> newObj;
	NEW_CLASSNN(newObj, Math::Geometry::MultiSurface(this->srid));
	Data::ArrayIterator<NN<Vector2D>> it = this->Iterator();
	while (it.HasNext())
	{
		newObj->AddGeometry(it.Next()->Clone());
	}
	return newObj;
}

Optional<Math::Geometry::Vector2D> Math::Geometry::MultiSurface::ToSimpleShape() const
{
	NN<Math::Geometry::MultiPolygon> newObj;
	NEW_CLASSNN(newObj, Math::Geometry::MultiPolygon(this->srid));
	Data::ArrayIterator<NN<Vector2D>> it = this->Iterator();
	while (it.HasNext())
	{
		NN<Math::Geometry::Vector2D> vec;
		if (it.Next()->ToSimpleShape().SetTo(vec))
		{
			if (vec->GetVectorType() == VectorType::Polygon)
			{
				newObj->AddGeometry(NN<Math::Geometry::Polygon>::ConvertFrom(vec));
			}
			else
			{
				printf("Error: MultiSurface SimpleShape is not polygon\r\n");
				newObj.Delete();
				return nullptr;
			}
		}
		else
		{
			printf("Error: Error in MultiSurface converting to simple shape\r\n");
			newObj.Delete();
			return nullptr;
		}
	}
	return newObj;
}
