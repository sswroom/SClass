#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "IO/BTScanLog.h"

void __stdcall IO::BTScanLog::FreeDev(NN<DevEntry> dev)
{
	OPTSTR_DEL(dev->name);
	dev->logs.Delete();
	MemFreeNN(dev);
}

IO::BTScanLog::BTScanLog(NN<Text::String> sourceName) : IO::ParsedObject(sourceName)
{
}

IO::BTScanLog::~BTScanLog()
{
	this->ClearList();
}

IO::ParserType IO::BTScanLog::GetParserType() const
{
	return IO::ParserType::BTScanLog;
}

NN<IO::BTScanLog::LogEntry> IO::BTScanLog::AddEntry(Int64 timeTicks, UInt64 macInt, RadioType radioType, AddressType addrType, UInt16 company, Optional<Text::String> name, Int8 rssi, Int8 txPower, Int8 measurePower, AdvType advType)
{
	NN<LogEntry> log = MemAllocNN(LogEntry);
	log->macInt = macInt;
	log->timeTicks = timeTicks;
	log->rssi = rssi;
	log->txPower = txPower;
	this->logs.Add(log);
	Optional<DevEntry> optdev;
	NN<DevEntry> dev;
	if (addrType == AT_RANDOM)
	{
		optdev = this->randDevs.Get(macInt);
	}
	else
	{
		optdev = this->pubDevs.Get(macInt);
	}
	if (!optdev.SetTo(dev))
	{
		dev = MemAllocNN(DevEntry);
		dev->macInt = macInt;
		dev->company = company;
		dev->radioType = radioType;
		dev->addrType = addrType;
		dev->measurePower = measurePower;
		dev->name = Text::String::CopyOrNull(name);
		dev->lastAdvType = advType;
		NEW_CLASSNN(dev->logs, Data::ArrayListNN<LogEntry>());
		if (addrType == AT_RANDOM)
		{
			this->randDevs.Put(macInt, dev);
		}
		else
		{
			this->pubDevs.Put(macInt, dev);
		}
	}
	NN<Text::String> nnname;
	if (dev->name.IsNull() && name.SetTo(nnname))
	{
		dev->name = nnname->Clone();
	}
	if (company && dev->company == 0)
	{
		dev->company = company;
	}
	if (advType != ADVT_UNKNOWN)
	{
		dev->lastAdvType = advType;
	}
	dev->logs->Add(log);
	return log;
}

NN<IO::BTScanLog::LogEntry> IO::BTScanLog::AddScanRec(NN<const IO::BTScanLog::ScanRecord3> rec)
{
	return this->AddEntry(rec->lastSeenTime, rec->macInt, rec->radioType, rec->addrType, rec->company, rec->name, rec->rssi, rec->txPower, rec->measurePower, rec->advType);
}

void IO::BTScanLog::AddBTRAWPacket(Int64 timeTicks, Data::ByteArrayR buff)
{
	IO::BTScanLog::ScanRecord3 rec;
	if (ParseBTRAWPacket(rec, timeTicks, buff))
	{
		this->AddScanRec(rec);
		OPTSTR_DEL(rec.name);
		return;
	}
}

void IO::BTScanLog::ClearList()
{
	this->logs.MemFreeAll();
	this->pubDevs.FreeAll(FreeDev);
	this->randDevs.FreeAll(FreeDev);
}

NN<const Data::ReadingListNN<IO::BTScanLog::DevEntry>> IO::BTScanLog::GetPublicList() const
{
	return this->pubDevs;
}

NN<const Data::ReadingListNN<IO::BTScanLog::DevEntry>> IO::BTScanLog::GetRandomList() const
{
	return this->randDevs;
}

Text::CStringNN IO::BTScanLog::RadioTypeGetName(RadioType radioType)
{
	switch (radioType)
	{
	case RT_HCI:
		return CSTR("HCI");
	case RT_LE:
		return CSTR("LE");
	case RT_UNKNOWN:
	default:
		return CSTR("UNK");
	}
}

Text::CStringNN IO::BTScanLog::AddressTypeGetName(AddressType addrType)
{
	switch (addrType)
	{
	case AT_PUBLIC:
		return CSTR("Public");
	case AT_RANDOM:
		return CSTR("Random");
	case AT_UNKNOWN:
	default:
		return CSTR("Unknown");
	}
}

Text::CStringNN IO::BTScanLog::AdvTypeGetName(AdvType advType)
{
	switch (advType)
	{
	case ADVT_IBEACON:
		return CSTR("iBeacon");
	case ADVT_FINDMY_BROADCAST:
		return CSTR("AirTag");
	case ADVT_HOMEKIT:
		return CSTR("HomeKit");
	case ADVT_AIRDROP:
		return CSTR("AirDrop");
	case ADVT_AIRPLAY_TARGET:
		return CSTR("Airplay Target");
	case ADVT_AIRPLAY_SRC:
		return CSTR("Airplay Source");
	case ADVT_AIRPRINT:
		return CSTR("AirPrint");
	case ADVT_HANDOFF:
		return CSTR("Handoff");
	case ADVT_MAGIC_SWITCH:
		return CSTR("Magic Switch");
	case ADVT_NEARBY_ACTION:
		return CSTR("Nearby Action");
	case ADVT_NEARBY_INFO:
		return CSTR("Nearby Info");
	case ADVT_PROXIMITY_PAIRING:
		return CSTR("Proximity Pairing");
	case ADVT_TETHERING_SRC:
		return CSTR("Tethering Source");
	case ADVT_TETHERING_TARGET:
		return CSTR("Teghering Target");
	case ADVT_EDDYSTONE:
		return CSTR("Eddystone");
	case ADVT_ALTBEACON:
		return CSTR("AltBeacon");
	case ADVT_UNKNOWN:
	default:
		return CSTR("Unknown");
	}
}

