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

Math::Vector2D *Math::Polyline3D::Clone()
{
	Math::Polyline3D *pl;
	NEW_CLASS(pl, Math::Polyline3D(this->srid, this->nPtOfst, this->nPoint));
	MemCopyNO(pl->ptOfstArr, this->ptOfstArr, sizeof(Int32) * nPtOfst);
	MemCopyNO(pl->pointArr, this->pointArr, sizeof(Double) * (nPoint << 1));
	MemCopyNO(pl->altitudes, this->altitudes, sizeof(Double) * nPoint);
	return pl;
}

Bool Math::Polyline3D::Support3D()
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
		Math::CoordinateSystem::ConvertXYZ(srcCSys, destCSys, this->pointArr[(i << 1)], this->pointArr[(i << 1) + 1], this->altitudes[i], &this->pointArr[(i << 1)], &this->pointArr[(i << 1) + 1], &this->altitudes[i]);
	}
}

Bool Math::Polyline3D::Equals(Math::Vector2D *vec)
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
		Double *ptList = pl->GetPointList(&nPoint);
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
		ptList = pl->GetAltitudeList(&nPoint);
		i = nPoint;
		while (i-- > 0)
		{
			if (ptList[i] != this->altitudes[i])
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

Math::Polyline *Math::Polyline3D::SplitByPoint(Double x, Double y)
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
	Double calZD;
	Double calX;
	Double calY;
	Double calZ;
	Double calD;
	Double dist = 0x7fffffff;
	Double calPtX = 0;
	Double calPtY = 0;
	Double calPtZ = 0;
	UOSInt minId = 0;
	Bool isPoint = false;

	while (k--)
	{
		m = ptOfsts[k];
		l--;
		while (l-- > m)
		{
			calH = points[(l << 1) + 1] - points[(l << 1) + 3];
			calW = points[(l << 1) + 0] - points[(l << 1) + 2];
			calZD = altitudes[l] - altitudes[l + 1];

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
				if (calZD == 0)
				{
					calZ = altitudes[l];
				}
				else
				{
					calZ = ((calY - (points[(l << 1) + 1])) * calZD / calH) + altitudes[l];
				}
			}
			else
			{
				calY = ((calX - (points[(l << 1) + 0])) * calH / calW) + points[(l << 1) + 1];
				if (calZD == 0)
				{
					calZ = altitudes[l];
				}
				else
				{
					calZ = ((calX - (points[(l << 1) + 0])) * calZD / calW) + altitudes[l];
				}
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
				calPtZ = calZ;
				isPoint = false;
				minId = l;
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
			calPtZ = altitudes[k];
			minId = k;
			isPoint = true;
		}
	}
	UInt32 *oldPtOfsts;
	UInt32 *newPtOfsts;
	Double *oldPoints;
	Double *newPoints;
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
		newPoints = MemAlloc(Double, (minId + 1) * 2);
		newAltitudes = MemAlloc(Double, minId + 1);
		l = minId + 1;
		while (l-- > 0)
		{
			newPoints[(l << 1) + 0] = oldPoints[(l << 1) + 0];
			newPoints[(l << 1) + 1] = oldPoints[(l << 1) + 1];
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
			newPoints[((l - minId) << 1) + 0] = oldPoints[(l << 1) + 0];
			newPoints[((l - minId) << 1) + 1] = oldPoints[(l << 1) + 1];
			newAltitudes[l - minId] = oldAltitudes[l];
		}
		this->nPoint = minId + 1;
		this->nPtOfst = k + 1;
		MemFree(oldPoints);
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
		newPoints = MemAlloc(Double, (minId + 2) * 2);
		newAltitudes = MemAlloc(Double,  minId + 2);
		l = minId + 1;
		while (l-- > 0)
		{
			newPoints[(l << 1) + 0] = oldPoints[(l << 1) + 0];
			newPoints[(l << 1) + 1] = oldPoints[(l << 1) + 1];
			newAltitudes[l] = oldAltitudes[l];
		}
		newPoints[((minId + 1) << 1) + 0] = calPtX;
		newPoints[((minId + 1) << 1) + 1] = calPtY;
		newAltitudes[minId + 1] = calPtZ;

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
			newPoints[((l - minId) << 1) + 0] = oldPoints[(l << 1) + 0];
			newPoints[((l - minId) << 1) + 1] = oldPoints[(l << 1) + 1];
			newAltitudes[l - minId] = oldAltitudes[l];
		}
		newPoints[0] = calPtX;
		newPoints[1] = calPtY;
		newAltitudes[0] = calPtZ;

		this->nPoint = minId + 2;
		this->nPtOfst = k + 1;
		MemFree(oldPoints);
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

