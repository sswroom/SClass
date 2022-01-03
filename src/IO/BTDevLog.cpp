#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "IO/BTDevLog.h"
#include "IO/FileStream.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Reader.h"
#include "Text/UTF8Writer.h"

Bool IO::BTDevLog::IsDefaultName(const UTF8Char *name)
{
	if (Text::StrCharCnt(name) == 17)
	{
		if (name[2] == '-' &&
			name[5] == '-' &&
			name[8] == '-' &&
			name[11] == '-' &&
			name[14] == '-')
		{
			return true;
		}
	}
	return false;
}

void IO::BTDevLog::FreeDev(DevEntry *dev)
{
	SDEL_TEXT(dev->name);
	MemFree(dev);
}

IO::BTDevLog::BTDevLog()
{
	NEW_CLASS(this->pubDevs, Data::UInt64Map<DevEntry*>());
	NEW_CLASS(this->randDevs, Data::UInt64Map<DevEntry*>());
}

IO::BTDevLog::~BTDevLog()
{
	this->ClearList();
	DEL_CLASS(this->pubDevs);
	DEL_CLASS(this->randDevs);
}

IO::BTDevLog::DevEntry *IO::BTDevLog::AddEntry(UInt64 macInt, const UTF8Char *name, Int8 txPower, Int8 measurePower, IO::BTScanLog::RadioType radioType, IO::BTScanLog::AddressType addrType, UInt16 company, IO::BTScanLog::AdvType advType)
{
	UInt8 mac[8];
	DevEntry *log;
	if (addrType == IO::BTScanLog::AT_RANDOM)
	{
		log = this->randDevs->Get(macInt);
	}
	else
	{
		log = this->pubDevs->Get(macInt);
	}
	if (log)
	{
		if (advType != IO::BTScanLog::ADVT_UNKNOWN)
		{
			log->advType = advType;
		}
		if (log->txPower == 0 || txPower != 0)
		{
			log->txPower = txPower;
		}
		if (log->company == 0 && company != 0)
		{
			log->company = company;
		}
		if (log->measurePower == 0 && measurePower != 0)
		{
			log->measurePower = measurePower;
		}
		if (log->name == 0 && name != 0)
		{
			log->name = Text::StrCopyNew(name);
		}
		else if (log->name != 0 && name != 0 && IsDefaultName(log->name) && !IsDefaultName(name))
		{
			Text::StrDelNew(log->name);
			log->name = Text::StrCopyNew(name);
		}
		return log;
	}
	WriteMUInt64(mac, macInt);
	log = MemAlloc(DevEntry, 1);
	log->mac[0] = mac[2];
	log->mac[1] = mac[3];
	log->mac[2] = mac[4];
	log->mac[3] = mac[5];
	log->mac[4] = mac[6];
	log->mac[5] = mac[7];
	log->macInt = macInt;
	log->name = SCOPY_TEXT(name);
	log->radioType = radioType;
	log->addrType = addrType;
	log->txPower = txPower;
	log->measurePower = measurePower;
	log->company = company;
	log->advType = advType;
	if (addrType == IO::BTScanLog::AT_RANDOM)
	{
		this->randDevs->Put(macInt, log);
	}
	else
	{
		this->pubDevs->Put(macInt, log);
	}
	return log;
}

void IO::BTDevLog::AppendList(Data::UInt64Map<IO::BTScanLog::ScanRecord3*> *devMap)
{
	IO::BTScanLog::ScanRecord3 *rec;
	Data::ArrayList<IO::BTScanLog::ScanRecord3*> *recList = devMap->GetValues();
	UOSInt i = recList->GetCount();
	while (i-- > 0)
	{
		rec = recList->GetItem(i);
		this->AddEntry(rec->macInt, rec->name, rec->txPower, rec->measurePower, rec->radioType, rec->addrType, rec->company, rec->advType);
	}
}

void IO::BTDevLog::ClearList()
{
	Data::ArrayList<DevEntry*> *logList;
	logList = this->randDevs->GetValues();
	LIST_FREE_FUNC(logList, this->FreeDev);
	logList = this->pubDevs->GetValues();
	LIST_FREE_FUNC(logList, this->FreeDev);
	this->randDevs->Clear();
	this->pubDevs->Clear();
}

