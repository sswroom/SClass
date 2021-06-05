#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "IO/TVCtrl/NECTVControl.h"
#include "Net/SocketFactory.h"
#include "Sync/MutexUsage.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#define RECVBUFFSIZE 256

UInt32 __stdcall IO::TVCtrl::NECTVControl::RecvThread(void *userObj)
{
	IO::TVCtrl::NECTVControl *me = (IO::TVCtrl::NECTVControl *)userObj;
	UInt8 buff[256];
	UOSInt recvSize;
	me->recvRunning = true;
	while (!me->recvToStop)
	{
		recvSize = me->stm->Read(buff, 256);
		if (recvSize <= 0)
		{
			Sync::Thread::Sleep(10);
		}
		else
		{
			Sync::MutexUsage mutUsage(me->mut);
			if (me->recvSize >= RECVBUFFSIZE)
			{
			}
			else if (me->recvSize + recvSize > RECVBUFFSIZE)
			{
				MemCopyNO(&me->recvBuff[me->recvSize], buff, RECVBUFFSIZE - me->recvSize);
				me->recvSize = RECVBUFFSIZE;
				me->recvEvt->Set();
			}
			else
			{
				MemCopyNO(&me->recvBuff[me->recvSize], buff, recvSize);
				me->recvSize += recvSize;
				me->recvEvt->Set();
			}
			mutUsage.EndUse();
		}
	}
	me->recvRunning = false;
	return 0;
}

Bool IO::TVCtrl::NECTVControl::SendCommand(const Char *cmd, Char *cmdReply, Int32 cmdTimeout)
{
	Data::DateTime dt;
	UInt8 buff[64];
	UOSInt cmdLen;
	UOSInt i;
	UInt8 bcc;
	dt.SetCurrTimeUTC();
	if (dt.CompareTo(this->nextTime) < 0)
	{
		Int64 timeDiff = this->nextTime->DiffMS(&dt);
		Sync::Thread::Sleep((UInt32)timeDiff);
	}
	
	cmdLen = Text::StrCharCnt(cmd);
	if (cmdLen <= 0 || cmdLen > 53)
		return false;
	buff[0] = 1; //SOH
	buff[1] = '0';
	if (this->monId == 0)
	{
		buff[2] = 0x2a;
	}
	else
	{
		buff[2] = (UInt8)(0x40 + this->monId);
	}
	buff[3] = '0';
	buff[4] = 'A'; //Command
	Text::StrHexByte((Char*)&buff[5], (UInt8)(cmdLen + 2));
	buff[7] = 2; //STX
	Text::StrConcat((Char*)&buff[8], cmd);
	buff[8 + cmdLen] = 3; //ETX

	bcc = 0;
	i = 9 + cmdLen;
	while (i-- > 1)
	{
		bcc = bcc ^ buff[i];
	}
	buff[9 + cmdLen] = bcc;
	buff[10 + cmdLen] = 13;

	Sync::MutexUsage mutUsage(this->mut);
	this->recvSize = 0;
	mutUsage.EndUse();


	this->stm->Write(buff, 11 + cmdLen);
	this->nextTime->SetCurrTimeUTC();
	Bool found = false;
	while (true)
	{
		Int64 timeDiff;
		dt.SetCurrTimeUTC();
		timeDiff = dt.DiffMS(this->nextTime);
		if (timeDiff >= cmdTimeout)
			break;
		if (this->recvSize > 0)
		{
			OSInt i = 0;
			while (i < this->recvSize)
			{
				if (this->recvBuff[i] == 13)
				{
					found = true;
					break;
				}
				i++;
			}
			if (found)
				break;
		}
		this->recvEvt->Wait(100);
	}
	this->nextTime->AddMS(600);
	if (found)
	{
		if (this->recvBuff[0] == 1 && this->recvBuff[7] == 2)
		{
			this->recvBuff[7] = 0;
			cmdLen = Text::StrHex2UInt32((Char*)&this->recvBuff[5]);
			if (this->recvBuff[6 + cmdLen] == 3)
			{
				this->recvBuff[6 + cmdLen] = 0;
				Text::StrConcat(cmdReply, (Char*)&this->recvBuff[8]);
				return true;
			}
		}
	}
	return false;
}

