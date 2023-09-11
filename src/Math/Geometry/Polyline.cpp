#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/CoordinateSystem.h"
#include "Math/Math.h"
#include "Math/Geometry/Polyline.h"
#include "Data/ArrayListDbl.h"

Math::Geometry::Polyline::Polyline(UInt32 srid, const Math::Coord2DDbl *pointArr, UOSInt nPoint, Bool hasZ, Bool hasM) : PointOfstCollection(srid, 1, nPoint, pointArr, hasZ, hasM)
{
	this->flags = 0;
	this->color = 0;
}

Math::Geometry::Polyline::Polyline(UInt32 srid, UOSInt nPtOfst, UOSInt nPoint, Bool hasZ, Bool hasM) : PointOfstCollection(srid, nPtOfst, nPoint, 0, hasZ, hasM)
{
	this->flags = 0;
	this->color = 0;
}

Math::Geometry::Polyline::~Polyline()
{
}

Math::Geometry::Vector2D::VectorType Math::Geometry::Polyline::GetVectorType() const
{
	return Math::Geometry::Vector2D::VectorType::Polyline;
}

NotNullPtr<Math::Geometry::Vector2D> Math::Geometry::Polyline::Clone() const
{
	NotNullPtr<Math::Geometry::Polyline> pl;
	NEW_CLASSNN(pl, Math::Geometry::Polyline(this->srid, this->nPtOfst, this->nPoint, this->HasZ(), this->HasM()));
	MemCopyNO(pl->ptOfstArr, this->ptOfstArr, sizeof(Int32) * this->nPtOfst);
	MemCopyAC(pl->pointArr, this->pointArr, sizeof(Math::Coord2DDbl) * nPoint);
	if (this->zArr)
	{	
		MemCopyAC(pl->zArr, this->zArr, sizeof(Double) * nPoint);
	}
	if (this->mArr)
	{	
		MemCopyAC(pl->mArr, this->mArr, sizeof(Double) * nPoint);
	}
	pl->flags = this->flags;
	pl->color = this->color;
	return pl;
}

Double Math::Geometry::Polyline::CalBoundarySqrDistance(Math::Coord2DDbl pt, OutParam<Math::Coord2DDbl> nearPt) const
{
	UOSInt k;
	UOSInt l;
	UInt32 m;
	UInt32 *ptOfsts;
	Math::Coord2DDbl *points;

	ptOfsts = this->ptOfstArr;
	points = this->pointArr;

	k = this->nPtOfst;
	l = this->nPoint;

	Double calBase;
	Math::Coord2DDbl calDiff;
	Math::Coord2DDbl calSqDiff;
	Math::Coord2DDbl calPt;
	Math::Coord2DDbl calPtOut = Math::Coord2DDbl(0, 0);
	Double calD;
	Double dist = 0x7fffffff;

	while (k--)
	{
		m = ptOfsts[k];
		l--;
		while (l-- > m)
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
	}
	k = this->nPoint;
	while (k-- > 0)
	{
		calDiff = pt - points[k];
		calSqDiff = calDiff * calDiff;
		calD = calSqDiff.x + calSqDiff.y;
		if (calD < dist)
		{
			dist = calD;
			calPtOut = points[k];
		}
	}
	nearPt.Set(calPtOut);
	return dist;
}

Bool Math::Geometry::Polyline::JoinVector(NotNullPtr<const Math::Geometry::Vector2D> vec)
{
	if (vec->GetVectorType() != Math::Geometry::Vector2D::VectorType::Polyline || this->HasZ() != vec->HasZ() || this->HasM() != vec->HasM())
	{
		return false;
	}
	Math::Geometry::Polyline *pl = (Math::Geometry::Polyline*)vec.Ptr();

	UInt32 *newPtOfsts = MemAlloc(UInt32, this->nPtOfst + pl->nPtOfst);
	MemCopyNO(newPtOfsts, this->ptOfstArr, this->nPtOfst * sizeof(UInt32));
	UOSInt i = pl->nPtOfst;
	while (i-- > 0)
	{
		newPtOfsts[this->nPtOfst + i] = pl->ptOfstArr[i] + (UInt32)this->nPoint;
	}
	this->nPtOfst += pl->nPtOfst;
	MemFree(this->ptOfstArr);
	this->ptOfstArr = newPtOfsts;

	Math::Coord2DDbl *newPoints = MemAllocA(Math::Coord2DDbl, (this->nPoint + pl->nPoint));
	MemCopyAC(newPoints, this->pointArr, this->nPoint * sizeof(Math::Coord2DDbl));
	MemCopyAC(&newPoints[this->nPoint], pl->pointArr, pl->nPoint * sizeof(Math::Coord2DDbl));
	MemFreeA(this->pointArr);
	this->pointArr = newPoints;

	if (this->zArr)
	{
		Double *newZ = MemAllocA(Double, (this->nPoint + pl->nPoint));
		MemCopyAC(newZ, this->zArr, this->nPoint * sizeof(Math::Coord2DDbl));
		MemCopyNO(&newZ[this->nPoint], pl->zArr, pl->nPoint * sizeof(Double));
		MemFreeA(this->zArr);
		this->zArr = newZ;
	}

	if (this->mArr)
	{
		Double *newM = MemAllocA(Double, (this->nPoint + pl->nPoint));
		MemCopyAC(newM, this->mArr, this->nPoint * sizeof(Math::Coord2DDbl));
		MemCopyNO(&newM[this->nPoint], pl->mArr, pl->nPoint * sizeof(Double));
		MemFreeA(this->mArr);
		this->mArr = newM;
	}
	this->nPoint += pl->nPoint;

	this->OptimizePolyline();
	return true;
}

