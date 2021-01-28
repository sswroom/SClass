#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Data/DateTime.h"
#include "IO/Path.h"
#include "Net/TFTPServer.h"
#include "Sync/Thread.h"
#include "Text/StringBuilderUTF8.h"

void __stdcall Net::TFTPServer::OnCommandPacket(const Net::SocketUtil::AddressInfo *addr, UInt16 port, const UInt8 *buff, UOSInt dataSize, void *userData)
{
	Net::TFTPServer *me = (Net::TFTPServer*)userData;
	const UTF8Char *fileName;
	const UTF8Char *mode;
	const UTF8Char *endPtr;
//	const UTF8Char *optName;
//	const UTF8Char *optVal;
	UInt16 opcode;
	UOSInt blkSize;
	UOSInt len;
	OSInt i;
	UInt8 repBuff[32];
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
	len = Text::StrCharCnt(fileName);
	mode = &fileName[len + 1];
	if (mode >= endPtr)
		return;
	
	if (!Text::StrEquals(mode, (const UTF8Char*)"octet"))
	{
		WriteMInt16(&repBuff[0], 5);
		WriteMInt16(&repBuff[2], 0);
		i = Text::StrConcat(&repBuff[4], (const UTF8Char*)"Mode not supported") - repBuff + 1;
		me->svr->SendTo(addr, port, repBuff, i);
		return;
	}
	if (Text::StrIndexOf(fileName, IO::Path::PATH_SEPERATOR) >= 0)
	{
		WriteMInt16(&repBuff[0], 5);
		WriteMInt16(&repBuff[2], 2);
		i = Text::StrConcat(&repBuff[4], (const UTF8Char*)"Access violation") - repBuff + 1;
		me->svr->SendTo(addr, port, repBuff, i);
		return;
	}
	Int64 sessId = (((UInt64)ReadMUInt32(addr->addr)) << 16) | port;
	SessionInfo *sess;
	me->mut->Lock();
	sess = me->sessMap->Get(sessId);
	me->mut->Unlock();
	if (sess)
	{
		WriteMInt16(&repBuff[0], 5);
		WriteMInt16(&repBuff[2], 4);
		i = Text::StrConcat(&repBuff[4], (const UTF8Char*)"Already started") - repBuff + 1;
		me->svr->SendTo(addr, port, repBuff, i);
		return;
	}
	
	Text::StringBuilderUTF8 sb;
	sb.Append(me->path);
	sb.Append(fileName);
	IO::Path::PathType pt;
	IO::FileStream *fs;
	pt = IO::Path::GetPathType(sb.ToString());
	if (opcode == 1)
	{
		if (pt == IO::Path::PT_FILE)
		{
			NEW_CLASS(fs, IO::FileStream(sb.ToString(), IO::FileStream::FILE_MODE_READONLY, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
			if (fs->IsError())
			{
				DEL_CLASS(fs);
				WriteMInt16(&repBuff[0], 5);
				WriteMInt16(&repBuff[2], 2);
				i = Text::StrConcat(&repBuff[4], (const UTF8Char*)"Cannot open file") - repBuff + 1;
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
			sess->fileName = Text::StrCopyNew(sb.ToString());
			me->mut->Lock();
			me->sessMap->Put(sess->sessId, sess);

			UInt8 *packet = MemAlloc(UInt8, sess->blockSize + 4);
			WriteMInt16(&packet[0], 3);
			WriteMInt16(&packet[2], sess->currBlock);
			len = sess->stm->Read(&packet[4], sess->blockSize);
			if (len != sess->blockSize)
			{
				sess->isLast = true;
			}
			me->mut->Unlock();
			me->dataSvr->SendTo(addr, port, packet, len + 4);
			MemFree(packet);
			if (me->log)
			{
				sb.ClearStr();
				sb.Append((const UTF8Char*)"Sending ");
				sb.Append(fileName);
				sb.Append((const UTF8Char*)" to ");
				Net::SocketUtil::GetAddrName(repBuff, addr, port);
				sb.Append(repBuff);
				me->log->LogMessage(sb.ToString(), IO::ILogHandler::LOG_LEVEL_ACTION);
			}
		}
		else
		{
			WriteMInt16(&repBuff[0], 5);
			WriteMInt16(&repBuff[2], 1);
			i = Text::StrConcat(&repBuff[4], (const UTF8Char*)"File not found") - repBuff + 1;
			me->svr->SendTo(addr, port, repBuff, i);
			return;
		}
	}
	else if (opcode == 2)
	{
		if (pt == IO::Path::PT_FILE)
		{
			WriteMInt16(&repBuff[0], 5);
			WriteMInt16(&repBuff[2], 6);
			i = Text::StrConcat(&repBuff[4], (const UTF8Char*)"File already exists") - repBuff + 1;
			me->svr->SendTo(addr, port, repBuff, i);
			return;
		}
		else if (pt == IO::Path::PT_DIRECTORY)
		{
			WriteMInt16(&repBuff[0], 5);
			WriteMInt16(&repBuff[2], 6);
			i = Text::StrConcat(&repBuff[4], (const UTF8Char*)"Directory already exists") - repBuff + 1;
			me->svr->SendTo(addr, port, repBuff, i);
			return;
		}
		else
		{
			NEW_CLASS(fs, IO::FileStream(sb.ToString(), IO::FileStream::FILE_MODE_CREATE, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
			if (fs->IsError())
			{
				DEL_CLASS(fs);
				WriteMInt16(&repBuff[0], 5);
				WriteMInt16(&repBuff[2], 2);
				i = Text::StrConcat(&repBuff[4], (const UTF8Char*)"Cannot open file") - repBuff + 1;
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
			sess->fileName = Text::StrCopyNew(sb.ToString());
			me->mut->Lock();
			me->sessMap->Put(sess->sessId, sess);
			me->mut->Unlock();

			WriteMInt16(&repBuff[0], 4);
			WriteMInt16(&repBuff[2], 0);
			me->dataSvr->SendTo(addr, port, repBuff, 4);

			if (me->log)
			{
				sb.ClearStr();
				sb.Append((const UTF8Char*)"Receiving ");
				sb.Append(fileName);
				sb.Append((const UTF8Char*)" to ");
				Net::SocketUtil::GetAddrName(repBuff, addr, port);
				sb.Append(repBuff);
				me->log->LogMessage(sb.ToString(), IO::ILogHandler::LOG_LEVEL_ACTION);
			}
		}
	}
	else
	{
		WriteMInt16(&repBuff[0], 5);
		WriteMInt16(&repBuff[2], 4);
		i = Text::StrConcat(&repBuff[4], (const UTF8Char*)"Unknown opcode") - repBuff + 1;
		me->svr->SendTo(addr, port, repBuff, i);
		return;
	}
	
}

void __stdcall Net::TFTPServer::OnDataPacket(const Net::SocketUtil::AddressInfo *addr, UInt16 port, const UInt8 *buff, UOSInt dataSize, void *userData)
{
	Net::TFTPServer *me = (Net::TFTPServer*)userData;
	Int64 sessId = (((UInt64)ReadMUInt32(addr->addr)) << 16) | port;
	SessionInfo *sess;
	UInt8 repBuff[32];
	me->mut->Lock();
	sess = me->sessMap->Get(sessId);
	if (sess == 0)
	{
		me->mut->Unlock();
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
						OSInt i;
						sb.Append((const UTF8Char*)"End receiving ");
						i = Text::StrLastIndexOf(sess->fileName, IO::Path::PATH_SEPERATOR);
						sb.Append(&sess->fileName[i + 1]);
						sb.Append((const UTF8Char*)" to ");
						Net::SocketUtil::GetAddrName(repBuff, addr, port);
						sb.Append(repBuff);
						me->log->LogMessage(sb.ToString(), IO::ILogHandler::LOG_LEVEL_ACTION);
					}
					me->sessMap->Remove(sess->sessId);
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
						OSInt i;
						sb.Append((const UTF8Char*)"End sending ");
						i = Text::StrLastIndexOf(sess->fileName, IO::Path::PATH_SEPERATOR);
						sb.Append(&sess->fileName[i + 1]);
						sb.Append((const UTF8Char*)" to ");
						Net::SocketUtil::GetAddrName(repBuff, addr, port);
						sb.Append(repBuff);
						me->log->LogMessage(sb.ToString(), IO::ILogHandler::LOG_LEVEL_ACTION);
					}
					me->sessMap->Remove(sess->sessId);
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
	me->mut->Unlock();
}

UInt32 __stdcall Net::TFTPServer::CheckThread(void *userObj)
{
	Net::TFTPServer *me = (Net::TFTPServer*)userObj;
	SessionInfo *sess;
	Data::ArrayList<SessionInfo*> *sessList;
	OSInt i;
	Data::DateTime *dt;
	Int64 currTime;
	me->threadRunning = true;
	NEW_CLASS(dt, Data::DateTime());
	while (!me->threadToStop)
	{
		me->mut->Lock();
		dt->SetCurrTimeUTC();
		currTime = dt->ToTicks();
		sessList = me->sessMap->GetValues();
		i = sessList->GetCount();
		while (i-- > 0)
		{
			sess = sessList->GetItem(i);
			if (currTime - sess->lastSignalTime >= 10000)
			{
				me->sessMap->Remove(sess->sessId);
				if (sess->isWrite)
				{
					sess->stm->Close();
					IO::Path::DeleteFile(sess->fileName);
				}
				me->ReleaseSess(sess);
			}
		}
		me->mut->Unlock();
		me->chkEvt->Wait(10000);
	}
	DEL_CLASS(dt);
	me->threadRunning = false;
	return 0;
}

void Net::TFTPServer::ReleaseSess(SessionInfo *sess)
{
	Text::StrDelNew(sess->fileName);
	DEL_CLASS(sess->stm);
	MemFree(sess);
}

Net::TFTPServer::TFTPServer(Net::SocketFactory *sockf, UInt16 port, IO::LogTool *log, const UTF8Char *path)
{
	this->log = log;
	this->svr = 0;
	this->dataSvr = 0;
	this->threadRunning = false;
	this->threadToStop = false;
	NEW_CLASS(this->chkEvt, Sync::Event(true, (const UTF8Char*)"Net.TFTPServer.chkEvt"));
	NEW_CLASS(this->mut, Sync::Mutex());
	NEW_CLASS(this->sessMap, Data::Integer64Map<SessionInfo*>());
	Text::StringBuilderUTF8 sb;
	sb.Append(path);
	if (!sb.EndsWith(IO::Path::PATH_SEPERATOR))
	{
		sb.AppendChar(IO::Path::PATH_SEPERATOR, 1);
	}
	this->path = Text::StrCopyNew(sb.ToString());
	NEW_CLASS(this->dataSvr, Net::UDPServer(sockf, 0, 0, 0, OnDataPacket, this, log, (const UTF8Char*)"TFTP: ", 2, false));
	if (this->dataSvr->IsError())
	{
		DEL_CLASS(this->dataSvr);
		this->dataSvr = 0;
	}
	NEW_CLASS(this->svr, Net::UDPServer(sockf, 0, port, 0, OnCommandPacket, this, log, (const UTF8Char*)"TFTP: ", 2, false));
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
		this->chkEvt->Set();
		while (this->threadRunning)
		{
			Sync::Thread::Sleep(1);
		}
	}
	Data::ArrayList<SessionInfo*> *sessList;
	OSInt i;
	sessList = this->sessMap->GetValues();
	i = sessList->GetCount();
	while (i-- > 0)
	{
		ReleaseSess(sessList->GetItem(i));
	}
	DEL_CLASS(this->mut);
	DEL_CLASS(this->sessMap);
	Text::StrDelNew(this->path);
}

Bool Net::TFTPServer::IsError()
{
	return this->svr == 0 || this->dataSvr == 0;
}
