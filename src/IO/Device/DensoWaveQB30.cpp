#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Data/DateTime.h"
#include "IO/Device/DensoWaveQB30.h"
#include "Sync/MutexUsage.h"
#include "Sync/SimpleThread.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"
#define RECVBUFFSIZE 256

void __stdcall IO::Device::DensoWaveQB30::RecvThread(NN<Sync::Thread> thread)
{
	NN<IO::Device::DensoWaveQB30> me = thread->GetUserObj().GetNN<IO::Device::DensoWaveQB30>();
	UInt8 buff[256];
	UnsafeArray<UTF8Char> sbuff;
	UOSInt recvSize;
	UOSInt i;
	Bool found;
	sbuff = MemAllocArr(UTF8Char, RECVBUFFSIZE + 1);
	while (!thread->IsStopping())
	{
		recvSize = me->stm->Read(BYTEARR(buff));
		if (recvSize <= 0)
		{
			thread->Wait(10);
		}
		else
		{
			Sync::MutexUsage mutUsage(me->recvMut);
			if (me->recvSize >= RECVBUFFSIZE)
			{
			}
			else if (me->recvSize + recvSize > RECVBUFFSIZE)
			{
				MemCopyNO(&me->recvBuff[me->recvSize], buff, RECVBUFFSIZE - me->recvSize);
				me->recvSize = RECVBUFFSIZE;
				me->recvEvt.Set();
			}
			else
			{
				MemCopyNO(&me->recvBuff[me->recvSize], buff, recvSize);
				me->recvSize += recvSize;
				me->recvEvt.Set();
			}
			if (me->currMode == IO::Device::DensoWaveQB30::MT_SCAN)
			{
				while (me->recvSize > 0)
				{
					found = false;
					i = 0;
					while (i < me->recvSize)
					{
						if (me->recvBuff[i] == 13)
						{
							Text::StrConcatC(sbuff, me->recvBuff, i);
							if (me->scanHdlr)
							{
								me->scanHdlr(me->scanHdlrObj, Text::CStringNN(sbuff, i));
							}
							me->stm->Write(CSTR("READOFF\r").ToByteArray());
							Sync::SimpleThread::Sleep(me->scanDelay);
							me->stm->Write(CSTR("READON\r").ToByteArray());

							found = true;
							if (i + 1 >= me->recvSize)
							{
								me->recvSize = 0;
							}
							else
							{
								MemCopyO(me->recvBuff, &me->recvBuff[i + 1], me->recvSize - i - 1);
							}
							break;
						}
						i++;
					}
					if (!found)
						break;
				}
			}
			mutUsage.EndUse();
		}
	}
	MemFreeArr(sbuff);
}

Bool IO::Device::DensoWaveQB30::ScanModeStart()
{
	Bool succ;
	Sync::MutexUsage mutUsage(this->reqMut);
	succ = this->stm->Write(CSTR("READON\r").ToByteArray()) == 7;
	return succ;
}

Bool IO::Device::DensoWaveQB30::ScanModeEnd()
{
	Bool succ;
	Sync::MutexUsage mutUsage(this->reqMut);
	succ = this->stm->Write(CSTR("READOFF\r").ToByteArray()) == 8;
	return succ;
}

Bool IO::Device::DensoWaveQB30::SettingModeStart()
{
	Bool succ;
	Sync::MutexUsage mutUsage(this->reqMut);
	succ = this->stm->Write(CSTR("START\r").ToByteArray()) == 6;
	return succ;
}

Bool IO::Device::DensoWaveQB30::SettingModeEnd()
{
	Bool succ;
	Sync::MutexUsage mutUsage(this->reqMut);
	succ = this->stm->Write(CSTR("END\r").ToByteArray()) == 4;
	return succ;
}

void IO::Device::DensoWaveQB30::ToIdleMode()
{
	if (this->currMode == IO::Device::DensoWaveQB30::MT_SCAN)
	{
		this->ScanModeEnd();
		this->currMode = IO::Device::DensoWaveQB30::MT_IDLE;
	}
	else if (this->currMode == IO::Device::DensoWaveQB30::MT_SETTING)
	{
		this->SettingModeEnd();
		this->currMode = IO::Device::DensoWaveQB30::MT_IDLE;
	}
}

Bool IO::Device::DensoWaveQB30::WaitForReply(UInt32 timeToWait)
{
	Data::DateTime startTime;
	Data::DateTime currTime;
	Int32 t;
	UOSInt currBuffSize = 0;
	UOSInt startIndex;
	UOSInt i;
	startTime.SetCurrTimeUTC();
	i = 0;
	while (true)
	{
		currTime.SetCurrTimeUTC();
		t = (Int32)currTime.DiffMS(startTime);
		if (currBuffSize != this->recvSize)
		{
			currBuffSize = this->recvSize;
			startIndex = 0;
			while (i < currBuffSize)
			{
				if (this->recvBuff[i] == 13)
				{
					this->recvBuff[i] = 0;
					if (Text::StrEquals(&this->recvBuff[startIndex], U8STR("OK")))
						return true;
					else 
						return false;
				}
				i++;
			}
		}
		if (t >= (Int32)timeToWait)
		{
			return false;
		}
		this->recvEvt.Wait(timeToWait - (UInt32)t);
	}
}

Bool IO::Device::DensoWaveQB30::WaitForReplyVal(UInt32 timeToWait, OutParam<Int32> retVal)
{
	Data::DateTime startTime;
	Data::DateTime currTime;
	Int32 t;
	UOSInt currBuffSize = 0;
	UOSInt startIndex;
	UOSInt i;
	startTime.SetCurrTimeUTC();
	i = 0;
	while (true)
	{
		currTime.SetCurrTimeUTC();
		t = (Int32)currTime.DiffMS(startTime);
		if (currBuffSize != this->recvSize)
		{
			currBuffSize = this->recvSize;
			startIndex = 0;
			while (i < currBuffSize)
			{
				if (this->recvBuff[i] == 13 || this->recvBuff[i] == 0)
				{
					this->recvBuff[i] = 0;
					return Text::StrToInt32(&this->recvBuff[startIndex], retVal);
				}
				i++;
			}
		}
		if (t >= (Int32)timeToWait)
		{
			return false;
		}
		this->recvEvt.Wait(timeToWait - (UInt32)t);
	}
}

Int32 IO::Device::DensoWaveQB30::ReadCommand(UnsafeArray<const UTF8Char> cmdStr, UOSInt cmdLen)
{
	Int32 result;
	Sync::MutexUsage mutUsage(this->reqMut);
	Sync::MutexUsage recvMutUsage(this->recvMut);
	this->recvSize = 0;
	recvMutUsage.EndUse();
	if (this->stm->Write(Data::ByteArrayR(cmdStr, cmdLen)) == cmdLen)
	{
		if (!this->WaitForReplyVal(1000, result))
		{
			result = -1;
		}
	}
	else
	{
		result = -1;
	}
	mutUsage.EndUse();
	return result;
}

Bool IO::Device::DensoWaveQB30::WriteCommand(UnsafeArray<const UTF8Char> cmdStr, UOSInt cmdLen)
{
	Bool succ = false;
	Sync::MutexUsage mutUsage(this->reqMut);
	Sync::MutexUsage recvMutUsage(this->recvMut);
	this->recvSize = 0;
	recvMutUsage.EndUse();
	if (this->stm->Write(Data::ByteArrayR(cmdStr, cmdLen)) == cmdLen)
	{
		if (this->WaitForReply(1000))
		{
			succ = true;
		}
	}
	mutUsage.EndUse();
	return succ;
}

IO::Device::DensoWaveQB30::DensoWaveQB30(NN<IO::Stream> stm) : IO::CodeScanner(CSTR("Denso Wave QB30")), thread(RecvThread, this, CSTR("DensoWaveQB30"))
{
	this->stm = stm;
	this->scanDelay = 1000;
//	NEW_CLASS(this->nextTime, Data::DateTime());
//	this->nextTime->SetCurrTimeUTC();

	this->recvBuff = MemAlloc(UInt8, RECVBUFFSIZE);
	this->recvSize = 0;
	this->currMode = IO::Device::DensoWaveQB30::MT_IDLE;
	this->scanHdlr = 0;
	this->scanHdlrObj = 0;
	this->thread.Start();
}

IO::Device::DensoWaveQB30::~DensoWaveQB30()
{
	this->ToIdleMode();
	this->thread.BeginStop();
	this->stm->Close();
	this->thread.WaitForEnd();
//	DEL_CLASS(this->nextTime);
	MemFree(this->recvBuff);
	this->stm.Delete();
}

void IO::Device::DensoWaveQB30::SetCurrMode(ModeType currMode)
{
	if (this->currMode == currMode)
		return;
	this->ToIdleMode();
	if (currMode == IO::Device::DensoWaveQB30::MT_SCAN)
	{
		this->currMode = currMode;
		Sync::MutexUsage mutUsage(this->recvMut);
		this->recvSize = 0;
		mutUsage.EndUse();
		this->ScanModeStart();
	}
	else if (currMode == IO::Device::DensoWaveQB30::MT_SETTING)
	{
		this->currMode = currMode;
		this->SettingModeStart();
	}
}

Bool IO::Device::DensoWaveQB30::SoftReset()
{
	Bool succ;
	Sync::MutexUsage mutUsage(this->reqMut);
	Sync::MutexUsage recvMutUsage(this->recvMut);
	this->recvSize = 0;
	recvMutUsage.EndUse();
	succ = this->stm->Write(CSTR("RESET\r").ToByteArray()) == 6;
	mutUsage.EndUse();
	if (succ)
	{
		this->currMode = IO::Device::DensoWaveQB30::MT_IDLE;
	}
	return succ;
}

Bool IO::Device::DensoWaveQB30::ResetDefault()
{
	Bool succ;
	Sync::MutexUsage mutUsage(this->reqMut);
	Sync::MutexUsage recvMutUsage(this->recvMut);
	this->recvSize = 0;
	recvMutUsage.EndUse();
	succ = this->stm->Write(CSTR("DEFAULT\r").ToByteArray()) == 8;
	mutUsage.EndUse();
	if (succ)
	{
		this->currMode = IO::Device::DensoWaveQB30::MT_IDLE;
	}
	return succ;
}

void IO::Device::DensoWaveQB30::HandleCodeScanned(ScanHandler hdlr, AnyType userObj)
{
	this->scanHdlr = hdlr;
	this->scanHdlrObj = userObj;
}

