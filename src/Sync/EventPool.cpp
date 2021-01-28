#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayList.h"
#include "Sync/EventPool.h"
#include <windows.h>

Sync::EventPool::EventPool()
{
	NEW_CLASS(this->handList, Data::ArrayList<void*>());
	NEW_CLASS(this->evtList, Data::ArrayList<Sync::Event*>());
	NEW_CLASS(this->objList, Data::ArrayList<void*>());
	NEW_CLASS(this->mut, Sync::Mutex());
	NEW_CLASS(this->mainEvt, Sync::Event(true, (const UTF8Char*)"Sync.EventPool.mainEvt"));
	this->state = 0;
	this->handList->Add(this->mainEvt->GetHandle());
	this->evtList->Add(this->mainEvt);
	this->objList->Add(0);
}

Sync::EventPool::~EventPool()
{
	this->state = 2;
	this->mainEvt->Set();
	this->mut->Lock();
	DEL_CLASS(this->mainEvt);
	this->mut->Unlock();
	DEL_CLASS(this->mut);
	DEL_CLASS(this->objList);
	DEL_CLASS(this->evtList);
	DEL_CLASS(this->handList);
}

void Sync::EventPool::AddEvent(Sync::Event *evt, void *obj)
{
	this->state = 1;
	this->mainEvt->Set();
	this->mut->Lock();
	this->handList->Add(evt->GetHandle());
	this->evtList->Add(evt);
	this->objList->Add(obj);
	this->mut->Unlock();
	this->state = 0;
	this->mainEvt->Set();
}

void Sync::EventPool::RemoveEvent(Sync::Event *evt)
{
	OSInt i;
	this->state = 1;
	this->mainEvt->Set();
	this->mut->Lock();
	i = this->evtList->GetCount();
	while (i-- > 0)
	{
		if (this->evtList->GetItem(i) == evt)
		{
			this->handList->RemoveAt(i);
			this->evtList->RemoveAt(i);
			this->objList->RemoveAt(i);
			break;
		}
	}
	this->mut->Unlock();
	this->state = 0;
	this->mainEvt->Set();
}

void *Sync::EventPool::Wait(Int32 timeout)
{
	UOSInt cnt;
	void **handArr;
	DWORD ret;
	while (true)
	{
		while (this->state == 1)
		{
			this->mainEvt->Wait();
		}
		if (this->state != 0)
		{
			return 0;
		}
		this->mut->Lock();
		if (this->state == 0)
		{
			handArr = this->handList->GetArray(&cnt);
			if (cnt > MAXIMUM_WAIT_OBJECTS)
			{
				cnt = MAXIMUM_WAIT_OBJECTS;
			}
			ret = WaitForMultipleObjects((DWORD)cnt, (HANDLE*)handArr, FALSE, timeout);
			if (ret == WAIT_TIMEOUT)
			{
				this->mut->Unlock();
				return 0;
			}
			else if (ret == WAIT_OBJECT_0)
			{
				this->mut->Unlock();
				continue;
			}
			else if (ret > WAIT_OBJECT_0 && ret < WAIT_OBJECT_0 + cnt)
			{
				void *obj = this->objList->GetItem(ret - WAIT_OBJECT_0);
				this->mut->Unlock();
				return obj;
			}
			else if (ret >= WAIT_ABANDONED_0 && ret < WAIT_ABANDONED_0 + cnt)
			{
				this->mut->Unlock();
				return 0;
			}
			else if (ret == WAIT_FAILED)
			{
				UInt32 err = GetLastError();
				this->mut->Unlock();
				return 0;
			}
			else
			{
				this->mut->Unlock();
				return 0;
			}
		}
		else
		{
			this->mut->Unlock();
		}
	}
}
