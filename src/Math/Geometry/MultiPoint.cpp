#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Geometry/MultiPoint.h"

Math::Geometry::MultiPoint::MultiPoint(UInt32 srid) : Math::Geometry::MultiGeometry<Math::Geometry::Point>(srid)
{
}

Math::Geometry::MultiPoint::~MultiPoint()
{
}

Math::Geometry::Vector2D::VectorType Math::Geometry::MultiPoint::GetVectorType() const
{
	return Math::Geometry::Vector2D::VectorType::MultiPoint;
}

NotNullPtr<Math::Geometry::Vector2D> Math::Geometry::MultiPoint::Clone() const
{
	NotNullPtr<Math::Geometry::MultiPoint> newObj;
	NEW_CLASSNN(newObj, Math::Geometry::MultiPoint(this->srid));
	Data::ArrayIterator<NotNullPtr<Point>> it = this->geometries.Iterator();
	while (it.HasNext())
	{
		newObj->AddGeometry(NotNullPtr<Math::Geometry::Point>::ConvertFrom(it.Next()->Clone()));
	}
	return newObj;
}