UOSInt IO::Device::DensoWaveQB30::GetCommandList(Data::ArrayList<DeviceCommand> *cmdList)
{
	UOSInt initCnt = cmdList->GetCount();
	cmdList->Add(DC_GET_READ_MODE);
	cmdList->Add(DC_SET_READ_MODE);
	cmdList->Add(DC_GET_BW_MODE);
	cmdList->Add(DC_SET_BW_MODE);
	cmdList->Add(DC_GET_READ_REPEAT_TIME);
	cmdList->Add(DC_SET_READ_REPEAT_TIME);
	cmdList->Add(DC_GET_BRIGHTNESS);
	cmdList->Add(DC_SET_BRIGHTNESS);
	cmdList->Add(DC_GET_SHT_SIGNAL);
	cmdList->Add(DC_SET_SHT_SIGNAL);
	cmdList->Add(DC_GET_SCAN_MODE);
	cmdList->Add(DC_SET_SCAN_MODE);
	cmdList->Add(DC_GET_SHUTTER_TIME);
	cmdList->Add(DC_SET_SHUTTER_TIME);
	cmdList->Add(DC_GET_GAIN);
	cmdList->Add(DC_SET_GAIN);
	cmdList->Add(DC_GET_LED_LEVEL);
	cmdList->Add(DC_SET_LED_LEVEL);
	cmdList->Add(DC_GET_LED_MODE);
	cmdList->Add(DC_SET_LED_MODE);
	cmdList->Add(DC_GET_OUTPUT_TIMING);
	cmdList->Add(DC_SET_OUTPUT_TIMING);
	cmdList->Add(DC_GET_UNREAD_DATA_SEND);
	cmdList->Add(DC_SET_UNREAD_DATA_SEND);
	cmdList->Add(DC_GET_INDIR_TIME);
	cmdList->Add(DC_SET_INDIR_TIME);
	cmdList->Add(DC_GET_TRIGGER_DELAY);
	cmdList->Add(DC_SET_TRIGGER_DELAY);
	cmdList->Add(DC_GET_SIGNAL_ON_DUR);
	cmdList->Add(DC_SET_SIGNAL_ON_DUR);
	cmdList->Add(DC_GET_SIGNAL_DELAY);
	cmdList->Add(DC_SET_SIGNAL_DELAY);
	cmdList->Add(DC_GET_LIGHT_LED);
	cmdList->Add(DC_SET_LIGHT_LED);
	cmdList->Add(DC_GET_MARKER_LIGHT);
	cmdList->Add(DC_SET_MARKER_LIGHT);
	cmdList->Add(DC_GET_DECODE_TIME_LIMIT);
	cmdList->Add(DC_SET_DECODE_TIME_LIMIT);
	cmdList->Add(DC_GET_OUTPUT1_TYPE);
	cmdList->Add(DC_SET_OUTPUT1_TYPE);
	cmdList->Add(DC_GET_OUTPUT2_TYPE);
	cmdList->Add(DC_SET_OUTPUT2_TYPE);
	cmdList->Add(DC_GET_AUTO_SENSE_MODE);
	cmdList->Add(DC_SET_AUTO_SENSE_MODE);
	cmdList->Add(DC_GET_CONT_READ_MODE_B);
	cmdList->Add(DC_SET_CONT_READ_MODE_B);
	cmdList->Add(DC_GET_QRCODE);
	cmdList->Add(DC_SET_QRCODE);
	cmdList->Add(DC_GET_MICRO_QRCODE);
	cmdList->Add(DC_SET_MICRO_QRCODE);
	cmdList->Add(DC_GET_PDF417);
	cmdList->Add(DC_SET_PDF417);
	cmdList->Add(DC_GET_DATAMATRIX);
	cmdList->Add(DC_SET_DATAMATRIX);
	cmdList->Add(DC_GET_BARCODE);
	cmdList->Add(DC_SET_BARCODE);
	cmdList->Add(DC_GET_INTERLEAVED_2OF5);
	cmdList->Add(DC_SET_INTERLEAVED_2OF5);
	cmdList->Add(DC_GET_CODABAR);
	cmdList->Add(DC_SET_CODABAR);
	cmdList->Add(DC_GET_CODABAR_START_STOP);
	cmdList->Add(DC_SET_CODEBAR_START_STOP);
	cmdList->Add(DC_GET_CODE39);
	cmdList->Add(DC_SET_CODE39);
	cmdList->Add(DC_GET_CODE128);
	cmdList->Add(DC_SET_CODE128);
	cmdList->Add(DC_GET_QRCODE_REVERSE);
	cmdList->Add(DC_SET_QRCODE_REVERSE);
	cmdList->Add(DC_GET_QRLINK_CODE);
	cmdList->Add(DC_SET_QRLINK_CODE);
	cmdList->Add(DC_GET_GS1_DATABAR);
	cmdList->Add(DC_SET_GS1_DATABAR);
	cmdList->Add(DC_GET_GS1_COMPOSITE);
	cmdList->Add(DC_SET_GS1_COMPOSITE);
	cmdList->Add(DC_GET_MICRO_PDF417);
	cmdList->Add(DC_SET_MICRO_PDF417);
	cmdList->Add(DC_GET_BARCODE_READ_MODE);
	cmdList->Add(DC_SET_BARCODE_READ_MODE);
	cmdList->Add(DC_GET_SQRC);
	cmdList->Add(DC_SET_SQRC);
	cmdList->Add(DC_GET_SQRC_KEY_UNMATCH);
	cmdList->Add(DC_SET_SQRC_KEY_UNMATCH);
	cmdList->Add(DC_GET_SQRC_KEY_MATCH);
	cmdList->Add(DC_SET_SQRC_KEY_MATCH);
	cmdList->Add(DC_GET_IQRCODE_SQUARE);
	cmdList->Add(DC_SET_IQRCODE_SQUARE);
	cmdList->Add(DC_GET_IQRCODE_RECT);
	cmdList->Add(DC_SET_IQRCODE_RECT);
	cmdList->Add(DC_GET_AZTEC_FULL);
	cmdList->Add(DC_SET_AZTEC_FULL);
	cmdList->Add(DC_GET_AZTEC_COMPACT);
	cmdList->Add(DC_SET_AZTEC_COMPACT);
	cmdList->Add(DC_GET_MENU_READ);
	cmdList->Add(DC_SET_MENU_READ);
	cmdList->Add(DC_GET_COMM_SEQ);
	cmdList->Add(DC_SET_COMM_SEQ);
	cmdList->Add(DC_GET_BAUD_RATE);
	cmdList->Add(DC_SET_BAUD_RATE);
	cmdList->Add(DC_GET_CODE_MARK);
	cmdList->Add(DC_SET_CODE_MARK);
	cmdList->Add(DC_GET_LINE_NUM);
	cmdList->Add(DC_SET_LINE_NUM);
	cmdList->Add(DC_GET_BCC);
	cmdList->Add(DC_SET_BCC);
	cmdList->Add(DC_GET_CTS_SIGNAL);
	cmdList->Add(DC_SET_CTS_SIGNAL);
	cmdList->Add(DC_GET_CTS_TIME);
	cmdList->Add(DC_SET_CTS_TIME);
	cmdList->Add(DC_GET_ACK_NAK_TIME);
	cmdList->Add(DC_SET_ACK_NAK_TIME);
	cmdList->Add(DC_GET_RECV_HDR);
	cmdList->Add(DC_SET_RECV_HDR);
	cmdList->Add(DC_GET_RECV_TERMINATOR);
	cmdList->Add(DC_GET_SEND_TERMINATOR);
	cmdList->Add(DC_GET_BUZZER);
	cmdList->Add(DC_SET_BUZZER);
	cmdList->Add(DC_GET_READ_ERR_BUZZER);
	cmdList->Add(DC_SET_READ_ERR_BUZZER);
	cmdList->Add(DC_GET_MAGIC_KEY);
	cmdList->Add(DC_SET_MAGIC_KEY);
	cmdList->Add(DC_GET_POWER_ON_BUZZER);
	cmdList->Add(DC_SET_POWER_ON_BUZZER);
	cmdList->Add(DC_GET_BUZZER_OFF);
	cmdList->Add(DC_SET_BUZZER_OFF);
	return cmdList->GetCount() - initCnt;
}

Text::CString IO::Device::DensoWaveQB30::GetCommandName(DeviceCommand dcmd)
{
	switch (dcmd)
	{
	case DC_GET_READ_MODE:
		return CSTR("Get Read Mode");
	case DC_SET_READ_MODE:
		return CSTR("Set Read Mode");
	case DC_GET_BW_MODE:
		return CSTR("Get B/W Mode");
	case DC_SET_BW_MODE:
		return CSTR("Set B/W Mode");
	case DC_GET_READ_REPEAT_TIME:
		return CSTR("Get Read Repeat Time");
	case DC_SET_READ_REPEAT_TIME:
		return CSTR("Set Read Repeat Time");
	case DC_GET_BRIGHTNESS:
		return CSTR("Get Brightness");
	case DC_SET_BRIGHTNESS:
		return CSTR("Set Brightness");
	case DC_GET_SHT_SIGNAL:
		return CSTR("Get SHT Signal");
	case DC_SET_SHT_SIGNAL:
		return CSTR("Set SHT Signal");
	case DC_GET_SCAN_MODE:
		return CSTR("Get Scan Mode");
	case DC_SET_SCAN_MODE:
		return CSTR("Set Scan Mode");
	case DC_GET_SHUTTER_TIME:
		return CSTR("Get Shutter Time");
	case DC_SET_SHUTTER_TIME:
		return CSTR("Set Shutter Time");
	case DC_GET_GAIN:
		return CSTR("Get Gain");
	case DC_SET_GAIN:
		return CSTR("Set Gain");
	case DC_GET_LED_LEVEL:
		return CSTR("Get LED Level");
	case DC_SET_LED_LEVEL:
		return CSTR("Set LED Level");
	case DC_GET_LED_MODE:
		return CSTR("Get LED Mode");
	case DC_SET_LED_MODE:
		return CSTR("Set LED Mode");
	case DC_GET_OUTPUT_TIMING:
		return CSTR("Get Data Output Timing");
	case DC_SET_OUTPUT_TIMING:
		return CSTR("Set Data Output Timing");
	case DC_GET_UNREAD_DATA_SEND:
		return CSTR("Get Unread Code Transfer Mode");
	case DC_SET_UNREAD_DATA_SEND:
		return CSTR("Set Unread Code Transfer Mode");
	case DC_GET_INDIR_TIME:
		return CSTR("Get Indirect Mode One-shot Time");
	case DC_SET_INDIR_TIME:
		return CSTR("Set Indirect Mode One-shot Time");
	case DC_GET_TRIGGER_DELAY:
		return CSTR("Get Trigger Signal Input Delay Time");
	case DC_SET_TRIGGER_DELAY:
		return CSTR("Set Trigger Signal Input Delay Time");
	case DC_GET_SIGNAL_ON_DUR:
		return CSTR("Get Signal Output ON Time Range");
	case DC_SET_SIGNAL_ON_DUR:
		return CSTR("Set Signal Output ON Time Range");
	case DC_GET_SIGNAL_DELAY:
		return CSTR("Get Signal Output Delay Time");
	case DC_SET_SIGNAL_DELAY:
		return CSTR("Set Signal Output Delay Time");
	case DC_GET_LIGHT_LED:
		return CSTR("Get Light LED");
	case DC_SET_LIGHT_LED:
		return CSTR("Set Light LED");
	case DC_GET_MARKER_LIGHT:
		return CSTR("Get Marker Light");
	case DC_SET_MARKER_LIGHT:
		return CSTR("Set Marker Light");
	case DC_GET_DECODE_TIME_LIMIT:
		return CSTR("Get Decode Time Limit");
	case DC_SET_DECODE_TIME_LIMIT:
		return CSTR("Set Decode Time Limit");
	case DC_GET_OUTPUT1_TYPE:
		return CSTR("Get Output 1 Setting");
	case DC_SET_OUTPUT1_TYPE:
		return CSTR("Set Output 1 Setting");
	case DC_GET_OUTPUT2_TYPE:
		return CSTR("Get Output 2 Setting");
	case DC_SET_OUTPUT2_TYPE:
		return CSTR("Set Output 2 Setting");
	case DC_GET_AUTO_SENSE_MODE:
		return CSTR("Get Auto-Sense Mode Read Command");
	case DC_SET_AUTO_SENSE_MODE:
		return CSTR("Set Auto-Sense Mode Read Command");
	case DC_GET_CONT_READ_MODE_B:
		return CSTR("Get Continuous Read Mode B Read Command");
	case DC_SET_CONT_READ_MODE_B:
		return CSTR("Set Continuous Read Mode B Read Command");
	case DC_GET_QRCODE:
		return CSTR("Get QR Code Reading");
	case DC_SET_QRCODE:
		return CSTR("Set QR Code Reading");
	case DC_GET_MICRO_QRCODE:
		return CSTR("Get Micro QR Code Reading");
	case DC_SET_MICRO_QRCODE:
		return CSTR("Set Micro QR Code Reading");
	case DC_GET_PDF417:
		return CSTR("Get PDF417 Reading");
	case DC_SET_PDF417:
		return CSTR("Set PDF417 Reading");
	case DC_GET_DATAMATRIX:
		return CSTR("Get DataMatrix Reading");
	case DC_SET_DATAMATRIX:
		return CSTR("Set DataMatrix Reading");
	case DC_GET_BARCODE:
		return CSTR("Get UPC-A, UPC-E, EAN-13, EAN-8 Code Reading");
	case DC_SET_BARCODE:
		return CSTR("Set UPC-A, UPC-E, EAN-13, EAN-8 Code Reading");
	case DC_GET_INTERLEAVED_2OF5:
		return CSTR("Get Interleaved 2of5 Code Reading");
	case DC_SET_INTERLEAVED_2OF5:
		return CSTR("Set Interleaved 2of5 Code Reading");
	case DC_GET_CODABAR:
		return CSTR("Get CODEBAR(NW-7) Code Reading");
	case DC_SET_CODABAR:
		return CSTR("Set CODEBAR(NW-7) Code Reading");
	case DC_GET_CODABAR_START_STOP:
		return CSTR("Get CODEBAR(NW-7) Start/Stop Code Send");
	case DC_SET_CODEBAR_START_STOP:
		return CSTR("Set CODEBAR(NW-7) Start/Stop Code Send");
	case DC_GET_CODE39:
		return CSTR("Get CODE39 Code Reading");
	case DC_SET_CODE39:
		return CSTR("Set CODE39 Code Reading");
	case DC_GET_CODE128:
		return CSTR("Get CODE128, GSI-128 Code Reading");
	case DC_SET_CODE128:
		return CSTR("Set CODE128, GSI-128 Code Reading");
	case DC_GET_QRCODE_REVERSE:
		return CSTR("Get QR Code, Micro QR Code, SQRC, iQR, DataMatrix, iQR, Aztec Invert");
	case DC_SET_QRCODE_REVERSE:
		return CSTR("Set QR Code, Micro QR Code, SQRC, iQR, DataMatrix, iQR, Aztec Invert");
	case DC_GET_QRLINK_CODE:
		return CSTR("Get QR Link Code Mode Setting");
	case DC_SET_QRLINK_CODE:
		return CSTR("Set QR Link Code Mode Setting");
	case DC_GET_GS1_DATABAR:
		return CSTR("Get GS1 DataBar Reading");
	case DC_SET_GS1_DATABAR:
		return CSTR("Set GS1 DataBar Reading");
	case DC_GET_GS1_COMPOSITE:
		return CSTR("Get GS1 Composite Reading");
	case DC_SET_GS1_COMPOSITE:
		return CSTR("Set GS1 Composite Reading");
	case DC_GET_MICRO_PDF417:
		return CSTR("Get Micro PDF417 Reading");
	case DC_SET_MICRO_PDF417:
		return CSTR("Set Micro PDF417 Reading");
	case DC_GET_BARCODE_READ_MODE:
		return CSTR("Get Barcode Reader Mode");
	case DC_SET_BARCODE_READ_MODE:
		return CSTR("Set Barcode Reader Mode");
	case DC_GET_SQRC:
		return CSTR("Get SQRC Reading");
	case DC_SET_SQRC:
		return CSTR("Set SQRC Reading");
	case DC_GET_SQRC_KEY_UNMATCH:
		return CSTR("Get SQRC Key Match Action");
	case DC_SET_SQRC_KEY_UNMATCH:
		return CSTR("Set SQRC Key Match Action");
	case DC_GET_SQRC_KEY_MATCH:
		return CSTR("Get SQRC Key Unmatch Action");
	case DC_SET_SQRC_KEY_MATCH:
		return CSTR("Set SQRC Key Unmatch Action");
	case DC_GET_IQRCODE_SQUARE:
		return CSTR("Get iQR Code(Square) Reading");
	case DC_SET_IQRCODE_SQUARE:
		return CSTR("Set iQR Code(Square) Reading");
	case DC_GET_IQRCODE_RECT:
		return CSTR("Get iQR Code(Rectangle) Reading");
	case DC_SET_IQRCODE_RECT:
		return CSTR("Set iQR Code(Rectangle) Reading");
	case DC_GET_AZTEC_FULL:
		return CSTR("Get Aztec Code(Full-range) Reading");
	case DC_SET_AZTEC_FULL:
		return CSTR("Set Aztec Code(Full-range) Reading");
	case DC_GET_AZTEC_COMPACT:
		return CSTR("Get Aztec Code(Compact) Reading");
	case DC_SET_AZTEC_COMPACT:
		return CSTR("Set Aztec Code(Compact) Reading");
	case DC_GET_MENU_READ:
		return CSTR("Get Menu Reading");
	case DC_SET_MENU_READ:
		return CSTR("Set Menu Reading");
	case DC_GET_COMM_SEQ:
		return CSTR("Get Communication Sequence");
	case DC_SET_COMM_SEQ:
		return CSTR("Set Communication Sequence");
	case DC_GET_BAUD_RATE:
		return CSTR("Get Baud Rate");
	case DC_SET_BAUD_RATE:
		return CSTR("Set Baud Rate");
	case DC_GET_CODE_MARK:
		return CSTR("Get Code Mark Transmission");
	case DC_SET_CODE_MARK:
		return CSTR("Set Code Mark Transmission");
	case DC_GET_LINE_NUM:
		return CSTR("Get Line Number Transmission");
	case DC_SET_LINE_NUM:
		return CSTR("Set Line Number Transmission");
	case DC_GET_BCC:
		return CSTR("Get BCC Transmission");
	case DC_SET_BCC:
		return CSTR("Set BCC Transmission");
	case DC_GET_CTS_SIGNAL:
		return CSTR("Get CTS Signal Control");
	case DC_SET_CTS_SIGNAL:
		return CSTR("Set CTS Signal Control");
	case DC_GET_CTS_TIME:
		return CSTR("Get CTS Interval");
	case DC_SET_CTS_TIME:
		return CSTR("Set CTS Interval");
	case DC_GET_ACK_NAK_TIME:
		return CSTR("Get ACK/NAK Response Time");
	case DC_SET_ACK_NAK_TIME:
		return CSTR("Set ACK/NAK Response Time");
	case DC_GET_RECV_HDR:
		return CSTR("Get Receive Header");
	case DC_SET_RECV_HDR:
		return CSTR("Set Receive Header");
	case DC_GET_RECV_TERMINATOR:
		return CSTR("Get Receive Terminator");
	case DC_GET_SEND_TERMINATOR:
		return CSTR("Get Send Terminator");
	case DC_GET_BUZZER:
		return CSTR("Get Buzzer Sound");
	case DC_SET_BUZZER:
		return CSTR("Set Buzzer Sound");
	case DC_GET_READ_ERR_BUZZER:
		return CSTR("Get Read Error Buzzer");
	case DC_SET_READ_ERR_BUZZER:
		return CSTR("Set Read Error Buzzer");
	case DC_GET_MAGIC_KEY:
		return CSTR("Get Magic Key Function");
	case DC_SET_MAGIC_KEY:
		return CSTR("Set Magic Key Function");
	case DC_GET_POWER_ON_BUZZER:
		return CSTR("Get Power On Buzzer");
	case DC_SET_POWER_ON_BUZZER:
		return CSTR("Set Power On Buzzer");
	case DC_GET_BUZZER_OFF:
		return CSTR("Get Force Buzzer Off");
	case DC_SET_BUZZER_OFF:
		return CSTR("Set Force Buzzer Off");
	default:
		return CSTR("Unknown");
	}
}

