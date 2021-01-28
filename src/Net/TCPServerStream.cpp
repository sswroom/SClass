#include "Stdafx.h"
#include "Net/TCPServerStream.h"
#include "Sync/Interlocked.h"
#include "Sync/Thread.h"
#include "Text/StringBuilder.h"

void __stdcall Net::TCPServerStream::ConnHandler(UInt32 *s, void *userObj)
{
	Net::TCPServerStream *me = (Net::TCPServerStream*)userObj;
	Net::TCPClient *cli;
	NEW_CLASS(cli, Net::TCPClient(me->sockf, s));
	me->connMut->Lock();
	if (me->currCli)
	{
		me->currCli->Close();
		me->readMut->Lock();
		DEL_CLASS(me->currCli);
		me->currCli = cli;
		me->readMut->Unlock();
	}
	else
	{
		me->readMut->Lock();
		me->currCli = cli;
		me->readMut->Unlock();
	}
	me->connMut->Unlock();
	me->readEvt->Set();
}

Net::TCPServerStream::TCPServerStream(Net::SocketFactory *sockf, UInt16 port, IO::LogTool *log) : IO::Stream((const UTF8Char*)"Net.TCPBoardcastSream")
{
	this->sockf = sockf;
	this->log = log;
	this->currCli = 0;
	NEW_CLASS(this->readEvt, Sync::Event(true, (const UTF8Char*)"Net.TCPServerStream.readEvt"));
	NEW_CLASS(this->connMut, Sync::Mutex());
	NEW_CLASS(this->readMut, Sync::Mutex());
	NEW_CLASS(this->svr, Net::TCPServer(sockf, port, log, ConnHandler, this, (const UTF8Char*)"SStm: "));
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

UOSInt Net::TCPServerStream::Read(UInt8 *buff, UOSInt size)
{
	Bool toClose = false;
	UOSInt readSize = 0;
	while (this->svr)
	{
		this->readMut->Lock();
		if (this->currCli)
		{
			readSize = this->currCli->Read(buff, size);
			if (readSize == 0)
				toClose = true;
		}
		else
		{
		}
		this->readMut->Unlock();
		if (toClose)
		{
			this->connMut->Lock();
			this->readMut->Lock();
			SDEL_CLASS(this->currCli);
			this->readMut->Unlock();
			this->connMut->Unlock();
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
	this->connMut->Lock();
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
	this->connMut->Unlock();
	if (toClose)
	{
		this->connMut->Lock();
		this->readMut->Lock();
		if (this->currCli != 0 && this->currCli == cli)
		{
			SDEL_CLASS(this->currCli);
		}
		this->readMut->Unlock();
		this->connMut->Unlock();
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
	this->connMut->Lock();
	if (this->currCli != 0)
	{
		this->currCli->Close();
	}
	this->connMut->Unlock();
	this->connMut->Lock();
	this->readMut->Lock();
	if (this->currCli != 0)
	{
		SDEL_CLASS(this->currCli);
	}
	this->readMut->Unlock();
	this->connMut->Unlock();
}

Bool Net::TCPServerStream::Recover()
{
	return false;
}

Bool Net::TCPServerStream::IsError()
{
	return this->svr == 0;
}
