#include "Stdafx.h"
#include "IO/SerialPort.h"
#include "IO/Device/AXCAN.h"
#include "Sync/MutexUsage.h"
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"
#include "Text/UTF8Reader.h"

#define CMDTIMEOUT 1000
//#define VERBOSE
#if defined(VERBOSE)
#include <stdio.h>
#endif

Bool IO::Device::AXCAN::SendSetCANBitRate(CANBitRate bitRate)
{
	switch (bitRate)
	{
	case CANBitRate::BR_5K:
		return SendCommand(CSTR("@B0"), CMDTIMEOUT);
	case CANBitRate::BR_10K:
		return SendCommand(CSTR("@B1"), CMDTIMEOUT);
	case CANBitRate::BR_20K:
		return SendCommand(CSTR("@B2"), CMDTIMEOUT);
	case CANBitRate::BR_40K:
		return SendCommand(CSTR("@B3"), CMDTIMEOUT);
	case CANBitRate::BR_50K:
		return SendCommand(CSTR("@B4"), CMDTIMEOUT);
	case CANBitRate::BR_80K:
		return SendCommand(CSTR("@B5"), CMDTIMEOUT);
	case CANBitRate::BR_100K:
		return SendCommand(CSTR("@B6"), CMDTIMEOUT);
	case CANBitRate::BR_125K:
		return SendCommand(CSTR("@B7"), CMDTIMEOUT);
	case CANBitRate::BR_200K:
		return SendCommand(CSTR("@B8"), CMDTIMEOUT);
	case CANBitRate::BR_250K:
		return SendCommand(CSTR("@B9"), CMDTIMEOUT);
	case CANBitRate::BR_400K:
		return SendCommand(CSTR("@BA"), CMDTIMEOUT);
	case CANBitRate::BR_500K:
		return SendCommand(CSTR("@BB"), CMDTIMEOUT);
	case CANBitRate::BR_600K:
		return SendCommand(CSTR("@BC"), CMDTIMEOUT);
	case CANBitRate::BR_800K:
		return SendCommand(CSTR("@BD"), CMDTIMEOUT);
	case CANBitRate::BR_1000K:
		return SendCommand(CSTR("@BE"), CMDTIMEOUT);
	default:
		return false;
	}
}

Bool IO::Device::AXCAN::SendOpenCANPort(UInt8 port, Bool silentMode, Bool loopback)
{
	UTF8Char sbuff[5];
	if (port != 1)
		return false;
	sbuff[0] = '@';
	sbuff[1] = 'O';
	sbuff[2] = '1';
	sbuff[3] = silentMode?'1':'0';
	sbuff[4] = loopback?'1':'0';
	return SendCommand(Text::CString(sbuff, 5), CMDTIMEOUT);
}

Bool IO::Device::AXCAN::SendCloseCANPort(UInt8 port)
{
	if (port != 1)
		return false;
	return SendCommand(CSTR("@C1"), CMDTIMEOUT);
}

Bool IO::Device::AXCAN::SendSetReportMode(Bool autoMode, Bool formattedData, Bool overwriteOnFull)
{
	UTF8Char sbuff[3];
	UOSInt v = 0;
	if (autoMode) v |= 1;
	if (formattedData) v |= 2;
	if (overwriteOnFull) v |= 4;
	sbuff[0] = '@';
	sbuff[1] = 'S';
	sbuff[2] = (UInt8)(v + '0');
	return SendCommand(Text::CString(sbuff, 3), (v == 0)?CMDTIMEOUT:0);
}

Bool IO::Device::AXCAN::SendCommandMode()
{
	return SendCommand(CSTR("+++"), CMDTIMEOUT);
}

Bool IO::Device::AXCAN::SendCommand(Text::CString cmd, UOSInt timeout)
{
	UInt8 buff[64];
	if (cmd.leng > 63)
		return false;
	cmd.ConcatTo(buff);
#if defined(VERBOSE)
	printf("AXCAN Send: %s\r\n", buff);
#endif
	buff[cmd.leng] = 13;
	if (this->stm == 0)
		return false;
	if (timeout == 0)
		return this->stm->Write(buff, cmd.leng + 1) == cmd.leng + 1;
	Sync::MutexUsage mutUsage(this->cmdMut);
	this->cmdEvent.Clear();
	this->cmdResultCode = INVALID_INDEX;
	if (this->stm->Write(buff, cmd.leng + 1) != cmd.leng + 1)
		return false;
	this->cmdEvent.Wait((Int64)timeout);
	return this->cmdResultCode == 0;
}

