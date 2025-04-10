#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListInt64.h"
#include "Data/Comparable.h"

Data::ArrayListInt64::ArrayListInt64() : Data::SortableArrayListNative<Int64>()
{
}

Data::ArrayListInt64::ArrayListInt64(UOSInt capacity) : Data::SortableArrayListNative<Int64>(capacity)
{
}

NN<Data::ArrayList<Int64>> Data::ArrayListInt64::Clone() const
{
	NN<Data::ArrayListInt64> newArr;
	NEW_CLASSNN(newArr, Data::ArrayListInt64(this->capacity));
	newArr->AddAll(*this);
	return newArr;
}

void Data::ArrayListInt64::AddRangeI32(Int32 *arr, UOSInt cnt)
{
	UOSInt i;
	UOSInt j;
	while (this->objCnt + cnt >= this->capacity)
	{
		UnsafeArray<Int64> newArr = MemAllocArr(Int64, this->capacity * 2);
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
		this->arr[--i] = arr[j];
	}
	this->objCnt += cnt;
}

void Data::ArrayListInt64::AddRangeI32(const Data::ArrayList<Int32> *arr)
{
	UOSInt i;
	UOSInt j;
	while (this->objCnt + arr->GetCount() >= this->capacity)
	{
		UnsafeArray<Int64> newArr = MemAllocArr(Int64, this->capacity * 2);
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
		this->arr[--i] = arr->GetItem(j);
	}
	this->objCnt += arr->GetCount();
}

