#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/CoordinateSystem.h"
#include "Math/Math.h"
#include "Math/Polyline.h"
#include "Data/ArrayListDbl.h"

Math::Polyline::Polyline(UInt32 srid, Math::Coord2D<Double> *pointArr, UOSInt nPoint) : PointCollection(srid)
{
	this->pointArr = MemAlloc(Math::Coord2D<Double>, nPoint << 1);
	MemCopyNO(this->pointArr, pointArr, sizeof(Math::Coord2D<Double>) * nPoint);
	this->nPoint = nPoint;
	this->nPtOfst = 1;
	this->ptOfstArr = MemAlloc(UInt32, 1);
	this->ptOfstArr[0] = 0;
	this->flags = 0;
	this->color = 0;
}

Math::Polyline::Polyline(UInt32 srid, UOSInt nPtOfst, UOSInt nPoint) : PointCollection(srid)
{
	if (nPtOfst == 0)
	{
		nPtOfst = 1;
	}
	this->pointArr = MemAlloc(Math::Coord2D<Double>, nPoint);
	this->nPoint = nPoint;
	MemClear(this->pointArr, sizeof(Math::Coord2D<Double>) * nPoint);
	this->nPtOfst = nPtOfst;
	this->ptOfstArr = MemAlloc(UInt32, nPtOfst);
	MemClear(this->ptOfstArr, sizeof(UInt32) * nPtOfst);
	this->flags = 0;
	this->color = 0;
}

Math::Polyline::~Polyline()
{
	MemFree(this->pointArr);
	MemFree(this->ptOfstArr);
}

Math::Vector2D::VectorType Math::Polyline::GetVectorType()
{
	return Math::Vector2D::VectorType::Polyline;
}

UInt32 *Math::Polyline::GetPtOfstList(UOSInt *nPtOfst)
{
	*nPtOfst = this->nPtOfst;
	return this->ptOfstArr;
}

Math::Coord2D<Double> *Math::Polyline::GetPointList(UOSInt *nPoint)
{
	*nPoint = this->nPoint;
	return this->pointArr;
}

Math::Coord2D<Double> Math::Polyline::GetCenter()
{
	Double maxLength = 0;
	UOSInt maxId = 0;
	Double currLength;
	UOSInt i = this->nPoint - 1;
	UOSInt j = this->nPtOfst;
	UOSInt k;
	Math::Coord2D<Double> lastPt;
	Math::Coord2D<Double> thisPt;
	while (j-- > 0)
	{
		lastPt = this->pointArr[i];
		currLength = 0;
		k = this->ptOfstArr[j];
		while (i-- > k)
		{
			thisPt = this->pointArr[i];
			currLength += Math_Sqrt((thisPt.x - lastPt.x) * (thisPt.x - lastPt.x) + (thisPt.y - lastPt.y) * (thisPt.y - lastPt.y));
			lastPt = thisPt;
		}
		if (currLength > maxLength)
		{
			maxLength = currLength;
			maxId = j;
		}
	}

	if (maxLength == 0)
	{
		return this->pointArr[0];
	}
	i = this->ptOfstArr[maxId];
	if ((UOSInt)maxId >= this->nPtOfst - 1)
	{
		j = this->nPoint;
	}
	else
	{
		j = this->ptOfstArr[maxId + 1];
	}
	maxLength = maxLength * 0.5;
	lastPt = this->pointArr[i];
	while (i < j)
	{
		i++;
		thisPt = this->pointArr[i];
		currLength = Math_Sqrt((thisPt.x - lastPt.x) * (thisPt.x - lastPt.x) + (thisPt.y - lastPt.y) * (thisPt.y - lastPt.y));
		if (currLength >= maxLength)
		{
			return Math::Coord2D<Double>(lastPt.x + (thisPt.x - lastPt.x) * maxLength / currLength,
				lastPt.y + (thisPt.y - lastPt.y) * maxLength / currLength);
		}
		else
		{
			maxLength -= currLength;
		}
		lastPt = thisPt;
	}
	return this->pointArr[0];
}

Math::Vector2D *Math::Polyline::Clone()
{
	Math::Polyline *pl;
	NEW_CLASS(pl, Math::Polyline(this->srid, this->nPtOfst, this->nPoint));
	MemCopyNO(pl->ptOfstArr, this->ptOfstArr, sizeof(Int32) * this->nPtOfst);
	MemCopyNO(pl->pointArr, this->pointArr, sizeof(Double) * (this->nPoint << 1));
	pl->flags = this->flags;
	pl->color = this->color;
	return pl;
}

