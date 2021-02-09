#include "Stdafx.h"
#include "Sync/RWMutexUsage.h"

Sync::RWMutexUsage::RWMutexUsage(Sync::RWMutex *mut, Bool writing)
{
	this->mut = mut;
	this->used = false;
	this->BeginUse(writing);
}

Sync::RWMutexUsage::~RWMutexUsage()
{
	this->EndUse();
}

void Sync::RWMutexUsage::BeginUse(Bool writing)
{
	if (!this->used)
	{
		this->writing = writing;
		if (this->writing)
		{
			this->mut->LockWrite();
		}
		else
		{
			this->mut->LockRead();
		}
		this->used = true;
	}
}

void Sync::RWMutexUsage::EndUse()
{
	if (this->used)
	{
		if (this->writing)
		{
			this->mut->UnlockWrite();
		}
		else
		{
			this->mut->UnlockRead();
		}
		this->used = false;
	}
}
