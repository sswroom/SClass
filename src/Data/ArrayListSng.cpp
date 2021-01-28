#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListSng.h"
#include "Data/IComparable.h"
#include <stdlib.h>

Data::ArrayListSng::ArrayListSng() : Data::SortableArrayList<Single>()
{
}

Data::ArrayListSng::ArrayListSng(OSInt Capacity) : Data::SortableArrayList<Single>(Capacity)
{
}

Data::ArrayList<Single> *Data::ArrayListSng::Clone()
{
	Data::ArrayListSng *newArr;
	NEW_CLASS(newArr, Data::ArrayListSng(this->capacity));
	newArr->AddRange(this);
	return newArr;
}

UOSInt Data::ArrayListSng::SortedInsert(Single Val)
{
	OSInt i;
	OSInt j;
	OSInt k;
	Single l;
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
		Single *newArr = MemAlloc(Single, this->capacity << 1);
		k = this->objCnt;
		while (k-- > 0)
		{
			newArr[k] = arr[k];
		}
		this->capacity = this->capacity << 1;
		MemFree(arr);
		arr = newArr;
	}
	j = objCnt;
	while (j > i)
	{
		arr[j] = arr[j - 1];
		j--;
	}
	objCnt++;
	arr[i] = Val;
	return (UOSInt)i;
}

OSInt Data::ArrayListSng::SortedIndexOf(Single Val)
{
	OSInt i;
	OSInt j;
	OSInt k;
	Single l;
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
