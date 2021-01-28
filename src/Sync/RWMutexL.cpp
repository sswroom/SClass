#include "Stdafx.h"
#include "MyMemory.h"
#include "Sync/Event.h"
#include "Sync/Interlocked.h"
#include "Sync/RWMutex.h"
#include "Sync/Thread.h"

#include <pthread.h>
#include <stdlib.h>

Sync::RWMutex::RWMutex()
{
	Sync::Event *evt;
	this->writeTId = 0;
	this->readCnt = 0;
	NEW_CLASS(evt, Sync::Event(true, (const UTF8Char*)"Sync.RWMutex.hand"));
	this->hand = evt;
	this->mutHand = malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init((pthread_mutex_t*)this->mutHand, 0);
}

Sync::RWMutex::~RWMutex()
{
	Sync::Event *evt = (Sync::Event*)this->hand;
	DEL_CLASS(evt);
	free(this->mutHand);
}

void Sync::RWMutex::LockRead()
{
	Sync::Event *evt = (Sync::Event*)this->hand;
	Bool locked = false;
	while (!locked)
	{
		pthread_mutex_lock((pthread_mutex_t*)this->mutHand);
		if (this->writeTId == 0)
		{
			Sync::Interlocked::Increment(&this->readCnt);
			locked = true;
		}
		pthread_mutex_unlock((pthread_mutex_t*)this->mutHand);
		if (locked)
			break;
		evt->Wait(1000);
	}
}

void Sync::RWMutex::UnlockRead()
{
	if (Sync::Interlocked::Decrement(&this->readCnt) == 0)
	{
		Sync::Event *evt = (Sync::Event*)this->hand;
		evt->Set();
	}
}

void Sync::RWMutex::LockWrite()
{
	Sync::Event *evt = (Sync::Event*)this->hand;
	Bool locked = false;
	while (!locked)
	{
		pthread_mutex_lock((pthread_mutex_t*)this->mutHand);
		if (this->writeTId == 0)
		{
			this->writeTId = Sync::Thread::GetThreadId();
			locked = true;
		}
		pthread_mutex_unlock((pthread_mutex_t*)this->mutHand);
		if (locked)
			break;
		evt->Wait(1000);
	}
	while (this->readCnt)
	{
		evt->Wait(1000);
	}
}

void Sync::RWMutex::UnlockWrite()
{
	this->writeTId = 0;
	Sync::Event *evt = (Sync::Event*)this->hand;
	evt->Set();
}
