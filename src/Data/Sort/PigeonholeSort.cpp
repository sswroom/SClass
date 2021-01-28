#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/IComparable.h"
#include "Data/Sort/PigeonholeSort.h"
#include "Sync/Interlocked.h"

Data::Sort::PigeonholeSort::PigeonholeSort(Int32 minVal, Int32 maxVal)
{
	this->minVal = minVal;
	this->maxVal = maxVal;
	this->valCount = MemAllocA(Int32, maxVal - minVal + 1);
}

Data::Sort::PigeonholeSort::~PigeonholeSort()
{
	MemFreeA(this->valCount);
}

void Data::Sort::PigeonholeSort::SortInt32(Int32 *arr, OSInt firstIndex, OSInt lastIndex)
{
	OSInt cnt = this->maxVal - this->minVal + 1;
	if (cnt & 3)
	{
		MemClear(this->valCount, sizeof(Int32) * cnt);
	}
	else
	{
		MemClearANC(this->valCount, sizeof(Int32) * cnt);
	}
	Int32 cval;
	OSInt i;
	while (firstIndex <= lastIndex)
		this->valCount[arr[firstIndex++] - this->minVal]++;
	i = 0;
	while (i < cnt)
	{
		cval = this->valCount[i];
		while (cval-- > 0)
		{
			*arr++ = (Int32)i + this->minVal;
		}
		i++;
	}
}