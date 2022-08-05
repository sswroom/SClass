#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Geometry/MultiPolygon.h"

Math::Geometry::MultiPolygon::MultiPolygon(UInt32 srid, Bool hasZ, Bool hasM) : Math::Geometry::MultiGeometry<Math::Geometry::Polygon>(srid, hasZ, hasM)
{
}

Math::Geometry::MultiPolygon::~MultiPolygon()
{
}

Math::Geometry::Vector2D::VectorType Math::Geometry::MultiPolygon::GetVectorType() const
{
	return Math::Geometry::Vector2D::VectorType::Multipolygon;
}

Math::Geometry::Vector2D *Math::Geometry::MultiPolygon::Clone() const
{
	Math::Geometry::MultiPolygon *newObj;
	NEW_CLASS(newObj, Math::Geometry::MultiPolygon(this->srid, this->hasZ, this->hasM));
	UOSInt i = 0;
	UOSInt j = this->geometries.GetCount();
	while (i < j)
	{
		newObj->AddGeometry((Math::Geometry::Polygon*)this->geometries.GetItem(i)->Clone());
		i++;
	}
	return newObj;
}
