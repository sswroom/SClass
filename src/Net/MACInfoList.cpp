#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "IO/WriteCacheStream.h"
#include "Net/MACInfoList.h"
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

UOSInt Net::MACInfoList::SetEntry(UInt64 macInt, const UTF8Char *name)
{
	Net::MACInfo::MACEntry *entry;
	this->modified = true;
	OSInt si = this->GetIndex(macInt);
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
		entry->rangeStart = macInt & 0xffffff000000;
		entry->rangeEnd = entry->rangeStart | 0xffffff;
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
	UInt64 rangeStart;
	NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileStream::FILE_MODE_READONLY, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
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
					sarr[2][Text::StrCharCnt(sarr[2]) - 3] = 0;
					this->SetEntry(rangeStart, &sarr[2][1]);
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
	IO::WriteCacheStream *cstm;
	UOSInt i;
	UOSInt j;
	NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileStream::FILE_MODE_CREATE, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
	if (fs->IsError())
	{
		DEL_CLASS(fs);
		return false;
	}
	Text::UTF8Writer *writer;
	Text::StringBuilderUTF8 sb;
	Net::MACInfo::MACEntry *entry;
	NEW_CLASS(cstm, IO::WriteCacheStream(fs));
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
		sb.Append((const UTF8Char*)"LL, \"");
		sb.Append((const UTF8Char*)entry->name);
		sb.Append((const UTF8Char*)"\"},");
		writer->WriteLine(sb.ToString());
		i++;
	}

	DEL_CLASS(writer);
	DEL_CLASS(cstm);
	DEL_CLASS(fs);
	this->modified = false;
	return true;
}
