#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Data/DateTime.h"
#include "IO/Path.h"
#include "Net/TFTPServer.h"
#include "Sync/MutexUsage.h"
#include "Sync/Thread.h"
#include "Text/StringBuilderUTF8.h"

void __stdcall Net::TFTPServer::OnCommandPacket(const Net::SocketUtil::AddressInfo *addr, UInt16 port, const UInt8 *buff, UOSInt dataSize, void *userData)
{
	Net::TFTPServer *me = (Net::TFTPServer*)userData;
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
	UTF8Char *sptr;
	if (dataSize < 4)
		return;
	opcode = ReadMUInt16(buff);
	if (opcode != 1 && opcode != 2)
		return;
	if (buff[dataSize - 1])
		return;
	blkSize = 512;
	fileName = &buff[2];
	endPtr = &buff[dataSize];
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
	SessionInfo *sess;
	Sync::MutexUsage mutUsage(&me->mut);
	sess = me->sessMap.Get(sessId);
	mutUsage.EndUse();
	if (sess)
	{
		WriteMInt16(&repBuff[0], 5);
		WriteMInt16(&repBuff[2], 4);
		i = (UOSInt)(Text::StrConcatC(&repBuff[4], UTF8STRC("Already started")) - repBuff + 1);
		me->svr->SendTo(addr, port, repBuff, i);
		return;
	}
	
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
			sess = MemAlloc(SessionInfo, 1);
			sess->sessId = sessId;
			sess->lastSignalTime = dt.ToTicks();
			sess->stm = fs;
			sess->blockSize = blkSize;
			sess->isWrite = false;
			sess->isLast = false;
			sess->currBlock = 1;
			sess->fileName = Text::String::New(sb.ToString(), sb.GetLength());
			Sync::MutexUsage mutUsage(&me->mut);
			me->sessMap.Put(sess->sessId, sess);

			UInt8 *packet = MemAlloc(UInt8, sess->blockSize + 4);
			WriteMInt16(&packet[0], 3);
			WriteMInt16(&packet[2], sess->currBlock);
			len = sess->stm->Read(&packet[4], sess->blockSize);
			if (len != sess->blockSize)
			{
				sess->isLast = true;
			}
			mutUsage.EndUse();
			me->dataSvr->SendTo(addr, port, packet, len + 4);
			MemFree(packet);
			if (me->log)
			{
				sb.ClearStr();
				sb.AppendC(UTF8STRC("Sending "));
				sb.AppendC(fileName, fileNameLen);
				sb.AppendC(UTF8STRC(" to "));
				sptr = Net::SocketUtil::GetAddrName(repBuff, addr, port);
				sb.AppendP(repBuff, sptr);
				me->log->LogMessage(sb.ToCString(), IO::ILogHandler::LogLevel::Action);
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
			sess = MemAlloc(SessionInfo, 1);
			sess->sessId = sessId;
			sess->lastSignalTime = dt.ToTicks();
			sess->stm = fs;
			sess->blockSize = blkSize;
			sess->isWrite = true;
			sess->isLast = false;
			sess->currBlock = 0;
			sess->fileName = Text::String::New(sb.ToString(), sb.GetLength());
			Sync::MutexUsage mutUsage(&me->mut);
			me->sessMap.Put(sess->sessId, sess);
			mutUsage.EndUse();

			WriteMInt16(&repBuff[0], 4);
			WriteMInt16(&repBuff[2], 0);
			me->dataSvr->SendTo(addr, port, repBuff, 4);

			if (me->log)
			{
				sb.ClearStr();
				sb.AppendC(UTF8STRC("Receiving "));
				sb.AppendC(fileName, fileNameLen);
				sb.AppendC(UTF8STRC(" to "));
				sptr = Net::SocketUtil::GetAddrName(repBuff, addr, port);
				sb.AppendP(repBuff, sptr);
				me->log->LogMessage(sb.ToCString(), IO::ILogHandler::LogLevel::Action);
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

void __stdcall Net::TFTPServer::OnDataPacket(const Net::SocketUtil::AddressInfo *addr, UInt16 port, const UInt8 *buff, UOSInt dataSize, void *userData)
{
	Net::TFTPServer *me = (Net::TFTPServer*)userData;
	UInt64 sessId = (((UInt64)ReadMUInt32(addr->addr)) << 16) | port;
	SessionInfo *sess;
	UInt8 repBuff[32];
	UTF8Char *sptr;
	Sync::MutexUsage mutUsage(&me->mut);
	sess = me->sessMap.Get(sessId);
	if (sess == 0)
	{
		mutUsage.EndUse();
		if (ReadMUInt16(buff) == 3)
		{
			WriteMInt16(&repBuff[0], 4);
			WriteMInt16(&repBuff[2], ReadMUInt16(&buff[2]));
			me->dataSvr->SendTo(addr, port, repBuff, 4);
		}
		return;
	}
	Data::DateTime dt;
	dt.SetCurrTimeUTC();
	if (sess->isWrite)
	{
		if (ReadMUInt16(buff) == 3)
		{
			UInt16 blkNum = ReadMUInt16(&buff[2]);
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
				sess->stm->Write(&buff[4], dataSize - 4);
				if (dataSize - 4 != sess->blockSize)
				{
					if (me->log)
					{
						Text::StringBuilderUTF8 sb;
						UOSInt i;
						sb.AppendC(UTF8STRC("End receiving "));
						i = sess->fileName->LastIndexOf(IO::Path::PATH_SEPERATOR);
						sb.AppendC(&sess->fileName->v[i + 1], sess->fileName->leng - i - 1);
						sb.AppendC(UTF8STRC(" to "));
						sptr = Net::SocketUtil::GetAddrName(repBuff, addr, port);
						sb.AppendP(repBuff, sptr);
						me->log->LogMessage(sb.ToCString(), IO::ILogHandler::LogLevel::Action);
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
		if (ReadMUInt16(buff) == 4)
		{
			UInt16 blkNum = ReadMUInt16(&buff[2]);
			if (blkNum == sess->currBlock)
			{
				if (sess->isLast)
				{
					if (me->log)
					{
						Text::StringBuilderUTF8 sb;
						UOSInt i;
						sb.AppendC(UTF8STRC("End sending "));
						i = sess->fileName->LastIndexOf(IO::Path::PATH_SEPERATOR);
						sb.AppendC(&sess->fileName->v[i + 1], sess->fileName->leng - i - 1);
						sb.AppendC(UTF8STRC(" to "));
						sptr = Net::SocketUtil::GetAddrName(repBuff, addr, port);
						sb.AppendP(repBuff, sptr);
						me->log->LogMessage(sb.ToCString(), IO::ILogHandler::LogLevel::Action);
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
					len = sess->stm->Read(&packet[4], sess->blockSize);
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

UInt32 __stdcall Net::TFTPServer::CheckThread(void *userObj)
{
	Net::TFTPServer *me = (Net::TFTPServer*)userObj;
	SessionInfo *sess;
	UOSInt i;
	Int64 currTime;
	me->threadRunning = true;
	while (!me->threadToStop)
	{
		Sync::MutexUsage mutUsage(&me->mut);
		currTime = Data::DateTimeUtil::GetCurrTimeMillis();
		i = me->sessMap.GetCount();
		while (i-- > 0)
		{
			sess = me->sessMap.GetItem(i);
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

void Net::TFTPServer::ReleaseSess(SessionInfo *sess)
{
	sess->fileName->Release();
	DEL_CLASS(sess->stm);
	MemFree(sess);
}

Net::TFTPServer::TFTPServer(Net::SocketFactory *sockf, UInt16 port, IO::LogTool *log, Text::CString path)
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
		Sync::Thread::Create(CheckThread, this);
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
			Sync::Thread::Sleep(1);
		}
	}
	UOSInt i;
	i = this->sessMap.GetCount();
	while (i-- > 0)
	{
		ReleaseSess(this->sessMap.GetItem(i));
	}
	this->path->Release();
}

Bool Net::TFTPServer::IsError()
{
	return this->svr == 0 || this->dataSvr == 0;
}
