#include "stdafx.h"
#include "MyMemory.h"
#include "IO/CortexControl.h"
#include "IO/SerialPort.h"
#include "Manage/HiResClock.h"
#include "Math/Math.h"
#include "Sync/Thread.h"
#include "Text/StringBuilder.h"

UInt32 __stdcall IO::CortexControl::RecvThread(void *userObj)
{
	IO::CortexControl *me = (IO::CortexControl *)userObj;
	UInt8 buff[260];
	OSInt buffSize = 0;
	OSInt recvSize;
	if (me->errWriter) me->errWriter->WriteLine(L"Thread started");
	me->recvRunning = true;
	while (!me->recvToStop)
	{
		if (me->errWriter) me->errWriter->WriteLine(L"Start Receive");
		recvSize = me->stm->Read(&buff[buffSize], 260 - buffSize);
		if (me->errWriter) me->errWriter->WriteLine(L"End Receive");
		if (recvSize <= 0)
		{
			if (me->recvToStop)
				break;
			else
				Sync::Thread::Sleep(10);
		}
		else
		{
			if (me->errWriter)
			{
				Text::StringBuilder sb;
				sb.Append(L"Recv: ");
				sb.AppendHex(&buff[buffSize], recvSize, ' ', Text::StringBuilder::LBT_CRLF);
				me->errWriter->WriteLine(sb.ToString());
			}
			buffSize += recvSize;
			recvSize = me->protoHdlr->ParseProtocol(me->stm, 0, 0, buff, buffSize);
			if (recvSize <= 0)
			{
				buffSize = 0;
			}
			else if (recvSize < buffSize)
			{
				MemCopy(buff, &buff[buffSize - recvSize], recvSize);
				buffSize = buffSize;
			}
			else
			{
			}
		}
	}
	if (me->errWriter) me->errWriter->WriteLine(L"Thread End");
	me->recvRunning = false;
	return 0;
}

IO::CortexControl::CortexControl(Int32 portNum, IO::IWriter *errWriter)
{
	this->stm = 0;
	this->protoHdlr = 0;
	this->sendMut = 0;
	this->sendEvt = 0;
	this->recvRunning = false;
	this->recvToStop = false;
	this->errWriter = errWriter;

	NEW_CLASS(this->stm, IO::SerialPort(portNum, 115200, false));
	if (((IO::SerialPort*)this->stm)->IsError())
	{
		DEL_CLASS(this->stm);
		this->stm = 0;
		return;
	}
	NEW_CLASS(this->protoHdlr, IO::ProtoCortexHandler(this));
	NEW_CLASS(this->sendMut, Sync::Mutex());
	NEW_CLASS(this->sendEvt, Sync::Event(true, L"IO.CortexControl.sendEvt"));

	Sync::Thread::Create(RecvThread, this);
}

IO::CortexControl::~CortexControl()
{
	this->recvToStop = true;
	if (this->stm)
	{
		this->stm->Close();
	}
	while (this->recvRunning)
	{
		Sync::Thread::Sleep(10);
	}
	SDEL_CLASS(this->sendEvt);
	SDEL_CLASS(this->sendMut);
	SDEL_CLASS(this->protoHdlr);
	SDEL_CLASS(this->stm);
}

Bool IO::CortexControl::IsError()
{
	return stm == 0;
}

