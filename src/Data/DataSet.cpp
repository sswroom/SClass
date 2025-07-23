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
	Optional<Text::String> opts;
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
	if (key.GetItemType() == Data::VariItem::ItemType::CStr)
	{
		opts = key.GetAsNewString();
		this->items[i << 1].SetStr(opts);
		OPTSTR_DEL(opts);
	}
	this->items[(i << 1) + 1] = value;
	if (value.GetItemType() == Data::VariItem::ItemType::CStr)
	{
		opts = value.GetAsNewString();
		this->items[(i << 1) + 1].SetStr(opts);
		OPTSTR_DEL(opts);
	}
	this->itemCnt = i + 1;
}


	NN<Data::SortableArrayListNative<Data::Timestamp>> Data::DataSetMonthGrouper::CreateKeyIndex() const
	{
		NN<Data::SortableArrayListNative<Data::Timestamp>> tsList;
		UOSInt i = this->ds->GetCount();
		NEW_CLASSNN(tsList, Data::SortableArrayListNative<Data::Timestamp>());
		if (i == 0)
		{
			return tsList;
		}
		Bool hasNull = false;
		Data::Timestamp min = nullptr;
		Data::Timestamp max = nullptr;
		Data::Timestamp ts;
		Data::VariItem item;
		while (i-- > 0)
		{
			if (this->ds->GetKey(i, item))
			{
				ts = item.GetAsTimestamp();
				if (ts.IsNull())
				{
					hasNull = true;
				}
				else if (min.IsNull())
				{
					min = ts;
					max = ts;
				}
				else
				{
					if (ts < min)
					{
						min = ts;
					}
					if (ts > max)
					{
						max = ts;
					}
				}
			}
		}
		if (hasNull)
		{
			tsList->Add(nullptr);
		}
		if (!min.IsNull())
		{
			ts = min.ClearDayOfMonth();
			while (ts <= max)
			{
				tsList->Add(ts);
				ts = ts.AddMonth(1);
			}
		}
		return tsList;
	}

	UOSInt Data::DataSetMonthGrouper::GetKeyIndex(NN<Data::SortableArrayListNative<Data::Timestamp>> keyIndex, UOSInt dataIndex) const
	{
		VariItem item;
		if (!this->ds->GetKey(dataIndex, item))
			return 0;
		Data::Timestamp ts = item.GetAsTimestamp();
		OSInt i = keyIndex->SortedIndexOf(ts);
		if (i == -1)
			return 0;
		if (i < 0)
			return (UOSInt)~i - 1;
		return (UOSInt)i;
	}
