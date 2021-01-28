#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/CoordinateSystem.h"
#include "Math/Math.h"
#include "Math/Polyline.h"
#include "Data/ArrayListDbl.h"

Math::Polyline::Polyline(Int32 srid, Double *points, UOSInt nPoints) : PointCollection(srid)
{
	this->points = MemAlloc(Double, nPoints << 1);
	MemCopyNO(this->points, points, sizeof(Double) * (nPoints << 1));
	this->nPoints = nPoints;
	this->nParts = 1;
	this->parts = MemAlloc(UInt32, 1);
	this->parts[0] = 0;
	this->flags = 0;
	this->color = 0;
}

Math::Polyline::Polyline(Int32 srid, UOSInt nParts, UOSInt nPoints) : PointCollection(srid)
{
	if (nParts == 0)
	{
		nParts = 1;
	}
	this->points = MemAlloc(Double, nPoints << 1);
	this->nPoints = nPoints;
	MemClear(this->points, sizeof(Double) * (nPoints << 1));
	this->nParts = nParts;
	this->parts = MemAlloc(UInt32, nParts);
	MemClear(this->parts, sizeof(UInt32) * nParts);
	this->flags = 0;
	this->color = 0;
}

Math::Polyline::~Polyline()
{
	MemFree(points);
	MemFree(parts);
}

Math::Vector2D::VectorType Math::Polyline::GetVectorType()
{
	return Math::Vector2D::VT_POLYLINE;
}

UInt32 *Math::Polyline::GetPartList(UOSInt *nParts)
{
	*nParts = this->nParts;
	return this->parts;
}

Double *Math::Polyline::GetPointList(UOSInt *nPoints)
{
	*nPoints = this->nPoints;
	return this->points;
}

