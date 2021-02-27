#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListInt64.h"
#include "Data/IComparable.h"

Data::ArrayListInt64::ArrayListInt64() : Data::SortableArrayList<Int64>()
{
}

Data::ArrayListInt64::ArrayListInt64(UOSInt capacity) : Data::SortableArrayList<Int64>(capacity)
{
}

Data::ArrayList<Int64> *Data::ArrayListInt64::Clone()
{
	Data::ArrayListInt64 *newArr;
	NEW_CLASS(newArr, Data::ArrayListInt64(this->capacity));
	newArr->AddRange(this);
	return newArr;
}

UOSInt Data::ArrayListInt64::SortedInsert(Int64 val)
{
	OSInt i;
	OSInt j;
	OSInt k;
	Int64 l;
	i = 0;
	j = objCnt - 1;
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
		Int64 *newArr = MemAlloc(Int64, this->capacity << 1);
		if (i > 0)
		{
			MemCopyNO(&newArr[0], &arr[0], (i) * sizeof(Int64));
		}
		newArr[i] = val;
		if ((UOSInt)i < this->objCnt)
		{
			MemCopyNO(&newArr[i + 1], &arr[i], (this->objCnt - i) * sizeof(Int64));
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
		arr[i] = val;
	}
	this->objCnt++;
	return (UOSInt)i;
}

OSInt Data::ArrayListInt64::SortedIndexOf(Int64 val)
{
	OSInt i;
	OSInt j;
	OSInt k;
	Int64 l;
	i = 0;
	j = objCnt - 1;
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

void Data::ArrayListInt64::AddRangeI32(Int32 *arr, UOSInt cnt)
{
	UOSInt i;
	UOSInt j;
	while (this->objCnt + cnt >= this->capacity)
	{
		Int64 *newArr = MemAlloc(Int64, this->capacity * 2);
		i = objCnt;
		while (i-- > 0)
		{
			newArr[i] = this->arr[i];
		}
		this->capacity = this->capacity << 1;
		MemFree(this->arr);
		this->arr = newArr;
	}
	i = objCnt + cnt;
	j = cnt;
	while (j-- > 0)
	{
		this->arr[--i] = arr[j];
	}
	this->objCnt += cnt;
}

void Data::ArrayListInt64::AddRangeI32(Data::ArrayList<Int32> *arr)
{
	UOSInt i;
	UOSInt j;
	while (this->objCnt + arr->GetCount() >= this->capacity)
	{
		Int64 *newArr = MemAlloc(Int64, this->capacity * 2);
		i = objCnt;
		while (i-- > 0)
		{
			newArr[i] = this->arr[i];
		}
		this->capacity = this->capacity << 1;
		MemFree(this->arr);
		this->arr = newArr;
	}
	i = objCnt + arr->GetCount();
	j = arr->GetCount();
	while (j-- > 0)
	{
		this->arr[--i] = arr->GetItem(j);
	}
	this->objCnt += arr->GetCount();
}