void IO::CortexControl::DataParsed(IO::Stream *stm, void *stmObj, Int32 cmdType, Int32 seqId, UInt8 *cmd, OSInt cmdSize)
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
				this->sendEvt->Set();
			}
			break;
		case 0x01: //CM_READ_DIO
			if (cmdSize >= 2)
			{
				this->sendHasResult = true;
				this->sendResult = ((cmd[1] & 0x000F) << 8) | cmd[0];
				this->sendEvt->Set();
			}
			break;
		case 0x02: //CM_WRITE_DIO
			if (cmdSize >= 0)
			{
				this->sendHasResult = true;
				this->sendEvt->Set();
			}
			break;
		case 0x03: //CM_ADC_READ_VIN
			if (cmdSize >= 2)
			{
				this->sendHasResult = true;
				this->sendResult = (cmd[1] << 8) | cmd[0];
				this->sendEvt->Set();
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
				this->sendEvt->Set();
			}
			break;
		case 0x07: //CM_READ_ODO
			if (cmdSize >= 2)
			{
				this->sendHasResult = true;
				this->sendResult = (cmd[1] << 8) | cmd[0];
				this->sendEvt->Set();
			}
			break;
		case 0x08: //CM_RESET_ODO
			if (cmdSize >= 0)
			{
				this->sendHasResult = true;
				this->sendEvt->Set();
			}
			break;
		case 0x09: //CM_TEST
			break;
		case 0x0a: //CM_GET_LUX
			if (cmdSize >= 2)
			{
				this->sendHasResult = true;
				this->sendResult = (cmd[1] << 8) | cmd[0];
				this->sendEvt->Set();
			}
			break;
		case 0x0b: //CM_GET_TEMP
			if (cmdSize >= 2)
			{
				this->sendHasResult = true;
				this->sendResult = (cmd[1] << 8) | cmd[0];
				this->sendEvt->Set();
			}
			break;
		case 0x0c: //CM_GET_KEY
			if (cmdSize >= 1)
			{
				this->sendHasResult = true;
				this->sendResult = cmd[0];
				this->sendEvt->Set();
			}
			break;
		case 0x0d: //CM_POWER_OFF
			if (cmdSize >= 0)
			{
				this->sendHasResult = true;
				this->sendEvt->Set();
			}
			break;
		case 0x0e: //CM_CODEC_POWER
			if (cmdSize >= 0)
			{
				this->sendHasResult = true;
				this->sendEvt->Set();
			}
			break;
		case 0x0f: //CM_WDT
			if (cmdSize >= 0)
			{
				this->sendHasResult = true;
				this->sendEvt->Set();
			}
			break;
		}
	}
}

void IO::CortexControl::DataSkipped(IO::Stream *stm, void *stmObj, UInt8 *buff, OSInt buffSize)
{
}

Bool IO::CortexControl::GetFWVersion(Int32 *majorVer, Int32 *minorVer)
{
	UInt8 buff[16];
	OSInt packetSize;
	Manage::HiResClock clk;
	Double t;
	Bool succ = false;
	packetSize = this->protoHdlr->BuildPacket(buff, 0, 0, 0, 0);
	this->sendMut->Lock();
	this->sendType = 0;
	this->sendHasResult = false;
	clk.Start();
	if (this->errWriter)
	{
		Text::StringBuilder sb;
		sb.Append(L"Send: ");
		sb.AppendHex(buff, packetSize, ' ', Text::StringBuilder::LBT_CRLF);
		this->errWriter->WriteLine(sb.ToString());
	}
	this->stm->Write(buff, packetSize);
	while (!this->sendHasResult)
	{
		t = clk.GetTimeDiff();
		if (t > 2)
			break;
		this->sendEvt->Wait(Math::Double2Int((2 - t) * 1000));
	}
	if (this->sendHasResult)
	{
		*majorVer = (this->sendResult & 0xf0) >> 4;
		*minorVer = this->sendResult & 0xf;
		succ = true;
	}
	this->sendMut->Unlock();
	return succ;
}

