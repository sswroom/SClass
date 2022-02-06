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
		entry->name = Text::StrCopyNewC(ents[i].name, ents[i].nameLen);
		entry->nameLen = ents[i].nameLen;
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

UOSInt Net::MACInfoList::SetEntry(UInt64 macInt, Text::CString name)
{
	UInt64 mask = 0xffffff;
	return SetEntry(macInt & ~mask, macInt | mask, name);
}

UOSInt Net::MACInfoList::SetEntry(UInt64 rangeStart, UInt64 rangeEnd, Text::CString name)
{
	Net::MACInfo::MACEntry *entry;
	this->modified = true;
	OSInt si = this->GetIndex(rangeStart);
	if (si >= 0)
	{
		entry = this->dataList->GetItem((UOSInt)si);
		SDEL_TEXT(entry->name);
		entry->name = Text::StrCopyNewC(name.v, name.leng);
		entry->nameLen = name.leng;
		return (UOSInt)si;
	}
	else
	{
		entry = MemAlloc(Net::MACInfo::MACEntry, 1);
		entry->rangeStart = rangeStart;
		entry->rangeEnd = rangeEnd;
		entry->name = Text::StrCopyNewC(name.v, name.leng);
		entry->nameLen = name.leng;
		this->dataList->Insert((UOSInt)~si, entry);
		return (UOSInt)~si;
	}
}

void Net::MACInfoList::Load()
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	this->modified = false;
	sptr = IO::Path::GetProcessFileName(sbuff);
	sptr = IO::Path::AppendPathC(sbuff, sptr, UTF8STRC("MACList.txt"));
	IO::FileStream *fs;
	Text::PString sarr[3];
	Text::UTF8Reader *reader;
	Text::StringBuilderUTF8 sb;
	Text::StringBuilderUTF8 sbName;
	UInt64 rangeStart;
	UInt64 rangeEnd;
	NEW_CLASS(fs, IO::FileStream(CSTRP(sbuff, sptr), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	if (!fs->IsError())
	{
		NEW_CLASS(reader, Text::UTF8Reader(fs));
		sb.ClearStr();
		while (reader->ReadLine(&sb, 1024))
		{
			if (sb.StartsWith(UTF8STRC("\t{")) && sb.EndsWith(UTF8STRC("\"},")))
			{
				if (Text::StrSplitTrimP(sarr, 3, sb.Substring(2), ',') == 3)
				{
					if (sarr[0].EndsWith(UTF8STRC("LL")))
					{
						sarr[0].RemoveChars(2);
					}
					if (sarr[1].EndsWith(UTF8STRC("LL")))
					{
						sarr[1].RemoveChars(2);
					}
					rangeStart = sarr[0].ToUInt64();
					rangeEnd = sarr[1].ToUInt64();
					sarr[2].RemoveChars(2);
					sbName.ClearStr();
					if (sarr[2].StartsWith(UTF8STRC("CSTR(")) && sarr[2].EndsWith(')'))
					{
						sarr[2].RemoveChars(1);
						Text::CPPText::FromCPPString(&sbName, sarr[2].v + 5);
					}
					else if (sarr[2].StartsWith(UTF8STRC("UTF8STRC(")) && sarr[2].EndsWith(')'))
					{
						sarr[2].RemoveChars(1);
						Text::CPPText::FromCPPString(&sbName, sarr[2].v + 9);
					}
					else
					{
						Text::CPPText::FromCPPString(&sbName, sarr[2].v);
					}
					this->SetEntry(rangeStart, rangeEnd, sbName.ToCString());
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
	UTF8Char *sptr;
	sptr = IO::Path::GetProcessFileName(sbuff);
	sptr = IO::Path::AppendPathC(sbuff, sptr, UTF8STRC("MACList.txt"));
	IO::FileStream *fs;
	IO::BufferedOutputStream *cstm;
	UOSInt i;
	UOSInt j;
	NEW_CLASS(fs, IO::FileStream(CSTRP(sbuff, sptr), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
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
		sb.AppendC(UTF8STRC("\t{0x"));
		sb.AppendHex64(entry->rangeStart);
		sb.AppendC(UTF8STRC("LL, 0x"));
		sb.AppendHex64(entry->rangeEnd);
		sb.AppendC(UTF8STRC("LL, UTF8STRC("));
		Text::CPPText::ToCPPString(&sb, entry->name);
		sb.AppendC(UTF8STRC(")},"));
		writer->WriteLineC(sb.ToString(), sb.GetLength());
		i++;
	}

	DEL_CLASS(writer);
	DEL_CLASS(cstm);
	DEL_CLASS(fs);
	this->modified = false;
	return true;
}
