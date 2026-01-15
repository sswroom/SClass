#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListDbl.h"
#include "Math/CoordinateSystem.h"
#include "Math/Math_C.h"
#include "Math/Geometry/LineString.h"
#include "Math/Geometry/Polygon.h"
#include "Math/Geometry/Polyline.h"

Math::Geometry::LineString::LineString(UInt32 srid, UOSInt nPoint, Bool hasZ, Bool hasM) : Vector2D(srid)
{
	this->pointArr = MemAllocAArr(Math::Coord2DDbl, nPoint);
	this->nPoint = nPoint;
	MemClearAC(this->pointArr.Ptr(), sizeof(Math::Coord2DDbl) * nPoint);
	if (hasZ)
	{
		this->zArr = MemAllocAArr(Double, nPoint);
	}
	else
	{
		this->zArr = nullptr;
	}
	if (hasM)
	{
		this->mArr = MemAllocAArr(Double, nPoint);
	}
	else
	{
		this->mArr = nullptr;
	}
}

Math::Geometry::LineString::LineString(UInt32 srid, UnsafeArray<const Math::Coord2DDbl> pointArr, UOSInt nPoint, UnsafeArrayOpt<Double> zArr, UnsafeArrayOpt<Double> mArr) : Vector2D(srid)
{
	this->pointArr = MemAllocAArr(Math::Coord2DDbl, nPoint);
	this->nPoint = nPoint;
	UnsafeArray<Double> arr;
	UnsafeArray<Double> narr;
	MemCopyAC(this->pointArr.Ptr(), pointArr.Ptr(), nPoint * sizeof(Math::Coord2DDbl));
	if (zArr.SetTo(arr))
	{
		this->zArr = narr = MemAllocAArr(Double, nPoint);
		MemCopyNO(narr.Ptr(), arr.Ptr(), sizeof(Double) * nPoint);
	}
	else
	{
		this->zArr = nullptr;
	}
	if (mArr.SetTo(arr))
	{
		this->mArr = narr = MemAllocAArr(Double, nPoint);
		MemCopyNO(narr.Ptr(), arr.Ptr(), sizeof(Double) * nPoint);
	}
	else
	{
		this->mArr = nullptr;
	}
}

Math::Geometry::LineString::~LineString()
{
	UnsafeArray<Double> arr;
	MemFreeAArr(this->pointArr);
	if (this->zArr.SetTo(arr))
		MemFreeAArr(arr);
	if (this->mArr.SetTo(arr))
		MemFreeAArr(arr);
}

Math::Geometry::Vector2D::VectorType Math::Geometry::LineString::GetVectorType() const
{
	return Math::Geometry::Vector2D::VectorType::LineString;
}

Math::Coord2DDbl Math::Geometry::LineString::GetCenter() const
{
	UnsafeArray<const Math::Coord2DDbl> points;
	UOSInt nPoints;

	Double maxX;
	Double maxY;
	Double minX;
	Double minY;
	Double v;
	points = this->GetPointListRead(nPoints);
	if (nPoints <= 0)
	{
		return Math::Coord2DDbl(0, 0);
	}
	else
	{
		UOSInt i = nPoints;
		minX = maxX = points[0].x;
		minY = maxY = points[0].y;

		while (i-- > 0)
		{
			v = points[i].x;
			if (v > maxX)
			{
				maxX = v;
			}
			if (v < minX)
			{
				minX = v;
			}
			v = points[i].y;
			if (v > maxY)
			{
				maxY = v;
			}
			else if (v < minY)
			{
				minY = v;
			}
		}
		return Math::Coord2DDbl((minX + maxX) * 0.5, (minY + maxY) * 0.5);
	}
}

NN<Math::Geometry::Vector2D> Math::Geometry::LineString::Clone() const
{
	NN<Math::Geometry::LineString> pl;
	NEW_CLASSNN(pl, Math::Geometry::LineString(this->srid, this->pointArr, this->nPoint, this->zArr, this->mArr));
	return pl;
}

Math::RectAreaDbl Math::Geometry::LineString::GetBounds() const
{
	UOSInt i = this->nPoint;
	Math::Coord2DDbl min;
	Math::Coord2DDbl max;
	min = max = this->pointArr[0];
	while (i > 1)
	{
		i -= 1;
		min = min.Min(this->pointArr[i]);
		max = max.Max(this->pointArr[i]);
	}
	return Math::RectAreaDbl(min, max);
}

