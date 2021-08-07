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

IO::BTDevLog::BTDevLog()
{
	NEW_CLASS(this->logs, Data::UInt64Map<DevEntry2*>());
}

IO::BTDevLog::~BTDevLog()
{
	this->ClearList();
	DEL_CLASS(this->logs);
}

IO::BTDevLog::DevEntry2 *IO::BTDevLog::AddEntry(UInt64 macInt, const UTF8Char *name, Int8 txPower, IO::BTScanLog::RadioType radioType, IO::BTScanLog::AddressType addrType, UInt16 company)
{
	UInt8 mac[8];
	DevEntry2 *log = this->logs->Get(macInt);
	if (log)
	{
		if (log->txPower == 0 || txPower != 0)
		{
			log->txPower = txPower;
		}
		if (log->company == 0 && company != 0)
		{
			log->company = company;
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
	log = MemAlloc(DevEntry2, 1);
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
	log->company = company;
	this->logs->Put(macInt, log);
	return log;
}

void IO::BTDevLog::AppendList(Data::UInt64Map<IO::BTScanLog::ScanRecord*> *devMap)
{
	IO::BTScanLog::ScanRecord *rec;
	Data::ArrayList<IO::BTScanLog::ScanRecord*> *recList = devMap->GetValues();
	UOSInt i = recList->GetCount();
	while (i-- > 0)
	{
		rec = recList->GetItem(i);
		this->AddEntry(rec->macInt, rec->name, rec->txPower, rec->radioType, rec->addrType, rec->company);
	}
}

void IO::BTDevLog::ClearList()
{
	DevEntry2 *log;
	Data::ArrayList<DevEntry2*> *logList = this->logs->GetValues();
	UOSInt i = logList->GetCount();
	while (i-- > 0)
	{
		log = logList->GetItem(i);
		SDEL_TEXT(log->name);
		MemFree(log);
	}
	this->logs->Clear();
}

Bool IO::BTDevLog::LoadFile(const UTF8Char *fileName)
{
	Text::StringBuilderUTF8 sb;
	IO::FileStream *fs;
	Text::UTF8Reader *reader;
	UTF8Char *sarr[7];
	UOSInt colCnt;
	UInt8 macBuff[8];
	UInt64 macInt;
	NEW_CLASS(fs, IO::FileStream(fileName, IO::FileStream::FILE_MODE_READONLY, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
	if (fs->IsError())
	{
		DEL_CLASS(fs);
		return false;
	}
	NEW_CLASS(reader, Text::UTF8Reader(fs));
	while (reader->ReadLine(&sb, 512))
	{
		colCnt = Text::StrSplit(sarr, 7, sb.ToString(), '\t');
		if ((colCnt == 4 || colCnt == 6) && Text::StrCharCnt(sarr[0]) == 17)
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
			this->AddEntry(macInt, name, (Int8)Text::StrToInt32(sarr[2]), radioType, addrType, company);
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
	NEW_CLASS(fs, IO::FileStream(fileName, IO::FileStream::FILE_MODE_CREATE, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
	if (fs->IsError())
	{
		DEL_CLASS(fs);
		return false;
	}
	NEW_CLASS(writer, Text::UTF8Writer(fs));
	Data::ArrayList<DevEntry2*> *logList = this->logs->GetValues();
	DevEntry2 *log;
	UOSInt i = 0;
	UOSInt j = logList->GetCount();
	while (i < j)
	{
		log = logList->GetItem(i);
		sb.ClearStr();
		sb.AppendHexBuff(log->mac, 6, ':', Text::LBT_NONE);
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
			sb.Append((const UTF8Char*)"HCI");
			break;
		case IO::BTScanLog::RT_LE:
			sb.Append((const UTF8Char*)"LE");
			break;
		case IO::BTScanLog::RT_UNKNOWN:
		default:
			sb.Append((const UTF8Char*)"UNK");
			break;
		}
		sb.AppendChar('\t', 1);
		switch (log->addrType)
		{
		case IO::BTScanLog::AT_PUBLIC:
			sb.Append((const UTF8Char*)"Public");
			break;
		case IO::BTScanLog::AT_RANDOM:
			sb.Append((const UTF8Char*)"Random");
			break;
		case IO::BTScanLog::RT_UNKNOWN:
		default:
			sb.Append((const UTF8Char*)"Unknown");
			break;
		}
		writer->WriteLine(sb.ToString());
		i++;
	}
	DEL_CLASS(writer);
	DEL_CLASS(fs);
	return true;
}

Data::ArrayList<IO::BTDevLog::DevEntry2*> *IO::BTDevLog::GetLogList()
{
	return this->logs->GetValues();
}
