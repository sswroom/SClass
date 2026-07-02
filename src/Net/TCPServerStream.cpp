#include "Stdafx.h"
#include "Net/TCPServerStream.h"
#include "Sync/Interlocked.h"
#include "Sync/MutexUsage.h"
#include "Sync/ThreadUtil.h"
#include "Text/StringBuilder.hpp"

void __stdcall Net::TCPServerStream::ConnHandler(NN<Socket> s, AnyType userObj)
{
	NN<Net::TCPServerStream> me = userObj.GetNN<Net::TCPServerStream>();
	NN<Net::TCPClient> cli;
	NEW_CLASSNN(cli, Net::TCPClient(me->sockf, s));
	Sync::MutexUsage connMutUsage(me->connMut);
	NN<Net::TCPClient> currCli;
	if (me->currCli.SetTo(currCli))
	{
		currCli->Close();
		Sync::MutexUsage mutUsage(me->readMut);
		currCli.Delete();
		me->currCli = cli;
	}
	else
	{
		Sync::MutexUsage mutUsage(me->readMut);
		me->currCli = cli;
	}
	connMutUsage.EndUse();
	me->readEvt.Set();
}

Net::TCPServerStream::TCPServerStream(NN<Net::SocketFactory> sockf, UInt16 port, NN<IO::LogTool> log) : IO::Stream(CSTR("Net.TCPBoardcastSream"))
{
	this->sockf = sockf;
	this->log = log;
	this->currCli = nullptr;
	NN<Net::TCPServer> svr;
	NEW_CLASSNN(svr, Net::TCPServer(sockf, nullptr, port, log, ConnHandler, this, CSTR("SStm: "), true));
	if (svr->IsV4Error())
	{
		svr.Delete();
		this->svr = nullptr;
	}
	else
	{
		this->svr = svr;
	}
}

Net::TCPServerStream::~TCPServerStream()
{
	this->Close();
}

Bool Net::TCPServerStream::IsDown() const
{
	if (this->svr.IsNull())
		return true;
	if (this->currCli.IsNull())
		return true;
	return false;
}

UIntOS Net::TCPServerStream::Read(const Data::ByteArray &buff)
{
	Bool toClose = false;
	UIntOS readSize = 0;
	NN<Net::TCPClient> currCli;
	while (this->svr.NotNull())
	{
		Sync::MutexUsage readMutUsage(this->readMut);
		if (this->currCli.SetTo(currCli))
		{
			readSize = currCli->Read(buff);
			if (readSize == 0)
				toClose = true;
		}
		else
		{
		}
		readMutUsage.EndUse();
		if (toClose)
		{
			Sync::MutexUsage mutUsage(this->connMut);
			readMutUsage.BeginUse();
			this->currCli.Delete();
			readMutUsage.EndUse();
			mutUsage.EndUse();
		}
		if (readSize != 0)
		{
			return readSize;
		}
		this->readEvt.Wait(100);
	}
	return 0;
}

UIntOS Net::TCPServerStream::Write(Data::ByteArrayR buff)
{
	Bool toClose = false;
	Optional<Net::TCPClient> cli = nullptr;
	NN<Net::TCPClient> currCli;
	Sync::MutexUsage mutUsage(this->connMut);
	UIntOS writeSize;
	if (this->currCli.SetTo(currCli))
	{
		cli = currCli;
		writeSize = currCli->Write(buff);
		if (writeSize == 0)
			toClose = true;
	}
	else
	{
		writeSize = 0;
	}
	mutUsage.EndUse();
	if (toClose)
	{
		mutUsage.BeginUse();
		Sync::MutexUsage readMutUsage(this->readMut);
		if (this->currCli.SetTo(currCli) && this->currCli == cli)
		{
			this->currCli.Delete();
		}
		readMutUsage.EndUse();
		mutUsage.EndUse();
	}
	return writeSize;
}

Int32 Net::TCPServerStream::Flush()
{
	return 0;
}

void Net::TCPServerStream::Close()
{
	this->svr.Delete();
	NN<Net::TCPClient> currCli;
	Sync::MutexUsage mutUsage(this->connMut);
	if (this->currCli.SetTo(currCli))
	{
		currCli->Close();
	}
	mutUsage.EndUse();
	mutUsage.BeginUse();
	Sync::MutexUsage readMutUsage(this->readMut);
	this->currCli.Delete();
	readMutUsage.EndUse();
	mutUsage.EndUse();
}

Bool Net::TCPServerStream::Recover()
{
	return false;
}

IO::StreamType Net::TCPServerStream::GetStreamType() const
{
	return IO::StreamType::TCPServer;
}

Bool Net::TCPServerStream::IsError() const
{
	return this->svr.IsNull();
}
