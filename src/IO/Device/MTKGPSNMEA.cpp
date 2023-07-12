#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/DateTime.h"
#include "IO/FileStream.h"
#include "IO/SerialPort.h"
#include "IO/Device/MTKGPSNMEA.h"
#include "Sync/MutexUsage.h"
#include "Sync/ThreadUtil.h"
#include "Text/CString.h"

#include <stdio.h>

void IO::Device::MTKGPSNMEA::ParseUnknownCmd(const UTF8Char *cmd, UOSInt cmdLen)
{
	if (Text::StrStartsWithC(cmd, cmdLen, UTF8STRC("$PMTK")))
	{
		if (Text::StrStartsWithC(cmd, cmdLen, UTF8STRC("$PMTK011,")))
		{
		}
		else if (Text::StrStartsWithC(cmd, cmdLen, UTF8STRC("$PMTK010,")))
		{
		}
		else
		{
			this->cmdWResults.Add(Text::String::New(cmd, cmdLen));
			this->cmdEvt.Set();
		}
	}
	else
	{
		this->cmdWResults.Add(Text::String::New(cmd, cmdLen));
		this->cmdEvt.Set();
	}
}

IO::Device::MTKGPSNMEA::MTKGPSNMEA(IO::Stream *stm, Bool relStm) : IO::GPSNMEA(stm, relStm)
{
	this->firmwareBuild = 0;
	this->firmwareRel = 0;
	this->productMode = 0;
	this->sdkVer = 0;
}

IO::Device::MTKGPSNMEA::~MTKGPSNMEA()
{
	this->stm->Close();
	LIST_FREE_STRING(&this->cmdWResults);
	SDEL_STRING(this->firmwareBuild);
	SDEL_STRING(this->firmwareRel);
	SDEL_STRING(this->productMode);
	SDEL_STRING(this->sdkVer);
}

Map::ILocationService::ServiceType IO::Device::MTKGPSNMEA::GetServiceType()
{
	return Map::ILocationService::ST_MTK;
}

void IO::Device::MTKGPSNMEA::HotStart()
{
	UInt8 buff[64];
	UOSInt cmdSize = GenNMEACommand(UTF8STRC("$PMTK101"), buff);
	this->stm->Write(buff, cmdSize);
}

void IO::Device::MTKGPSNMEA::WarmStart()
{
	UInt8 buff[64];
	UOSInt cmdSize = GenNMEACommand(UTF8STRC("$PMTK102"), buff);
	this->stm->Write(buff, cmdSize);
}

void IO::Device::MTKGPSNMEA::ColdStart()
{
	UInt8 buff[64];
	UOSInt cmdSize = GenNMEACommand(UTF8STRC("$PMTK103"), buff);
	this->stm->Write(buff, cmdSize);
}

void IO::Device::MTKGPSNMEA::FactoryReset()
{
	UInt8 buff[64];
	UOSInt cmdSize = GenNMEACommand(UTF8STRC("$PMTK104"), buff);
	this->stm->Write(buff, cmdSize);
}

Bool IO::Device::MTKGPSNMEA::IsMTKDevice()
{
	UInt8 buff[64];
	Bool succ = false;
	UOSInt cmdSize = GenNMEACommand(UTF8STRC("$PMTK000"), buff);
	Text::String *result = SendMTKCommand(buff, cmdSize, UTF8STRC("$PMTK001"), 2000);
	if (result == 0)
		return false;
	succ = result->StartsWith(UTF8STRC("$PMTK001,0,3*"));
	result->Release();
	return succ;
}

Bool IO::Device::MTKGPSNMEA::QueryFirmware()
{
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	Text::PString sarr2[2];
	Text::PString sarr[5];
	UOSInt i;
	UInt8 buff[64];
	UOSInt cmdSize = GenNMEACommand(UTF8STRC("$PMTK605"), buff);
	Text::String *result = SendMTKCommand(buff, cmdSize, UTF8STRC("$PMTK705"), 2000);
	if (result == 0)
		return false;
	
	sptr = result->ConcatTo(sbuff);
	result->Release();
	Text::StrSplitP(sarr2, 2, {sbuff, (UOSInt)(sptr - sbuff)}, '*');
	i = Text::StrSplitP(sarr, 5, sarr2[0], ',');
	if (i == 5)
	{
		SDEL_STRING(this->firmwareBuild);
		SDEL_STRING(this->firmwareRel);
		SDEL_STRING(this->productMode);
		SDEL_STRING(this->sdkVer);
		this->firmwareRel = Text::String::New(sarr[1].v, sarr[1].leng);
		this->firmwareBuild = Text::String::New(sarr[2].v, sarr[2].leng);
		this->productMode = Text::String::New(sarr[3].v, sarr[3].leng);
		this->sdkVer = Text::String::New(sarr[4].v, sarr[4].leng);
		return true;
	}
	else
	{
		return false;
	}
}

