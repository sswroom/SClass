#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayList.h"
#include "Data/DateTime.h"
#include "IO/Stream.h"
#include "IO/Path.h"
#include "IO/FileStream.h"
#include "IO/Console.h"
#include "Manage/Process.h"
#include "Manage/MonConn.h"
#include "Net/SocketFactory.h"
#include "Net/TCPClient.h"
#include "Sync/Event.h"
#include "Sync/Interlocked.h"
#include "Sync/Mutex.h"
#include "Sync/MutexUsage.h"
#include "Sync/Thread.h"
#include "Text/Encoding.h"


#if defined(_WIN32)
#include <windows.h>
#endif

OSInt Manage::MonConn::BuildPacket(UInt8 *outbuff, UInt8 *data, OSInt dataSize, Int32 cmdType, Int32 cmdSeq)
{
	*(UInt16*)&outbuff[0] = *(UInt16*)"sM";
	*(UInt16*)&outbuff[2] = (UInt16)(dataSize + 10);
	*(UInt16*)&outbuff[4] = cmdType;
	*(UInt16*)&outbuff[6] = cmdSeq;
	MemCopyNO(&outbuff[8], data, dataSize);
	*(UInt16*)&outbuff[dataSize + 8] = CalCheck(outbuff);
	return dataSize + 10;
}

UInt16 Manage::MonConn::CalCheck(UInt8 *packet)
{
#ifdef HAS_ASM32
	_asm
	{
		mov edx,packet
		movzx ecx,word ptr [edx+2]
		mov eax,0
		sub ecx,2
mmccclop:
		mov bl,byte ptr [edx]
		add al,bl
		rcl ah,1
		inc edx
		xor ah,bl
		dec ecx
		jnz mmccclop
	}
#else
	Int32 size = *(UInt16*)&packet[2];
	UInt8 ChkDigit1 = 0;
	UInt8 ChkDigit2 = 0;
	Int32 i;
	Int32 j;
	ChkDigit2 = 0;
	i = 0;
	while (i < size - 2)
	{
		j = ChkDigit1 + packet[i];
		ChkDigit1 = j & 255;
		ChkDigit2 = ((ChkDigit2 << 1) | (j >> 8)) ^ packet[i];
	}
	return (((Int32)ChkDigit2) << 8) | ChkDigit1;
#endif
}

UInt8 *Manage::MonConn::FindPacket(UInt8 *buff, OSInt buffSize)
{
	Int32 i = 0;
	while (i < buffSize - 4)
	{
		if (*(UInt16*)&buff[i] == *(UInt16*)"sM")
		{
			Int32 psize = *(UInt16*)&buff[i + 2];
			if (psize <= 3000)
			{
				if (psize > buffSize - i)
					return &buff[i];

				UInt16 chkVal = CalCheck(&buff[i]);
				if (chkVal == *(UInt16*)&buff[i + psize - 2])
					return &buff[i];
			}
			i++;
		}
		i++;
	}
	return 0;
}

Bool Manage::MonConn::IsCompletePacket(UInt8 *buff, OSInt buffSize)
{
	Int32 packSize;
	if (buffSize < 10)
		return false;
	if (*(UInt16*)&buff[0] != *(UInt16*)"sM")
		return false;
	packSize = *(UInt16*)&buff[2];
	if (packSize > 3000)
		return false;
	if (packSize > buffSize)
		return false;
	if (*(UInt16*)&buff[packSize - 2] == CalCheck(buff))
		return true;
	return false;
}

void Manage::MonConn::ParsePacket(UInt8 *buff, UInt16 *cmdSize, UInt16 *cmdType, UInt16 *cmdSeq, UInt8** cmdData)
{
	*cmdSize = *(UInt16*)&buff[2];
	*cmdType = *(UInt16*)&buff[4];
	*cmdSeq = *(UInt16*)&buff[6];
	*cmdData = &buff[8];
}

