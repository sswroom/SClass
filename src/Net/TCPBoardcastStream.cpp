#include "Stdafx.h"
#include "Net/TCPBoardcastStream.h"
#include "Sync/Interlocked.h"
#include "Sync/MutexUsage.h"
#include "Sync/Thread.h"
#include "Text/StringBuilderUTF8.h"

void __stdcall Net::TCPBoardcastStream::ConnHandler(UInt32 *s, void *userObj)
{
	Net::TCPBoardcastStream *me = (Net::TCPBoardcastStream*)userObj;
	Net::TCPClient *cli;
	NEW_CLASS(cli, Net::TCPClient(me->sockf, s));
	me->cliMgr->AddClient(cli, 0);
	if (me->writeBuffSize > 0)
	{
		UTF8Char sbuff[32];
		Text::StringBuilderUTF8 sb;
		OSInt size = me->writeBuffSize;
		me->writeBuffSize = 0;
		if (me->log)
		{
			sb.Append((const UTF8Char*)"Sending to ");
			me->sockf->GetRemoteName(sbuff, s);
			sb.Append(sbuff);
			sb.Append((const UTF8Char*)" with ");
			sb.AppendOSInt(size);
			me->log->LogMessage(sb.ToString(), IO::ILogHandler::LOG_LEVEL_RAW);
		}
		cli->Write(me->writeBuff, size);
	}
}

void __stdcall Net::TCPBoardcastStream::ClientEvent(Net::TCPClient *cli, void *userObj, void *cliData, Net::TCPClientMgr::TCPEventType evtType)
{
	switch (evtType)
	{
	case Net::TCPClientMgr::TCP_EVENT_DISCONNECT:
		DEL_CLASS(cli);
		break;
	default:
		break;
	}
}

void __stdcall Net::TCPBoardcastStream::ClientData(Net::TCPClient *cli, void *userObj, void *cliData, const UInt8 *buff, UOSInt size)
{
	Net::TCPBoardcastStream *me = (Net::TCPBoardcastStream*)userObj;
	Text::StringBuilderUTF8 sb;
	UTF8Char sbuff[32];
	if (me->log)
	{
		sb.Append((const UTF8Char*)"Recv from ");
		cli->GetRemoteName(sbuff);
		sb.Append(sbuff);
		sb.Append((const UTF8Char*)" with ");
		sb.AppendOSInt(size);
		me->log->LogMessage(sb.ToString(), IO::ILogHandler::LOG_LEVEL_RAW);
	}
	Sync::MutexUsage mutUsage(me->readMut);
	OSInt readBuffSize = me->readBuffPtr2 - me->readBuffPtr1;
	if (readBuffSize < 0)
	{
		readBuffSize += 16384;
	}
	if (readBuffSize + size >= 16384)
	{
		size = 16383 - readBuffSize;
	}
	if (size > 0)
	{
		if (me->readBuffPtr2 + size > 16384)
		{
			MemCopyNO(&me->readBuff[me->readBuffPtr2], buff, 16384 - me->readBuffPtr2);
			MemCopyNO(me->readBuff, &buff[16384 - me->readBuffPtr2], size - (16384 - me->readBuffPtr2));
			me->readBuffPtr2 = size - (16384 - me->readBuffPtr2);
		}
		else
		{
			MemCopyNO(&me->readBuff[me->readBuffPtr2], buff, size);
			me->readBuffPtr2 += size;
			if (me->readBuffPtr2 >= 16384)
			{
				me->readBuffPtr2 -= 16384;
			}
		}
	}
	mutUsage.EndUse();
	
	if (me->log)
	{
		sb.ClearStr();
		sb.Append((const UTF8Char*)"Recv readBuffPtr2 = ");
		sb.AppendOSInt(me->readBuffPtr2);
		me->log->LogMessage(sb.ToString(), IO::ILogHandler::LOG_LEVEL_RAW);
	}
}

void __stdcall Net::TCPBoardcastStream::ClientTimeout(Net::TCPClient *cli, void *userObj, void *cliData)
{
	Net::TCPBoardcastStream *me = (Net::TCPBoardcastStream*)userObj;
	if (me->log)
	{
		Text::StringBuilderUTF8 sb;
		UTF8Char sbuff[32];
		sb.Append((const UTF8Char*)"Timeout processing ");
		cli->GetRemoteName(sbuff);
		sb.Append(sbuff);
		me->log->LogMessage(sb.ToString(), IO::ILogHandler::LOG_LEVEL_RAW);
	}
}

