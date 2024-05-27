#include "Stdafx.h"
#include "Net/TCPBoardcastStream.h"
#include "Sync/Interlocked.h"
#include "Sync/MutexUsage.h"
#include "Sync/SimpleThread.h"
#include "Text/StringBuilderUTF8.h"

void __stdcall Net::TCPBoardcastStream::ConnHandler(NN<Socket> s, AnyType userObj)
{
	NN<Net::TCPBoardcastStream> me = userObj.GetNN<Net::TCPBoardcastStream>();
	NN<Net::TCPClient> cli;
	NEW_CLASSNN(cli, Net::TCPClient(me->sockf, s));
	me->cliMgr->AddClient(cli, 0);
	if (me->writeBuffSize > 0)
	{
		UTF8Char sbuff[32];
		UTF8Char *sptr;
		Text::StringBuilderUTF8 sb;
		UOSInt size = me->writeBuffSize;
		me->writeBuffSize = 0;
		if (me->log->HasHandler())
		{
			sb.AppendC(UTF8STRC("Sending to "));
			sptr = me->sockf->GetRemoteName(sbuff, s);
			sb.AppendP(sbuff, sptr);
			sb.AppendC(UTF8STRC(" with "));
			sb.AppendUOSInt(size);
			me->log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Raw);
		}
		cli->Write(me->writeBuff, size);
	}
}

void __stdcall Net::TCPBoardcastStream::ClientEvent(NN<Net::TCPClient> cli, AnyType userObj, AnyType cliData, Net::TCPClientMgr::TCPEventType evtType)
{
	switch (evtType)
	{
	case Net::TCPClientMgr::TCP_EVENT_DISCONNECT:
		cli.Delete();
		break;
	case Net::TCPClientMgr::TCP_EVENT_CONNECT:
	case Net::TCPClientMgr::TCP_EVENT_HASDATA:
	case Net::TCPClientMgr::TCP_EVENT_SHUTDOWN:
	default:
		break;
	}
}

void __stdcall Net::TCPBoardcastStream::ClientData(NN<Net::TCPClient> cli, AnyType userObj, AnyType cliData, const Data::ByteArrayR &buff)
{
	NN<Net::TCPBoardcastStream> me = userObj.GetNN<Net::TCPBoardcastStream>();
	Text::StringBuilderUTF8 sb;
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	if (me->log->HasHandler())
	{
		sb.AppendC(UTF8STRC("Recv from "));
		sptr = cli->GetRemoteName(sbuff);
		sb.AppendP(sbuff, sptr);
		sb.AppendC(UTF8STRC(" with "));
		sb.AppendUOSInt(buff.GetSize());
		me->log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Raw);
	}
	Sync::MutexUsage mutUsage(me->readMut);
	UOSInt readBuffSize = me->readBuffPtr2 - me->readBuffPtr1;
	if ((OSInt)readBuffSize < 0)
	{
		readBuffSize += 16384;
	}
	UOSInt size = buff.GetSize();
	if (readBuffSize + size >= 16384)
	{
		size = 16383 - readBuffSize;
	}
	if (size > 0)
	{
		if (me->readBuffPtr2 + size > 16384)
		{
			MemCopyNO(&me->readBuff[me->readBuffPtr2], buff.Ptr().Ptr(), 16384 - me->readBuffPtr2);
			MemCopyNO(me->readBuff, &buff[16384 - me->readBuffPtr2], size - (16384 - me->readBuffPtr2));
			me->readBuffPtr2 = size - (16384 - me->readBuffPtr2);
		}
		else
		{
			MemCopyNO(&me->readBuff[me->readBuffPtr2], buff.Ptr().Ptr(), size);
			me->readBuffPtr2 += size;
			if (me->readBuffPtr2 >= 16384)
			{
				me->readBuffPtr2 -= 16384;
			}
		}
	}
	mutUsage.EndUse();
	
	if (me->log->HasHandler())
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("Recv readBuffPtr2 = "));
		sb.AppendUOSInt(me->readBuffPtr2);
		me->log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Raw);
	}
}

void __stdcall Net::TCPBoardcastStream::ClientTimeout(NN<Net::TCPClient> cli, AnyType userObj, AnyType cliData)
{
	NN<Net::TCPBoardcastStream> me = userObj.GetNN<Net::TCPBoardcastStream>();
	if (me->log->HasHandler())
	{
		Text::StringBuilderUTF8 sb;
		UTF8Char sbuff[32];
		UTF8Char *sptr;
		sb.AppendC(UTF8STRC("Timeout processing "));
		sptr = cli->GetRemoteName(sbuff);
		sb.AppendP(sbuff, sptr);
		me->log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Raw);
	}
}

