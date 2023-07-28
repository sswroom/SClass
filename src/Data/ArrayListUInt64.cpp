#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListUInt64.h"
#include "Data/IComparable.h"

Data::ArrayListUInt64::ArrayListUInt64() : Data::SortableArrayListNative<UInt64>()
{
}

Data::ArrayListUInt64::ArrayListUInt64(UOSInt capacity) : Data::SortableArrayListNative<UInt64>(capacity)
{
}

NotNullPtr<Data::ArrayList<UInt64>> Data::ArrayListUInt64::Clone() const
{
	NotNullPtr<Data::ArrayList<UInt64>> newArr;
	NEW_CLASSNN(newArr, Data::ArrayListUInt64(this->capacity));
	newArr->AddAll(*this);
	return newArr;
}

void Data::ArrayListUInt64::AddRangeU32(const UInt32 *arr, UOSInt cnt)
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

void Data::ArrayListUInt64::AddRangeU32(const Data::ArrayList<UInt32> *arr)
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