IO::Device::DensoWaveQB30::CommandType IO::Device::DensoWaveQB30::GetCommandParamType(DeviceCommand dcmd, Int32 *minVal, Int32 *maxVal)
{
	switch (dcmd)
	{
	case DC_GET_READ_MODE:
		*minVal = 0;
		*maxVal = 6;
		return IO::Device::DensoWaveQB30::CT_GET_COMMAND_NAME;
	case DC_SET_READ_MODE:
		*minVal = 0;
		*maxVal = 6;
		return IO::Device::DensoWaveQB30::CT_SELECT_COMMAND;
	case DC_GET_BW_MODE:
		*minVal = 0;
		*maxVal = 2;
		return IO::Device::DensoWaveQB30::CT_GET_COMMAND_NAME;
	case DC_SET_BW_MODE:
		*minVal = 0;
		*maxVal = 2;
		return IO::Device::DensoWaveQB30::CT_SELECT_COMMAND;
	case DC_GET_READ_REPEAT_TIME:
		*minVal = 0;
		*maxVal = 99;
		return IO::Device::DensoWaveQB30::CT_GET_COMMAND;
	case DC_SET_READ_REPEAT_TIME:
		*minVal = 0;
		*maxVal = 99;
		return IO::Device::DensoWaveQB30::CT_SET_COMMAND;
	case DC_GET_BRIGHTNESS:
		*minVal = 0;
		*maxVal = 3;
		return IO::Device::DensoWaveQB30::CT_GET_COMMAND_NAME;
	case DC_SET_BRIGHTNESS:
		*minVal = 0;
		*maxVal = 3;
		return IO::Device::DensoWaveQB30::CT_SELECT_COMMAND;
	case DC_GET_SHT_SIGNAL:
		*minVal = 0;
		*maxVal = 1;
		return IO::Device::DensoWaveQB30::CT_GET_COMMAND_NAME;
	case DC_SET_SHT_SIGNAL:
		*minVal = 0;
		*maxVal = 1;
		return IO::Device::DensoWaveQB30::CT_SELECT_COMMAND;
	case DC_GET_SCAN_MODE:
		*minVal = 0;
		*maxVal = 2;
		return IO::Device::DensoWaveQB30::CT_GET_COMMAND_NAME;
	case DC_SET_SCAN_MODE:
		*minVal = 0;
		*maxVal = 2;
		return IO::Device::DensoWaveQB30::CT_SELECT_COMMAND;
	case DC_GET_SHUTTER_TIME:
		*minVal = 1;
		*maxVal = 99;
		return IO::Device::DensoWaveQB30::CT_GET_COMMAND;
	case DC_SET_SHUTTER_TIME:
		*minVal = 1;
		*maxVal = 99;
		return IO::Device::DensoWaveQB30::CT_SET_COMMAND;
	case DC_GET_GAIN:
		*minVal = 0;
		*maxVal = 15;
		return IO::Device::DensoWaveQB30::CT_GET_COMMAND;
	case DC_SET_GAIN:
		*minVal = 0;
		*maxVal = 15;
		return IO::Device::DensoWaveQB30::CT_SET_COMMAND;
	case DC_GET_LED_LEVEL:
		*minVal = 0;
		*maxVal = 2;
		return IO::Device::DensoWaveQB30::CT_GET_COMMAND_NAME;
	case DC_SET_LED_LEVEL:
		*minVal = 0;
		*maxVal = 2;
		return IO::Device::DensoWaveQB30::CT_SELECT_COMMAND;
	case DC_GET_LED_MODE:
		*minVal = 0;
		*maxVal = 1;
		return IO::Device::DensoWaveQB30::CT_GET_COMMAND_NAME;
	case DC_SET_LED_MODE:
		*minVal = 0;
		*maxVal = 1;
		return IO::Device::DensoWaveQB30::CT_SELECT_COMMAND;
	case DC_GET_OUTPUT_TIMING:
		*minVal = 0;
		*maxVal = 1;
		return IO::Device::DensoWaveQB30::CT_GET_COMMAND_NAME;
	case DC_SET_OUTPUT_TIMING:
		*minVal = 0;
		*maxVal = 1;
		return IO::Device::DensoWaveQB30::CT_SELECT_COMMAND;
	case DC_GET_UNREAD_DATA_SEND:
		*minVal = 0;
		*maxVal = 1;
		return IO::Device::DensoWaveQB30::CT_GET_COMMAND_NAME;
	case DC_SET_UNREAD_DATA_SEND:
		*minVal = 0;
		*maxVal = 1;
		return IO::Device::DensoWaveQB30::CT_SELECT_COMMAND;
	case DC_GET_INDIR_TIME:
		*minVal = 10;
		*maxVal = 999;
		return IO::Device::DensoWaveQB30::CT_GET_COMMAND;
	case DC_SET_INDIR_TIME:
		*minVal = 10;
		*maxVal = 999;
		return IO::Device::DensoWaveQB30::CT_SET_COMMAND;
	case DC_GET_TRIGGER_DELAY:
		*minVal = 0;
		*maxVal = 999;
		return IO::Device::DensoWaveQB30::CT_GET_COMMAND;
	case DC_SET_TRIGGER_DELAY:
		*minVal = 0;
		*maxVal = 999;
		return IO::Device::DensoWaveQB30::CT_SET_COMMAND;
	case DC_GET_SIGNAL_ON_DUR:
		*minVal = 0;
		*maxVal = 255;
		return IO::Device::DensoWaveQB30::CT_GET_COMMAND;
	case DC_SET_SIGNAL_ON_DUR:
		*minVal = 0;
		*maxVal = 255;
		return IO::Device::DensoWaveQB30::CT_SET_COMMAND;
	case DC_GET_SIGNAL_DELAY:
		*minVal = 0;
		*maxVal = 99;
		return IO::Device::DensoWaveQB30::CT_GET_COMMAND;
	case DC_SET_SIGNAL_DELAY:
		*minVal = 0;
		*maxVal = 99;
		return IO::Device::DensoWaveQB30::CT_SET_COMMAND;
	case DC_GET_LIGHT_LED:
		*minVal = 0;
		*maxVal = 2;
		return IO::Device::DensoWaveQB30::CT_GET_COMMAND_NAME;
	case DC_SET_LIGHT_LED:
		*minVal = 0;
		*maxVal = 2;
		return IO::Device::DensoWaveQB30::CT_SELECT_COMMAND;
	case DC_GET_MARKER_LIGHT:
		*minVal = 0;
		*maxVal = 1;
		return IO::Device::DensoWaveQB30::CT_GET_COMMAND_NAME;
	case DC_SET_MARKER_LIGHT:
		*minVal = 0;
		*maxVal = 1;
		return IO::Device::DensoWaveQB30::CT_SELECT_COMMAND;
	case DC_GET_DECODE_TIME_LIMIT:
		*minVal = 0;
		*maxVal = 999;
		return IO::Device::DensoWaveQB30::CT_GET_COMMAND;
	case DC_SET_DECODE_TIME_LIMIT:
		*minVal = 0;
		*maxVal = 999;
		return IO::Device::DensoWaveQB30::CT_SET_COMMAND;
	case DC_GET_OUTPUT1_TYPE:
		*minVal = 0;
		*maxVal = 5;
		return IO::Device::DensoWaveQB30::CT_GET_COMMAND_NAME;
	case DC_SET_OUTPUT1_TYPE:
		*minVal = 0;
		*maxVal = 5;
		return IO::Device::DensoWaveQB30::CT_SELECT_COMMAND;
	case DC_GET_OUTPUT2_TYPE:
		*minVal = 0;
		*maxVal = 6;
		return IO::Device::DensoWaveQB30::CT_GET_COMMAND_NAME;
	case DC_SET_OUTPUT2_TYPE:
		*minVal = 0;
		*maxVal = 6;
		return IO::Device::DensoWaveQB30::CT_SELECT_COMMAND;
	case DC_GET_AUTO_SENSE_MODE:
		*minVal = 0;
		*maxVal = 1;
		return IO::Device::DensoWaveQB30::CT_GET_COMMAND_NAME;
	case DC_SET_AUTO_SENSE_MODE:
		*minVal = 0;
		*maxVal = 1;
		return IO::Device::DensoWaveQB30::CT_SELECT_COMMAND;
	case DC_GET_CONT_READ_MODE_B:
		*minVal = 0;
		*maxVal = 1;
		return IO::Device::DensoWaveQB30::CT_GET_COMMAND_NAME;
	case DC_SET_CONT_READ_MODE_B:
		*minVal = 0;
		*maxVal = 1;
		return IO::Device::DensoWaveQB30::CT_SELECT_COMMAND;
	case DC_GET_QRCODE:
		*minVal = 0;
		*maxVal = 1;
		return IO::Device::DensoWaveQB30::CT_GET_COMMAND_NAME;
	case DC_SET_QRCODE:
		*minVal = 0;
		*maxVal = 1;
		return IO::Device::DensoWaveQB30::CT_SELECT_COMMAND;
	case DC_GET_MICRO_QRCODE:
		*minVal = 0;
		*maxVal = 1;
		return IO::Device::DensoWaveQB30::CT_GET_COMMAND_NAME;
	case DC_SET_MICRO_QRCODE:
		*minVal = 0;
		*maxVal = 1;
		return IO::Device::DensoWaveQB30::CT_SELECT_COMMAND;
	case DC_GET_PDF417:
		*minVal = 0;
		*maxVal = 1;
		return IO::Device::DensoWaveQB30::CT_GET_COMMAND_NAME;
	case DC_SET_PDF417:
		*minVal = 0;
		*maxVal = 1;
		return IO::Device::DensoWaveQB30::CT_SELECT_COMMAND;
	case DC_GET_DATAMATRIX:
		*minVal = 0;
		*maxVal = 1;
		return IO::Device::DensoWaveQB30::CT_GET_COMMAND_NAME;
	case DC_SET_DATAMATRIX:
		*minVal = 0;
		*maxVal = 1;
		return IO::Device::DensoWaveQB30::CT_SELECT_COMMAND;
	case DC_GET_BARCODE:
		*minVal = 0;
		*maxVal = 1;
		return IO::Device::DensoWaveQB30::CT_GET_COMMAND_NAME;
	case DC_SET_BARCODE:
		*minVal = 0;
		*maxVal = 1;
		return IO::Device::DensoWaveQB30::CT_SELECT_COMMAND;
	case DC_GET_INTERLEAVED_2OF5:
		*minVal = 0;
		*maxVal = 3;
		return IO::Device::DensoWaveQB30::CT_GET_COMMAND_NAME;
	case DC_SET_INTERLEAVED_2OF5:
		*minVal = 0;
		*maxVal = 3;
		return IO::Device::DensoWaveQB30::CT_SELECT_COMMAND;
	case DC_GET_CODABAR:
		*minVal = 0;
		*maxVal = 3;
		return IO::Device::DensoWaveQB30::CT_GET_COMMAND_NAME;
	case DC_SET_CODABAR:
		*minVal = 0;
		*maxVal = 3;
		return IO::Device::DensoWaveQB30::CT_SELECT_COMMAND;
	case DC_GET_CODABAR_START_STOP:
		*minVal = 0;
		*maxVal = 1;
		return IO::Device::DensoWaveQB30::CT_GET_COMMAND_NAME;
	case DC_SET_CODEBAR_START_STOP:
		*minVal = 0;
		*maxVal = 1;
		return IO::Device::DensoWaveQB30::CT_SELECT_COMMAND;
	case DC_GET_CODE39:
		*minVal = 0;
		*maxVal = 3;
		return IO::Device::DensoWaveQB30::CT_GET_COMMAND_NAME;
	case DC_SET_CODE39:
		*minVal = 0;
		*maxVal = 3;
		return IO::Device::DensoWaveQB30::CT_SELECT_COMMAND;
	case DC_GET_CODE128:
		*minVal = 0;
		*maxVal = 1;
		return IO::Device::DensoWaveQB30::CT_GET_COMMAND_NAME;
	case DC_SET_CODE128:
		*minVal = 0;
		*maxVal = 1;
		return IO::Device::DensoWaveQB30::CT_SELECT_COMMAND;
	case DC_GET_QRCODE_REVERSE:
		*minVal = 0;
		*maxVal = 1;
		return IO::Device::DensoWaveQB30::CT_GET_COMMAND_NAME;
	case DC_SET_QRCODE_REVERSE:
		*minVal = 0;
		*maxVal = 1;
		return IO::Device::DensoWaveQB30::CT_SELECT_COMMAND;
	case DC_GET_QRLINK_CODE:
		*minVal = 0;
		*maxVal = 2;
		return IO::Device::DensoWaveQB30::CT_GET_COMMAND_NAME;
	case DC_SET_QRLINK_CODE:
		*minVal = 0;
		*maxVal = 2;
		return IO::Device::DensoWaveQB30::CT_SELECT_COMMAND;
	case DC_GET_GS1_DATABAR:
		*minVal = 0;
		*maxVal = 1;
		return IO::Device::DensoWaveQB30::CT_GET_COMMAND_NAME;
	case DC_SET_GS1_DATABAR:
		*minVal = 0;
		*maxVal = 1;
		return IO::Device::DensoWaveQB30::CT_SELECT_COMMAND;
	case DC_GET_GS1_COMPOSITE:
		*minVal = 0;
		*maxVal = 1;
		return IO::Device::DensoWaveQB30::CT_GET_COMMAND_NAME;
	case DC_SET_GS1_COMPOSITE:
		*minVal = 0;
		*maxVal = 1;
		return IO::Device::DensoWaveQB30::CT_SELECT_COMMAND;
	case DC_GET_MICRO_PDF417:
		*minVal = 0;
		*maxVal = 1;
		return IO::Device::DensoWaveQB30::CT_GET_COMMAND_NAME;
	case DC_SET_MICRO_PDF417:
		*minVal = 0;
		*maxVal = 1;
		return IO::Device::DensoWaveQB30::CT_SELECT_COMMAND;
	case DC_GET_BARCODE_READ_MODE:
		*minVal = 0;
		*maxVal = 1;
		return IO::Device::DensoWaveQB30::CT_GET_COMMAND_NAME;
	case DC_SET_BARCODE_READ_MODE:
		*minVal = 0;
		*maxVal = 1;
		return IO::Device::DensoWaveQB30::CT_SELECT_COMMAND;
	case DC_GET_SQRC:
		*minVal = 0;
		*maxVal = 1;
		return IO::Device::DensoWaveQB30::CT_GET_COMMAND_NAME;
	case DC_SET_SQRC:
		*minVal = 0;
		*maxVal = 1;
		return IO::Device::DensoWaveQB30::CT_SELECT_COMMAND;
	case DC_GET_SQRC_KEY_UNMATCH:
		*minVal = 0;
		*maxVal = 1;
		return IO::Device::DensoWaveQB30::CT_GET_COMMAND_NAME;
	case DC_SET_SQRC_KEY_UNMATCH:
		*minVal = 0;
		*maxVal = 1;
		return IO::Device::DensoWaveQB30::CT_SELECT_COMMAND;
	case DC_GET_SQRC_KEY_MATCH:
		*minVal = 0;
		*maxVal = 1;
		return IO::Device::DensoWaveQB30::CT_GET_COMMAND_NAME;
	case DC_SET_SQRC_KEY_MATCH:
		*minVal = 0;
		*maxVal = 1;
		return IO::Device::DensoWaveQB30::CT_SELECT_COMMAND;
	case DC_GET_IQRCODE_SQUARE:
		*minVal = 0;
		*maxVal = 1;
		return IO::Device::DensoWaveQB30::CT_GET_COMMAND_NAME;
	case DC_SET_IQRCODE_SQUARE:
		*minVal = 0;
		*maxVal = 1;
		return IO::Device::DensoWaveQB30::CT_SELECT_COMMAND;
	case DC_GET_IQRCODE_RECT:
		*minVal = 0;
		*maxVal = 1;
		return IO::Device::DensoWaveQB30::CT_GET_COMMAND_NAME;
	case DC_SET_IQRCODE_RECT:
		*minVal = 0;
		*maxVal = 1;
		return IO::Device::DensoWaveQB30::CT_SELECT_COMMAND;
	case DC_GET_AZTEC_FULL:
		*minVal = 0;
		*maxVal = 1;
		return IO::Device::DensoWaveQB30::CT_GET_COMMAND_NAME;
	case DC_SET_AZTEC_FULL:
		*minVal = 0;
		*maxVal = 1;
		return IO::Device::DensoWaveQB30::CT_SELECT_COMMAND;
	case DC_GET_AZTEC_COMPACT:
		*minVal = 0;
		*maxVal = 1;
		return IO::Device::DensoWaveQB30::CT_GET_COMMAND_NAME;
	case DC_SET_AZTEC_COMPACT:
		*minVal = 0;
		*maxVal = 1;
		return IO::Device::DensoWaveQB30::CT_SELECT_COMMAND;
	case DC_GET_MENU_READ:
		*minVal = 0;
		*maxVal = 1;
		return IO::Device::DensoWaveQB30::CT_GET_COMMAND_NAME;
	case DC_SET_MENU_READ:
		*minVal = 0;
		*maxVal = 1;
		return IO::Device::DensoWaveQB30::CT_SELECT_COMMAND;
	case DC_GET_COMM_SEQ:
		*minVal = 0;
		*maxVal = 1;
		return IO::Device::DensoWaveQB30::CT_GET_COMMAND_NAME;
	case DC_SET_COMM_SEQ:
		*minVal = 0;
		*maxVal = 1;
		return IO::Device::DensoWaveQB30::CT_SELECT_COMMAND;
	case DC_GET_BAUD_RATE:
		*minVal = 0;
		*maxVal = 5;
		return IO::Device::DensoWaveQB30::CT_GET_COMMAND_NAME;
	case DC_SET_BAUD_RATE:
		*minVal = 0;
		*maxVal = 5;
		return IO::Device::DensoWaveQB30::CT_SELECT_COMMAND;
	case DC_GET_CODE_MARK:
		*minVal = 0;
		*maxVal = 1;
		return IO::Device::DensoWaveQB30::CT_GET_COMMAND_NAME;
	case DC_SET_CODE_MARK:
		*minVal = 0;
		*maxVal = 1;
		return IO::Device::DensoWaveQB30::CT_SELECT_COMMAND;
	case DC_GET_LINE_NUM:
		*minVal = 0;
		*maxVal = 2;
		return IO::Device::DensoWaveQB30::CT_GET_COMMAND_NAME;
	case DC_SET_LINE_NUM:
		*minVal = 0;
		*maxVal = 2;
		return IO::Device::DensoWaveQB30::CT_SELECT_COMMAND;
	case DC_GET_BCC:
		*minVal = 0;
		*maxVal = 1;
		return IO::Device::DensoWaveQB30::CT_GET_COMMAND_NAME;
	case DC_SET_BCC:
		*minVal = 0;
		*maxVal = 1;
		return IO::Device::DensoWaveQB30::CT_SELECT_COMMAND;
	case DC_GET_CTS_SIGNAL:
		*minVal = 0;
		*maxVal = 1;
		return IO::Device::DensoWaveQB30::CT_GET_COMMAND_NAME;
	case DC_SET_CTS_SIGNAL:
		*minVal = 0;
		*maxVal = 1;
		return IO::Device::DensoWaveQB30::CT_SELECT_COMMAND;
	case DC_GET_CTS_TIME:
		*minVal = 1;
		*maxVal = 99;
		return IO::Device::DensoWaveQB30::CT_GET_COMMAND;
	case DC_SET_CTS_TIME:
		*minVal = 1;
		*maxVal = 99;
		return IO::Device::DensoWaveQB30::CT_SET_COMMAND;
	case DC_GET_ACK_NAK_TIME:
		*minVal = 1;
		*maxVal = 99;
		return IO::Device::DensoWaveQB30::CT_GET_COMMAND;
	case DC_SET_ACK_NAK_TIME:
		*minVal = 1;
		*maxVal = 99;
		return IO::Device::DensoWaveQB30::CT_SET_COMMAND;
	case DC_GET_RECV_HDR:
		*minVal = 0;
		*maxVal = 1;
		return IO::Device::DensoWaveQB30::CT_GET_COMMAND_NAME;
	case DC_SET_RECV_HDR:
		*minVal = 0;
		*maxVal = 1;
		return IO::Device::DensoWaveQB30::CT_SELECT_COMMAND;
	case DC_GET_RECV_TERMINATOR:
		*minVal = 0;
		*maxVal = 3;
		return IO::Device::DensoWaveQB30::CT_GET_COMMAND_NAME;
	case DC_GET_SEND_TERMINATOR:
		*minVal = 0;
		*maxVal = 5;
		return IO::Device::DensoWaveQB30::CT_GET_COMMAND_NAME;
	case DC_GET_BUZZER:
		*minVal = 0;
		*maxVal = 1;
		return IO::Device::DensoWaveQB30::CT_GET_COMMAND_NAME;
	case DC_SET_BUZZER:
		*minVal = 0;
		*maxVal = 1;
		return IO::Device::DensoWaveQB30::CT_SELECT_COMMAND;
	case DC_GET_READ_ERR_BUZZER:
		*minVal = 0;
		*maxVal = 1;
		return IO::Device::DensoWaveQB30::CT_GET_COMMAND_NAME;
	case DC_SET_READ_ERR_BUZZER:
		*minVal = 0;
		*maxVal = 1;
		return IO::Device::DensoWaveQB30::CT_SELECT_COMMAND;
	case DC_GET_MAGIC_KEY:
		*minVal = 0;
		*maxVal = 1;
		return IO::Device::DensoWaveQB30::CT_GET_COMMAND_NAME;
	case DC_SET_MAGIC_KEY:
		*minVal = 0;
		*maxVal = 1;
		return IO::Device::DensoWaveQB30::CT_SELECT_COMMAND;
	case DC_GET_POWER_ON_BUZZER:
		*minVal = 0;
		*maxVal = 1;
		return IO::Device::DensoWaveQB30::CT_GET_COMMAND_NAME;
	case DC_SET_POWER_ON_BUZZER:
		*minVal = 0;
		*maxVal = 1;
		return IO::Device::DensoWaveQB30::CT_SELECT_COMMAND;
	case DC_GET_BUZZER_OFF:
		*minVal = 0;
		*maxVal = 1;
		return IO::Device::DensoWaveQB30::CT_GET_COMMAND_NAME;
	case DC_SET_BUZZER_OFF:
		*minVal = 0;
		*maxVal = 1;
		return IO::Device::DensoWaveQB30::CT_SELECT_COMMAND;
	default:
		return IO::Device::DensoWaveQB30::CT_UNKNOWN;
	}
}