UInt32 __stdcall Manage::MonConn::ConnRThread(void *conn)
{
	Manage::MonConn *me = (Manage::MonConn*)conn;
	UInt8 *dataBuff;
	OSInt buffSize;

	me->ConnRRunning = true;
	dataBuff = MemAlloc(UInt8, 3000);

	while (!me->ToStop || me->ConnTRunning)
	{
		if (me->cli == 0)
		{
			me->cliErr = true;
			NEW_CLASS(me->cli, Net::TCPClient(me->sockf, (const UTF8Char*)"127.0.0.1", me->port));
			me->cliErr = me->cli->IsConnectError();
			if (!me->cliErr)
				me->svrMonConn++;
			me->requesting = false;
		}

		if (me->cliErr)
		{
			DEL_CLASS(me->cli);
			me->cli = 0;
		}

		if (me->cli)
		{
			buffSize = me->cli->Read(dataBuff, 3000);
			if (buffSize)
			{
				UInt8 *packet = dataBuff;
				while ((packet = Manage::MonConn::FindPacket(packet, buffSize - (packet - dataBuff))) != 0)
				{
					if (Manage::MonConn::IsCompletePacket(packet, buffSize - (packet - dataBuff)))
					{
						UInt16 cmdSize;
						UInt16 cmdType;
						UInt16 cmdSeq;
						UInt8 *data;
						Manage::MonConn::ParsePacket(packet, &cmdSize, &cmdType, &cmdSeq, &data);

						if (cmdType & 0x8000)
						{
							UInt8 *lastCmd;
							if (me->cmdList->GetCount())
							{
								lastCmd = (UInt8*)me->cmdList->GetItem(0);
								if ((cmdType & 0x7fff) == *(UInt16*)&lastCmd[4] && cmdSeq == *(UInt16*)&lastCmd[6])
								{
									MemFree(me->cmdList->RemoveAt(0));
									me->requesting = false;
									me->connTEvt->Set();


									if (cmdType == 0x8000)
									{
										if (*(UInt16*)data != 0)
										{
											me->hdlr(Manage::MON_EVT_PROCESS_START_ERR, *(UInt16*)data, me->userObj);
										}
									}
								}
							}
						}
						else
						{

						}

						packet = packet + cmdSize;
					}
					else
					{
						break;
					}
				}
			}
			else
			{
				me->cliErr = true;
			}
		}
		else
		{
			me->connREvt->Wait(1000);
		}
	}
	if (me->cli)
	{
		DEL_CLASS(me->cli);
		me->cli = 0;
	}

	MemFree(dataBuff);
	me->ConnRRunning = false;
	me->msgWriter->WriteLine((const UTF8Char*)"MonConn RThread Stopped");
	return 0;
}

UInt32 __stdcall Manage::MonConn::ConnTThread(void *conn)
{
	Manage::MonConn *me = (Manage::MonConn*)conn;
	UInt8 *data;
	me->ConnTRunning = true;
	while (true)
	{
		if (me->ToStop)
		{
			if (me->cmdList->GetCount() == 0)
				break;
		}

		if (me->cli && !me->cliErr)
		{
			Data::DateTime currTime;
			currTime.SetCurrTimeUTC();
			if (me->requesting)
			{
				if (currTime.DiffMS(me->lastReqTime) > 60000)//3000
				{
					me->requesting = false;
				}
			}
			else
			{
				if (currTime.DiffMS(me->lastKATime) > 60000)
				{
					UOSInt procId = Manage::Process::GetCurrProcId();
					me->AddCommand((UInt8*)&procId, 4, 5);
					me->lastKATime->SetCurrTimeUTC();
				}

				if (me->cmdList->GetCount())
				{
					data = (UInt8*)me->cmdList->GetItem(0);
					me->requesting = true;
					me->lastReqTime->SetCurrTimeUTC();
					me->cli->Write(data, *(UInt16*)&data[2]);
				}
			}
		}
		me->connTEvt->Wait(500);
	}
	if (me->cli)
	{
		me->cli->Close();
	}
	if (me->connREvt)
		me->connREvt->Set();
	me->ConnTRunning = false;
	me->msgWriter->WriteLine((const UTF8Char*)"MonConn TThread Stopped");
	return 0;
}

void Manage::MonConn::AddCommand(UInt8 *data, OSInt dataSize, Int32 cmdType)
{
	UInt8 *buff = MemAlloc(UInt8, dataSize + 10);
	Sync::MutexUsage mutUsage(cmdSeqMut);
	Manage::MonConn::BuildPacket(buff, data, dataSize, cmdType, cmdSeq++);
	mutUsage.EndUse();
	cmdList->Add(buff);
	connTEvt->Set();
}

Manage::MonConn::MonConn(EventHandler hdlr, void *userObj, Net::SocketFactory *sockf, IO::IWriter *msgWriter)
{
	UTF8Char buff[256];
	NEW_CLASS(lastReqTime, Data::DateTime());
	NEW_CLASS(cmdList, Data::SyncArrayList<UInt8*>());
	NEW_CLASS(cmdSeqMut, Sync::Mutex());
	NEW_CLASS(lastKATime, Data::DateTime());
	lastKATime->SetCurrTimeUTC();
	this->msgWriter = msgWriter;
	this->userObj = userObj;
	this->hdlr = hdlr;
	this->sockf = sockf;
	this->ConnTRunning = false;
	this->ConnRRunning = false;
	this->ToStop = false;
	this->svrMonConn = 0;
	this->port = 0;
	this->cli = 0;
	this->cmdSeq = 0;
	this->connREvt = 0;
	this->connTEvt = 0;
#if defined(_WIN32)
	SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOALIGNMENTFAULTEXCEPT | SEM_NOGPFAULTERRORBOX);
