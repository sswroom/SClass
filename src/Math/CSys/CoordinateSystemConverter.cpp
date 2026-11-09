#include "Stdafx.h"
#include "Math/CSys/CoordinateSystemConverter.h"

Math::CSys::CoordinateSystemConverter::CoordinateSystemConverter(NN<const CoordinateSystem> srcCsys, NN<const CoordinateSystem> destCsys)
{
	this->srcCsys = srcCsys;
	this->destCsys = destCsys;
}

Math::CSys::CoordinateSystemConverter::~CoordinateSystemConverter()
{
}

UInt32 Math::CSys::CoordinateSystemConverter::GetSourceSRID() const
{
	return this->srcCsys->GetSRID();
}

UInt32 Math::CSys::CoordinateSystemConverter::GetOutputSRID() const
{
	return this->destCsys->GetSRID();
}

Math::Coord2DDbl Math::CSys::CoordinateSystemConverter::Convert2D(Math::Coord2DDbl coord) const
{
	return CoordinateSystem::Convert(this->srcCsys, this->destCsys, coord);
}

Math::Vector3D Math::CSys::CoordinateSystemConverter::Convert3D(Math::Vector3D vec3) const
{
	return CoordinateSystem::Convert3D(this->srcCsys, this->destCsys, vec3);
}

void Math::CSys::CoordinateSystemConverter::Convert2DArr(UnsafeArray<const Math::Coord2DDbl> srcArr, UnsafeArray<Math::Coord2DDbl> destArr, UIntOS nPoints) const
{
	CoordinateSystem::ConvertArray(this->srcCsys, this->destCsys, srcArr, destArr, nPoints);
}