Net::TCPBoardcastStream::TCPBoardcastStream(Net::SocketFactory *sockf, Int32 port, IO::LogTool *log) : IO::Stream((const UTF8Char*)"Net.TCPBoardcastSream")
{
	this->sockf = sockf;
	this->log = log;
	this->readCnt = 0;
	NEW_CLASS(this->cliMgr, Net::TCPClientMgr(600, ClientEvent, ClientData, this, 3, ClientTimeout));
	NEW_CLASS(this->readMut, Sync::Mutex());
	this->readBuff = MemAlloc(UInt8, 16384);
	this->writeBuff = MemAlloc(UInt8, 2048);
	this->readBuffPtr1 = 0;
	this->readBuffPtr2 = 0;
	this->writeBuffSize = 0;
	NEW_CLASS(this->svr, Net::TCPServer(sockf, port, log, ConnHandler, this, (const UTF8Char*)"BStm: "));
	if (this->svr->IsV4Error())
	{
		SDEL_CLASS(this->svr);
		return;
	}
}

Net::TCPBoardcastStream::~TCPBoardcastStream()
{
	SDEL_CLASS(this->svr);
	DEL_CLASS(this->cliMgr);
	while (this->readCnt > 0)
	{
		Sync::Thread::Sleep(10);
	}
	DEL_CLASS(this->readMut);
	MemFree(this->readBuff);
	MemFree(this->writeBuff);
}

UOSInt Net::TCPBoardcastStream::Read(UInt8 *buff, UOSInt size)
{
	OSInt readBuffSize;

	Sync::Interlocked::Increment(&this->readCnt);
	while (true)
	{
		if (this->svr == 0)
		{
			Sync::Interlocked::Decrement(&this->readCnt);
			return 0;
		}
		readBuffSize = this->readBuffPtr2 - this->readBuffPtr1;
		if (readBuffSize < 0)
		{
			readBuffSize += 16384;
		}
		if (readBuffSize > 0)
			break;
		Sync::Thread::Sleep(10);
	}
	Sync::Interlocked::Decrement(&this->readCnt);
	Sync::MutexUsage mutUsage(this->readMut);
	if ((UOSInt)readBuffSize >= size)
	{
	}
	else
	{
		size = readBuffSize;	
	}

	if (readBuffPtr1 + size > 16384)
	{
		MemCopyNO(buff, &this->readBuff[this->readBuffPtr1], 16384 - readBuffPtr1);
		MemCopyNO(&buff[16384 - readBuffPtr1], this->readBuff, size - (16384 - readBuffPtr1));
		readBuffPtr1 = size - (16384 - readBuffPtr1);
	}
	else
	{
		MemCopyNO(buff, &this->readBuff[this->readBuffPtr1], size);
		this->readBuffPtr1 += size;
		if (this->readBuffPtr1 >= 16384)
		{
			this->readBuffPtr1 -= 16384;
		}
	}
	mutUsage.EndUse();
	if (this->log)
	{
		Text::StringBuilderUTF8 sb;
		sb.Append((const UTF8Char*)"TBS ");
		sb.AppendOSInt(size);
		sb.Append((const UTF8Char*)" bytes returned");
		this->log->LogMessage(sb.ToString(), IO::ILogHandler::LOG_LEVEL_RAW);
	}
	return size;
}

UOSInt Net::TCPBoardcastStream::Write(const UInt8 *buff, UOSInt size)
{
	Bool cliFound = false;
	UOSInt i;
	Net::TCPClient *cli;
	UTF8Char sbuff[32];
	Text::StringBuilderUTF8 sb;
	void *cliData;
	Sync::MutexUsage mutUsage;
	this->cliMgr->UseGetClient(&mutUsage);
	i = this->cliMgr->GetClientCount();
	while (i-- > 0)
	{
		
		cli = this->cliMgr->GetClient(i, &cliData);
		cli->Write(buff, size);

		if (this->log)
		{
			sb.ClearStr();
			sb.Append((const UTF8Char*)"Sending to ");
			cli->GetRemoteName(sbuff);
			sb.Append(sbuff);
			sb.Append((const UTF8Char*)" with ");
			sb.AppendOSInt(size);
			this->log->LogMessage(sb.ToString(), IO::ILogHandler::LOG_LEVEL_RAW);
		}

		cliFound = true;
	}
	mutUsage.EndUse();
	if (!cliFound)
	{
		OSInt buffSizeLeft = 2048 - this->writeBuffSize;
		if (buffSizeLeft >= (OSInt)size)
		{
			MemCopyNO(&this->writeBuff[this->writeBuffSize], buff, size);
			this->writeBuffSize += size;
		}
		else
		{
			MemCopyO(this->writeBuff, &this->writeBuff[size - buffSizeLeft], this->writeBuffSize - size + buffSizeLeft);
			this->writeBuffSize -= size - buffSizeLeft;
			MemCopyNO(&this->writeBuff[this->writeBuffSize], buff, size);
			this->writeBuffSize += size;
		}
	}
	return size;
}

Int32 Net::TCPBoardcastStream::Flush()
{
	return 0;
}

void Net::TCPBoardcastStream::Close()
{
	SDEL_CLASS(this->svr);
}

Bool Net::TCPBoardcastStream::Recover()
{
	return false;
}

Bool Net::TCPBoardcastStream::IsError()
{
	return this->svr == 0;
}
