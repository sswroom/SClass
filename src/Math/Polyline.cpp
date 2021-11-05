#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/CoordinateSystem.h"
#include "Math/Math.h"
#include "Math/Polyline.h"
#include "Data/ArrayListDbl.h"

Math::Polyline::Polyline(UInt32 srid, Double *pointArr, UOSInt nPoint) : PointCollection(srid)
{
	this->pointArr = MemAlloc(Double, nPoint << 1);
	MemCopyNO(this->pointArr, pointArr, sizeof(Double) * (nPoint << 1));
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
	this->pointArr = MemAlloc(Double, nPoint << 1);
	this->nPoint = nPoint;
	MemClear(this->pointArr, sizeof(Double) * (nPoint << 1));
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

Double *Math::Polyline::GetPointList(UOSInt *nPoint)
{
	*nPoint = this->nPoint;
	return this->pointArr;
}

void Math::Polyline::GetCenter(Double *x, Double *y)
{
	Double maxLength = 0;
	UOSInt maxId = 0;
	Double currLength;
	UOSInt i = this->nPoint - 1;
	UOSInt j = this->nPtOfst;
	UOSInt k;
	Double lastX;
	Double lastY;
	Double thisX;
	Double thisY;
	while (j-- > 0)
	{
		lastX = this->pointArr[(i << 1)];
		lastY = this->pointArr[(i << 1) + 1];
		currLength = 0;
		k = this->ptOfstArr[j];
		while (i-- > k)
		{
			thisX = this->pointArr[(i << 1)];
			thisY = this->pointArr[(i << 1) + 1];
			currLength += Math::Sqrt((thisX - lastX) * (thisX - lastX) + (thisY - lastY) * (thisY - lastY));
			lastX = thisX;
			lastY = thisY;
		}
		if (currLength > maxLength)
		{
			maxLength = currLength;
			maxId = j;
		}
	}

	if (maxLength == 0)
	{
		*x = this->pointArr[0];
		*y = this->pointArr[1];
		return;
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
	lastX = this->pointArr[i << 1];
	lastY = this->pointArr[(i << 1) + 1];
	while (i < j)
	{
		i++;
		thisX = this->pointArr[(i << 1)];
		thisY = this->pointArr[(i << 1) + 1];
		currLength = Math::Sqrt((thisX - lastX) * (thisX - lastX) + (thisY - lastY) * (thisY - lastY));
		if (currLength >= maxLength)
		{
			*x = lastX + (thisX - lastX) * maxLength / currLength;
			*y = lastY + (thisY - lastY) * maxLength / currLength;
			return;
		}
		else
		{
			maxLength -= currLength;
		}
		lastX = thisX;
		lastY = thisY;
	}
	*x = this->pointArr[0];
	*y = this->pointArr[1];
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
	UOSInt i = this->nPoint << 1;
	Double x1;
	Double y1;
	Double x2;
	Double y2;
	x1 = x2 = this->pointArr[0];
	y1 = y2 = this->pointArr[1];
	while (i > 2)
	{
		i -= 2;
		if (x1 > this->pointArr[i])
		{
			x1 = this->pointArr[i];
		}
		if (x2 < this->pointArr[i])
		{
			x2 = this->pointArr[i];
		}
		if (y1 > this->pointArr[i + 1])
		{
			y1 = this->pointArr[i + 1];
		}
		if (y2 < this->pointArr[i + 1])
		{
			y2 = this->pointArr[i + 1];
		}
		i -= 2;
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
	Double *points;

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
			calH = points[(l << 1) + 1] - points[(l << 1) + 3];
			calW = points[(l << 1) + 0] - points[(l << 1) + 2];

			if (calH == 0)
			{
				calX = x;
			}
			else
			{
				calX = (calBase = (calW * calW)) * x;
				calBase += calH * calH;
				calX += calH * calH * (points[(l << 1) + 0]);
				calX += (y - points[(l << 1) + 1]) * calH * calW;
				calX /= calBase;
			}

			if (calW == 0)
			{
				calY = y;
			}
			else
			{
				calY = ((calX - (points[(l << 1) + 0])) * calH / calW) + points[(l << 1) + 1];
			}

			if (calW < 0)
			{
				if (points[(l << 1) + 0] > calX)
					continue;
				if (points[(l << 1) + 2] < calX)
					continue;
			}
			else
			{
				if (points[(l << 1) + 0] < calX)
					continue;
				if (points[(l << 1) + 2] > calX)
					continue;
			}

			if (calH < 0)
			{
				if (points[(l << 1) + 1] > calY)
					continue;
				if (points[(l << 1) + 3] < calY)
					continue;
			}
			else
			{
				if (points[(l << 1) + 1] < calY)
					continue;
				if (points[(l << 1) + 3] > calY)
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
		calH = y - points[(k << 1) + 1];
		calW = x - points[(k << 1) + 0];
		calD = calW * calW + calH * calH;
		if (calD < dist)
		{
			dist = calD;
			calPtX = points[(k << 1) + 0];
			calPtY = points[(k << 1) + 1];
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
	Double *newPoints = MemAlloc(Double, (this->nPoint + pl->nPoint) * 2);
	MemCopyNO(newPtOfsts, this->ptOfstArr, this->nPtOfst * sizeof(UInt32));
	MemCopyNO(newPoints, this->pointArr, this->nPoint * 2 * sizeof(Double));
	MemCopyNO(&newPoints[this->nPoint * 2], pl->pointArr, pl->nPoint * 2 * sizeof(Double));
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
		Math::CoordinateSystem::ConvertXYZ(srcCSys, destCSys, this->pointArr[(i << 1)], this->pointArr[(i << 1) + 1], 0, &this->pointArr[(i << 1)], &this->pointArr[(i << 1) + 1], 0);
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
	Double *oldPoints;
	Double *newPoints;
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
		newPoints = MemAlloc(Double, (minId + 1) * 2);
		l = minId + 1;
		while (l-- > 0)
		{
			newPoints[(l << 1) + 0] = oldPoints[(l << 1) + 0];
			newPoints[(l << 1) + 1] = oldPoints[(l << 1) + 1];
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
			newPoints[((l - minId) << 1) + 0] = oldPoints[(l << 1) + 0];
			newPoints[((l - minId) << 1) + 1] = oldPoints[(l << 1) + 1];
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
		newPoints = MemAlloc(Double, (minId + 2) * 2);
		l = minId + 1;
		while (l-- > 0)
		{
			newPoints[(l << 1) + 0] = oldPoints[(l << 1) + 0];
			newPoints[(l << 1) + 1] = oldPoints[(l << 1) + 1];
		}
		newPoints[((minId + 1) << 1) + 0] = calPtX;
		newPoints[((minId + 1) << 1) + 1] = calPtY;

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
			newPoints[((l - minId) << 1) + 0] = oldPoints[(l << 1) + 0];
			newPoints[((l - minId) << 1) + 1] = oldPoints[(l << 1) + 1];
		}
		newPoints[0] = calPtX;
		newPoints[1] = calPtY;

		this->nPoint = minId + 2;
		this->nPtOfst = k + 1;
		MemFree(oldPoints);
		MemFree(oldPtOfsts);

		return newPL;
	}
}

void Math::Polyline::OptimizePolyline()
{
	Double *tmpPoints = MemAlloc(Double, this->nPoint * 2);
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
			if (this->pointArr[((lastChkPoint - 1) << 1)] == this->pointArr[(thisPoints << 1)] && this->pointArr[((lastChkPoint - 1) << 1) + 1] == this->pointArr[(thisPoints << 1) + 1])
			{
				MemCopyNO(tmpPoints, &this->pointArr[thisPoints << 1], sizeof(Double) * 2 * (lastPoints - thisPoints));
				if (lastPoints < this->nPoint)
				{
					MemCopyO(&this->pointArr[(lastPoints << 1) - 2], &this->pointArr[(lastPoints << 1)], sizeof(Double) * 2 * (this->nPoint - lastPoints));
				}
				if (lastChkPoint < thisPoints)
				{
					MemCopyNO(&tmpPoints[2 * (lastPoints - thisPoints)], &this->pointArr[lastChkPoint << 1], sizeof(Double) * 2 * (thisPoints - lastChkPoint));
					MemCopyNO(&this->pointArr[lastChkPoint << 1], tmpPoints + 2, sizeof(Double) * 2 * (lastPoints - lastChkPoint - 1));
				}
				else
				{
					MemCopyNO(&this->pointArr[lastChkPoint << 1], tmpPoints + 2, sizeof(Double) * 2 * (lastPoints - thisPoints - 1));
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
			else if (this->pointArr[(thisChkPoint << 1)] == this->pointArr[((lastPoints - 1) << 1)] && this->pointArr[(thisChkPoint << 1) + 1] == this->pointArr[((lastPoints - 1) << 1) + 1])
			{
				MemCopyNO(tmpPoints, &this->pointArr[thisPoints << 1], sizeof(Double) * 2 * (lastPoints - thisPoints));
				if (lastPoints < this->nPoint)
				{
					MemCopyO(&this->pointArr[(lastPoints << 1) - 2], &this->pointArr[(lastPoints << 1)], sizeof(Double) * 2 * (this->nPoint - lastPoints));
				}
//				MemCopyO(&points[(thisChkPoint + lastPoints - thisPoints - 1) << 1], &points[thisChkPoint << 1], sizeof(Double) * 2 * (thisPoints - thisChkPoint));
//				MemCopyNO(&points[thisChkPoint << 1], tmpPoints, sizeof(Double) * 2 * (lastPoints - thisPoints - 1));
				MemCopyNO(&tmpPoints[(lastPoints - thisPoints) << 1], &this->pointArr[(thisChkPoint + 1) << 1], sizeof(Double) * 2 * (thisPoints - thisChkPoint - 1));
				MemCopyNO(&this->pointArr[thisChkPoint << 1], tmpPoints, sizeof(Double) * 2 * (lastPoints - thisChkPoint - 1));
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
			else if (this->pointArr[(thisChkPoint << 1)] == this->pointArr[(thisPoints << 1)] && this->pointArr[(thisChkPoint << 1) + 1] == this->pointArr[(thisPoints << 1) + 1])
			{
				Double *srcPt;
				Double *destPt;
				UInt32 ptCnt;

				MemCopyNO(tmpPoints, &this->pointArr[thisPoints << 1], sizeof(Double) * 2 * (lastPoints - thisPoints));
				if (lastPoints < this->nPoint)
				{
					MemCopyO(&this->pointArr[(lastPoints << 1) - 2], &this->pointArr[(lastPoints << 1)], sizeof(Double) * 2 * (this->nPoint - lastPoints));
				}
				MemCopyO(&this->pointArr[(thisChkPoint + lastPoints - thisPoints - 1) << 1], &this->pointArr[thisChkPoint << 1], sizeof(Double) * 2 * (thisPoints - thisChkPoint));

				srcPt = tmpPoints;
				destPt = &this->pointArr[thisChkPoint << 1];
				ptCnt = (lastPoints - thisPoints - 1);
				while (ptCnt-- > 0)
				{
					destPt[0] = srcPt[(ptCnt << 1) + 2];
					destPt[1] = srcPt[(ptCnt << 1) + 3];
					destPt += 2;
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
			else if (this->pointArr[((lastChkPoint - 1) << 1)] == this->pointArr[((lastPoints - 1) << 1)] && this->pointArr[((lastChkPoint - 1) << 1) + 1] == this->pointArr[((lastPoints - 1) << 1) + 1])
			{
				Double *srcPt;
				Double *destPt;
				UInt32 ptCnt;

				MemCopyNO(tmpPoints, &this->pointArr[thisPoints << 1], sizeof(Double) * 2 * (lastPoints - thisPoints));
				if (lastPoints < this->nPoint)
				{
					MemCopyO(&this->pointArr[(lastPoints << 1) - 2], &this->pointArr[(lastPoints << 1)], sizeof(Double) * 2 * (this->nPoint - lastPoints));
				}
				if (lastChkPoint < thisPoints)
				{
					MemCopyO(&this->pointArr[(lastChkPoint + lastPoints - thisPoints - 1) << 1], &this->pointArr[lastChkPoint << 1], sizeof(Double) * 2 * (thisPoints - lastChkPoint));
				}
				srcPt = tmpPoints;
				destPt = &this->pointArr[lastChkPoint << 1];
				ptCnt = (lastPoints - thisPoints - 1);
				while (ptCnt-- > 0)
				{
					destPt[0] = srcPt[(ptCnt << 1)];
					destPt[1] = srcPt[(ptCnt << 1) + 1];
					destPt += 2;
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
	Double *points;

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
			calH = points[(l << 1) + 1] - points[(l << 1) + 3];
			calW = points[(l << 1) + 0] - points[(l << 1) + 2];

			if (calH == 0)
			{
				calX = x;
			}
			else
			{
				calX = (calBase = (calW * calW)) * x;
				calBase += calH * calH;
				calX += calH * calH * (points[(l << 1) + 0]);
				calX += (y - points[(l << 1) + 1]) * calH * calW;
				calX /= calBase;
			}

			if (calW == 0)
			{
				calY = y;
			}
			else
			{
				calY = ((calX - (points[(l << 1) + 0])) * calH / calW) + points[(l << 1) + 1];
			}

			if (calW < 0)
			{
				if (points[(l << 1) + 0] > calX)
					continue;
				if (points[(l << 1) + 2] < calX)
					continue;
			}
			else
			{
				if (points[(l << 1) + 0] < calX)
					continue;
				if (points[(l << 1) + 2] > calX)
					continue;
			}

			if (calH < 0)
			{
				if (points[(l << 1) + 1] > calY)
					continue;
				if (points[(l << 1) + 3] < calY)
					continue;
			}
			else
			{
				if (points[(l << 1) + 1] < calY)
					continue;
				if (points[(l << 1) + 3] > calY)
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
		calH = y - points[(k << 1) + 1];
		calW = x - points[(k << 1) + 0];
		calD = calW * calW + calH * calH;
		if (calD < dist)
		{
			dist = calD;
			calPtX = points[(k << 1) + 0];
			calPtY = points[(k << 1) + 1];
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

	deg = Math::ArcTan2(this->pointArr[2] - this->pointArr[0], this->pointArr[3] - this->pointArr[1]);
	lastPtX = -Math::Cos(deg) * dist + this->pointArr[0];
	lastPtY = Math::Sin(deg) * dist + this->pointArr[1];

	outPoints->Add(lastPtX);
	outPoints->Add(lastPtY);

	i = 2;
	while (i < this->nPoint)
	{
		deg = Math::ArcTan2(this->pointArr[(i << 1) + 0] - this->pointArr[(i << 1) - 2], this->pointArr[(i << 1) + 1] - this->pointArr[(i << 1) - 1]);
		nextPtX = -Math::Cos(deg) * dist + this->pointArr[(i << 1) - 2];
		nextPtY = Math::Sin(deg) * dist + this->pointArr[(i << 1) - 1];

		t1 = (this->pointArr[(i << 1) - 3] - this->pointArr[(i << 1) - 1]) / (this->pointArr[(i << 1) - 4] - this->pointArr[(i << 1) - 2]);
		t2 = (this->pointArr[(i << 1) - 1] - this->pointArr[(i << 1) + 1]) / (this->pointArr[(i << 1) - 2] - this->pointArr[(i << 1) + 0]);
		if (t1 != t2)
		{
			Double x1 = this->pointArr[(i << 1) - 4];
			Double x2 = this->pointArr[(i << 1) - 2];
			Double x3 = this->pointArr[(i << 1) + 0];
			Double x4 = lastPtX;
			Double x6 = nextPtX;
			Double y1 = this->pointArr[(i << 1) - 3];
			Double y2 = this->pointArr[(i << 1) - 1];
			Double y3 = this->pointArr[(i << 1) + 1];
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

	deg = Math::ArcTan2(this->pointArr[(this->nPoint << 1) - 2] - this->pointArr[(this->nPoint << 1) - 4], this->pointArr[(this->nPoint << 1) - 1] - this->pointArr[(this->nPoint << 1) - 3]);
	lastPtX = -Math::Cos(deg) * dist + this->pointArr[(this->nPoint << 1) - 2];
	lastPtY = Math::Sin(deg) * dist + this->pointArr[(this->nPoint << 1) - 1];

	outPoints->Add(lastPtX);
	outPoints->Add(lastPtY);

	lastPtX = Math::Cos(deg) * dist + this->pointArr[(this->nPoint << 1) - 2];
	lastPtY = -Math::Sin(deg) * dist + this->pointArr[(this->nPoint << 1) - 1];

	outPoints->Add(lastPtX);
	outPoints->Add(lastPtY);

	i = this->nPoint;
	while (i > 2)
	{
		i -= 1;
		deg = Math::ArcTan2(this->pointArr[(i << 1) - 4] - this->pointArr[(i << 1) - 2], this->pointArr[(i << 1) - 3] - this->pointArr[(i << 1) - 1]);
		nextPtX = -Math::Cos(deg) * dist + this->pointArr[(i << 1) - 4];
		nextPtY = Math::Sin(deg) * dist + this->pointArr[(i << 1) - 3];

		t2 = (this->pointArr[(i << 1) - 3] - this->pointArr[(i << 1) - 1]) / (this->pointArr[(i << 1) - 4] - this->pointArr[(i << 1) - 2]);
		t1 = (this->pointArr[(i << 1) - 1] - this->pointArr[(i << 1) + 1]) / (this->pointArr[(i << 1) - 2] - this->pointArr[(i << 1) + 0]);

		if (t1 != t2)
		{
			Double x1 = this->pointArr[(i << 1) + 0];
			Double x2 = this->pointArr[(i << 1) - 2];
			Double x3 = this->pointArr[(i << 1) - 4];
			Double x4 = lastPtX;
			Double x6 = nextPtX;
			Double y1 = this->pointArr[(i << 1) + 1];
			Double y2 = this->pointArr[(i << 1) - 1];
			Double y3 = this->pointArr[(i << 1) - 3];
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
	deg = Math::ArcTan2(this->pointArr[2] - this->pointArr[0], this->pointArr[3] - this->pointArr[1]);

	lastPtX = Math::Cos(deg) * dist + this->pointArr[0];
	lastPtY = -Math::Sin(deg) * dist + this->pointArr[1];

	outPoints->Add(lastPtX);
	outPoints->Add(lastPtY);

	Math::Polygon *pg;
	UOSInt nPoints;
	Double *pts;
	NEW_CLASS(pg, Math::Polygon(this->srid, 1, outPoints->GetCount() >> 1));
	pts = pg->GetPointList(&nPoints);
	nPoints = nPoints << 1;
	i = 0;
	while (i < nPoints)
	{
		pts[i] = outPoints->GetItem(i);
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
