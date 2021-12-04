#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/SyncLinkedList.h"
#include "Sync/MutexUsage.h"

Data::SyncLinkedList::SyncLinkedList()
{
	NEW_CLASS(this->mut, Sync::Mutex());
	this->firstItem = 0;
	this->lastItem = 0;
}

Data::SyncLinkedList::~SyncLinkedList()
{
	Data::LinkedListItem *item;
	Data::LinkedListItem *item2;
	item = this->firstItem;
	while (item)
	{
		item2 = item->nextItem;
		MemFree(item);
		item = item2;
	}
	this->lastItem = 0;
	this->firstItem = 0;
	DEL_CLASS(this->mut);
}

Bool Data::SyncLinkedList::HasItems()
{
	return this->firstItem != 0;
}

void Data::SyncLinkedList::Put(void *item)
{
	Sync::MutexUsage mutUsage(this->mut);
	if (this->lastItem)
	{
		this->lastItem->nextItem = MemAlloc(Data::LinkedListItem, 1);
		this->lastItem = this->lastItem->nextItem;
	}
	else
	{
		this->firstItem = this->lastItem = MemAlloc(Data::LinkedListItem, 1);
	}
	this->lastItem->item = item;
	this->lastItem->nextItem = 0;
}

void *Data::SyncLinkedList::Get()
{
	Data::LinkedListItem *item;
	void *obj = 0;
	Sync::MutexUsage mutUsage(this->mut);
	if (this->firstItem)
	{
		item = this->firstItem;
		if ((this->firstItem = item->nextItem) == 0)
		{
			this->lastItem = 0;
		}
		obj = item->item;
		MemFree(item);
	}
	return obj;
}

void *Data::SyncLinkedList::GetNoRemove()
{
	void *obj = 0;
	Sync::MutexUsage mutUsage(this->mut);
	if (this->firstItem)
	{
		obj = this->firstItem->item;
	}
	return obj;
}

void *Data::SyncLinkedList::GetLastNoRemove()
{
	void *obj = 0;
	Sync::MutexUsage mutUsage(this->mut);
	if (this->lastItem)
	{
		obj = this->lastItem->item;
	}
	return obj;
}

UOSInt Data::SyncLinkedList::GetCount()
{
	UOSInt cnt = 0;
	Sync::MutexUsage mutUsage(this->mut);
	Data::LinkedListItem *item = this->firstItem;
	while (item)
	{
		cnt++;
		item = item->nextItem;
	}
	return cnt;
}

UOSInt Data::SyncLinkedList::IndexOf(void *item)
{
	UOSInt cnt = 0;
	Sync::MutexUsage mutUsage(this->mut);
	Data::LinkedListItem *llItem = this->firstItem;
	while (llItem)
	{
		if (llItem->item == item)
		{
			mutUsage.EndUse();
			return cnt;
		}
		cnt++;
		llItem = llItem->nextItem;
	}
	return INVALID_INDEX;
}