void Math::Polyline::GetBounds(Double *minX, Double *minY, Double *maxX, Double *maxY)
{
	UOSInt i = this->nPoint;
	Double x1;
	Double y1;
	Double x2;
	Double y2;
	x1 = x2 = this->pointArr[0].x;
	y1 = y2 = this->pointArr[0].y;
	while (i > 1)
	{
		i -= 1;
		if (x1 > this->pointArr[i].x)
		{
			x1 = this->pointArr[i].x;
		}
		if (x2 < this->pointArr[i].x)
		{
			x2 = this->pointArr[i].x;
		}
		if (y1 > this->pointArr[i].y)
		{
			y1 = this->pointArr[i].y;
		}
		if (y2 < this->pointArr[i].y)
		{
			y2 = this->pointArr[i].y;
		}
	}
	*minX = x1;
	*minY = y1;
	*maxX = x2;
	*maxY = y2;
}

Double Math::Polyline::CalSqrDistance(Double x, Double y, Double *nearPtX, Double *nearPtY)
{
	UOSInt k;
	UOSInt l;
	UInt32 m;
	UInt32 *ptOfsts;
	Math::Coord2D<Double> *points;

	ptOfsts = this->ptOfstArr;
	points = this->pointArr;

	k = this->nPtOfst;
	l = this->nPoint;

	Double calBase;
	Double calH;
	Double calW;
	Double calX;
	Double calY;
	Double calD;
	Double dist = 0x7fffffff;
	Double calPtX = 0;
	Double calPtY = 0;

	while (k--)
	{
		m = ptOfsts[k];
		l--;
		while (l-- > m)
		{
			calH = points[l].y - points[l + 1].y;
			calW = points[l].x - points[l + 1].x;

			if (calH == 0)
			{
				calX = x;
			}
			else
			{
				calX = (calBase = (calW * calW)) * x;
				calBase += calH * calH;
				calX += calH * calH * points[l].x;
				calX += (y - points[l].y) * calH * calW;
				calX /= calBase;
			}

			if (calW == 0)
			{
				calY = y;
			}
			else
			{
				calY = ((calX - points[l].x) * calH / calW) + points[l].y;
			}

			if (calW < 0)
			{
				if (points[l].x > calX)
					continue;
				if (points[l + 1].x < calX)
					continue;
			}
			else
			{
				if (points[l].x < calX)
					continue;
				if (points[l + 1].x > calX)
					continue;
			}

			if (calH < 0)
			{
				if (points[l].y > calY)
					continue;
				if (points[l + 1].y < calY)
					continue;
			}
			else
			{
				if (points[l].y < calY)
					continue;
				if (points[l + 1].y > calY)
					continue;
			}

			calH = y - calY;
			calW = x - calX;
			calD = calW * calW + calH * calH;
			if (calD < dist)
			{
				dist = calD;
				calPtX = calX;
				calPtY = calY;
			}
		}
	}
	k = this->nPoint;
	while (k-- > 0)
	{
		calH = y - points[k].y;
		calW = x - points[k].x;
		calD = calW * calW + calH * calH;
		if (calD < dist)
		{
			dist = calD;
			calPtX = points[k].x;
			calPtY = points[k].y;
		}
	}
	if (nearPtX && nearPtY)
	{
		*nearPtX = calPtX;
		*nearPtY = calPtY;
	}
	return dist;
}

Bool Math::Polyline::JoinVector(Math::Vector2D *vec)
{
	if (vec->GetVectorType() != Math::Vector2D::VectorType::Polyline)
	{
		return false;
	}
	Math::Polyline *pl = (Math::Polyline*)vec;
	UInt32 *newPtOfsts = MemAlloc(UInt32, this->nPtOfst + pl->nPtOfst);
	Math::Coord2D<Double> *newPoints = MemAlloc(Math::Coord2D<Double>, (this->nPoint + pl->nPoint));
	MemCopyNO(newPtOfsts, this->ptOfstArr, this->nPtOfst * sizeof(UInt32));
	MemCopyNO(newPoints, this->pointArr, this->nPoint * sizeof(Math::Coord2D<Double>));
	MemCopyNO(&newPoints[this->nPoint], pl->pointArr, pl->nPoint * sizeof(Math::Coord2D<Double>));
	UOSInt i = pl->nPtOfst;
	while (i-- > 0)
	{
		newPtOfsts[this->nPtOfst + i] = pl->ptOfstArr[i] + (UInt32)this->nPoint;
	}
	MemFree(this->ptOfstArr);
	MemFree(this->pointArr);
	this->ptOfstArr = newPtOfsts;
	this->pointArr = newPoints;
	this->nPtOfst += pl->nPtOfst;
	this->nPoint += pl->nPoint;
	//////////////////////////////////
	this->OptimizePolyline();
	return true;
}

