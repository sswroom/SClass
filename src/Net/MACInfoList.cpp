#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "IO/BufferedOutputStream.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "Net/MACInfoList.h"
#include "Text/CPPText.h"
#include "Text/MyString.h"
#include "Text/UTF8Reader.h"
#include "Text/UTF8Writer.h"

Net::MACInfoList::MACInfoList()
{
	this->modified = false;
	NEW_CLASS(this->dataList, Data::ArrayList<Net::MACInfo::MACEntry*>());
	UOSInt cnt;
	Net::MACInfo::MACEntry *ents = Net::MACInfo::GetMACEntryList(&cnt);
	Net::MACInfo::MACEntry *entry;
	UOSInt i = 0;
	while (i < cnt)
	{
		entry = MemAlloc(Net::MACInfo::MACEntry, 1);
		entry->rangeStart = ents[i].rangeStart;
		entry->rangeEnd = ents[i].rangeEnd;
		entry->name = Text::StrCopyNew(ents[i].name);
		this->dataList->Add(entry);
		i++;
	}
	this->Load();
}

Net::MACInfoList::~MACInfoList()
{
	if (this->modified)
	{
		this->Store();
	}

	UOSInt i;
	Net::MACInfo::MACEntry *entry;
	i = this->dataList->GetCount();
	while (i-- > 0)
	{
		entry = this->dataList->GetItem(i);
		SDEL_TEXT(entry->name);
		MemFree(entry);
	}
	DEL_CLASS(this->dataList);
}

UOSInt Net::MACInfoList::GetCount()
{
	return this->dataList->GetCount();
}

const Net::MACInfo::MACEntry *Net::MACInfoList::GetItem(UOSInt index)
{
	return this->dataList->GetItem(index);
}

OSInt Net::MACInfoList::GetIndex(UInt64 macInt)
{
	OSInt i;
	OSInt j;
	OSInt k;
	Net::MACInfo::MACEntry *entry;
	i = 0;
	j = (OSInt)this->dataList->GetCount() - 1;
	while (i <= j)
	{
		k = (i + j) >> 1;
		entry = this->dataList->GetItem((UOSInt)k);
		if (entry->rangeStart > macInt)
		{
			j = k - 1;
		}
		else if (entry->rangeEnd < macInt)
		{
			i = k + 1;
		}
		else
		{
			return k;
		}
	}
	return ~i;
}

const Net::MACInfo::MACEntry *Net::MACInfoList::GetEntry(UInt64 macInt)
{
	OSInt si = this->GetIndex(macInt);
	if (si >= 0)
	{
		return this->dataList->GetItem((UOSInt)si);
	}
	return 0;
}

const Net::MACInfo::MACEntry *Net::MACInfoList::GetEntryOUI(const UInt8 *oui)
{
	UInt8 macBuff[8];
	macBuff[0] = 0;
	macBuff[1] = 0;
	macBuff[2] = oui[0];
	macBuff[3] = oui[1];
	macBuff[4] = oui[2];
	macBuff[5] = 0;
	macBuff[6] = 0;
	macBuff[7] = 0;
	OSInt si = this->GetIndex(ReadMUInt64(macBuff));
	if (si >= 0)
	{
		return this->dataList->GetItem((UOSInt)si);
	}
	return 0;
}

UOSInt Net::MACInfoList::SetEntry(UInt64 macInt, const UTF8Char *name)
{
	UInt64 mask = 0xffffff;
	return SetEntry(macInt & ~mask, macInt | mask, name);
}

UOSInt Net::MACInfoList::SetEntry(UInt64 rangeStart, UInt64 rangeEnd, const UTF8Char *name)
{
	Net::MACInfo::MACEntry *entry;
	this->modified = true;
	OSInt si = this->GetIndex(rangeStart);
	if (si >= 0)
	{
		entry = this->dataList->GetItem((UOSInt)si);
		SDEL_TEXT(entry->name);
		entry->name = (const Char*)Text::StrCopyNew(name);
		return (UOSInt)si;
	}
	else
	{
		entry = MemAlloc(Net::MACInfo::MACEntry, 1);
		entry->rangeStart = rangeStart;
		entry->rangeEnd = rangeEnd;
		entry->name = (const Char*)Text::StrCopyNew(name);
		this->dataList->Insert((UOSInt)~si, entry);
		return (UOSInt)~si;
	}
}

