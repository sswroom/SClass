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
	NEW_CLASS(this->logs, Data::UInt64Map<DevEntry*>());
}

IO::BTDevLog::~BTDevLog()
{
	this->ClearList();
	DEL_CLASS(this->logs);
}

IO::BTDevLog::DevEntry *IO::BTDevLog::AddEntry(UInt64 macInt, const UTF8Char *name, Int32 txPower)
{
	UInt8 mac[8];
	DevEntry *log = this->logs->Get(macInt);
	if (log)
	{
		if (log->txPower == 0 || txPower != 0)
		{
			log->txPower = txPower;
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
	log->txPower = txPower;
	NEW_CLASS(log->keys, Data::ArrayListUInt32());
	this->logs->Put(macInt, log);
	return log;
}

void IO::BTDevLog::AppendList(Data::UInt64Map<IO::BTScanner::ScanRecord*> *devMap)
{
	IO::BTScanner::ScanRecord *rec;
	DevEntry *log;
	UOSInt j;
	UOSInt k;
	Data::ArrayList<IO::BTScanner::ScanRecord*> *recList = devMap->GetValues();
	UOSInt i = recList->GetCount();
	while (i-- > 0)
	{
		rec = recList->GetItem(i);
		log = this->AddEntry(rec->macInt, rec->name, rec->txPower);
		j = 0;
		k = rec->keys->GetCount();
		while (j < k)
		{
			if (log->keys->SortedIndexOf(rec->keys->GetItem(j)) < 0)
			{
				log->keys->SortedInsert(rec->keys->GetItem(j));
			}
			j++;
		}
	}
}

void IO::BTDevLog::ClearList()
{
	DevEntry *log;
	Data::ArrayList<DevEntry*> *logList = this->logs->GetValues();
	UOSInt i = logList->GetCount();
	while (i-- > 0)
	{
		log = logList->GetItem(i);
		SDEL_TEXT(log->name);
		DEL_CLASS(log->keys);
		MemFree(log);
	}
	this->logs->Clear();
}

Bool IO::BTDevLog::LoadFile(const UTF8Char *fileName)
{
	Text::StringBuilderUTF8 sb;
	IO::FileStream *fs;
	Text::UTF8Reader *reader;
	UTF8Char *sarr[5];
	UInt8 macBuff[8];
	UInt64 macInt;
	DevEntry *log;
	NEW_CLASS(fs, IO::FileStream(fileName, IO::FileStream::FILE_MODE_READONLY, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
	if (fs->IsError())
	{
		DEL_CLASS(fs);
		return false;
	}
	NEW_CLASS(reader, Text::UTF8Reader(fs));
	while (reader->ReadLine(&sb, 512))
	{
		if (Text::StrSplit(sarr, 4, sb.ToString(), '\t') == 4 && Text::StrCharCnt(sarr[0]) == 17)
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
			log = this->AddEntry(macInt, name, Text::StrToInt32(sarr[2]));
			if (sarr[3][0])
			{
				sarr[1] = sarr[3];
				UOSInt i = 2;
				UInt16 key;
				while (i == 2)
				{
					i = Text::StrSplit(sarr, 2, sarr[1], ',');
					key = (UInt16)Text::StrHex2Int16C(sarr[0]);
					if (log->keys->SortedIndexOf(key) < 0)
					{
						log->keys->SortedInsert(key);
					}
				}
			}
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
	Data::ArrayList<DevEntry*> *logList = this->logs->GetValues();
	DevEntry *log;
	UOSInt k;
	UOSInt l;
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
		k = 0;
		l = log->keys->GetCount();
		while (k < l)
		{
			if (k > 0)
			{
				sb.AppendChar(',', 1);
			}
			sb.AppendHex16((UInt16)log->keys->GetItem(k));
			k++;
		}
		writer->WriteLine(sb.ToString());
		i++;
	}
	DEL_CLASS(writer);
	DEL_CLASS(fs);
	return true;
}

Data::ArrayList<IO::BTDevLog::DevEntry*> *IO::BTDevLog::GetLogList()
{
	return this->logs->GetValues();
}
