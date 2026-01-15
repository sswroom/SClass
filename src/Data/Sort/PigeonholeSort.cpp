#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/Comparable.h"
#include "Data/Sort/PigeonholeSort.h"
#include "Sync/Interlocked.h"

Data::Sort::PigeonholeSort::PigeonholeSort(Int32 minVal, Int32 maxVal)
{
	this->minVal = minVal;
	this->maxVal = maxVal;
	this->valCount = MemAllocAArr(Int32, maxVal - minVal + 1);
}

Data::Sort::PigeonholeSort::~PigeonholeSort()
{
	MemFreeAArr(this->valCount);
}

void Data::Sort::PigeonholeSort::SortInt32(UnsafeArray<Int32> arr, IntOS firstIndex, IntOS lastIndex)
{
	IntOS cnt = this->maxVal - this->minVal + 1;
	if (cnt & 3)
	{
		MemClear(this->valCount.Ptr(), sizeof(Int32) * cnt);
	}
	else
	{
		MemClearANC(this->valCount.Ptr(), sizeof(Int32) * cnt);
	}
	Int32 cval;
	IntOS i;
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