Bool IO::TVCtrl::NECTVControl::GetParameter(UInt8 opCodePage, UInt8 opCode, UInt16 *maxVal, UInt16 *currVal, Int32 cmdTimeout)
{
	Data::DateTime dt;
	UInt8 buff[64];
	OSInt cmdLen;
	OSInt i;
	UInt8 bcc;
	dt.SetCurrTimeUTC();
	if (dt.CompareTo(this->nextTime) < 0)
	{
		Int64 timeDiff = this->nextTime->DiffMS(&dt);
		Sync::Thread::Sleep((UInt32)timeDiff);
	}
	
	buff[0] = 1; //SOH
	buff[1] = '0';
	if (this->monId == 0)
	{
		buff[2] = 0x2a;
	}
	else
	{
		buff[2] = (UInt8)(0x40 + this->monId);
	}
	buff[3] = '0';
	buff[4] = 'C'; //Command
	Text::StrHexByte((Char*)&buff[5], 6);
	buff[7] = 2; //STX
	Text::StrHexByte((Char*)&buff[8], opCodePage);
	Text::StrHexByte((Char*)&buff[10], opCode);
	buff[12] = 3; //ETX

	bcc = 0;
	i = 13;
	while (i-- > 1)
	{
		bcc = bcc ^ buff[i];
	}
	buff[13] = bcc;
	buff[14] = 13;

	Sync::MutexUsage mutUsage(this->mut);
	this->recvSize = 0;
	mutUsage.EndUse();


	this->stm->Write(buff, 15);
	this->nextTime->SetCurrTimeUTC();
	Bool found = false;
	while (true)
	{
		Int64 timeDiff;
		dt.SetCurrTimeUTC();
		timeDiff = dt.DiffMS(this->nextTime);
		if (timeDiff >= cmdTimeout)
			break;
		if (this->recvSize > 0)
		{
			OSInt i = 0;
			while (i < this->recvSize)
			{
				if (this->recvBuff[i] == 13)
				{
					found = true;
					break;
				}
				i++;
			}
			if (found)
				break;
		}
		this->recvEvt->Wait(100);
	}
	this->nextTime->AddMS(600);
	if (found)
	{
		if (this->recvBuff[0] == 1 && this->recvBuff[7] == 2)
		{
			this->recvBuff[7] = 0;
			cmdLen = Text::StrHex2Int32((Char*)&this->recvBuff[5]);
			if (this->recvBuff[6 + cmdLen] == 3)
			{
				this->recvBuff[6 + cmdLen] = 0;
				if (this->recvBuff[8] != '0' || this->recvBuff[9] != '0')
					return false;
				Text::StrHex2Bytes((Char*)&this->recvBuff[16], buff);
				*maxVal = ReadMUInt16(buff);
				*currVal = ReadMUInt16(&buff[2]);
				return true;
			}
		}
	}
	return false;
}

Bool IO::TVCtrl::NECTVControl::SetParameter(UInt8 opCodePage, UInt8 opCode, UInt16 val, Int32 cmdTimeout)
{
	Data::DateTime dt;
	UInt8 buff[64];
	OSInt cmdLen;
	OSInt i;
	UInt8 bcc;
	dt.SetCurrTimeUTC();
	if (dt.CompareTo(this->nextTime) < 0)
	{
		Int64 timeDiff = this->nextTime->DiffMS(&dt);
		Sync::Thread::Sleep((UInt32)timeDiff);
	}
	
	buff[0] = 1; //SOH
	buff[1] = '0';
	if (this->monId == 0)
	{
		buff[2] = 0x2a;
	}
	else
	{
		buff[2] = (UInt8)(0x40 + this->monId);
	}
	buff[3] = '0';
	buff[4] = 'E'; //Command
	Text::StrHexByte((Char*)&buff[5], 10);
	buff[7] = 2; //STX
	Text::StrHexByte((Char*)&buff[8], opCodePage);
	Text::StrHexByte((Char*)&buff[10], opCode);
	Text::StrHexByte((Char*)&buff[12], (UInt8)(val >> 8));
	Text::StrHexByte((Char*)&buff[14], (UInt8)(val & 255));
	buff[16] = 3; //ETX

	bcc = 0;
	i = 17;
	while (i-- > 1)
	{
		bcc = bcc ^ buff[i];
	}
	buff[17] = bcc;
	buff[18] = 13;

	Sync::MutexUsage mutUsage(this->mut);
	this->recvSize = 0;
	mutUsage.EndUse();


	this->stm->Write(buff, 19);
	this->nextTime->SetCurrTimeUTC();
	Bool found = false;
	while (true)
	{
		Int64 timeDiff;
		dt.SetCurrTimeUTC();
		timeDiff = dt.DiffMS(this->nextTime);
		if (timeDiff >= cmdTimeout)
			break;
		if (this->recvSize > 0)
		{
			OSInt i = 0;
			while (i < this->recvSize)
			{
				if (this->recvBuff[i] == 13)
				{
					found = true;
					break;
				}
				i++;
			}
			if (found)
				break;
		}
		this->recvEvt->Wait(100);
	}
	this->nextTime->AddMS(600);
	if (found)
	{
		if (this->recvBuff[0] == 1 && this->recvBuff[7] == 2)
		{
			this->recvBuff[7] = 0;
			cmdLen = Text::StrHex2Int32((Char*)&this->recvBuff[5]);
			if (this->recvBuff[6 + cmdLen] == 3 && cmdLen == 18)
			{
				return true;
			}
		}
	}
	return false;
}

