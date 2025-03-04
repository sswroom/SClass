#include "Stdafx.h"
#include "Sync/RWMutexUsage.h"

Sync::RWMutexUsage::RWMutexUsage()
{
	this->mut = 0;
	this->used = false;
	this->writing = false;
}

Sync::RWMutexUsage::RWMutexUsage(NN<const Sync::RWMutex> mut, Bool writing)
{
	this->mut = mut.Ptr();
	this->writing = writing;
	this->used = true;
	if (writing)
		((Sync::RWMutex*)this->mut)->LockWrite();
	else
		((Sync::RWMutex*)this->mut)->LockRead();
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
			((Sync::RWMutex*)this->mut)->LockWrite();
		}
		else
		{
			((Sync::RWMutex*)this->mut)->LockRead();
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
			((Sync::RWMutex*)this->mut)->UnlockWrite();
		}
		else
		{
			((Sync::RWMutex*)this->mut)->UnlockRead();
		}
		this->used = false;
	}
}

void Sync::RWMutexUsage::ReplaceMutex(NN<const Sync::RWMutex> mut, Bool writing)
{
	if (!this->used)
	{
		this->mut = mut.Ptr();
		this->BeginUse(writing);
	}
	else if (this->mut != mut.Ptr() || (!this->writing && writing))
	{
		this->EndUse();
		this->mut = mut.Ptr();
		this->BeginUse(writing);
	}
}
