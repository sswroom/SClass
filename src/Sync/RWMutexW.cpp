#include "Stdafx.h"
#include "MyMemory.h"
#include "Sync/Interlocked.h"
#include "Sync/RWMutex.h"
#include "Sync/Thread.h"

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
Sync::RWMutex::RWMutex()
{
	this->writeTId = 0;
	this->readCnt = 0;
	this->hand = CreateEvent(0, FALSE, FALSE, 0);
	this->mutHand = malloc(sizeof(CRITICAL_SECTION));
	InitializeCriticalSection((LPCRITICAL_SECTION)this->mutHand);
}

Sync::RWMutex::~RWMutex()
{
	CloseHandle(this->hand);
	DeleteCriticalSection((LPCRITICAL_SECTION)this->mutHand);
	free(this->mutHand);
}

void Sync::RWMutex::LockRead()
{
	Bool locked = false;
	while (!locked)
	{
		EnterCriticalSection((LPCRITICAL_SECTION)this->mutHand);
		if (this->writeTId == 0)
		{
			Sync::Interlocked::Increment(&this->readCnt);
			locked = true;
		}
		LeaveCriticalSection((LPCRITICAL_SECTION)this->mutHand);
		if (locked)
			break;
		WaitForSingleObject(this->hand, 1000);
	}
}

void Sync::RWMutex::UnlockRead()
{
	if (Sync::Interlocked::Decrement(&this->readCnt) == 0)
	{
		SetEvent(this->hand);
	}
}

void Sync::RWMutex::LockWrite()
{
	Bool locked = false;
	while (!locked)
	{
		EnterCriticalSection((LPCRITICAL_SECTION)this->mutHand);
		if (this->writeTId == 0)
		{
			this->writeTId = Sync::Thread::GetThreadId();
			locked = true;
		}
		LeaveCriticalSection((LPCRITICAL_SECTION)this->mutHand);
		if (locked)
			break;
		WaitForSingleObject(this->hand, 1000);
	}
	while (this->readCnt)
	{
		WaitForSingleObject(this->hand, 1000);
	}
}

void Sync::RWMutex::UnlockWrite()
{
	this->writeTId = 0;
	SetEvent(this->hand);
}