void Math::Polyline::ConvCSys(Math::CoordinateSystem *srcCSys, Math::CoordinateSystem *destCSys)
{
	UOSInt i = this->nPoint;
	while (i-- > 0)
	{
		Math::CoordinateSystem::ConvertXYZ(srcCSys, destCSys, this->pointArr[i].x, this->pointArr[i].y, 0, &this->pointArr[i].x, &this->pointArr[i].y, 0);
	}
}

Bool Math::Polyline::Equals(Math::Vector2D *vec)
{
	if (vec == 0)
		return false;
	if (vec->GetSRID() != this->srid)
	{
		return false;
	}
	if (vec->GetVectorType() == VectorType::Polyline && !vec->Support3D())
	{
		Math::Polyline *pl = (Math::Polyline*)vec;
		UOSInt nPtOfst;
		UOSInt nPoint;
		UInt32 *ptOfst = pl->GetPtOfstList(&nPtOfst);
		Math::Coord2D<Double> *ptList = pl->GetPointList(&nPoint);
		if (nPtOfst != this->nPtOfst || nPoint != this->nPoint)
		{
			return false;
		}
		UOSInt i = nPtOfst;
		while (i-- > 0)
		{
			if (ptOfst[i] != this->ptOfstArr[i])
			{
				return false;
			}
		}
		i = nPoint << 1;
		while (i-- > 0)
		{
			if (ptList[i] != this->pointArr[i])
			{
				return false;
			}
		}
		return true;
	}
	else
	{
		return false;
	}
}

Math::Polyline *Math::Polyline::SplitByPoint(Double x, Double y)
{
	UOSInt k;
	UOSInt l;
	UInt32 *ptOfsts;

	ptOfsts = this->ptOfstArr;

	k = this->nPtOfst;
	l = this->nPoint;

	Double calPtX;
	Double calPtY;
	Bool isPoint;
	UOSInt minId = (UOSInt)this->GetPointNo(x, y, &isPoint, &calPtX, &calPtY);

	UInt32 *oldPtOfsts;
	UInt32 *newPtOfsts;
	Math::Coord2D<Double> *oldPoints;
	Math::Coord2D<Double> *newPoints;
	Math::Polyline *newPL;
	if (isPoint)
	{
		if (minId == this->nPoint - 1 || minId == 0 || minId == (UOSInt)-1)
		{
			return 0;
		}
		k = this->nPtOfst;
		while (k-- > 1)
		{
			if (this->ptOfstArr[k] == minId || (this->ptOfstArr[k] - 1) == minId)
			{
				return 0;
			}
		}
		
		oldPtOfsts = this->ptOfstArr;
		oldPoints = this->pointArr;

		k = this->nPtOfst;
		while (k-- > 0)
		{
			if (oldPtOfsts[k] < minId)
			{
				break;
			}
		}
		newPtOfsts = MemAlloc(UInt32, k + 1);
		newPoints = MemAlloc(Math::Coord2D<Double>, (minId + 1));
		l = minId + 1;
		while (l-- > 0)
		{
			newPoints[l] = oldPoints[l];
		}
		l = k + 1;
		while (l-- > 0)
		{
			newPtOfsts[l] = oldPtOfsts[l];
		}

		this->ptOfstArr = newPtOfsts;
		this->pointArr = newPoints;
		NEW_CLASS(newPL, Math::Polyline(this->srid, this->nPtOfst - k, this->nPoint - minId));
		newPtOfsts = newPL->GetPtOfstList(&l);
		l = this->nPtOfst;
		while (--l > k)
		{
			newPtOfsts[l - k] = ptOfsts[l] - (UInt32)minId;
		}
		newPtOfsts[0] = 0;
		newPoints = newPL->GetPointList(&l);
		l = this->nPoint;
		while (l-- > minId)
		{
			newPoints[l - minId] = oldPoints[l];
		}
		this->nPoint = minId + 1;
		this->nPtOfst = k + 1;
		MemFree(oldPoints);
		MemFree(oldPtOfsts);

		return newPL;
	}
	else
	{
		oldPtOfsts = this->ptOfstArr;
		oldPoints = this->pointArr;

		k = this->nPtOfst;
		while (k-- > 0)
		{
			if (oldPtOfsts[k] <= minId)
			{
				break;
			}
		}
		newPtOfsts = MemAlloc(UInt32, k + 1);
		newPoints = MemAlloc(Math::Coord2D<Double>, (minId + 2));
		l = minId + 1;
		while (l-- > 0)
		{
			newPoints[l] = oldPoints[l];
		}
		newPoints[minId + 1].x = calPtX;
		newPoints[minId + 1].y = calPtY;

		l = k + 1;
		while (l-- > 0)
		{
			newPtOfsts[l] = oldPtOfsts[l];
		}

		this->ptOfstArr = newPtOfsts;
		this->pointArr = newPoints;
		NEW_CLASS(newPL, Math::Polyline(this->srid, this->nPtOfst - k, this->nPoint - minId));
		newPtOfsts = newPL->GetPtOfstList(&l);
		l = this->nPtOfst;
		while (--l > k)
		{
			newPtOfsts[l - k] = ptOfsts[l] - (UInt32)minId;
		}
		newPtOfsts[0] = 0;
		newPoints = newPL->GetPointList(&l);
		l = this->nPoint;
		while (--l > minId)
		{
			newPoints[l - minId] = oldPoints[l];
		}
		newPoints[0].x = calPtX;
		newPoints[0].y = calPtY;

		this->nPoint = minId + 2;
		this->nPtOfst = k + 1;
		MemFree(oldPoints);
		MemFree(oldPtOfsts);

		return newPL;
	}
}

