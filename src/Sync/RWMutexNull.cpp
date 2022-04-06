#include "Stdafx.h"
#include "MyMemory.h"
#include "Sync/Event.h"
#include "Sync/Interlocked.h"
#include "Sync/RWMutex.h"
#include "Sync/Thread.h"

Sync::RWMutex::RWMutex()
{
	this->writeTId = 0;
	this->readCnt = 0;
	this->clsData = 0;
}

Sync::RWMutex::~RWMutex()
{
}

void Sync::RWMutex::LockRead()
{
}

void Sync::RWMutex::UnlockRead()
{
}

void Sync::RWMutex::LockWrite()
{
}

void Sync::RWMutex::UnlockWrite()
{
}
