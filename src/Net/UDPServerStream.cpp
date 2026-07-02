#include "Stdafx.h"
#include "Net/UDPServerStream.h"
#include "Sync/Interlocked.h"
#include "Sync/MutexUsage.h"
#include "Sync/ThreadUtil.h"
#include "Text/StringBuilder.hpp"

#define BUFFSIZE 2048

void __stdcall Net::UDPServerStream::OnUDPPacket(NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, Data::ByteArrayR data, AnyType userData)
{
	NN<Net::UDPServerStream> me = userData.GetNN<Net::UDPServerStream>();
	Sync::MutexUsage mutUsage(me->dataMut);
	me->lastAddr = addr.Ptr()[0];
	me->lastPort = port;
	if (data.GetSize() >= BUFFSIZE)
	{
		MemCopyNO(me->buff, &data[data.GetSize() - BUFFSIZE], BUFFSIZE);
		me->buffSize = BUFFSIZE;
	}
	else if (me->buffSize + data.GetSize() > BUFFSIZE)
	{
		MemCopyO(me->buff, &me->buff[me->buffSize - (BUFFSIZE - data.GetSize())], BUFFSIZE - data.GetSize());
		MemCopyNO(&me->buff[BUFFSIZE - data.GetSize()], &data[0], data.GetSize());
		me->buffSize = BUFFSIZE;
	}
	else
	{
		MemCopyNO(&me->buff[me->buffSize], &data[0], data.GetSize());
		me->buffSize += data.GetSize();
	}
	me->readEvt.Set();
}

Net::UDPServerStream::UDPServerStream(NN<Net::SocketFactory> sockf, UInt16 port, NN<IO::LogTool> log) : IO::Stream(CSTR("Net.UDPServerSream"))
{
	this->sockf = sockf;
	this->log = log;
	this->lastAddr.addrType = Net::AddrType::Unknown;
	this->lastPort = 0;
	this->buffSize = 0;
	this->isClient = false;
	this->buff = MemAlloc(UInt8, BUFFSIZE);
	this->svr = nullptr;
	NN<Net::UDPServer> svr;
	NEW_CLASSNN(svr, Net::UDPServer(sockf, nullptr, port, nullptr, OnUDPPacket, this, log, CSTR("UDPStm: "), 2, false));
	if (svr->IsError())
	{
		svr.Delete();
		return;
	}
	else
	{
		this->svr = svr;
	}
}

Net::UDPServerStream::~UDPServerStream()
{
	this->Close();
	MemFree(this->buff);
}

Bool Net::UDPServerStream::IsDown() const
{
	return this->svr.IsNull();
}

UIntOS Net::UDPServerStream::Read(const Data::ByteArray &buff)
{
	while (this->svr.NotNull() && this->buffSize == 0)
	{
		this->readEvt.Wait(10000);
	}
	if (this->svr.IsNull() || this->buffSize == 0)
		return 0;
	UIntOS ret;
	Sync::MutexUsage mutUsage(this->dataMut);
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

UIntOS Net::UDPServerStream::Write(Data::ByteArrayR buff)
{
	NN<Net::UDPServer> svr;
	Sync::MutexUsage mutUsage(this->dataMut);
	if (this->lastAddr.addrType == Net::AddrType::Unknown || !this->svr.SetTo(svr))
	{
		return 0;
	}
	if (svr->SendTo(this->lastAddr, this->lastPort, buff.Arr(), buff.GetSize()))
	{
		return buff.GetSize();
	}
	else
	{
		return 0;
	}
}

Int32 Net::UDPServerStream::Flush()
{
	return 0;
}

void Net::UDPServerStream::Close()
{
	this->svr.Delete();
	this->readEvt.Set();
}

Bool Net::UDPServerStream::Recover()
{
	return false;
}

Bool Net::UDPServerStream::IsError() const
{
	return this->svr.IsNull();
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