Double Math::Geometry::LineString::CalBoundarySqrDistance(Math::Coord2DDbl pt, OutParam<Math::Coord2DDbl> nearPt) const
{
	UOSInt l;
	UnsafeArray<Math::Coord2DDbl> points;

	points = this->pointArr;

	l = this->nPoint;

	Double calBase;
	Math::Coord2DDbl calDiff;
	Math::Coord2DDbl calSqDiff;
	Math::Coord2DDbl calPt;
	Math::Coord2DDbl calPtOut = Math::Coord2DDbl(0, 0);
	Double calD;
	Double dist = 0x7fffffff;

	l--;
	while (l-- > 0)
	{
		calDiff = points[l] - points[l + 1];

		if (calDiff.y == 0)
		{
			calPt.x = pt.x;
		}
		else
		{
			calSqDiff = calDiff * calDiff;
			calBase = calSqDiff.x + calSqDiff.y;
			calPt.x = calSqDiff.x * pt.x;
			calPt.x += calSqDiff.y * points[l].x;
			calPt.x += (pt.y - points[l].y) * calDiff.x * calDiff.y;
			calPt.x /= calBase;
		}

		if (calDiff.x == 0)
		{
			calPt.y = pt.y;
		}
		else
		{
			calPt.y = ((calPt.x - points[l].x) * calDiff.y / calDiff.x) + points[l].y;
		}

		if (calDiff.x < 0)
		{
			if (points[l].x > calPt.x)
				continue;
			if (points[l + 1].x < calPt.x)
				continue;
		}
		else
		{
			if (points[l].x < calPt.x)
				continue;
			if (points[l + 1].x > calPt.x)
				continue;
		}

		if (calDiff.y < 0)
		{
			if (points[l].y > calPt.y)
				continue;
			if (points[l + 1].y < calPt.y)
				continue;
		}
		else
		{
			if (points[l].y < calPt.y)
				continue;
			if (points[l + 1].y > calPt.y)
				continue;
		}

		calDiff = pt - calPt;
		calSqDiff = calDiff * calDiff;
		calD = calSqDiff.x + calSqDiff.y;
		if (calD < dist)
		{
			dist = calD;
			calPtOut = calPt;
		}
	}
	l = this->nPoint;
	while (l-- > 0)
	{
		calDiff = pt - points[l];
		calSqDiff = calDiff * calDiff;
		calD = calSqDiff.x + calSqDiff.y;
		if (calD < dist)
		{
			dist = calD;
			calPtOut = points[l];
		}
	}
	nearPt.Set(calPtOut);
	return dist;
}

Double Math::Geometry::LineString::CalArea() const
{
	return 0;
}

Bool Math::Geometry::LineString::JoinVector(NN<const Math::Geometry::Vector2D> vec)
{
	if (vec->GetVectorType() != Math::Geometry::Vector2D::VectorType::LineString || this->HasZ() != vec->HasZ() || this->HasM() != vec->HasM())
	{
		return false;
	}
	NN<LineString> ls = NN<LineString>::ConvertFrom(vec);
	UOSInt nPoint;
	UOSInt i;
	UOSInt j;
	UnsafeArray<const Math::Coord2DDbl> points = ls->GetPointListRead(nPoint);
	UnsafeArray<Math::Coord2DDbl> newPoints;
	if (points[0] == this->pointArr[this->nPoint - 1])
	{
		newPoints = MemAllocAArr(Math::Coord2DDbl, this->nPoint + nPoint - 1);
		MemCopyAC(newPoints.Ptr(), this->pointArr.Ptr(), this->nPoint * sizeof(Math::Coord2DDbl));
		MemCopyAC(&newPoints[this->nPoint], &points[1], (nPoint - 1) * sizeof(Math::Coord2DDbl));
		MemFreeAArr(this->pointArr);
		this->pointArr = newPoints;
		this->nPoint += nPoint - 1;
		return true;
	}
	else if (points[nPoint - 1] == this->pointArr[this->nPoint - 1])
	{
		newPoints = MemAllocAArr(Math::Coord2DDbl, this->nPoint + nPoint - 1);
		MemCopyAC(newPoints.Ptr(), this->pointArr.Ptr(), this->nPoint * sizeof(Math::Coord2DDbl));
		i = nPoint - 1;
		j = this->nPoint;
		while (i-- > 0)
		{
			newPoints[j] = points[i];
			j++;
		}
		MemFreeAArr(this->pointArr);
		this->pointArr = newPoints;
		this->nPoint += nPoint - 1;
		return true;
	}
	else if (points[nPoint - 1] == this->pointArr[0])
	{
		newPoints = MemAllocAArr(Math::Coord2DDbl, this->nPoint + nPoint - 1);
		MemCopyAC(newPoints.Ptr(), points.Ptr(), (nPoint - 1) * sizeof(Math::Coord2DDbl));
		MemCopyAC(&newPoints[nPoint - 1], this->pointArr.Ptr(), this->nPoint * sizeof(Math::Coord2DDbl));
		MemFreeAArr(this->pointArr);
		this->pointArr = newPoints;
		this->nPoint += nPoint - 1;
		return true;
	}
	else if (points[0] == this->pointArr[0])
	{
		newPoints = MemAllocAArr(Math::Coord2DDbl, this->nPoint + nPoint - 1);
		MemCopyAC(&newPoints[nPoint - 1], this->pointArr.Ptr(), this->nPoint * sizeof(Math::Coord2DDbl));
		i = nPoint - 1;
		j = 1;
		while (i-- > 0)
		{
			newPoints[i] = points[j];
			j++;
		}
		MemFreeAArr(this->pointArr);
		this->pointArr = newPoints;
		this->nPoint += nPoint - 1;
		return true;
	}
	return false;
}

Bool Math::Geometry::LineString::HasZ() const
{
	return this->zArr.NotNull();
}

Bool Math::Geometry::LineString::HasM() const
{
	return this->mArr.NotNull();
}

Bool Math::Geometry::LineString::GetZBounds(OutParam<Double> min, OutParam<Double> max) const
{
	UnsafeArray<Double> zArr;
	if (!this->zArr.SetTo(zArr))
		return false;
	Double minVal = zArr[0];
	Double maxVal = minVal;
	UOSInt i = this->nPoint;
	while (i-- > 1)
	{
		if (zArr[i] < minVal)
			minVal = zArr[i];
		if (zArr[i] > maxVal)
			maxVal = zArr[i];
	}
	min.Set(minVal);
	max.Set(maxVal);
	return true;
}