Bool IO::CortexControl::ReadDIO(Int32 *dioValues)
{
	UInt8 buff[16];
	OSInt packetSize;
	Manage::HiResClock clk;
	Double t;
	Bool succ = false;
	packetSize = this->protoHdlr->BuildPacket(buff, 1, 0, 0, 0);
	this->sendMut->Lock();
	this->sendType = 1;
	this->sendHasResult = false;
	clk.Start();
	if (this->errWriter)
	{
		Text::StringBuilder sb;
		sb.Append(L"Send: ");
		sb.AppendHex(buff, packetSize, ' ', Text::StringBuilder::LBT_CRLF);
		this->errWriter->WriteLine(sb.ToString());
	}
	this->stm->Write(buff, packetSize);
	while (!this->sendHasResult)
	{
		t = clk.GetTimeDiff();
		if (t > 2)
			break;
		this->sendEvt->Wait(Math::Double2Int((2 - t) * 1000));
	}
	if (this->sendHasResult)
	{
		*dioValues = this->sendResult;
		succ = true;
	}
	this->sendMut->Unlock();
	return succ;
}

Bool IO::CortexControl::WriteDIO(Int32 outVal, Int32 outMask)
{
	UInt8 buff[16];
	UInt8 cmd[2];
	OSInt packetSize;
	Manage::HiResClock clk;
	Double t;
	Bool succ = false;
	cmd[0] = (UInt8)(outVal & 0xff);
	cmd[1] = (UInt8)(outMask & 0xff);
	packetSize = this->protoHdlr->BuildPacket(buff, 2, 0, cmd, 2);
	this->sendMut->Lock();
	this->sendType = 2;
	this->sendHasResult = false;
	clk.Start();
	if (this->errWriter)
	{
		Text::StringBuilder sb;
		sb.Append(L"Send: ");
		sb.AppendHex(buff, packetSize, ' ', Text::StringBuilder::LBT_CRLF);
		this->errWriter->WriteLine(sb.ToString());
	}
	this->stm->Write(buff, packetSize);
	while (!this->sendHasResult)
	{
		t = clk.GetTimeDiff();
		if (t > 2)
			break;
		this->sendEvt->Wait(Math::Double2Int((2 - t) * 1000));
	}
	if (this->sendHasResult)
	{
		succ = true;
	}
	this->sendMut->Unlock();
	return succ;
}

Bool IO::CortexControl::ReadVin(Int32 *voltage)
{
	UInt8 buff[16];
	OSInt packetSize;
	Manage::HiResClock clk;
	Double t;
	Bool succ = false;
	packetSize = this->protoHdlr->BuildPacket(buff, 3, 0, 0, 0);
	this->sendMut->Lock();
	this->sendType = 3;
	this->sendHasResult = false;
	clk.Start();
	if (this->errWriter)
	{
		Text::StringBuilder sb;
		sb.Append(L"Send: ");
		sb.AppendHex(buff, packetSize, ' ', Text::StringBuilder::LBT_CRLF);
		this->errWriter->WriteLine(sb.ToString());
	}
	this->stm->Write(buff, packetSize);
	while (!this->sendHasResult)
	{
		t = clk.GetTimeDiff();
		if (t > 2)
			break;
		this->sendEvt->Wait(Math::Double2Int((2 - t) * 1000));
	}
	if (this->sendHasResult)
	{
		*voltage = this->sendResult;
		succ = true;
	}
	this->sendMut->Unlock();
	return succ;
}

Bool IO::CortexControl::ReadVBatt(Int32 *voltage)
{
	UInt8 buff[16];
	OSInt packetSize;
	Manage::HiResClock clk;
	Double t;
	Bool succ = false;
	packetSize = this->protoHdlr->BuildPacket(buff, 6, 0, 0, 0);
	this->sendMut->Lock();
	this->sendType = 6;
	this->sendHasResult = false;
	clk.Start();
	if (this->errWriter)
	{
		Text::StringBuilder sb;
		sb.Append(L"Send: ");
		sb.AppendHex(buff, packetSize, ' ', Text::StringBuilder::LBT_CRLF);
		this->errWriter->WriteLine(sb.ToString());
	}
	this->stm->Write(buff, packetSize);
	while (!this->sendHasResult)
	{
		t = clk.GetTimeDiff();
		if (t > 2)
			break;
		this->sendEvt->Wait(Math::Double2Int((2 - t) * 1000));
	}
	if (this->sendHasResult)
	{
		*voltage = this->sendResult;
		succ = true;
	}
	this->sendMut->Unlock();
	return succ;
}

