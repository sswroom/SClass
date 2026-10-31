#include "Stdafx.h"
#include "Core/ByteTool_C.h"
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

NN<IO::BTScanLog::LogEntry> IO::BTScanLog::AddEntry64(Int64 timeTicks, UInt64 mac64Int, RadioType radioType, AddressType addrType, UInt16 company, Optional<Text::String> name, Int8 rssi, Int8 txPower, Int8 measurePower, AdvType advType)
{
	NN<LogEntry> log = MemAllocNN(LogEntry);
	log->mac64Int = mac64Int;
	log->timeTicks = timeTicks;
	log->rssi = rssi;
	log->txPower = txPower;
	this->logs.Add(log);
	Optional<DevEntry> optdev;
	NN<DevEntry> dev;
	if (addrType == AT_RANDOM)
	{
		optdev = this->randDevs.Get(mac64Int);
	}
	else
	{
		optdev = this->pubDevs.Get(mac64Int);
	}
	if (!optdev.SetTo(dev))
	{
		dev = MemAllocNN(DevEntry);
		dev->mac64Int = mac64Int;
		dev->company = company;
		dev->radioType = radioType;
		dev->addrType = addrType;
		dev->measurePower = measurePower;
		dev->name = Text::String::CopyOrNull(name);
		dev->lastAdvType = advType;
		NEW_CLASSNN(dev->logs, Data::ArrayListNN<LogEntry>());
		if (addrType == AT_RANDOM)
		{
			this->randDevs.Put(mac64Int, dev);
		}
		else
		{
			this->pubDevs.Put(mac64Int, dev);
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
	if (advType != AdvType::Unknown)
	{
		dev->lastAdvType = advType;
	}
	dev->logs->Add(log);
	return log;
}

NN<IO::BTScanLog::LogEntry> IO::BTScanLog::AddScanRec(NN<const IO::BTScanLog::ScanRecord5> rec)
{
	return this->AddEntry64(rec->lastSeenTime, rec->mac64Int, rec->radioType, rec->addrType, rec->company, rec->name, rec->rssi, rec->txPower, rec->measurePower, rec->advType);
}

void IO::BTScanLog::AddBTRAWPacket(Int64 timeTicks, Data::ByteArrayR buff)
{
	IO::BTScanLog::ScanRecord5 rec;
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
	case AdvType::IBeacon:
		return CSTR("iBeacon");
	case AdvType::FindMyBroadcast:
		return CSTR("AirTag");
	case AdvType::HomeKit:
		return CSTR("HomeKit");
	case AdvType::AirDrop:
		return CSTR("AirDrop");
	case AdvType::AirPlayTarget:
		return CSTR("Airplay Target");
	case AdvType::AirPlaySrc:
		return CSTR("Airplay Source");
	case AdvType::AirPrint:
		return CSTR("AirPrint");
	case AdvType::Handoff:
		return CSTR("Handoff");
	case AdvType::MagicSwitch:
		return CSTR("Magic Switch");
	case AdvType::NearbyAction:
		return CSTR("Nearby Action");
	case AdvType::NearbyInfo:
		return CSTR("Nearby Info");
	case AdvType::ProximityPairing:
		return CSTR("Proximity Pairing");
	case AdvType::TetheringSrc:
		return CSTR("Tethering Source");
	case AdvType::TetheringTarget:
		return CSTR("Teghering Target");
	case AdvType::Eddystone:
		return CSTR("Eddystone");
	case AdvType::AltBeacon:
		return CSTR("AltBeacon");
	case AdvType::Unsupported:
		return CSTR("Unsupported");
	case AdvType::Unknown:
	default:
		return CSTR("Unknown");
	}
}

Bool IO::BTScanLog::ParseBTRAWPacket(NN<IO::BTScanLog::ScanRecord5> rec, Int64 timeTicks, Data::ByteArrayR buff)
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
	rec->flags = 0;
	rec->advType = AdvType::Unknown;
	rec->serviceUUID = 0;
	rec->serviceDataLen = 0;
	rec->advData[0] = 0;
	UInt8 mac[8];
	UInt8 addrType;
	if (buff[4] == 4 && buff[5] == 0x3E) //HCI Event, LE Meta
	{
		UInt8 len = buff[6];
		if ((UIntOS)len + 7 > buff.GetSize())
		{
			return false;
		}
		UIntOS optEnd;
		UIntOS i;
		if (buff[7] == 0xd) //Sub Event: LE Extended Advertising Report (0x0d)
		{
			UInt8 numReports = buff[8];
			if (numReports != 1)
			{
				return false;
			}
			addrType = buff[11];
			mac[0] = buff[17];
			mac[1] = buff[16];
			mac[2] = buff[15];
			mac[3] = buff[14];
			mac[4] = buff[13];
			mac[5] = buff[12];
			mac[6] = 0;
			mac[7] = 0;
			rec->rssi = (Int8)buff[22];
			rec->txPower = (Int8)buff[21];

			optEnd = (UIntOS)buff[32] + 33;
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
			mac[0] = buff[16];
			mac[1] = buff[15];
			mac[2] = buff[14];
			mac[3] = buff[13];
			mac[4] = buff[12];
			mac[5] = buff[11];
			mac[6] = 0;
			mac[7] = 0;
			rec->txPower = 0;

			optEnd = (UIntOS)buff[17] + 18;
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
		rec->mac64Int = ReadMUInt64(mac);
		rec->mac[0] = mac[0];
		rec->mac[1] = mac[1];
		rec->mac[2] = mac[2];
		rec->mac[3] = mac[3];
		rec->mac[4] = mac[4];
		rec->mac[5] = mac[5];
		return true;
	}
	else if (buff[4] == 4 && buff[5] == 0x2F) //HCI Event, Extended Inquiry Result
	{
		UInt8 len = buff[6];
		if ((UIntOS)len + 7 > buff.GetSize() || len < 15)
		{
			return false;
		}
		rec->rssi = (Int8)buff[21];
		UIntOS optEnd;
		UIntOS i;
		UInt8 numReports = buff[7];
		if (numReports != 1)
		{
			return false;
		}
		mac[0] = buff[13];
		mac[1] = buff[12];
		mac[2] = buff[11];
		mac[3] = buff[10];
		mac[4] = buff[9];
		mac[5] = buff[8];
		mac[6] = 0;
		mac[7] = 0;
		optEnd = buff.GetSize();
		i = 22;

		ParseAdvisement(rec, buff.Arr(), i, optEnd);

		rec->addrType = AT_PUBLIC;
		rec->txPower = 0;
		rec->radioType = RT_HCI;
		rec->mac64Int = ReadMUInt64(mac);
		rec->mac[0] = mac[0];
		rec->mac[1] = mac[1];
		rec->mac[2] = mac[2];
		rec->mac[3] = mac[3];
		rec->mac[4] = mac[4];
		rec->mac[5] = mac[5];
		return true;
	}
	return false;
}

void IO::BTScanLog::ParseAdvisement(NN<ScanRecord5> rec, UnsafeArray<const UInt8> buff, UIntOS ofst, UIntOS endOfst)
{
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	UIntOS i = ofst;
	UIntOS j;

	sbuff[0] = 0;
	sptr = sbuff;
	while (i + 1 < endOfst)
	{
		UInt8 optLen = buff[i];
		if (optLen == 0 || optLen + i + 1 > endOfst)
		{
			break;
		}
		if (buff[i + 1] == 1) // Flags
		{
			// 0 LE Limited Discoverable Mode
			// 1 LE General Discoverable Mode
			// 2 BR/EDR Not Supported
			// 3 Simultaneous LE and BR/EDR to Same Device Capable (Controller)
			// 4 Simultaneous LE and BR/EDR to Same Device Capable (Host)
			rec->flags = buff[i + 2];
		}
		else if (buff[i + 1] == 3) //UUIDs
		{
			UInt16 uuid;
			j = 2;
			while (j < optLen)
			{
				uuid = ReadLUInt16(&buff[i + j]);
				if (uuid == 0xFEAA)
				{
					rec->advType = AdvType::Eddystone;
				}
				j += 2;
			}
		}
		else if (buff[i + 1] == 8 || buff[i + 1] == 9) // Shortened Local Name / Complete Local Name
		{
			sptr = Text::StrConcatC(sbuff, &buff[i + 2], (UIntOS)optLen - 1);
		}
		else if (buff[i + 1] == 0x16) // Service Data
		{
			rec->serviceUUID = ReadLUInt16(&buff[i + 2]);
			rec->serviceDataLen = (UInt8)(optLen - 3);
			MemCopyNO(rec->serviceData, &buff[i + 4], rec->serviceDataLen);
		}
		else if (buff[i + 1] == 0xFF) // Manufacturer-specific data
		{
			rec->company = ReadLUInt16(&buff[i + 2]);
			if (rec->company == 0x4C)
			{
				switch (buff[i + 4])
				{
				case 2:
					rec->advType = AdvType::IBeacon;
					if (optLen == 26)
					{
						rec->measurePower = (Int8)buff[i + 26];
					}
					break;
				case 3:
					rec->advType = AdvType::AirPrint;
					break;
				case 5:
					rec->advType = AdvType::AirDrop;
					break;
				case 6:
					rec->advType = AdvType::HomeKit;
					break;
				case 7:
					rec->advType = AdvType::ProximityPairing;
					break;
				case 9:
					rec->advType = AdvType::AirPlayTarget;
					break;
				case 10:
					rec->advType = AdvType::AirPlaySrc;
					break;
				case 11:
					rec->advType = AdvType::MagicSwitch;
					break;
				case 12:
					rec->advType = AdvType::Handoff;
					break;
				case 13:
					rec->advType = AdvType::TetheringTarget;
					break;
				case 14:
					rec->advType = AdvType::TetheringSrc;
					break;
				case 15:
					rec->advType = AdvType::NearbyAction;
					break;
				case 16:
					rec->advType = AdvType::NearbyInfo;
					break;
				case 18:
					rec->advType = AdvType::FindMyBroadcast;
					break;
				default:
					rec->advType = AdvType::Unknown;
					break;
				}
			}
			else if (optLen == 27 && ReadMUInt16(&buff[i + 4]) == 0xBEAC)
			{
				rec->advType = AdvType::AltBeacon;
				rec->measurePower = (Int8)buff[i + 26];
			}
			if (rec->advType == AdvType::Unknown)
			{
				rec->advType = AdvType::Unsupported;
			}
			MemCopyNO(rec->advData, &buff[i], (UIntOS)optLen + 1);
		}
		else
		{
			printf("BTScanLog: Unknown AD Type 0x%02X\n", buff[i + 1]);
		}
		i += 1 + (UIntOS)optLen;
	}
	if (sbuff[0])
	{
		rec->name = Text::String::New(sbuff, (UIntOS)(sptr - sbuff)).Ptr();
	}
	else
	{
		rec->name = nullptr;
	}
}
