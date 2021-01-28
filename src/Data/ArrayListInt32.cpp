#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListInt32.h"

Data::ArrayListInt32::ArrayListInt32() : Data::SortableArrayList<Int32>()
{
}

Data::ArrayListInt32::ArrayListInt32(OSInt Capacity) : Data::SortableArrayList<Int32>(Capacity)
{
}

Data::ArrayList<Int32> *Data::ArrayListInt32::Clone()
{
	Data::ArrayListInt32 *newArr;
	NEW_CLASS(newArr, Data::ArrayListInt32(this->capacity));
	newArr->AddRange(this);
	return newArr;
}

UOSInt Data::ArrayListInt32::SortedInsert(Int32 Val)
{
	OSInt i;
	OSInt j;
	OSInt k;
	Int32 l;
	i = 0;
	j = objCnt - 1;
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
		Int32 *newArr = MemAlloc(Int32, this->capacity << 1);
		if (i > 0)
		{
			MemCopyNO(&newArr[0], &arr[0], (i) * sizeof(Int32));
		}
		newArr[i] = Val;
		if ((UOSInt)i < this->objCnt)
		{
			MemCopyNO(&newArr[i + 1], &arr[i], (this->objCnt - i) * sizeof(Int32));
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

OSInt Data::ArrayListInt32::SortedIndexOf(Int32 Val)
{
	OSInt i;
	OSInt j;
	OSInt k;
	Int32 l;
	i = 0;
	j = objCnt - 1;
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
