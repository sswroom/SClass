#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/CoordinateSystem.h"
#include "Math/Math_C.h"
#include "Math/Geometry/Polyline.h"
#include "Data/ArrayListDbl.h"

NN<Math::Geometry::Vector2D> Math::Geometry::Polyline::Clone() const
{
	NN<Math::Geometry::Polyline> pl;
	NEW_CLASSNN(pl, Math::Geometry::Polyline(this->srid));
	Data::ArrayIterator<NN<LineString>> it = this->geometries.Iterator();
	while (it.HasNext())
	{
		pl->AddGeometry(NN<LineString>::ConvertFrom(it.Next()->Clone()));
	}
	pl->flags = this->flags;
	pl->color = this->color;
	return pl;
}

Double Math::Geometry::Polyline::CalBoundarySqrDistance(Math::Coord2DDbl pt, OutParam<Math::Coord2DDbl> nearPt) const
{
	Data::ArrayIterator<NN<Math::Geometry::LineString>> it = this->geometries.Iterator();
	if (!it.HasNext())
	{
		nearPt.Set(Math::Coord2DDbl(0, 0));
		return 9999999.0;
	}
	Math::Coord2DDbl minPt;
	Double minDist = it.Next()->CalBoundarySqrDistance(pt, minPt);
	while (it.HasNext())
	{
		Math::Coord2DDbl thisPt;
		Double thisDist = it.Next()->CalBoundarySqrDistance(pt, thisPt);
		if (thisDist < minDist)
		{
			minDist = thisDist;
			minPt = thisPt;
		}
	}
	nearPt.Set(minPt);
	return minDist;
}

Bool Math::Geometry::Polyline::JoinVector(NN<const Math::Geometry::Vector2D> vec)
{
	if (vec->GetVectorType() != Math::Geometry::Vector2D::VectorType::Polyline || this->HasZ() != vec->HasZ() || this->HasM() != vec->HasM())
	{
		return false;
	}
	NN<Math::Geometry::Polyline> pl = NN<Math::Geometry::Polyline>::ConvertFrom(vec);
	Data::ArrayIterator<NN<LineString>> it = pl->Iterator();
	while (it.HasNext())
	{
		this->AddGeometry(NN<LineString>::ConvertFrom(it.Next()->Clone()));
	}
	return true;
}

void Math::Geometry::Polyline::AddFromPtOfst(UnsafeArray<UInt32> ptOfstList, UOSInt nPtOfst, UnsafeArray<Math::Coord2DDbl> pointList, UOSInt nPoint, UnsafeArrayOpt<Double> zList, UnsafeArrayOpt<Double> mList)
{
	NN<LineString> lineString;
	UOSInt i = 0;
	UOSInt j;
	UOSInt k;
	UOSInt tmp;
	UnsafeArray<Math::Coord2DDbl> ptArr;
	UnsafeArray<Double> zArr;
	UnsafeArray<Double> mArr;
	UnsafeArray<Double> nnList;
	while (i < nPtOfst)
	{
		j = ptOfstList[i];
		if (i + 1 >= nPtOfst)
			k = nPoint;
		else
			k = ptOfstList[i + 1];
		NEW_CLASSNN(lineString, LineString(this->srid, (k - j), zList.NotNull(), mList.NotNull()));
		ptArr = lineString->GetPointList(tmp);
		MemCopyNO(ptArr.Ptr(), &pointList[j], (k - j) * sizeof(Math::Coord2DDbl));
		if (zList.SetTo(nnList) && lineString->GetZList(tmp).SetTo(zArr))
		{
			MemCopyNO(zArr.Ptr(), &nnList[j], (k - j) * sizeof(Double));
		}
		if (mList.SetTo(nnList) && lineString->GetMList(tmp).SetTo(mArr))
		{
			MemCopyNO(mArr.Ptr(), &nnList[j], (k - j) * sizeof(Double));
		}
		this->AddGeometry(lineString);
		i++;
	}
}

