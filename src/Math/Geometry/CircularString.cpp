#include "Stdafx.h"
#include "Math/GeometryTool.h"
#include "Math/Geometry/CircularString.h"

Math::Geometry::CircularString::CircularString(UInt32 srid, UIntOS nPoint, Bool hasZ, Bool hasM) : LineString(srid, nPoint | 1, hasZ, hasM)
{
}

Math::Geometry::CircularString::CircularString(UInt32 srid, UnsafeArray<const Math::Coord2DDbl> pointArr, UIntOS nPoint, UnsafeArrayOpt<Double> zArr, UnsafeArrayOpt<Double> mArr) : LineString(srid, pointArr, nPoint, zArr, mArr)
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

Optional<Math::Geometry::Vector2D> Math::Geometry::CircularString::ToSimpleShape() const
{
	if (this->nPoint > 2 && (this->nPoint & 1) != 0)
	{
		Data::ArrayListA<Math::Coord2DDbl> ptList;
		UIntOS ret = 0;
		UIntOS i = 2;
		while (i < this->nPoint)
		{
			if (ret > 0)
			{
				ptList.RemoveAt(ptList.GetCount() - 1);
				ret--;
			}
			ret += Math::GeometryTool::ArcToLine(this->pointArr[i - 2], pointArr[i - 1], pointArr[i], 2.5, ptList);
		}
		NN<Math::Geometry::LineString> ls;
		NEW_CLASSNN(ls, Math::Geometry::LineString(this->srid, ptList.Arr(), ptList.GetCount(), nullptr, nullptr));
		return ls;
	}
	return nullptr;
}