Bool IO::BTScanLog::ParseBTRAWPacket(NN<IO::BTScanLog::ScanRecord3> rec, Int64 timeTicks, Data::ByteArrayR buff)
{
	if (buff.GetSize() < 19)
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
	rec->advType = ADVT_UNKNOWN;
	UInt8 mac[8];
	UInt8 addrType;
	if (buff[4] == 4 && buff[5] == 0x3E) //HCI Event, LE Meta
	{
		UInt8 len = buff[6];
		if ((UOSInt)len + 7 > buff.GetSize())
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
			if (optEnd < buff.GetSize())
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

		if (optEnd > buff.GetSize())
		{
			optEnd = buff.GetSize();
		}
		ParseAdvisement(rec, buff.Arr(), i, optEnd);

		if (addrType == 0)
		{
			rec->addrType = AT_PUBLIC;
		}
		else if (addrType == 1)
		{
			rec->addrType = AT_RANDOM;
		}
		else
		{
			rec->addrType = AT_UNKNOWN;
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
		if ((UOSInt)len + 7 > buff.GetSize() || len < 15)
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
		optEnd = buff.GetSize();
		i = 22;

		ParseAdvisement(rec, buff.Arr(), i, optEnd);

		rec->addrType = AT_PUBLIC;
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

void IO::BTScanLog::ParseAdvisement(NN<ScanRecord3> rec, UnsafeArray<const UInt8> buff, UOSInt ofst, UOSInt endOfst)
{
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	UOSInt i = ofst;
	UOSInt j;

	sbuff[0] = 0;
	sptr = sbuff;
	while (i + 1 < endOfst)
	{
		UInt8 optLen = buff[i];
		if (optLen == 0 || optLen + i + 1 > endOfst)
		{
			break;
		}
		if (buff[i + 1] == 8 || buff[i + 1] == 9)
		{
			sptr = Text::StrConcatC(sbuff, &buff[i + 2], (UOSInt)optLen - 1);
		}
		else if (buff[i + 1] == 3) //UUIDs
		{
			UInt16 uuid;
			j = 2;
			while (j < optLen)
			{
				uuid = ReadUInt16(&buff[i + j]);
				if (uuid == 0xFEAA)
				{
					rec->advType = ADVT_EDDYSTONE;
				}
				j += 2;
			}
		}
		else if (buff[i + 1] == 0xFF)
		{
			rec->company = ReadUInt16(&buff[i + 2]);
			if (rec->company == 0x4C)
			{
				switch (buff[i + 4])
				{
				case 2:
					rec->advType = ADVT_IBEACON;
					if (optLen == 26)
					{
						rec->measurePower = (Int8)buff[i + 26];
					}
					break;
				case 3:
					rec->advType = ADVT_AIRPRINT;
					break;
				case 5:
					rec->advType = ADVT_AIRDROP;
					break;
				case 6:
					rec->advType = ADVT_HOMEKIT;
					break;
				case 7:
					rec->advType = ADVT_PROXIMITY_PAIRING;
					break;
				case 9:
					rec->advType = ADVT_AIRPLAY_TARGET;
					break;
				case 10:
					rec->advType = ADVT_AIRPLAY_SRC;
					break;
				case 11:
					rec->advType = ADVT_MAGIC_SWITCH;
					break;
				case 12:
					rec->advType = ADVT_HANDOFF;
					break;
				case 13:
					rec->advType = ADVT_TETHERING_TARGET;
					break;
				case 14:
					rec->advType = ADVT_TETHERING_SRC;
					break;
				case 15:
					rec->advType = ADVT_NEARBY_ACTION;
					break;
				case 16:
					rec->advType = ADVT_NEARBY_INFO;
					break;
				case 18:
					rec->advType = ADVT_FINDMY_BROADCAST;
					break;
				default:
					rec->advType = ADVT_UNKNOWN;
					break;
				}
			}
			else if (optLen == 27 && ReadMUInt16(&buff[i + 4]) == 0xBEAC)
			{
				rec->advType = ADVT_ALTBEACON;
				rec->measurePower = (Int8)buff[i + 26];
			}
		}
		i += 1 + (UOSInt)optLen;
	}
	if (sbuff[0])
	{
		rec->name = Text::String::New(sbuff, (UOSInt)(sptr - sbuff)).Ptr();
	}
	else
	{
		rec->name = 0;
	}
}
