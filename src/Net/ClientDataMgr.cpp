#include "Stdafx.h"
#include "MyMemory.h"
#include "Net/ClientDataMgr.h"

Net::ClientDataMgr::ClientDataMgr(UIntOS maxBuffSize)
{
	this->maxBuffSize = maxBuffSize;
	this->buff = MemAllocArr(UInt8, maxBuffSize);
	this->currDataSize = 0;
	this->userData = 0;
}
Net::ClientDataMgr::~ClientDataMgr()
{
	MemFreeArr(this->buff);
}

void Net::ClientDataMgr::AddData(UnsafeArray<UInt8> buff, UIntOS buffSize)
{
	if (this->currDataSize + buffSize > this->currDataSize)
	{
		this->currDataSize = 0;
		if (this->maxBuffSize < buffSize)
		{
			MemCopyNO(&this->buff[0], &buff[buffSize - this->maxBuffSize], this->maxBuffSize);
			this->currDataSize = this->maxBuffSize;
		}
		else
		{
			MemCopyNO(&this->buff[0], &buff[0], buffSize);
			this->currDataSize = buffSize;
		}
	}
	else
	{
		MemCopyNO(&this->buff[this->currDataSize], &buff[0], buffSize);
		this->currDataSize += buffSize;
	}
}

UnsafeArray<UInt8> Net::ClientDataMgr::GetData(OutParam<UIntOS> dataSize)
{
	dataSize.Set(this->currDataSize);
	return this->buff;
}

void Net::ClientDataMgr::HandleData(UIntOS handleSize)
{
	if (handleSize >= this->currDataSize)
	{
		this->currDataSize = 0;
	}
	else
	{
		MemCopyNO(&this->buff[0], &this->buff[handleSize], this->currDataSize - handleSize);
		this->currDataSize -= handleSize;
	}
}
