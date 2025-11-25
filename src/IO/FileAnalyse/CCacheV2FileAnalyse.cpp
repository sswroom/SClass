#include "Stdafx.h"
#include "Data/ByteBuffer.h"
#include "Core/ByteTool_C.h"
#include "Data/UUID.h"
#include "IO/FileAnalyse/CCacheV2FileAnalyse.h"
#include "Math/Math_C.h"
#include "Text/MyStringW.h"
#include "Text/StringBuilderUTF8.h"

void __stdcall IO::FileAnalyse::CCacheV2FileAnalyse::ParseThread(NN<Sync::Thread> thread)
{
	NN<IO::FileAnalyse::CCacheV2FileAnalyse> me = thread->GetUserObj().GetNN<IO::FileAnalyse::CCacheV2FileAnalyse>();
//	UInt64 dataSize;
//	UInt64 ofst;
//	UInt32 lastSize;
//	Int32 rowSize;
//	UInt8 tagHdr[15];
	NN<IO::FileAnalyse::CCacheV2FileAnalyse::TagInfo> tag;
//	Bool lastIsFree = false;

	tag = MemAllocNN(IO::FileAnalyse::CCacheV2FileAnalyse::TagInfo);
	tag->ofst = 0;
	tag->size = 40;
	tag->row = 0;
	tag->col = 0;
	tag->tagType = TagType::Header;
	me->tags.Add(tag);

	tag = MemAllocNN(IO::FileAnalyse::CCacheV2FileAnalyse::TagInfo);
	tag->ofst = 40;
	tag->size = 0x20018;
	tag->row = 0;
	tag->col = 0;
	tag->tagType = TagType::UserHeader;
	me->tags.Add(tag);
	UInt8 index[0x20000];
	if (me->fd->GetRealData(64, 0x20000, BYTEARR(index)) == 0x20000)
	{
		UOSInt ofst;
		UInt64 idx;
		UInt64 tileOfst;
		UOSInt tileSize;
		UOSInt i = 0;
		UOSInt j;
		while (i < 128)
		{
			j = 0;
			while (j < 128)
			{
				ofst = i * 1024 + j * 8;
				idx = ReadUInt64(&index[ofst]);
				tileOfst = idx % 0x10000000000LL;
				tileSize = (UOSInt)(idx / 0x10000000000LL);
				if (tileSize != 0)
				{
					tag = MemAllocNN(IO::FileAnalyse::CCacheV2FileAnalyse::TagInfo);
					tag->ofst = tileOfst - 4;
					tag->size = tileSize + 4;
					tag->row = i;
					tag->col = j;
					tag->tagType = TagType::Tile;
					me->tags.Add(tag);
				}
				j++;
			}
			i++;
		}
	}
}

IO::FileAnalyse::CCacheV2FileAnalyse::CCacheV2FileAnalyse(NN<IO::StreamData> fd) : thread(ParseThread, this, CSTR("CCacheV2FileAnalyse"))
{
	UInt8 buff[64];
	this->fd = 0;
	this->pauseParsing = false;
	fd->GetRealData(0, 64, BYTEARR(buff));
	if (ReadUInt64(&buff[24]) != fd->GetDataSize())
	{
		return;
	}
	if (ReadUInt32(&buff[40]) != 0x20014)
	{
		return;
	}
	this->maxRowSize = ReadUInt32(&buff[4]);
	this->fd = fd->GetPartialData(0, fd->GetDataSize()).Ptr();
	this->thread.Start();
}

IO::FileAnalyse::CCacheV2FileAnalyse::~CCacheV2FileAnalyse()
{
	this->thread.Stop();
	SDEL_CLASS(this->fd);
	this->tags.MemFreeAll();
}

Text::CStringNN IO::FileAnalyse::CCacheV2FileAnalyse::GetFormatName()
{
	return CSTR("Compact Cache V2");
}

UOSInt IO::FileAnalyse::CCacheV2FileAnalyse::GetFrameCount()
{
	return this->tags.GetCount();
}

Bool IO::FileAnalyse::CCacheV2FileAnalyse::GetFrameName(UOSInt index, NN<Text::StringBuilderUTF8> sb)
{
	NN<IO::FileAnalyse::CCacheV2FileAnalyse::TagInfo> tag;
	if (!this->tags.GetItem(index).SetTo(tag))
		return false;
	sb->AppendU64(tag->ofst);
	sb->AppendC(UTF8STRC(": Type="));
	sb->Append(TagTypeGetName(tag->tagType));
	sb->AppendC(UTF8STRC(", size="));
	sb->AppendUOSInt(tag->size);
	return true;
}

