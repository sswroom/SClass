#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "IO/BTLog.h"
#include "IO/FileStream.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Writer.h"

IO::BTLog::BTLog()
{
	NEW_CLASS(this->logs, Data::UInt64Map<LogEntry*>());
}

IO::BTLog::~BTLog()
{
	this->ClearList();
	DEL_CLASS(this->logs);
}

IO::BTLog::LogEntry *IO::BTLog::AddEntry(UInt64 macInt, const UTF8Char *name, Int32 txPower)
{
	UInt8 mac[8];
	LogEntry *log = this->logs->Get(macInt);
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
		return log;
	}
	WriteMUInt64(mac, macInt);
	log = MemAlloc(LogEntry, 1);
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

void IO::BTLog::AppendList(Data::UInt64Map<IO::ProgCtrl::BluetoothCtlProgCtrl::DeviceInfo*> *devMap)
{
	IO::ProgCtrl::BluetoothCtlProgCtrl::DeviceInfo *dev;
	LogEntry *log;
	UOSInt j;
	UOSInt k;
	Data::ArrayList<IO::ProgCtrl::BluetoothCtlProgCtrl::DeviceInfo*> *devList = devMap->GetValues();
	UOSInt i = devList->GetCount();
	while (i-- > 0)
	{
		dev = devList->GetItem(i);
		log = this->AddEntry(dev->macInt, dev->name, dev->txPower);
		j = 0;
		k = dev->keys->GetCount();
		while (j < k)
		{
			if (log->keys->SortedIndexOf(dev->keys->GetItem(j)) < 0)
			{
				log->keys->SortedInsert(dev->keys->GetItem(j));
			}
			j++;
		}
	}
}

void IO::BTLog::ClearList()
{
	LogEntry *log;
	Data::ArrayList<LogEntry*> *logList = this->logs->GetValues();
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

Bool IO::BTLog::Store(const UTF8Char *fileName)
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
	Data::ArrayList<LogEntry*> *logList = this->logs->GetValues();
	LogEntry *log;
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