Double Math::Geometry::Polyline::CalcHLength() const
{
	Double dist = 0;
	Data::ArrayIterator<NN<LineString>> it = this->Iterator();
	while (it.HasNext())
	{
		dist += it.Next()->CalcHLength();
	}
	return dist;
}

Double Math::Geometry::Polyline::Calc3DLength() const
{
	Double dist = 0;
	Data::ArrayIterator<NN<LineString>> it = this->Iterator();
	while (it.HasNext())
	{
		dist += it.Next()->Calc3DLength();
	}
	return dist;
}

UOSInt Math::Geometry::Polyline::FillPointOfstList(UnsafeArray<Math::Coord2DDbl> pointList, UnsafeArray<UInt32> ptOfstList, UnsafeArrayOpt<Double> zList, UnsafeArrayOpt<Double> mList) const
{
	UOSInt totalCnt = 0;
	UOSInt nPoint;
	NN<LineString> lineString;
	UnsafeArray<Math::Coord2DDbl> thisPtList;
	UnsafeArray<Double> dList;
	UnsafeArray<Double> nnList;
	UOSInt k;
	Data::ArrayIterator<NN<LineString>> it = this->geometries.Iterator();
	UOSInt i = 0;
	while (it.HasNext())
	{
		ptOfstList[i] = (UInt32)totalCnt;
		lineString = it.Next();
		thisPtList = lineString->GetPointList(nPoint);
		MemCopyNO(&pointList[totalCnt], thisPtList.Ptr(), sizeof(Math::Coord2DDbl) * nPoint);
		if (zList.SetTo(nnList))
		{
			if (lineString->GetZList(k).SetTo(dList))
			{
				MemCopyNO(&nnList[totalCnt], dList.Ptr(), sizeof(Double) * k);
			}
		}
		if (mList.SetTo(nnList))
		{
			if (lineString->GetMList(k).SetTo(dList))
			{
				MemCopyNO(&nnList[totalCnt], dList.Ptr(), sizeof(Double) * k);
			}
		}
		totalCnt += nPoint;
		i++;
	}
	return totalCnt;
}

Math::Coord2DDbl Math::Geometry::Polyline::CalcPosAtDistance(Double dist) const
{
	NN<Math::Geometry::LineString> lineString;
	UnsafeArray<Math::Coord2DDbl> points;
	Math::Coord2DDbl lastPt = Math::Coord2DDbl(0, 0);
	Data::ArrayIterator<NN<LineString>> it = this->geometries.Iterator();
	UOSInt k;
	UOSInt nPoint;
	Math::Coord2DDbl diff;
	Double thisDist;
	while (it.HasNext())
	{
		lineString = it.Next();
		points = lineString->GetPointList(nPoint);
		if (dist <= 0)
		{
			return points[0];
		}
		else
		{
			lastPt = points[nPoint - 1];
			k = 1;
			while (k < nPoint)
			{
				diff = points[k - 1] - points[k];
				thisDist = Math_Sqrt(diff.x * diff.x + diff.y * diff.y);
				if (thisDist > dist)
				{
					return points[k - 1] + (points[k] - points[k - 1]) * dist / thisDist;
				}
				else
				{
					dist -= thisDist;
				}
				k++;
			}
		}
	}
	return lastPt;
}

