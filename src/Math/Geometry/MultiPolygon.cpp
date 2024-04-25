#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Geometry/MultiPolygon.h"

Math::Geometry::MultiPolygon::MultiPolygon(UInt32 srid) : Math::Geometry::MultiGeometry<Math::Geometry::Polygon>(srid)
{
}

Math::Geometry::MultiPolygon::~MultiPolygon()
{
}

Math::Geometry::Vector2D::VectorType Math::Geometry::MultiPolygon::GetVectorType() const
{
	return Math::Geometry::Vector2D::VectorType::MultiPolygon;
}

NN<Math::Geometry::Vector2D> Math::Geometry::MultiPolygon::Clone() const
{
	NN<Math::Geometry::MultiPolygon> newObj;
	NEW_CLASSNN(newObj, Math::Geometry::MultiPolygon(this->srid));
	Data::ArrayIterator<NN<Polygon>> it = this->Iterator();
	while (it.HasNext())
	{
		newObj->AddGeometry(NN<Math::Geometry::Polygon>::ConvertFrom(it.Next()->Clone()));
	}
	return newObj;
}
