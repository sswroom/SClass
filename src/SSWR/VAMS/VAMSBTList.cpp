#include "Stdafx.h"
#include "Data/DateTime.h"
#include "SSWR/VAMS/VAMSBTList.h"
#include "Sync/MutexUsage.h"

SSWR::VAMS::VAMSBTList::VAMSBTList()
{
}

SSWR::VAMS::VAMSBTList::~VAMSBTList()
{
	Data::FastStringMap<AvlBleItem*> *item;
	AvlBleItem *bleItem;
	UOSInt i = this->itemMap.GetCount();
	UOSInt j;
	while (i-- > 0)
	{
		item = this->itemMap.GetItem(i);
		j = item->GetCount();
		while (j-- > 0)
		{
			bleItem = item->GetItem(j);
			bleItem->avlNo->Release();
			MemFree(bleItem);
		}
		DEL_CLASS(item);
	}
}

void SSWR::VAMS::VAMSBTList::AddItem(Text::String *avlNo, Int32 progId, Int64 ts, Int16 rssi)
{
	Data::DateTime dt;
	dt.SetCurrTimeUTC();
	if (ts > dt.ToTicks() + 30000)
	{
		return;
	}
	Sync::MutexUsage mutUsage(this->mut);
	Data::FastStringMap<AvlBleItem*> *progMap = this->itemMap.Get(progId);
	if (progMap == 0)
	{
		NEW_CLASS(progMap, Data::FastStringMap<AvlBleItem*>());
		this->itemMap.Put(progId, progMap);
	}
	AvlBleItem *item = progMap->Get(avlNo);
	if (item == 0)
	{
		item = MemAlloc(AvlBleItem, 1);
		item->avlNo = avlNo->Clone();
		item->progId = progId;
		item->lastDevTS = ts;
		item->rssi = rssi;
		item->lastProcTS = 0;
		progMap->Put(avlNo, item);
	}
	else if (item->lastDevTS < ts)
	{
		item->lastDevTS = ts;
		item->rssi = rssi;
	}
}

UOSInt SSWR::VAMS::VAMSBTList::QueryByProgId(Data::ArrayList<AvlBleItem *> *itemList, Int32 progId, Int32 timeoutIntervalMs)
{
	UOSInt ret = 0;
	AvlBleItem *item;
	Sync::MutexUsage mutUsage(this->mut);
	Data::FastStringMap<AvlBleItem *> *progMap = this->itemMap.Get(progId);
	if (progMap == 0)
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
		item = progMap->GetItem(i);
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


UOSInt SSWR::VAMS::VAMSBTList::GetProgList(Data::ArrayList<Int32> *progList)
{
	Sync::MutexUsage mutUsage(this->mut);
	return this->itemMap.AddKeysTo(progList);
}