Bool Math::Geometry::LineString::GetMBounds(OutParam<Double> min, OutParam<Double> max) const
{
	UnsafeArray<Double> mArr;
	if (!this->mArr.SetTo(mArr))
		return false;
	Double minVal = mArr[0];
	Double maxVal = minVal;
	UOSInt i = this->nPoint;
	while (i-- > 1)
	{
		if (mArr[i] < minVal)
			minVal = mArr[i];
		if (mArr[i] > maxVal)
			maxVal = mArr[i];
	}
	min.Set(minVal);
	max.Set(maxVal);
	return true;
}

void Math::Geometry::LineString::Convert(NN<Math::CoordinateConverter> converter)
{
	UnsafeArray<Double> zArr;
	if (this->zArr.SetTo(zArr))
	{
		Math::Vector3 tmpPos;
		UOSInt i = this->nPoint;
		while (i-- > 0)
		{
			tmpPos = converter->Convert3D(Math::Vector3(this->pointArr[i], zArr[i]));
			this->pointArr[i] = tmpPos.GetXY();
			zArr[i] = tmpPos.GetZ();
		}
		this->srid = converter->GetOutputSRID();
	}
	else
	{
		converter->Convert2DArr(this->pointArr, this->pointArr, this->nPoint);
		this->srid = converter->GetOutputSRID();
	}
}

Bool Math::Geometry::LineString::Equals(NN<const Vector2D> vec, Bool sameTypeOnly, Bool nearlyVal, Bool no3DGeometry) const
{
	if (vec->GetSRID() != this->srid)
	{
		return false;
	}
	if (vec->GetVectorType() == this->GetVectorType() && (no3DGeometry || (this->HasZ() == vec->HasZ() && this->HasM() == vec->HasM())))
	{
		NN<Math::Geometry::LineString> pl = NN<Math::Geometry::LineString>::ConvertFrom(vec);
		UOSInt nPoint;
		UnsafeArray<Math::Coord2DDbl> ptList = pl->GetPointList(nPoint);
		UnsafeArray<Double> thisArr;
		UnsafeArray<Double> valArr;
		if (nPoint != this->nPoint)
		{
			return false;
		}
		UOSInt i;
		if (nearlyVal)
		{
			i = nPoint;
			while (i-- > 0)
			{
				if (!ptList[i].EqualsNearly(this->pointArr[i]))
				{
					return false;
				}
			}
			if (!no3DGeometry)
			{
				if (this->zArr.SetTo(thisArr) && pl->zArr.SetTo(valArr))
				{
					i = nPoint;
					while (i-- > 0)
					{
						if (!Math::NearlyEqualsDbl(valArr[i], thisArr[i]))
						{
							return false;
						}
					}
				}
				if (this->mArr.SetTo(thisArr) && pl->mArr.SetTo(valArr))
				{
					i = nPoint;
					while (i-- > 0)
					{
						if (!Math::NearlyEqualsDbl(valArr[i], thisArr[i]))
						{
							return false;
						}
					}
				}
			}
		}
		else
		{
			i = nPoint;
			while (i-- > 0)
			{
				if (ptList[i] != this->pointArr[i])
				{
					return false;
				}
			}
			if (!no3DGeometry)
			{
				if (this->zArr.SetTo(thisArr) && pl->zArr.SetTo(valArr))
				{
					i = nPoint;
					while (i-- > 0)
					{
						if (valArr[i] != thisArr[i])
						{
							return false;
						}
					}
				}
				if (this->mArr.SetTo(thisArr) && pl->mArr.SetTo(valArr))
				{
					i = nPoint;
					while (i-- > 0)
					{
						if (valArr[i] != thisArr[i])
						{
							return false;
						}
					}
				}
			}
		}
		return true;
	}
	else
	{
		return false;
	}
}

UOSInt Math::Geometry::LineString::GetCoordinates(NN<Data::ArrayListA<Math::Coord2DDbl>> coordList) const
{
	return coordList->AddRange(this->pointArr, this->nPoint);
}

Bool Math::Geometry::LineString::InsideOrTouch(Math::Coord2DDbl coord) const
{
	Double thisX;
	Double thisY;
	Double lastX;
	Double lastY;
	UOSInt j;
	UOSInt l;
	Double tmpX;

	l = this->nPoint;
	lastX = this->pointArr[0].x;
	lastY = this->pointArr[0].y;
	while (l-- > 0)
	{
		thisX = this->pointArr[l].x;
		thisY = this->pointArr[l].y;
		j = 0;
		if (lastY > coord.y)
			j += 1;
		if (thisY > coord.y)
			j += 1;

		if (j == 1)
		{
			tmpX = lastX - (lastX - thisX) * (lastY - coord.y) / (lastY - thisY);
			if (tmpX == coord.x)
			{
				return true;
			}
		}
		else if (thisY == coord.y && lastY == coord.y)
		{
			if ((thisX >= coord.x && lastX <= coord.x) || (lastX >= coord.x && thisX <= coord.x))
			{
				return true;
			}
		}
		else if (thisY == coord.y && thisX == coord.x)
		{
			return true;
		}

		lastX = thisX;
		lastY = thisY;
	}
	return false;
}

void Math::Geometry::LineString::SwapXY()
{
	UOSInt i = this->nPoint;
	while (i-- > 0)
	{
		this->pointArr[i] = this->pointArr[i].SwapXY();
	}
}

void Math::Geometry::LineString::MultiplyCoordinatesXY(Double v)
{
	UOSInt i = this->nPoint;
	while (i-- > 0)
	{
		this->pointArr[i] = this->pointArr[i] * v;
	}
}