Optional<Math::Geometry::Polyline> Math::Geometry::Polyline::SplitByPoint(Math::Coord2DDbl pt)
{
	Math::Coord2DDbl calPt;
	Double calZ;
	Double calM;
	Bool isPoint;
	UOSInt minId = (UOSInt)this->GetPointNo(pt, isPoint, calPt, calZ, calM);

	NN<Math::Geometry::LineString> lineString;
	Math::Geometry::Polyline *newPL;
	
	if (minId == (UOSInt)-1)
		return nullptr;
	Data::ArrayIterator<NN<LineString>> it = this->geometries.Iterator();
	UOSInt i = 0;
	while (it.HasNext())
	{
		lineString = it.Next();
		UOSInt nPoint = lineString->GetPointCount();
		if (minId == 0 && isPoint)
		{
			if (i == 0)
				return nullptr;
			NEW_CLASS(newPL, Math::Geometry::Polyline(this->srid));
			while (this->geometries.RemoveAt(i).SetTo(lineString))
			{
				newPL->AddGeometry(lineString);
			}
			return newPL;
		}
		else if (minId == nPoint - 1)
		{
			if (i + 1 == this->geometries.GetCount())
				return nullptr;
			NEW_CLASS(newPL, Math::Geometry::Polyline(this->srid));
			while (this->geometries.RemoveAt(i + 1).SetTo(lineString))
			{
				newPL->AddGeometry(lineString);
			}
			return newPL;
		}
		else if (minId < nPoint)
		{
			NEW_CLASS(newPL, Math::Geometry::Polyline(this->srid));
			if (lineString->SplitByPoint(pt).SetTo(lineString))
			{
				newPL->AddGeometry(lineString);
			}
			while (this->geometries.RemoveAt(i + 1).SetTo(lineString))
			{
				newPL->AddGeometry(lineString);
			}
			return newPL;
		}
		else
		{
			minId -= nPoint;
		}
		i++;
	}
	return nullptr;
}