Bool IO::Device::MTKGPSNMEA::IsLogEnabled()
{
	UInt8 buff[64];
	UTF8Char *sptr;
	UOSInt cmdSize = GenNMEACommand(UTF8STRC("$PMTK182,2,7"), buff);
	UTF8Char sbuff[128];
	Text::String *result = SendMTKCommand(buff, cmdSize, UTF8STRC("$PMTK182,3,7,"), 2000);
	if (result == 0)
		return false;
	sptr = result->ConcatTo(sbuff);
	UOSInt i = Text::StrIndexOfCharC(sbuff, (UOSInt)(sptr - sbuff), '*');
	if (i == INVALID_INDEX)
	{
		result->Release();
		return false;
	}
	sbuff[i] = 0;
	i = Text::StrLastIndexOfCharC(sbuff, i, ',');
	i = Text::StrToUInt32(&sbuff[i + 1]);
	result->Release();
	return (i & 2) != 0;
}

Bool IO::Device::MTKGPSNMEA::DisableLog()
{
	UInt8 buff[64];
	UOSInt cmdSize = GenNMEACommand(UTF8STRC("$PMTK182,5"), buff);
	Text::String *result = SendMTKCommand(buff, cmdSize, UTF8STRC("$PMTK001,182,5,3"), 2000);
	if (result == 0)
		return false;
	result->Release();
	return true;
}

Bool IO::Device::MTKGPSNMEA::EnableLog()
{
	UInt8 buff[64];
	UOSInt cmdSize = GenNMEACommand(UTF8STRC("$PMTK182,4"), buff);
	Text::String *result = SendMTKCommand(buff, cmdSize, UTF8STRC("$PMTK001,182,4,3"), 2000);
	if (result == 0)
		return false;
	result->Release();
	return true;
}

UOSInt IO::Device::MTKGPSNMEA::CalLogBlockCount(UOSInt logSize)
{
	if (logSize & 65535)
	{
		return (logSize >> 16) + 1;
	}
	else
	{
		return (logSize >> 16);
	}
}

Bool IO::Device::MTKGPSNMEA::ReadLogPart(UOSInt addr, UInt8 *buff)
{
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	UOSInt i;
	UOSInt j;
	UInt8 cbuff[64];
	sptr = Text::StrConcatC(sbuff, UTF8STRC("$PMTK182,7,"));
	sptr = Text::StrHexVal32(sptr, (UInt32)addr);
	sptr = Text::StrConcatC(sptr, UTF8STRC(","));
	sptr = Text::StrHexVal32(sptr, 0x400);
	Data::DateTime dt;
	Data::DateTime dt2;
	Text::String *data = 0;
	Text::String *resp = 0;
	Text::String *cmdRes;
	i = GenNMEACommand(sbuff, (UOSInt)(sptr - sbuff), cbuff);
	Sync::MutexUsage mutUsage(&this->cmdMut);
	this->stm->Write(cbuff, i);
	
	dt.SetCurrTimeUTC();
	while (true)
	{
		this->cmdEvt.Wait(1000);
		while (this->cmdWResults.GetCount() > 0)
		{
			dt.SetCurrTimeUTC();
			cmdRes = this->cmdWResults.RemoveAt(0);
			if (cmdRes->StartsWith(UTF8STRC("$PMTK182,8")))
			{
				SDEL_STRING(data);
				data = cmdRes;
			}
			else if (cmdRes->StartsWith(UTF8STRC("$PMTK001,182,7,")))
			{
				resp = cmdRes;
				break;
			}
			else
			{
				cmdRes->Release();
			}
		}
		if (resp)
			break;
		dt2.SetCurrTimeUTC();
		if (dt2.DiffMS(&dt) >= 2000)
			break;
	}

	mutUsage.EndUse();
	if (resp && data)
	{
		Bool succ = false;
		Text::StrConcatC(sbuff, &data->v[11], 8);
		i = Text::StrHex2UInt32C(sbuff);
		j = data->leng;
		if (i == addr && j == 2071)
		{
			j = Text::StrHex2Bytes(&data->v[20], buff);
			if (j == 1024)
			{
				succ = true;
			}
		}
		data->Release();
		resp->Release();
		return succ;
	}
	else
	{
		SDEL_STRING(data);
		SDEL_STRING(resp);
		return false;
	}
}