void Net::MACInfoList::Load()
{
	UTF8Char sbuff[512];
	this->modified = false;
	IO::Path::GetProcessFileName(sbuff);
	IO::Path::AppendPath(sbuff, (const UTF8Char*)"MACList.txt");
	IO::FileStream *fs;
	UTF8Char *sarr[3];
	Text::UTF8Reader *reader;
	Text::StringBuilderUTF8 sb;
	Text::StringBuilderUTF8 sbName;
	UInt64 rangeStart;
	UInt64 rangeEnd;
	NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	if (!fs->IsError())
	{
		NEW_CLASS(reader, Text::UTF8Reader(fs));
		sb.ClearStr();
		while (reader->ReadLine(&sb, 1024))
		{
			if (sb.StartsWith((const UTF8Char*)"\t{") && sb.EndsWith((const UTF8Char*)"\"},"))
			{
				if (Text::StrSplitTrim(sarr, 3, sb.ToString() + 2, ',') == 3)
				{
					if (Text::StrEndsWith(sarr[0], (const UTF8Char*)"LL"))
					{
						sarr[0][Text::StrCharCnt(sarr[0]) - 2] = 0;
					}
					if (Text::StrEndsWith(sarr[1], (const UTF8Char*)"LL"))
					{
						sarr[1][Text::StrCharCnt(sarr[1]) - 2] = 0;
					}
					rangeStart = Text::StrToUInt64(sarr[0]);
					rangeEnd = Text::StrToUInt64(sarr[1]);
					sarr[2][Text::StrCharCnt(sarr[2]) - 2] = 0;
					sbName.ClearStr();
					Text::CPPText::FromCPPString(&sbName, sarr[2]);
					this->SetEntry(rangeStart, rangeEnd, sbName.ToString());
				}
			}
			else
			{
				sb.ClearStr();
			}
			sb.ClearStr();
		}
		DEL_CLASS(reader);
	}
	DEL_CLASS(fs);
}

Bool Net::MACInfoList::Store()
{
	UTF8Char sbuff[512];
	IO::Path::GetProcessFileName(sbuff);
	IO::Path::AppendPath(sbuff, (const UTF8Char*)"MACList.txt");
	IO::FileStream *fs;
	IO::BufferedOutputStream *cstm;
	UOSInt i;
	UOSInt j;
	NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	if (fs->IsError())
	{
		DEL_CLASS(fs);
		return false;
	}
	Text::UTF8Writer *writer;
	Text::StringBuilderUTF8 sb;
	Net::MACInfo::MACEntry *entry;
	NEW_CLASS(cstm, IO::BufferedOutputStream(fs, 8192));
	NEW_CLASS(writer, Text::UTF8Writer(cstm));
	writer->WriteSignature();
	i = 0;
	j = this->dataList->GetCount();
	while (i < j)
	{
		entry = this->dataList->GetItem(i);
		sb.ClearStr();
		sb.Append((const UTF8Char*)"\t{0x");
		sb.AppendHex64(entry->rangeStart);
		sb.Append((const UTF8Char*)"LL, 0x");
		sb.AppendHex64(entry->rangeEnd);
		sb.Append((const UTF8Char*)"LL, ");
		Text::CPPText::ToCPPString(&sb, (const UTF8Char*)entry->name);
		sb.Append((const UTF8Char*)"},");
		writer->WriteLine(sb.ToString());
		i++;
	}

	DEL_CLASS(writer);
	DEL_CLASS(cstm);
	DEL_CLASS(fs);
	this->modified = false;
	return true;
}
