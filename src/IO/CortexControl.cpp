#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/CortexControl.h"
#include "IO/SerialPort.h"
#include "Manage/HiResClock.h"
#include "Math/Math.h"
#include "Sync/MutexUsage.h"
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"
#include "Text/StringBuilderUTF8.h"

UInt32 __stdcall IO::CortexControl::RecvThread(void *userObj)
{
	IO::CortexControl *me = (IO::CortexControl *)userObj;
	UInt8 buff[260];
	UOSInt buffSize = 0;
	UOSInt recvSize;
	if (me->errWriter) me->errWriter->WriteLineC(UTF8STRC("Thread started"));
	me->recvRunning = true;
	while (!me->recvToStop)
	{
		if (me->errWriter) me->errWriter->WriteLineC(UTF8STRC("Start Receive"));
		recvSize = me->stm->Read(Data::ByteArray(&buff[buffSize], 260 - buffSize));
		if (me->errWriter) me->errWriter->WriteLineC(UTF8STRC("End Receive"));
		if (recvSize <= 0)
		{
			if (me->recvToStop)
				break;
			else
				Sync::SimpleThread::Sleep(10);
		}
		else
		{
			if (me->errWriter)
			{
				Text::StringBuilderUTF8 sb;
				sb.AppendC(UTF8STRC("Recv: "));
				sb.AppendHexBuff(&buff[buffSize], recvSize, ' ', Text::LineBreakType::CRLF);
				me->errWriter->WriteLineC(sb.ToString(), sb.GetLength());
			}
			buffSize += recvSize;
			recvSize = me->protoHdlr.ParseProtocol(me->stm, 0, 0, Data::ByteArrayR(buff, buffSize));
			if (recvSize <= 0)
			{
				buffSize = 0;
			}
			else if (recvSize < buffSize)
			{
				MemCopyO(buff, &buff[buffSize - recvSize], recvSize);
				buffSize = buffSize;
			}
			else
			{
			}
		}
	}
	if (me->errWriter) me->errWriter->WriteLineC(UTF8STRC("Thread End"));
	me->recvRunning = false;
	return 0;
}

IO::CortexControl::CortexControl(UOSInt portNum, IO::Writer *errWriter) : protoHdlr(*this)
{
	this->recvRunning = false;
	this->recvToStop = false;
	this->errWriter = errWriter;
	NEW_CLASSNN(this->stm, IO::SerialPort(portNum, 115200, IO::SerialPort::PARITY_NONE, false));
	if (!this->stm->IsError())
	{
		Sync::ThreadUtil::Create(RecvThread, this);
	}
}

IO::CortexControl::~CortexControl()
{
	this->recvToStop = true;
	this->stm->Close();
	while (this->recvRunning)
	{
		Sync::SimpleThread::Sleep(10);
	}
	this->stm.Delete();
}

Bool IO::CortexControl::IsError()
{
	return this->stm->IsError();
}