Math::Geometry::Polyline *Math::Geometry::Polyline::SplitByPoint(Math::Coord2DDbl pt)
{
	UOSInt k;
	UOSInt l;
	UInt32 *ptOfsts;

	ptOfsts = this->ptOfstArr;

	k = this->nPtOfst;
	l = this->nPoint;

	Math::Coord2DDbl calPt;
	Double calZ;
	Double calM;
	Bool isPoint;
	UOSInt minId = (UOSInt)this->GetPointNo(pt, &isPoint, &calPt, &calZ, &calM);

	UInt32 *oldPtOfsts;
	UInt32 *newPtOfsts;
	Math::Coord2DDbl *oldPoints;
	Math::Coord2DDbl *newPoints;
	Double *oldZ;
	Double *newZ;
	Double *oldM;
	Double *newM;
	Math::Geometry::Polyline *newPL;
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
		oldZ = this->zArr;
		oldM = this->mArr;

		k = this->nPtOfst;
		while (k-- > 0)
		{
			if (oldPtOfsts[k] < minId)
			{
				break;
			}
		}
		newPtOfsts = MemAlloc(UInt32, k + 1);
		newPoints = MemAllocA(Math::Coord2DDbl, (minId + 1));
		if (oldZ)
		{
			newZ = MemAllocA(Double, (minId + 1));
			MemCopyAC(newZ, oldZ, sizeof(Double) * (minId + 1));
		}
		else
		{
			newZ = 0;
		}
		if (oldM)
		{
			newM = MemAllocA(Double, (minId + 1));
			MemCopyAC(newM, oldM, sizeof(Double) * (minId + 1));
		}
		else
		{
			newM = 0;
		}
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
		this->zArr = newZ;
		this->mArr = newM;
		NEW_CLASS(newPL, Math::Geometry::Polyline(this->srid, this->nPtOfst - k, this->nPoint - minId, this->zArr != 0, this->mArr != 0));
		newPtOfsts = newPL->GetPtOfstList(l);
		l = this->nPtOfst;
		while (--l > k)
		{
			newPtOfsts[l - k] = ptOfsts[l] - (UInt32)minId;
		}
		newPtOfsts[0] = 0;
		newPoints = newPL->GetPointList(l);
		l = this->nPoint;
		while (l-- > minId)
		{
			newPoints[l - minId] = oldPoints[l];
		}
		if (oldZ)
		{
			l = this->nPoint;
			newZ = newPL->GetZList(l);
			while (l-- > minId)
			{
				newZ[l - minId] = oldZ[l];
			}
			MemFreeA(oldZ);
		}
		if (oldM)
		{
			l = this->nPoint;
			newM = newPL->GetMList(l);
			while (l-- > minId)
			{
				newM[l - minId] = oldM[l];
			}
			MemFreeA(oldM);
		}
		this->nPoint = minId + 1;
		this->nPtOfst = k + 1;
		MemFreeA(oldPoints);
		MemFree(oldPtOfsts);

		return newPL;
	}
	else
	{
		oldPtOfsts = this->ptOfstArr;
		oldPoints = this->pointArr;
		oldZ = this->zArr;
		oldM = this->mArr;

		k = this->nPtOfst;
		while (k-- > 0)
		{
			if (oldPtOfsts[k] <= minId)
			{
				break;
			}
		}
		newPtOfsts = MemAlloc(UInt32, k + 1);
		newPoints = MemAllocA(Math::Coord2DDbl, (minId + 2));
		if (oldZ)
		{
			newZ = MemAllocA(Double, (minId + 2));
			MemCopyAC(newZ, oldZ, sizeof(Double) * (minId + 1));
			newZ[minId + 1] = calZ;
		}
		else
		{
			newZ = 0;
		}
		if (oldM)
		{
			newM = MemAllocA(Double, (minId + 2));
			MemCopyAC(newM, oldM, sizeof(Double) * (minId + 1));
			newM[minId + 1] = calM;
		}
		else
		{
			newM = 0;
		}
		l = minId + 1;
		while (l-- > 0)
		{
			newPoints[l] = oldPoints[l];
		}
		newPoints[minId + 1] = calPt;

		l = k + 1;
		while (l-- > 0)
		{
			newPtOfsts[l] = oldPtOfsts[l];
		}

		this->ptOfstArr = newPtOfsts;
		this->pointArr = newPoints;
		this->zArr = newZ;
		this->mArr = newM;
		NEW_CLASS(newPL, Math::Geometry::Polyline(this->srid, this->nPtOfst - k, this->nPoint - minId, oldZ != 0, oldM != 0));

		newPtOfsts = newPL->GetPtOfstList(l);
		l = this->nPtOfst;
		while (--l > k)
		{
			newPtOfsts[l - k] = ptOfsts[l] - (UInt32)minId;
		}
		newPtOfsts[0] = 0;
		MemFree(oldPtOfsts);

		newPoints = newPL->GetPointList(l);
		l = this->nPoint;
		while (--l > minId)
		{
			newPoints[l - minId] = oldPoints[l];
		}
		newPoints[0] = calPt;
		MemFreeA(oldPoints);

		if (oldZ)
		{
			newZ = newPL->GetZList(l);
			l = this->nPoint;
			while (--l > minId)
			{
				newZ[l - minId] = oldZ[l];
			}
			newZ[0] = calZ;
			MemFreeA(oldZ);
		}

		if (oldM)
		{
			newM = newPL->GetMList(l);
			l = this->nPoint;
			while (--l > minId)
			{
				newM[l - minId] = oldM[l];
			}
			newM[0] = calM;
			MemFreeA(oldM);
		}

		this->nPoint = minId + 2;
		this->nPtOfst = k + 1;

		return newPL;
	}
}