/*void Math::Geometry::Polyline::OptimizePolyline()
{
	if (this->zArr || this->mArr)
		return;
	Math::Coord2DDbl *tmpPoints = MemAllocA(Math::Coord2DDbl, this->nPoint);
	UInt32 lastPoints = (UInt32)this->nPoint;
	UInt32 thisPoints;
	UInt32 lastChkPoint;
	UInt32 thisChkPoint;
	UOSInt i = this->nPtOfst;
	UOSInt j;
	while (i-- > 0)
	{
		thisPoints = this->ptOfstArr[i];
		lastChkPoint = thisPoints;
		j = i;
		while (j-- > 0)
		{
			thisChkPoint = this->ptOfstArr[j];
			if (this->pointArr[lastChkPoint - 1] == this->pointArr[thisPoints])
			{
				MemCopyNO(tmpPoints, &this->pointArr[thisPoints], sizeof(Math::Coord2DDbl) * (lastPoints - thisPoints));
				if (lastPoints < this->nPoint)
				{
					MemCopyO(&this->pointArr[lastPoints - 1], &this->pointArr[lastPoints], sizeof(Math::Coord2DDbl) * (this->nPoint - lastPoints));
				}
				if (lastChkPoint < thisPoints)
				{
					MemCopyNO(&tmpPoints[(lastPoints - thisPoints)], &this->pointArr[lastChkPoint], sizeof(Math::Coord2DDbl) * (thisPoints - lastChkPoint));
					MemCopyNO(&this->pointArr[lastChkPoint], tmpPoints + 1, sizeof(Math::Coord2DDbl) * (lastPoints - lastChkPoint - 1));
				}
				else
				{
					MemCopyNO(&this->pointArr[lastChkPoint], tmpPoints + 1, sizeof(Math::Coord2DDbl) * (lastPoints - thisPoints - 1));
				}
				this->nPtOfst -= 1;
				while (++j < i)
				{
					this->ptOfstArr[j] += lastPoints - thisPoints - 1;
				}
				while (j < this->nPtOfst)
				{
					this->ptOfstArr[j] = this->ptOfstArr[j + 1] - 1;
					j++;
				}
				this->nPoint -= 1;
				if (i >= this->nPtOfst)
				{
					thisPoints = (UInt32)this->nPoint;
				}
				else
				{
					thisPoints = this->ptOfstArr[i];
				}
				break;
			}
			else if (this->pointArr[thisChkPoint] == this->pointArr[lastPoints - 1])
			{
				MemCopyNO(tmpPoints, &this->pointArr[thisPoints], sizeof(Math::Coord2DDbl) * (lastPoints - thisPoints));
				if (lastPoints < this->nPoint)
				{
					MemCopyO(&this->pointArr[lastPoints - 1], &this->pointArr[lastPoints], sizeof(Math::Coord2DDbl) * (this->nPoint - lastPoints));
				}
//				MemCopyO(&points[(thisChkPoint + lastPoints - thisPoints - 1) << 1], &points[thisChkPoint << 1], sizeof(Double) * 2 * (thisPoints - thisChkPoint));
//				MemCopyNO(&points[thisChkPoint << 1], tmpPoints, sizeof(Double) * 2 * (lastPoints - thisPoints - 1));
				MemCopyNO(&tmpPoints[(lastPoints - thisPoints)], &this->pointArr[(thisChkPoint + 1)], sizeof(Math::Coord2DDbl) * (thisPoints - thisChkPoint - 1));
				MemCopyNO(&this->pointArr[thisChkPoint], tmpPoints, sizeof(Math::Coord2DDbl) * (lastPoints - thisChkPoint - 1));
				this->nPtOfst -= 1;
				while (++j < i)
				{
					this->ptOfstArr[j] += lastPoints - thisPoints - 1;
				}
				while (j < this->nPtOfst)
				{
					this->ptOfstArr[j] = this->ptOfstArr[j + 1] - 1;
					j++;
				}
				this->nPoint -= 1;
				if (i >= this->nPtOfst)
				{
					thisPoints = (UInt32)this->nPoint;
				}
				else
				{
					thisPoints = this->ptOfstArr[i];
				}
				break;
			}
			else if (this->pointArr[thisChkPoint] == this->pointArr[thisPoints])
			{
				Math::Coord2DDbl *srcPt;
				Math::Coord2DDbl *destPt;
				UInt32 ptCnt;

				MemCopyNO(tmpPoints, &this->pointArr[thisPoints], sizeof(Math::Coord2DDbl) * (lastPoints - thisPoints));
				if (lastPoints < this->nPoint)
				{
					MemCopyO(&this->pointArr[lastPoints - 1], &this->pointArr[lastPoints], sizeof(Math::Coord2DDbl) * (this->nPoint - lastPoints));
				}
				MemCopyO(&this->pointArr[(thisChkPoint + lastPoints - thisPoints - 1)], &this->pointArr[thisChkPoint], sizeof(Math::Coord2DDbl) * (thisPoints - thisChkPoint));

				srcPt = tmpPoints;
				destPt = &this->pointArr[thisChkPoint];
				ptCnt = (lastPoints - thisPoints - 1);
				while (ptCnt-- > 0)
				{
					destPt[0] = srcPt[ptCnt + 1];
					destPt += 1;
				}
				this->nPtOfst -= 1;
				while (++j < i)
				{
					this->ptOfstArr[j] += lastPoints - thisPoints - 1;
				}
				while (j < this->nPtOfst)
				{
					this->ptOfstArr[j] = this->ptOfstArr[j + 1] - 1;
					j++;
				}
				this->nPoint -= 1;
				if (i >= this->nPtOfst)
				{
					thisPoints = (UInt32)this->nPoint;
				}
				else
				{
					thisPoints = this->ptOfstArr[i];
				}
				break;
			}
			else if (this->pointArr[(lastChkPoint - 1)] == this->pointArr[(lastPoints - 1)])
			{
				Math::Coord2DDbl *srcPt;
				Math::Coord2DDbl *destPt;
				UInt32 ptCnt;

				MemCopyNO(tmpPoints, &this->pointArr[thisPoints], sizeof(Math::Coord2DDbl) * (lastPoints - thisPoints));
				if (lastPoints < this->nPoint)
				{
					MemCopyO(&this->pointArr[lastPoints - 1], &this->pointArr[lastPoints], sizeof(Math::Coord2DDbl) * (this->nPoint - lastPoints));
				}
				if (lastChkPoint < thisPoints)
				{
					MemCopyO(&this->pointArr[(lastChkPoint + lastPoints - thisPoints - 1)], &this->pointArr[lastChkPoint], sizeof(Math::Coord2DDbl) * (thisPoints - lastChkPoint));
				}
				srcPt = tmpPoints;
				destPt = &this->pointArr[lastChkPoint];
				ptCnt = (lastPoints - thisPoints - 1);
				while (ptCnt-- > 0)
				{
					destPt[0] = srcPt[ptCnt];
					destPt += 1;
				}
				this->nPtOfst -= 1;
				while (++j < i)
				{
					this->ptOfstArr[j] += lastPoints - thisPoints - 1;
				}
				while (j < this->nPtOfst)
				{
					this->ptOfstArr[j] = this->ptOfstArr[j + 1] - 1;
					j++;
				}
				this->nPoint -= 1;
				if (i >= this->nPtOfst)
				{
					thisPoints = (UInt32)this->nPoint;
				}
				else
				{
					thisPoints = this->ptOfstArr[i];
				}
				break;
			}
			lastChkPoint = thisChkPoint;
		}
		lastPoints = thisPoints;
	}
	MemFreeA(tmpPoints);
}*/

