#include "Stdafx.h"
#include "MyMemory.h"
#include "Sync/Interlocked.h"
#include "Sync/RWMutex.h"
#include "Sync/Thread.h"

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>

struct Sync::RWMutex::ClassData
{
	HANDLE hand;
	CRITICAL_SECTION mutHand;
};

Sync::RWMutex::RWMutex()
{
	this->writeTId = 0;
	this->readCnt = 0;
	this->clsData = MemAlloc(ClassData, 1);
	this->clsData->hand = CreateEvent(0, FALSE, FALSE, 0);
	InitializeCriticalSection(&this->clsData->mutHand);
}

Sync::RWMutex::~RWMutex()
{
	CloseHandle(this->clsData->hand);
	DeleteCriticalSection(&this->clsData->mutHand);
	MemFree(this->clsData);
}

void Sync::RWMutex::LockRead()
{
	Bool locked = false;
	while (!locked)
	{
		EnterCriticalSection(&this->clsData->mutHand);
		if (this->writeTId == 0)
		{
			Sync::Interlocked::Increment(&this->readCnt);
			locked = true;
		}
		LeaveCriticalSection(&this->clsData->mutHand);
		if (locked)
			break;
		WaitForSingleObject(this->clsData->hand, 1000);
	}
}

void Sync::RWMutex::UnlockRead()
{
	if (Sync::Interlocked::Decrement(&this->readCnt) == 0)
	{
		SetEvent(this->clsData->hand);
	}
}

void Sync::RWMutex::LockWrite()
{
	Bool locked = false;
	while (!locked)
	{
		EnterCriticalSection(&this->clsData->mutHand);
		if (this->writeTId == 0)
		{
			this->writeTId = Sync::Thread::GetThreadId();
			locked = true;
		}
		LeaveCriticalSection(&this->clsData->mutHand);
		if (locked)
			break;
		WaitForSingleObject(this->clsData->hand, 1000);
	}
	while (this->readCnt)
	{
		WaitForSingleObject(this->clsData->hand, 1000);
	}
}

void Sync::RWMutex::UnlockWrite()
{
	this->writeTId = 0;
	SetEvent(this->clsData->hand);
}
