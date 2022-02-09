#include "Stdafx.h"
#include "Net/TCPServerStream.h"
#include "Sync/Interlocked.h"
#include "Sync/MutexUsage.h"
#include "Sync/Thread.h"
#include "Text/StringBuilder.h"

void __stdcall Net::TCPServerStream::ConnHandler(Socket *s, void *userObj)
{
	Net::TCPServerStream *me = (Net::TCPServerStream*)userObj;
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
	me->readEvt->Set();
}

Net::TCPServerStream::TCPServerStream(Net::SocketFactory *sockf, UInt16 port, IO::LogTool *log) : IO::Stream(CSTR("Net.TCPBoardcastSream"))
{
	this->sockf = sockf;
	this->log = log;
	this->currCli = 0;
	NEW_CLASS(this->readEvt, Sync::Event(true));
	NEW_CLASS(this->connMut, Sync::Mutex());
	NEW_CLASS(this->readMut, Sync::Mutex());
	NEW_CLASS(this->svr, Net::TCPServer(sockf, port, log, ConnHandler, this, CSTR("SStm: ")));
	if (this->svr->IsV4Error())
	{
		SDEL_CLASS(this->svr);return;
	}
}

Net::TCPServerStream::~TCPServerStream()
{
	this->Close();
	DEL_CLASS(this->readMut);
	DEL_CLASS(this->connMut);
	DEL_CLASS(this->readEvt);
}

Bool Net::TCPServerStream::IsDown()
{
	if (this->svr == 0)
		return true;
	if (this->currCli == 0)
		return true;
	return false;
}

UOSInt Net::TCPServerStream::Read(UInt8 *buff, UOSInt size)
{
	Bool toClose = false;
	UOSInt readSize = 0;
	while (this->svr)
	{
		Sync::MutexUsage readMutUsage(this->readMut);
		if (this->currCli)
		{
			readSize = this->currCli->Read(buff, size);
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
		this->readEvt->Wait(100);
	}
	return 0;
}

UOSInt Net::TCPServerStream::Write(const UInt8 *buff, UOSInt size)
{
	Bool toClose = false;
	Net::TCPClient *cli = 0;
	Sync::MutexUsage mutUsage(this->connMut);
	if (this->currCli)
	{
		cli = this->currCli;
		size = this->currCli->Write(buff, size);
		if (size == 0)
			toClose = true;
	}
	else
	{
		size = 0;
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
	return size;
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

Bool Net::TCPServerStream::IsError()
{
	return this->svr == 0;
}