Bool IO::BTDevLog::LoadFile(const UTF8Char *fileName)
{
	Text::StringBuilderUTF8 sb;
	IO::FileStream *fs;
	Text::UTF8Reader *reader;
	UTF8Char *sarr[9];
	UOSInt colCnt;
	UInt8 macBuff[8];
	UInt64 macInt;
	UInt32 advType;
	NEW_CLASS(fs, IO::FileStream(fileName, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	if (fs->IsError())
	{
		DEL_CLASS(fs);
		return false;
	}
	NEW_CLASS(reader, Text::UTF8Reader(fs));
	while (reader->ReadLine(&sb, 512))
	{
		colCnt = Text::StrSplit(sarr, 9, sb.ToString(), '\t');
		if ((colCnt == 4 || colCnt == 6 || colCnt == 7 || colCnt == 8) && Text::StrCharCnt(sarr[0]) == 17)
		{
			macBuff[0] = 0;
			macBuff[1] = 0;
			macBuff[2] = Text::StrHex2UInt8C(&sarr[0][0]);
			macBuff[3] = Text::StrHex2UInt8C(&sarr[0][3]);
			macBuff[4] = Text::StrHex2UInt8C(&sarr[0][6]);
			macBuff[5] = Text::StrHex2UInt8C(&sarr[0][9]);
			macBuff[6] = Text::StrHex2UInt8C(&sarr[0][12]);
			macBuff[7] = Text::StrHex2UInt8C(&sarr[0][15]);
			macInt = ReadMUInt64(macBuff);
			const UTF8Char *name = sarr[1];
			if (name[0] == 0)
			{
				name = 0;
			}
			IO::BTScanLog::RadioType radioType = IO::BTScanLog::RT_UNKNOWN;
			IO::BTScanLog::AddressType addrType = IO::BTScanLog::AT_UNKNOWN;
			Int8 measurePower = 0;
			advType = 0;
			if (colCnt >= 6)
			{
				if (Text::StrEquals(sarr[4], (const UTF8Char*)"HCI"))
				{
					radioType = IO::BTScanLog::RT_HCI;
				}
				else if (Text::StrEquals(sarr[4], (const UTF8Char*)"LE"))
				{
					radioType = IO::BTScanLog::RT_LE;
				}

				if (Text::StrEquals(sarr[5], (const UTF8Char*)"Public"))
				{
					addrType = IO::BTScanLog::AT_PUBLIC;
				}
				else if (Text::StrEquals(sarr[5], (const UTF8Char*)"Random"))
				{
					addrType = IO::BTScanLog::AT_RANDOM;
				}
			}
			if (colCnt >= 7)
			{
				measurePower = (Int8)Text::StrToInt32(sarr[6]);
			}
			if (colCnt >= 8)
			{
				advType = Text::StrToUInt32(sarr[7]);
			}
			UInt16 company = 0;
			if (sarr[3][0])
			{
				sarr[1] = sarr[3];
				UOSInt i = 2;
				while (i == 2)
				{
					i = Text::StrSplit(sarr, 2, sarr[1], ',');
					company = Text::StrHex2UInt16C(sarr[0]);
				}
			}
			this->AddEntry(macInt, name, (Int8)Text::StrToInt32(sarr[2]), measurePower, radioType, addrType, company, (IO::BTScanLog::AdvType)advType);
		}
		sb.ClearStr();
	}
	DEL_CLASS(reader);
	DEL_CLASS(fs);
	return true;
}

Bool IO::BTDevLog::StoreFile(const UTF8Char *fileName)
{
	Text::StringBuilderUTF8 sb;
	IO::FileStream *fs;
	Text::UTF8Writer *writer;
	NEW_CLASS(fs, IO::FileStream(fileName, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	if (fs->IsError())
	{
		DEL_CLASS(fs);
		return false;
	}
	NEW_CLASS(writer, Text::UTF8Writer(fs));
	Data::ArrayList<DevEntry*> logList;
	logList.AddAll(this->pubDevs->GetValues());
	logList.AddAll(this->randDevs->GetValues());
	DevEntry *log;
	UOSInt i = 0;
	UOSInt j = logList.GetCount();
	while (i < j)
	{
		log = logList.GetItem(i);
		sb.ClearStr();
		sb.AppendHexBuff(log->mac, 6, ':', Text::LineBreakType::None);
		sb.AppendChar('\t', 1);
		if (log->name)
		{
			sb.Append(log->name);
		}
		sb.AppendChar('\t', 1);
		sb.AppendI32(log->txPower);
		sb.AppendChar('\t', 1);
		sb.AppendHex16(log->company);
		sb.AppendChar('\t', 1);
		switch (log->radioType)
		{
		case IO::BTScanLog::RT_HCI:
			sb.AppendC(UTF8STRC("HCI"));
			break;
		case IO::BTScanLog::RT_LE:
			sb.AppendC(UTF8STRC("LE"));
			break;
		case IO::BTScanLog::RT_UNKNOWN:
		default:
			sb.AppendC(UTF8STRC("UNK"));
			break;
		}
		sb.AppendChar('\t', 1);
		switch (log->addrType)
		{
		case IO::BTScanLog::AT_PUBLIC:
			sb.AppendC(UTF8STRC("Public"));
			break;
		case IO::BTScanLog::AT_RANDOM:
			sb.AppendC(UTF8STRC("Random"));
			break;
		case IO::BTScanLog::RT_UNKNOWN:
		default:
			sb.AppendC(UTF8STRC("Unknown"));
			break;
		}
		sb.AppendChar('\t', 1);
		sb.AppendI32(log->measurePower);
		sb.AppendChar('\t', 1);
		sb.AppendU32((UInt32)log->advType);
		writer->WriteLine(sb.ToString());
		i++;
	}
	DEL_CLASS(writer);
	DEL_CLASS(fs);
	return true;
}

Data::ArrayList<IO::BTDevLog::DevEntry*> *IO::BTDevLog::GetPublicList()
{
	return this->pubDevs->GetValues();
}

Data::ArrayList<IO::BTDevLog::DevEntry*> *IO::BTDevLog::GetRandomList()
{
	return this->randDevs->GetValues();
}