void Math::Polyline::OptimizePolyline()
{
	Math::Coord2D<Double> *tmpPoints = MemAlloc(Math::Coord2D<Double>, this->nPoint);
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
				MemCopyNO(tmpPoints, &this->pointArr[thisPoints], sizeof(Math::Coord2D<Double>) * (lastPoints - thisPoints));
				if (lastPoints < this->nPoint)
				{
					MemCopyO(&this->pointArr[lastPoints - 1], &this->pointArr[lastPoints], sizeof(Math::Coord2D<Double>) * (this->nPoint - lastPoints));
				}
				if (lastChkPoint < thisPoints)
				{
					MemCopyNO(&tmpPoints[(lastPoints - thisPoints)], &this->pointArr[lastChkPoint], sizeof(Math::Coord2D<Double>) * (thisPoints - lastChkPoint));
					MemCopyNO(&this->pointArr[lastChkPoint], tmpPoints + 1, sizeof(Math::Coord2D<Double>) * (lastPoints - lastChkPoint - 1));
				}
				else
				{
					MemCopyNO(&this->pointArr[lastChkPoint], tmpPoints + 1, sizeof(Math::Coord2D<Double>) * (lastPoints - thisPoints - 1));
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
				MemCopyNO(tmpPoints, &this->pointArr[thisPoints], sizeof(Math::Coord2D<Double>) * (lastPoints - thisPoints));
				if (lastPoints < this->nPoint)
				{
					MemCopyO(&this->pointArr[lastPoints - 1], &this->pointArr[lastPoints], sizeof(Math::Coord2D<Double>) * (this->nPoint - lastPoints));
				}
//				MemCopyO(&points[(thisChkPoint + lastPoints - thisPoints - 1) << 1], &points[thisChkPoint << 1], sizeof(Double) * 2 * (thisPoints - thisChkPoint));
//				MemCopyNO(&points[thisChkPoint << 1], tmpPoints, sizeof(Double) * 2 * (lastPoints - thisPoints - 1));
				MemCopyNO(&tmpPoints[(lastPoints - thisPoints)], &this->pointArr[(thisChkPoint + 1)], sizeof(Math::Coord2D<Double>) * (thisPoints - thisChkPoint - 1));
				MemCopyNO(&this->pointArr[thisChkPoint], tmpPoints, sizeof(Math::Coord2D<Double>) * (lastPoints - thisChkPoint - 1));
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
				Math::Coord2D<Double> *srcPt;
				Math::Coord2D<Double> *destPt;
				UInt32 ptCnt;

				MemCopyNO(tmpPoints, &this->pointArr[thisPoints], sizeof(Math::Coord2D<Double>) * (lastPoints - thisPoints));
				if (lastPoints < this->nPoint)
				{
					MemCopyO(&this->pointArr[lastPoints - 1], &this->pointArr[lastPoints], sizeof(Math::Coord2D<Double>) * (this->nPoint - lastPoints));
				}
				MemCopyO(&this->pointArr[(thisChkPoint + lastPoints - thisPoints - 1)], &this->pointArr[thisChkPoint], sizeof(Math::Coord2D<Double>) * (thisPoints - thisChkPoint));

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
				Math::Coord2D<Double> *srcPt;
				Math::Coord2D<Double> *destPt;
				UInt32 ptCnt;

				MemCopyNO(tmpPoints, &this->pointArr[thisPoints], sizeof(Math::Coord2D<Double>) * (lastPoints - thisPoints));
				if (lastPoints < this->nPoint)
				{
					MemCopyO(&this->pointArr[lastPoints - 1], &this->pointArr[lastPoints], sizeof(Math::Coord2D<Double>) * (this->nPoint - lastPoints));
				}
				if (lastChkPoint < thisPoints)
				{
					MemCopyO(&this->pointArr[(lastChkPoint + lastPoints - thisPoints - 1)], &this->pointArr[lastChkPoint], sizeof(Math::Coord2D<Double>) * (thisPoints - lastChkPoint));
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
	MemFree(tmpPoints);
}

OSInt Math::Polyline::GetPointNo(Double x, Double y, Bool *isPoint, Double *calPtXOut, Double *calPtYOut)
{
	UOSInt k;
	UOSInt l;
	UInt32 m;
	UInt32 *ptOfsts;
	Math::Coord2D<Double> *points;

	ptOfsts = this->ptOfstArr;
	points = this->pointArr;

	k = this->nPtOfst;
	l = this->nPoint;

	Double calBase;
	Double calH;
	Double calW;
	Double calX;
	Double calY;
	Double calD;
	Double dist = 0x7fffffff;
	Double calPtX = 0;
	Double calPtY = 0;
	OSInt minId = -1;
	Bool isPointI = false;

	while (k--)
	{
		m = ptOfsts[k];
		l--;
		while (l-- > m)
		{
			calH = points[l].y - points[l + 1].y;
			calW = points[l].x - points[l + 1].x;

			if (calH == 0)
			{
				calX = x;
			}
			else
			{
				calX = (calBase = (calW * calW)) * x;
				calBase += calH * calH;
				calX += calH * calH * (points[l].x);
				calX += (y - points[l].y) * calH * calW;
				calX /= calBase;
			}

			if (calW == 0)
			{
				calY = y;
			}
			else
			{
				calY = ((calX - (points[l].x)) * calH / calW) + points[l].y;
			}

			if (calW < 0)
			{
				if (points[l].x > calX)
					continue;
				if (points[l + 1].x < calX)
					continue;
			}
			else
			{
				if (points[l].x < calX)
					continue;
				if (points[l + 1].x > calX)
					continue;
			}

			if (calH < 0)
			{
				if (points[l].y > calY)
					continue;
				if (points[l + 1].y < calY)
					continue;
			}
			else
			{
				if (points[l].y < calY)
					continue;
				if (points[l + 1].y > calY)
					continue;
			}

			calH = y - calY;
			calW = x - calX;
			calD = calW * calW + calH * calH;
			if (calD < dist)
			{
				dist = calD;
				calPtX = calX;
				calPtY = calY;
				isPointI = false;
				minId = (OSInt)l;
			}
		}
	}
	k = this->nPoint;
	while (k-- > 0)
	{
		calH = y - points[k].y;
		calW = x - points[k].x;
		calD = calW * calW + calH * calH;
		if (calD < dist)
		{
			dist = calD;
			calPtX = points[k].x;
			calPtY = points[k].y;
			minId = (OSInt)k;
			isPointI = true;
		}
	}

	if (isPoint)
	{
		*isPoint = isPointI;
	}
	if (calPtXOut)
	{
		*calPtXOut = calPtX;
	}
	if (calPtYOut)
	{
		*calPtYOut = calPtY;
	}
	return minId;
}

Math::Polygon *Math::Polyline::CreatePolygonByDist(Double dist)
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

	Math::Polygon *pg;
	UOSInt nPoints;
	Math::Coord2D<Double> *pts;
	NEW_CLASS(pg, Math::Polygon(this->srid, 1, outPoints->GetCount() >> 1));
	pts = pg->GetPointList(&nPoints);
	i = 0;
	while (i < nPoints)
	{
		pts[i].x = outPoints->GetItem((i << 1));
		pts[i].y = outPoints->GetItem((i << 1) + 1);
		i++;
	}
	DEL_CLASS(outPoints);
	return pg;
}

Bool Math::Polyline::HasColor()
{
	return (this->flags & 1) != 0;
}

UInt32 Math::Polyline::GetColor()
{
	return this->color;
}
void Math::Polyline::SetColor(UInt32 color)
{
	this->color = color;
	this->flags |= 1;
}
