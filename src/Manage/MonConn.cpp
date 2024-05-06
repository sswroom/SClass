#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayList.h"
#include "Data/ByteBuffer.h"
#include "Data/ByteTool.h"
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
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"
#include "Text/Encoding.h"


#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif

UOSInt Manage::MonConn::BuildPacket(UInt8 *outbuff, UInt8 *data, UOSInt dataSize, UInt16 cmdType, UInt16 cmdSeq)
{
	WriteNUInt16(&outbuff[0], ReadNUInt16((const UInt8*)"sM"));
	WriteUInt16(&outbuff[2], (UInt16)(dataSize + 10));
	WriteUInt16(&outbuff[4], cmdType);
	WriteUInt16(&outbuff[6], cmdSeq);
	MemCopyNO(&outbuff[8], data, dataSize);
	WriteUInt16(&outbuff[dataSize + 8], CalCheck(outbuff));
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
	UInt32 size = *(UInt16*)&packet[2];
	UInt8 chkDigit1 = 0;
	UInt8 chkDigit2 = 0;
	UInt32 i;
	UInt32 j;
	chkDigit2 = 0;
	i = 0;
	while (i < size - 2)
	{
		j = (UInt32)chkDigit1 + packet[i];
		chkDigit1 = (UInt8)(j & 255);
		chkDigit2 = (UInt8)(((UInt32)(chkDigit2 << 1) | (j >> 8)) ^ packet[i]);
	}
	return (UInt16)((((UInt32)chkDigit2) << 8) | chkDigit1);
#endif
}

UInt8 *Manage::MonConn::FindPacket(UInt8 *buff, UOSInt buffSize)
{
	UOSInt i = 0;
	while (i < buffSize - 4)
	{
		if (ReadNUInt16(&buff[i]) == ReadNUInt16((const UInt8*)"sM"))
		{
			UInt32 psize = ReadUInt16(&buff[i + 2]);
			if (psize <= 3000)
			{
				if (psize > buffSize - i)
					return &buff[i];

				UInt16 chkVal = CalCheck(&buff[i]);
				if (chkVal == ReadUInt16(&buff[i + psize - 2]))
					return &buff[i];
			}
			i++;
		}
		i++;
	}
	return 0;
}

Bool Manage::MonConn::IsCompletePacket(UInt8 *buff, UOSInt buffSize)
{
	UInt32 packSize;
	if (buffSize < 10)
		return false;
	if (ReadNUInt16(&buff[0]) != ReadNUInt16((const UInt8*)"sM"))
		return false;
	packSize = ReadUInt16(&buff[2]);
	if (packSize > 3000)
		return false;
	if (packSize > buffSize)
		return false;
	if (ReadUInt16(&buff[packSize - 2]) == CalCheck(buff))
		return true;
	return false;
}

void Manage::MonConn::ParsePacket(UInt8 *buff, UInt16 *cmdSize, UInt16 *cmdType, UInt16 *cmdSeq, UInt8** cmdData)
{
	*cmdSize = ReadUInt16(&buff[2]);
	*cmdType = ReadUInt16(&buff[4]);
	*cmdSeq = ReadUInt16(&buff[6]);
	*cmdData = &buff[8];
}

