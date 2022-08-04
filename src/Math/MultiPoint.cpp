#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/MultiPoint.h"

Math::MultiPoint::MultiPoint(UInt32 srid) : Math::MultiGeometry<Math::Point>(srid)
{
}

Math::MultiPoint::~MultiPoint()
{
}

Math::Vector2D::VectorType Math::MultiPoint::GetVectorType() const
{
	return Math::Vector2D::VectorType::Multipoint;
}

Math::Vector2D *Math::MultiPoint::Clone() const
{
	Math::MultiPoint *newObj;
	NEW_CLASS(newObj, Math::MultiPoint(this->srid));
	UOSInt i = 0;
	UOSInt j = this->geometries.GetCount();
	while (i < j)
	{
		newObj->AddGeometry((Math::Point*)this->geometries.GetItem(i)->Clone());
		i++;
	}
	return newObj;
}