Text::CString IO::Device::DensoWaveQB30::GetCommandParamName(DeviceCommand dcmd, Int32 cmdVal)
{
	switch (dcmd)
	{
	case DC_GET_READ_MODE:
	case DC_SET_READ_MODE:
		if (cmdVal == -1)
			return CSTR("Unknown");
		if (cmdVal == 0)
			return CSTR("Continuous Read Mode A");
		if (cmdVal == 1)
			return CSTR("Continuous Read Mode B");
		if (cmdVal == 2)
			return CSTR("Direct Trigger Mode");
		if (cmdVal == 3)
			return CSTR("Indirect Trigger Mode");
		if (cmdVal == 4)
			return CSTR("Direct Software Trigger Mode");
		if (cmdVal == 5)
			return CSTR("Indirect Software Trigger Mode");
		if (cmdVal == 6)
			return CSTR("Auto Sense Mode");
		return nullptr;
	case DC_GET_BW_MODE:
	case DC_SET_BW_MODE:
		if (cmdVal == 0)
			return CSTR("Normal Code");
		if (cmdVal == 1)
			return CSTR("Inverted Code");
		if (cmdVal == 2)
			return CSTR("Auto Detect");
		return nullptr;
	case DC_GET_READ_REPEAT_TIME:
	case DC_SET_READ_REPEAT_TIME:
		return nullptr;
	case DC_GET_BRIGHTNESS:
	case DC_SET_BRIGHTNESS:
		if (cmdVal == 0)
			return CSTR("Auto");
		if (cmdVal == 1)
			return CSTR("Fixed Shutter, Auto Gain");
		if (cmdVal == 2)
			return CSTR("Fixed Shutter, Fixed Gain");
		return nullptr;
	case DC_GET_SHT_SIGNAL:
	case DC_SET_SHT_SIGNAL:
		if (cmdVal == 0)
			return CSTR("Tr OFF");
		if (cmdVal == 1)
			return CSTR("Tr ON");
		return nullptr;
	case DC_GET_SCAN_MODE:
	case DC_SET_SCAN_MODE:
		if (cmdVal == 0)
			return CSTR("Capture Mode Off");
		if (cmdVal == 1)
			return CSTR("Scan Entry Mode");
		if (cmdVal == 2)
			return CSTR("File Entry Mode");
		return nullptr;
	case DC_GET_SHUTTER_TIME:
	case DC_SET_SHUTTER_TIME:
		return nullptr;
	case DC_GET_GAIN:
	case DC_SET_GAIN:
		return nullptr;
	case DC_GET_LED_LEVEL:
	case DC_SET_LED_LEVEL:
		if (cmdVal == 0)
			return CSTR("Strong");
		if (cmdVal == 1)
			return CSTR("Medium");
		if (cmdVal == 2)
			return CSTR("Weak");
		return nullptr;
	case DC_GET_LED_MODE:
	case DC_SET_LED_MODE:
		if (cmdVal == 0)
			return CSTR("LED Off");
		if (cmdVal == 1)
			return CSTR("LED On");
		return nullptr;
	case DC_GET_OUTPUT_TIMING:
	case DC_SET_OUTPUT_TIMING:
		if (cmdVal == 0)
			return CSTR("Sync");
		if (cmdVal == 1)
			return CSTR("Async");
		return nullptr;
	case DC_GET_UNREAD_DATA_SEND:
	case DC_SET_UNREAD_DATA_SEND:
		if (cmdVal == 0)
			return CSTR("Allow");
		if (cmdVal == 1)
			return CSTR("Disallow");
		return nullptr;
	case DC_GET_INDIR_TIME:
	case DC_SET_INDIR_TIME:
		return nullptr;
	case DC_GET_TRIGGER_DELAY:
	case DC_SET_TRIGGER_DELAY:
		return nullptr;
	case DC_GET_SIGNAL_ON_DUR:
	case DC_SET_SIGNAL_ON_DUR:
		return nullptr;
	case DC_GET_SIGNAL_DELAY:
	case DC_SET_SIGNAL_DELAY:
		return nullptr;
	case DC_GET_LIGHT_LED:
	case DC_SET_LIGHT_LED:
		if (cmdVal == 0)
			return CSTR("Always Off");
		if (cmdVal == 1)
			return CSTR("Auto");
		if (cmdVal == 2)
			return CSTR("Always On");
		return nullptr;
	case DC_GET_MARKER_LIGHT:
	case DC_SET_MARKER_LIGHT:
		if (cmdVal == 0)
			return CSTR("Always Off");
		if (cmdVal == 1)
			return CSTR("Normal");
		return nullptr;
	case DC_GET_DECODE_TIME_LIMIT:
	case DC_SET_DECODE_TIME_LIMIT:
		return nullptr;
	case DC_GET_OUTPUT1_TYPE:
	case DC_SET_OUTPUT1_TYPE:
		if (cmdVal == 0)
			return CSTR("Unused");
		if (cmdVal == 1)
			return CSTR("Output OK Signal");
		if (cmdVal == 2)
			return CSTR("Output NG Signal");
		if (cmdVal == 3)
			return CSTR("Output Scan OK Signal");
		if (cmdVal == 4)
			return CSTR("Output Scan NG Signal");
		if (cmdVal == 5)
			return CSTR("Output READY Signal");
		return nullptr;
	case DC_GET_OUTPUT2_TYPE:
	case DC_SET_OUTPUT2_TYPE:
		if (cmdVal == 0)
			return CSTR("Unused");
		if (cmdVal == 1)
			return CSTR("Output OK Signal");
		if (cmdVal == 2)
			return CSTR("Output NG Signal");
		if (cmdVal == 3)
			return CSTR("Output Scan OK Signal");
		if (cmdVal == 4)
			return CSTR("Output Scan NG Signal");
		if (cmdVal == 5)
			return CSTR("Output READY Signal");
		if (cmdVal == 6)
			return CSTR("Output SHT Signal");
		return nullptr;
	case DC_GET_AUTO_SENSE_MODE:
	case DC_SET_AUTO_SENSE_MODE:
		if (cmdVal == 0)
			return CSTR("Disable");
		if (cmdVal == 1)
			return CSTR("Enable");
		return nullptr;
	case DC_GET_CONT_READ_MODE_B:
	case DC_SET_CONT_READ_MODE_B:
		if (cmdVal == 0)
			return CSTR("Disable");
		if (cmdVal == 1)
			return CSTR("Enable");
		return nullptr;
	case DC_GET_QRCODE:
	case DC_SET_QRCODE:
		if (cmdVal == 0)
			return CSTR("Allow");
		if (cmdVal == 1)
			return CSTR("Disallow");
		return nullptr;
	case DC_GET_MICRO_QRCODE:
	case DC_SET_MICRO_QRCODE:
		if (cmdVal == 0)
			return CSTR("Allow");
		if (cmdVal == 1)
			return CSTR("Disallow");
		return nullptr;
	case DC_GET_PDF417:
	case DC_SET_PDF417:
		if (cmdVal == 0)
			return CSTR("Allow");
		if (cmdVal == 1)
			return CSTR("Disallow");
		return nullptr;
	case DC_GET_DATAMATRIX:
	case DC_SET_DATAMATRIX:
		if (cmdVal == 0)
			return CSTR("Allow");
		if (cmdVal == 1)
			return CSTR("Disallow");
		return nullptr;
	case DC_GET_BARCODE:
	case DC_SET_BARCODE:
		if (cmdVal == 0)
			return CSTR("Allow");
		if (cmdVal == 1)
			return CSTR("Disallow");
		return nullptr;
	case DC_GET_INTERLEAVED_2OF5:
	case DC_SET_INTERLEAVED_2OF5:
		if (cmdVal == 0)
			return CSTR("No C/D, Allow read");
		if (cmdVal == 1)
			return CSTR("Has C/D, Allow read, C/D Transfer Allowed");
		if (cmdVal == 2)
			return CSTR("Has C/D, Allow read, C/D Transfer not Allowed");
		if (cmdVal == 3)
			return CSTR("Read not allowed");
		return nullptr;
	case DC_GET_CODABAR:
	case DC_SET_CODABAR:
		if (cmdVal == 0)
			return CSTR("No C/D, Allow read");
		if (cmdVal == 1)
			return CSTR("Has C/D, Allow read, C/D Transfer Allowed");
		if (cmdVal == 2)
			return CSTR("Has C/D, Allow read, C/D Transfer not Allowed");
		if (cmdVal == 3)
			return CSTR("Read not allowed");
		return nullptr;
	case DC_GET_CODABAR_START_STOP:
	case DC_SET_CODEBAR_START_STOP:
		if (cmdVal == 0)
			return CSTR("Allow");
		if (cmdVal == 1)
			return CSTR("Disallow");
		return nullptr;
	case DC_GET_CODE39:
	case DC_SET_CODE39:
		if (cmdVal == 0)
			return CSTR("No C/D, Allow read");
		if (cmdVal == 1)
			return CSTR("Has C/D, Allow read, C/D Transfer Allowed");
		if (cmdVal == 2)
			return CSTR("Has C/D, Allow read, C/D Transfer not Allowed");
		if (cmdVal == 3)
			return CSTR("Read not allowed");
		return nullptr;
	case DC_GET_CODE128:
	case DC_SET_CODE128:
		if (cmdVal == 0)
			return CSTR("Allow");
		if (cmdVal == 1)
			return CSTR("Disallow");
		return nullptr;
	case DC_GET_QRCODE_REVERSE:
	case DC_SET_QRCODE_REVERSE:
		if (cmdVal == 0)
			return CSTR("Normal Code");
		if (cmdVal == 1)
			return CSTR("Inverted Code");
		return nullptr;
	case DC_GET_QRLINK_CODE:
	case DC_SET_QRLINK_CODE:
		if (cmdVal == 0)
			return CSTR("Non-editing Mode");
		if (cmdVal == 1)
			return CSTR("All Editing Mode");
		if (cmdVal == 1)
			return CSTR("Editing Mode");
		return nullptr;
	case DC_GET_GS1_DATABAR:
	case DC_SET_GS1_DATABAR:
		if (cmdVal == 0)
			return CSTR("Allow");
		if (cmdVal == 1)
			return CSTR("Disallow");
		return nullptr;
	case DC_GET_GS1_COMPOSITE:
	case DC_SET_GS1_COMPOSITE:
		if (cmdVal == 0)
			return CSTR("Allow");
		if (cmdVal == 1)
			return CSTR("Disallow");
		return nullptr;
	case DC_GET_MICRO_PDF417:
	case DC_SET_MICRO_PDF417:
		if (cmdVal == 0)
			return CSTR("Allow");
		if (cmdVal == 1)
			return CSTR("Disallow");
		return nullptr;
	case DC_GET_BARCODE_READ_MODE:
	case DC_SET_BARCODE_READ_MODE:
		if (cmdVal == 0)
			return CSTR("Allow");
		if (cmdVal == 1)
			return CSTR("Disallow");
		return nullptr;
	case DC_GET_SQRC:
	case DC_SET_SQRC:
		if (cmdVal == 0)
			return CSTR("Allow");
		if (cmdVal == 1)
			return CSTR("Disallow");
		return nullptr;
	case DC_GET_SQRC_KEY_UNMATCH:
	case DC_SET_SQRC_KEY_UNMATCH:
		if (cmdVal == 0)
			return CSTR("Reading not allowed");
		if (cmdVal == 1)
			return CSTR("Only Output Public Data");
		return nullptr;
	case DC_GET_SQRC_KEY_MATCH:
	case DC_SET_SQRC_KEY_MATCH:
		if (cmdVal == 0)
			return CSTR("Output Public Data+Private Data");
		if (cmdVal == 1)
			return CSTR("Only Output Private Data");
		return nullptr;
	case DC_GET_IQRCODE_SQUARE:
	case DC_SET_IQRCODE_SQUARE:
		if (cmdVal == 0)
			return CSTR("Allow");
		if (cmdVal == 1)
			return CSTR("Disallow");
		return nullptr;
	case DC_GET_IQRCODE_RECT:
	case DC_SET_IQRCODE_RECT:
		if (cmdVal == 0)
			return CSTR("Allow");
		if (cmdVal == 1)
			return CSTR("Disallow");
		return nullptr;
	case DC_GET_AZTEC_FULL:
	case DC_SET_AZTEC_FULL:
		if (cmdVal == 0)
			return CSTR("Allow");
		if (cmdVal == 1)
			return CSTR("Disallow");
		return nullptr;
	case DC_GET_AZTEC_COMPACT:
	case DC_SET_AZTEC_COMPACT:
		if (cmdVal == 0)
			return CSTR("Allow");
		if (cmdVal == 1)
			return CSTR("Disallow");
		return nullptr;
	case DC_GET_MENU_READ:
	case DC_SET_MENU_READ:
		if (cmdVal == 0)
			return CSTR("Allow");
		if (cmdVal == 1)
			return CSTR("Disallow");
		return nullptr;
	case DC_GET_COMM_SEQ:
	case DC_SET_COMM_SEQ:
		if (cmdVal == 0)
			return CSTR("No Protocol");
		if (cmdVal == 1)
			return CSTR("ACK/NCK");
		return nullptr;
	case DC_GET_BAUD_RATE:
	case DC_SET_BAUD_RATE:
		if (cmdVal == 0)
			return CSTR("4800bps");
		if (cmdVal == 1)
			return CSTR("9600bps");
		if (cmdVal == 2)
			return CSTR("19200bps");
		if (cmdVal == 3)
			return CSTR("38400bps");
		if (cmdVal == 4)
			return CSTR("57600bps");
		if (cmdVal == 5)
			return CSTR("115200bps");
		return nullptr;
	case DC_GET_CODE_MARK:
	case DC_SET_CODE_MARK:
		if (cmdVal == 0)
			return CSTR("Not Allow");
		if (cmdVal == 1)
			return CSTR("Allow");
		return nullptr;
	case DC_GET_LINE_NUM:
	case DC_SET_LINE_NUM:
		if (cmdVal == 0)
			return CSTR("Not Allow");
		if (cmdVal == 1)
			return CSTR("Transfer in 2 Lines");
		if (cmdVal == 2)
			return CSTR("Transfer in 4 Lines");
		return nullptr;
	case DC_GET_BCC:
	case DC_SET_BCC:
		if (cmdVal == 0)
			return CSTR("Not Allow");
		if (cmdVal == 1)
			return CSTR("Allow");
		return nullptr;
	case DC_GET_CTS_SIGNAL:
	case DC_SET_CTS_SIGNAL:
		if (cmdVal == 0)
			return CSTR("No Control");
		if (cmdVal == 1)
			return CSTR("Has Control");
		return nullptr;
	case DC_GET_CTS_TIME:
	case DC_SET_CTS_TIME:
		return nullptr;
	case DC_GET_ACK_NAK_TIME:
	case DC_SET_ACK_NAK_TIME:
		return nullptr;
	case DC_GET_RECV_HDR:
	case DC_SET_RECV_HDR:
		if (cmdVal == 0)
			return CSTR("No");
		if (cmdVal == 1)
			return CSTR("STX");
		return nullptr;
	case DC_GET_RECV_TERMINATOR:
		if (cmdVal == 0)
			return CSTR("ETX");
		if (cmdVal == 1)
			return CSTR("CR");
		if (cmdVal == 2)
			return CSTR("LF");
		if (cmdVal == 3)
			return CSTR("CR+LF");
		return nullptr;
	case DC_GET_SEND_TERMINATOR:
		if (cmdVal == 0)
			return CSTR("No");
		if (cmdVal == 1)
			return CSTR("ETX");
		if (cmdVal == 2)
			return CSTR("CR");
		if (cmdVal == 3)
			return CSTR("LF");
		if (cmdVal == 4)
			return CSTR("CR+LF");
		if (cmdVal == 5)
			return CSTR("Custom");
		return nullptr;
	case DC_GET_BUZZER:
	case DC_SET_BUZZER:
		if (cmdVal == 0)
			return CSTR("Allow");
		if (cmdVal == 1)
			return CSTR("Not Allow");
		return nullptr;
	case DC_GET_READ_ERR_BUZZER:
	case DC_SET_READ_ERR_BUZZER:
		if (cmdVal == 0)
			return CSTR("Allow");
		if (cmdVal == 1)
			return CSTR("Not Allow");
		return nullptr;
	case DC_GET_MAGIC_KEY:
	case DC_SET_MAGIC_KEY:
		if (cmdVal == 0)
			return CSTR("No Function");
		if (cmdVal == 1)
			return CSTR("Marker Switch Mode");
		return nullptr;
	case DC_GET_POWER_ON_BUZZER:
	case DC_SET_POWER_ON_BUZZER:
		if (cmdVal == 0)
			return CSTR("Allow");
		if (cmdVal == 1)
			return CSTR("Not Allow");
		return nullptr;
	case DC_GET_BUZZER_OFF:
	case DC_SET_BUZZER_OFF:
		if (cmdVal == 0)
			return CSTR("Not Allow");
		if (cmdVal == 1)
			return CSTR("Allow");
		return nullptr;
	default:
		return nullptr;
	}
}

