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

NN<Math::Geometry::Vector2D> Math::Geometry::CircularString::Clone() const
{
	NN<Math::Geometry::CircularString> pl;
	UnsafeArray<Double> thisArr;
	UnsafeArray<Double> plArr;
	NEW_CLASSNN(pl, Math::Geometry::CircularString(this->srid, this->nPoint, this->zArr.NotNull(), this->mArr.NotNull()));
	MemCopyAC(pl->pointArr.Ptr(), this->pointArr.Ptr(), sizeof(Math::Coord2DDbl) * nPoint);
	if (this->zArr.SetTo(thisArr) && pl->zArr.SetTo(plArr))
	{	
		MemCopyAC(plArr.Ptr(), thisArr.Ptr(), sizeof(Double) * nPoint);
	}
	if (this->mArr.SetTo(thisArr) && pl->mArr.SetTo(plArr))
	{	
		MemCopyAC(plArr.Ptr(), thisArr.Ptr(), sizeof(Double) * nPoint);
	}
	return pl;
}