UInt32 __stdcall Manage::MonConn::ConnRThread(AnyType conn)
{
	NN<Manage::MonConn> me = conn.GetNN<Manage::MonConn>();
	UOSInt buffSize;
	me->ConnRRunning = true;
	{
		Data::ByteBuffer dataBuff(3000);

		while (!me->ToStop || me->ConnTRunning)
		{
			if (me->cli == 0)
			{
				me->cliErr = true;
				NEW_CLASS(me->cli, Net::TCPClient(me->sockf, CSTR("127.0.0.1"), me->port, me->timeout));
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
				buffSize = me->cli->Read(dataBuff);
				if (buffSize)
				{
					UInt8 *packet = dataBuff.Ptr().Ptr();
					while ((packet = Manage::MonConn::FindPacket(packet, buffSize - (UOSInt)(packet - dataBuff.Ptr()))) != 0)
					{
						if (Manage::MonConn::IsCompletePacket(packet, buffSize - (UOSInt)(packet - dataBuff.Ptr())))
						{
							UInt16 cmdSize;
							UInt16 cmdType;
							UInt16 cmdSeq;
							UInt8 *data;
							Manage::MonConn::ParsePacket(packet, &cmdSize, &cmdType, &cmdSeq, &data);

							if (cmdType & 0x8000)
							{
								UInt8 *lastCmd;
								if (me->cmdList.GetCount())
								{
									lastCmd = me->cmdList.GetItem(0);
									if ((cmdType & 0x7fff) == ReadUInt16(&lastCmd[4]) && cmdSeq == ReadUInt16(&lastCmd[6]))
									{
										MemFree(me->cmdList.RemoveAt(0));
										me->requesting = false;
										me->connTEvt->Set();


										if (cmdType == 0x8000)
										{
											if (ReadUInt16(data) != 0)
											{
												me->hdlr(Manage::MON_EVT_PROCESS_START_ERR, ReadUInt16(data), me->userObj);
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
	}
	me->ConnRRunning = false;
	me->msgWriter->WriteLine(CSTR("MonConn RThread Stopped"));
	return 0;
}

UInt32 __stdcall Manage::MonConn::ConnTThread(AnyType conn)
{
	NN<Manage::MonConn> me = conn.GetNN<Manage::MonConn>();
	UInt8 *data;
	me->ConnTRunning = true;
	while (true)
	{
		if (me->ToStop)
		{
			if (me->cmdList.GetCount() == 0)
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
					me->lastKATime.SetCurrTimeUTC();
				}

				if (me->cmdList.GetCount())
				{
					data = me->cmdList.GetItem(0);
					me->requesting = true;
					me->lastReqTime.SetCurrTimeUTC();
					me->cli->Write(data, ReadUInt16(&data[2]));
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
	me->msgWriter->WriteLine(CSTR("MonConn TThread Stopped"));
	return 0;
}

void Manage::MonConn::AddCommand(UInt8 *data, UOSInt dataSize, UInt16 cmdType)
{
	UInt8 *buff = MemAlloc(UInt8, dataSize + 10);
	Sync::MutexUsage mutUsage(this->cmdSeqMut);
	Manage::MonConn::BuildPacket(buff, data, dataSize, cmdType, cmdSeq++);
	mutUsage.EndUse();
	this->cmdList.Add(buff);
	connTEvt->Set();
}

Manage::MonConn::MonConn(EventHandler hdlr, AnyType userObj, NN<Net::SocketFactory> sockf, NN<IO::Writer> msgWriter, Data::Duration timeout)
{
	UTF8Char buff[256];
	UTF8Char *sptr;
	this->lastKATime.SetCurrTimeUTC();
	this->msgWriter = msgWriter;
	this->userObj = userObj;
	this->timeout = timeout;
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
#if defined(_WIN32) || defined(_WIN64)
	SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOALIGNMENTFAULTEXCEPT | SEM_NOGPFAULTERRORBOX);
#endif

	sptr = IO::Path::GetTempFile(buff, UTF8STRC("SvrMonitor.dat"));
	{
		IO::FileStream file(CSTRP(buff, sptr), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		if (!file.IsError())
		{
			file.Read(Data::ByteArray((UInt8*)&this->port, 2));
		}
	}
	if (this->port)
	{
		NEW_CLASS(this->connREvt, Sync::Event());
		NEW_CLASS(this->connTEvt, Sync::Event());
		Sync::ThreadUtil::Create(ConnRThread, this);
		Sync::ThreadUtil::Create(ConnTThread, this);
		while (!this->ConnTRunning || !this->ConnRRunning)
			Sync::SimpleThread::Sleep(10);
		UInt32 i = 30;
		while (i-- > 0)
		{
			if (this->svrMonConn)
				break;
			Sync::SimpleThread::Sleep(100);
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
		Sync::SimpleThread::Sleep(10);
	}
	while (this->ConnTRunning)
	{
		Sync::SimpleThread::Sleep(10);
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
	UOSInt i = this->cmdList.GetCount();
	while (i-- > 0)
	{
		MemFree(this->cmdList.RemoveAt(i));
	}
}

Bool Manage::MonConn::IsError()
{
	return this->ConnRRunning == 0 || this->ConnTRunning == 0 || this->svrMonConn == 0;
}

void Manage::MonConn::StartProcess(Int32 name)
{
	UOSInt procId = Manage::Process::GetCurrProcId();
	UInt8 buff[8];
	WriteUInt32(&buff[0], (UInt32)procId);
	WriteInt32(&buff[4], name);
	AddCommand(buff, 8, 0);
}

void Manage::MonConn::EndProcess()
{
	UOSInt procId = Manage::Process::GetCurrProcId();
	UInt8 buff[4];
	WriteUInt32(&buff[0], (UInt32)procId);
	AddCommand(buff, 4, 1);
}

void Manage::MonConn::StartTCPPort(UInt16 portNum)
{
	UOSInt procId = Manage::Process::GetCurrProcId();
	UInt8 buff[6];
	WriteUInt32(&buff[0], (UInt32)procId);
	WriteUInt16(&buff[4], portNum);
	AddCommand(buff, 6, 2);
}

void Manage::MonConn::StartUDPPort(UInt16 portNum)
{
	UOSInt procId = Manage::Process::GetCurrProcId();
	UInt8 buff[6];
	WriteUInt32(&buff[0], (UInt32)procId);
	WriteUInt16(&buff[4], portNum);
	AddCommand(buff, 6, 3);
}

void Manage::MonConn::AddLogMessage(Int32 name, Int32 name2, UInt16 logLevel, Text::CString msg)
{
	UOSInt procId = Manage::Process::GetCurrProcId();
	UInt8 *buff = MemAlloc(UInt8, 14 + msg.leng + 1);
	WriteUInt32(&buff[0], (UInt32)procId);
	WriteInt32(&buff[4], name);
	WriteInt32(&buff[8], name2);
	WriteUInt16(&buff[12], logLevel);
	msg.ConcatTo((UTF8Char*)&buff[14]);
	AddCommand(buff, 14 + msg.leng, 4);
	MemFree(buff);
}
