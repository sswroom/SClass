#include "Stdafx.h"
#include "Net/UDPServerStream.h"
#include "Sync/Interlocked.h"
#include "Sync/MutexUsage.h"
#include "Sync/ThreadUtil.h"
#include "Text/StringBuilder.h"

#define BUFFSIZE 2048

void __stdcall Net::UDPServerStream::OnUDPPacket(const Net::SocketUtil::AddressInfo *addr, UInt16 port, const UInt8 *buff, UOSInt dataSize, void *userData)
{
	Net::UDPServerStream *me = (Net::UDPServerStream*)userData;
	Sync::MutexUsage mutUsage(&me->dataMut);
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
	me->readEvt.Set();
}

Net::UDPServerStream::UDPServerStream(Net::SocketFactory *sockf, UInt16 port, IO::LogTool *log) : IO::Stream(CSTR("Net.UDPServerSream"))
{
	this->sockf = sockf;
	this->log = log;
	this->lastAddr.addrType = Net::AddrType::Unknown;
	this->lastPort = 0;
	this->buffSize = 0;
	this->isClient = false;
	this->buff = MemAlloc(UInt8, BUFFSIZE);
	NEW_CLASS(this->svr, Net::UDPServer(sockf, 0, port, CSTR_NULL, OnUDPPacket, this, log, CSTR("UDPStm: "), 2, false));
	if (this->svr->IsError())
	{
		SDEL_CLASS(this->svr);
		return;
	}
}

Net::UDPServerStream::~UDPServerStream()
{
	this->Close();
	MemFree(this->buff);
}

Bool Net::UDPServerStream::IsDown() const
{
	return this->svr == 0;
}

UOSInt Net::UDPServerStream::Read(Data::ByteArray buff)
{
	while (this->svr != 0 && this->buffSize == 0)
	{
		this->readEvt.Wait(10000);
	}
	if (this->svr == 0 || this->buffSize == 0)
		return 0;
	UOSInt ret;
	Sync::MutexUsage mutUsage(&this->dataMut);
	if (this->buffSize > buff.GetSize())
	{
		buff.CopyFrom(Data::ByteArrayR(this->buff, buff.GetSize()));
		MemCopyO(this->buff, &this->buff[buff.GetSize()], this->buffSize - buff.GetSize());
		this->buffSize -= buff.GetSize();
		ret = buff.GetSize();
	}
	else
	{
		buff.CopyFrom(Data::ByteArrayR(this->buff, this->buffSize));
		ret = this->buffSize;
		this->buffSize = 0;
	}
	return ret;
}

UOSInt Net::UDPServerStream::Write(const UInt8 *buff, UOSInt size)
{
	Sync::MutexUsage mutUsage(&this->dataMut);
	if (this->lastAddr.addrType == Net::AddrType::Unknown)
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
	this->readEvt.Set();
}

Bool Net::UDPServerStream::Recover()
{
	return false;
}

Bool Net::UDPServerStream::IsError() const
{
	return this->svr == 0;
}

IO::StreamType Net::UDPServerStream::GetStreamType() const
{
	return this->isClient?IO::StreamType::UDPClient:IO::StreamType::UDPServer;
}

void Net::UDPServerStream::SetClientAddr(const Net::SocketUtil::AddressInfo *addr, UInt16 port)
{
	this->lastAddr = *addr;
	this->lastPort = port;
	this->isClient = true;
}