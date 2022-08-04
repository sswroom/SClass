#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/MultiPolygon.h"

Math::MultiPolygon::MultiPolygon(UInt32 srid) : Math::MultiGeometry<Math::Polygon>(srid)
{
}

Math::MultiPolygon::~MultiPolygon()
{
}

Math::Vector2D::VectorType Math::MultiPolygon::GetVectorType() const
{
	return Math::Vector2D::VectorType::Multipolygon;
}

Math::Vector2D *Math::MultiPolygon::Clone() const
{
	Math::MultiPolygon *newObj;
	NEW_CLASS(newObj, Math::MultiPolygon(this->srid));
	UOSInt i = 0;
	UOSInt j = this->geometries.GetCount();
	while (i < j)
	{
		newObj->AddGeometry((Math::Polygon*)this->geometries.GetItem(i)->Clone());
		i++;
	}
	return newObj;
}
