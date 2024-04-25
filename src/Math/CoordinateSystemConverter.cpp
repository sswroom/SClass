#include "Stdafx.h"
#include "Math/CoordinateSystemConverter.h"

Math::CoordinateSystemConverter::CoordinateSystemConverter(NN<const Math::CoordinateSystem> srcCsys, NN<const Math::CoordinateSystem> destCsys)
{
	this->srcCsys = srcCsys;
	this->destCsys = destCsys;
}

Math::CoordinateSystemConverter::~CoordinateSystemConverter()
{
}

UInt32 Math::CoordinateSystemConverter::GetSourceSRID() const
{
	return this->srcCsys->GetSRID();
}

UInt32 Math::CoordinateSystemConverter::GetOutputSRID() const
{
	return this->destCsys->GetSRID();
}

Math::Coord2DDbl Math::CoordinateSystemConverter::Convert2D(Math::Coord2DDbl coord) const
{
	return Math::CoordinateSystem::Convert(this->srcCsys, this->destCsys, coord);
}

Math::Vector3 Math::CoordinateSystemConverter::Convert3D(Math::Vector3 vec3) const
{
	return Math::CoordinateSystem::Convert3D(this->srcCsys, this->destCsys, vec3);
}

void Math::CoordinateSystemConverter::Convert2DArr(const Math::Coord2DDbl *srcArr, Math::Coord2DDbl *destArr, UOSInt nPoints) const
{
	Math::CoordinateSystem::ConvertArray(this->srcCsys, this->destCsys, srcArr, destArr, nPoints);
}