UOSInt IO::FileAnalyse::CCacheV2FileAnalyse::GetFrameIndex(UInt64 ofst)
{
	OSInt i = 0;
	OSInt j = (OSInt)this->tags.GetCount() - 1;
	OSInt k;
	NN<TagInfo> pack;
	while (i <= j)
	{
		k = (i + j) >> 1;
		pack = this->tags.GetItemNoCheck((UOSInt)k);
		if (ofst < pack->ofst)
		{
			j = k - 1;
		}
		else if (ofst >= pack->ofst + pack->size)
		{
			i = k + 1;
		}
		else
		{
			return (UOSInt)k;
		}
	}
	return INVALID_INDEX;
}

Optional<IO::FileAnalyse::FrameDetail> IO::FileAnalyse::CCacheV2FileAnalyse::GetFrameDetail(UOSInt index)
{
	NN<IO::FileAnalyse::FrameDetail> frame;
	UTF8Char sbuff[1024];
	UnsafeArray<UTF8Char> sptr;
	NN<IO::FileAnalyse::CCacheV2FileAnalyse::TagInfo> tag;
	if (!this->tags.GetItem(index).SetTo(tag))
		return 0;
	
	NEW_CLASSNN(frame, IO::FileAnalyse::FrameDetail(tag->ofst, tag->size));
	sptr = TagTypeGetName(tag->tagType).ConcatTo(Text::StrConcatC(sbuff, UTF8STRC("Type=")));
	frame->AddHeader(CSTRP(sbuff, sptr));

	Data::ByteBuffer tagData(tag->size);
	this->fd->GetRealData(tag->ofst, tag->size, tagData);
	if (tag->tagType == TagType::Header)
	{
		frame->AddUInt(0, 4, CSTR("Version"), ReadUInt32(&tagData[0]));
		frame->AddUInt(4, 4, CSTR("Record Count"), ReadUInt32(&tagData[4]));
		frame->AddUInt(8, 4, CSTR("Maximum Tile Size"), ReadUInt32(&tagData[8]));
		frame->AddUInt(12, 4, CSTR("Offset Byte Count"), ReadUInt32(&tagData[12]));
		frame->AddUInt64(16, CSTR("Slack Space"), ReadUInt64(&tagData[16]));
		frame->AddUInt64(24, CSTR("File Size"), ReadUInt64(&tagData[24]));
		frame->AddUInt64(32, CSTR("User Header Offset"), ReadUInt64(&tagData[32]));
	}
	else if (tag->tagType == TagType::UserHeader)
	{
		frame->AddUInt(0, 4, CSTR("User Header Size"), ReadUInt32(&tagData[0]));
		frame->AddUInt(4, 4, CSTR("Legacy1"), ReadUInt32(&tagData[4]));
		frame->AddUInt(8, 4, CSTR("Legacy2"), ReadUInt32(&tagData[8]));
		frame->AddUInt(12, 4, CSTR("Legacy3"), ReadUInt32(&tagData[12]));
		frame->AddUInt(16, 4, CSTR("Legacy4"), ReadUInt32(&tagData[16]));
		frame->AddUInt(20, 4, CSTR("Index Size"), ReadUInt32(&tagData[20]));
		UInt64 idx;
		UOSInt ofst;
		Text::StringBuilderUTF8 sb;
		UOSInt i = 0;
		UOSInt j;
		while (i < 128)
		{
			j = 0;
			while (j < 128)
			{
				ofst = 24 + i * 1024 + j * 8;
				idx = ReadUInt64(&tagData[ofst]);
				sb.ClearStr();
				sb.AppendUTF8Char('R');
				sb.AppendUOSInt(i);
				sb.AppendUTF8Char('C');
				sb.AppendUOSInt(i);
				sb.Append(CSTR("TileOffset"));
				frame->AddUInt64(ofst, sb.ToCString(), idx % 0x10000000000LL);
				sb.RemoveChars(10);
				sb.Append(CSTR("TileSize"));
				frame->AddUInt64(ofst, sb.ToCString(), idx / 0x10000000000LL);
				j++;
			}
			i++;
		}
	}
	else if (tag->tagType == TagType::Tile)
	{
		frame->AddUInt(0, 4, CSTR("Tile Size"), ReadUInt32(&tagData[0]));
	}
	return frame;
}

Bool IO::FileAnalyse::CCacheV2FileAnalyse::IsError()
{
	return this->fd == 0;
}

Bool IO::FileAnalyse::CCacheV2FileAnalyse::IsParsing()
{
	return this->thread.IsRunning();
}

Bool IO::FileAnalyse::CCacheV2FileAnalyse::TrimPadding(Text::CStringNN outputFile)
{
	return false;
}

Text::CStringNN IO::FileAnalyse::CCacheV2FileAnalyse::TagTypeGetName(TagType tagType)
{
	switch (tagType)
	{
	case TagType::Header:
		return CSTR("Header");
	case TagType::UserHeader:
		return CSTR("UserHeader");
	case TagType::Tile:
		return CSTR("Tile");
	default:
		return CSTR("Unknown");
	}
}