OSInt Math::Geometry::Polyline::GetPointNo(Math::Coord2DDbl pt, OptOut<Bool> isPoint, OptOut<Math::Coord2DDbl> calPtOutPtr, OptOut<Double> calZOutPtr, OptOut<Double> calMOutPtr)
{
	UOSInt k;
	UOSInt l;
	UnsafeArray<Math::Coord2DDbl> points;
	UnsafeArrayOpt<Double> zArr;
	UnsafeArrayOpt<Double> mArr;
	UnsafeArray<Double> nnzArr;
	UnsafeArray<Double> nnmArr;
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

	NN<Math::Geometry::LineString> lineString;
	UOSInt currId = 0;
	Data::ArrayIterator<NN<LineString>> it = this->geometries.Iterator();
	while (it.HasNext())
	{
		lineString = it.Next();
		k = 0;
		zArr = lineString->GetZList(l);
		mArr = lineString->GetMList(l);
		points = lineString->GetPointList(l);
		while (k < l)
		{
			calDiff = pt - points[k];
			calSqDiff = calDiff * calDiff;
			calD = calSqDiff.x + calSqDiff.y;
			if (calD < dist)
			{
				dist = calD;
				calPtOut = points[k];
				calZOut = zArr.SetTo(nnzArr)?nnzArr[k]:0;
				calMOut = mArr.SetTo(nnmArr)?nnmArr[k]:0;
				minId = (OSInt)(currId + k);
				isPointI = true;
			}

			k++;
			if (k < l)
			{
				calDiff = points[k - 1] - points[k];

				if (calDiff.x == 0 && calDiff.y == 0)
				{
					calPt.x = pt.x;
					calPt.y = pt.y;
					calZ = zArr.SetTo(nnzArr)?nnzArr[k - 1]:0;
					calM = mArr.SetTo(nnmArr)?nnmArr[k - 1]:0;
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
						calPt.x += calSqDiff.y * points[k - 1].x;
						calPt.x += (pt.y - points[k - 1].y) * calDiff.x * calDiff.y;
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
						Double ratio = (calPt.x - (points[k - 1].x)) / calDiff.x;
						calPt.y = (ratio * calDiff.y) + points[k - 1].y;
						if (zArr.SetTo(nnzArr))
						{
							calZ = (ratio * (nnzArr[k - 1] - nnzArr[k])) + nnzArr[k - 1];
						}
						else
						{
							calZ = 0;
						}
						if (mArr.SetTo(nnmArr))
						{
							calM = (ratio * (nnmArr[k - 1] - nnmArr[k])) + nnmArr[k - 1];
						}
						else
						{
							calM = 0;
						}
					}
				}

				if (calDiff.x < 0)
				{
					if (points[k - 1].x > calPt.x)
						continue;
					if (points[k].x < calPt.x)
						continue;
				}
				else
				{
					if (points[k - 1].x < calPt.x)
						continue;
					if (points[k].x > calPt.x)
						continue;
				}

				if (calDiff.y < 0)
				{
					if (points[k - 1].y > calPt.y)
						continue;
					if (points[k].y < calPt.y)
						continue;
				}
				else
				{
					if (points[k - 1].y < calPt.y)
						continue;
					if (points[k].y > calPt.y)
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
					minId = (OSInt)(currId + k - 1);
				}
			}
		}
		currId += l;
	}

	isPoint.Set(isPointI);
	calPtOutPtr.Set(calPtOut);
	calZOutPtr.Set(calZOut);
	calMOutPtr.Set(calMOut);
	return minId;
}

