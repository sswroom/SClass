#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/Comparable.h"
#include "Data/Sort/CountingSort.h"

Data::Sort::CountingSort::CountingSort(Int32 minVal, Int32 maxVal)
{
	this->minVal = minVal;
	this->maxVal = maxVal;
	this->valCount = MemAllocAArr(Int32, maxVal - minVal + 1);
}

Data::Sort::CountingSort::~CountingSort()
{
	MemFreeAArr(this->valCount);
}

void Data::Sort::CountingSort::SortInt32(UnsafeArray<const Int32> inputArr, UnsafeArray<Int32> outputArr, OSInt firstIndex, OSInt lastIndex)
{
	OSInt i;
	OSInt cnt = this->maxVal - this->minVal + 1;
	Int32 cval;
	if (cnt & 3)
	{
		MemClear(this->valCount.Ptr(), sizeof(Int32) * cnt);
	}
	else
	{
		MemClearANC(this->valCount.Ptr(), sizeof(Int32) * cnt);
	}
	outputArr += firstIndex;
	i = firstIndex;
	while (i <= lastIndex)
		this->valCount[inputArr[i++] - this->minVal]++;

	cval = (Int32)firstIndex;
	i = 0;
	while (i < cnt)
	{
		cval += this->valCount[i];
		this->valCount[i] = cval;
		i++;
	}

	i = firstIndex;
	while (i <= lastIndex)
	{
		cval = inputArr[i];
		outputArr[--(this->valCount[cval])] = cval;
		i++;
	}
}