#include "Stdafx.h"
#include "Sync/MutexUsage.h"

Sync::MutexUsage::MutexUsage(Sync::Mutex *mut)
{
	this->mut = mut;
	this->used = true;
	if (this->mut) this->mut->Use();
}

Sync::MutexUsage::~MutexUsage()
{
	this->EndUse();
}

void Sync::MutexUsage::BeginUse()
{
	if (this->mut && !this->used)
	{
		this->mut->Use();
		this->used = true;
	}
}

void Sync::MutexUsage::EndUse()
{
	if (this->mut && this->used)
	{
		this->mut->Unuse();
		this->used = false;
	}
}