IO::TVCtrl::NECTVControl::NECTVControl(IO::Stream *stm, Int32 monId)
{
	this->stm = stm;
	this->monId = monId;
	NEW_CLASS(this->nextTime, Data::DateTime());
	this->nextTime->SetCurrTimeUTC();

	NEW_CLASS(this->mut, Sync::Mutex());
	NEW_CLASS(this->recvEvt, Sync::Event(true, (const UTF8Char*)"IO.TVControl.recvEvt"));
	this->recvBuff = MemAlloc(UInt8, RECVBUFFSIZE);
	this->recvSize = 0;
	this->recvRunning = false;
	this->recvToStop = false;

	Sync::Thread::Create(RecvThread, this);
}

IO::TVCtrl::NECTVControl::~NECTVControl()
{
	this->recvToStop = true;
	while (this->recvRunning)
	{
		Sync::Thread::Sleep(10);
	}
	DEL_CLASS(this->nextTime);
	DEL_CLASS(this->recvEvt);
	DEL_CLASS(this->mut);
	MemFree(this->recvBuff);
}

Bool IO::TVCtrl::NECTVControl::SendInstruction(CommandType ct)
{
	Char buff[64];
	switch (ct)
	{
	case CT_POWERON:
		if (SendCommand("C203D60001", buff, 2000) && buff[0] == '0' && buff[1] == '0')
			return true;
		return false;
	case CT_POWEROFF:
		if (SendCommand("C203D60004", buff, 2000) && buff[0] == '0' && buff[1] == '0')
			return true;
		return false;
	case CT_SETCURRDATETIME:
		{
			Data::DateTime dt;
			UInt8 byteBuff[9];
			byteBuff[0] = 0xc2;
			byteBuff[1] = 0x12;
			dt.SetCurrTime();
			byteBuff[2] = (UInt8)(dt.GetYear() - 2000);
			byteBuff[3] = dt.GetMonth();
			byteBuff[4] = dt.GetDay();
			byteBuff[5] = (UInt8)dt.GetWeekday();
			byteBuff[6] = dt.GetHour();
			byteBuff[7] = dt.GetMinute();
			byteBuff[8] = 0;
			Text::StrHexBytes(buff, byteBuff, 9, 0);
			if (SendCommand(buff, buff, 2000) && buff[0] == 'C' && buff[1] == '3' && buff[2] == '1' && buff[3] == '2')
				return true;
		}
		return false;
	case CT_INPUT_DSUB:
		return SetParameter(0x00, 0x60, 1, 2000);
	case CT_INPUT_VIDEO:
		return SetParameter(0x00, 0x60, 5, 2000);
	case CT_INPUT_SVIDEO:
		return SetParameter(0x00, 0x60, 7, 2000);
	case CT_INPUT_DVI_D:
		return SetParameter(0x00, 0x60, 3, 2000);
	case CT_INPUT_YPBPR:
		return SetParameter(0x00, 0x60, 12, 2000);
	case CT_INPUT_YPBPR2:
		return SetParameter(0x00, 0x60, 14, 2000);
	case CT_INPUT_HDMI1:
		return SetParameter(0x00, 0x60, 17, 2000);
	case CT_INPUT_HDMI2:
		return SetParameter(0x00, 0x60, 18, 2000);
	case CT_INPUT_DISPLAYPORT:
		return SetParameter(0x00, 0x60, 15, 2000);
	case CT_INPUT_DISPLAYPORT2:
		return SetParameter(0x00, 0x60, 16, 2000);
	case CT_INPUT_DISPLAYPORT3:
		return SetParameter(0x00, 0x60, 128, 2000);
	case CT_IRLOCK:
		return SetParameter(0x02, 0x3F, 4, 2000);
	case CT_IRUNLOCK:
		return SetParameter(0x02, 0x3F, 1, 2000);
	default:
		return false;
	}
}

