#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "IO/BTScanLog.h"

void IO::BTScanLog::FreeDev(DevEntry* dev)
{
	SDEL_TEXT(dev->name);
	DEL_CLASS(dev->logs);
	MemFree(dev);
}

IO::BTScanLog::BTScanLog(const UTF8Char *sourceName) : IO::ParsedObject(sourceName)
{
	NEW_CLASS(this->pubDevs, Data::UInt64Map<DevEntry*>());
	NEW_CLASS(this->randDevs, Data::UInt64Map<DevEntry*>());
	NEW_CLASS(this->logs, Data::ArrayList<LogEntry*>());
}

IO::BTScanLog::~BTScanLog()
{
	this->ClearList();
	DEL_CLASS(this->pubDevs);
	DEL_CLASS(this->randDevs);
	DEL_CLASS(this->logs);
}

IO::ParsedObject::ParserType IO::BTScanLog::GetParserType()
{
	return IO::ParsedObject::PT_BTSCANLOG;
}

IO::BTScanLog::LogEntry *IO::BTScanLog::AddEntry(Int64 timeTicks, UInt64 macInt, RadioType radioType, AddressType addrType, UInt16 company, const UTF8Char *name, Int8 rssi, Int8 txPower, Int8 measurePower)
{
	LogEntry *log = MemAlloc(LogEntry, 1);
	log->macInt = macInt;
	log->timeTicks = timeTicks;
	log->rssi = rssi;
	log->txPower = txPower;
	this->logs->Add(log);
	DevEntry *dev;
	if (addrType == AT_RANDOM)
	{
		dev = this->randDevs->Get(macInt);
	}
	else
	{
		dev = this->pubDevs->Get(macInt);
	}
	if (dev == 0)
	{
		dev = MemAlloc(DevEntry, 1);
		dev->macInt = macInt;
		dev->company = company;
		dev->radioType = radioType;
		dev->addrType = addrType;
		dev->measurePower = measurePower;
		dev->name = SCOPY_TEXT(name);
		NEW_CLASS(dev->logs, Data::ArrayList<LogEntry*>());
		if (addrType == AT_RANDOM)
		{
			this->randDevs->Put(macInt, dev);
		}
		else
		{
			this->pubDevs->Put(macInt, dev);
		}
	}
	if (name && dev->name == 0)
	{
		dev->name = Text::StrCopyNew(name);
	}
	if (company && dev->company == 0)
	{
		dev->company = company;
	}
	dev->logs->Add(log);
	return log;
}

IO::BTScanLog::LogEntry *IO::BTScanLog::AddScanRec(const IO::BTScanLog::ScanRecord2 *rec)
{
	return this->AddEntry(rec->lastSeenTime, rec->macInt, rec->radioType, rec->addrType, rec->company, rec->name, rec->rssi, rec->txPower, rec->measurePower);
}

void IO::BTScanLog::AddBTRAWPacket(Int64 timeTicks, const UInt8 *buff, UOSInt buffSize)
{
	IO::BTScanLog::ScanRecord2 rec;
	if (ParseBTRAWPacket(&rec, timeTicks, buff, buffSize))
	{
		this->AddScanRec(&rec);
		SDEL_TEXT(rec.name);
		return;
	}
}

void IO::BTScanLog::ClearList()
{
	LIST_FREE_FUNC(this->logs, MemFree);
	Data::ArrayList<IO::BTScanLog::DevEntry*> *devList;
	devList = this->GetPublicList();
	LIST_FREE_FUNC(devList, this->FreeDev);
	devList = this->GetRandomList();
	LIST_FREE_FUNC(devList, this->FreeDev);
	this->logs->Clear();
	this->pubDevs->Clear();
	this->randDevs->Clear();
}

Data::ArrayList<IO::BTScanLog::DevEntry*> *IO::BTScanLog::GetPublicList()
{
	return this->pubDevs->GetValues();
}

Data::ArrayList<IO::BTScanLog::DevEntry*> *IO::BTScanLog::GetRandomList()
{
	return this->randDevs->GetValues();
}

const UTF8Char *IO::BTScanLog::RadioTypeGetName(RadioType radioType)
{
	switch (radioType)
	{
	case RT_HCI:
		return (const UTF8Char*)"HCI";
	case RT_LE:
		return (const UTF8Char*)"LE";
	case RT_UNKNOWN:
	default:
		return (const UTF8Char*)"UNK";
	}
}

const UTF8Char *IO::BTScanLog::AddressTypeGetName(AddressType addrType)
{
	switch (addrType)
	{
	case AT_PUBLIC:
		return (const UTF8Char*)"Public";
	case AT_RANDOM:
		return (const UTF8Char*)"Random";
	case AT_UNKNOWN:
	default:
		return (const UTF8Char*)"Unknown";
	}
}

