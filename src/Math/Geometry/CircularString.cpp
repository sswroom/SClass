#include "Stdafx.h"
#include "Math/Geometry/CircularString.h"

Math::Geometry::CircularString::CircularString(UInt32 srid, UOSInt nPoint, Bool hasZ, Bool hasM) : LineString(srid, nPoint | 1, hasZ, hasM)
{
}

Math::Geometry::CircularString::~CircularString()
{

}

Math::Geometry::Vector2D::VectorType Math::Geometry::CircularString::GetVectorType() const
{
	return VectorType::CircularString;
}

Math::Geometry::Vector2D *Math::Geometry::CircularString::Clone() const
{
	Math::Geometry::CircularString *pl;
	NEW_CLASS(pl, Math::Geometry::CircularString(this->srid, this->nPoint, this->zArr != 0, this->mArr != 0));
	MemCopyAC(pl->pointArr, this->pointArr, sizeof(Math::Coord2DDbl) * nPoint);
	if (this->zArr)
	{	
		MemCopyAC(pl->zArr, this->zArr, sizeof(Double) * nPoint);
	}
	if (this->mArr)
	{	
		MemCopyAC(pl->mArr, this->mArr, sizeof(Double) * nPoint);
	}
	return pl;
}
