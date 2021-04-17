#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/IComparable.h"
#include "Data/ArrayListCmp.h"
#include <stdlib.h>

Data::ArrayListCmp::ArrayListCmp() : Data::SortableArrayList<Data::IComparable*>()
{
}

Data::ArrayListCmp::ArrayListCmp(UOSInt capacity) : Data::SortableArrayList<Data::IComparable*>(capacity)
{
}

Data::ArrayList<Data::IComparable*> *Data::ArrayListCmp::Clone()
{
	Data::ArrayListCmp *newArr;
	NEW_CLASS(newArr, Data::ArrayListCmp(this->capacity));
	newArr->AddRange(this);
	return newArr;
}

UOSInt Data::ArrayListCmp::SortedInsert(Data::IComparable *val)
{
	OSInt i;
	OSInt j;
	OSInt k;
	OSInt l;
	i = 0;
	j = (OSInt)objCnt - 1;
	while (i <= j)
	{
		k = (i + j) >> 1;
		l = arr[k]->CompareTo(val);
		if (l > 0)
		{
			j = k - 1;
		}
		else if (l < 0)
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
		Data::IComparable **newArr = MemAlloc(Data::IComparable*, this->capacity << 1);
		k = (OSInt)this->objCnt;
		while (k-- > 0)
		{
			newArr[k] = arr[k];
		}
		this->capacity = this->capacity << 1;
		MemFree(arr);
		arr = newArr;
	}
	j = (OSInt)objCnt;
	while (j > i)
	{
		arr[j] = arr[j - 1];
		j--;
	}
	objCnt++;
	arr[i] = val;
	return (UOSInt)i;
}

OSInt Data::ArrayListCmp::SortedIndexOf(Data::IComparable *val)
{
	OSInt i;
	OSInt j;
	OSInt k;
	OSInt l;
	i = 0;
	j = (OSInt)objCnt - 1;
	while (i <= j)
	{
		k = (i + j) >> 1;
		l = arr[k]->CompareTo(val);
		if (l > 0)
		{
			j = k - 1;
		}
		else if (l < 0)
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