UOSInt Math::Geometry::LineString::GetPointCount() const
{
	return this->nPoint;
}

Bool Math::Geometry::LineString::HasArea() const
{
	return false;
}

UOSInt Math::Geometry::LineString::CalcHIntersacts(Double y, NN<Data::ArrayListNative<Double>> xList) const
{
	if (this->nPoint < 2)
		return 0;
	UOSInt initCnt = xList->GetCount();
	UOSInt i = 1;
	UOSInt j = this->nPoint;
	Math::Coord2DDbl lastPt = this->pointArr[0];
	Math::Coord2DDbl thisPt;
	Double thisX;
	while (i < j)
	{
		thisPt = this->pointArr[i];
		if ((lastPt.y >= y && thisPt.y < y) || (thisPt.y >= y && lastPt.y < y))
		{
			thisX = lastPt.x + (y - lastPt.y) / (thisPt.y - lastPt.y) * (thisPt.x - lastPt.x);
			xList->Add(thisX);
		}
		lastPt = thisPt;
		i++;
	}
	if (this->HasArea())
	{
		thisPt = this->pointArr[0];
		if ((lastPt.y >= y && thisPt.y < y) || (thisPt.y >= y && lastPt.y < y))
		{
			thisX = lastPt.x + (y - lastPt.y) / (thisPt.y - lastPt.y) * (thisPt.x - lastPt.x);
			xList->Add(thisX);
		}
	}
	return xList->GetCount() - initCnt;
}

Math::Coord2DDbl Math::Geometry::LineString::GetDisplayCenter() const
{
	Math::RectAreaDbl bounds = this->GetBounds();
	Math::Coord2DDbl pt = bounds.GetCenter();
	Data::ArrayListNative<Double> xList;
	this->CalcHIntersacts(pt.y, xList);
	if (xList.GetCount() == 0)
	{
		return Math::Coord2DDbl(0, 0);
	}
	return Math::Coord2DDbl(xList.GetItem(xList.GetCount() >> 1), pt.y);
}

Math::Coord2DDbl Math::Geometry::LineString::GetPoint(UOSInt index) const
{
	if (index >= this->nPoint)
	{
		return this->pointArr[0];
	}
	else
	{
		return this->pointArr[index];
	}
}

Double Math::Geometry::LineString::CalcHLength() const
{
	Double leng = 0;
	Math::Coord2DDbl diff;
	UOSInt i = this->nPoint;
	while (i-- > 1)
	{
		diff = this->pointArr[i] - this->pointArr[i - 1];
		leng += Math_Sqrt(diff.x * diff.x + diff.y * diff.y);
	}
	return leng;
}

Double Math::Geometry::LineString::Calc3DLength() const
{
	UnsafeArray<Double> zArr;
	if (this->zArr.SetTo(zArr))
	{
		Double leng = 0;
		Math::Coord2DDbl diff;
		Double zDiff;
		UOSInt i = this->nPoint;
		while (i-- > 1)
		{
			diff = this->pointArr[i] - this->pointArr[i - 1];
			zDiff = zArr[i] - zArr[i - 1];
			leng += Math_Sqrt(diff.x * diff.x + diff.y * diff.y + zDiff * zDiff);
		}
		return leng;
	}
	else
	{
		return this->CalcHLength();
	}
}

void Math::Geometry::LineString::Reverse()
{
	if (this->nPoint == 0)
		return;
	UOSInt i = 0;
	UOSInt j = this->nPoint - 1;
	Math::Coord2DDbl pos;
	Double tmpZ;
	Double tmpM;
	UnsafeArray<Double> zArr;
	UnsafeArray<Double> mArr;
	if (this->zArr.SetTo(zArr))
	{
		if (this->mArr.SetTo(mArr))
		{
			while (i < j)
			{
				pos = this->pointArr[i];
				tmpZ = zArr[i];
				tmpM = mArr[i];
				this->pointArr[i] = this->pointArr[j];
				zArr[i] = zArr[j];
				mArr[i] = mArr[j];
				this->pointArr[j] = pos;
				zArr[i] = tmpZ;
				mArr[j] = tmpM;
				i++;
				j--;
			}
		}
		else
		{
			while (i < j)
			{
				pos = this->pointArr[i];
				tmpZ = zArr[i];
				this->pointArr[i] = this->pointArr[j];
				zArr[i] = zArr[j];
				this->pointArr[j] = pos;
				zArr[j] = tmpZ;
				i++;
				j--;
			}
		}
	}
	else
	{
		if (this->mArr.SetTo(mArr))
		{
			while (i < j)
			{
				pos = this->pointArr[i];
				tmpM = mArr[i];
				this->pointArr[i] = this->pointArr[j];
				mArr[i] = mArr[j];
				this->pointArr[j] = pos;
				mArr[j] = tmpM;
				i++;
				j--;
			}
		}
		else
		{
			while (i < j)
			{
				pos = this->pointArr[i];
				this->pointArr[i] = this->pointArr[j];
				this->pointArr[j] = pos;
				i++;
				j--;
			}
		}
	}
}