Net::TCPBoardcastStream::TCPBoardcastStream(NN<Net::SocketFactory> sockf, UInt16 port, NN<IO::LogTool> log) : IO::Stream(CSTR("Net.TCPBoardcastSream"))
{
	this->sockf = sockf;
	this->log = log;
	this->readCnt = 0;
	NEW_CLASS(this->cliMgr, Net::TCPClientMgr(600, ClientEvent, ClientData, this, 3, ClientTimeout));
	this->readBuff = MemAlloc(UInt8, 16384);
	this->writeBuff = MemAlloc(UInt8, 2048);
	this->readBuffPtr1 = 0;
	this->readBuffPtr2 = 0;
	this->writeBuffSize = 0;
	NEW_CLASS(this->svr, Net::TCPServer(sockf, 0, port, log, ConnHandler, this, CSTR("BStm: "), true));
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
		Sync::SimpleThread::Sleep(10);
	}
	MemFree(this->readBuff);
	MemFree(this->writeBuff);
}

Bool Net::TCPBoardcastStream::IsDown() const
{
	if (this->svr == 0)
	{
		return true;
	}
	return this->cliMgr->GetClientCount() == 0;
}

UOSInt Net::TCPBoardcastStream::Read(const Data::ByteArray &buff)
{
	UOSInt readBuffSize;

	Sync::Interlocked::IncrementU32(this->readCnt);
	while (true)
	{
		if (this->svr == 0)
		{
			Sync::Interlocked::DecrementU32(this->readCnt);
			return 0;
		}
		readBuffSize = this->readBuffPtr2 - this->readBuffPtr1;
		if ((OSInt)readBuffSize < 0)
		{
			readBuffSize += 16384;
		}
		if ((OSInt)readBuffSize > 0)
			break;
		Sync::SimpleThread::Sleep(10);
	}
	Sync::Interlocked::DecrementU32(this->readCnt);
	Sync::MutexUsage mutUsage(this->readMut);
	Data::ByteArray myBuff = buff;
	if ((UOSInt)readBuffSize >= myBuff.GetSize())
	{
	}
	else
	{
		myBuff = myBuff.WithSize(readBuffSize);
	}

	if (readBuffPtr1 + myBuff.GetSize() > 16384)
	{
		myBuff.CopyFrom(Data::ByteArrayR(&this->readBuff[this->readBuffPtr1], 16384 - readBuffPtr1));
		myBuff.CopyFrom(16384 - readBuffPtr1, Data::ByteArrayR(this->readBuff, myBuff.GetSize() - (16384 - readBuffPtr1)));
		readBuffPtr1 = myBuff.GetSize() - (16384 - readBuffPtr1);
	}
	else
	{
		myBuff.CopyFrom(Data::ByteArrayR(&this->readBuff[this->readBuffPtr1], myBuff.GetSize()));
		this->readBuffPtr1 += myBuff.GetSize();
		if (this->readBuffPtr1 >= 16384)
		{
			this->readBuffPtr1 -= 16384;
		}
	}
	mutUsage.EndUse();
	if (this->log->HasHandler())
	{
		Text::StringBuilderUTF8 sb;
		sb.AppendC(UTF8STRC("TBS "));
		sb.AppendUOSInt(myBuff.GetSize());
		sb.AppendC(UTF8STRC(" bytes returned"));
		this->log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Raw);
	}
	return myBuff.GetSize();
}

UOSInt Net::TCPBoardcastStream::Write(const UInt8 *buff, UOSInt size)
{
	Bool cliFound = false;
	UOSInt i;
	NN<Net::TCPClient> cli;
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	Text::StringBuilderUTF8 sb;
	AnyType cliData;
	Sync::MutexUsage mutUsage;
	this->cliMgr->UseGetClient(mutUsage);
	i = this->cliMgr->GetClientCount();
	while (i-- > 0)
	{
		if (this->cliMgr->GetClient(i, cliData).SetTo(cli))
		{
			cli->Write(buff, size);

			if (this->log->HasHandler())
			{
				sb.ClearStr();
				sb.AppendC(UTF8STRC("Sending to "));
				sptr = cli->GetRemoteName(sbuff);
				sb.AppendP(sbuff, sptr);
				sb.AppendC(UTF8STRC(" with "));
				sb.AppendUOSInt(size);
				this->log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Raw);
			}

			cliFound = true;
		}
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

IO::StreamType Net::TCPBoardcastStream::GetStreamType() const
{
	return IO::StreamType::TCPBoardcast;
}

Bool Net::TCPBoardcastStream::IsError() const
{
	return this->svr == 0;
}
