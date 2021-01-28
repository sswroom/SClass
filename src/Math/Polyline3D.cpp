#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/CoordinateSystem.h"
#include "Math/Polyline3D.h"

Math::Polyline3D::Polyline3D(Int32 srid, UOSInt nParts, UOSInt nPoints) : Math::Polyline(srid, nParts, nPoints)
{
	this->altitudes = MemAlloc(Double, nPoints);
	MemClear(this->altitudes, sizeof(Double) * nPoints);
}

Math::Polyline3D::~Polyline3D()
{
	MemFree(this->altitudes);
}

Math::Vector2D *Math::Polyline3D::Clone()
{
	Math::Polyline3D *pl;
	NEW_CLASS(pl, Math::Polyline3D(this->srid, this->nParts, this->nPoints));
	MemCopyNO(pl->parts, this->parts, sizeof(Int32) * nParts);
	MemCopyNO(pl->points, this->points, sizeof(Double) * (nPoints << 1));
	MemCopyNO(pl->altitudes, this->altitudes, sizeof(Double) * nPoints);
	return pl;
}

Bool Math::Polyline3D::Support3D()
{
	return true;
}

Bool Math::Polyline3D::JoinVector(Math::Vector2D *vec)
{
	if (vec->GetVectorType() != Math::Vector2D::VT_POLYLINE || !vec->Support3D())
	{
		return false;
	}
	////////////////////////////////////////////
	return false;
}

void Math::Polyline3D::ConvCSys(Math::CoordinateSystem *srcCSys, Math::CoordinateSystem *destCSys)
{
	OSInt i = this->nPoints;
	while (i-- > 0)
	{
		Math::CoordinateSystem::ConvertXYZ(srcCSys, destCSys, this->points[(i << 1)], this->points[(i << 1) + 1], this->altitudes[i], &this->points[(i << 1)], &this->points[(i << 1) + 1], &this->altitudes[i]);
	}
}

Math::Polyline *Math::Polyline3D::SplitByPoint(Double x, Double y)
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
	Double calZD;
	Double calX;
	Double calY;
	Double calZ;
	Double calD;
	Double dist = 0x7fffffff;
	Double calPtX = 0;
	Double calPtY = 0;
	Double calPtZ = 0;
	UOSInt minId = -1;
	Bool isPoint;

	while (k--)
	{
		m = parts[k];
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
			calPtZ = altitudes[k];
			minId = k;
			isPoint = true;
		}
	}
	UInt32 *oldParts;
	UInt32 *newParts;
	Double *oldPoints;
	Double *newPoints;
	Double *oldAltitudes;
	Double *newAltitudes;
	Math::Polyline3D *newPL;
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
		oldAltitudes = this->altitudes;

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
			newParts[l] = oldParts[l];
		}

		this->parts = newParts;
		this->points = newPoints;
		this->altitudes = newAltitudes;
		NEW_CLASS(newPL, Math::Polyline3D(this->srid, this->nParts - k, this->nPoints - minId));
		newParts = newPL->GetPartList(&l);
		l = this->nParts;
		while (--l > k)
		{
			newParts[l - k] = parts[l] - (UInt32)minId;
		}
		newParts[0] = 0;
		newPoints = newPL->GetPointList(&l);
		newAltitudes = newPL->GetAltitudeList(&l);
		l = this->nPoints;
		while (l-- > minId)
		{
			newPoints[((l - minId) << 1) + 0] = oldPoints[(l << 1) + 0];
			newPoints[((l - minId) << 1) + 1] = oldPoints[(l << 1) + 1];
			newAltitudes[l - minId] = oldAltitudes[l];
		}
		this->nPoints = minId + 1;
		this->nParts = k + 1;
		MemFree(oldPoints);
		MemFree(oldParts);
		MemFree(oldAltitudes);

		return newPL;
	}
	else
	{
		oldParts = this->parts;
		oldPoints = this->points;
		oldAltitudes = this->altitudes;

		k = nParts;
		while (k-- > 0)
		{
			if (oldParts[k] < minId)
			{
				break;
			}
		}
		newParts = MemAlloc(UInt32, k + 1);
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
			newParts[l] = oldParts[l];
		}

		this->parts = newParts;
		this->points = newPoints;
		this->altitudes = newAltitudes;
		NEW_CLASS(newPL, Math::Polyline3D(this->srid, this->nParts - k, this->nPoints - minId));
		newParts = newPL->GetPartList(&l);
		l = this->nParts;
		while (--l > k)
		{
			newParts[l - k] = parts[l] - (UInt32)minId;
		}
		newParts[0] = 0;
		newPoints = newPL->GetPointList(&l);
		newAltitudes = newPL->GetAltitudeList(&l);
		l = this->nPoints;
		while (--l > minId)
		{
			newPoints[((l - minId) << 1) + 0] = oldPoints[(l << 1) + 0];
			newPoints[((l - minId) << 1) + 1] = oldPoints[(l << 1) + 1];
			newAltitudes[l - minId] = oldAltitudes[l];
		}
		newPoints[0] = calPtX;
		newPoints[1] = calPtY;
		newAltitudes[0] = calPtZ;

		this->nPoints = minId + 2;
		this->nParts = k + 1;
		MemFree(oldPoints);
		MemFree(oldParts);
		MemFree(oldAltitudes);


		return newPL;
	}
}

Double *Math::Polyline3D::GetAltitudeList(UOSInt *nPoints)
{
	*nPoints = this->nPoints;
	return this->altitudes;
}