void Math::Geometry::LineString::GetNearEnd(Math::Coord2DDbl coord, OutParam<Math::Coord2DDbl> nearEndPt, OptOut<Double> nearEndZ) const
{
	Double dist1 = this->pointArr[0].CalcLengTo(coord);
	Double dist2 = this->pointArr[this->nPoint - 1].CalcLengTo(coord);
	UnsafeArray<Double> zArr;
	if (dist1 < dist2)
	{
		nearEndPt.Set(this->pointArr[0]);
		if (this->zArr.SetTo(zArr))
		{
			nearEndZ.Set(zArr[0]);
		}
		else
		{
			nearEndZ.Set(NAN);
		}
	}
	else
	{
		nearEndPt.Set(this->pointArr[this->nPoint - 1]);
		if (this->zArr.SetTo(zArr))
		{
			nearEndZ.Set(zArr[this->nPoint - 1]);
		}
		else
		{
			nearEndZ.Set(NAN);
		}
	}
}

Optional<Math::Geometry::LineString> Math::Geometry::LineString::SplitByPoint(Math::Coord2DDbl pt)
{
	UOSInt l;
	l = this->nPoint;

	Math::Coord2DDbl calPt;
	Double calZ;
	Double calM;
	Bool isPoint;
	UOSInt minId = (UOSInt)this->GetPointNo(pt, isPoint, calPt, calZ, calM);

	UnsafeArray<Math::Coord2DDbl> oldPoints;
	UnsafeArray<Math::Coord2DDbl> newPoints;
	UnsafeArrayOpt<Double> oldZ;
	UnsafeArrayOpt<Double> newZ;
	UnsafeArrayOpt<Double> oldM;
	UnsafeArrayOpt<Double> newM;
	UnsafeArray<Double> oldArr;
	UnsafeArray<Double> newArr;
	Math::Geometry::LineString *newPL;
	if (isPoint)
	{
		if (minId == this->nPoint - 1 || minId == 0 || minId == (UOSInt)-1)
		{
			return nullptr;
		}
		
		oldPoints = this->pointArr;
		oldZ = this->zArr;
		oldM = this->mArr;

		newPoints = MemAllocAArr(Math::Coord2DDbl, (minId + 1));
		if (oldZ.SetTo(oldArr))
		{
			newZ = newArr = MemAllocAArr(Double, (minId + 1));
			MemCopyAC(newArr.Ptr(), oldArr.Ptr(), sizeof(Double) * (minId + 1));
		}
		else
		{
			newZ = nullptr;
		}
		if (oldM.SetTo(oldArr))
		{
			newM = newArr = MemAllocAArr(Double, (minId + 1));
			MemCopyAC(newArr.Ptr(), oldArr.Ptr(), sizeof(Double) * (minId + 1));
		}
		else
		{
			newM = nullptr;
		}
		l = minId + 1;
		while (l-- > 0)
		{
			newPoints[l] = oldPoints[l];
		}

		this->pointArr = newPoints;
		this->zArr = newZ;
		this->mArr = newM;
		NEW_CLASS(newPL, Math::Geometry::LineString(this->srid, this->nPoint - minId, this->zArr.NotNull(), this->mArr.NotNull()));
		newPoints = newPL->GetPointList(l);
		l = this->nPoint;
		while (l-- > minId)
		{
			newPoints[l - minId] = oldPoints[l];
		}
		if (oldZ.SetTo(oldArr) && newPL->GetZList(l).SetTo(newArr))
		{
			l = this->nPoint;
			while (l-- > minId)
			{
				newArr[l - minId] = oldArr[l];
			}
			MemFreeAArr(oldArr);
		}
		if (oldM.SetTo(oldArr) && newPL->GetMList(l).SetTo(newArr))
		{
			l = this->nPoint;
			while (l-- > minId)
			{
				newArr[l - minId] = oldArr[l];
			}
			MemFreeAArr(oldArr);
		}
		this->nPoint = minId + 1;
		MemFreeAArr(oldPoints);

		return newPL;
	}
	else
	{
		oldPoints = this->pointArr;
		oldZ = this->zArr;
		oldM = this->mArr;

		newPoints = MemAllocAArr(Math::Coord2DDbl, (minId + 2));
		if (oldZ.SetTo(oldArr))
		{
			newZ = newArr = MemAllocAArr(Double, (minId + 2));
			MemCopyAC(newArr.Ptr(), oldArr.Ptr(), sizeof(Double) * (minId + 1));
			newArr[minId + 1] = calZ;
		}
		else
		{
			newZ = nullptr;
		}
		if (oldM.SetTo(oldArr))
		{
			newM = newArr = MemAllocAArr(Double, (minId + 2));
			MemCopyAC(newArr.Ptr(), oldArr.Ptr(), sizeof(Double) * (minId + 1));
			newArr[minId + 1] = calM;
		}
		else
		{
			newM = nullptr;
		}
		l = minId + 1;
		while (l-- > 0)
		{
			newPoints[l] = oldPoints[l];
		}
		newPoints[minId + 1] = calPt;

		this->pointArr = newPoints;
		this->zArr = newZ;
		this->mArr = newM;
		NEW_CLASS(newPL, Math::Geometry::LineString(this->srid, this->nPoint - minId, oldZ.NotNull(), oldM.NotNull()));

		newPoints = newPL->GetPointList(l);
		l = this->nPoint;
		while (--l > minId)
		{
			newPoints[l - minId] = oldPoints[l];
		}
		newPoints[0] = calPt;
		MemFreeAArr(oldPoints);

		if (oldZ.SetTo(oldArr) && newPL->GetZList(l).SetTo(newArr))
		{
			l = this->nPoint;
			while (--l > minId)
			{
				newArr[l - minId] = oldArr[l];
			}
			newArr[0] = calZ;
			MemFreeAArr(oldArr);
		}

		if (oldM.SetTo(oldArr) && newPL->GetMList(l).SetTo(newArr))
		{
			l = this->nPoint;
			while (--l > minId)
			{
				newArr[l - minId] = oldArr[l];
			}
			newArr[0] = calM;
			MemFreeAArr(oldArr);
		}

		this->nPoint = minId + 2;

		return newPL;
	}
}