void Math::Polyline::GetCenter(Double *x, Double *y)
{
	Double maxLength = 0;
	OSInt maxId = 0;
	Double currLength;
	OSInt i = this->nPoints - 1;
	OSInt j = this->nParts;
	OSInt k;
	Double lastX;
	Double lastY;
	Double thisX;
	Double thisY;
	while (j-- > 0)
	{
		lastX = this->points[(i << 1)];
		lastY = this->points[(i << 1) + 1];
		currLength = 0;
		k = this->parts[j];
		while (i-- > k)
		{
			thisX = this->points[(i << 1)];
			thisY = this->points[(i << 1) + 1];
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
		*x = this->points[0];
		*y = this->points[1];
		return;
	}
	i = this->parts[maxId];
	if ((UOSInt)maxId >= this->nParts - 1)
	{
		j = this->nPoints;
	}
	else
	{
		j = this->parts[maxId + 1];
	}
	maxLength = maxLength * 0.5;
	lastX = this->points[i << 1];
	lastY = this->points[(i << 1) + 1];
	while (i < j)
	{
		i++;
		thisX = this->points[(i << 1)];
		thisY = this->points[(i << 1) + 1];
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
	*x = this->points[0];
	*y = this->points[1];
}

Math::Vector2D *Math::Polyline::Clone()
{
	Math::Polyline *pl;
	NEW_CLASS(pl, Math::Polyline(this->srid, this->nParts, this->nPoints));
	MemCopyNO(pl->parts, this->parts, sizeof(Int32) * nParts);
	MemCopyNO(pl->points, this->points, sizeof(Double) * (nPoints << 1));
	pl->flags = this->flags;
	pl->color = this->color;
	return pl;
}

void Math::Polyline::GetBounds(Double *minX, Double *minY, Double *maxX, Double *maxY)
{
	OSInt i = this->nPoints << 1;
	Double x1;
	Double y1;
	Double x2;
	Double y2;
	x1 = x2 = this->points[0];
	y1 = y2 = this->points[1];
	while (i > 2)
	{
		i -= 2;
		if (x1 > this->points[i])
		{
			x1 = this->points[i];
		}
		if (x2 < this->points[i])
		{
			x2 = this->points[i];
		}
		if (y1 > this->points[i + 1])
		{
			y1 = this->points[i + 1];
		}
		if (y2 < this->points[i + 1])
		{
			y2 = this->points[i + 1];
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
	UInt32 *parts;
	Double *points;

	parts = this->parts;
	points = this->points;

	k = this->nParts;
	l = this->nPoints;

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
		m = parts[k];
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
	k = this->nPoints;
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
	if (vec->GetVectorType() != Math::Vector2D::VT_POLYLINE)
	{
		return false;
	}
	Math::Polyline *pl = (Math::Polyline*)vec;
	UInt32 *newParts = MemAlloc(UInt32, this->nParts + pl->nParts);
	Double *newPoints = MemAlloc(Double, (this->nPoints + pl->nPoints) * 2);
	MemCopyNO(newParts, this->parts, this->nParts * sizeof(UInt32));
	MemCopyNO(newPoints, this->points, this->nPoints * 2 * sizeof(Double));
	MemCopyNO(&newPoints[this->nPoints * 2], pl->points, pl->nPoints * 2 * sizeof(Double));
	OSInt i = pl->nParts;
	while (i-- > 0)
	{
		newParts[this->nParts + i] = pl->parts[i] + (UInt32)this->nPoints;
	}
	MemFree(this->parts);
	MemFree(this->points);
	this->parts = newParts;
	this->points = newPoints;
	this->nParts += pl->nParts;
	this->nPoints += pl->nPoints;
	//////////////////////////////////
	this->OptimizePolyline();
	return true;
}

void Math::Polyline::ConvCSys(Math::CoordinateSystem *srcCSys, Math::CoordinateSystem *destCSys)
{
	UOSInt i = this->nPoints;
	while (i-- > 0)
	{
		Math::CoordinateSystem::ConvertXYZ(srcCSys, destCSys, this->points[(i << 1)], this->points[(i << 1) + 1], 0, &this->points[(i << 1)], &this->points[(i << 1) + 1], 0);
	}
}

Math::Polyline *Math::Polyline::SplitByPoint(Double x, Double y)
{
	UOSInt k;
	UOSInt l;
	UInt32 *parts;

	parts = this->parts;

	k = this->nParts;
	l = this->nPoints;

	Double calPtX;
	Double calPtY;
	Bool isPoint;
	UOSInt minId = this->GetPointNo(x, y, &isPoint, &calPtX, &calPtY);

	UInt32 *oldParts;
	UInt32 *newParts;
	Double *oldPoints;
	Double *newPoints;
	Math::Polyline *newPL;
	if (isPoint)
	{
		if (minId == this->nPoints - 1 || minId == 0)
		{
			return 0;
		}
		k = nParts;
		while (k-- > 1)
		{
			if (this->parts[k] == minId || (this->parts[k] - 1) == minId)
			{
				return 0;
			}
		}
		
		oldParts = this->parts;
		oldPoints = this->points;

		k = nParts;
		while (k-- > 0)
		{
			if (oldParts[k] < minId)
			{
				break;
			}
		}
		newParts = MemAlloc(UInt32, k + 1);
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
			newParts[l] = oldParts[l];
		}

		this->parts = newParts;
		this->points = newPoints;
		NEW_CLASS(newPL, Math::Polyline(this->srid, this->nParts - k, this->nPoints - minId));
		newParts = newPL->GetPartList(&l);
		l = this->nParts;
		while (--l > k)
		{
			newParts[l - k] = parts[l] - (Int32)minId;
		}
		newParts[0] = 0;
		newPoints = newPL->GetPointList(&l);
		l = this->nPoints;
		while (l-- > minId)
		{
			newPoints[((l - minId) << 1) + 0] = oldPoints[(l << 1) + 0];
			newPoints[((l - minId) << 1) + 1] = oldPoints[(l << 1) + 1];
		}
		this->nPoints = minId + 1;
		this->nParts = k + 1;
		MemFree(oldPoints);
		MemFree(oldParts);

		return newPL;
	}
	else
	{
		oldParts = this->parts;
		oldPoints = this->points;

		k = nParts;
		while (k-- > 0)
		{
			if (oldParts[k] <= minId)
			{
				break;
			}
		}
		newParts = MemAlloc(UInt32, k + 1);
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
			newParts[l] = oldParts[l];
		}

		this->parts = newParts;
		this->points = newPoints;
		NEW_CLASS(newPL, Math::Polyline(this->srid, this->nParts - k, this->nPoints - minId));
		newParts = newPL->GetPartList(&l);
		l = this->nParts;
		while (--l > k)
		{
			newParts[l - k] = parts[l] - (Int32)minId;
		}
		newParts[0] = 0;
		newPoints = newPL->GetPointList(&l);
		l = this->nPoints;
		while (--l > minId)
		{
			newPoints[((l - minId) << 1) + 0] = oldPoints[(l << 1) + 0];
			newPoints[((l - minId) << 1) + 1] = oldPoints[(l << 1) + 1];
		}
		newPoints[0] = calPtX;
		newPoints[1] = calPtY;

		this->nPoints = minId + 2;
		this->nParts = k + 1;
		MemFree(oldPoints);
		MemFree(oldParts);

		return newPL;
	}
}

void Math::Polyline::OptimizePolyline()
{
	Double *tmpPoints = MemAlloc(Double, this->nPoints * 2);
	UInt32 lastPoints = (Int32)this->nPoints;
	UInt32 thisPoints;
	UInt32 lastChkPoint;
	UInt32 thisChkPoint;
	UOSInt i = nParts;
	UOSInt j;
	while (i-- > 0)
	{
		thisPoints = this->parts[i];
		lastChkPoint = thisPoints;
		j = i;
		while (j-- > 0)
		{
			thisChkPoint = this->parts[j];
			if (points[((lastChkPoint - 1) << 1)] == points[(thisPoints << 1)] && points[((lastChkPoint - 1) << 1) + 1] == points[(thisPoints << 1) + 1])
			{
				MemCopyNO(tmpPoints, &points[thisPoints << 1], sizeof(Double) * 2 * (lastPoints - thisPoints));
				if (lastPoints < this->nPoints)
				{
					MemCopyO(&points[(lastPoints << 1) - 2], &points[(lastPoints << 1)], sizeof(Double) * 2 * (this->nPoints - lastPoints));
				}
				if (lastChkPoint < thisPoints)
				{
					MemCopyNO(&tmpPoints[2 * (lastPoints - thisPoints)], &points[lastChkPoint << 1], sizeof(Double) * 2 * (thisPoints - lastChkPoint));
					MemCopyNO(&points[lastChkPoint << 1], tmpPoints + 2, sizeof(Double) * 2 * (lastPoints - lastChkPoint - 1));
				}
				else
				{
					MemCopyNO(&points[lastChkPoint << 1], tmpPoints + 2, sizeof(Double) * 2 * (lastPoints - thisPoints - 1));
				}
				this->nParts -= 1;
				while (++j < i)
				{
					this->parts[j] += lastPoints - thisPoints - 1;
				}
				while (j < this->nParts)
				{
					this->parts[j] = this->parts[j + 1] - 1;
					j++;
				}
				this->nPoints -= 1;
				if (i >= nParts)
				{
					thisPoints = (UInt32)this->nPoints;
				}
				else
				{
					thisPoints = this->parts[i];
				}
				break;
			}
			else if (points[(thisChkPoint << 1)] == points[((lastPoints - 1) << 1)] && points[(thisChkPoint << 1) + 1] == points[((lastPoints - 1) << 1) + 1])
			{
				MemCopyNO(tmpPoints, &points[thisPoints << 1], sizeof(Double) * 2 * (lastPoints - thisPoints));
				if (lastPoints < this->nPoints)
				{
					MemCopyO(&points[(lastPoints << 1) - 2], &points[(lastPoints << 1)], sizeof(Double) * 2 * (this->nPoints - lastPoints));
				}
//				MemCopyO(&points[(thisChkPoint + lastPoints - thisPoints - 1) << 1], &points[thisChkPoint << 1], sizeof(Double) * 2 * (thisPoints - thisChkPoint));
//				MemCopyNO(&points[thisChkPoint << 1], tmpPoints, sizeof(Double) * 2 * (lastPoints - thisPoints - 1));
				MemCopyNO(&tmpPoints[(lastPoints - thisPoints) << 1], &points[(thisChkPoint + 1) << 1], sizeof(Double) * 2 * (thisPoints - thisChkPoint - 1));
				MemCopyNO(&points[thisChkPoint << 1], tmpPoints, sizeof(Double) * 2 * (lastPoints - thisChkPoint - 1));
				this->nParts -= 1;
				while (++j < i)
				{
					this->parts[j] += lastPoints - thisPoints - 1;
				}
				while (j < this->nParts)
				{
					this->parts[j] = this->parts[j + 1] - 1;
					j++;
				}
				this->nPoints -= 1;
				if (i >= nParts)
				{
					thisPoints = (UInt32)this->nPoints;
				}
				else
				{
					thisPoints = this->parts[i];
				}
				break;
			}
			else if (points[(thisChkPoint << 1)] == points[(thisPoints << 1)] && points[(thisChkPoint << 1) + 1] == points[(thisPoints << 1) + 1])
			{
				Double *srcPt;
				Double *destPt;
				Int32 ptCnt;

				MemCopyNO(tmpPoints, &points[thisPoints << 1], sizeof(Double) * 2 * (lastPoints - thisPoints));
				if (lastPoints < this->nPoints)
				{
					MemCopyO(&points[(lastPoints << 1) - 2], &points[(lastPoints << 1)], sizeof(Double) * 2 * (this->nPoints - lastPoints));
				}
				MemCopyO(&points[(thisChkPoint + lastPoints - thisPoints - 1) << 1], &points[thisChkPoint << 1], sizeof(Double) * 2 * (thisPoints - thisChkPoint));

				srcPt = tmpPoints;
				destPt = &points[thisChkPoint << 1];
				ptCnt = (lastPoints - thisPoints - 1);
				while (ptCnt-- > 0)
				{
					destPt[0] = srcPt[(ptCnt << 1) + 2];
					destPt[1] = srcPt[(ptCnt << 1) + 3];
					destPt += 2;
				}
				this->nParts -= 1;
				while (++j < i)
				{
					this->parts[j] += lastPoints - thisPoints - 1;
				}
				while (j < this->nParts)
				{
					this->parts[j] = this->parts[j + 1] - 1;
					j++;
				}
				this->nPoints -= 1;
				if (i >= nParts)
				{
					thisPoints = (Int32)this->nPoints;
				}
				else
				{
					thisPoints = this->parts[i];
				}
				break;
			}
			else if (points[((lastChkPoint - 1) << 1)] == points[((lastPoints - 1) << 1)] && points[((lastChkPoint - 1) << 1) + 1] == points[((lastPoints - 1) << 1) + 1])
			{
				Double *srcPt;
				Double *destPt;
				Int32 ptCnt;

				MemCopyNO(tmpPoints, &points[thisPoints << 1], sizeof(Double) * 2 * (lastPoints - thisPoints));
				if (lastPoints < this->nPoints)
				{
					MemCopyO(&points[(lastPoints << 1) - 2], &points[(lastPoints << 1)], sizeof(Double) * 2 * (this->nPoints - lastPoints));
				}
				if (lastChkPoint < thisPoints)
				{
					MemCopyO(&points[(lastChkPoint + lastPoints - thisPoints - 1) << 1], &points[lastChkPoint << 1], sizeof(Double) * 2 * (thisPoints - lastChkPoint));
				}
				srcPt = tmpPoints;
				destPt = &points[lastChkPoint << 1];
				ptCnt = (lastPoints - thisPoints - 1);
				while (ptCnt-- > 0)
				{
					destPt[0] = srcPt[(ptCnt << 1)];
					destPt[1] = srcPt[(ptCnt << 1) + 1];
					destPt += 2;
				}
				this->nParts -= 1;
				while (++j < i)
				{
					this->parts[j] += lastPoints - thisPoints - 1;
				}
				while (j < this->nParts)
				{
					this->parts[j] = this->parts[j + 1] - 1;
					j++;
				}
				this->nPoints -= 1;
				if (i >= nParts)
				{
					thisPoints = (Int32)this->nPoints;
				}
				else
				{
					thisPoints = this->parts[i];
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
	UInt32 *parts;
	Double *points;

	parts = this->parts;
	points = this->points;

	k = this->nParts;
	l = this->nPoints;

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
		m = parts[k];
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
				minId = l;
			}
		}
	}
	k = this->nPoints;
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
			minId = k;
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
	if (this->nPoints < 2)
		return 0;
	if (this->nParts > 1)
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

	deg = Math::ArcTan2(this->points[2] - this->points[0], this->points[3] - this->points[1]);
	lastPtX = -Math::Cos(deg) * dist + this->points[0];
	lastPtY = Math::Sin(deg) * dist + this->points[1];

	outPoints->Add(lastPtX);
	outPoints->Add(lastPtY);

	i = 2;
	while (i < this->nPoints)
	{
		deg = Math::ArcTan2(this->points[(i << 1) + 0] - this->points[(i << 1) - 2], this->points[(i << 1) + 1] - this->points[(i << 1) - 1]);
		nextPtX = -Math::Cos(deg) * dist + this->points[(i << 1) - 2];
		nextPtY = Math::Sin(deg) * dist + this->points[(i << 1) - 1];

		t1 = (this->points[(i << 1) - 3] - this->points[(i << 1) - 1]) / (this->points[(i << 1) - 4] - this->points[(i << 1) - 2]);
		t2 = (this->points[(i << 1) - 1] - this->points[(i << 1) + 1]) / (this->points[(i << 1) - 2] - this->points[(i << 1) + 0]);
		if (t1 != t2)
		{
			Double x1 = this->points[(i << 1) - 4];
			Double x2 = this->points[(i << 1) - 2];
			Double x3 = this->points[(i << 1) + 0];
			Double x4 = lastPtX;
			Double x6 = nextPtX;
			Double y1 = this->points[(i << 1) - 3];
			Double y2 = this->points[(i << 1) - 1];
			Double y3 = this->points[(i << 1) + 1];
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

	deg = Math::ArcTan2(this->points[(this->nPoints << 1) - 2] - this->points[(this->nPoints << 1) - 4], this->points[(this->nPoints << 1) - 1] - this->points[(this->nPoints << 1) - 3]);
	lastPtX = -Math::Cos(deg) * dist + this->points[(this->nPoints << 1) - 2];
	lastPtY = Math::Sin(deg) * dist + this->points[(this->nPoints << 1) - 1];

	outPoints->Add(lastPtX);
	outPoints->Add(lastPtY);

	lastPtX = Math::Cos(deg) * dist + this->points[(this->nPoints << 1) - 2];
	lastPtY = -Math::Sin(deg) * dist + this->points[(this->nPoints << 1) - 1];

	outPoints->Add(lastPtX);
	outPoints->Add(lastPtY);

	i = nPoints;
	while (i > 2)
	{
		i -= 1;
		deg = Math::ArcTan2(this->points[(i << 1) - 4] - this->points[(i << 1) - 2], this->points[(i << 1) - 3] - this->points[(i << 1) - 1]);
		nextPtX = -Math::Cos(deg) * dist + this->points[(i << 1) - 4];
		nextPtY = Math::Sin(deg) * dist + this->points[(i << 1) - 3];

		t2 = (this->points[(i << 1) - 3] - this->points[(i << 1) - 1]) / (this->points[(i << 1) - 4] - this->points[(i << 1) - 2]);
		t1 = (this->points[(i << 1) - 1] - this->points[(i << 1) + 1]) / (this->points[(i << 1) - 2] - this->points[(i << 1) + 0]);

		if (t1 != t2)
		{
			Double x1 = this->points[(i << 1) + 0];
			Double x2 = this->points[(i << 1) - 2];
			Double x3 = this->points[(i << 1) - 4];
			Double x4 = lastPtX;
			Double x6 = nextPtX;
			Double y1 = this->points[(i << 1) + 1];
			Double y2 = this->points[(i << 1) - 1];
			Double y3 = this->points[(i << 1) - 3];
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
	deg = Math::ArcTan2(this->points[2] - this->points[0], this->points[3] - this->points[1]);

	lastPtX = Math::Cos(deg) * dist + this->points[0];
	lastPtY = -Math::Sin(deg) * dist + this->points[1];

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

Int32 Math::Polyline::GetColor()
{
	return this->color;
}
void Math::Polyline::SetColor(Int32 color)
{
	this->color = color;
	this->flags |= 1;
}
