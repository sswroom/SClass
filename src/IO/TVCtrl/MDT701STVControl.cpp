#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/TVCtrl/MDT701STVControl.h"
#include "Sync/MutexUsage.h"
#include "Sync/Thread.h"
#define RECVBUFFSIZE 256

UInt32 __stdcall IO::TVCtrl::MDT701STVControl::RecvThread(void *userObj)
{
	IO::TVCtrl::MDT701STVControl *me = (IO::TVCtrl::MDT701STVControl *)userObj;
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

Bool IO::TVCtrl::MDT701STVControl::SendBasicCommand(const Char *buff, UOSInt buffSize, const Char *cmdReply, UOSInt replySize, UInt32 cmdTimeout, UInt32 cmdInterval)
{
	Data::DateTime dt;
	dt.SetCurrTimeUTC();
	if (dt.CompareTo(this->nextTime) < 0)
	{
		Int64 timeDiff = this->nextTime->DiffMS(&dt);
		Sync::Thread::Sleep((UInt32)timeDiff);
	}
	
	Sync::MutexUsage mutUsage(this->mut);
	this->recvSize = 0;
	mutUsage.EndUse();

	this->stm->Write((const UInt8*)buff, buffSize);
	this->nextTime->SetCurrTimeUTC();
	while (true)
	{
		Int64 timeDiff;
		dt.SetCurrTimeUTC();
		timeDiff = dt.DiffMS(this->nextTime);
		if (timeDiff >= (Int32)cmdTimeout || replySize <= this->recvSize)
			break;
		this->recvEvt->Wait(cmdTimeout);
	}
	if (replySize <= this->recvSize)
	{
		Bool eq = true;
		UOSInt i = 0;
		while (i < replySize)
		{
			if (cmdReply[i] != this->recvBuff[i])
			{
				eq = false;
				break;
			}
			if (eq)
			{
				return true;
			}
		}
	}
	return false;
}

IO::TVCtrl::MDT701STVControl::MDT701STVControl(IO::Stream *stm, Int32 monId)
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

IO::TVCtrl::MDT701STVControl::~MDT701STVControl()
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

Bool IO::TVCtrl::MDT701STVControl::SendInstruction(CommandType ct)
{
	switch (ct)
	{
	case CT_POWERON:
		return SendBasicCommand("00!\r", 4, "00!\r", 4, 200, 20000);
	case CT_POWEROFF:
		return SendBasicCommand("00\"\r", 4, "00\"\r", 4, 200, 20000);
	case CT_FORCEPOWEROFF:
		//////////////////////////////////////////////////
		//return SendBasicCommand("", 0, "", 0, 200, 15000);
		return false;
	case CT_AUTOSETUP:
		return SendBasicCommand("00r09\r", 6, "00r09\r", 6, 200, 6000);
	case CT_FACTORYRESET:
		//////////////////////////////////////////////////
		//return SendBasicCommand("", 0, "", 0, 200, 15000);
		return false;
	case CT_SCREENRESET:
		//////////////////////////////////////////////////
		//return SendBasicCommand("", 0, "", 0, 200, 15000);
		return false;
	case CT_VOLUMEUP:
		return SendBasicCommand("00r06\r", 6, "00r06\r", 6, 200, 600);
	case CT_VOLUMEDOWN:
		return SendBasicCommand("00r07\r", 6, "00r07\r", 6, 200, 600);
	case CT_AUDIOMUTE:
		return SendBasicCommand("00ra6\r", 6, "00ra6\r", 6, 200, 600);
	case CT_INPUT_DSUB:
		return SendBasicCommand("00_r3\r", 6, "00_r3\r", 6, 3000, 5000);
	case CT_INPUT_VIDEO:
		return SendBasicCommand("00_v1\r", 6, "00_v1\r", 6, 3000, 5000);
	case CT_INPUT_HDMI1:
		return SendBasicCommand("00_r1\r", 6, "00_r1\r", 6, 3000, 5000);
	case CT_INPUT_HDMI2:
		return SendBasicCommand("00_r7\r", 6, "00_r7\r", 6, 3000, 5000);
	case CT_INPUT_DVI_D:
		return SendBasicCommand("00_r2\r", 6, "00_r2\r", 6, 3000, 5000);
	case CT_INPUT_DISPLAYPORT:
		return SendBasicCommand("00_r6\r", 6, "00_r6\r", 6, 3000, 5000);
	case CT_INPUT_YPBPR:
		return SendBasicCommand("00_v2\r", 6, "00_v2\r", 6, 3000, 5000);
	case CT_INPUT_SVIDEO:
		return SendBasicCommand("00_v3\r", 6, "00_v3\r", 6, 3000, 5000);
	case CT_GETPOWERSTATUS:
	case CT_INPUT_DISPLAYPORT2:
	case CT_INPUT_DISPLAYPORT3:
	case CT_INPUT_YPBPR2:
	case CT_GETCURRINPUT:
	case CT_GETDATETIME:
	case CT_SETCURRDATETIME:
	case CT_SELFDIAGNOSIS:
	case CT_GETSERIALNO:
	case CT_GETMODEL:
	case CT_GETFIRMWARE:
	case CT_GETFIRMWARE2:
	case CT_GETPOWERSAVEMODE:
	case CT_GETPOWERSAVETIME:
	case CT_GETAUTOSTANDBYTIME:
	case CT_GETLANMAC:
	case CT_GETLANIP:
	case CT_GETTEMPERATURE1:
	case CT_GETTEMPERATURE2:
	case CT_GETTEMPERATURE3:
	case CT_GETBACKLIGHT:
	case CT_SETBACKLIGHT:
	case CT_GETCONTRAST:
	case CT_SETCONTRAST:
	case CT_GETSHARPNESS:
	case CT_SETSHARPNESS:
	case CT_GETBRIGHTNESS:
	case CT_SETBRIGNTNESS:
	case CT_GETHUE:
	case CT_SETHUE:
	case CT_GETSATURATION:
	case CT_SETSATURATION:
	case CT_GETCOLORTEMP:
	case CT_SETCOLORTEMP:
	case CT_IRLOCK:
	case CT_IRUNLOCK:
	case CT_GETIRSTATUS:
	case CT_GETHPOS:
	case CT_GETVPOS:
	default:
		return false;
	}
}

Bool IO::TVCtrl::MDT701STVControl::SendGetCommand(CommandType ct, Int32 *val, UTF8Char *sbuff)
{
	return false;
}

Bool IO::TVCtrl::MDT701STVControl::SendSetCommand(CommandType ct, Int32 val)
{
	return false;
}

#define ADDCMD(a, b) {cmdList->Add(a);cmdFormats->Add(b);}
void IO::TVCtrl::MDT701STVControl::GetSupportedCmd(Data::ArrayList<CommandType> *cmdList, Data::ArrayList<CommandFormat> *cmdFormats)
{
	ADDCMD(CT_POWERON, CF_INSTRUCTION);
	ADDCMD(CT_POWEROFF, CF_INSTRUCTION);
	ADDCMD(CT_AUTOSETUP, CF_INSTRUCTION);
	ADDCMD(CT_VOLUMEUP, CF_INSTRUCTION);
	ADDCMD(CT_VOLUMEDOWN, CF_INSTRUCTION);
	ADDCMD(CT_AUDIOMUTE, CF_INSTRUCTION);
	ADDCMD(CT_INPUT_DSUB, CF_INSTRUCTION);
	ADDCMD(CT_INPUT_VIDEO, CF_INSTRUCTION);
	ADDCMD(CT_INPUT_HDMI1, CF_INSTRUCTION);
	ADDCMD(CT_INPUT_HDMI2, CF_INSTRUCTION);
	ADDCMD(CT_INPUT_DVI_D, CF_INSTRUCTION);
	ADDCMD(CT_INPUT_DISPLAYPORT, CF_INSTRUCTION);
	ADDCMD(CT_INPUT_YPBPR, CF_INSTRUCTION);
	ADDCMD(CT_INPUT_SVIDEO, CF_INSTRUCTION);
}

Bool IO::TVCtrl::MDT701STVControl::GetInfo(IO::TVControl::TVInfo *info)
{
	info->name = (const UTF8Char*)"Mitsubishi MDT701S";
	info->tvType = IO::TVControl::TVT_MDT701S;
	info->defBaudRate = 9600;
	return true;
}
