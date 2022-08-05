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
	return Math::Geometry::Vector2D::VectorType::Multipoint;
}

Math::Geometry::Vector2D *Math::Geometry::MultiPoint::Clone() const
{
	Math::Geometry::MultiPoint *newObj;
	NEW_CLASS(newObj, Math::Geometry::MultiPoint(this->srid));
	UOSInt i = 0;
	UOSInt j = this->geometries.GetCount();
	while (i < j)
	{
		newObj->AddGeometry((Math::Geometry::Point*)this->geometries.GetItem(i)->Clone());
		i++;
	}
	return newObj;
}