/*Math::Geometry::Polygon *Math::Geometry::Polyline::CreatePolygonByDist(Double dist) const
{
	if (this->nPoint < 2)
		return 0;
	if (this->nPtOfst > 1)
		return 0;

	Data::ArrayListDbl *outPoints;
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
	NEW_CLASS(outPoints, Data::ArrayListDbl());

	deg = Math_ArcTan2(this->pointArr[1].x - this->pointArr[0].x, this->pointArr[1].y - this->pointArr[0].y);
	lastPtX = -Math_Cos(deg) * dist + this->pointArr[0].x;
	lastPtY = Math_Sin(deg) * dist + this->pointArr[0].y;

	outPoints->Add(lastPtX);
	outPoints->Add(lastPtY);

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

			outPoints->Add(thisPtX);
			outPoints->Add(thisPtY);

		}
		lastPtX = thisPtX;
		lastPtY = thisPtY;
		i += 1;
	}

	deg = Math_ArcTan2(this->pointArr[this->nPoint - 1].x - this->pointArr[this->nPoint - 2].x, this->pointArr[this->nPoint - 1].y - this->pointArr[this->nPoint - 2].y);
	lastPtX = -Math_Cos(deg) * dist + this->pointArr[this->nPoint - 1].x;
	lastPtY = Math_Sin(deg) * dist + this->pointArr[this->nPoint- 1].y;

	outPoints->Add(lastPtX);
	outPoints->Add(lastPtY);

	lastPtX = Math_Cos(deg) * dist + this->pointArr[this->nPoint - 1].x;
	lastPtY = -Math_Sin(deg) * dist + this->pointArr[this->nPoint - 1].y;

	outPoints->Add(lastPtX);
	outPoints->Add(lastPtY);

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

			outPoints->Add(thisPtX);
			outPoints->Add(thisPtY);
		}

		lastPtX = thisPtX;
		lastPtY = thisPtY;
	}
	deg = Math_ArcTan2(this->pointArr[1].x - this->pointArr[0].x, this->pointArr[1].y - this->pointArr[0].y);

	lastPtX = Math_Cos(deg) * dist + this->pointArr[0].x;
	lastPtY = -Math_Sin(deg) * dist + this->pointArr[0].y;

	outPoints->Add(lastPtX);
	outPoints->Add(lastPtY);

	Math::Geometry::Polygon *pg;
	UOSInt nPoints;
	Math::Coord2DDbl *pts;
	NEW_CLASS(pg, Math::Geometry::Polygon(this->srid, 1, outPoints->GetCount() >> 1, false, false));
	pts = pg->GetPointList(nPoints);
	i = 0;
	while (i < nPoints)
	{
		pts[i].x = outPoints->GetItem((i << 1));
		pts[i].y = outPoints->GetItem((i << 1) + 1);
		i++;
	}
	DEL_CLASS(outPoints);
	return pg;
}*/