Bool IO::BTScanLog::ParseBTRAWPacket(IO::BTScanLog::ScanRecord2 *rec, Int64 timeTicks, const UInt8 *buff, UOSInt buffSize)
{
	UTF8Char sbuff[256];
	if (buffSize < 19)
	{
		return false;
	}
	UInt32 dir = ReadMUInt32(&buff[0]);
	if (dir != 1) //Rcvd
	{
		return false;
	}
	rec->inRange = true;
	rec->connected = false;
	rec->company = 0;
	rec->lastSeenTime = timeTicks;
	rec->measurePower = 0;
	UInt8 mac[8];
	UInt8 addrType;
	if (buff[4] == 4 && buff[5] == 0x3E) //HCI Event, LE Meta
	{
		UInt8 len = buff[6];
		if ((UOSInt)len + 7 > buffSize)
		{
			return false;
		}
		UOSInt optEnd;
		UOSInt i;
		if (buff[7] == 0xd) //Sub Event: LE Extended Advertising Report (0x0d)
		{
			UInt8 numReports = buff[8];
			if (numReports != 1)
			{
				return false;
			}
			addrType = buff[11];
			mac[0] = 0;
			mac[1] = 0;
			mac[2] = buff[17];
			mac[3] = buff[16];
			mac[4] = buff[15];
			mac[5] = buff[14];
			mac[6] = buff[13];
			mac[7] = buff[12];
			rec->rssi = (Int8)buff[22];
			rec->txPower = (Int8)buff[21];

			optEnd = (UOSInt)buff[32] + 33;
			i = 33;
		}
		else if (buff[7] == 0x2)
		{
			UInt8 numReports = buff[8];
			if (numReports != 1)
			{
				return false;
			}
			addrType = buff[10];
			mac[0] = 0;
			mac[1] = 0;
			mac[2] = buff[16];
			mac[3] = buff[15];
			mac[4] = buff[14];
			mac[5] = buff[13];
			mac[6] = buff[12];
			mac[7] = buff[11];
			rec->txPower = 0;

			optEnd = (UOSInt)buff[17] + 18;
			i = 18;
			if (optEnd < buffSize)
			{
				rec->rssi = (Int8)buff[optEnd];
			}
			else
			{
				rec->rssi = 0;
			}
		}
		else
		{
			return false;
		}

		if (optEnd > buffSize)
		{
			optEnd = buffSize;
		}
		sbuff[0] = 0;
		while (i + 1 < optEnd)
		{
			UInt8 optLen = buff[i];
			if (optLen == 0 || optLen + i + 1 > optEnd)
			{
				break;
			}
			if (buff[i + 1] == 8 || buff[i + 1] == 9)
			{
				Text::StrConcatC(sbuff, &buff[i + 2], (UOSInt)optLen - 1);
			}
			else if (buff[i + 1] == 0xFF)
			{
				rec->company = ReadUInt16(&buff[i + 2]);
				if (rec->company == 0x4C && optLen == 26)
				{
					rec->measurePower = (Int8)buff[i + 26];
				}
			}
			i += 1 + (UOSInt)optLen;
		}

		rec->addrType = AT_UNKNOWN;
		if (addrType == 0)
		{
			rec->addrType = AT_PUBLIC;
		}
		else if (addrType == 1)
		{
			rec->addrType = AT_RANDOM;
		}
		if (sbuff[0])
		{
			rec->name = Text::StrCopyNew(sbuff);
		}
		else
		{
			rec->name = 0;
		}
		rec->radioType = RT_LE;
		rec->macInt = ReadMUInt64(mac);
		rec->mac[0] = mac[2];
		rec->mac[1] = mac[3];
		rec->mac[2] = mac[4];
		rec->mac[3] = mac[5];
		rec->mac[4] = mac[6];
		rec->mac[5] = mac[7];
		return true;
	}
	else if (buff[4] == 4 && buff[5] == 0x2F) //HCI Event, Extended Inquiry Result
	{
		UInt8 len = buff[6];
		if ((UOSInt)len + 7 > buffSize || len < 15)
		{
			return false;
		}
		rec->rssi = (Int8)buff[21];
		UOSInt optEnd;
		UOSInt i;
		UInt8 numReports = buff[7];
		if (numReports != 1)
		{
			return false;
		}
		mac[0] = 0;
		mac[1] = 0;
		mac[2] = buff[13];
		mac[3] = buff[12];
		mac[4] = buff[11];
		mac[5] = buff[10];
		mac[6] = buff[9];
		mac[7] = buff[8];
		optEnd = buffSize;
		i = 22;

		sbuff[0] = 0;
		while (i + 1 < optEnd)
		{
			UInt8 optLen = buff[i];
			if (optLen == 0 || optLen + i + 1 > optEnd)
			{
				break;
			}
			if (buff[i + 1] == 8 || buff[i + 1] == 9)
			{
				Text::StrConcatC(sbuff, &buff[i + 2], (UOSInt)optLen - 1);
			}
			else if (buff[i + 1] == 0xff)
			{
				rec->company = ReadUInt16(&buff[i + 2]);
				if (rec->company == 0x4C && optLen == 26)
				{
					rec->measurePower = (Int8)buff[i + 26];
				}
			}
			i += 1 + (UOSInt)optLen;	
		}

		rec->addrType = AT_PUBLIC;
		if (sbuff[0])
		{
			rec->name = Text::StrCopyNew(sbuff);
		}
		else
		{
			rec->name = 0;
		}
		rec->txPower = 0;
		rec->radioType = RT_HCI;
		rec->macInt = ReadMUInt64(mac);
		rec->mac[0] = mac[2];
		rec->mac[1] = mac[3];
		rec->mac[2] = mac[4];
		rec->mac[3] = mac[5];
		rec->mac[4] = mac[6];
		rec->mac[5] = mac[7];
		return true;
	}
	return false;
}
