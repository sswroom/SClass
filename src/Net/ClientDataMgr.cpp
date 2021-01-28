#include "Stdafx.h"
#include "MyMemory.h"
#include "Net/ClientDataMgr.h"
#include <memory.h>

Net::ClientDataMgr::ClientDataMgr(Int32 maxBuffSize)
{
	this->maxBuffSize = maxBuffSize;
	this->buff = MemAlloc(UInt8, maxBuffSize);
	this->currDataSize = 0;
	this->userData = 0;
}
Net::ClientDataMgr::~ClientDataMgr()
{
	MemFree(this->buff);
}

void Net::ClientDataMgr::AddData(UInt8 *buff, OSInt buffSize)
{
	if (this->currDataSize + buffSize > this->currDataSize)
	{
		this->currDataSize = 0;
		if (this->maxBuffSize < buffSize)
		{
			MemCopy(this->buff, &buff[buffSize - this->maxBuffSize], this->maxBuffSize);
			this->currDataSize = this->maxBuffSize;
		}
		else
		{
			MemCopy(this->buff, buff, buffSize);
			this->currDataSize = buffSize;
		}
	}
	else
	{
		MemCopy(&this->buff[this->currDataSize], buff, buffSize);
		this->currDataSize += buffSize;
	}
}

UInt8 *Net::ClientDataMgr::GetData(OSInt *dataSize)
{
	*dataSize = this->currDataSize;
	return this->buff;
}

void Net::ClientDataMgr::HandleData(OSInt handleSize)
{
	if (handleSize >= this->currDataSize)
	{
		this->currDataSize = 0;
	}
	else
	{
		MemCopy(this->buff, &this->buff[handleSize], this->currDataSize - handleSize);
		this->currDataSize -= handleSize;
	}
}
