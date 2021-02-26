#include "Stdafx.h"
#include "Sync/MutexUsage.h"

Sync::MutexUsage::MutexUsage()
{
	this->mut = 0;
	this->used = false;
}

Sync::MutexUsage::MutexUsage(Sync::Mutex *mut)
{
	this->mut = mut;
	this->used = false;
	if (this->mut)
	{
		this->used = true;
 		this->mut->Lock();
	}
}

Sync::MutexUsage::~MutexUsage()
{
	this->EndUse();
}

void Sync::MutexUsage::BeginUse()
{
	if (this->mut && !this->used)
	{
		this->mut->Lock();
		this->used = true;
	}
}

void Sync::MutexUsage::EndUse()
{
	if (this->mut && this->used)
	{
		this->mut->Unlock();
		this->used = false;
	}
}

void Sync::MutexUsage::ReplaceMutex(Sync::Mutex *mut)
{
	this->EndUse();
	this->mut = mut;
	this->BeginUse();
}
