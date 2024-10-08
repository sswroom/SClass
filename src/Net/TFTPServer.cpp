#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Data/DateTime.h"
#include "IO/Path.h"
#include "Net/TFTPServer.h"
#include "Sync/MutexUsage.h"
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"
#include "Text/StringBuilderUTF8.h"

void __stdcall Net::TFTPServer::OnCommandPacket(NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, Data::ByteArrayR data, AnyType userData)
{
	NN<Net::TFTPServer> me = userData.GetNN<Net::TFTPServer>();
	const UTF8Char *fileName;
	UOSInt fileNameLen;
	const UTF8Char *mode;
	const UTF8Char *endPtr;
//	const UTF8Char *optName;
//	const UTF8Char *optVal;
	UInt16 opcode;
	UOSInt blkSize;
	UOSInt i;
	UOSInt len;
	UInt8 repBuff[32];
	UnsafeArray<UTF8Char> sptr;
	if (data.GetSize() < 4)
		return;
	opcode = ReadMUInt16(&data[0]);
	if (opcode != 1 && opcode != 2)
		return;
	if (data[data.GetSize() - 1])
		return;
	blkSize = 512;
	fileName = &data[2];
	endPtr = &data[data.GetSize()];
	fileNameLen = Text::StrCharCnt(fileName);
	mode = &fileName[fileNameLen + 1];
	if (mode >= endPtr)
		return;
	
	if (!Text::StrEquals(mode, (const UTF8Char*)"octet"))
	{
		WriteMInt16(&repBuff[0], 5);
		WriteMInt16(&repBuff[2], 0);
		i = (UOSInt)(Text::StrConcatC(&repBuff[4], UTF8STRC("Mode not supported")) - repBuff + 1);
		me->svr->SendTo(addr, port, repBuff, i);
		return;
	}
	if (Text::StrIndexOfCharC(fileName, fileNameLen, IO::Path::PATH_SEPERATOR) != INVALID_INDEX)
	{
		WriteMInt16(&repBuff[0], 5);
		WriteMInt16(&repBuff[2], 2);
		i = (UOSInt)(Text::StrConcatC(&repBuff[4], UTF8STRC("Access violation")) - repBuff + 1);
		me->svr->SendTo(addr, port, repBuff, i);
		return;
	}
	UInt64 sessId = (((UInt64)ReadMUInt32(addr->addr)) << 16) | port;
	NN<SessionInfo> sess;
	Sync::MutexUsage mutUsage(me->mut);
	if (me->sessMap.Get(sessId).SetTo(sess))
	{
		mutUsage.EndUse();
		WriteMInt16(&repBuff[0], 5);
		WriteMInt16(&repBuff[2], 4);
		i = (UOSInt)(Text::StrConcatC(&repBuff[4], UTF8STRC("Already started")) - repBuff + 1);
		me->svr->SendTo(addr, port, repBuff, i);
		return;
	}
	mutUsage.EndUse();
	
	Text::StringBuilderUTF8 sb;
	sb.Append(me->path);
	sb.AppendC(fileName, fileNameLen);
	IO::Path::PathType pt;
	IO::FileStream *fs;
	pt = IO::Path::GetPathType(sb.ToCString());
	if (opcode == 1)
	{
		if (pt == IO::Path::PathType::File)
		{
			NEW_CLASS(fs, IO::FileStream(sb.ToCString(), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
			if (fs->IsError())
			{
				DEL_CLASS(fs);
				WriteMInt16(&repBuff[0], 5);
				WriteMInt16(&repBuff[2], 2);
				i = (UOSInt)(Text::StrConcatC(&repBuff[4], UTF8STRC("Cannot open file")) - repBuff + 1);
				me->svr->SendTo(addr, port, repBuff, i);
				return;
			}
			Data::DateTime dt;
			dt.SetCurrTimeUTC();
			sess = MemAllocNN(SessionInfo);
			sess->sessId = sessId;
			sess->lastSignalTime = dt.ToTicks();
			sess->stm = fs;
			sess->blockSize = blkSize;
			sess->isWrite = false;
			sess->isLast = false;
			sess->currBlock = 1;
			sess->fileName = Text::String::New(sb.ToString(), sb.GetLength());
			Sync::MutexUsage mutUsage(me->mut);
			me->sessMap.Put(sess->sessId, sess);

			UInt8 *packet = MemAlloc(UInt8, sess->blockSize + 4);
			WriteMInt16(&packet[0], 3);
			WriteMInt16(&packet[2], sess->currBlock);
			len = sess->stm->Read(Data::ByteArray(&packet[4], sess->blockSize));
			if (len != sess->blockSize)
			{
				sess->isLast = true;
			}
			mutUsage.EndUse();
			me->dataSvr->SendTo(addr, port, packet, len + 4);
			MemFree(packet);
			if (me->log->HasHandler())
			{
				sb.ClearStr();
				sb.AppendC(UTF8STRC("Sending "));
				sb.AppendC(fileName, fileNameLen);
				sb.AppendC(UTF8STRC(" to "));
				sptr = Net::SocketUtil::GetAddrName(repBuff, addr, port).Or(repBuff);
				sb.AppendP(repBuff, sptr);
				me->log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Action);
			}
		}
		else
		{
			WriteMInt16(&repBuff[0], 5);
			WriteMInt16(&repBuff[2], 1);
			i = (UOSInt)(Text::StrConcatC(&repBuff[4], UTF8STRC("File not found")) - repBuff + 1);
			me->svr->SendTo(addr, port, repBuff, i);
			return;
		}
	}
	else if (opcode == 2)
	{
		if (pt == IO::Path::PathType::File)
		{
			WriteMInt16(&repBuff[0], 5);
			WriteMInt16(&repBuff[2], 6);
			i = (UOSInt)(Text::StrConcatC(&repBuff[4], UTF8STRC("File already exists")) - repBuff + 1);
			me->svr->SendTo(addr, port, repBuff, i);
			return;
		}
		else if (pt == IO::Path::PathType::Directory)
		{
			WriteMInt16(&repBuff[0], 5);
			WriteMInt16(&repBuff[2], 6);
			i = (UOSInt)(Text::StrConcatC(&repBuff[4], UTF8STRC("Directory already exists")) - repBuff + 1);
			me->svr->SendTo(addr, port, repBuff, i);
			return;
		}
		else
		{
			NEW_CLASS(fs, IO::FileStream(sb.ToCString(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
			if (fs->IsError())
			{
				DEL_CLASS(fs);
				WriteMInt16(&repBuff[0], 5);
				WriteMInt16(&repBuff[2], 2);
				i = (UOSInt)(Text::StrConcatC(&repBuff[4], UTF8STRC("Cannot open file")) - repBuff + 1);
				me->svr->SendTo(addr, port, repBuff, i);
				return;
			}
			Data::DateTime dt;
			dt.SetCurrTimeUTC();
			sess = MemAllocNN(SessionInfo);
			sess->sessId = sessId;
			sess->lastSignalTime = dt.ToTicks();
			sess->stm = fs;
			sess->blockSize = blkSize;
			sess->isWrite = true;
			sess->isLast = false;
			sess->currBlock = 0;
			sess->fileName = Text::String::New(sb.ToString(), sb.GetLength());
			Sync::MutexUsage mutUsage(me->mut);
			me->sessMap.Put(sess->sessId, sess);
			mutUsage.EndUse();

			WriteMInt16(&repBuff[0], 4);
			WriteMInt16(&repBuff[2], 0);
			me->dataSvr->SendTo(addr, port, repBuff, 4);

			if (me->log->HasHandler())
			{
				sb.ClearStr();
				sb.AppendC(UTF8STRC("Receiving "));
				sb.AppendC(fileName, fileNameLen);
				sb.AppendC(UTF8STRC(" to "));
				sptr = Net::SocketUtil::GetAddrName(repBuff, addr, port).Or(repBuff);
				sb.AppendP(repBuff, sptr);
				me->log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Action);
			}
		}
	}
	else
	{
		WriteMInt16(&repBuff[0], 5);
		WriteMInt16(&repBuff[2], 4);
		i = (UOSInt)(Text::StrConcatC(&repBuff[4], UTF8STRC("Unknown opcode")) - repBuff + 1);
		me->svr->SendTo(addr, port, repBuff, i);
		return;
	}
	
}

void __stdcall Net::TFTPServer::OnDataPacket(NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, Data::ByteArrayR data, AnyType userData)
{
	NN<Net::TFTPServer> me = userData.GetNN<Net::TFTPServer>();
	UInt64 sessId = (((UInt64)ReadMUInt32(addr->addr)) << 16) | port;
	NN<SessionInfo> sess;
	UInt8 repBuff[32];
	UnsafeArray<UTF8Char> sptr;
	Sync::MutexUsage mutUsage(me->mut);
	if (!me->sessMap.Get(sessId).SetTo(sess))
	{
		mutUsage.EndUse();
		if (ReadMUInt16(&data[0]) == 3)
		{
			WriteMInt16(&repBuff[0], 4);
			WriteMInt16(&repBuff[2], ReadMUInt16(&data[2]));
			me->dataSvr->SendTo(addr, port, repBuff, 4);
		}
		return;
	}
	Data::DateTime dt;
	dt.SetCurrTimeUTC();
	if (sess->isWrite)
	{
		if (ReadMUInt16(&data[0]) == 3)
		{
			UInt16 blkNum = ReadMUInt16(&data[2]);
			if (blkNum == sess->currBlock)
			{
				WriteMInt16(&repBuff[0], 4);
				WriteMInt16(&repBuff[2], blkNum);
				me->svr->SendTo(addr, port, repBuff, 4);
				sess->lastSignalTime = dt.ToTicks();
			}
			else if (blkNum == sess->currBlock + 1)
			{
				sess->currBlock = blkNum;
				sess->stm->Write(data.SubArray(4));
				if (data.GetSize() - 4 != sess->blockSize)
				{
					if (me->log->HasHandler())
					{
						Text::StringBuilderUTF8 sb;
						UOSInt i;
						sb.AppendC(UTF8STRC("End receiving "));
						i = sess->fileName->LastIndexOf(IO::Path::PATH_SEPERATOR);
						sb.AppendC(&sess->fileName->v[i + 1], sess->fileName->leng - i - 1);
						sb.AppendC(UTF8STRC(" to "));
						sptr = Net::SocketUtil::GetAddrName(repBuff, addr, port).Or(repBuff);
						sb.AppendP(repBuff, sptr);
						me->log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Action);
					}
					me->sessMap.Remove(sess->sessId);
					me->ReleaseSess(sess);
				}
				WriteMInt16(&repBuff[0], 4);
				WriteMInt16(&repBuff[2], blkNum);
				me->dataSvr->SendTo(addr, port, repBuff, 4);
				sess->lastSignalTime = dt.ToTicks();
			}
		}
	}
	else
	{
		if (ReadMUInt16(&data[0]) == 4)
		{
			UInt16 blkNum = ReadMUInt16(&data[2]);
			if (blkNum == sess->currBlock)
			{
				if (sess->isLast)
				{
					if (me->log->HasHandler())
					{
						Text::StringBuilderUTF8 sb;
						UOSInt i;
						sb.AppendC(UTF8STRC("End sending "));
						i = sess->fileName->LastIndexOf(IO::Path::PATH_SEPERATOR);
						sb.AppendC(&sess->fileName->v[i + 1], sess->fileName->leng - i - 1);
						sb.AppendC(UTF8STRC(" to "));
						sptr = Net::SocketUtil::GetAddrName(repBuff, addr, port).Or(repBuff);
						sb.AppendP(repBuff, sptr);
						me->log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Action);
					}
					me->sessMap.Remove(sess->sessId);
					me->ReleaseSess(sess);
				}
				else
				{
					UOSInt len;
					UInt8 *packet = MemAlloc(UInt8, sess->blockSize + 4);
					sess->currBlock++;
					WriteMInt16(&packet[0], 3);
					WriteMInt16(&packet[2], sess->currBlock);
					len = sess->stm->Read(Data::ByteArray(&packet[4], sess->blockSize));
					if (len != sess->blockSize)
					{
						sess->isLast = true;
					}
					me->dataSvr->SendTo(addr, port, packet, len + 4);
					MemFree(packet);
					sess->lastSignalTime = dt.ToTicks();
				}
			}
		}
		
	}
	mutUsage.EndUse();
}

UInt32 __stdcall Net::TFTPServer::CheckThread(AnyType userObj)
{
	NN<Net::TFTPServer> me = userObj.GetNN<Net::TFTPServer>();
	NN<SessionInfo> sess;
	UOSInt i;
	Int64 currTime;
	me->threadRunning = true;
	while (!me->threadToStop)
	{
		Sync::MutexUsage mutUsage(me->mut);
		currTime = Data::DateTimeUtil::GetCurrTimeMillis();
		i = me->sessMap.GetCount();
		while (i-- > 0)
		{
			sess = me->sessMap.GetItemNoCheck(i);
			if (currTime - sess->lastSignalTime >= 10000)
			{
				me->sessMap.Remove(sess->sessId);
				if (sess->isWrite)
				{
					sess->stm->Close();
					IO::Path::DeleteFile(sess->fileName->v);
				}
				me->ReleaseSess(sess);
			}
		}
		mutUsage.EndUse();
		me->chkEvt.Wait(10000);
	}
	me->threadRunning = false;
	return 0;
}

void Net::TFTPServer::ReleaseSess(NN<SessionInfo> sess)
{
	sess->fileName->Release();
	DEL_CLASS(sess->stm);
	MemFreeNN(sess);
}

Net::TFTPServer::TFTPServer(NN<Net::SocketFactory> sockf, UInt16 port, NN<IO::LogTool> log, Text::CStringNN path)
{
	this->log = log;
	this->svr = 0;
	this->dataSvr = 0;
	this->threadRunning = false;
	this->threadToStop = false;
	Text::StringBuilderUTF8 sb;
	sb.Append(path);
	if (!sb.EndsWith(IO::Path::PATH_SEPERATOR))
	{
		sb.AppendChar(IO::Path::PATH_SEPERATOR, 1);
	}
	this->path = Text::String::New(sb.ToString(), sb.GetLength());
	NEW_CLASS(this->dataSvr, Net::UDPServer(sockf, 0, 0, CSTR_NULL, OnDataPacket, this, log, CSTR("TFTP: "), 2, false));
	if (this->dataSvr->IsError())
	{
		DEL_CLASS(this->dataSvr);
		this->dataSvr = 0;
	}
	NEW_CLASS(this->svr, Net::UDPServer(sockf, 0, port, CSTR_NULL, OnCommandPacket, this, log, CSTR("TFTP: "), 2, false));
	if (this->svr->IsError())
	{
		DEL_CLASS(this->svr);
		this->svr = 0;
	}
	if (this->dataSvr && this->svr)
	{
		Sync::ThreadUtil::Create(CheckThread, this);
	}
}

Net::TFTPServer::~TFTPServer()
{
	SDEL_CLASS(this->svr);
	SDEL_CLASS(this->dataSvr);
	if (this->threadRunning)
	{
		this->threadToStop = true;
		this->chkEvt.Set();
		while (this->threadRunning)
		{
			Sync::SimpleThread::Sleep(1);
		}
	}
	UOSInt i;
	i = this->sessMap.GetCount();
	while (i-- > 0)
	{
		ReleaseSess(this->sessMap.GetItemNoCheck(i));
	}
	this->path->Release();
}

Bool Net::TFTPServer::IsError()
{
	return this->svr == 0 || this->dataSvr == 0;
}
