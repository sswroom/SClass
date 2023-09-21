#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Geometry/MultiPoint.h"

Math::Geometry::MultiPoint::MultiPoint(UInt32 srid, Bool hasZ, Bool hasM) : Math::Geometry::MultiGeometry<Math::Geometry::Point>(srid, hasZ, hasM)
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
	NEW_CLASSNN(newObj, Math::Geometry::MultiPoint(this->srid, this->hasZ, this->hasM));
	UOSInt i = 0;
	UOSInt j = this->geometries.GetCount();
	while (i < j)
	{
		newObj->AddGeometry(NotNullPtr<Math::Geometry::Point>::ConvertFrom(this->geometries.GetItem(i)->Clone()));
		i++;
	}
	return newObj;
}