void IO::CortexControl::DataParsed(NotNullPtr<IO::Stream> stm, void *stmObj, Int32 cmdType, Int32 seqId, const UInt8 *cmd, UOSInt cmdSize)
{
	if (cmdType == (this->sendType | 0x80))
	{
		switch (this->sendType)
		{
		case 0x00: //CM_GET_FW_VERSION
			if (cmdSize >= 1)
			{
				this->sendHasResult = true;
				this->sendResult = cmd[0];
				this->sendEvt.Set();
			}
			break;
		case 0x01: //CM_READ_DIO
			if (cmdSize >= 2)
			{
				this->sendHasResult = true;
				this->sendResult = ((cmd[1] & 0x000F) << 8) | cmd[0];
				this->sendEvt.Set();
			}
			break;
		case 0x02: //CM_WRITE_DIO
			if (cmdSize >= 0)
			{
				this->sendHasResult = true;
				this->sendEvt.Set();
			}
			break;
		case 0x03: //CM_ADC_READ_VIN
			if (cmdSize >= 2)
			{
				this->sendHasResult = true;
				this->sendResult = (cmd[1] << 8) | cmd[0];
				this->sendEvt.Set();
			}
			break;
		case 0x04: //CM_READ_EE
			break;
		case 0x05: //CM_WRITE_EE
			break;
		case 0x06: //CM_ADC_READ_VBATT
			if (cmdSize >= 2)
			{
				this->sendHasResult = true;
				this->sendResult = (cmd[1] << 8) | cmd[0];
				this->sendEvt.Set();
			}
			break;
		case 0x07: //CM_READ_ODO
			if (cmdSize >= 2)
			{
				this->sendHasResult = true;
				this->sendResult = (cmd[1] << 8) | cmd[0];
				this->sendEvt.Set();
			}
			break;
		case 0x08: //CM_RESET_ODO
			if (cmdSize >= 0)
			{
				this->sendHasResult = true;
				this->sendEvt.Set();
			}
			break;
		case 0x09: //CM_TEST
			break;
		case 0x0a: //CM_GET_LUX
			if (cmdSize >= 2)
			{
				this->sendHasResult = true;
				this->sendResult = (cmd[1] << 8) | cmd[0];
				this->sendEvt.Set();
			}
			break;
		case 0x0b: //CM_GET_TEMP
			if (cmdSize >= 2)
			{
				this->sendHasResult = true;
				this->sendResult = (cmd[1] << 8) | cmd[0];
				this->sendEvt.Set();
			}
			break;
		case 0x0c: //CM_GET_KEY
			if (cmdSize >= 1)
			{
				this->sendHasResult = true;
				this->sendResult = cmd[0];
				this->sendEvt.Set();
			}
			break;
		case 0x0d: //CM_POWER_OFF
			if (cmdSize >= 0)
			{
				this->sendHasResult = true;
				this->sendEvt.Set();
			}
			break;
		case 0x0e: //CM_CODEC_POWER
			if (cmdSize >= 0)
			{
				this->sendHasResult = true;
				this->sendEvt.Set();
			}
			break;
		case 0x0f: //CM_WDT
			if (cmdSize >= 0)
			{
				this->sendHasResult = true;
				this->sendEvt.Set();
			}
			break;
		}
	}
}

void IO::CortexControl::DataSkipped(NotNullPtr<IO::Stream> stm, void *stmObj, const UInt8 *buff, UOSInt buffSize)
{
}

Bool IO::CortexControl::GetFWVersion(Int32 *majorVer, Int32 *minorVer)
{
	UInt8 buff[16];
	UOSInt packetSize;
	Manage::HiResClock clk;
	Double t;
	Bool succ = false;
	packetSize = this->protoHdlr.BuildPacket(buff, 0, 0, 0, 0, 0);
	Sync::MutexUsage mutUsage(this->sendMut);
	this->sendType = 0;
	this->sendHasResult = false;
	clk.Start();
	if (this->errWriter)
	{
		Text::StringBuilderUTF8 sb;
		sb.AppendC(UTF8STRC("Send: "));
		sb.AppendHexBuff(buff, packetSize, ' ', Text::LineBreakType::CRLF);
		this->errWriter->WriteLineC(sb.ToString(), sb.GetLength());
	}
	this->stm->Write(buff, packetSize);
	while (!this->sendHasResult)
	{
		t = clk.GetTimeDiff();
		if (t > 2)
			break;
		this->sendEvt.Wait((UInt32)Double2Int32((2 - t) * 1000));
	}
	if (this->sendHasResult)
	{
		*majorVer = (this->sendResult & 0xf0) >> 4;
		*minorVer = this->sendResult & 0xf;
		succ = true;
	}
	return succ;
}

Bool IO::CortexControl::ReadDIO(Int32 *dioValues)
{
	UInt8 buff[16];
	UOSInt packetSize;
	Manage::HiResClock clk;
	Double t;
	Bool succ = false;
	packetSize = this->protoHdlr.BuildPacket(buff, 1, 0, 0, 0, 0);
	Sync::MutexUsage mutUsage(this->sendMut);
	this->sendType = 1;
	this->sendHasResult = false;
	clk.Start();
	if (this->errWriter)
	{
		Text::StringBuilderUTF8 sb;
		sb.AppendC(UTF8STRC("Send: "));
		sb.AppendHexBuff(buff, packetSize, ' ', Text::LineBreakType::CRLF);
		this->errWriter->WriteLineC(sb.ToString(), sb.GetLength());
	}
	this->stm->Write(buff, packetSize);
	while (!this->sendHasResult)
	{
		t = clk.GetTimeDiff();
		if (t > 2)
			break;
		this->sendEvt.Wait((UInt32)Double2Int32((2 - t) * 1000));
	}
	if (this->sendHasResult)
	{
		*dioValues = this->sendResult;
		succ = true;
	}
	return succ;
}

