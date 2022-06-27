#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/CoordinateSystem.h"
#include "Math/Polyline3D.h"

Math::Polyline3D::Polyline3D(UInt32 srid, UOSInt nPtOfst, UOSInt nPoint) : Math::Polyline(srid, nPtOfst, nPoint)
{
	this->altitudes = MemAlloc(Double, nPoint);
	MemClear(this->altitudes, sizeof(Double) * nPoint);
}

Math::Polyline3D::~Polyline3D()
{
	MemFree(this->altitudes);
}

Math::Vector2D *Math::Polyline3D::Clone() const
{
	Math::Polyline3D *pl;
	NEW_CLASS(pl, Math::Polyline3D(this->srid, this->nPtOfst, this->nPoint));
	MemCopyNO(pl->ptOfstArr, this->ptOfstArr, sizeof(Int32) * nPtOfst);
	MemCopyNO(pl->pointArr, this->pointArr, sizeof(Double) * (nPoint << 1));
	MemCopyNO(pl->altitudes, this->altitudes, sizeof(Double) * nPoint);
	return pl;
}

Bool Math::Polyline3D::Support3D() const
{
	return true;
}

Bool Math::Polyline3D::JoinVector(Math::Vector2D *vec)
{
	if (vec->GetVectorType() != Math::Vector2D::VectorType::Polyline || !vec->Support3D())
	{
		return false;
	}
	////////////////////////////////////////////
	return false;
}

void Math::Polyline3D::ConvCSys(Math::CoordinateSystem *srcCSys, Math::CoordinateSystem *destCSys)
{
	UOSInt i = this->nPoint;
	while (i-- > 0)
	{
		Math::CoordinateSystem::ConvertXYZ(srcCSys, destCSys, this->pointArr[i].x, this->pointArr[i].y, this->altitudes[i], &this->pointArr[i].x, &this->pointArr[i].y, &this->altitudes[i]);
	}
}

