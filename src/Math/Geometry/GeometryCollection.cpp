#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Geometry/GeometryCollection.h"

Math::Geometry::GeometryCollection::GeometryCollection(UInt32 srid, Bool hasZ, Bool hasM) : Math::Geometry::MultiGeometry<Math::Geometry::Vector2D>(srid, hasZ, hasM)
{
}

Math::Geometry::GeometryCollection::~GeometryCollection()
{
}

Math::Geometry::Vector2D::VectorType Math::Geometry::GeometryCollection::GetVectorType() const
{
	return Math::Geometry::Vector2D::VectorType::GeometryCollection;
}

NotNullPtr<Math::Geometry::Vector2D> Math::Geometry::GeometryCollection::Clone() const
{
	NotNullPtr<Math::Geometry::GeometryCollection> newObj;
	NEW_CLASSNN(newObj, Math::Geometry::GeometryCollection(this->srid, this->hasZ, this->hasM));
	UOSInt i = 0;
	UOSInt j = this->geometries.GetCount();
	while (i < j)
	{
		newObj->AddGeometry(this->geometries.GetItem(i)->Clone());
		i++;
	}
	return newObj;
}