Bool IO::TVCtrl::NECTVControl::SendGetCommand(CommandType ct, Int32 *val, UTF8Char *sbuff)
{
	Char buff[64];
	UInt16 maxVal;
	UInt16 currVal;
	switch (ct)
	{
	case CT_GETPOWERSTATUS:
		if (!SendCommand("01D6", buff, 2000) || buff[2] != '0' || buff[3] != '0')
			return false;
		if (buff[12] == '0' && buff[13] == '0' && buff[14] == '0' && buff[15] == '1')
		{
			*val = 1;
			Text::StrConcat(sbuff, (const UTF8Char*)"On");
			return true;
		}
		else if (buff[12] == '0' && buff[13] == '0' && buff[14] == '0' && buff[15] == '2')
		{
			*val = 2;
			Text::StrConcat(sbuff, (const UTF8Char*)"Stand-by (power save)");
			return true;
		}
		else if (buff[12] == '0' && buff[13] == '0' && buff[14] == '0' && buff[15] == '3')
		{
			*val = 3;
			Text::StrConcat(sbuff, (const UTF8Char*)"Suspend (power save)");
			return true;
		}
		else if (buff[12] == '0' && buff[13] == '0' && buff[14] == '0' && buff[15] == '4')
		{
			*val = 4;
			Text::StrConcat(sbuff, (const UTF8Char*)"Off");
			return true;
		}
		return false;
	case CT_GETDATETIME:
		if (!SendCommand("C211", buff, 2000) || buff[0] != 'C' || buff[1] != '3' || buff[2] != '1' || buff[3] != '1')
			return false;
		{
			UInt8 byteBuff[32];
			Data::DateTime dt;
			Text::StrHex2Bytes(&buff[4], byteBuff);
			dt.SetValue((UInt16)(2000 + byteBuff[0]), byteBuff[1], byteBuff[2], byteBuff[4], byteBuff[5], 0, 0);
			dt.ToString(sbuff, "yyyy-MM-dd HH:mm");
			*val = (Int32)dt.ToUnixTimestamp();
		}
		return true;
	case CT_SELFDIAGNOSIS:
		if (!SendCommand("B1", buff, 2000) || buff[0] != 'A' || buff[1] != '1')
			return false;
		if (buff[2] == '0' && buff[3] == '0')
		{
			*val = 0;
			Text::StrConcat(sbuff, (const UTF8Char*)"Normal");
			return true;
		}
		else if (buff[2] == '7' && buff[3] == '0')
		{
			*val = 0x70;
			Text::StrConcat(sbuff, (const UTF8Char*)"Standby-power +3.3V abnormality");
			return true;
		}
		else if (buff[2] == '7' && buff[3] == '1')
		{
			*val = 0x71;
			Text::StrConcat(sbuff, (const UTF8Char*)"Standby-power +5V abnormality");
			return true;
		}
		else if (buff[2] == '7' && buff[3] == '2')
		{
			*val = 0x72;
			Text::StrConcat(sbuff, (const UTF8Char*)"Panel-power +12V abnormality");
			return true;
		}
		else if (buff[2] == '7' && buff[3] == '8')
		{
			*val = 0x78;
			Text::StrConcat(sbuff, (const UTF8Char*)"Inverter power/Option slot2 power +24V Abnormality");
			return true;
		}
		else if (buff[2] == '8' && buff[3] == '0')
		{
			*val = 0x80;
			Text::StrConcat(sbuff, (const UTF8Char*)"Cooling fan-1 abnormality");
			return true;
		}
		else if (buff[2] == '8' && buff[3] == '1')
		{
			*val = 0x81;
			Text::StrConcat(sbuff, (const UTF8Char*)"Cooling fan-2 abnormality");
			return true;
		}
		else if (buff[2] == '8' && buff[3] == '2')
		{
			*val = 0x82;
			Text::StrConcat(sbuff, (const UTF8Char*)"Cooling fan-3 abnormality");
			return true;
		}
		else if (buff[2] == '9' && buff[3] == '1')
		{
			*val = 0x91;
			Text::StrConcat(sbuff, (const UTF8Char*)"LED Backlight abnormality");
			return true;
		}
		else if (buff[2] == 'A' && buff[3] == '0')
		{
			*val = 0xa0;
			Text::StrConcat(sbuff, (const UTF8Char*)"Temperature abnormality - shutdown");
			return true;
		}
		else if (buff[2] == 'A' && buff[3] == '1')
		{
			*val = 0xa1;
			Text::StrConcat(sbuff, (const UTF8Char*)"Temperature abnormality - half brightness");
			return true;
		}
		else if (buff[2] == 'A' && buff[3] == '2')
		{
			*val = 0xa2;
			Text::StrConcat(sbuff, (const UTF8Char*)"SENSOR reached at the temperature that the user had specified.");
			return true;
		}
		else if (buff[2] == 'B' && buff[3] == '0')
		{
			*val = 0xb0;
			Text::StrConcat(sbuff, (const UTF8Char*)"No signal");
			return true;
		}
		else if (buff[2] == 'D' && buff[3] == '0')
		{
			*val = 0xd0;
			Text::StrConcat(sbuff, (const UTF8Char*)"PROOF OF PLAY buffer reduction");
			return true;
		}
		else if (buff[2] == 'E' && buff[3] == '0')
		{
			*val = 0xe0;
			Text::StrConcat(sbuff, (const UTF8Char*)"System error");
			return true;
		}
		return false;
	case CT_GETSERIALNO:
		if (!SendCommand("C216", buff, 2000) || buff[0] != 'C' || buff[1] != '3' || buff[2] != '1' || buff[3] != '6')
			return false;
		{
			UInt8 serialNo[16];
			UOSInt i;
			*val = 0;
			i = Text::StrHex2Bytes(&buff[4], serialNo);
			sbuff[i] = 0;
			while (i-- > 0)
			{
				sbuff[i] = serialNo[i];
			}
		}
		return true;
	case CT_GETMODEL:
		if (!SendCommand("C217", buff, 2000) || buff[0] != 'C' || buff[1] != '3' || buff[2] != '1' || buff[3] != '7')
			return false;
		{
			UInt8 model[16];
			UOSInt i;
			*val = 0;
			i = Text::StrHex2Bytes(&buff[4], model);
			sbuff[i] = 0;
			while (i-- > 0)
			{
				sbuff[i] = model[i];
			}
		}
		return true;
	case CT_GETFIRMWARE:
		if (!SendCommand("CA0200", buff, 2000) || buff[0] != 'C' || buff[1] != 'B' || buff[2] != '0' || buff[3] != '2' || buff[4] != '0' || buff[5] != '0')
			return false;
		{
			*val = 0;
			*sbuff++ = (UTF8Char)buff[8];
			*sbuff++ = (UTF8Char)buff[9];
			*sbuff++ = (UTF8Char)buff[10];
			*sbuff++ = (UTF8Char)buff[11];
			*sbuff++ = (UTF8Char)buff[12];
			*sbuff++ = (UTF8Char)buff[13];
			*sbuff++ = (UTF8Char)buff[14];
			*sbuff = 0;
		}
		return true;
	case CT_GETFIRMWARE2:
		if (!SendCommand("CA0201", buff, 2000) || buff[0] != 'C' || buff[1] != 'B' || buff[2] != '0' || buff[3] != '2' || buff[4] != '0' || buff[5] != '0')
			return false;
		{
			*val = 0;
			*sbuff++ = (UTF8Char)buff[8];
			*sbuff++ = (UTF8Char)buff[9];
			*sbuff++ = (UTF8Char)buff[10];
			*sbuff++ = (UTF8Char)buff[11];
			*sbuff++ = (UTF8Char)buff[12];
			*sbuff++ = (UTF8Char)buff[13];
			*sbuff++ = (UTF8Char)buff[14];
			*sbuff = 0;
		}
		return true;
	case CT_GETPOWERSAVEMODE:
		if (!SendCommand("CA0B00", buff, 2000) || buff[0] != 'C' || buff[1] != 'B' || buff[2] != '0' || buff[3] != 'B' || buff[4] != '0' || buff[5] != '0')
			return false;
		{
			UInt8 byteBuff[16];
			Text::StrHex2Bytes(&buff[6], byteBuff);
			*val = byteBuff[0];
			if (byteBuff[0] == 0)
			{
				Text::StrConcat(sbuff, (const UTF8Char*)"Auto Power Save");
			}
			else if (byteBuff[0] == 1)
			{
				Text::StrConcat(sbuff, (const UTF8Char*)"Auto Standby");
			}
			else if (byteBuff[0] == 2)
			{
				Text::StrConcat(sbuff, (const UTF8Char*)"Power Save Off");
			}
			else
			{
				Text::StrConcat(sbuff, (const UTF8Char*)"Unknown mode");
			}
		}
		return true;
	case CT_GETPOWERSAVETIME:
		if (!SendCommand("CA0B02", buff, 2000) || buff[0] != 'C' || buff[1] != 'B' || buff[2] != '0' || buff[3] != 'B' || buff[4] != '0' || buff[5] != '2')
			return false;
		{
			UInt8 byteBuff[16];
			Text::StrHex2Bytes(&buff[6], byteBuff);
			*val = byteBuff[0] * 5;
			Text::StrConcat(Text::StrInt32(sbuff, byteBuff[0] * 5), (const UTF8Char*)" secs");
		}
		return true;
	case CT_GETAUTOSTANDBYTIME:
		if (!SendCommand("CA0B04", buff, 2000) || buff[0] != 'C' || buff[1] != 'B' || buff[2] != '0' || buff[3] != 'B' || buff[4] != '0' || buff[5] != '4')
			return false;
		{
			UInt8 byteBuff[16];
			Text::StrHex2Bytes(&buff[6], byteBuff);
			*val = byteBuff[0] * 5;
			Text::StrConcat(Text::StrInt32(sbuff, byteBuff[0] * 5), (const UTF8Char*)" secs");
		}
		return true;
	case CT_GETLANMAC:
		if (!SendCommand("C22A02", buff, 2000) || buff[0] != 'C' || buff[1] != '3' || buff[2] != '2' || buff[3] != 'A' || buff[4] != '0' || buff[5] != '0' || buff[6] != '0' || buff[7] != '2')
			return false;
		{
			UInt8 byteBuff[16];
			Text::StrHex2Bytes(&buff[8], byteBuff);
			*val = byteBuff[0];
			Text::StrHexBytes(sbuff, &byteBuff[1], 6, ':');
		}
		return true;
	case CT_GETCURRINPUT:
		if (!GetParameter(0x00, 0x60, &maxVal, &currVal, 2000))
			return false;
		*val = currVal;
		switch (currVal)
		{
		case 1:
			Text::StrConcat(sbuff, (const UTF8Char*)"VGA");
			break;
		case 2:
			Text::StrConcat(sbuff, (const UTF8Char*)"RGB/HV");
			break;
		case 3:
			Text::StrConcat(sbuff, (const UTF8Char*)"DVI");
			break;
		case 4:
			Text::StrConcat(sbuff, (const UTF8Char*)"HDMI");
			break;
		case 5:
			Text::StrConcat(sbuff, (const UTF8Char*)"Video");
			break;
		case 7:
			Text::StrConcat(sbuff, (const UTF8Char*)"S-Video");
			break;
		case 12:
			Text::StrConcat(sbuff, (const UTF8Char*)"Y/Pb/Pr");
			break;
		case 13:
			Text::StrConcat(sbuff, (const UTF8Char*)"OPTION");
			break;
		case 14:
			Text::StrConcat(sbuff, (const UTF8Char*)"Y/Pb/Pr2");
			break;
		case 15:
			Text::StrConcat(sbuff, (const UTF8Char*)"Display Port");
			break;
		case 16:
			Text::StrConcat(sbuff, (const UTF8Char*)"Display Port2");
			break;
		case 17:
			Text::StrConcat(sbuff, (const UTF8Char*)"HDMI");
			break;
		case 18:
			Text::StrConcat(sbuff, (const UTF8Char*)"HDMI2");
			break;
		case 128:
			Text::StrConcat(sbuff, (const UTF8Char*)"Display Port3");
			break;
		default:
			Text::StrConcat(sbuff, (const UTF8Char*)"Unknown");
			break;
		}
		return true;
	case CT_GETTEMPERATURE1:
		if (!SetParameter(0x02, 0x78, 1, 2000))
			return false;
		if (!GetParameter(0x02, 0x79, &maxVal, &currVal, 2000))
			return false;
		*val = (Int16)currVal;
		Text::StrDouble(sbuff, 0.5 * (Int16)currVal);
		return true;
	case CT_GETTEMPERATURE2:
		if (!SetParameter(0x02, 0x78, 2, 2000))
			return false;
		if (!GetParameter(0x02, 0x79, &maxVal, &currVal, 2000))
			return false;
		*val = (Int16)currVal;
		Text::StrDouble(sbuff, 0.5 * (Int16)currVal);
		return true;
	case CT_GETTEMPERATURE3:
		if (!SetParameter(0x02, 0x78, 3, 2000))
			return false;
		if (!GetParameter(0x02, 0x79, &maxVal, &currVal, 2000))
			return false;
		*val = (Int16)currVal;
		Text::StrDouble(sbuff, 0.5 * (Int16)currVal);
		return true;
	case CT_GETBACKLIGHT:
		if (!GetParameter(0x00, 0x10, &maxVal, &currVal, 2000))
			return false;
		*val = currVal;
		Text::StrInt32(sbuff, currVal);
		return true;
	case CT_GETCONTRAST:
		if (!GetParameter(0x00, 0x12, &maxVal, &currVal, 2000))
			return false;
		*val = currVal;
		Text::StrInt32(sbuff, currVal);
		return true;
	case CT_GETSHARPNESS:
		if (!GetParameter(0x00, 0x8c, &maxVal, &currVal, 2000))
			return false;
		*val = currVal;
		Text::StrInt32(sbuff, currVal);
		return true;
	case CT_GETBRIGHTNESS:
		if (!GetParameter(0x00, 0x92, &maxVal, &currVal, 2000))
			return false;
		*val = currVal;
		Text::StrInt32(sbuff, currVal);
		return true;
	case CT_GETHUE:
		if (!GetParameter(0x00, 0x90, &maxVal, &currVal, 2000))
			return false;
		*val = currVal;
		Text::StrInt32(sbuff, currVal);
		return true;
	case CT_GETSATURATION:
		if (!GetParameter(0x02, 0x1f, &maxVal, &currVal, 2000))
			return false;
		*val = currVal;
		Text::StrInt32(sbuff, currVal);
		return true;
	case CT_GETCOLORTEMP:
		if (!GetParameter(0x00, 0x54, &maxVal, &currVal, 2000))
			return false;
		*val = currVal * 100 + 2600;
		Text::StrInt32(sbuff, currVal * 100 + 2600);
		return true;
	case CT_GETIRSTATUS:
		if (!GetParameter(0x02, 0x3F, &maxVal, &currVal, 2000))
			return false;
		*val = (currVal == 4)?1:0;
		if (currVal == 4)
			Text::StrConcat(sbuff, (const UTF8Char*)"Locked");
		else
			Text::StrConcat(sbuff, (const UTF8Char*)"Not locked");
		return true;
	case CT_GETHPOS:
		if (!GetParameter(0x00, 0x20, &maxVal, &currVal, 2000))
			return false;
		*val = currVal;
		Text::StrInt32(sbuff, currVal);
		return true;
	case CT_GETVPOS:
		if (!GetParameter(0x00, 0x30, &maxVal, &currVal, 2000))
			return false;
		*val = currVal;
		Text::StrInt32(sbuff, currVal);
		return true;
	default:
		return false;
	}
}