OSInt Math::Geometry::LineString::GetPointNo(Math::Coord2DDbl pt, OptOut<Bool> isPoint, OptOut<Math::Coord2DDbl> calPtOutPtr, OptOut<Double> calZOutPtr, OptOut<Double> calMOutPtr)
{
	UOSInt l;
	UnsafeArray<Math::Coord2DDbl> points;
	UnsafeArrayOpt<Double> zArr;
	UnsafeArrayOpt<Double> mArr;
	UnsafeArray<Double> arr;

	points = this->pointArr;
	zArr = this->zArr;
	mArr = this->mArr;

	l = this->nPoint;

	Double calBase;
	Math::Coord2DDbl calDiff;
	Math::Coord2DDbl calSqDiff;
	Math::Coord2DDbl calPt;
	Math::Coord2DDbl calPtOut = Math::Coord2DDbl(0, 0);
	Double calZOut = 0;
	Double calMOut = 0;
	Double calZ;
	Double calM;
	Double calD;
	Double dist = 0x7fffffff;
	OSInt minId = -1;
	Bool isPointI = false;

	l--;
	while (l-- > 0)
	{
		calDiff = points[l] - points[l + 1];

		if (calDiff.x == 0 && calDiff.y == 0)
		{
			calPt.x = pt.x;
			calPt.y = pt.y;
			if (zArr.SetTo(arr))
			{
				calZ = arr[l];
			}
			else
			{
				calZ = 0;
			}
			if (mArr.SetTo(arr))
			{
				calM = arr[l];
			}
			else
			{
				calM = 0;
			}
		}
		else
		{
			if (calDiff.y == 0)
			{
				calPt.x = pt.x;
			}
			else
			{
				calSqDiff = calDiff * calDiff;
				calBase = calSqDiff.x + calSqDiff.y;
				calPt.x = calSqDiff.x * pt.x;
				calPt.x += calSqDiff.y * points[l].x;
				calPt.x += (pt.y - points[l].y) * calDiff.x * calDiff.y;
				calPt.x /= calBase;

				if (calDiff.x == 0)
				{
					////////////////////////////////
					calZ = 0;
					calM = 0;
				}
			}

			if (calDiff.x == 0)
			{
				calPt.y = pt.y;
			}
			else
			{
				Double ratio = (calPt.x - (points[l].x)) / calDiff.x;
				calPt.y = (ratio * calDiff.y) + points[l].y;
				if (zArr.SetTo(arr))
				{
					calZ = (ratio * (arr[l] - arr[l + 1])) + arr[l];
				}
				else
				{
					calZ = 0;
				}
				if (mArr.SetTo(arr))
				{
					calM = (ratio * (arr[l] - arr[l + 1])) + arr[l];
				}
				else
				{
					calM = 0;
				}
			}
		}

		if (calDiff.x < 0)
		{
			if (points[l].x > calPt.x)
				continue;
			if (points[l + 1].x < calPt.x)
				continue;
		}
		else
		{
			if (points[l].x < calPt.x)
				continue;
			if (points[l + 1].x > calPt.x)
				continue;
		}

		if (calDiff.y < 0)
		{
			if (points[l].y > calPt.y)
				continue;
			if (points[l + 1].y < calPt.y)
				continue;
		}
		else
		{
			if (points[l].y < calPt.y)
				continue;
			if (points[l + 1].y > calPt.y)
				continue;
		}

		calDiff = pt - calPt;
		calSqDiff = calDiff * calDiff;
		calD = calSqDiff.x + calSqDiff.y;
		if (calD < dist)
		{
			dist = calD;
			calPtOut = calPt;
			calZOut = calZ;
			calMOut = calM;
			isPointI = false;
			minId = (OSInt)l;
		}
	}
	l = this->nPoint;
	while (l-- > 0)
	{
		calDiff = pt - points[l];
		calSqDiff = calDiff * calDiff;
		calD = calSqDiff.x + calSqDiff.y;
		if (calD < dist)
		{
			dist = calD;
			calPtOut = points[l];
			calZOut = zArr.SetTo(arr)?arr[l]:0;
			calMOut = mArr.SetTo(arr)?arr[l]:0;
			minId = (OSInt)l;
			isPointI = true;
		}
	}

	isPoint.Set(isPointI);
	calPtOutPtr.Set(calPtOut);
	calZOutPtr.Set(calZOut);
	calMOutPtr.Set(calMOut);
	return minId;
}