Bool IO::Device::MTKGPSNMEA::ReadLogBlock(UOSInt addr, UInt8 *buff)
{
	UOSInt retryCnt = 0;
	UOSInt ofst = 0;
	Bool succ;
	while (ofst < 65536)
	{
		succ = this->ReadLogPart(addr + ofst, &buff[ofst]);
		if (succ)
		{
			ofst += 1024;
			retryCnt = 0;
		}
		else
		{
			if (++retryCnt >= 3)
			{
				//return false;
				ofst += 1024;
				retryCnt = 0;
				printf("MTKGPSNMEA: Skipping block %d\r\n", (Int32)(addr + ofst));
			}
		}
	}
	return true;
}

Bool IO::Device::MTKGPSNMEA::ParseLog(Map::GPSTrack *gps)
{
	Bool logEnabled = this->IsLogEnabled();
	this->DisableLog();
	UOSInt logSize = this->GetLogSize();
	UOSInt endAddr = this->CalLogBlockCount(logSize) << 16;
	UOSInt addr = 0;
	Bool succ = true;
	UInt8 *block;
	block = MemAlloc(UInt8, 65536);
	while (addr < endAddr)
	{
		succ = this->ReadLogBlock(addr, block);
		if (!succ)
			break;
		succ = ParseBlock(block, gps);
		if (!succ)
			break;
		addr += 65536;
	}

	MemFree(block);
	if (logEnabled)
	{
		this->EnableLog();
	}
	return succ;
}

Bool IO::Device::MTKGPSNMEA::DelLogData()
{
	UInt8 buff[64];
	UOSInt cmdSize = GenNMEACommand(UTF8STRC("$PMTK182,6,1"), buff);
	Text::String *result = SendMTKCommand(buff, cmdSize, UTF8STRC("$PMTK001,182,6,3"), 30000);
	if (result == 0)
		return false;
	result->Release();
	return true;
}

Bool IO::Device::MTKGPSNMEA::SetLogFormat(LogFormat lf)
{
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	UInt8 buff[64];
	sptr = Text::StrHexVal32(Text::StrConcatC(sbuff, UTF8STRC("$PMTK182,1,2,")), (UInt32)lf);
	UOSInt cmdSize = GenNMEACommand(sbuff, (UOSInt)(sptr - sbuff), buff);
	Text::String *result = SendMTKCommand(buff, cmdSize, UTF8STRC("$PMTK001,182,1,3"), 2000);
	if (result == 0)
		return false;
	result->Release();
	return true;
}

Bool IO::Device::MTKGPSNMEA::SetLogInterval(UInt32 sec)
{
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	UInt8 buff[64];
	sptr = Text::StrUInt32(Text::StrConcatC(sbuff, UTF8STRC("$PMTK182,1,3,")), sec * 10);
	UOSInt cmdSize = GenNMEACommand(sbuff, (UOSInt)(sptr - sbuff), buff);
	Text::String *result = SendMTKCommand(buff, cmdSize, UTF8STRC("$PMTK001,182,1,3"), 2000);
	if (result == 0)
		return false;
	result->Release();
	return true;
}

Bool IO::Device::MTKGPSNMEA::SetLogDistance(UInt32 meter)
{
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	UInt8 buff[64];
	sptr = Text::StrUInt32(Text::StrConcatC(sbuff, UTF8STRC("$PMTK182,1,4,")), meter * 10);
	UOSInt cmdSize = GenNMEACommand(sbuff, (UOSInt)(sptr - sbuff), buff);
	Text::String *result = SendMTKCommand(buff, cmdSize, UTF8STRC("$PMTK001,182,1,3"), 2000);
	if (result == 0)
		return false;
	result->Release();
	return true;
}