Bool IO::TVCtrl::NECTVControl::SendSetCommand(CommandType ct, Int32 val)
{
	switch (ct)
	{
	case CT_SETBACKLIGHT:
		if (val < 0 || val > 100)
			return false;
		return SetParameter(0x00, 0x10, (UInt16)val, 2000);
	case CT_SETCONTRAST:
		if (val < 0 || val > 100)
			return false;
		return SetParameter(0x00, 0x12, (UInt16)val, 2000);
	case CT_SETSHARPNESS:
		if (val < 0 || val > 24)
			return false;
		return SetParameter(0x00, 0x8c, (UInt16)val, 2000);
	case CT_SETBRIGNTNESS:
		if (val < 0 || val > 100)
			return false;
		return SetParameter(0x00, 0x92, (UInt16)val, 2000);
	case CT_SETHUE:
		if (val < 0 || val > 100)
			return false;
		return SetParameter(0x00, 0x90, (UInt16)val, 2000);
	case CT_SETSATURATION:
		if (val < 0 || val > 100)
			return false;
		return SetParameter(0x02, 0x1f, (UInt16)val, 2000);
	case CT_SETCOLORTEMP:
		if (val < 2600 || val > 10000)
			return false;
		return SetParameter(0x00, 0x54, (UInt16)((val - 2600) / 100), 2000);
	default:
		return false;
	}
}

