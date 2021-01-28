#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/SyncArrayList.h"

Data::SyncArrayList::SyncArrayList()
{
	NEW_CLASS(arr, Data::ArrayList<void*>());
	NEW_CLASS(mut, Sync::Mutex());
}

Data::SyncArrayList::SyncArrayList(OSInt Capacity)
{
	NEW_CLASS(arr, Data::ArrayList<void*>(Capacity));
	NEW_CLASS(mut, Sync::Mutex());
}

Data::SyncArrayList::~SyncArrayList()
{
	DEL_CLASS(arr);
	DEL_CLASS(mut);
}

void Data::SyncArrayList::Add(void *val)
{
	mut->Lock();
	arr->Add(val);
	mut->Unlock();
}

void Data::SyncArrayList::AddRange(ArrayList<void*> *arr)
{
	mut->Lock();
	arr->AddRange(arr);
	mut->Unlock();
}

void *Data::SyncArrayList::RemoveAt(OSInt Index)
{
	void *ret;
	mut->Lock();
	ret = arr->RemoveAt(Index);
	mut->Unlock();
	return ret;
}

void Data::SyncArrayList::Insert(OSInt Index, void *Val)
{
	mut->Lock();
	arr->Insert(Index, Val);
	mut->Unlock();
}

OSInt Data::SyncArrayList::IndexOf(void *Val)
{
	OSInt ret;
	mut->Lock();
	ret = arr->IndexOf(Val);
	mut->Unlock();
	return ret;
}

void Data::SyncArrayList::Clear()
{
	mut->Lock();
	arr->Clear();
	mut->Unlock();
}

Data::SyncArrayList *Data::SyncArrayList::Clone()
{
	Data::SyncArrayList *newArr;
	mut->Lock();
	NEW_CLASS(newArr, Data::SyncArrayList(arr->GetCapacity()));
	newArr->AddRange(arr);
	mut->Unlock();
	return newArr;
}

OSInt Data::SyncArrayList::GetCount()
{
	return arr->GetCount();
}

OSInt Data::SyncArrayList::GetCapacity()
{
	return arr->GetCapacity();
}

void *Data::SyncArrayList::GetItem(OSInt Index)
{
	void *ret;
	mut->Lock();
	ret = arr->GetItem(Index);
	mut->Unlock();
	return ret;
}

void Data::SyncArrayList::SetItem(OSInt Index, void *Val)
{
	mut->Lock();
	arr->SetItem(Index, Val);
	mut->Unlock();
}