Int32 IO::Device::DensoWaveQB30::GetCommand(DeviceCommand dcmd)
{
	switch (dcmd)
	{
	case DC_GET_READ_MODE:
		return this->ReadCommand(UTF8STRC("RD01\r"));
	case DC_GET_BW_MODE:
		return this->ReadCommand(UTF8STRC("RD02\r"));
	case DC_GET_READ_REPEAT_TIME:
		return this->ReadCommand(UTF8STRC("RD04\r"));
	case DC_GET_BRIGHTNESS:
		return this->ReadCommand(UTF8STRC("RD07\r"));
	case DC_GET_SHT_SIGNAL:
		return this->ReadCommand(UTF8STRC("RD09\r"));
	case DC_GET_SCAN_MODE:
		return this->ReadCommand(UTF8STRC("RD0B\r"));
	case DC_GET_SHUTTER_TIME:
		return this->ReadCommand(UTF8STRC("RD0G\r"));
	case DC_GET_GAIN:
		return this->ReadCommand(UTF8STRC("RD0K\r"));
	case DC_GET_LED_LEVEL:
		return this->ReadCommand(UTF8STRC("RD0L\r"));
	case DC_GET_LED_MODE:
		return this->ReadCommand(UTF8STRC("RD0M\r"));
	case DC_GET_OUTPUT_TIMING:
		return this->ReadCommand(UTF8STRC("RD11\r"));
	case DC_GET_UNREAD_DATA_SEND:
		return this->ReadCommand(UTF8STRC("RD12\r"));
	case DC_GET_INDIR_TIME:
		return this->ReadCommand(UTF8STRC("RD15\r"));
	case DC_GET_TRIGGER_DELAY:
		return this->ReadCommand(UTF8STRC("RD16\r"));
	case DC_GET_SIGNAL_ON_DUR:
		return this->ReadCommand(UTF8STRC("RD17\r"));
	case DC_GET_SIGNAL_DELAY:
		return this->ReadCommand(UTF8STRC("RD18\r"));
	case DC_GET_LIGHT_LED:
		return this->ReadCommand(UTF8STRC("RD1C\r"));
	case DC_GET_MARKER_LIGHT:
		return this->ReadCommand(UTF8STRC("RD1J\r"));
	case DC_GET_DECODE_TIME_LIMIT:
		return this->ReadCommand(UTF8STRC("RD1D\r"));
	case DC_GET_OUTPUT1_TYPE:
		return this->ReadCommand(UTF8STRC("RD1E\r"));
	case DC_GET_OUTPUT2_TYPE:
		return this->ReadCommand(UTF8STRC("RD1G\r"));
	case DC_GET_AUTO_SENSE_MODE:
		return this->ReadCommand(UTF8STRC("RD1H\r"));
	case DC_GET_CONT_READ_MODE_B:
		return this->ReadCommand(UTF8STRC("RD1I\r"));
	case DC_GET_QRCODE:
		return this->ReadCommand(UTF8STRC("RD41\r"));
	case DC_GET_MICRO_QRCODE:
		return this->ReadCommand(UTF8STRC("RD42\r"));
	case DC_GET_PDF417:
		return this->ReadCommand(UTF8STRC("RD43\r"));
	case DC_GET_DATAMATRIX:
		return this->ReadCommand(UTF8STRC("RD44\r"));
	case DC_GET_BARCODE:
		return this->ReadCommand(UTF8STRC("RD45\r"));
	case DC_GET_INTERLEAVED_2OF5:
		return this->ReadCommand(UTF8STRC("RD46\r"));
	case DC_GET_CODABAR:
		return this->ReadCommand(UTF8STRC("RD47\r"));
	case DC_GET_CODABAR_START_STOP:
		return this->ReadCommand(UTF8STRC("RD48\r"));
	case DC_GET_CODE39:
		return this->ReadCommand(UTF8STRC("RD49\r"));
	case DC_GET_CODE128:
		return this->ReadCommand(UTF8STRC("RD4A\r"));
	case DC_GET_QRCODE_REVERSE:
		return this->ReadCommand(UTF8STRC("RD4B\r"));
	case DC_GET_QRLINK_CODE:
		return this->ReadCommand(UTF8STRC("RD4F\r"));
	case DC_GET_GS1_DATABAR:
		return this->ReadCommand(UTF8STRC("RD4I\r"));
	case DC_GET_GS1_COMPOSITE:
		return this->ReadCommand(UTF8STRC("RD4J\r"));
	case DC_GET_MICRO_PDF417:
		return this->ReadCommand(UTF8STRC("RD4K\r"));
	case DC_GET_BARCODE_READ_MODE:
		return this->ReadCommand(UTF8STRC("RD4L\r"));
	case DC_GET_SQRC:
		return this->ReadCommand(UTF8STRC("RD4M\r"));
	case DC_GET_SQRC_KEY_UNMATCH:
		return this->ReadCommand(UTF8STRC("RD4P\r"));
	case DC_GET_SQRC_KEY_MATCH:
		return this->ReadCommand(UTF8STRC("RD4O\r"));
	case DC_GET_IQRCODE_SQUARE:
		return this->ReadCommand(UTF8STRC("RD4Q\r"));
	case DC_GET_IQRCODE_RECT:
		return this->ReadCommand(UTF8STRC("RD4R\r"));
	case DC_GET_AZTEC_FULL:
		return this->ReadCommand(UTF8STRC("RD4V\r"));
	case DC_GET_AZTEC_COMPACT:
		return this->ReadCommand(UTF8STRC("RD4W\r"));
	case DC_GET_MENU_READ:
		return this->ReadCommand(UTF8STRC("RD4Z\r"));
	case DC_GET_COMM_SEQ:
		return this->ReadCommand(UTF8STRC("RD21\r"));
	case DC_GET_BAUD_RATE:
		return this->ReadCommand(UTF8STRC("RD22\r"));
	case DC_GET_CODE_MARK:
		return this->ReadCommand(UTF8STRC("RD25\r"));
	case DC_GET_LINE_NUM:
		return this->ReadCommand(UTF8STRC("RD26\r"));
	case DC_GET_BCC:
		return this->ReadCommand(UTF8STRC("RD27\r"));
	case DC_GET_CTS_SIGNAL:
		return this->ReadCommand(UTF8STRC("RD28\r"));
	case DC_GET_CTS_TIME:
		return this->ReadCommand(UTF8STRC("RD29\r"));
	case DC_GET_ACK_NAK_TIME:
		return this->ReadCommand(UTF8STRC("RD2A\r"));
	case DC_GET_RECV_HDR:
		return this->ReadCommand(UTF8STRC("RD2C\r"));
	case DC_GET_RECV_TERMINATOR:
		return this->ReadCommand(UTF8STRC("RD2D\r"));
	case DC_GET_SEND_TERMINATOR:
		return this->ReadCommand(UTF8STRC("RD2F\r"));
	case DC_GET_BUZZER:
		return this->ReadCommand(UTF8STRC("RD33\r"));
	case DC_GET_READ_ERR_BUZZER:
		return this->ReadCommand(UTF8STRC("RD35\r"));
	case DC_GET_MAGIC_KEY:
		return this->ReadCommand(UTF8STRC("RD37\r"));
	case DC_GET_POWER_ON_BUZZER:
		return this->ReadCommand(UTF8STRC("RD38\r"));
	case DC_GET_BUZZER_OFF:
		return this->ReadCommand(UTF8STRC("RD39\r"));
	case DC_SET_READ_MODE:
	case DC_SET_BW_MODE:
	case DC_SET_READ_REPEAT_TIME:
	case DC_SET_BRIGHTNESS:
	case DC_SET_SHT_SIGNAL:
	case DC_SET_SCAN_MODE:
	case DC_SET_SHUTTER_TIME:
	case DC_SET_GAIN:
	case DC_SET_LED_LEVEL:
	case DC_SET_LED_MODE:
	case DC_SET_OUTPUT_TIMING:
	case DC_SET_UNREAD_DATA_SEND:
	case DC_SET_INDIR_TIME:
	case DC_SET_TRIGGER_DELAY:
	case DC_SET_SIGNAL_ON_DUR:
	case DC_SET_SIGNAL_DELAY:
	case DC_SET_LIGHT_LED:
	case DC_SET_MARKER_LIGHT:
	case DC_SET_DECODE_TIME_LIMIT:
	case DC_SET_OUTPUT1_TYPE:
	case DC_SET_OUTPUT2_TYPE:
	case DC_SET_AUTO_SENSE_MODE:
	case DC_SET_CONT_READ_MODE_B:
	case DC_SET_QRCODE:
	case DC_SET_MICRO_QRCODE:
	case DC_SET_PDF417:
	case DC_SET_DATAMATRIX:
	case DC_SET_BARCODE:
	case DC_SET_INTERLEAVED_2OF5:
	case DC_SET_CODABAR:
	case DC_SET_CODEBAR_START_STOP:
	case DC_SET_CODE39:
	case DC_SET_CODE128:
	case DC_SET_QRCODE_REVERSE:
	case DC_SET_QRLINK_CODE:
	case DC_SET_GS1_DATABAR:
	case DC_SET_GS1_COMPOSITE:
	case DC_SET_MICRO_PDF417:
	case DC_SET_BARCODE_READ_MODE:
	case DC_SET_SQRC:
	case DC_SET_SQRC_KEY_UNMATCH:
	case DC_SET_SQRC_KEY_MATCH:
	case DC_SET_IQRCODE_SQUARE:
	case DC_SET_IQRCODE_RECT:
	case DC_SET_AZTEC_FULL:
	case DC_SET_AZTEC_COMPACT:
	case DC_SET_MENU_READ:
	case DC_SET_COMM_SEQ:
	case DC_SET_BAUD_RATE:
	case DC_SET_CODE_MARK:
	case DC_SET_LINE_NUM:
	case DC_SET_BCC:
	case DC_SET_CTS_SIGNAL:
	case DC_SET_CTS_TIME:
	case DC_SET_ACK_NAK_TIME:
	case DC_SET_RECV_HDR:
	case DC_SET_BUZZER:
	case DC_SET_READ_ERR_BUZZER:
	case DC_SET_MAGIC_KEY:
	case DC_SET_POWER_ON_BUZZER:
	case DC_SET_BUZZER_OFF:
	default:
		return -1;
	}
	return -1;
}