#define ADDCMD(a, b) {cmdList->Add(a);cmdFormats->Add(b);}
void IO::TVCtrl::NECTVControl::GetSupportedCmd(Data::ArrayList<CommandType> *cmdList, Data::ArrayList<CommandFormat> *cmdFormats)
{
	ADDCMD(CT_POWERON, CF_INSTRUCTION);
	ADDCMD(CT_POWEROFF, CF_INSTRUCTION);
	ADDCMD(CT_GETPOWERSTATUS, CF_GETCOMMAND);
	ADDCMD(CT_GETCURRINPUT, CF_GETCOMMAND);
	ADDCMD(CT_INPUT_DSUB, CF_INSTRUCTION);
	ADDCMD(CT_INPUT_VIDEO, CF_INSTRUCTION);
	ADDCMD(CT_INPUT_SVIDEO, CF_INSTRUCTION);
	ADDCMD(CT_INPUT_DVI_D, CF_INSTRUCTION);
	ADDCMD(CT_INPUT_YPBPR, CF_INSTRUCTION);
	ADDCMD(CT_INPUT_YPBPR2, CF_INSTRUCTION);
	ADDCMD(CT_INPUT_HDMI1, CF_INSTRUCTION);
	ADDCMD(CT_INPUT_HDMI2, CF_INSTRUCTION);
	ADDCMD(CT_INPUT_DISPLAYPORT, CF_INSTRUCTION);
	ADDCMD(CT_INPUT_DISPLAYPORT2, CF_INSTRUCTION);
	ADDCMD(CT_INPUT_DISPLAYPORT3, CF_INSTRUCTION);
	ADDCMD(CT_GETDATETIME, CF_GETCOMMAND);
	ADDCMD(CT_SETCURRDATETIME, CF_INSTRUCTION);
	ADDCMD(CT_SELFDIAGNOSIS, CF_GETCOMMAND);
	ADDCMD(CT_GETSERIALNO, CF_GETCOMMAND);
	ADDCMD(CT_GETMODEL, CF_GETCOMMAND);
	ADDCMD(CT_GETFIRMWARE, CF_GETCOMMAND);
	ADDCMD(CT_GETFIRMWARE2, CF_GETCOMMAND);
	ADDCMD(CT_GETPOWERSAVEMODE, CF_GETCOMMAND);
	ADDCMD(CT_GETAUTOSTANDBYTIME, CF_GETCOMMAND);
	ADDCMD(CT_GETLANMAC, CF_GETCOMMAND);
	ADDCMD(CT_GETTEMPERATURE1, CF_GETCOMMAND);
	ADDCMD(CT_GETTEMPERATURE2, CF_GETCOMMAND);
	ADDCMD(CT_GETTEMPERATURE3, CF_GETCOMMAND);
	ADDCMD(CT_GETBACKLIGHT, CF_GETCOMMAND);
	ADDCMD(CT_SETBACKLIGHT, CF_SETCOMMAND);
	ADDCMD(CT_GETCONTRAST, CF_GETCOMMAND);
	ADDCMD(CT_SETCONTRAST, CF_SETCOMMAND);
	ADDCMD(CT_GETSHARPNESS, CF_GETCOMMAND);
	ADDCMD(CT_SETSHARPNESS, CF_SETCOMMAND);
	ADDCMD(CT_GETBRIGHTNESS, CF_GETCOMMAND);
	ADDCMD(CT_SETBRIGNTNESS, CF_SETCOMMAND);
	ADDCMD(CT_GETHUE, CF_GETCOMMAND);
	ADDCMD(CT_SETHUE, CF_SETCOMMAND);
	ADDCMD(CT_GETSATURATION, CF_GETCOMMAND);
	ADDCMD(CT_SETSATURATION, CF_SETCOMMAND);
	ADDCMD(CT_GETCOLORTEMP, CF_GETCOMMAND);
	ADDCMD(CT_SETCOLORTEMP, CF_SETCOMMAND);
	ADDCMD(CT_IRLOCK, CF_INSTRUCTION);
	ADDCMD(CT_IRUNLOCK, CF_INSTRUCTION);
	ADDCMD(CT_GETIRSTATUS, CF_GETCOMMAND);
	ADDCMD(CT_GETHPOS, CF_GETCOMMAND);
	ADDCMD(CT_GETVPOS, CF_GETCOMMAND);
}

Bool IO::TVCtrl::NECTVControl::GetInfo(IO::TVControl::TVInfo *info)
{
	info->name = (const UTF8Char*)"NEC LCD Monitor";
	info->tvType = IO::TVControl::TVT_NEC;
	info->defBaudRate = 9600;
	return true;
}
