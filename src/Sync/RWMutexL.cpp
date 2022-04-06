#include "Stdafx.h"
#include "MyMemory.h"
#include "Sync/Event.h"
#include "Sync/Interlocked.h"
#include "Sync/RWMutex.h"
#include "Sync/Thread.h"

#include <pthread.h>
#include <stdlib.h>

struct Sync::RWMutex::ClassData
{
	pthread_cond_t evtCond;
	pthread_mutex_t evtMutex;
	pthread_mutex_t mutHand;
	Bool isSet;
};

void RWMutex_Wait(Sync::RWMutex::ClassData *clsData, UOSInt timeout)
{
	struct timespec outtime;
	pthread_mutex_lock(&clsData->evtMutex);
	if (clsData->isSet)
	{
		clsData->isSet = false;
		pthread_mutex_unlock(&clsData->evtMutex);
	}
	else
	{
		clock_gettime(CLOCK_REALTIME, &outtime);
		outtime.tv_sec += (time_t)(timeout / 1000);
		outtime.tv_nsec += (long)((timeout % 1000) * 1000000);
		if (outtime.tv_nsec >= 1000000000)
		{
			outtime.tv_nsec -= 1000000000;
			outtime.tv_sec++;
		}
		pthread_cond_timedwait(&clsData->evtCond, &clsData->evtMutex, &outtime);
		pthread_mutex_unlock(&clsData->evtMutex);
	}
}

Sync::RWMutex::RWMutex()
{
	this->writeTId = 0;
	this->readCnt = 0;
	this->clsData = MemAllocA(ClassData, 1);
	pthread_cond_init(&this->clsData->evtCond, 0);
	pthread_mutex_init(&this->clsData->evtMutex, 0);
	pthread_mutex_init(&this->clsData->mutHand, 0);
	this->clsData->isSet = false;
}

Sync::RWMutex::~RWMutex()
{
	pthread_mutex_lock(&this->clsData->evtMutex);
	pthread_cond_destroy(&this->clsData->evtCond);
	pthread_mutex_unlock(&this->clsData->evtMutex);
	pthread_mutex_destroy(&this->clsData->evtMutex);
	pthread_mutex_destroy(&this->clsData->mutHand);
	MemFreeA(this->clsData);
}

void Sync::RWMutex::LockRead()
{
	Bool locked = false;
	while (!locked)
	{
		pthread_mutex_lock(&this->clsData->mutHand);
		if (this->writeTId == 0)
		{
			Sync::Interlocked::Increment(&this->readCnt);
			locked = true;
		}
		pthread_mutex_unlock(&this->clsData->mutHand);
		if (locked)
			break;
		RWMutex_Wait(this->clsData, 1000);
	}
}

void Sync::RWMutex::UnlockRead()
{
	if (Sync::Interlocked::Decrement(&this->readCnt) == 0)
	{
		pthread_mutex_lock(&this->clsData->evtMutex);
		this->clsData->isSet = true;
		pthread_cond_signal(&this->clsData->evtCond);
		pthread_mutex_unlock(&this->clsData->evtMutex);
	}
}

void Sync::RWMutex::LockWrite()
{
	Bool locked = false;
	while (!locked)
	{
		pthread_mutex_lock(&this->clsData->evtMutex);
		if (this->writeTId == 0)
		{
			this->writeTId = Sync::Thread::GetThreadId();
			locked = true;
		}
		pthread_mutex_unlock(&this->clsData->evtMutex);
		if (locked)
			break;
		RWMutex_Wait(this->clsData, 1000);
	}
	while (this->readCnt)
	{
		RWMutex_Wait(this->clsData, 1000);
	}
}

void Sync::RWMutex::UnlockWrite()
{
	this->writeTId = 0;
//	Interlocked_IncrementU32(&status->useCnt);
	pthread_mutex_lock(&this->clsData->evtMutex);
	this->clsData->isSet = true;
	pthread_cond_signal(&this->clsData->evtCond);
	pthread_mutex_unlock(&this->clsData->evtMutex);
//	Interlocked_DecrementU32(&status->useCnt);
}