Bool IO::Device::DensoWaveQB30::SetCommand(DeviceCommand dcmd, Int32 val)
{
	UTF8Char cbuff[12];
	UnsafeArray<UTF8Char> csptr;
	switch (dcmd)
	{
	case DC_SET_READ_MODE:
		csptr = Text::StrConcat(Text::StrInt32(Text::StrConcatC(cbuff, UTF8STRC("WR01#")), val), U8STR("\r"));
		return this->WriteCommand(cbuff, (UOSInt)(csptr - cbuff));
	case DC_SET_BW_MODE:
		csptr = Text::StrConcat(Text::StrInt32(Text::StrConcatC(cbuff, UTF8STRC("WR02#")), val), U8STR("\r"));
		return this->WriteCommand(cbuff, (UOSInt)(csptr - cbuff));
	case DC_SET_READ_REPEAT_TIME:
		if (val < 10)
			csptr = Text::StrConcat(Text::StrInt32(Text::StrConcatC(cbuff, UTF8STRC("WR04#0")), val), U8STR("\r"));
		else
			csptr = Text::StrConcat(Text::StrInt32(Text::StrConcatC(cbuff, UTF8STRC("WR04#")), val), U8STR("\r"));
		return this->WriteCommand(cbuff, (UOSInt)(csptr - cbuff));
	case DC_SET_BRIGHTNESS:
		csptr = Text::StrConcat(Text::StrInt32(Text::StrConcatC(cbuff, UTF8STRC("WR07#")), val), U8STR("\r"));
		return this->WriteCommand(cbuff, (UOSInt)(csptr - cbuff));
	case DC_SET_SHT_SIGNAL:
		csptr = Text::StrConcat(Text::StrInt32(Text::StrConcatC(cbuff, UTF8STRC("WR09#")), val), U8STR("\r"));
		return this->WriteCommand(cbuff, (UOSInt)(csptr - cbuff));
	case DC_SET_SCAN_MODE:
		csptr = Text::StrConcat(Text::StrInt32(Text::StrConcatC(cbuff, UTF8STRC("WR0B#")), val), U8STR("\r"));
		return this->WriteCommand(cbuff, (UOSInt)(csptr - cbuff));
	case DC_SET_SHUTTER_TIME:
		if (val < 10)
			csptr = Text::StrConcat(Text::StrInt32(Text::StrConcatC(cbuff, UTF8STRC("WR0G#0")), val), U8STR("\r"));
		else
			csptr = Text::StrConcat(Text::StrInt32(Text::StrConcatC(cbuff, UTF8STRC("WR0G#")), val), U8STR("\r"));
		return this->WriteCommand(cbuff, (UOSInt)(csptr - cbuff));
	case DC_SET_GAIN:
		if (val < 10)
			csptr = Text::StrConcat(Text::StrInt32(Text::StrConcatC(cbuff, UTF8STRC("WR0K#0")), val), U8STR("\r"));
		else
			csptr = Text::StrConcat(Text::StrInt32(Text::StrConcatC(cbuff, UTF8STRC("WR0K#")), val), U8STR("\r"));
		return this->WriteCommand(cbuff, (UOSInt)(csptr - cbuff));
	case DC_SET_LED_LEVEL:
		csptr = Text::StrConcat(Text::StrInt32(Text::StrConcatC(cbuff, UTF8STRC("WR0L#")), val), U8STR("\r"));
		return this->WriteCommand(cbuff, (UOSInt)(csptr - cbuff));
	case DC_SET_LED_MODE:
		csptr = Text::StrConcat(Text::StrInt32(Text::StrConcatC(cbuff, UTF8STRC("WR0M#")), val), U8STR("\r"));
		return this->WriteCommand(cbuff, (UOSInt)(csptr - cbuff));
	case DC_SET_OUTPUT_TIMING:
		csptr = Text::StrConcat(Text::StrInt32(Text::StrConcatC(cbuff, UTF8STRC("WR11#")), val), U8STR("\r"));
		return this->WriteCommand(cbuff, (UOSInt)(csptr - cbuff));
	case DC_SET_UNREAD_DATA_SEND:
		csptr = Text::StrConcat(Text::StrInt32(Text::StrConcatC(cbuff, UTF8STRC("WR12#")), val), U8STR("\r"));
		return this->WriteCommand(cbuff, (UOSInt)(csptr - cbuff));
	case DC_SET_INDIR_TIME:
		if (val < 10)
			csptr = Text::StrConcat(Text::StrInt32(Text::StrConcatC(cbuff, UTF8STRC("WR15#00")), val), U8STR("\r"));
		else if (val < 100)
			csptr = Text::StrConcat(Text::StrInt32(Text::StrConcatC(cbuff, UTF8STRC("WR15#0")), val), U8STR("\r"));
		else
			csptr = Text::StrConcat(Text::StrInt32(Text::StrConcatC(cbuff, UTF8STRC("WR15#")), val), U8STR("\r"));
		return this->WriteCommand(cbuff, (UOSInt)(csptr - cbuff));
	case DC_SET_TRIGGER_DELAY:
		if (val < 10)
			csptr = Text::StrConcat(Text::StrInt32(Text::StrConcatC(cbuff, UTF8STRC("WR16#00")), val), U8STR("\r"));
		else if (val < 100)
			csptr = Text::StrConcat(Text::StrInt32(Text::StrConcatC(cbuff, UTF8STRC("WR16#0")), val), U8STR("\r"));
		else
			csptr = Text::StrConcat(Text::StrInt32(Text::StrConcatC(cbuff, UTF8STRC("WR16#")), val), U8STR("\r"));
		return this->WriteCommand(cbuff, (UOSInt)(csptr - cbuff));
	case DC_SET_SIGNAL_ON_DUR:
		if (val < 10)
			csptr = Text::StrConcat(Text::StrInt32(Text::StrConcatC(cbuff, UTF8STRC("WR17#00")), val), U8STR("\r"));
		else if (val < 100)
			csptr = Text::StrConcat(Text::StrInt32(Text::StrConcatC(cbuff, UTF8STRC("WR17#0")), val), U8STR("\r"));
		else
			csptr = Text::StrConcat(Text::StrInt32(Text::StrConcatC(cbuff, UTF8STRC("WR17#")), val), U8STR("\r"));
		return this->WriteCommand(cbuff, (UOSInt)(csptr - cbuff));
	case DC_SET_SIGNAL_DELAY:
		if (val < 10)
			csptr = Text::StrConcat(Text::StrInt32(Text::StrConcatC(cbuff, UTF8STRC("WR18#0")), val), U8STR("\r"));
		else
			csptr = Text::StrConcat(Text::StrInt32(Text::StrConcatC(cbuff, UTF8STRC("WR18#")), val), U8STR("\r"));
		return this->WriteCommand(cbuff, (UOSInt)(csptr - cbuff));
	case DC_SET_LIGHT_LED:
		csptr = Text::StrConcat(Text::StrInt32(Text::StrConcatC(cbuff, UTF8STRC("WR1C#")), val), U8STR("\r"));
		return this->WriteCommand(cbuff, (UOSInt)(csptr - cbuff));
	case DC_SET_MARKER_LIGHT:
		csptr = Text::StrConcat(Text::StrInt32(Text::StrConcatC(cbuff, UTF8STRC("WR1J#")), val), U8STR("\r"));
		return this->WriteCommand(cbuff, (UOSInt)(csptr - cbuff));
	case DC_SET_DECODE_TIME_LIMIT:
		if (val < 10)
			csptr = Text::StrConcat(Text::StrInt32(Text::StrConcatC(cbuff, UTF8STRC("WR1D#00")), val), U8STR("\r"));
		else if (val < 100)
			csptr = Text::StrConcat(Text::StrInt32(Text::StrConcatC(cbuff, UTF8STRC("WR1D#0")), val), U8STR("\r"));
		else
			csptr = Text::StrConcat(Text::StrInt32(Text::StrConcatC(cbuff, UTF8STRC("WR1D#")), val), U8STR("\r"));
		return this->WriteCommand(cbuff, (UOSInt)(csptr - cbuff));
	case DC_SET_OUTPUT1_TYPE:
		csptr = Text::StrConcat(Text::StrInt32(Text::StrConcatC(cbuff, UTF8STRC("WR1E#")), val), U8STR("\r"));
		return this->WriteCommand(cbuff, (UOSInt)(csptr - cbuff));
	case DC_SET_OUTPUT2_TYPE:
		csptr = Text::StrConcat(Text::StrInt32(Text::StrConcatC(cbuff, UTF8STRC("WR1G#")), val), U8STR("\r"));
		return this->WriteCommand(cbuff, (UOSInt)(csptr - cbuff));
	case DC_SET_AUTO_SENSE_MODE:
		csptr = Text::StrConcat(Text::StrInt32(Text::StrConcatC(cbuff, UTF8STRC("WR1H#")), val), U8STR("\r"));
		return this->WriteCommand(cbuff, (UOSInt)(csptr - cbuff));
	case DC_SET_CONT_READ_MODE_B:
		csptr = Text::StrConcat(Text::StrInt32(Text::StrConcatC(cbuff, UTF8STRC("WR1I#")), val), U8STR("\r"));
		return this->WriteCommand(cbuff, (UOSInt)(csptr - cbuff));
	case DC_SET_QRCODE:
		csptr = Text::StrConcat(Text::StrInt32(Text::StrConcatC(cbuff, UTF8STRC("WR41#")), val), U8STR("\r"));
		return this->WriteCommand(cbuff, (UOSInt)(csptr - cbuff));
	case DC_SET_MICRO_QRCODE:
		csptr = Text::StrConcat(Text::StrInt32(Text::StrConcatC(cbuff, UTF8STRC("WR42#")), val), U8STR("\r"));
		return this->WriteCommand(cbuff, (UOSInt)(csptr - cbuff));
	case DC_SET_PDF417:
		csptr = Text::StrConcat(Text::StrInt32(Text::StrConcatC(cbuff, UTF8STRC("WR43#")), val), U8STR("\r"));
		return this->WriteCommand(cbuff, (UOSInt)(csptr - cbuff));
	case DC_SET_DATAMATRIX:
		csptr = Text::StrConcat(Text::StrInt32(Text::StrConcatC(cbuff, UTF8STRC("WR44#")), val), U8STR("\r"));
		return this->WriteCommand(cbuff, (UOSInt)(csptr - cbuff));
	case DC_SET_BARCODE:
		csptr = Text::StrConcat(Text::StrInt32(Text::StrConcatC(cbuff, UTF8STRC("WR45#")), val), U8STR("\r"));
		return this->WriteCommand(cbuff, (UOSInt)(csptr - cbuff));
	case DC_SET_INTERLEAVED_2OF5:
		csptr = Text::StrConcat(Text::StrInt32(Text::StrConcatC(cbuff, UTF8STRC("WR46#")), val), U8STR("\r"));
		return this->WriteCommand(cbuff, (UOSInt)(csptr - cbuff));
	case DC_SET_CODABAR:
		csptr = Text::StrConcat(Text::StrInt32(Text::StrConcatC(cbuff, UTF8STRC("WR47#")), val), U8STR("\r"));
		return this->WriteCommand(cbuff, (UOSInt)(csptr - cbuff));
	case DC_SET_CODEBAR_START_STOP:
		csptr = Text::StrConcat(Text::StrInt32(Text::StrConcatC(cbuff, UTF8STRC("WR48#")), val), U8STR("\r"));
		return this->WriteCommand(cbuff, (UOSInt)(csptr - cbuff));
	case DC_SET_CODE39:
		csptr = Text::StrConcat(Text::StrInt32(Text::StrConcatC(cbuff, UTF8STRC("WR49#")), val), U8STR("\r"));
		return this->WriteCommand(cbuff, (UOSInt)(csptr - cbuff));
	case DC_SET_CODE128:
		csptr = Text::StrConcat(Text::StrInt32(Text::StrConcatC(cbuff, UTF8STRC("WR4A#")), val), U8STR("\r"));
		return this->WriteCommand(cbuff, (UOSInt)(csptr - cbuff));
	case DC_SET_QRCODE_REVERSE:
		csptr = Text::StrConcat(Text::StrInt32(Text::StrConcatC(cbuff, UTF8STRC("WR4B#")), val), U8STR("\r"));
		return this->WriteCommand(cbuff, (UOSInt)(csptr - cbuff));
	case DC_SET_QRLINK_CODE:
		csptr = Text::StrConcat(Text::StrInt32(Text::StrConcatC(cbuff, UTF8STRC("WR4F#")), val), U8STR("\r"));
		return this->WriteCommand(cbuff, (UOSInt)(csptr - cbuff));
	case DC_SET_GS1_DATABAR:
		csptr = Text::StrConcat(Text::StrInt32(Text::StrConcatC(cbuff, UTF8STRC("WR4I#")), val), U8STR("\r"));
		return this->WriteCommand(cbuff, (UOSInt)(csptr - cbuff));
	case DC_SET_GS1_COMPOSITE:
		csptr = Text::StrConcat(Text::StrInt32(Text::StrConcatC(cbuff, UTF8STRC("WR4J#")), val), U8STR("\r"));
		return this->WriteCommand(cbuff, (UOSInt)(csptr - cbuff));
	case DC_SET_MICRO_PDF417:
		csptr = Text::StrConcat(Text::StrInt32(Text::StrConcatC(cbuff, UTF8STRC("WR4K#")), val), U8STR("\r"));
		return this->WriteCommand(cbuff, (UOSInt)(csptr - cbuff));
	case DC_SET_BARCODE_READ_MODE:
		csptr = Text::StrConcat(Text::StrInt32(Text::StrConcatC(cbuff, UTF8STRC("WR4L#")), val), U8STR("\r"));
		return this->WriteCommand(cbuff, (UOSInt)(csptr - cbuff));
	case DC_SET_SQRC:
		csptr = Text::StrConcat(Text::StrInt32(Text::StrConcatC(cbuff, UTF8STRC("WR4M#")), val), U8STR("\r"));
		return this->WriteCommand(cbuff, (UOSInt)(csptr - cbuff));
	case DC_SET_SQRC_KEY_UNMATCH:
		csptr = Text::StrConcat(Text::StrInt32(Text::StrConcatC(cbuff, UTF8STRC("WR4P#")), val), U8STR("\r"));
		return this->WriteCommand(cbuff, (UOSInt)(csptr - cbuff));
	case DC_SET_SQRC_KEY_MATCH:
		csptr = Text::StrConcat(Text::StrInt32(Text::StrConcatC(cbuff, UTF8STRC("WR4O#")), val), U8STR("\r"));
		return this->WriteCommand(cbuff, (UOSInt)(csptr - cbuff));
	case DC_SET_IQRCODE_SQUARE:
		csptr = Text::StrConcat(Text::StrInt32(Text::StrConcatC(cbuff, UTF8STRC("WR4Q#")), val), U8STR("\r"));
		return this->WriteCommand(cbuff, (UOSInt)(csptr - cbuff));
	case DC_SET_IQRCODE_RECT:
		csptr = Text::StrConcat(Text::StrInt32(Text::StrConcatC(cbuff, UTF8STRC("WR4R#")), val), U8STR("\r"));
		return this->WriteCommand(cbuff, (UOSInt)(csptr - cbuff));
	case DC_SET_AZTEC_FULL:
		csptr = Text::StrConcat(Text::StrInt32(Text::StrConcatC(cbuff, UTF8STRC("WR4V#")), val), U8STR("\r"));
		return this->WriteCommand(cbuff, (UOSInt)(csptr - cbuff));
	case DC_SET_AZTEC_COMPACT:
		csptr = Text::StrConcat(Text::StrInt32(Text::StrConcatC(cbuff, UTF8STRC("WR4W#")), val), U8STR("\r"));
		return this->WriteCommand(cbuff, (UOSInt)(csptr - cbuff));
	case DC_SET_MENU_READ:
		csptr = Text::StrConcat(Text::StrInt32(Text::StrConcatC(cbuff, UTF8STRC("WR4Z#")), val), U8STR("\r"));
		return this->WriteCommand(cbuff, (UOSInt)(csptr - cbuff));
	case DC_SET_COMM_SEQ:
		csptr = Text::StrConcat(Text::StrInt32(Text::StrConcatC(cbuff, UTF8STRC("WR21#")), val), U8STR("\r"));
		return this->WriteCommand(cbuff, (UOSInt)(csptr - cbuff));
	case DC_SET_BAUD_RATE:
		csptr = Text::StrConcat(Text::StrInt32(Text::StrConcatC(cbuff, UTF8STRC("WR22#")), val), U8STR("\r"));
		return this->WriteCommand(cbuff, (UOSInt)(csptr - cbuff));
	case DC_SET_CODE_MARK:
		csptr = Text::StrConcat(Text::StrInt32(Text::StrConcatC(cbuff, UTF8STRC("WR25#")), val), U8STR("\r"));
		return this->WriteCommand(cbuff, (UOSInt)(csptr - cbuff));
	case DC_SET_LINE_NUM:
		csptr = Text::StrConcat(Text::StrInt32(Text::StrConcatC(cbuff, UTF8STRC("WR26#")), val), U8STR("\r"));
		return this->WriteCommand(cbuff, (UOSInt)(csptr - cbuff));
	case DC_SET_BCC:
		csptr = Text::StrConcat(Text::StrInt32(Text::StrConcatC(cbuff, UTF8STRC("WR27#")), val), U8STR("\r"));
		return this->WriteCommand(cbuff, (UOSInt)(csptr - cbuff));
	case DC_SET_CTS_SIGNAL:
		csptr = Text::StrConcat(Text::StrInt32(Text::StrConcatC(cbuff, UTF8STRC("WR28#")), val), U8STR("\r"));
		return this->WriteCommand(cbuff, (UOSInt)(csptr - cbuff));
	case DC_SET_CTS_TIME:
		if (val < 10)
			csptr = Text::StrConcat(Text::StrInt32(Text::StrConcatC(cbuff, UTF8STRC("WR29#0")), val), U8STR("\r"));
		else
			csptr = Text::StrConcat(Text::StrInt32(Text::StrConcatC(cbuff, UTF8STRC("WR29#")), val), U8STR("\r"));
		return this->WriteCommand(cbuff, (UOSInt)(csptr - cbuff));
	case DC_SET_ACK_NAK_TIME:
		if (val < 10)
			csptr = Text::StrConcat(Text::StrInt32(Text::StrConcatC(cbuff, UTF8STRC("WR2A#0")), val), U8STR("\r"));
		else
			csptr = Text::StrConcat(Text::StrInt32(Text::StrConcatC(cbuff, UTF8STRC("WR2A#")), val), U8STR("\r"));
		return this->WriteCommand(cbuff, (UOSInt)(csptr - cbuff));
	case DC_SET_RECV_HDR:
		csptr = Text::StrConcat(Text::StrInt32(Text::StrConcatC(cbuff, UTF8STRC("WR2C#")), val), U8STR("\r"));
		return this->WriteCommand(cbuff, (UOSInt)(csptr - cbuff));
	case DC_SET_BUZZER:
		csptr = Text::StrConcat(Text::StrInt32(Text::StrConcatC(cbuff, UTF8STRC("WR33#")), val), U8STR("\r"));
		return this->WriteCommand(cbuff, (UOSInt)(csptr - cbuff));
	case DC_SET_READ_ERR_BUZZER:
		csptr = Text::StrConcat(Text::StrInt32(Text::StrConcatC(cbuff, UTF8STRC("WR35#")), val), U8STR("\r"));
		return this->WriteCommand(cbuff, (UOSInt)(csptr - cbuff));
	case DC_SET_MAGIC_KEY:
		csptr = Text::StrConcat(Text::StrInt32(Text::StrConcatC(cbuff, UTF8STRC("WR37#")), val), U8STR("\r"));
		return this->WriteCommand(cbuff, (UOSInt)(csptr - cbuff));
	case DC_SET_POWER_ON_BUZZER:
		csptr = Text::StrConcat(Text::StrInt32(Text::StrConcatC(cbuff, UTF8STRC("WR38#")), val), U8STR("\r"));
		return this->WriteCommand(cbuff, (UOSInt)(csptr - cbuff));
	case DC_SET_BUZZER_OFF:
		csptr = Text::StrConcat(Text::StrInt32(Text::StrConcatC(cbuff, UTF8STRC("WR39#")), val), U8STR("\r"));
		return this->WriteCommand(cbuff, (UOSInt)(csptr - cbuff));
	case DC_GET_READ_MODE:
	case DC_GET_BW_MODE:
	case DC_GET_READ_REPEAT_TIME:
	case DC_GET_BRIGHTNESS:
	case DC_GET_SHT_SIGNAL:
	case DC_GET_SCAN_MODE:
	case DC_GET_SHUTTER_TIME:
	case DC_GET_GAIN:
	case DC_GET_LED_LEVEL:
	case DC_GET_LED_MODE:
	case DC_GET_OUTPUT_TIMING:
	case DC_GET_UNREAD_DATA_SEND:
	case DC_GET_INDIR_TIME:
	case DC_GET_TRIGGER_DELAY:
	case DC_GET_SIGNAL_ON_DUR:
	case DC_GET_SIGNAL_DELAY:
	case DC_GET_LIGHT_LED:
	case DC_GET_MARKER_LIGHT:
	case DC_GET_DECODE_TIME_LIMIT:
	case DC_GET_OUTPUT1_TYPE:
	case DC_GET_OUTPUT2_TYPE:
	case DC_GET_AUTO_SENSE_MODE:
	case DC_GET_CONT_READ_MODE_B:
	case DC_GET_QRCODE:
	case DC_GET_MICRO_QRCODE:
	case DC_GET_PDF417:
	case DC_GET_DATAMATRIX:
	case DC_GET_BARCODE:
	case DC_GET_INTERLEAVED_2OF5:
	case DC_GET_CODABAR:
	case DC_GET_CODABAR_START_STOP:
	case DC_GET_CODE39:
	case DC_GET_CODE128:
	case DC_GET_QRCODE_REVERSE:
	case DC_GET_QRLINK_CODE:
	case DC_GET_GS1_DATABAR:
	case DC_GET_GS1_COMPOSITE:
	case DC_GET_MICRO_PDF417:
	case DC_GET_BARCODE_READ_MODE:
	case DC_GET_SQRC:
	case DC_GET_SQRC_KEY_UNMATCH:
	case DC_GET_SQRC_KEY_MATCH:
	case DC_GET_IQRCODE_SQUARE:
	case DC_GET_IQRCODE_RECT:
	case DC_GET_AZTEC_FULL:
	case DC_GET_AZTEC_COMPACT:
	case DC_GET_MENU_READ:
	case DC_GET_COMM_SEQ:
	case DC_GET_BAUD_RATE:
	case DC_GET_CODE_MARK:
	case DC_GET_LINE_NUM:
	case DC_GET_BCC:
	case DC_GET_CTS_SIGNAL:
	case DC_GET_CTS_TIME:
	case DC_GET_ACK_NAK_TIME:
	case DC_GET_RECV_HDR:
	case DC_GET_RECV_TERMINATOR:
	case DC_GET_SEND_TERMINATOR:
	case DC_GET_BUZZER:
	case DC_GET_READ_ERR_BUZZER:
	case DC_GET_MAGIC_KEY:
	case DC_GET_POWER_ON_BUZZER:
	case DC_GET_BUZZER_OFF:
	default:
		return false;
	}
}
