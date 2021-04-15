#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListDbl.h"
#include "Data/IComparable.h"
#include <stdlib.h>

Data::ArrayListDbl::ArrayListDbl() : Data::SortableArrayList<Double>()
{
}

Data::ArrayListDbl::ArrayListDbl(UOSInt capacity) : Data::SortableArrayList<Double>(capacity)
{
}

Data::ArrayList<Double> *Data::ArrayListDbl::Clone()
{
	Data::ArrayListDbl *newArr = new Data::ArrayListDbl(this->capacity);
	newArr->AddRange(this);
	return newArr;
}

UOSInt Data::ArrayListDbl::SortedInsert(Double val)
{
	OSInt i;
	OSInt j;
	OSInt k;
	Double l;
	i = 0;
	j = (OSInt)objCnt - 1;
	while (i <= j)
	{
		k = (i + j) >> 1;
		l = arr[k];
		if (l > val)
		{
			j = k - 1;
		}
		else if (l < val)
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
		Double *newArr = MemAlloc(Double, this->capacity << 1);
		if (i > 0)
		{
			MemCopyNO(&newArr[0], &arr[0], (UOSInt)(i) * sizeof(Double));
		}
		newArr[i] = val;
		if ((UOSInt)i < this->objCnt)
		{
			MemCopyNO(&newArr[i + 1], &arr[i], (this->objCnt - (UOSInt)i) * sizeof(Double));
		}
		this->capacity = this->capacity << 1;
		MemFree(arr);
		arr = newArr;
	}
	else
	{
		j = (OSInt)this->objCnt;
		while (j > i)
		{
			arr[j] = arr[j - 1];
			j--;
		}
		arr[i] = val;
	}
	this->objCnt++;
	return (UOSInt)i;
}

OSInt Data::ArrayListDbl::SortedIndexOf(Double val)
{
	OSInt i;
	OSInt j;
	OSInt k;
	Double l;
	i = 0;
	j = (OSInt)objCnt - 1;
	while (i <= j)
	{
		k = (i + j) >> 1;
		l = arr[k];
		if (l > val)
		{
			j = k - 1;
		}
		else if (l < val)
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