UInt32 __stdcall IO::Device::AXCAN::SerialThread(void *userObj)
{
	IO::Device::AXCAN *me = (IO::Device::AXCAN*)userObj;
	NotNullPtr<IO::Stream> stm;
	if (stm.Set(me->stm)) 
	{
		Text::UTF8Reader reader(stm);
		me->ParseReader(&reader);
	}
	me->threadRunning = false;
	return 0;
}

IO::Device::AXCAN::AXCAN(CANHandler *hdlr) : cmdEvent(false)
{
	this->hdlr = hdlr;
	this->stm = 0;
	this->threadRunning = false;
	this->cmdResultCode = 0;
}

IO::Device::AXCAN::~AXCAN()
{
	this->CloseSerialPort(false);
}

Bool IO::Device::AXCAN::OpenSerialPort(UOSInt portNum, UInt32 serialBaudRate, CANBitRate bitRate)
{
	this->CloseSerialPort(false);
	NotNullPtr<IO::SerialPort> port;
	NEW_CLASSNN(port, IO::SerialPort(portNum, serialBaudRate, IO::SerialPort::PARITY_NONE, false));
	if (port->IsError())
	{
		port.Delete();
		return false;
	}
	return this->OpenStream(port, bitRate);
}

Bool IO::Device::AXCAN::OpenStream(NotNullPtr<IO::Stream> stm, CANBitRate bitRate)
{
	this->stm = stm.Ptr();
	this->threadRunning = true;
	Sync::ThreadHandle *hand = Sync::ThreadUtil::CreateWithHandle(SerialThread, this);
	if (hand == 0)
	{
		this->threadRunning = false;
		DEL_CLASS(this->stm);
		this->stm = 0;
		return false;
	}
	Sync::ThreadUtil::CloseHandle(hand);
	if (this->SendSetCANBitRate(bitRate) && this->SendOpenCANPort(1, false, false) && this->SendSetReportMode(true, true, false))
	{
		return true;
	}
	else
	{
		this->CloseSerialPort(true);
		return false;
	}
}

void IO::Device::AXCAN::CloseSerialPort(Bool force)
{
	if (this->stm)
	{
		if (!force)
		{
			this->SendCommandMode();
			this->SendSetReportMode(false, false, false);
			this->SendCloseCANPort(1);
		}
		DEL_CLASS(this->stm);
		while (this->threadRunning)
		{
			Sync::SimpleThread::Sleep(1);
		}
		this->stm = 0;
	}
}

void IO::Device::AXCAN::CANStop()
{
	this->CloseSerialPort(false);
}

void IO::Device::AXCAN::ToString(NotNullPtr<Text::StringBuilderUTF8> sb) const
{
	if (this->stm)
	{
		sb->AppendC(UTF8STRC("AXCAN - "));
		sb->Append(this->stm->GetSourceNameObj());
	}
	else
	{
		sb->AppendC(UTF8STRC("AXCAN"));
	}
}

void IO::Device::AXCAN::ParseReader(IO::Reader *reader)
{
	Text::StringBuilderUTF8 sb;
	UInt8 buff[16];
	while (reader->ReadLine(sb, 1024))
	{
#if defined(VERBOSE)
		printf("AXCAN Recv: %s\r\n", sb.ToString());
#endif
		if (sb.StartsWith('#'))
		{
			if (sb.Equals(UTF8STRC("#OK")))
			{
				this->cmdResultCode = 0;
				this->cmdEvent.Set();
			}
			else if (sb.StartsWith(UTF8STRC("#ERROR")))
			{
				if (Text::StrToUOSInt(&sb.v[6], this->cmdResultCode))
				{
					this->cmdEvent.Set();
				}
			}
		}
		else if(sb.StartsWith(UTF8STRC("@F")) && sb.leng > 7)
		{
			UInt32 id;
			UInt8 len;
			Bool rtr;
			if (sb.v[6] == '1' && sb.leng >= 17)
			{
				id = Text::StrHex2UInt32C(&sb.v[7]);
				rtr = sb.v[15] != '0';
				len = (UInt8)(Text::StrHex2UInt8C(&sb.v[15]) & 15);
				if (sb.leng == (UOSInt)len * 2 + 17 && Text::StrHex2Bytes(&sb.v[17], buff) == len)
				{
					this->hdlr->CANMessage(id, rtr, buff, len);
				}
			}
			else if (sb.v[6] == '0' && sb.leng >= 12)
			{
				id = Text::StrHex2UInt16C(&sb.v[6]);
				rtr = sb.v[10] != '0';
				len = (UInt8)(Text::StrHex2UInt8C(&sb.v[10]) & 15);
				if (sb.leng == (UOSInt)len * 2 + 12 && Text::StrHex2Bytes(&sb.v[12], buff) == len)
				{
					this->hdlr->CANMessage(id, rtr, buff, len);
				}
			}
		}
		sb.ClearStr();
	}
}
