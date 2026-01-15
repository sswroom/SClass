#include "Stdafx.h"
#include "Data/DataSet.h"
#include "Data/VariItemHashCalc.h"
#include "Data/VariItemComparator.h"

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
	this->items[i << 1] = VariItem();
	this->items[i << 1].Set(key);
	if (key.GetItemType() == Data::VariItem::ItemType::CStr)
	{
		opts = key.GetAsNewString();
		this->items[i << 1].SetStr(opts);
		OPTSTR_DEL(opts);
	}
	this->items[(i << 1) + 1] = VariItem();
	this->items[(i << 1) + 1].Set(value);
	if (value.GetItemType() == Data::VariItem::ItemType::CStr)
	{
		opts = value.GetAsNewString();
		this->items[(i << 1) + 1].SetStr(opts);
		OPTSTR_DEL(opts);
	}
	this->itemCnt = i + 1;
}

void Data::DataSet::ValueCounts(NN<Data::ArrayListNative<UInt32>> result) const
{
	struct ValueItem
	{
		NN<VariItem> item;
		UInt32 count;
	};
	Data::VariItemHashCalc hashCalc;
	Data::Int64FastMapNN<ValueItem> itemMap;
	NN<ValueItem> vitem;
	Int64 hash;
	VariItem item;
	UOSInt i = 0;
	UOSInt j = this->GetCount();
	while (i < j)
	{
		if (this->GetValue(i, item))
		{
			hash = hashCalc.Hash(item);
			if (!itemMap.Get(hash).SetTo(vitem))
			{
				vitem = MemAllocNN(ValueItem);
				vitem->item = item.Clone();
				vitem->count = 1;
				itemMap.Put(hash, vitem);
			}
			else
			{
				while (!vitem->item->Equals(item))
				{
					hash++;
					if (!itemMap.Get(hash).SetTo(vitem))
					{
						vitem = MemAllocNN(ValueItem);
						vitem->item = item.Clone();
						vitem->count = 0;
						itemMap.Put(hash, vitem);
						break;
					}
				}
				vitem->count++;
			}
		}
		else
		{
			printf("Failed to get the value\r\n");
		}
		i++;
	}
	i = 0;
	j = itemMap.GetCount();
	while (i < j)
	{
		vitem = itemMap.GetItemNoCheck(i);
		result->Add(vitem->count);
		vitem->item.Delete();
		MemFreeNN(vitem);
		i++;
	}
}


NN<Data::DataSet> Data::DataSet::ValueCountsAsDS() const
{
	struct ValueItem
	{
		NN<VariItem> item;
		UInt32 count;
	};
	Data::VariItemHashCalc hashCalc;
	Data::Int64FastMapNN<ValueItem> itemMap;
	NN<ValueItem> vitem;
	Int64 hash;
	VariItem item;
	VariItem::ItemValue iv;
	UOSInt i = 0;
	UOSInt j = this->GetCount();
	while (i < j)
	{
		if (this->GetValue(i, item))
		{
			hash = hashCalc.Hash(item);
			if (!itemMap.Get(hash).SetTo(vitem))
			{
				vitem = MemAllocNN(ValueItem);
				vitem->item = item.Clone();
				vitem->count = 1;
				itemMap.Put(hash, vitem);
			}
			else
			{
				while (!vitem->item->Equals(item))
				{
					hash++;
					if (!itemMap.Get(hash).SetTo(vitem))
					{
						vitem = MemAllocNN(ValueItem);
						vitem->item = item.Clone();
						vitem->count = 0;
						itemMap.Put(hash, vitem);
						break;
					}
				}
				vitem->count++;
			}
		}
		else
		{
			printf("Failed to get the value\r\n");
		}
		i++;
	}
	NN<Data::DataSet> newDS;
	Data::VariItem cntItem;
	NEW_CLASSNN(newDS, Data::DataSet());
	i = 0;
	j = itemMap.GetCount();
	while (i < j)
	{
		vitem = itemMap.GetItemNoCheck(i);
		cntItem.SetU32(vitem->count);
		newDS->AddItem(vitem->item.Ptr()[0], cntItem);
		vitem->item.Delete();
		MemFreeNN(vitem);
		i++;
	}
	return newDS;
}

void Data::DataSet::SortByValue(NN<Data::Comparator<NN<VariItem>>> comparator)
{
	this->Sort(&this->items[1], this->items, 0, (OSInt)this->itemCnt - 1, comparator);
}

void Data::DataSet::SortByKey(NN<Data::Comparator<NN<VariItem>>> comparator)
{
	this->Sort(this->items, &this->items[1], 0, (OSInt)this->itemCnt - 1, comparator);
}

void Data::DataSet::SortByValue()
{
	VariItemComparator comparator;
	this->SortByValue(NN<VariItemComparator>(comparator));
}

void Data::DataSet::SortByValueInv()
{
	VariItemComparatorInv comparator;
	this->SortByValue(NN<VariItemComparatorInv>(comparator));
}