Bool IO::CortexControl::ReadOdometerCounter(Int32 *odoCount)
{
	UInt8 buff[16];
	OSInt packetSize;
	Manage::HiResClock clk;
	Double t;
	Bool succ = false;
	packetSize = this->protoHdlr->BuildPacket(buff, 7, 0, 0, 0);
	this->sendMut->Lock();
	this->sendType = 7;
	this->sendHasResult = false;
	clk.Start();
	if (this->errWriter)
	{
		Text::StringBuilder sb;
		sb.Append(L"Send: ");
		sb.AppendHex(buff, packetSize, ' ', Text::StringBuilder::LBT_CRLF);
		this->errWriter->WriteLine(sb.ToString());
	}
	this->stm->Write(buff, packetSize);
	while (!this->sendHasResult)
	{
		t = clk.GetTimeDiff();
		if (t > 2)
			break;
		this->sendEvt->Wait(Math::Double2Int((2 - t) * 1000));
	}
	if (this->sendHasResult)
	{
		*odoCount = this->sendResult;
		succ = true;
	}
	this->sendMut->Unlock();
	return succ;
}

Bool IO::CortexControl::ResetOdometerCounter()
{
	UInt8 buff[16];
	OSInt packetSize;
	Manage::HiResClock clk;
	Double t;
	Bool succ = false;
	packetSize = this->protoHdlr->BuildPacket(buff, 8, 0, 0, 0);
	this->sendMut->Lock();
	this->sendType = 8;
	this->sendHasResult = false;
	clk.Start();
	if (this->errWriter)
	{
		Text::StringBuilder sb;
		sb.Append(L"Send: ");
		sb.AppendHex(buff, packetSize, ' ', Text::StringBuilder::LBT_CRLF);
		this->errWriter->WriteLine(sb.ToString());
	}
	this->stm->Write(buff, packetSize);
	while (!this->sendHasResult)
	{
		t = clk.GetTimeDiff();
		if (t > 2)
			break;
		this->sendEvt->Wait(Math::Double2Int((2 - t) * 1000));
	}
	if (this->sendHasResult)
	{
		succ = true;
	}
	this->sendMut->Unlock();
	return succ;
}

Bool IO::CortexControl::ReadEnvBrightness(Int32 *brightness)
{
	UInt8 buff[16];
	OSInt packetSize;
	Manage::HiResClock clk;
	Double t;
	Bool succ = false;
	packetSize = this->protoHdlr->BuildPacket(buff, 10, 0, 0, 0);
	this->sendMut->Lock();
	this->sendType = 10;
	this->sendHasResult = false;
	clk.Start();
	if (this->errWriter)
	{
		Text::StringBuilder sb;
		sb.Append(L"Send: ");
		sb.AppendHex(buff, packetSize, ' ', Text::StringBuilder::LBT_CRLF);
		this->errWriter->WriteLine(sb.ToString());
	}
	this->stm->Write(buff, packetSize);
	while (!this->sendHasResult)
	{
		t = clk.GetTimeDiff();
		if (t > 2)
			break;
		this->sendEvt->Wait(Math::Double2Int((2 - t) * 1000));
	}
	if (this->sendHasResult)
	{
		*brightness = this->sendResult;
		succ = true;
	}
	this->sendMut->Unlock();
	return succ;
}