Bool IO::CortexControl::WriteDIO(Int32 outVal, Int32 outMask)
{
	UInt8 buff[16];
	UInt8 cmd[2];
	UOSInt packetSize;
	Manage::HiResClock clk;
	Double t;
	Bool succ = false;
	cmd[0] = (UInt8)(outVal & 0xff);
	cmd[1] = (UInt8)(outMask & 0xff);
	packetSize = this->protoHdlr.BuildPacket(buff, 2, 0, cmd, 2, 0);
	Sync::MutexUsage mutUsage(this->sendMut);
	this->sendType = 2;
	this->sendHasResult = false;
	clk.Start();
	if (this->errWriter)
	{
		Text::StringBuilderUTF8 sb;
		sb.AppendC(UTF8STRC("Send: "));
		sb.AppendHexBuff(buff, packetSize, ' ', Text::LineBreakType::CRLF);
		this->errWriter->WriteLineC(sb.ToString(), sb.GetLength());
	}
	this->stm->Write(buff, packetSize);
	while (!this->sendHasResult)
	{
		t = clk.GetTimeDiff();
		if (t > 2)
			break;
		this->sendEvt.Wait((UInt32)Double2Int32((2 - t) * 1000));
	}
	if (this->sendHasResult)
	{
		succ = true;
	}
	return succ;
}

Bool IO::CortexControl::ReadVin(Int32 *voltage)
{
	UInt8 buff[16];
	UOSInt packetSize;
	Manage::HiResClock clk;
	Double t;
	Bool succ = false;
	packetSize = this->protoHdlr.BuildPacket(buff, 3, 0, 0, 0, 0);
	Sync::MutexUsage mutUsage(this->sendMut);
	this->sendType = 3;
	this->sendHasResult = false;
	clk.Start();
	if (this->errWriter)
	{
		Text::StringBuilderUTF8 sb;
		sb.AppendC(UTF8STRC("Send: "));
		sb.AppendHexBuff(buff, packetSize, ' ', Text::LineBreakType::CRLF);
		this->errWriter->WriteLineC(sb.ToString(), sb.GetLength());
	}
	this->stm->Write(buff, packetSize);
	while (!this->sendHasResult)
	{
		t = clk.GetTimeDiff();
		if (t > 2)
			break;
		this->sendEvt.Wait((UInt32)Double2Int32((2 - t) * 1000));
	}
	if (this->sendHasResult)
	{
		*voltage = this->sendResult;
		succ = true;
	}
	return succ;
}

Bool IO::CortexControl::ReadVBatt(Int32 *voltage)
{
	UInt8 buff[16];
	UOSInt packetSize;
	Manage::HiResClock clk;
	Double t;
	Bool succ = false;
	packetSize = this->protoHdlr.BuildPacket(buff, 6, 0, 0, 0, 0);
	Sync::MutexUsage mutUsage(this->sendMut);
	this->sendType = 6;
	this->sendHasResult = false;
	clk.Start();
	if (this->errWriter)
	{
		Text::StringBuilderUTF8 sb;
		sb.AppendC(UTF8STRC("Send: "));
		sb.AppendHexBuff(buff, packetSize, ' ', Text::LineBreakType::CRLF);
		this->errWriter->WriteLineC(sb.ToString(), sb.GetLength());
	}
	this->stm->Write(buff, packetSize);
	while (!this->sendHasResult)
	{
		t = clk.GetTimeDiff();
		if (t > 2)
			break;
		this->sendEvt.Wait((UInt32)Double2Int32((2 - t) * 1000));
	}
	if (this->sendHasResult)
	{
		*voltage = this->sendResult;
		succ = true;
	}
	return succ;
}