#endif

	IO::Path::GetTempFile(buff, (const UTF8Char*)"SvrMonitor.dat");
	IO::FileStream *file;
	NEW_CLASS(file, IO::FileStream(buff, IO::FileStream::FILE_MODE_READONLY, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
	if (!file->IsError())
	{
		file->Read((UInt8*)&this->port, 2);
	}
	DEL_CLASS(file);
	if (this->port)
	{
		NEW_CLASS(this->connREvt, Sync::Event((const UTF8Char*)"Manage.MonConn.ConnREvt"));
		NEW_CLASS(this->connTEvt, Sync::Event((const UTF8Char*)"Manage.MonConn.ConnTEvt"));
		Sync::Thread::Create(ConnRThread, this);
		Sync::Thread::Create(ConnTThread, this);
		while (!this->ConnTRunning || !this->ConnRRunning)
			Sync::Thread::Sleep(10);
		Int32 i = 30;
		while (i-- > 0)
		{
			if (this->svrMonConn)
				break;
			Sync::Thread::Sleep(100);
		}
	}
}

Manage::MonConn::~MonConn()
{
	this->ToStop = true;
	if (this->connTEvt)
		this->connTEvt->Set();

	while (this->ConnRRunning)
	{
		Sync::Thread::Sleep(10);
	}
	while (this->ConnTRunning)
	{
		Sync::Thread::Sleep(10);
	}
	if (this->connREvt)
	{
		DEL_CLASS(this->connREvt);
		this->connREvt = 0;
	}
	if (this->connTEvt)
	{
		DEL_CLASS(this->connTEvt);
		this->connTEvt = 0;
	}
	DEL_CLASS(cmdSeqMut);
	OSInt i = cmdList->GetCount();
	while (i-- > 0)
	{
		MemFree(cmdList->RemoveAt(i));
	}
	DEL_CLASS(lastKATime);
	DEL_CLASS(cmdList);
	DEL_CLASS(lastReqTime);
}

Bool Manage::MonConn::IsError()
{
	return this->ConnRRunning == 0 || this->ConnTRunning == 0 || this->svrMonConn == 0;
}

void Manage::MonConn::StartProcess(Int32 name)
{
	UOSInt procId = Manage::Process::GetCurrProcId();
	UInt8 buff[8];
	*(Int32*)&buff[0] = (Int32)(OSInt)procId;
	*(Int32*)&buff[4] = name;
	AddCommand(buff, 8, 0);
}

void Manage::MonConn::EndProcess()
{
	UOSInt procId = Manage::Process::GetCurrProcId();
	UInt8 buff[4];
	*(Int32*)&buff[0] = (Int32)(OSInt)procId;
	AddCommand(buff, 4, 1);
}

void Manage::MonConn::StartTCPPort(Int32 portNum)
{
	UOSInt procId = Manage::Process::GetCurrProcId();
	UInt8 buff[6];
	*(Int32*)&buff[0] = (Int32)(OSInt)procId;
	*(UInt16*)&buff[4] = portNum;
	AddCommand(buff, 6, 2);
}

void Manage::MonConn::StartUDPPort(Int32 portNum)
{
	UOSInt procId = Manage::Process::GetCurrProcId();
	UInt8 buff[4];
	*(Int32*)&buff[0] = (Int32)(OSInt)procId;
	*(UInt16*)&buff[4] = portNum;
	AddCommand(buff, 6, 3);
}

void Manage::MonConn::AddLogMessage(Int32 name, Int32 name2, Int32 logLevel, const UTF8Char *msg)
{
	UOSInt procId = Manage::Process::GetCurrProcId();
	OSInt strSize = Text::StrCharCnt(msg);
	UInt8 *buff = MemAlloc(UInt8, 14 + strSize + 1);
	*(Int32*)&buff[0] = (Int32)(OSInt)procId;
	*(Int32*)&buff[4] = name;
	*(Int32*)&buff[8] = name2;
	*(UInt16*)&buff[12] = logLevel;
	Text::StrConcat((UTF8Char*)&buff[14], msg);
	AddCommand(buff, 11 + strSize, 4);
	MemFree(buff);
}
