#include "Stdafx.h"
#include "Net/TCPBoardcastStream.h"
#include "Sync/Interlocked.h"
#include "Sync/MutexUsage.h"
#include "Sync/Thread.h"
#include "Text/StringBuilderUTF8.h"

void __stdcall Net::TCPBoardcastStream::ConnHandler(Socket *s, void *userObj)
{
	Net::TCPBoardcastStream *me = (Net::TCPBoardcastStream*)userObj;
	Net::TCPClient *cli;
	NEW_CLASS(cli, Net::TCPClient(me->sockf, s));
	me->cliMgr->AddClient(cli, 0);
	if (me->writeBuffSize > 0)
	{
		UTF8Char sbuff[32];
		Text::StringBuilderUTF8 sb;
		UOSInt size = me->writeBuffSize;
		me->writeBuffSize = 0;
		if (me->log)
		{
			sb.AppendC(UTF8STRC("Sending to "));
			me->sockf->GetRemoteName(sbuff, s);
			sb.Append(sbuff);
			sb.AppendC(UTF8STRC(" with "));
			sb.AppendUOSInt(size);
			me->log->LogMessageC(sb.ToString(), sb.GetLength(), IO::ILogHandler::LOG_LEVEL_RAW);
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
	case Net::TCPClientMgr::TCP_EVENT_CONNECT:
	case Net::TCPClientMgr::TCP_EVENT_HASDATA:
	case Net::TCPClientMgr::TCP_EVENT_SHUTDOWN:
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
		sb.AppendC(UTF8STRC("Recv from "));
		cli->GetRemoteName(sbuff);
		sb.Append(sbuff);
		sb.AppendC(UTF8STRC(" with "));
		sb.AppendUOSInt(size);
		me->log->LogMessageC(sb.ToString(), sb.GetLength(), IO::ILogHandler::LOG_LEVEL_RAW);
	}
	Sync::MutexUsage mutUsage(me->readMut);
	UOSInt readBuffSize = me->readBuffPtr2 - me->readBuffPtr1;
	if ((OSInt)readBuffSize < 0)
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
		sb.AppendC(UTF8STRC("Recv readBuffPtr2 = "));
		sb.AppendUOSInt(me->readBuffPtr2);
		me->log->LogMessageC(sb.ToString(), sb.GetLength(), IO::ILogHandler::LOG_LEVEL_RAW);
	}
}

void __stdcall Net::TCPBoardcastStream::ClientTimeout(Net::TCPClient *cli, void *userObj, void *cliData)
{
	Net::TCPBoardcastStream *me = (Net::TCPBoardcastStream*)userObj;
	if (me->log)
	{
		Text::StringBuilderUTF8 sb;
		UTF8Char sbuff[32];
		sb.AppendC(UTF8STRC("Timeout processing "));
		cli->GetRemoteName(sbuff);
		sb.Append(sbuff);
		me->log->LogMessageC(sb.ToString(), sb.GetLength(), IO::ILogHandler::LOG_LEVEL_RAW);
	}
}

Net::TCPBoardcastStream::TCPBoardcastStream(Net::SocketFactory *sockf, UInt16 port, IO::LogTool *log) : IO::Stream(UTF8STRC("Net.TCPBoardcastSream"))
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
	UOSInt readBuffSize;

	Sync::Interlocked::Increment(&this->readCnt);
	while (true)
	{
		if (this->svr == 0)
		{
			Sync::Interlocked::Decrement(&this->readCnt);
			return 0;
		}
		readBuffSize = this->readBuffPtr2 - this->readBuffPtr1;
		if ((OSInt)readBuffSize < 0)
		{
			readBuffSize += 16384;
		}
		if ((OSInt)readBuffSize > 0)
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
		sb.AppendC(UTF8STRC("TBS "));
		sb.AppendUOSInt(size);
		sb.AppendC(UTF8STRC(" bytes returned"));
		this->log->LogMessageC(sb.ToString(), sb.GetLength(), IO::ILogHandler::LOG_LEVEL_RAW);
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
			sb.AppendC(UTF8STRC("Sending to "));
			cli->GetRemoteName(sbuff);
			sb.Append(sbuff);
			sb.AppendC(UTF8STRC(" with "));
			sb.AppendUOSInt(size);
			this->log->LogMessageC(sb.ToString(), sb.GetLength(), IO::ILogHandler::LOG_LEVEL_RAW);
		}

		cliFound = true;
	}
	mutUsage.EndUse();
	if (!cliFound)
	{
		UOSInt buffSizeLeft = 2048 - this->writeBuffSize;
		if (buffSizeLeft >= size)
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
