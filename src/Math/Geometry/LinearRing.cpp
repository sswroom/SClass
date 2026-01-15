#include "Stdafx.h"
#include "Data/ArrayListDbl.h"
#include "Data/Sort/ArtificialQuickSort_C.h"
#include "Math/Geometry/LinearRing.h"

Math::Geometry::LinearRing::LinearRing(UInt32 srid, UIntOS nPoint, Bool hasZ, Bool hasM) : LineString(srid, nPoint, hasZ, hasM)
{
}

Math::Geometry::LinearRing::LinearRing(UInt32 srid, UnsafeArray<const Math::Coord2DDbl> pointArr, UIntOS nPoint, UnsafeArrayOpt<Double> zArr, UnsafeArrayOpt<Double> mArr) : LineString(srid, pointArr, nPoint, zArr, mArr)
{
}

Math::Geometry::LinearRing::~LinearRing()
{
}

Math::Geometry::Vector2D::VectorType Math::Geometry::LinearRing::GetVectorType() const
{
	return Math::Geometry::Vector2D::VectorType::LinearRing;
}

NN<Math::Geometry::Vector2D> Math::Geometry::LinearRing::Clone() const
{
	NN<Math::Geometry::LinearRing> lr;
	UnsafeArray<Double> thisArr;
	UnsafeArray<Double> lrArr;
	NEW_CLASSNN(lr, Math::Geometry::LinearRing(this->srid, this->nPoint, this->zArr.NotNull(), this->mArr.NotNull()));
	MemCopyNO(lr->pointArr.Ptr(), this->pointArr.Ptr(), sizeof(Math::Coord2DDbl) * this->nPoint);
	if (this->zArr.SetTo(thisArr) && lr->zArr.SetTo(lrArr))
	{
		MemCopyNO(lrArr.Ptr(), thisArr.Ptr(), sizeof(Double) * this->nPoint);
	}
	if (this->mArr.SetTo(thisArr) && lr->mArr.SetTo(lrArr))
	{
		MemCopyNO(lrArr.Ptr(), thisArr.Ptr(), sizeof(Double) * this->nPoint);
	}
	return lr;
}

Bool Math::Geometry::LinearRing::InsideOrTouch(Math::Coord2DDbl coord) const
{
	Math::Coord2DDbl thisPt;
	Math::Coord2DDbl lastPt;
	Math::Coord2DDbl diff;
	UIntOS l;
	Int32 leftCnt = 0;
	Double tmpX;

	l = this->nPoint;
	lastPt = this->pointArr[0];
	while (l-- > 0)
	{
		thisPt = this->pointArr[l];
		if ((lastPt.y > coord.y) ^ (thisPt.y > coord.y))
		{
			diff = lastPt - thisPt;
			tmpX = lastPt.x - diff.x * (lastPt.y - coord.y) / diff.y;
			if (tmpX == coord.x)
			{
				return true;
			}
			else if (tmpX < coord.x)
				leftCnt++;
		}
		else if (thisPt.y == coord.y)
		{
			if (thisPt.x == coord.x)
			{
				return true;
			}
			else if (lastPt.y == coord.y && ((thisPt.x >= coord.x && lastPt.x <= coord.x) || (lastPt.x >= coord.x && thisPt.x <= coord.x)))
			{
				return true;
			}
		}

		lastPt = thisPt;
	}

	return (leftCnt & 1) != 0;
}

Double Math::Geometry::LinearRing::CalArea() const
{
	Math::Coord2DDbl lastPt = this->pointArr[0];
	Double total = 0;
	UIntOS i = this->nPoint;
	while (i-- > 0)
	{
		total = total + (lastPt.x * this->pointArr[i].y - lastPt.y * this->pointArr[i].x);
		lastPt = this->pointArr[i];
	}
	if (total < 0)
		return total * -0.5;
	else
		return total * 0.5;
}

Bool Math::Geometry::LinearRing::HasArea() const
{
	return true;
}

Math::Coord2DDbl Math::Geometry::LinearRing::GetDisplayCenter() const
{
	Math::RectAreaDbl bounds = this->GetBounds();
	Math::Coord2DDbl pt = bounds.GetCenter();
	Data::ArrayListDbl xList;
	this->CalcHIntersacts(pt.y, xList);
	if (xList.GetCount() == 0)
	{
		return Math::Coord2DDbl(0, 0);
	}
	ArtificialQuickSort_SortDouble(xList.Arr().Ptr(), 0, (IntOS)xList.GetCount() - 1);
	Double x = LinearRing::GetIntersactsCenter(xList);
	return Math::Coord2DDbl(x, pt.y);
}

Bool Math::Geometry::LinearRing::IsOpen() const
{
	return this->pointArr[0] != this->pointArr[this->nPoint - 1];
}

Bool Math::Geometry::LinearRing::IsClose() const
{
	return this->pointArr[0] == this->pointArr[this->nPoint - 1];
}

Double Math::Geometry::LinearRing::GetIntersactsCenter(NN<Data::ArrayListNative<Double>> vals)
{
	if (vals->GetCount() == 0)
	{
		return 0;
	}
	UIntOS i = vals->GetCount();
	if ((i & 1) != 0)
	{
		return 0;
	}
	Double totalLength = 0;
	Double leng;
	while (i > 0)
	{
		i -= 2;
		totalLength += vals->GetItem(i + 1) - vals->GetItem(i);
	}
	totalLength = totalLength * 0.5;
	i = vals->GetCount();
	while (i > 0)
	{
		i -= 2;
		leng = vals->GetItem(i + 1) - vals->GetItem(i);
		if (totalLength <= leng)
			return vals->GetItem(i + 1) - totalLength;
		totalLength -= leng;
	}
	return vals->GetItem(0);
}

NN<Math::Geometry::LinearRing> Math::Geometry::LinearRing::CreateFromCircle(UInt32 srid, Math::Coord2DDbl center, Double radiusX, Double radiusY, UIntOS nPoints)
{
	NN<Math::Geometry::LinearRing> lr;
	NEW_CLASSNN(lr, Math::Geometry::LinearRing(srid, nPoints + 1, false, false));
	Double ratio = 2 * Math::PI / UIntOS2Double(nPoints);
	UIntOS i;
	UIntOS j;
	Double angle;
	UnsafeArray<Math::Coord2DDbl> pointList = lr->GetPointList(j);
	i = 0;
	while (i < j)
	{
		angle = UIntOS2Double(i) * ratio;
		pointList[i] = center + Math::Coord2DDbl(radiusX * Math_Cos(angle), radiusY * Math_Sin(angle));
		i++;
	}
	return lr;
}
