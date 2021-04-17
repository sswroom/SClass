#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListUInt64.h"
#include "Data/IComparable.h"

Data::ArrayListUInt64::ArrayListUInt64() : Data::SortableArrayList<UInt64>()
{
}

Data::ArrayListUInt64::ArrayListUInt64(UOSInt capacity) : Data::SortableArrayList<UInt64>(capacity)
{
}

Data::ArrayList<UInt64> *Data::ArrayListUInt64::Clone()
{
	Data::ArrayListUInt64 *newArr;
	NEW_CLASS(newArr, Data::ArrayListUInt64(this->capacity));
	newArr->AddRange(this);
	return newArr;
}

UOSInt Data::ArrayListUInt64::SortedInsert(UInt64 val)
{
	OSInt i;
	OSInt j;
	OSInt k;
	UInt64 l;
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
		UInt64 *newArr = MemAlloc(UInt64, this->capacity << 1);
		if (i > 0)
		{
			MemCopyNO(&newArr[0], &arr[0], (UOSInt)(i) * sizeof(UInt64));
		}
		newArr[i] = val;
		if ((UOSInt)i < this->objCnt)
		{
			MemCopyNO(&newArr[i + 1], &arr[i], (this->objCnt - (UOSInt)i) * sizeof(UInt64));
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

OSInt Data::ArrayListUInt64::SortedIndexOf(UInt64 val)
{
	OSInt i;
	OSInt j;
	OSInt k;
	UInt64 l;
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

void Data::ArrayListUInt64::AddRangeU32(UInt32 *arr, UOSInt cnt)
{
	UOSInt i;
	UOSInt j;
	while (this->objCnt + cnt >= this->capacity)
	{
		UInt64 *newArr = MemAlloc(UInt64, this->capacity * 2);
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

void Data::ArrayListUInt64::AddRangeU32(Data::ArrayList<UInt32> *arr)
{
	UOSInt i;
	UOSInt j;
	while (this->objCnt + arr->GetCount() >= this->capacity)
	{
		UInt64 *newArr = MemAlloc(UInt64, this->capacity * 2);
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