Bool IO::CortexControl::ReadOdometerCounter(Int32 *odoCount)
{
	UInt8 buff[16];
	UOSInt packetSize;
	Manage::HiResClock clk;
	Double t;
	Bool succ = false;
	packetSize = this->protoHdlr.BuildPacket(buff, 7, 0, 0, 0, 0);
	Sync::MutexUsage mutUsage(this->sendMut);
	this->sendType = 7;
	this->sendHasResult = false;
	clk.Start();
	if (this->errWriter)
	{
		Text::StringBuilderUTF8 sb;
		sb.AppendC(UTF8STRC("Send: "));
		sb.AppendHexBuff(buff, packetSize, ' ', Text::LineBreakType::CRLF);
		this->errWriter->WriteLineC(sb.ToString(), sb.GetLength());
	}
	this->stm->Write(buff, packetSize);
	while (!this->sendHasResult)
	{
		t = clk.GetTimeDiff();
		if (t > 2)
			break;
		this->sendEvt.Wait((UInt32)Double2Int32((2 - t) * 1000));
	}
	if (this->sendHasResult)
	{
		*odoCount = this->sendResult;
		succ = true;
	}
	return succ;
}

Bool IO::CortexControl::ResetOdometerCounter()
{
	UInt8 buff[16];
	UOSInt packetSize;
	Manage::HiResClock clk;
	Double t;
	Bool succ = false;
	packetSize = this->protoHdlr.BuildPacket(buff, 8, 0, 0, 0, 0);
	Sync::MutexUsage mutUsage(this->sendMut);
	this->sendType = 8;
	this->sendHasResult = false;
	clk.Start();
	if (this->errWriter)
	{
		Text::StringBuilderUTF8 sb;
		sb.AppendC(UTF8STRC("Send: "));
		sb.AppendHexBuff(buff, packetSize, ' ', Text::LineBreakType::CRLF);
		this->errWriter->WriteLineC(sb.ToString(), sb.GetLength());
	}
	this->stm->Write(buff, packetSize);
	while (!this->sendHasResult)
	{
		t = clk.GetTimeDiff();
		if (t > 2)
			break;
		this->sendEvt.Wait((UInt32)Double2Int32((2 - t) * 1000));
	}
	if (this->sendHasResult)
	{
		succ = true;
	}
	return succ;
}

Bool IO::CortexControl::ReadEnvBrightness(Int32 *brightness)
{
	UInt8 buff[16];
	UOSInt packetSize;
	Manage::HiResClock clk;
	Double t;
	Bool succ = false;
	packetSize = this->protoHdlr.BuildPacket(buff, 10, 0, 0, 0, 0);
	Sync::MutexUsage mutUsage(this->sendMut);
	this->sendType = 10;
	this->sendHasResult = false;
	clk.Start();
	if (this->errWriter)
	{
		Text::StringBuilderUTF8 sb;
		sb.AppendC(UTF8STRC("Send: "));
		sb.AppendHexBuff(buff, packetSize, ' ', Text::LineBreakType::CRLF);
		this->errWriter->WriteLineC(sb.ToString(), sb.GetLength());
	}
	this->stm->Write(buff, packetSize);
	while (!this->sendHasResult)
	{
		t = clk.GetTimeDiff();
		if (t > 2)
			break;
		this->sendEvt.Wait((UInt32)Double2Int32((2 - t) * 1000));
	}
	if (this->sendHasResult)
	{
		*brightness = this->sendResult;
		succ = true;
	}
	return succ;
}

