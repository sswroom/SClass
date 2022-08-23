#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/CoordinateSystem.h"
#include "Math/Geometry/PointOfstCollection.h"

Math::Geometry::PointOfstCollection::PointOfstCollection(UInt32 srid, UOSInt nPtOfst, UOSInt nPoint, const Math::Coord2DDbl *pointArr, Bool hasZ, Bool hasM) : Math::Geometry::PointCollection(srid, nPoint, pointArr)
{
	if (nPtOfst == 0)
	{
		nPtOfst = 1;
	}
	this->nPtOfst = nPtOfst;
	this->ptOfstArr = MemAlloc(UInt32, nPtOfst);
	MemClear(this->ptOfstArr, sizeof(UInt32) * nPtOfst);
	if (hasZ)
	{
		this->zArr = MemAllocA(Double, nPoint);
	}
	else
	{
		this->zArr = 0;
	}
	if (hasM)
	{
		this->mArr = MemAllocA(Double, nPoint);
	}
	else
	{
		this->mArr = 0;
	}
}

Math::Geometry::PointOfstCollection::~PointOfstCollection()
{
	MemFree(this->ptOfstArr);
	if (this->zArr)
	{
		MemFreeA(this->zArr);
		this->zArr = 0;
	}
	if (this->mArr)
	{
		MemFreeA(this->mArr);
		this->mArr = 0;
	}
}

Math::Coord2DDbl Math::Geometry::PointOfstCollection::GetCenter() const
{
	Double maxLength = 0;
	UOSInt maxId = 0;
	Double currLength;
	UOSInt i = this->nPoint - 1;
	UOSInt j = this->nPtOfst;
	UOSInt k;
	Math::Coord2DDbl lastPt;
	Math::Coord2DDbl thisPt;
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
			return Math::Coord2DDbl(lastPt.x + (thisPt.x - lastPt.x) * maxLength / currLength,
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

void Math::Geometry::PointOfstCollection::ConvCSys(Math::CoordinateSystem *srcCSys, Math::CoordinateSystem *destCSys)
{
	if (this->zArr)
	{
		UOSInt i = this->nPoint;
		while (i-- > 0)
		{
			Math::CoordinateSystem::ConvertXYZ(srcCSys, destCSys, this->pointArr[i].x, this->pointArr[i].y, this->zArr[i], &this->pointArr[i].x, &this->pointArr[i].y, &this->zArr[i]);
		}
	}
	else
	{
		Math::CoordinateSystem::ConvertXYArray(srcCSys, destCSys, this->pointArr, this->pointArr, this->nPoint);
	}
}

Bool Math::Geometry::PointOfstCollection::Equals(Math::Geometry::Vector2D *vec) const
{
	if (vec == 0)
		return false;
	if (vec->GetSRID() != this->srid)
	{
		return false;
	}
	if (vec->GetVectorType() == this->GetVectorType() && this->HasZ() == vec->HasZ() && this->HasM() == vec->HasM())
	{
		Math::Geometry::PointOfstCollection *pl = (Math::Geometry::PointOfstCollection*)vec;
		UOSInt nPtOfst;
		UOSInt nPoint;
		UInt32 *ptOfst = pl->GetPtOfstList(&nPtOfst);
		Math::Coord2DDbl *ptList = pl->GetPointList(&nPoint);
		Double *valArr;
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
		if (this->zArr)
		{
			valArr = pl->zArr;
			i = nPoint;
			while (i-- > 0)
			{
				if (valArr[i] != this->zArr[i])
				{
					return false;
				}
			}
		}
		if (this->mArr)
		{
			valArr = pl->mArr;
			i = nPoint;
			while (i-- > 0)
			{
				if (valArr[i] != this->mArr[i])
				{
					return false;
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

Bool Math::Geometry::PointOfstCollection::HasZ() const
{
	return this->zArr != 0;
}

Double *Math::Geometry::PointOfstCollection::GetZList(UOSInt *nPoint)
{
	*nPoint = this->nPoint;
	return this->zArr;
}

Bool Math::Geometry::PointOfstCollection::HasM() const
{
	return this->mArr != 0;
}

Double *Math::Geometry::PointOfstCollection::GetMList(UOSInt *nPoint)
{
	*nPoint = this->nPoint;
	return this->mArr;
}
