#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListDate.h"
#include "Data/Comparable.h"
#include "Data/Sort/QuickBubbleSort.h"
#include <stdlib.h>

void Data::ArrayListDate::Init(UOSInt capacity)
{
	objCnt = 0;
	this->capacity = capacity;
	arr = MemAllocArr(Int64, capacity);
}

Data::ArrayListDate::ArrayListDate()
{
	Init(40);
}

Data::ArrayListDate::ArrayListDate(UOSInt capacity)
{
	Init(Capacity);
}

Data::ArrayListDate::~ArrayListDate()
{
	MemFreeArr(arr);
}

UOSInt Data::ArrayListDate::Add(NN<Data::DateTime> val)
{
	UOSInt ret;
	if (objCnt == this->capacity)
	{
		UnsafeArray<Int64> newArr = MemAllocArr(Int64, this->capacity << 1);
		UOSInt i = this->capacity;
		while (i-- > 0)
		{
			newArr[i] = arr[i];
		}
		this->capacity = this->capacity << 1;
		MemFreeArr(arr);
		arr = newArr;
	}
	arr[ret = objCnt++] = val->ToTicks();
	return ret;
}

void Data::ArrayListDate::AddAll(NN<const ArrayListDate> arr)
{
	UOSInt i;
	UOSInt j;
	while (objCnt + arr->GetCount() >= this->capacity)
	{
		UnsafeArray<Int64> newArr = MemAllocArr(Int64, this->capacity << 1);
		i = objCnt;
		while (i-- > 0)
		{
			newArr[i] = this->arr[i];
		}
		this->capacity = this->capacity << 1;
		MemFreeArr(this->arr);
		this->arr = newArr;
	}
	i = objCnt + arr->GetCount();
	j = arr->GetCount();
	while (j-- > 0)
	{
		this->arr[--i] = arr->arr[j];
	}
	this->objCnt += arr->objCnt;
}

void Data::ArrayListDate::AddRange(UnsafeArray<NN<const Data::DateTime>> arr, UOSInt cnt)
{
	UOSInt i;
	UOSInt j;
	while (objCnt + cnt >= this->capacity)
	{
		UnsafeArray<Int64> newArr = MemAllocArr(Int64, this->capacity << 1);
		i = objCnt;
		while (i-- > 0)
		{
			newArr[i] = this->arr[i];
		}
		this->capacity = this->capacity << 1;
		MemFreeArr(this->arr);
		this->arr = newArr;
	}
	i = objCnt + cnt;
	j = cnt;
	while (j-- > 0)
	{
		this->arr[--i] = NN<Data::DateTime>::ConvertFrom(arr[j])->ToTicks();
	}
	this->objCnt += cnt;
}

void Data::ArrayListDate::GetItem(UOSInt index, NN<Data::DateTime> valOut) const
{
	valOut->SetTicks(this->arr[index]);
}

void Data::ArrayListDate::SetItem(UOSInt index, NN<Data::DateTime> val)
{
	if (index == objCnt)
	{
		Add(val);
	}
	else if (index < objCnt)
	{
		this->arr[index] = val->ToTicks();
	}
	else
	{
		return;
	}
}

void Data::ArrayListDate::Clear()
{
	OSInt i = objCnt;
	while (i-- > 0)
	{
		arr[i] = 0;
	}
	this->objCnt = 0;
}

NN<Data::ArrayListDate> Data::ArrayListDate::Clone() const
{
	NN<Data::ArrayListDate> newArr;
	NEW_CLASSNN(newArr, Data::ArrayListDate(this->capacity));
	newArr->AddAll(*this);
	return newArr;
}

UOSInt Data::ArrayListDate::GetCount() const
{
	return this->objCnt;
}

UOSInt Data::ArrayListDate::GetCapacity() const
{
	return this->capacity;
}

void Data::ArrayListDate::RemoveAt(UOSInt index)
{
	if (index >= objCnt)
		return;
	while (++index < objCnt)
	{
		arr[index - 1] = arr[index];
	}
	arr[--objCnt] = 0;
}

void Data::ArrayListDate::Insert(UOSInt index, NN<Data::DateTime> val)
{
	UOSInt j;
	if (objCnt == this->capacity)
	{
		UnsafeArray<Int64> newArr = MemAllocArr(Int64, this->capacity << 1);
		j = this->objCnt;
		while (j-- > 0)
		{
			newArr[j] = this->arr[j];
		}
		this->capacity = this->capacity << 1;
		MemFreeArr(arr);
		arr = newArr;
	}
	j = objCnt;
	while (j > index)
	{
		arr[j] = arr[j - 1];
		j--;
	}
	objCnt++;
	arr[index] = val->ToTicks();
}

UOSInt Data::ArrayListDate::SortedInsert(NN<Data::DateTime> val)
{
	OSInt i;
	OSInt j;
	OSInt k;
	Int64 l;
	Int64 ts = val->ToTicks();
	i = 0;
	j = objCnt - 1;
	while (i <= j)
	{
		k = (i + j) >> 1;
		l = arr[k];
		if (l > ts)
		{
			j = k - 1;
		}
		else if (l < ts)
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
		UnsafeArray<Int64> newArr = MemAllocArr(Int64, this->capacity << 1);
		k = (OSInt)this->objCnt;
		while (k-- > 0)
		{
			newArr[k] = arr[k];
		}
		this->capacity = this->capacity << 1;
		MemFreeArr(arr);
		arr = newArr;
	}
	j =(OSInt)objCnt;
	while (j > i)
	{
		arr[j] = arr[j - 1];
		j--;
	}
	objCnt++;
	arr[i] = ts;
	return (UOSInt)i;
}

OSInt Data::ArrayListDate::SortedIndexOf(NN<Data::DateTime> Val) const
{
	OSInt i;
	OSInt j;
	OSInt k;
	Int64 l;
	Int64 ts = val->ToTicks();
	i = 0;
	j = objCnt - 1;
	while (i <= j)
	{
		k = (i + j) >> 1;
		l = arr[k];
		if (l > ts)
		{
			j = k - 1;
		}
		else if (l < ts)
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

UOSInt Data::ArrayListDate::IndexOf(NN<Data::DateTime> val) const
{
	Int64 ts = val->ToTicks();
	UOSInt i = objCnt;
	while (i-- > 0)
		if (arr[i] == ts)
			return i;
	return INVALID_INDEX;
}

void Data::ArrayListDate::Sort(Bool decending)
{
	if (decending)
	{
		Data::Sort::QuickBubbleSort::SortInv(this->arr, 0, this->objCnt - 1);
	}
	else
	{
		Data::Sort::QuickBubbleSort::Sort(this->arr, 0, this->objCnt - 1);
	}
}
