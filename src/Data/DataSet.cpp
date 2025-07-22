#include "Stdafx.h"
#include "Data/DataSet.h"

Data::DataSet::DataSet()
{
	this->capacity = 128;
	this->itemCnt = 0;
	this->items = MemAllocArr(VariItem, this->capacity << 1);
}

Data::DataSet::~DataSet()
{
	UOSInt i = this->itemCnt << 1;
	while (i-- > 0)
	{
		this->items[i].SetNull();
	}
	MemFreeArr(this->items);
}

void Data::DataSet::AddItem(const VariItem& key, const VariItem& value)
{
	UOSInt i;
	if (this->itemCnt >= this->capacity)
	{
		this->capacity = this->capacity << 1;
		UnsafeArray<VariItem> newItems = MemAllocArr(VariItem, this->capacity << 1);
		i = 0;
		UOSInt j = this->itemCnt * 2;
		while (i < j)
		{
			newItems[i] = this->items[i];
			i++;
		}
		MemFreeArr(this->items);
		this->items = newItems;
	}
	i = this->itemCnt;
	this->items[i << 1] = key;
	this->items[(i << 1) + 1] = value;
	this->itemCnt = i + 1;
}
