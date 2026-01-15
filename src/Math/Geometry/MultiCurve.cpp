#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Geometry/Polyline.h"
#include "Math/Geometry/MultiCurve.h"

Math::Geometry::MultiCurve::MultiCurve(UInt32 srid) : Math::Geometry::MultiGeometry<Math::Geometry::Vector2D>(srid)
{
}

Math::Geometry::MultiCurve::~MultiCurve()
{
}

void Math::Geometry::MultiCurve::AddGeometry(NN<Vector2D> geometry)
{
	VectorType t = geometry->GetVectorType();
	if (t == VectorType::CompoundCurve || t == VectorType::LineString)
	{
		this->geometries.Add(geometry);
	}
	else
	{
		printf("Error: Adding %s to MultiCurve\r\n", Math::Geometry::Vector2D::VectorTypeGetName(geometry->GetVectorType()).v.Ptr());
		geometry.Delete();
	}
}

Math::Geometry::Vector2D::VectorType Math::Geometry::MultiCurve::GetVectorType() const
{
	return Math::Geometry::Vector2D::VectorType::MultiCurve;
}

NN<Math::Geometry::Vector2D> Math::Geometry::MultiCurve::Clone() const
{
	NN<Math::Geometry::MultiCurve> newObj;
	NEW_CLASSNN(newObj, Math::Geometry::MultiCurve(this->srid));
	Data::ArrayIterator<NN<Vector2D>> it = this->Iterator();
	while (it.HasNext())
	{
		newObj->AddGeometry(it.Next()->Clone());
	}
	return newObj;
}

Optional<Math::Geometry::Vector2D> Math::Geometry::MultiCurve::ToSimpleShape() const
{
	NN<Math::Geometry::Polyline> newObj;
	NEW_CLASSNN(newObj, Math::Geometry::Polyline(this->srid));
	Data::ArrayIterator<NN<Vector2D>> it = this->Iterator();
	while (it.HasNext())
	{
		NN<Math::Geometry::Vector2D> vec;
		if (it.Next()->ToSimpleShape().SetTo(vec))
		{
			if (vec->GetVectorType() == VectorType::LineString)
			{
				newObj->AddGeometry(NN<Math::Geometry::LineString>::ConvertFrom(vec));
			}
			else
			{
				printf("Error: MultiCurve SimpleShape is not polygon\r\n");
				newObj.Delete();
				return nullptr;
			}
		}
		else
		{
			printf("Error: Error in MultiCurve converting to simple shape\r\n");
			newObj.Delete();
			return nullptr;
		}
	}
	return newObj;
}
