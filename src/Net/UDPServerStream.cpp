#include "Stdafx.h"
#include "Net/UDPServerStream.h"
#include "Sync/Interlocked.h"
#include "Sync/MutexUsage.h"
#include "Sync/Thread.h"
#include "Text/StringBuilder.h"
#define BUFFSIZE 2048

void __stdcall Net::UDPServerStream::OnUDPPacket(const Net::SocketUtil::AddressInfo *addr, UInt16 port, const UInt8 *buff, UOSInt dataSize, void *userData)
{
	Net::UDPServerStream *me = (Net::UDPServerStream*)userData;
	Sync::MutexUsage mutUsage(me->dataMut);
	me->lastAddr = *addr;
	me->lastPort = port;
	if (dataSize >= BUFFSIZE)
	{
		MemCopyNO(me->buff, &buff[dataSize - BUFFSIZE], BUFFSIZE);
		me->buffSize = BUFFSIZE;
	}
	else if (me->buffSize + dataSize > BUFFSIZE)
	{
		MemCopyO(me->buff, &me->buff[me->buffSize - (BUFFSIZE - dataSize)], BUFFSIZE - dataSize);
		MemCopyNO(&me->buff[BUFFSIZE - dataSize], buff, dataSize);
		me->buffSize = BUFFSIZE;
	}
	else
	{
		MemCopyNO(&me->buff[me->buffSize], buff, dataSize);
		me->buffSize += dataSize;
	}
}

Net::UDPServerStream::UDPServerStream(Net::SocketFactory *sockf, Int32 port, IO::LogTool *log) : IO::Stream((const UTF8Char*)"Net.UDPServerSream")
{
	this->sockf = sockf;
	this->log = log;
	this->lastAddr.addrType = Net::SocketUtil::AT_UNKNOWN;
	this->lastPort = 0;
	this->buffSize = 0;
	this->buff = MemAlloc(UInt8, BUFFSIZE);
	NEW_CLASS(this->readEvt, Sync::Event(true, (const UTF8Char*)"Net.UDPServerStream.readEvt"));
	NEW_CLASS(this->dataMut, Sync::Mutex());
	NEW_CLASS(this->svr, Net::UDPServer(sockf, 0, port, 0, OnUDPPacket, this, log, (const UTF8Char*)"UDPStm: ", 2, false));
	if (this->svr->IsError())
	{
		SDEL_CLASS(this->svr);
		return;
	}
}

Net::UDPServerStream::~UDPServerStream()
{
	this->Close();
	DEL_CLASS(this->dataMut);
	DEL_CLASS(this->readEvt);
	MemFree(this->buff);
}

UOSInt Net::UDPServerStream::Read(UInt8 *buff, UOSInt size)
{
	while (this->svr != 0 && this->buffSize == 0)
	{
		this->readEvt->Wait(10000);
	}
	if (this->svr == 0 || this->buffSize == 0)
		return 0;
	UOSInt ret;
	Sync::MutexUsage mutUsage(this->dataMut);
	if (this->buffSize > size)
	{
		MemCopyNO(buff, this->buff, size);
		MemCopyNO(this->buff, &this->buff[size], this->buffSize - size);
		this->buffSize -= size;
		ret = size;
	}
	else
	{
		MemCopyNO(buff, this->buff, this->buffSize);
		ret = this->buffSize;
		this->buffSize = 0;
	}
	return ret;
}

UOSInt Net::UDPServerStream::Write(const UInt8 *buff, UOSInt size)
{
	Sync::MutexUsage mutUsage(this->dataMut);
	if (this->lastAddr.addrType == Net::SocketUtil::AT_UNKNOWN)
	{
		return 0;
	}
	this->svr->SendTo(&this->lastAddr, this->lastPort, buff, size);
	return size;
}

Int32 Net::UDPServerStream::Flush()
{
	return 0;
}

void Net::UDPServerStream::Close()
{
	SDEL_CLASS(this->svr);
}

Bool Net::UDPServerStream::Recover()
{
	return false;
}

Bool Net::UDPServerStream::IsError()
{
	return this->svr == 0;
}