Bool IO::CortexControl::ReadTemperature(Int32 *temperature)
{
	UInt8 buff[16];
	OSInt packetSize;
	Manage::HiResClock clk;
	Double t;
	Bool succ = false;
	packetSize = this->protoHdlr->BuildPacket(buff, 11, 0, 0, 0);
	this->sendMut->Lock();
	this->sendType = 11;
	this->sendHasResult = false;
	clk.Start();
	if (this->errWriter)
	{
		Text::StringBuilder sb;
		sb.Append(L"Send: ");
		sb.AppendHex(buff, packetSize, ' ', Text::StringBuilder::LBT_CRLF);
		this->errWriter->WriteLine(sb.ToString());
	}
	this->stm->Write(buff, packetSize);
	while (!this->sendHasResult)
	{
		t = clk.GetTimeDiff();
		if (t > 2)
			break;
		this->sendEvt->Wait(Math::Double2Int((2 - t) * 1000));
	}
	if (this->sendHasResult)
	{
		*temperature = this->sendResult;
		succ = true;
	}
	this->sendMut->Unlock();
	return succ;
}

Bool IO::CortexControl::PowerOff()
{
	UInt8 buff[16];
	OSInt packetSize;
	Manage::HiResClock clk;
	Double t;
	Bool succ = false;
	packetSize = this->protoHdlr->BuildPacket(buff, 13, 0, 0, 0);
	this->sendMut->Lock();
	this->sendType = 13;
	this->sendHasResult = false;
	clk.Start();
	if (this->errWriter)
	{
		Text::StringBuilder sb;
		sb.Append(L"Send: ");
		sb.AppendHex(buff, packetSize, ' ', Text::StringBuilder::LBT_CRLF);
		this->errWriter->WriteLine(sb.ToString());
	}
	this->stm->Write(buff, packetSize);
	while (!this->sendHasResult)
	{
		t = clk.GetTimeDiff();
		if (t > 2)
			break;
		this->sendEvt->Wait(Math::Double2Int((2 - t) * 1000));
	}
	if (this->sendHasResult)
	{
		succ = true;
	}
	this->sendMut->Unlock();
	return succ;
}

Bool IO::CortexControl::HDACodecPower(Bool turnOn)
{
	UInt8 buff[16];
	UInt8 cmd;
	OSInt packetSize;
	Manage::HiResClock clk;
	Double t;
	Bool succ = false;
	cmd = turnOn?1:0;
	packetSize = this->protoHdlr->BuildPacket(buff, 14, 0, &cmd, 1);
	this->sendMut->Lock();
	this->sendType = 14;
	this->sendHasResult = false;
	clk.Start();
	if (this->errWriter)
	{
		Text::StringBuilder sb;
		sb.Append(L"Send: ");
		sb.AppendHex(buff, packetSize, ' ', Text::StringBuilder::LBT_CRLF);
		this->errWriter->WriteLine(sb.ToString());
	}
	this->stm->Write(buff, packetSize);
	while (!this->sendHasResult)
	{
		t = clk.GetTimeDiff();
		if (t > 2)
			break;
		this->sendEvt->Wait(Math::Double2Int((2 - t) * 1000));
	}
	if (this->sendHasResult)
	{
		succ = true;
	}
	this->sendMut->Unlock();
	return succ;
}

Bool IO::CortexControl::SetWatchdogTimeout(UInt8 timeout)
{
	UInt8 buff[16];
	OSInt packetSize;
	Manage::HiResClock clk;
	Double t;
	Bool succ = false;
	packetSize = this->protoHdlr->BuildPacket(buff, 15, 0, &timeout, 1);
	this->sendMut->Lock();
	this->sendType = 15;
	this->sendHasResult = false;
	clk.Start();
	if (this->errWriter)
	{
		Text::StringBuilder sb;
		sb.Append(L"Send: ");
		sb.AppendHex(buff, packetSize, ' ', Text::StringBuilder::LBT_CRLF);
		this->errWriter->WriteLine(sb.ToString());
	}
	this->stm->Write(buff, packetSize);
	while (!this->sendHasResult)
	{
		t = clk.GetTimeDiff();
		if (t > 2)
			break;
		this->sendEvt->Wait(Math::Double2Int((2 - t) * 1000));
	}
	if (this->sendHasResult)
	{
		succ = true;
	}
	this->sendMut->Unlock();
	return succ;
}
