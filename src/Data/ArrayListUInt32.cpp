#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListUInt32.h"

Data::ArrayListUInt32::ArrayListUInt32() : Data::SortableArrayList<UInt32>()
{
}

Data::ArrayListUInt32::ArrayListUInt32(UOSInt capacity) : Data::SortableArrayList<UInt32>(capacity)
{
}

Data::ArrayList<UInt32> *Data::ArrayListUInt32::Clone()
{
	Data::ArrayListUInt32 *newArr;
	NEW_CLASS(newArr, Data::ArrayListUInt32(this->capacity));
	newArr->AddRange(this);
	return newArr;
}

UOSInt Data::ArrayListUInt32::SortedInsert(UInt32 Val)
{
	OSInt i;
	OSInt j;
	OSInt k;
	UInt32 l;
	i = 0;
	j = (OSInt)objCnt - 1;
	while (i <= j)
	{
		k = (i + j) >> 1;
		l = arr[k];
		if (l > Val)
		{
			j = k - 1;
		}
		else if (l < Val)
		{
			i = k + 1;
		}
		else
		{
			i = k + 1;
			break;
		}
	}

	if (objCnt == this->capacity)
	{
		UInt32 *newArr = MemAlloc(UInt32, this->capacity << 1);
		if (i > 0)
		{
			MemCopyNO(&newArr[0], &arr[0], (UOSInt)(i) * sizeof(UInt32));
		}
		newArr[i] = Val;
		if ((UOSInt)i < this->objCnt)
		{
			MemCopyNO(&newArr[i + 1], &arr[i], (this->objCnt - (UOSInt)i) * sizeof(UInt32));
		}
		this->capacity = this->capacity << 1;
		MemFree(arr);
		arr = newArr;
	}
	else
	{
		j = this->objCnt;
		while (j > i)
		{
			arr[j] = arr[j - 1];
			j--;
		}
		arr[i] = Val;
	}
	this->objCnt++;
	return (UOSInt)i;
}

OSInt Data::ArrayListUInt32::SortedIndexOf(UInt32 Val)
{
	OSInt i;
	OSInt j;
	OSInt k;
	UInt32 l;
	i = 0;
	j = (OSInt)objCnt - 1;
	while (i <= j)
	{
		k = (i + j) >> 1;
		l = arr[k];
		if (l > Val)
		{
			j = k - 1;
		}
		else if (l < Val)
		{
			i = k + 1;
		}
		else
		{
			return k;
		}
	}
	return -i - 1;
}