void Data::DataSet::SortByKey()
{
	VariItemComparator comparator;
	this->SortByKey(NN<VariItemComparator>(comparator));
}

void Data::DataSet::SortByKeyInv()
{
	VariItemComparatorInv comparator;
	this->SortByKey(NN<VariItemComparatorInv>(comparator));
}

void Data::DataSet::Presort(UnsafeArray<VariItem> keyArr, UnsafeArray<VariItem> valArr, OSInt left, OSInt right, NN<Data::Comparator<NN<VariItem>>> comparator)
{
	VariItem temp = keyArr[left * 2];
	VariItem temp2;
	VariItem v;
	while (left < right)
	{
		temp = keyArr[left * 2];
		temp2 = keyArr[right * 2];
		if (comparator->Compare(temp, temp2) > 0)
		{
			keyArr[left * 2] = temp2;
			keyArr[right * 2] = temp;
			v = valArr[left * 2];
			valArr[left * 2] = valArr[right * 2];
			valArr[right * 2] = v;
		}
		left++;
		right--;
	}
}

void Data::DataSet::Sort(UnsafeArray<VariItem> keyArr, UnsafeArray<VariItem> valArr, OSInt firstIndex, OSInt lastIndex, NN<Data::Comparator<NN<VariItem>>> comparator)
{
#if _OSINT_SIZE == 16
	OSInt levi[256];
	OSInt desni[256];
#else
	UnsafeArray<OSInt> levi = MemAllocArr(OSInt, 65536);
	UnsafeArray<OSInt> desni = &levi[32768];
#endif
	OSInt index;
	OSInt i;
	OSInt left;
	OSInt right;
	VariItem meja;
	VariItem mejaV;
	OSInt left1;
	OSInt right1;
	VariItem temp;
	VariItem tempV;

	this->Presort(keyArr, valArr, firstIndex, lastIndex, comparator);

	index = 0;
	levi[index] = firstIndex;
	desni[index] = lastIndex;

	while ( index >= 0 )
	{
		left = levi[index];
		right = desni[index];
		i = right - left;
		if (i <= 0)
		{
			index--;
		}
		else if (i <= 64)
		{
			this->ISortB(keyArr, valArr, left, right, comparator);
			index--;
		}
		else
		{
			meja = keyArr[ (left + right) & ~1 ];
			mejaV = valArr[ (left + right) & ~1 ];
			left1 = left;
			right1 = right;
			while (true)
			{
				while (comparator->Compare(keyArr[right1 * 2], meja) >= 0)
				{
					if (--right1 < left1)
						break;
				}
				while (comparator->Compare(keyArr[left1 * 2], meja) < 0)
				{
					if (++left1 > right1)
						break;
				}
				if (left1 > right1)
					break;

				temp = keyArr[right1 * 2];
				tempV = valArr[right1 * 2];
				keyArr[right1 * 2] = keyArr[left1 * 2];
				valArr[(right1--) * 2] = valArr[left1 * 2];
				keyArr[left1 * 2] = temp;
				valArr[(left1++) * 2] = tempV;
			}
			if (left1 == left)
			{
				keyArr[(left + right) & ~1] = keyArr[left * 2];
				valArr[(left + right) & ~1] = valArr[left * 2];
				keyArr[left * 2] = meja;
				valArr[left * 2] = mejaV;
				levi[index] = left + 1;
				desni[index] = right;
			}
			else
			{
				desni[index] = --left1;
				right1++;
				index++;
				levi[index] = right1;
				desni[index] = right;
			}
		}
	}
#if _OSINT_SIZE != 16
	MemFreeArr(levi);
#endif
}

void Data::DataSet::ISortB(UnsafeArray<VariItem> keyArr, UnsafeArray<VariItem> valArr, OSInt left, OSInt right, NN<Data::Comparator<NN<VariItem>>> comparator)
{
	OSInt i;
	OSInt j;
	OSInt k;
	OSInt l;
	VariItem temp;
	VariItem temp1;
	VariItem temp2;
	VariItem val2;
	temp1 = keyArr[left * 2];
	i = left + 1;
	while (i <= right)
	{
		temp2 = keyArr[i * 2];
		if ( comparator->Compare(temp1, temp2) > 0)
		{
			val2 = valArr[i * 2];
			j = left;
			k = i - 1;
			while (j <= k)
			{
				l = (j + k) >> 1;
				temp = keyArr[l * 2];
				if (comparator->Compare(temp, temp2) > 0)
				{
					k = l - 1;
				}
				else
				{
					j = l + 1;
				}
			}
			k = i;
			while (k > j)
			{
				k--;
				keyArr[k * 2 + 2] = keyArr[k * 2];
				valArr[k * 2 + 2] = valArr[k * 2];
			}
			keyArr[j * 2] = temp2;
			valArr[j * 2] = val2;
		}
		else
		{
			temp1 = temp2;
		}
		i++;
	}
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