Bool IO::Device::MTKGPSNMEA::SetLogMode(LogMode lm)
{
	Text::CString cmd;
	UInt8 buff[64];
	if (lm == LM_OVERWRITE)
	{
		cmd = CSTR("$PMTK182,1,6,1");
	}
	else if (lm == LM_STOP)
	{
		cmd = CSTR("$PMTK182,1,6,2");
	}
	else
	{
		return false;
	}
	UOSInt cmdSize = GenNMEACommand(cmd.v, cmd.leng, buff);
	Text::String *result = SendMTKCommand(buff, cmdSize, UTF8STRC("$PMTK001,182,1,3"), 2000);
	if (result == 0)
		return false;
	result->Release();
	return true;
}

IO::Device::MTKGPSNMEA::LogFormat IO::Device::MTKGPSNMEA::GetLogFormat()
{
	UTF8Char sbuff[128];
	UTF8Char *sptr;
	UInt8 buff[64];
	UOSInt cmdSize = GenNMEACommand(UTF8STRC("$PMTK182,2,2"), buff);
	Text::String *result = SendMTKCommand(buff, cmdSize, UTF8STRC("$PMTK182,3,2"), 2000);
	if (result == 0)
		return LF_UNKNOWN;
	sptr = result->ConcatTo(sbuff);
	UOSInt i = Text::StrIndexOfCharC(sbuff, (UOSInt)(sptr - sbuff), '*');
	if (i == INVALID_INDEX)
	{
		result->Release();
		return LF_UNKNOWN;
	}
	sbuff[i] = 0;
	i = Text::StrLastIndexOfCharC(sbuff, i, ',');
	i = (UInt32)Text::StrHex2Int32C(&sbuff[i + 1]);
	result->Release();
	return (LogFormat)i;
}

UInt32 IO::Device::MTKGPSNMEA::GetLogInterval()
{
	UTF8Char sbuff[128];
	UInt8 buff[64];
	UOSInt cmdSize = GenNMEACommand(UTF8STRC("$PMTK182,2,3"), buff);
	Text::String *result = SendMTKCommand(buff, cmdSize, UTF8STRC("$PMTK182,3,3"), 2000);
	if (result == 0)
		return 0;
	result->ConcatTo(sbuff);
	UOSInt i = Text::StrIndexOfChar(sbuff, '*');
	if (i == INVALID_INDEX)
	{
		result->Release();
		return 0;
	}
	sbuff[i] = 0;
	i = Text::StrLastIndexOfCharC(sbuff, i, ',');
	i = Text::StrToUInt32(&sbuff[i + 1]);
	result->Release();
	return (UInt32)i / 10;
}

UInt32 IO::Device::MTKGPSNMEA::GetLogDistance()
{
	UTF8Char sbuff[128];
	UInt8 buff[64];
	UOSInt cmdSize = GenNMEACommand(UTF8STRC("$PMTK182,2,4"), buff);
	Text::String *result = SendMTKCommand(buff, cmdSize, UTF8STRC("$PMTK182,3,4"), 2000);
	if (result == 0)
		return 0;
	result->ConcatTo(sbuff);
	UOSInt i = Text::StrIndexOfChar(sbuff, '*');
	if (i == INVALID_INDEX)
	{
		result->Release();
		return 0;
	}
	sbuff[i] = 0;
	i = Text::StrLastIndexOfCharC(sbuff, i, ',');
	i = Text::StrToUInt32(&sbuff[i + 1]);
	result->Release();
	return (UInt32)i / 10;
}

IO::Device::MTKGPSNMEA::LogMode IO::Device::MTKGPSNMEA::GetLogMode()
{
	UTF8Char sbuff[128];
	LogMode lm = LM_UNKNOWN;
	UInt8 buff[64];
	UOSInt cmdSize = GenNMEACommand(UTF8STRC("$PMTK182,2,6"), buff);
	Text::String *result = SendMTKCommand(buff, cmdSize, UTF8STRC("$PMTK182,3,6"), 2000);
	if (result == 0)
		return LM_UNKNOWN;
	result->ConcatTo(sbuff);
	UOSInt i = Text::StrIndexOfChar(sbuff, '*');
	if (i == INVALID_INDEX)
	{
		result->Release();
		return LM_UNKNOWN;
	}
	sbuff[i] = 0;
	i = Text::StrLastIndexOfCharC(sbuff, i, ',');
	i = Text::StrToUInt32(&sbuff[i + 1]);
	result->Release();
	if (i == 1)
	{
		lm = LM_OVERWRITE;
	}
	else if (i == 2)
	{
		lm = LM_STOP;
	}
	return lm;
}