Optional<Math::Geometry::Polygon> Math::Geometry::LineString::CreatePolygonByDist(Double dist) const
{
	if (this->nPoint < 2)
		return nullptr;

	Data::ArrayListDbl outPoints;
	Double lastPtX = 0;
	Double lastPtY = 0;
	Double thisPtX = 0;
	Double thisPtY = 0;
	Double nextPtX;
	Double nextPtY;
	Double t1;
	Double t2;
	Double deg;
	UOSInt i;

	deg = Math_ArcTan2(this->pointArr[1].x - this->pointArr[0].x, this->pointArr[1].y - this->pointArr[0].y);
	lastPtX = -Math_Cos(deg) * dist + this->pointArr[0].x;
	lastPtY = Math_Sin(deg) * dist + this->pointArr[0].y;

	outPoints.Add(lastPtX);
	outPoints.Add(lastPtY);

	i = 2;
	while (i < this->nPoint)
	{
		deg = Math_ArcTan2(this->pointArr[i].x - this->pointArr[i - 1].x, this->pointArr[i].y - this->pointArr[i - 1].y);
		nextPtX = -Math_Cos(deg) * dist + this->pointArr[i - 1].x;
		nextPtY = Math_Sin(deg) * dist + this->pointArr[i - 1].y;

		t1 = (this->pointArr[i - 2].y - this->pointArr[i - 1].y) / (this->pointArr[i - 2].x - this->pointArr[i - 1].x);
		t2 = (this->pointArr[i - 1].y - this->pointArr[i].y) / (this->pointArr[i - 1].x - this->pointArr[i].x);
		if (t1 != t2)
		{
			Double x1 = this->pointArr[i - 2].x;
			Double x2 = this->pointArr[i - 1].x;
			Double x3 = this->pointArr[i].x;
			Double x4 = lastPtX;
			Double x6 = nextPtX;
			Double y1 = this->pointArr[i - 2].y;
			Double y2 = this->pointArr[i - 1].y;
			Double y3 = this->pointArr[i].y;
			Double y4 = lastPtY;
			Double y6 = nextPtY;

			thisPtX = (x4 * (x2 - x3) * (y2 - y1) - x6 * (x2 - x1) * (y2 - y3) + y6 * (x2 - x1) * (x2 - x3) - y4 * (x2 - x1) * (x2 - x3)) / ((y2 - y1) * (x2 - x3) - (x2 - x1) * (y2 - y3));
			if ((x2 - x1) == 0)
				thisPtY = y6 + (y2 - y3) / (x2 - x3) * (thisPtX - x6);
			else
				thisPtY = y4 + (y2 - y1) / (x2 - x1) * (thisPtX - x4);

			outPoints.Add(thisPtX);
			outPoints.Add(thisPtY);

		}
		lastPtX = thisPtX;
		lastPtY = thisPtY;
		i += 1;
	}

	deg = Math_ArcTan2(this->pointArr[this->nPoint - 1].x - this->pointArr[this->nPoint - 2].x, this->pointArr[this->nPoint - 1].y - this->pointArr[this->nPoint - 2].y);
	lastPtX = -Math_Cos(deg) * dist + this->pointArr[this->nPoint - 1].x;
	lastPtY = Math_Sin(deg) * dist + this->pointArr[this->nPoint- 1].y;

	outPoints.Add(lastPtX);
	outPoints.Add(lastPtY);

	lastPtX = Math_Cos(deg) * dist + this->pointArr[this->nPoint - 1].x;
	lastPtY = -Math_Sin(deg) * dist + this->pointArr[this->nPoint - 1].y;

	outPoints.Add(lastPtX);
	outPoints.Add(lastPtY);

	i = this->nPoint;
	while (i > 2)
	{
		i -= 1;
		deg = Math_ArcTan2(this->pointArr[i - 2].x - this->pointArr[i - 1].x, this->pointArr[i - 2].y - this->pointArr[i - 1].y);
		nextPtX = -Math_Cos(deg) * dist + this->pointArr[i - 2].x;
		nextPtY = Math_Sin(deg) * dist + this->pointArr[i - 2].y;

		t2 = (this->pointArr[i - 2].y - this->pointArr[i - 1].y) / (this->pointArr[i - 2].x - this->pointArr[i - 1].x);
		t1 = (this->pointArr[i - 1].y - this->pointArr[i].y) / (this->pointArr[i - 1].x - this->pointArr[i].x);

		if (t1 != t2)
		{
			Double x1 = this->pointArr[i].x;
			Double x2 = this->pointArr[i - 1].x;
			Double x3 = this->pointArr[i - 2].x;
			Double x4 = lastPtX;
			Double x6 = nextPtX;
			Double y1 = this->pointArr[i].y;
			Double y2 = this->pointArr[i - 1].y;
			Double y3 = this->pointArr[i - 2].y;
			Double y4 = lastPtY;
			Double y6 = nextPtY;



			thisPtX = (x4 * (x2 - x3) * (y2 - y1) - x6 * (x2 - x1) * (y2 - y3) + y6 * (x2 - x1) * (x2 - x3) - y4 * (x2 - x1) * (x2 - x3)) / ((y2 - y1) * (x2 - x3) - (x2 - x1) * (y2 - y3));
			if ((x2 - x1) == 0)
				thisPtY = y6 + (y2 - y3) / (x2 - x3) * (thisPtX - x6);
			else
				thisPtY = y4 + (y2 - y1) / (x2 - x1) * (thisPtX - x4);

			outPoints.Add(thisPtX);
			outPoints.Add(thisPtY);
		}

		lastPtX = thisPtX;
		lastPtY = thisPtY;
	}
	deg = Math_ArcTan2(this->pointArr[1].x - this->pointArr[0].x, this->pointArr[1].y - this->pointArr[0].y);

	lastPtX = Math_Cos(deg) * dist + this->pointArr[0].x;
	lastPtY = -Math_Sin(deg) * dist + this->pointArr[0].y;

	outPoints.Add(lastPtX);
	outPoints.Add(lastPtY);

	Math::Geometry::Polygon *pg;
	NN<Math::Geometry::LinearRing> lr;
	UOSInt nPoints;
	UnsafeArray<Math::Coord2DDbl> pts;
	NEW_CLASS(pg, Math::Geometry::Polygon(this->srid));
	NEW_CLASSNN(lr, Math::Geometry::LinearRing(this->srid, outPoints.GetCount() >> 1, false, false));
	pts = lr->GetPointList(nPoints);
	i = 0;
	while (i < nPoints)
	{
		pts[i].x = outPoints.GetItem((i << 1));
		pts[i].y = outPoints.GetItem((i << 1) + 1);
		i++;
	}
	pg->AddGeometry(lr);
	return pg;
}