Bool IO::CortexControl::ReadTemperature(Int32 *temperature)
{
	UInt8 buff[16];
	UOSInt packetSize;
	Manage::HiResClock clk;
	Double t;
	Bool succ = false;
	packetSize = this->protoHdlr.BuildPacket(buff, 11, 0, 0, 0, 0);
	Sync::MutexUsage mutUsage(this->sendMut);
	this->sendType = 11;
	this->sendHasResult = false;
	clk.Start();
	if (this->errWriter)
	{
		Text::StringBuilderUTF8 sb;
		sb.AppendC(UTF8STRC("Send: "));
		sb.AppendHexBuff(buff, packetSize, ' ', Text::LineBreakType::CRLF);
		this->errWriter->WriteLineC(sb.ToString(), sb.GetLength());
	}
	this->stm->Write(buff, packetSize);
	while (!this->sendHasResult)
	{
		t = clk.GetTimeDiff();
		if (t > 2)
			break;
		this->sendEvt.Wait((UInt32)Double2Int32((2 - t) * 1000));
	}
	if (this->sendHasResult)
	{
		*temperature = this->sendResult;
		succ = true;
	}
	return succ;
}

Bool IO::CortexControl::PowerOff()
{
	UInt8 buff[16];
	UOSInt packetSize;
	Manage::HiResClock clk;
	Double t;
	Bool succ = false;
	packetSize = this->protoHdlr.BuildPacket(buff, 13, 0, 0, 0, 0);
	Sync::MutexUsage mutUsage(this->sendMut);
	this->sendType = 13;
	this->sendHasResult = false;
	clk.Start();
	if (this->errWriter)
	{
		Text::StringBuilderUTF8 sb;
		sb.AppendC(UTF8STRC("Send: "));
		sb.AppendHexBuff(buff, packetSize, ' ', Text::LineBreakType::CRLF);
		this->errWriter->WriteLineC(sb.ToString(), sb.GetLength());
	}
	this->stm->Write(buff, packetSize);
	while (!this->sendHasResult)
	{
		t = clk.GetTimeDiff();
		if (t > 2)
			break;
		this->sendEvt.Wait((UInt32)Double2Int32((2 - t) * 1000));
	}
	if (this->sendHasResult)
	{
		succ = true;
	}
	return succ;
}

Bool IO::CortexControl::HDACodecPower(Bool turnOn)
{
	UInt8 buff[16];
	UInt8 cmd;
	UOSInt packetSize;
	Manage::HiResClock clk;
	Double t;
	Bool succ = false;
	cmd = turnOn?1:0;
	packetSize = this->protoHdlr.BuildPacket(buff, 14, 0, &cmd, 1, 0);
	Sync::MutexUsage mutUsage(this->sendMut);
	this->sendType = 14;
	this->sendHasResult = false;
	clk.Start();
	if (this->errWriter)
	{
		Text::StringBuilderUTF8 sb;
		sb.AppendC(UTF8STRC("Send: "));
		sb.AppendHexBuff(buff, packetSize, ' ', Text::LineBreakType::CRLF);
		this->errWriter->WriteLineC(sb.ToString(), sb.GetLength());
	}
	this->stm->Write(buff, packetSize);
	while (!this->sendHasResult)
	{
		t = clk.GetTimeDiff();
		if (t > 2)
			break;
		this->sendEvt.Wait((UInt32)Double2Int32((2 - t) * 1000));
	}
	if (this->sendHasResult)
	{
		succ = true;
	}
	return succ;
}

Bool IO::CortexControl::SetWatchdogTimeout(UInt8 timeout)
{
	UInt8 buff[16];
	UOSInt packetSize;
	Manage::HiResClock clk;
	Double t;
	Bool succ = false;
	packetSize = this->protoHdlr.BuildPacket(buff, 15, 0, &timeout, 1, 0);
	Sync::MutexUsage mutUsage(this->sendMut);
	this->sendType = 15;
	this->sendHasResult = false;
	clk.Start();
	if (this->errWriter)
	{
		Text::StringBuilderUTF8 sb;
		sb.AppendC(UTF8STRC("Send: "));
		sb.AppendHexBuff(buff, packetSize, ' ', Text::LineBreakType::CRLF);
		this->errWriter->WriteLineC(sb.ToString(), sb.GetLength());
	}
	this->stm->Write(buff, packetSize);
	while (!this->sendHasResult)
	{
		t = clk.GetTimeDiff();
		if (t > 2)
			break;
		this->sendEvt.Wait((UInt32)Double2Int32((2 - t) * 1000));
	}
	if (this->sendHasResult)
	{
		succ = true;
	}
	return succ;
}