UOSInt IO::Device::MTKGPSNMEA::GetLogSize()
{
	UTF8Char sbuff[128];
	UInt8 buff[64];
	UOSInt cmdSize = GenNMEACommand(UTF8STRC("$PMTK182,2,8"), buff);
	Text::String *result = SendMTKCommand(buff, cmdSize, UTF8STRC("$PMTK182,3,8"), 2000);
	if (result == 0)
		return 0;
	result->ConcatTo(sbuff);
	UOSInt i = Text::StrIndexOfChar(sbuff, '*');
	if (i == INVALID_INDEX)
	{
		result->Release();
		return 0;
	}
	sbuff[i] = 0;
	i = Text::StrLastIndexOfCharC(sbuff, i, ',');
	i = (UInt32)Text::StrHex2Int32C(&sbuff[i + 1]);
	result->Release();
	return i;
}

Text::String *IO::Device::MTKGPSNMEA::SendMTKCommand(const UInt8 *cmdBuff, UOSInt cmdSize, const UTF8Char *resultStart, UOSInt resultStartLen, Data::Duration timeout)
{
	Data::DateTime dt;
	Data::DateTime dt2;
	Text::String *cmdRes;

	Sync::MutexUsage mutUsage(&this->cmdMut);
	this->stm->Write(cmdBuff, cmdSize);
	Text::String *resultStr = 0;
	
	dt.SetCurrTimeUTC();
	while (true)
	{
		this->cmdEvt.Wait(1000);
		while (this->cmdWResults.GetCount() > 0)
		{
			dt.SetCurrTimeUTC();
			cmdRes = this->cmdWResults.RemoveAt(0);
			if (cmdRes->StartsWith(resultStart, resultStartLen))
			{
				resultStr = cmdRes;
				break;
			}
			else
			{
				cmdRes->Release();
			}
		}
		if (resultStr)
			break;
		dt2.SetCurrTimeUTC();
		if (dt2.Diff(&dt) >= timeout)
			break;
	}
	mutUsage.EndUse();
	return resultStr;
}

Text::String *IO::Device::MTKGPSNMEA::GetFirmwareRel()
{
	return this->firmwareRel;
}

Text::String *IO::Device::MTKGPSNMEA::GetFirmwareBuild()
{
	return this->firmwareBuild;
}

Text::String *IO::Device::MTKGPSNMEA::GetProductMode()
{
	return this->productMode;
}

Text::String *IO::Device::MTKGPSNMEA::GetSDKVer()
{
	return this->sdkVer;
}

UOSInt IO::Device::MTKGPSNMEA::GetMTKSerialPort()
{
	UOSInt port = IO::SerialPort::GetUSBPort();
	if (port)
		return port;
	port = IO::SerialPort::GetBTPort();
	return port;
}

