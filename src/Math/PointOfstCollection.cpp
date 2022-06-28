#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/PointOfstCollection.h"

Math::PointOfstCollection::PointOfstCollection(UInt32 srid, UOSInt nPtOfst, UOSInt nPoint, Math::Coord2DDbl *pointArr) : Math::PointCollection(srid, nPoint, pointArr)
{
	if (nPtOfst == 0)
	{
		nPtOfst = 1;
	}
	this->nPtOfst = nPtOfst;
	this->ptOfstArr = MemAlloc(UInt32, nPtOfst);
	MemClear(this->ptOfstArr, sizeof(UInt32) * nPtOfst);
}

Math::PointOfstCollection::~PointOfstCollection()
{
	MemFree(this->ptOfstArr);
}

Math::Coord2DDbl Math::PointOfstCollection::GetCenter() const
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