NN<Math::Geometry::Polyline> Math::Geometry::LineString::CreatePolyline() const
{
	NN<Math::Geometry::Polyline> pl;
	NEW_CLASSNN(pl, Math::Geometry::Polyline(this->srid));
	pl->AddGeometry(NN<LineString>::ConvertFrom(this->Clone()));
	return pl;
}

Optional<Math::Geometry::LineString> Math::Geometry::LineString::JoinLines(NN<Data::ArrayListNN<LineString>> lines)
{
	if (lines->GetCount() == 0)
		return nullptr;
	Bool hasZ = true;
	Bool hasM = true;
	UOSInt nPoints = 0;
	Math::Coord2DDbl lastPt;
	UOSInt i = 1;
	UOSInt j = lines->GetCount();
	UOSInt k;
	NN<LineString> ls;
	NN<LineString> ls2;
	UInt32 srid;
	ls = lines->GetItemNoCheck(0);
	srid = ls->GetSRID();
	if (!ls->HasM()) hasM = false;
	if (!ls->HasZ()) hasZ = false;
	if (j > 1)
	{
		ls2 = lines->GetItemNoCheck(1);
		lastPt = ls->GetPoint(ls->GetPointCount() - 1);
		if (ls2->GetPoint(0) == lastPt || ls2->GetPoint(ls2->GetPointCount() - 1) == lastPt)
		{
		}
		else
		{
			lastPt = ls->GetPoint(0);
		}
	}
	else
	{
		lastPt = ls->GetPoint(ls->GetPointCount() - 1);
	}
	nPoints = ls->GetPointCount();
	while (i < j)
	{
		ls = lines->GetItemNoCheck(i);
		if (!ls->HasM()) hasM = false;
		if (!ls->HasZ()) hasZ = false;
		nPoints += ls->GetPointCount();
		if (ls->GetPoint(0) == lastPt)
		{
			nPoints--;
			lastPt = ls->GetPoint(ls->GetPointCount() - 1);
		}
		else if (ls->GetPoint(ls->GetPointCount() - 1) == lastPt)
		{
			nPoints--;
			lastPt = ls->GetPoint(0);
		}
		else
		{
/*			printf("LineString.JoinLines: two lines cannot join, Lines: %d/%d\r\n", (UInt32)i, (UInt32)j);
			Math::WKTWriter writer;
			Text::StringBuilderUTF8 sb;
			writer.ToText(sb, ls);
			printf("LineString.JoinLines: this line: %s\r\n", sb.v.Ptr());
			sb.ClearStr();
			writer.ToText(sb, lines->GetItemNoCheck(i - 1));
			printf("LineString.JoinLines: last line: %s\r\n", sb.v.Ptr());*/
			return nullptr;
		}
		i++;
	}

	NN<LineString> newLine;
	NEW_CLASSNN(newLine, LineString(srid, nPoints, hasZ, hasM));
	UnsafeArray<Math::Coord2DDbl> ptList;
	UnsafeArray<Math::Coord2DDbl> sptList;
	UnsafeArray<Double> sList;
	UnsafeArray<Double> nList;
	UOSInt i2;
	UOSInt j2;
	UOSInt tmp;
	UOSInt startI;
	ptList = newLine->GetPointList(tmp);
	k = 0;
	i = 0;
	while (i < j)
	{
		ls = lines->GetItemNoCheck(i);
		nPoints = ls->GetPointCount();
		if (k == 0 || ls->GetPoint(0) == lastPt)
		{
			if (k == 0)
			{
				startI = 0;
			}
			else
			{
				startI = 1;
				nPoints--;
			}
			MemCopyNO(&ptList[k], &ls->GetPointList(tmp)[startI], sizeof(Math::Coord2DDbl) * nPoints);
			if (newLine->GetZList(tmp).SetTo(nList) && ls->GetZList(tmp).SetTo(sList))
			{
				MemCopyNO(&nList[k], &sList[startI], sizeof(Double) * nPoints);
			}
			if (newLine->GetMList(tmp).SetTo(nList) && ls->GetMList(tmp).SetTo(sList))
			{
				MemCopyNO(&nList[k], &sList[startI], sizeof(Double) * nPoints);
			}
			lastPt = ls->GetPoint(ls->GetPointCount() - 1);
		}
		else
		{
			nPoints--;
			sptList = ls->GetPointList(tmp);			
			i2 = 0;
			j2 = nPoints;
			while (j2-- > 0)
			{
				ptList[k + i2] = sptList[j2];
				i2++;
			}
			if (newLine->GetZList(tmp).SetTo(nList) && ls->GetZList(tmp).SetTo(sList))
			{
				i2 = 0;
				j2 = nPoints;
				while (j2-- > 0)
				{
					nList[k + i2] = sList[j2];
					i2++;
				}
			}
			if (newLine->GetMList(tmp).SetTo(nList) && ls->GetMList(tmp).SetTo(sList))
			{
				i2 = 0;
				j2 = nPoints;
				while (j2-- > 0)
				{
					nList[k + i2] = sList[j2];
					i2++;
				}
			}
			lastPt = ls->GetPoint(0);
		}
		
		k += nPoints;
		i++;
	}
	return newLine;
}
