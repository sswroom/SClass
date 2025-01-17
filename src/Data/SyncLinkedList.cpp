#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/SyncLinkedList.h"
#include "Sync/MutexUsage.h"

Data::SyncLinkedList::SyncLinkedList()
{
	this->firstItem = 0;
	this->lastItem = 0;
}

Data::SyncLinkedList::~SyncLinkedList()
{
	Optional<Data::LinkedListItem> item;
	Optional<Data::LinkedListItem> item2;
	NN<Data::LinkedListItem> nnitem;
	item = this->firstItem;
	while (item.SetTo(nnitem))
	{
		item2 = nnitem->nextItem;
		MemFreeNN(nnitem);
		item = item2;
	}
	this->lastItem = 0;
	this->firstItem = 0;
}

Bool Data::SyncLinkedList::HasItems()
{
	return this->firstItem.NotNull();
}

void Data::SyncLinkedList::Put(AnyType item)
{
	NN<Data::LinkedListItem> nnitem;
	NN<Data::LinkedListItem> lastItem;
	Sync::MutexUsage mutUsage(this->mut);
	if (this->lastItem.SetTo(nnitem))
	{
		nnitem->nextItem = lastItem = MemAllocNN(Data::LinkedListItem);
		this->lastItem = lastItem;
	}
	else
	{
		this->firstItem = this->lastItem = lastItem = MemAllocNN(Data::LinkedListItem);
	}
	lastItem->item = item;
	lastItem->nextItem = 0;
}

AnyType Data::SyncLinkedList::Get()
{
	NN<Data::LinkedListItem> item;
	AnyType obj = 0;
	Sync::MutexUsage mutUsage(this->mut);
	if (this->firstItem.SetTo(item))
	{
		if ((this->firstItem = item->nextItem).IsNull())
		{
			this->lastItem = 0;
		}
		mutUsage.EndUse();
		obj = item->item;
		MemFreeNN(item);
	}
	return obj;
}

AnyType Data::SyncLinkedList::GetNoRemove()
{
	NN<Data::LinkedListItem> item;
	AnyType obj = 0;
	Sync::MutexUsage mutUsage(this->mut);
	if (this->firstItem.SetTo(item))
	{
		obj = item->item;
	}
	return obj;
}

AnyType Data::SyncLinkedList::GetLastNoRemove()
{
	NN<Data::LinkedListItem> item;
	AnyType obj = 0;
	Sync::MutexUsage mutUsage(this->mut);
	if (this->lastItem.SetTo(item))
	{
		obj = item->item;
	}
	return obj;
}

UOSInt Data::SyncLinkedList::GetCount()
{
	UOSInt cnt = 0;
	Sync::MutexUsage mutUsage(this->mut);
	Optional<Data::LinkedListItem> item = this->firstItem;
	NN<LinkedListItem> nnitem;
	while (item.SetTo(nnitem))
	{
		cnt++;
		item = nnitem->nextItem;
	}
	return cnt;
}

UOSInt Data::SyncLinkedList::IndexOf(AnyType item)
{
	UOSInt cnt = 0;
	Sync::MutexUsage mutUsage(this->mut);
	Optional<Data::LinkedListItem> llItem = this->firstItem;
	NN<LinkedListItem> nnitem;
	while (llItem.SetTo(nnitem))
	{
		if (nnitem->item == item)
		{
			return cnt;
		}
		cnt++;
		llItem = nnitem->nextItem;
	}
	return INVALID_INDEX;
}
