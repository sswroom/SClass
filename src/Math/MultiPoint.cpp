#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/MultiPoint.h"

Math::MultiPoint::MultiPoint(UInt32 srid, UOSInt nPoints) : Math::PointCollection(srid, nPoints, 0)
{
}

Math::MultiPoint::~MultiPoint()
{
}

Math::Vector2D::VectorType Math::MultiPoint::GetVectorType()
{
	return Math::Vector2D::VectorType::Multipoint;
}