Bool IO::Device::MTKGPSNMEA::ParseBlock(UInt8 *block, Map::GPSTrack *gps)
{
	Int32 bitmask;
	Map::GPSTrack::GPSRecord3 rec;
	bitmask = *(Int32*)&block[2];
	if ((bitmask & 0x3f) != 0x3f)
		return false;

	Data::DateTime currTime;
	currTime.SetCurrTimeUTC();
	currTime.SetYear((UInt16)(currTime.GetYear() - 5));
	Data::TimeInstant minTime = currTime.ToInstant();
	currTime.SetCurrTimeUTC();
	currTime.SetYear((UInt16)(currTime.GetYear() + 1));
	Data::TimeInstant maxTime = currTime.ToInstant();

	OSInt currOfst = 0x200;
	OSInt recStart;
	while (currOfst <= 65532)
	{
		if (*(UInt32*)&block[currOfst] == 0xaaaaaaaa && currOfst <= 65520 && (*(Int32*)&block[currOfst + 4] & 0xffffff) == 0xaaaaaa && *(UInt32*)&block[currOfst + 12] == 0xbbbbbbbb)
		{
			if (block[currOfst + 7] == 2)
			{
				bitmask = *(Int32*)&block[currOfst + 8];
				if ((bitmask & 0x3f) != 0x3f)
				{
					printf("MTKGPSNMEA: wrong bitmask\r\n");
					return false;
				}
			}
			currOfst += 16;
		}
		else if (*(UInt32*)&block[currOfst] == 0xffffffff)
		{
			return true;
		}
		else
		{
			recStart = currOfst;
//			if (bitmask & 1) // UTC
//			{
				rec.recTime = Data::TimeInstant(ReadUInt32(&block[currOfst]), 0);
				currOfst += 4;
				while (rec.recTime < minTime)
				{
					rec.recTime.sec += 619315200; //7168 days
				}
				while (rec.recTime > maxTime)
				{
					rec.recTime.sec -= 619315200; //7168 days
				}
//			}
//			if (bitmask & 2) // VALID
//			{
				rec.valid = (block[currOfst] & 1) != 1;
				currOfst += 2;
//			}
//			if (bitmask & 4) // LATITUDE
//			{
				rec.pos.SetLat(*(Double*)&block[currOfst]);
				currOfst += 8;
//			}
//			if (bitmask & 8) // LONGITUDE
//			{
				rec.pos.SetLon(*(Double*)&block[currOfst]);
				currOfst += 8;
//			}
			if (bitmask & 0x10) // HEIGHT
			{
				rec.altitude = *(Single*)&block[currOfst];
				currOfst += 4;
			}
			else
			{
				rec.altitude = 0;
			}
			if (bitmask & 0x20) // SPEED
			{
				rec.speed = (*(Single*)&block[currOfst]) / 1.852;
				currOfst += 4;
			}
			else
			{
				rec.speed = 0;
			}
			if (bitmask & 0x40) // HEADING
			{
				rec.heading = *(Single*)&block[currOfst];
				currOfst += 4;
			}
			else
			{
				rec.heading = 0;
			}
			if (bitmask & 0x80) // DSTA
			{
				currOfst += 2;
			}
			if (bitmask & 0x100) // DAGE
			{
				currOfst += 4;
			}
			if (bitmask & 0x200) // PDOP
			{
//				Double pdop = *(UInt16*)&block[currOfst] * 0.01;
				currOfst += 2;
			}
			if (bitmask & 0x400) // HDOP
			{
//				Double hdop = *(UInt16*)&block[currOfst] * 0.01;
				currOfst += 2;
			}
			if (bitmask & 0x800) // VDOP
			{
//				Double vdop = *(UInt16*)&block[currOfst] * 0.01;
				currOfst += 2;
			}
			if (bitmask & 0x1000) // NSAT
			{
				rec.nSateUsedGPS = block[currOfst];
				rec.nSateViewGPS = block[currOfst + 1];
				currOfst += 2;
			}
			else
			{
				rec.nSateUsedGPS = 0;
				rec.nSateViewGPS = 0;
			}
			if (bitmask & 0x2000) //SID-ELEVATION-AZIMUTH-SNR
			{
				Int32 size = 0;
				if (bitmask & 0x2000) //SID
				{
					size += 4;
				}
				if (bitmask & 0x4000) //ELEVATION
				{
					size += 2;
				}
				if (bitmask & 0x8000) //AZIMUTH
				{
					size += 2;
				}
				if (bitmask & 0x10000)
				{
					size += 2;
				}
				currOfst += rec.nSateUsed * size;
			}
			if (bitmask & 0x20000) // RCR
			{
				currOfst += 2;
			}
			if (bitmask & 0x40000) // MILLISECOND
			{
				currOfst += 2;
			}
			if (bitmask & 0x80000) // DISTANCE
			{
				currOfst += 8;
			}
			if (block[currOfst] != '*')
			{
				printf("MTKGPSNMEA: not star\r\n");
				return false;
			}
			rec.nSateUsed = rec.nSateUsedGPS;
			rec.nSateUsedGLO = 0;
			rec.nSateUsedSBAS = 0;
			rec.nSateViewGLO = 0;
			rec.nSateViewGA = 0;
			rec.nSateViewQZSS = 0;
			rec.nSateViewBD = 0;
			UInt8 chk = 0;
			while (recStart < currOfst)
			{
				chk = chk ^ block[recStart];
				recStart++;
			}
			if (block[currOfst + 1] == chk)
			{
				gps->AddRecord(&rec);
			}
			currOfst += 2;
		}
	}
	return true;
}
