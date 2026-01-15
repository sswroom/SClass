#include "Stdafx.h"
#include "Data/DateTime.h"
#include "SSWR/VAMS/VAMSBTList.h"
#include "Sync/MutexUsage.h"

SSWR::VAMS::VAMSBTList::VAMSBTList()
{
}

SSWR::VAMS::VAMSBTList::~VAMSBTList()
{
	NN<Data::FastStringMapNN<AvlBleItem>> item;
	NN<AvlBleItem> bleItem;
	UOSInt i = this->itemMap.GetCount();
	UOSInt j;
	while (i-- > 0)
	{
		item = this->itemMap.GetItemNoCheck(i);
		j = item->GetCount();
		while (j-- > 0)
		{
			bleItem = item->GetItemNoCheck(j);
			bleItem->avlNo->Release();
			MemFreeNN(bleItem);
		}
		item.Delete();
	}
}

void SSWR::VAMS::VAMSBTList::AddItem(NN<Text::String> avlNo, Int32 progId, Int64 ts, Int16 rssi)
{
	Data::DateTime dt;
	dt.SetCurrTimeUTC();
	if (ts > dt.ToTicks() + 30000)
	{
		return;
	}
	Sync::MutexUsage mutUsage(this->mut);
	NN<Data::FastStringMapNN<AvlBleItem>> progMap;
	if (!this->itemMap.Get(progId).SetTo(progMap))
	{
		NEW_CLASSNN(progMap, Data::FastStringMapNN<AvlBleItem>());
		this->itemMap.Put(progId, progMap);
	}
	NN<AvlBleItem> item;
	if (!progMap->GetNN(avlNo).SetTo(item))
	{
		item = MemAllocNN(AvlBleItem);
		item->avlNo = avlNo->Clone();
		item->progId = progId;
		item->lastDevTS = ts;
		item->rssi = rssi;
		item->lastProcTS = 0;
		progMap->PutNN(avlNo, item);
	}
	else if (item->lastDevTS < ts)
	{
		item->lastDevTS = ts;
		item->rssi = rssi;
	}
}

UOSInt SSWR::VAMS::VAMSBTList::QueryByProgId(NN<Data::ArrayListNN<AvlBleItem>> itemList, Int32 progId, Int32 timeoutIntervalMs)
{
	UOSInt ret = 0;
	NN<AvlBleItem> item;
	Sync::MutexUsage mutUsage(this->mut);
	NN<Data::FastStringMapNN<AvlBleItem>> progMap;
	if (!this->itemMap.Get(progId).SetTo(progMap))
	{
		return 0;
	}
	Data::DateTime dt;
	dt.SetCurrTimeUTC();
	Int64 toTime = dt.ToTicks();
	Int64 fromTime = toTime - timeoutIntervalMs;
	UOSInt i = 0;
	UOSInt j = progMap->GetCount();
	while (i < j)
	{
		item = progMap->GetItemNoCheck(i);
		if (item->lastDevTS >= fromTime)
		{
			itemList->Add(item);
			ret++;
		}
		i++;
	}
	return ret;
}

void SSWR::VAMS::VAMSBTList::KARecv(Int32 progId)
{
	Data::DateTime dt;
	dt.SetCurrTimeUTC();
	Sync::MutexUsage mutUsage(this->mut);
	this->kaMap.Put(progId, dt.ToTicks());
}

Int64 SSWR::VAMS::VAMSBTList::GetLastKeepAlive(Int32 progId)
{
	Sync::MutexUsage mutUsage(this->mut);
	return this->kaMap.Get(progId);
}

Bool SSWR::VAMS::VAMSBTList::HasProg(Int32 progId)
{
	Sync::MutexUsage mutUsage(this->mut);
	return this->itemMap.ContainsKey(progId);
}


UOSInt SSWR::VAMS::VAMSBTList::GetProgList(NN<Data::ArrayListNative<Int32>> progList)
{
	Sync::MutexUsage mutUsage(this->mut);
	return this->itemMap.AddKeysTo(progList);
}
