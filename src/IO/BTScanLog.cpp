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
	NEW_CLASS(this->devs, Data::UInt64Map<DevEntry*>());
	NEW_CLASS(this->logs, Data::ArrayList<LogEntry*>());
}

IO::BTScanLog::~BTScanLog()
{
	this->ClearList();
	DEL_CLASS(this->devs);
	DEL_CLASS(this->logs);
}

IO::ParsedObject::ParserType IO::BTScanLog::GetParserType()
{
	return IO::ParsedObject::PT_BTSCANLOG;
}

IO::BTScanLog::LogEntry *IO::BTScanLog::AddEntry(Int64 timeTicks, UInt64 macInt, RadioType radioType, AddressType addrType, UInt16 company, const UTF8Char *name, Int8 rssi, Int8 txPower)
{
	LogEntry *log = MemAlloc(LogEntry, 1);
	log->macInt = macInt;
	log->timeTicks = timeTicks;
	log->rssi = rssi;
	log->txPower = txPower;
	this->logs->Add(log);
	DevEntry *dev = this->devs->Get(macInt);
	if (dev == 0)
	{
		dev = MemAlloc(DevEntry, 1);
		dev->macInt = macInt;
		dev->company = company;
		dev->radioType = radioType;
		dev->addrType = addrType;
		dev->name = SCOPY_TEXT(name);
		NEW_CLASS(dev->logs, Data::ArrayList<LogEntry*>());
		this->devs->Put(macInt, dev);
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

void IO::BTScanLog::AddBTRAWPacket(Int64 timeTicks, const UInt8 *buff, UOSInt buffSize)
{
	UTF8Char sbuff[256];
	if (buffSize < 19)
	{
		return;
	}
	UInt32 dir = ReadMUInt32(&buff[0]);
	if (dir != 1) //Rcvd
	{
		return;
	}
	if (buff[4] == 4 && buff[5] == 0x3E) //HCI Event, LE Meta
	{
		UInt8 len = buff[6];
		if ((UOSInt)len + 7 > buffSize)
		{
			return;
		}
		UInt8 addrType;
		Int8 txPower;
		Int8 rssi;
		UInt8 mac[8];
		UOSInt optEnd;
		UOSInt i;
		UInt16 company = 0;
		if (buff[7] == 0xd) //Sub Event: LE Extended Advertising Report (0x0d)
		{
			UInt8 numReports = buff[8];
			if (numReports != 1)
			{
				return;
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
			rssi = (Int8)buff[22];
			txPower = (Int8)buff[21];

			optEnd = (UOSInt)buff[32] + 33;
			i = 33;
		}
		else if (buff[7] == 0x2)
		{
			UInt8 numReports = buff[8];
			if (numReports != 1)
			{
				return;
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
			txPower = 0;

			optEnd = (UOSInt)buff[17] + 18;
			i = 18;
			if (optEnd < buffSize)
			{
				rssi = (Int8)buff[optEnd];
			}
			else
			{
				rssi = 0;
			}
		}
		else
		{
			return;
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
				company = ReadUInt16(&buff[i + 2]);
			}
			i += 1 + (UOSInt)optLen;
		}


		AddressType aType = AT_UNKNOWN;
		if (addrType == 0)
		{
			aType = AT_PUBLIC;
		}
		else if (addrType == 1)
		{
			aType = AT_RANDOM;
		}
		const UTF8Char *name;
		if (sbuff[0])
		{
			name = sbuff;
		}
		else
		{
			name = 0;
		}
		this->AddEntry(timeTicks, ReadMUInt64(mac), RT_LE, aType, company, name, rssi, txPower);
	}
	else if (buff[4] == 4 && buff[5] == 0x2F) //HCI Event, Extended Inquiry Result
	{
		UInt8 len = buff[6];
		if ((UOSInt)len + 7 > buffSize || len < 15)
		{
			return;
		}
		Int8 rssi = (Int8)buff[21];
		UInt8 mac[8];
		UOSInt optEnd;
		UOSInt i;
		UInt8 numReports = buff[7];
		UInt16 company = 0;
		if (numReports != 1)
		{
			return;
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
				company = ReadUInt16(&buff[i + 2]);
			}
			i += 1 + (UOSInt)optLen;	
		}

		AddressType aType = AT_PUBLIC;
		const UTF8Char *name;
		if (sbuff[0])
		{
			name = sbuff;
		}
		else
		{
			name = 0;
		}
		this->AddEntry(timeTicks, ReadMUInt64(mac), RT_HCI, aType, company, name, rssi, 0);
	}
}

void IO::BTScanLog::ClearList()
{
	LIST_FREE_FUNC(this->logs, MemFree);
	Data::ArrayList<IO::BTScanLog::DevEntry*> *devList = this->GetDevList();
	LIST_FREE_FUNC(devList, this->FreeDev);
	this->logs->Clear();
	this->devs->Clear();
}

Data::ArrayList<IO::BTScanLog::DevEntry*> *IO::BTScanLog::GetDevList()
{
	return this->devs->GetValues();
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
