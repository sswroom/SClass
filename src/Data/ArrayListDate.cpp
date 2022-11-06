#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListDate.h"
#include "Data/IComparable.h"
#include "Data/Sort/QuickBubbleSort.h"
#include <stdlib.h>

void Data::ArrayListDate::Init(UOSInt Capacity)
{
	objCnt = 0;
	this->capacity = Capacity;
	arr = MemAlloc(Int64, Capacity);
}

Data::ArrayListDate::ArrayListDate()
{
	Init(40);
}

Data::ArrayListDate::ArrayListDate(UOSInt Capacity)
{
	Init(Capacity);
}

Data::ArrayListDate::~ArrayListDate()
{
	MemFree(arr);
	arr = 0;
}

UOSInt Data::ArrayListDate::Add(Data::DateTime *Val)
{
	UOSInt ret;
	if (objCnt == this->capacity)
	{
		Int64 *newArr = MemAlloc(Int64, this->capacity << 1);
		UOSInt i = this->capacity;
		while (i-- > 0)
		{
			newArr[i] = arr[i];
		}
		this->capacity = this->capacity << 1;
		MemFree(arr);
		arr = newArr;
	}
	arr[ret = objCnt++] = Val->ToTicks();
	return ret;
}

void Data::ArrayListDate::AddRange(const ArrayListDate * arr)
{
	UOSInt i;
	UOSInt j;
	while (objCnt + arr->GetCount() >= this->capacity)
	{
		Int64 *newArr = MemAlloc(Int64, this->capacity << 1);
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
		this->arr[--i] = arr->arr[j];
	}
	this->objCnt += arr->objCnt;
}

void Data::ArrayListDate::AddRange(const Data::DateTime **arr, UOSInt cnt)
{
	UOSInt i;
	UOSInt j;
	while (objCnt + cnt >= this->capacity)
	{
		Int64 *newArr = MemAlloc(Int64, this->capacity << 1);
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
		this->arr[--i] = ((Data::DateTime*)arr[j])->ToTicks();
	}
	this->objCnt += cnt;
}

void Data::ArrayListDate::GetItem(UOSInt Index, Data::DateTime *ValOut) const
{
	*ValOut = this->arr[Index];
}

void Data::ArrayListDate::SetItem(UOSInt Index, Data::DateTime *Val)
{
	if (Index == objCnt)
	{
		Add(Val);
	}
	else if (Index < objCnt)
	{
		this->arr[Index] = Val->ToTicks();
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

Data::ArrayListDate *Data::ArrayListDate::Clone() const
{
	Data::ArrayListDate *newArr = new Data::ArrayListDate(this->capacity);
	newArr->AddRange(this);
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

void Data::ArrayListDate::Insert(UOSInt Index, Data::DateTime *Val)
{
	UOSInt j;
	if (objCnt == this->capacity)
	{
		Int64 *newArr = MemAlloc(Int64, this->capacity << 1);
		j = this->objCnt;
		while (j-- > 0)
		{
			newArr[j] = this->arr[j];
		}
		this->capacity = this->capacity << 1;
		MemFree(arr);
		arr = newArr;
	}
	j = objCnt;
	while (j > Index)
	{
		arr[j] = arr[j - 1];
		j--;
	}
	objCnt++;
	arr[Index] = Val->ToTicks();
}

UOSInt Data::ArrayListDate::SortedInsert(Data::DateTime *Val)
{
	OSInt i;
	OSInt j;
	OSInt k;
	Int64 l;
	Int64 ts = Val->ToTicks();
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
		Int64 *newArr = MemAlloc(Int64, this->capacity << 1);
		k = (OSInt)this->objCnt;
		while (k-- > 0)
		{
			newArr[k] = arr[k];
		}
		this->capacity = this->capacity << 1;
		MemFree(arr);
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

OSInt Data::ArrayListDate::SortedIndexOf(Data::DateTime *Val) const
{
	OSInt i;
	OSInt j;
	OSInt k;
	Int64 l;
	Int64 ts = Val->ToTicks();
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

UOSInt Data::ArrayListDate::IndexOf(Data::DateTime *Val) const
{
	Int64 ts = Val->ToTicks();
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