void Math::Geometry::Polyline::OptimizePolyline()
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
}

OSInt Math::Geometry::Polyline::GetPointNo(Math::Coord2DDbl pt, Bool *isPoint, Math::Coord2DDbl *calPtOutPtr, Double *calZOutPtr, Double *calMOutPtr)
{
	UOSInt k;
	UOSInt l;
	UInt32 m;
	UInt32 *ptOfsts;
	Math::Coord2DDbl *points;
	Double *zArr;
	Double *mArr;

	ptOfsts = this->ptOfstArr;
	points = this->pointArr;
	zArr = this->zArr;
	mArr = this->mArr;

	k = this->nPtOfst;
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

	while (k--)
	{
		m = ptOfsts[k];
		l--;
		while (l-- > m)
		{
			calDiff = points[l] - points[l + 1];

			if (calDiff.x == 0 && calDiff.y == 0)
			{
				calPt.x = pt.x;
				calPt.y = pt.y;
				if (zArr)
				{
					calZ = zArr[l];
				}
				else
				{
					calZ = 0;
				}
				if (mArr)
				{
					calM = mArr[l];
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
					if (zArr)
					{
						calZ = (ratio * (zArr[l] - zArr[l + 1])) + zArr[l];
					}
					else
					{
						calZ = 0;
					}
					if (mArr)
					{
						calM = (ratio * (mArr[l] - mArr[l + 1])) + mArr[l];
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
	}
	k = this->nPoint;
	while (k-- > 0)
	{
		calDiff = pt - points[k];
		calSqDiff = calDiff * calDiff;
		calD = calSqDiff.x + calSqDiff.y;
		if (calD < dist)
		{
			dist = calD;
			calPtOut = points[k];
			calZOut = zArr?zArr[k]:0;
			calMOut = mArr?mArr[k]:0;
			minId = (OSInt)k;
			isPointI = true;
		}
	}

	if (isPoint)
	{
		*isPoint = isPointI;
	}
	if (calPtOutPtr)
	{
		*calPtOutPtr = calPtOut;
	}
	if (calZOutPtr)
	{
		*calZOutPtr = calZOut;
	}
	if (calMOutPtr)
	{
		*calMOutPtr = calMOut;
	}
	return minId;
}

Math::Geometry::Polygon *Math::Geometry::Polyline::CreatePolygonByDist(Double dist) const
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
}

Bool Math::Geometry::Polyline::HasColor() const
{
	return (this->flags & 1) != 0;
}

UInt32 Math::Geometry::Polyline::GetColor() const
{
	return this->color;
}
void Math::Geometry::Polyline::SetColor(UInt32 color)
{
	this->color = color;
	this->flags |= 1;
}
