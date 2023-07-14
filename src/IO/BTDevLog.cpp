#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "IO/BTDevLog.h"
#include "IO/FileStream.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Reader.h"
#include "Text/UTF8Writer.h"

Bool IO::BTDevLog::IsDefaultName(Text::String *name)
{
	if (name->leng == 17)
	{
		if (name->v[2] == '-' &&
			name->v[5] == '-' &&
			name->v[8] == '-' &&
			name->v[11] == '-' &&
			name->v[14] == '-')
		{
			return true;
		}
	}
	return false;
}

void IO::BTDevLog::FreeDev(DevEntry *dev)
{
	SDEL_STRING(dev->name);
	MemFree(dev);
}

IO::BTDevLog::BTDevLog()
{
}

IO::BTDevLog::~BTDevLog()
{
	this->ClearList();
}

IO::BTDevLog::DevEntry *IO::BTDevLog::AddEntry(UInt64 macInt, Text::String *name, Int8 txPower, Int8 measurePower, IO::BTScanLog::RadioType radioType, IO::BTScanLog::AddressType addrType, UInt16 company, IO::BTScanLog::AdvType advType)
{
	UInt8 mac[8];
	DevEntry *log;
	if (addrType == IO::BTScanLog::AT_RANDOM)
	{
		log = this->randDevs.Get(macInt);
	}
	else
	{
		log = this->pubDevs.Get(macInt);
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
			log->name = name->Clone().Ptr();
		}
		else if (log->name != 0 && name != 0 && IsDefaultName(log->name) && !IsDefaultName(name))
		{
			log->name->Release();
			log->name = name->Clone().Ptr();
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
	log->name = SCOPY_STRING(name);
	log->radioType = radioType;
	log->addrType = addrType;
	log->txPower = txPower;
	log->measurePower = measurePower;
	log->company = company;
	log->advType = advType;
	if (addrType == IO::BTScanLog::AT_RANDOM)
	{
		this->randDevs.Put(macInt, log);
	}
	else
	{
		this->pubDevs.Put(macInt, log);
	}
	return log;
}

void IO::BTDevLog::AppendList(Data::FastMap<UInt64, IO::BTScanLog::ScanRecord3*> *devMap)
{
	IO::BTScanLog::ScanRecord3 *rec;
	UOSInt i = devMap->GetCount();
	while (i-- > 0)
	{
		rec = devMap->GetItem(i);
		this->AddEntry(rec->macInt, rec->name, rec->txPower, rec->measurePower, rec->radioType, rec->addrType, rec->company, rec->advType);
	}
}

void IO::BTDevLog::ClearList()
{
	LIST_CALL_FUNC(&this->randDevs, this->FreeDev);
	LIST_CALL_FUNC(&this->pubDevs, this->FreeDev);
	this->randDevs.Clear();
	this->pubDevs.Clear();
}

Bool IO::BTDevLog::LoadFile(Text::CString fileName)
{
	Text::StringBuilderUTF8 sb;
	Text::PString sarr[9];
	UOSInt colCnt;
	UInt8 macBuff[8];
	UInt64 macInt;
	UInt32 advType;
	IO::FileStream fs(fileName, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	if (fs.IsError())
	{
		return false;
	}
	Text::UTF8Reader reader(fs);
	while (reader.ReadLine(sb, 512))
	{
		colCnt = Text::StrSplitP(sarr, 9, sb, '\t');
		if ((colCnt == 4 || colCnt == 6 || colCnt == 7 || colCnt == 8) && sarr[0].leng == 17)
		{
			macBuff[0] = 0;
			macBuff[1] = 0;
			macBuff[2] = Text::StrHex2UInt8C(&sarr[0].v[0]);
			macBuff[3] = Text::StrHex2UInt8C(&sarr[0].v[3]);
			macBuff[4] = Text::StrHex2UInt8C(&sarr[0].v[6]);
			macBuff[5] = Text::StrHex2UInt8C(&sarr[0].v[9]);
			macBuff[6] = Text::StrHex2UInt8C(&sarr[0].v[12]);
			macBuff[7] = Text::StrHex2UInt8C(&sarr[0].v[15]);
			macInt = ReadMUInt64(macBuff);
			Text::CString name = sarr[1].ToCString();
			if (name.leng == 0)
			{
				name = CSTR_NULL;
			}
			IO::BTScanLog::RadioType radioType = IO::BTScanLog::RT_UNKNOWN;
			IO::BTScanLog::AddressType addrType = IO::BTScanLog::AT_UNKNOWN;
			Int8 measurePower = 0;
			advType = 0;
			if (colCnt >= 6)
			{
				if (sarr[4].Equals(UTF8STRC("HCI")))
				{
					radioType = IO::BTScanLog::RT_HCI;
				}
				else if (sarr[4].Equals(UTF8STRC("LE")))
				{
					radioType = IO::BTScanLog::RT_LE;
				}

				if (sarr[5].Equals(UTF8STRC("Public")))
				{
					addrType = IO::BTScanLog::AT_PUBLIC;
				}
				else if (sarr[5].Equals(UTF8STRC("Random")))
				{
					addrType = IO::BTScanLog::AT_RANDOM;
				}
			}
			if (colCnt >= 7)
			{
				measurePower = (Int8)Text::StrToInt32(sarr[6].v);
			}
			if (colCnt >= 8)
			{
				advType = Text::StrToUInt32(sarr[7].v);
			}
			UInt16 company = 0;
			if (sarr[3].v[0])
			{
				sarr[1] = sarr[3];
				UOSInt i = 2;
				while (i == 2)
				{
					i = Text::StrSplitP(sarr, 2, sarr[1], ',');
					company = Text::StrHex2UInt16C(sarr[0].v);
				}
			}
			Text::String *nameStr = Text::String::NewOrNull(name);
			this->AddEntry(macInt, nameStr, (Int8)Text::StrToInt32(sarr[2].v), measurePower, radioType, addrType, company, (IO::BTScanLog::AdvType)advType);
			SDEL_STRING(nameStr);
		}
		sb.ClearStr();
	}
	return true;
}

Bool IO::BTDevLog::StoreFile(Text::CString fileName)
{
	Text::StringBuilderUTF8 sb;
	IO::FileStream fs(fileName, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	if (fs.IsError())
	{
		return false;
	}
	Text::UTF8Writer writer(fs);
	Data::ArrayList<DevEntry*> logList;
	logList.AddAll(&this->pubDevs);
	logList.AddAll(&this->randDevs);
	DevEntry *log;
	UOSInt i = 0;
	UOSInt j = logList.GetCount();
	while (i < j)
	{
		log = logList.GetItem(i);
		sb.ClearStr();
		sb.AppendHexBuff(log->mac, 6, ':', Text::LineBreakType::None);
		sb.AppendUTF8Char('\t');
		if (log->name)
		{
			sb.Append(log->name);
		}
		sb.AppendUTF8Char('\t');
		sb.AppendI32(log->txPower);
		sb.AppendUTF8Char('\t');
		sb.AppendHex16(log->company);
		sb.AppendUTF8Char('\t');
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
		sb.AppendUTF8Char('\t');
		switch (log->addrType)
		{
		case IO::BTScanLog::AT_PUBLIC:
			sb.AppendC(UTF8STRC("Public"));
			break;
		case IO::BTScanLog::AT_RANDOM:
			sb.AppendC(UTF8STRC("Random"));
			break;
		case IO::BTScanLog::AT_UNKNOWN:
		default:
			sb.AppendC(UTF8STRC("Unknown"));
			break;
		}
		sb.AppendUTF8Char('\t');
		sb.AppendI32(log->measurePower);
		sb.AppendUTF8Char('\t');
		sb.AppendU32((UInt32)log->advType);
		writer.WriteLineC(sb.ToString(), sb.GetLength());
		i++;
	}
	return true;
}

const Data::ReadingList<IO::BTDevLog::DevEntry*> *IO::BTDevLog::GetPublicList() const
{
	return &this->pubDevs;
}

const Data::ReadingList<IO::BTDevLog::DevEntry*> *IO::BTDevLog::GetRandomList() const
{
	return &this->randDevs;
}
