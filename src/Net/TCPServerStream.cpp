#include "Stdafx.h"
#include "Net/TCPServerStream.h"
#include "Sync/Interlocked.h"
#include "Sync/MutexUsage.h"
#include "Sync/ThreadUtil.h"
#include "Text/StringBuilder.h"

void __stdcall Net::TCPServerStream::ConnHandler(NN<Socket> s, AnyType userObj)
{
	NN<Net::TCPServerStream> me = userObj.GetNN<Net::TCPServerStream>();
	Net::TCPClient *cli;
	NEW_CLASS(cli, Net::TCPClient(me->sockf, s));
	Sync::MutexUsage connMutUsage(me->connMut);
	if (me->currCli)
	{
		me->currCli->Close();
		Sync::MutexUsage mutUsage(me->readMut);
		DEL_CLASS(me->currCli);
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
	this->currCli = 0;
	NEW_CLASS(this->svr, Net::TCPServer(sockf, 0, port, log, ConnHandler, this, CSTR("SStm: "), true));
	if (this->svr->IsV4Error())
	{
		SDEL_CLASS(this->svr);return;
	}
}

Net::TCPServerStream::~TCPServerStream()
{
	this->Close();
}

Bool Net::TCPServerStream::IsDown() const
{
	if (this->svr == 0)
		return true;
	if (this->currCli == 0)
		return true;
	return false;
}

UOSInt Net::TCPServerStream::Read(const Data::ByteArray &buff)
{
	Bool toClose = false;
	UOSInt readSize = 0;
	while (this->svr)
	{
		Sync::MutexUsage readMutUsage(this->readMut);
		if (this->currCli)
		{
			readSize = this->currCli->Read(buff);
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
			SDEL_CLASS(this->currCli);
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

UOSInt Net::TCPServerStream::Write(Data::ByteArrayR buff)
{
	Bool toClose = false;
	Net::TCPClient *cli = 0;
	Sync::MutexUsage mutUsage(this->connMut);
	UOSInt writeSize;
	if (this->currCli)
	{
		cli = this->currCli;
		writeSize = this->currCli->Write(buff);
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
		if (this->currCli != 0 && this->currCli == cli)
		{
			SDEL_CLASS(this->currCli);
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
	SDEL_CLASS(this->svr);
	Sync::MutexUsage mutUsage(this->connMut);
	if (this->currCli != 0)
	{
		this->currCli->Close();
	}
	mutUsage.EndUse();
	mutUsage.BeginUse();
	Sync::MutexUsage readMutUsage(this->readMut);
	if (this->currCli != 0)
	{
		SDEL_CLASS(this->currCli);
	}
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
	return this->svr == 0;
}
