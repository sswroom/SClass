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
	UOSInt cnt;
	UnsafeArray<Net::MACInfo::MACEntry> ents = Net::MACInfo::GetMACEntryList(cnt);
	NN<Net::MACInfo::MACEntry> entry;
	UOSInt i = 0;
	while (i < cnt)
	{
		entry = MemAllocNN(Net::MACInfo::MACEntry);
		entry->rangeStart = ents[i].rangeStart;
		entry->rangeEnd = ents[i].rangeEnd;
		entry->name = Text::StrCopyNewC(ents[i].name, ents[i].nameLen).Ptr();
		entry->nameLen = ents[i].nameLen;
		this->dataList.Add(entry);
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
	NN<Net::MACInfo::MACEntry> entry;
	i = this->dataList.GetCount();
	while (i-- > 0)
	{
		entry = this->dataList.GetItemNoCheck(i);
		if (entry->name) Text::StrDelNew(entry->name);
		MemFreeNN(entry);
	}
}

UOSInt Net::MACInfoList::GetCount() const
{
	return this->dataList.GetCount();
}

NN<const Net::MACInfo::MACEntry> Net::MACInfoList::GetItemNoCheck(UOSInt index) const
{
	return this->dataList.GetItemNoCheck(index);
}

Optional<const Net::MACInfo::MACEntry> Net::MACInfoList::GetItem(UOSInt index) const
{
	return this->dataList.GetItem(index);
}

OSInt Net::MACInfoList::GetIndex(UInt64 macInt)
{
	OSInt i;
	OSInt j;
	OSInt k;
	NN<Net::MACInfo::MACEntry> entry;
	i = 0;
	j = (OSInt)this->dataList.GetCount() - 1;
	while (i <= j)
	{
		k = (i + j) >> 1;
		entry = this->dataList.GetItemNoCheck((UOSInt)k);
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

Optional<const Net::MACInfo::MACEntry> Net::MACInfoList::GetEntry(UInt64 macInt)
{
	OSInt si = this->GetIndex(macInt);
	if (si >= 0)
	{
		return this->dataList.GetItem((UOSInt)si);
	}
	return 0;
}

Optional<const Net::MACInfo::MACEntry> Net::MACInfoList::GetEntryOUI(const UInt8 *oui)
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
		return this->dataList.GetItem((UOSInt)si);
	}
	return 0;
}

UOSInt Net::MACInfoList::SetEntry(UInt64 macInt, Text::CStringNN name)
{
	UInt64 mask = 0xffffff;
	return SetEntry(macInt & ~mask, macInt | mask, name);
}

UOSInt Net::MACInfoList::SetEntry(UInt64 rangeStart, UInt64 rangeEnd, Text::CStringNN name)
{
	NN<Net::MACInfo::MACEntry> entry;
	this->modified = true;
	OSInt si = this->GetIndex(rangeStart);
	if (si >= 0)
	{
		entry = this->dataList.GetItemNoCheck((UOSInt)si);
		if (entry->name) Text::StrDelNew(entry->name);
		entry->name = Text::StrCopyNewC(name.v, name.leng).Ptr();
		entry->nameLen = name.leng;
		return (UOSInt)si;
	}
	else
	{
		entry = MemAllocNN(Net::MACInfo::MACEntry);
		entry->rangeStart = rangeStart;
		entry->rangeEnd = rangeEnd;
		entry->name = Text::StrCopyNewC(name.v, name.leng).Ptr();
		entry->nameLen = name.leng;
		this->dataList.Insert((UOSInt)~si, entry);
		return (UOSInt)~si;
	}
}

void Net::MACInfoList::Load()
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	this->modified = false;
	sptr = IO::Path::GetProcessFileName(sbuff).Or(sbuff);
	sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("MACList.txt"));
	Text::PString sarr[3];
	Text::StringBuilderUTF8 sb;
	Text::StringBuilderUTF8 sbName;
	UInt64 rangeStart;
	UInt64 rangeEnd;
	IO::FileStream fs(CSTRP(sbuff, sptr), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	if (!fs.IsError())
	{
		Text::UTF8Reader reader(fs);
		sb.ClearStr();
		while (reader.ReadLine(sb, 1024))
		{
			if (sb.StartsWith(UTF8STRC("\t{")) && sb.EndsWith(UTF8STRC("\")},")))
			{
				sb.RemoveChars(2);
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
						Text::CPPText::FromCPPString(sbName, sarr[2].v + 5);
					}
					else if (sarr[2].StartsWith(UTF8STRC("UTF8STRC(")) && sarr[2].EndsWith(')'))
					{
						sarr[2].RemoveChars(1);
						Text::CPPText::FromCPPString(sbName, sarr[2].v + 9);
					}
					else if (sarr[2].StartsWith(UTF8STRC("UTF8STRCPTR(")) && sarr[2].EndsWith(')'))
					{
						sarr[2].RemoveChars(1);
						Text::CPPText::FromCPPString(sbName, sarr[2].v + 12);
					}
					else
					{
						Text::CPPText::FromCPPString(sbName, sarr[2].v);
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
	}
}

Bool Net::MACInfoList::Store()
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	sptr = IO::Path::GetProcessFileName(sbuff).Or(sbuff);
	sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("MACList.txt"));
	UOSInt i;
	UOSInt j;
	IO::FileStream fs(CSTRP(sbuff, sptr), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	if (fs.IsError())
	{
		return false;
	}
	else
	{
		Text::StringBuilderUTF8 sb;
		NN<Net::MACInfo::MACEntry> entry;
		IO::BufferedOutputStream cstm(fs, 8192);
		Text::UTF8Writer writer(cstm);
		writer.WriteSignature();
		i = 0;
		j = this->dataList.GetCount();
		while (i < j)
		{
			entry = this->dataList.GetItemNoCheck(i);
			sb.ClearStr();
			sb.AppendC(UTF8STRC("\t{0x"));
			sb.AppendHex64(entry->rangeStart);
			sb.AppendC(UTF8STRC("LL, 0x"));
			sb.AppendHex64(entry->rangeEnd);
			sb.AppendC(UTF8STRC("LL, UTF8STRCPTR("));
			Text::CPPText::ToCPPString(sb, entry->name);
			sb.AppendC(UTF8STRC(")},"));
			writer.WriteLine(sb.ToCString());
			i++;
		}
	}

	this->modified = false;
	return true;
}
