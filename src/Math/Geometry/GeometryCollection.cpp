#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Geometry/GeometryCollection.h"

Math::Geometry::GeometryCollection::GeometryCollection(UInt32 srid) : Math::Geometry::MultiGeometry<Math::Geometry::Vector2D>(srid)
{
}

Math::Geometry::GeometryCollection::~GeometryCollection()
{
}

Math::Geometry::Vector2D::VectorType Math::Geometry::GeometryCollection::GetVectorType() const
{
	return Math::Geometry::Vector2D::VectorType::GeometryCollection;
}

NN<Math::Geometry::Vector2D> Math::Geometry::GeometryCollection::Clone() const
{
	NN<Math::Geometry::GeometryCollection> newObj;
	NEW_CLASSNN(newObj, Math::Geometry::GeometryCollection(this->srid));
	Data::ArrayIterator<NN<Math::Geometry::Vector2D>> it = this->geometries.Iterator();
	while (it.HasNext())
	{
		newObj->AddGeometry(it.Next()->Clone());
	}
	return newObj;
}
