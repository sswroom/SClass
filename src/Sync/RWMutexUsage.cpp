#include "Stdafx.h"
#include "Sync/RWMutexUsage.h"

Sync::RWMutexUsage::RWMutexUsage()
{
	this->mut = 0;
	this->used = false;
	this->writing = false;
}

Sync::RWMutexUsage::RWMutexUsage(NotNullPtr<Sync::RWMutex> mut, Bool writing)
{
	this->mut = mut.Ptr();
	this->writing = writing;
	this->used = true;
	if (writing)
		this->mut->LockWrite();
	else
		this->mut->LockRead();
}

Sync::RWMutexUsage::~RWMutexUsage()
{
	this->EndUse();
}

void Sync::RWMutexUsage::BeginUse(Bool writing)
{
	if (!this->used && this->mut)
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

void Sync::RWMutexUsage::ReplaceMutex(NotNullPtr<Sync::RWMutex> mut, Bool writing)
{
	if (!this->used)
	{
		this->mut = mut.Ptr();
		this->BeginUse(writing);
	}
	else if (this->mut != mut.Ptr() || (writing != this->writing))
	{
		this->EndUse();
		this->mut = mut.Ptr();
		this->BeginUse(writing);
	}
}
