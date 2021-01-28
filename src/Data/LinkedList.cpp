#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/LinkedList.h"

Data::LinkedList::LinkedList()
{
	NEW_CLASS(this->mut, Sync::Mutex());
	this->firstItem = 0;
	this->lastItem = 0;
}

Data::LinkedList::~LinkedList()
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

Bool Data::LinkedList::HasItems()
{
	return this->firstItem != 0;
}

void Data::LinkedList::Put(void *item)
{
	this->mut->Lock();
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
	this->mut->Unlock();
}

void *Data::LinkedList::Get()
{
	Data::LinkedListItem *item;
	void *obj = 0;
	this->mut->Lock();
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
	this->mut->Unlock();
	return obj;
}

void *Data::LinkedList::GetNoRemove()
{
	void *obj = 0;
	this->mut->Lock();
	if (this->firstItem)
	{
		obj = this->firstItem->item;
	}
	this->mut->Unlock();
	return obj;
}

OSInt Data::LinkedList::GetCount()
{
	OSInt cnt = 0;
	Data::LinkedListItem *item = this->firstItem;
	while (item)
	{
		cnt++;
		item = item->nextItem;
	}
	return cnt;
}