Bool Math::Polyline3D::Equals(Math::Vector2D *vec) const
{
	if (vec == 0)
		return false;
	if (vec->GetSRID() != this->srid)
	{
		return false;
	}
	if (vec->GetVectorType() == VectorType::Polyline && vec->Support3D())
	{
		Math::Polyline3D *pl = (Math::Polyline3D*)vec;
		UOSInt nPtOfst;
		UOSInt nPoint;
		UInt32 *ptOfst = pl->GetPtOfstList(&nPtOfst);
		Math::Coord2DDbl *ptList = pl->GetPointList(&nPoint);
		Double *alts;
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
		i = nPoint;
		while (i-- > 0)
		{
			if (ptList[i] != this->pointArr[i])
			{
				return false;
			}
		}
		alts = pl->GetAltitudeList(&nPoint);
		i = nPoint;
		while (i-- > 0)
		{
			if (alts[i] != this->altitudes[i])
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

Math::Polyline *Math::Polyline3D::SplitByPoint(Math::Coord2DDbl pt)
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
	Double calDiffZ;
	Double calPtZ;
	Double calD;
	Double dist = 0x7fffffff;
	Double calPtOutZ = 0;
	UOSInt minId = 0;
	Bool isPoint = false;

	while (k--)
	{
		m = ptOfsts[k];
		l--;
		while (l-- > m)
		{
			calDiff = points[l] - points[l + 1];
			calDiffZ = altitudes[l] - altitudes[l + 1];

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
				if (calDiffZ == 0)
				{
					calPtZ = altitudes[l];
				}
				else
				{
					calPtZ = ((calPt.y - points[l].y) * calDiffZ / calDiff.y) + altitudes[l];
				}
			}
			else
			{
				calPt.y = ((calPt.x - points[l].x) * calDiff.y / calDiff.x) + points[l].y;
				if (calDiffZ == 0)
				{
					calPtZ = altitudes[l];
				}
				else
				{
					calPtZ = ((calPt.x - points[l].x) * calDiffZ / calDiff.x) + altitudes[l];
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
				calPtOutZ = calPtZ;
				isPoint = false;
				minId = l;
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
			calPtOutZ = altitudes[k];
			minId = k;
			isPoint = true;
		}
	}
	UInt32 *oldPtOfsts;
	UInt32 *newPtOfsts;
	Math::Coord2DDbl *oldPoints;
	Math::Coord2DDbl *newPoints;
	Double *oldAltitudes;
	Double *newAltitudes;
	Math::Polyline3D *newPL;
	if (isPoint)
	{
		if (minId == this->nPoint - 1 || minId == 0)
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
		oldAltitudes = this->altitudes;

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
		newAltitudes = MemAlloc(Double, minId + 1);
		l = minId + 1;
		while (l-- > 0)
		{
			newPoints[l] = oldPoints[l];
			newAltitudes[l] = oldAltitudes[l];
		}
		l = k + 1;
		while (l-- > 0)
		{
			newPtOfsts[l] = oldPtOfsts[l];
		}

		this->ptOfstArr = newPtOfsts;
		this->pointArr = newPoints;
		this->altitudes = newAltitudes;
		NEW_CLASS(newPL, Math::Polyline3D(this->srid, this->nPtOfst - k, this->nPoint - minId));
		newPtOfsts = newPL->GetPtOfstList(&l);
		l = this->nPtOfst;
		while (--l > k)
		{
			newPtOfsts[l - k] = ptOfsts[l] - (UInt32)minId;
		}
		newPtOfsts[0] = 0;
		newPoints = newPL->GetPointList(&l);
		newAltitudes = newPL->GetAltitudeList(&l);
		l = this->nPoint;
		while (l-- > minId)
		{
			newPoints[(l - minId)] = oldPoints[l];
			newAltitudes[l - minId] = oldAltitudes[l];
		}
		this->nPoint = minId + 1;
		this->nPtOfst = k + 1;
		MemFreeA(oldPoints);
		MemFree(oldPtOfsts);
		MemFree(oldAltitudes);

		return newPL;
	}
	else
	{
		oldPtOfsts = this->ptOfstArr;
		oldPoints = this->pointArr;
		oldAltitudes = this->altitudes;

		k = this->nPtOfst;
		while (k-- > 0)
		{
			if (oldPtOfsts[k] < minId)
			{
				break;
			}
		}
		newPtOfsts = MemAlloc(UInt32, k + 1);
		newPoints = MemAllocA(Math::Coord2DDbl, (minId + 2));
		newAltitudes = MemAlloc(Double,  minId + 2);
		l = minId + 1;
		while (l-- > 0)
		{
			newPoints[l] = oldPoints[l];
			newAltitudes[l] = oldAltitudes[l];
		}
		newPoints[minId + 1] = calPtOut;
		newAltitudes[minId + 1] = calPtOutZ;

		l = k + 1;
		while (l-- > 0)
		{
			newPtOfsts[l] = oldPtOfsts[l];
		}

		this->ptOfstArr = newPtOfsts;
		this->pointArr = newPoints;
		this->altitudes = newAltitudes;
		NEW_CLASS(newPL, Math::Polyline3D(this->srid, this->nPtOfst - k, this->nPoint - minId));
		newPtOfsts = newPL->GetPtOfstList(&l);
		l = this->nPtOfst;
		while (--l > k)
		{
			newPtOfsts[l - k] = ptOfsts[l] - (UInt32)minId;
		}
		newPtOfsts[0] = 0;
		newPoints = newPL->GetPointList(&l);
		newAltitudes = newPL->GetAltitudeList(&l);
		l = this->nPoint;
		while (--l > minId)
		{
			newPoints[l - minId] = oldPoints[l];
			newAltitudes[l - minId] = oldAltitudes[l];
		}
		newPoints[0] = calPtOut;
		newAltitudes[0] = calPtOutZ;

		this->nPoint = minId + 2;
		this->nPtOfst = k + 1;
		MemFreeA(oldPoints);
		MemFree(oldPtOfsts);
		MemFree(oldAltitudes);

		return newPL;
	}
}

Double *Math::Polyline3D::GetAltitudeList(UOSInt *nPoint)
{
	*nPoint = this->nPoint;
	return this->altitudes;
}

