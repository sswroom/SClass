#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Geometry/Vector2D.h"

Math::Geometry::Vector2D::Vector2D(UInt32 srid)
{
	this->srid = srid;
}

Math::Geometry::Vector2D::~Vector2D()
{

}

UInt32 Math::Geometry::Vector2D::GetSRID() const
{
	return this->srid;
}

void Math::Geometry::Vector2D::SetSRID(UInt32 srid)
{
	this->srid = srid;
}

Bool Math::Geometry::Vector2D::VectorTypeIsPoint(VectorType vecType)
{
	return vecType == VectorType::Point || vecType == VectorType::MultiPoint;
}
