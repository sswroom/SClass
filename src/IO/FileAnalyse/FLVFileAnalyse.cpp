#include "Stdafx.h"
#include "Data/ByteBuffer.h"
#include "Data/ByteTool.h"
#include "IO/FileAnalyse/FLVFileAnalyse.h"
#include "IO/FileAnalyse/SBFrameDetail.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"

UOSInt IO::FileAnalyse::FLVFileAnalyse::ParseScriptDataVal(UnsafeArray<UInt8> data, UOSInt ofst, UOSInt endOfst, NN<Text::StringBuilderUTF8> sb)
{
	if (ofst >= endOfst)
	{
		return endOfst;
	}
	switch (data[ofst])
	{
	case 0:
		if (ofst + 9 <= endOfst)
		{
			Text::SBAppendF64(sb, ReadMDouble(&data[ofst + 1]));
		}
		return ofst + 9;
	case 1:
		if (ofst + 2 <= endOfst)
		{
			if (data[ofst + 1])
			{
				sb->AppendC(UTF8STRC("true"));
			}
			else
			{
				sb->AppendC(UTF8STRC("false"));
			}
		}
		return ofst + 2;
	case 2:
		if (ofst + 3 <= endOfst)
		{
			UInt32 strSize = ReadMUInt16(&data[ofst + 1]);
			if (ofst + 3 + strSize <= endOfst)
			{
				sb->AppendC((UTF8Char*)&data[ofst + 3], strSize);
			}
			return ofst + 3 + strSize;
		}
		else
		{
			return endOfst;
		}
	case 8:
		if (ofst + 5 <= endOfst)
		{
			UInt32 arrCnt = ReadMUInt32(&data[ofst + 1]);
			UInt32 i;
			UInt32 strLen;
			sb->AppendC(UTF8STRC("["));
			ofst += 5;
			i = 0;
			while (i < arrCnt)
			{
				if (ofst + 2 <= endOfst)
				{
					strLen = ReadMUInt16(&data[ofst]);
					if (i > 0)
					{
						sb->AppendC(UTF8STRC(",\r\n\t"));
					}
					else
					{
						sb->AppendC(UTF8STRC("\r\n\t"));
					}
					if (ofst + 2 + strLen <= endOfst)
					{
						sb->AppendC((UTF8Char*)&data[ofst + 2], strLen);
						ofst += 2 + strLen;
						sb->AppendC(UTF8STRC(" = "));
						ofst = ParseScriptDataVal(data, ofst, endOfst, sb);
					}
					else
					{
						ofst = endOfst;
					}
				}
				else
				{
					ofst = endOfst;
					break;
				}
				i++;
			}
			sb->AppendC(UTF8STRC("]"));
			if (ofst + 3 <= endOfst && data[ofst] == 0 && data[ofst + 1] == 0 && data[ofst + 2] == 9)
			{
				ofst += 3;
			}
			return ofst;
		}
		else
		{
			return endOfst;
		}
	default:
		return endOfst;
	}
}

void IO::FileAnalyse::FLVFileAnalyse::ParseScriptData(UnsafeArray<UInt8> data, UOSInt ofst, UOSInt endOfst, UOSInt frameOfst, NN<IO::FileAnalyse::FrameDetailHandler> frame)
{
	Text::StringBuilderUTF8 sbName;
	Text::StringBuilderUTF8 sbVal;
	UOSInt ofstName = ParseScriptDataVal(data, ofst, endOfst, sbName);
	UOSInt ofstVal = ParseScriptDataVal(data, ofstName, endOfst, sbVal);
	frame->AddField(frameOfst + ofst, ofstVal - ofst, sbName.ToCString(), sbVal.ToCString());
}

void __stdcall IO::FileAnalyse::FLVFileAnalyse::ParseThread(NN<Sync::Thread> thread)
{
	NN<IO::FileAnalyse::FLVFileAnalyse> me = thread->GetUserObj().GetNN<IO::FileAnalyse::FLVFileAnalyse>();
	UInt64 dataSize;
	UInt64 ofst;
	UInt32 lastSize;
	UInt8 tagHdr[15];
	NN<IO::FileAnalyse::FLVFileAnalyse::FLVTag> tag;
	ofst = me->hdrSize + 4;
	dataSize = me->fd->GetDataSize();
	lastSize = 0;
	while (ofst < dataSize - 11 && !thread->IsStopping())
	{
		if (me->fd->GetRealData(ofst - 4, 15, BYTEARR(tagHdr)) != 15)
			break;
		
		if (ReadMUInt32(tagHdr) != lastSize)
			break;

		lastSize = ReadMUInt24(&tagHdr[5]) + 11;
		if (lastSize <= 11)
		{
			break;
		}
		tag = MemAllocNN(IO::FileAnalyse::FLVFileAnalyse::FLVTag);
		tag->ofst = ofst;
		tag->size = lastSize;
		tag->tagType = tagHdr[4] & 0x1f;
		me->tags.Add(tag);
		ofst += lastSize + 4;
	}
}

IO::FileAnalyse::FLVFileAnalyse::FLVFileAnalyse(NN<IO::StreamData> fd) : thread(ParseThread, this, CSTR("FLVFileAnalyse"))
{
	UInt8 buff[256];
	this->fd = 0;
	this->pauseParsing = false;
	fd->GetRealData(0, 256, BYTEARR(buff));
	if (buff[0] != 'F' || buff[1] != 'L' || buff[2] != 'V' || buff[3] != 1)
	{
		return;
	}
	hdrSize = ReadMUInt32(&buff[5]);
	if (hdrSize < 9 || hdrSize > 100)
	{
		return;
	}
	this->fd = fd->GetPartialData(0, fd->GetDataSize()).Ptr();
	this->thread.Start();
}

IO::FileAnalyse::FLVFileAnalyse::~FLVFileAnalyse()
{
	this->thread.Stop();
	SDEL_CLASS(this->fd);
	this->tags.MemFreeAll();
}

Text::CStringNN IO::FileAnalyse::FLVFileAnalyse::GetFormatName()
{
	return CSTR("FLV");
}

UOSInt IO::FileAnalyse::FLVFileAnalyse::GetFrameCount()
{
	return 1 + this->tags.GetCount();
}

Bool IO::FileAnalyse::FLVFileAnalyse::GetFrameName(UOSInt index, NN<Text::StringBuilderUTF8> sb)
{
	if (index == 0)
	{
		sb->AppendC(UTF8STRC("FLV Header"));
		return true;
	}
	NN<IO::FileAnalyse::FLVFileAnalyse::FLVTag> tag;
	if (!this->tags.GetItem(index - 1).SetTo(tag))
		return false;
	sb->AppendU64(tag->ofst);
	sb->AppendC(UTF8STRC(": Type="));
	sb->AppendU16(tag->tagType);
	sb->AppendC(UTF8STRC(", size="));
	sb->AppendUOSInt(tag->size);
	return true;
}

Bool IO::FileAnalyse::FLVFileAnalyse::GetFrameDetail(UOSInt index, NN<Text::StringBuilderUTF8> sb)
{
	UInt8 buff[128];
	if (index == 0)
	{
		this->fd->GetRealData(0, this->hdrSize, BYTEARR(buff));
		sb->AppendC(UTF8STRC("Version = "));
		sb->AppendU16(buff[3]);
		sb->AppendC(UTF8STRC("\r\nTypeFlagsReserved = "));
		sb->AppendU16((UInt16)(buff[4] >> 3));
		sb->AppendC(UTF8STRC("\r\nTypeFlagsAudio = "));
		sb->AppendU16((UInt16)((buff[4] >> 2) & 1));
		sb->AppendC(UTF8STRC("\r\nTypeFlagsReserved = "));
		sb->AppendU16((UInt16)((buff[4] >> 1) & 1));
		sb->AppendC(UTF8STRC("\r\nTypeFlagsVideo = "));
		sb->AppendU16((UInt16)(buff[4] & 1));
		sb->AppendC(UTF8STRC("\r\nDataOffset = "));
		sb->AppendI32(ReadMInt32(&buff[5]));
		return true;
	}
	NN<IO::FileAnalyse::FLVFileAnalyse::FLVTag> tag;
	if (!this->tags.GetItem(index - 1).SetTo(tag))
		return false;
	sb->AppendC(UTF8STRC("Tag"));
	sb->AppendUOSInt(index);
	this->fd->GetRealData(tag->ofst, 11, BYTEARR(buff));
	sb->AppendC(UTF8STRC("\r\nReserved = "));
	sb->AppendU16((UInt16)(buff[0] >> 6));
	sb->AppendC(UTF8STRC("\r\nFilter = "));
	sb->AppendU16((UInt16)((buff[0] >> 5) & 1));
	sb->AppendC(UTF8STRC("\r\nTagType = "));
	sb->AppendU16(buff[0] & 0x1f);
	if (tag->tagType == 8)
	{
		sb->AppendC(UTF8STRC(" (audio)"));
	}
	else if (tag->tagType == 9)
	{
		sb->AppendC(UTF8STRC(" (video)"));
	}
	else if (tag->tagType == 18)
	{
		sb->AppendC(UTF8STRC(" (script data)"));
	}
	sb->AppendC(UTF8STRC("\r\nDataSize = "));
	sb->AppendU32(ReadMUInt24(&buff[1]));
	sb->AppendC(UTF8STRC("\r\nTimestamp = "));
	sb->AppendU32(ReadMUInt24(&buff[4]));
	sb->AppendC(UTF8STRC("\r\nTimestamp Extended = "));
	sb->AppendU16(buff[7]);
	if (tag->tagType == 8)
	{
		Data::ByteBuffer tagData(tag->size);
		this->fd->GetRealData(tag->ofst, tag->size, tagData);

		sb->AppendC(UTF8STRC("\r\n"));
		if (tag->size >= 256 + 11)
		{
			sb->AppendHexBuff(tagData.WithSize(256), ' ', Text::LineBreakType::CRLF);
		}
		else
		{
			sb->AppendHexBuff(tagData.WithSize(tag->size - 11), ' ', Text::LineBreakType::CRLF);
		}
	}
	else if (tag->tagType == 9)
	{
		Data::ByteBuffer tagData(tag->size);
		this->fd->GetRealData(tag->ofst, tag->size, tagData);

		sb->AppendC(UTF8STRC("\r\n"));
		if (tag->size >= 256 + 11)
		{
			sb->AppendHexBuff(tagData.WithSize(256), ' ', Text::LineBreakType::CRLF);
		}
		else
		{
			sb->AppendHexBuff(tagData.WithSize(tag->size - 11), ' ', Text::LineBreakType::CRLF);
		}
	}
	else if (tag->tagType == 18)
	{
		Data::ByteBuffer tagData(tag->size);
		this->fd->GetRealData(tag->ofst, tag->size, tagData);
		IO::FileAnalyse::SBFrameDetail frame(sb);
		ParseScriptData(tagData.Arr(), 11, tag->size, 0, frame);
	}
	return true;
}

UOSInt IO::FileAnalyse::FLVFileAnalyse::GetFrameIndex(UInt64 ofst)
{
	if (ofst < this->hdrSize)
	{
		return 0;
	}
	OSInt i = 0;
	OSInt j = (OSInt)this->tags.GetCount() - 1;
	OSInt k;
	NN<FLVTag> pack;
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
			return (UOSInt)k + 1;
		}
	}
	return INVALID_INDEX;
}

Optional<IO::FileAnalyse::FrameDetail> IO::FileAnalyse::FLVFileAnalyse::GetFrameDetail(UOSInt index)
{
	NN<IO::FileAnalyse::FrameDetail> frame;
	UInt8 buff[128];
	UTF8Char sbuff[128];
	UnsafeArray<UTF8Char> sptr;
	Text::CString vName;
	if (index == 0)
	{
		NEW_CLASSNN(frame, IO::FileAnalyse::FrameDetail(0, this->hdrSize));
		this->fd->GetRealData(0, this->hdrSize, BYTEARR(buff));
		sptr = Text::StrConcatC(sbuff, buff, 3);
		frame->AddField(0, 3, CSTR("Magic"), CSTRP(sbuff, sptr));
		sptr = Text::StrUInt16(sbuff, buff[3]);
		frame->AddField(3, 1, CSTR("Version"), CSTRP(sbuff, sptr));
		sptr = Text::StrHexByte(Text::StrConcatC(sbuff, UTF8STRC("0x")), buff[4]);
		frame->AddField(4, 1, CSTR("TypeFlags"), CSTRP(sbuff, sptr));
		sptr = Text::StrUInt16(sbuff, (UInt16)(buff[4] >> 3));
		frame->AddSubfield(4, 1, CSTR("Reserved"), CSTRP(sbuff, sptr));
		sptr = Text::StrUInt16(sbuff, (UInt16)((buff[4] >> 2) & 1));
		frame->AddSubfield(4, 1, CSTR("Audio"), CSTRP(sbuff, sptr));
		sptr = Text::StrUInt16(sbuff, (UInt16)((buff[4] >> 1) & 1));
		frame->AddSubfield(4, 1, CSTR("Reserved"), CSTRP(sbuff, sptr));
		sptr = Text::StrUInt16(sbuff, (UInt16)(buff[4] & 1));
		frame->AddSubfield(4, 1, CSTR("Video"), CSTRP(sbuff, sptr));
		sptr = Text::StrUInt32(sbuff, ReadMUInt32(&buff[5]));
		frame->AddField(5, 4, CSTR("DataOffset"), CSTRP(sbuff, sptr));
		return frame;
	}
	NN<IO::FileAnalyse::FLVFileAnalyse::FLVTag> tag;
	if (!this->tags.GetItem(index - 1).SetTo(tag))
		return 0;
	
	NEW_CLASSNN(frame, IO::FileAnalyse::FrameDetail(tag->ofst, tag->size));
	sptr = Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("Tag")), index);
	frame->AddHeader(CSTRP(sbuff, sptr));

	this->fd->GetRealData(tag->ofst, 11, BYTEARR(buff));
	frame->AddUInt(0, 1, CSTR("Reserved"), (UInt16)(buff[0] >> 6));
	frame->AddUInt(0, 1, CSTR("Filter"), (UInt16)((buff[0] >> 5) & 1));
	vName = CSTR_NULL;
	switch (tag->tagType)
	{
	case 8:
		vName = CSTR("audio");
		break;
	case 9:
		vName = CSTR("video");
		break;
	case 18:
		vName = CSTR("script data");
		break;
	}
	frame->AddUIntName(0, 1, CSTR("TagType"), buff[0] & 0x1f, vName);
	frame->AddUInt(1, 3, CSTR("DataSize"), ReadMUInt24(&buff[1]));
	frame->AddUInt(4, 3, CSTR("Timestamp"), ReadMUInt24(&buff[4]));
	frame->AddUInt(7, 1, CSTR("Timestamp Extended"), buff[7]);
	if (tag->tagType == 8)
	{
		Data::ByteBuffer tagData(tag->size);
		this->fd->GetRealData(tag->ofst, tag->size, tagData);

		if (tag->size >= 256 + 11)
		{
			Text::StringBuilderUTF8 sb;
			sb.AppendHexBuff(&tagData[11], 256, ' ', Text::LineBreakType::CRLF);
			frame->AddField(11, tag->size - 11, CSTR("data"), sb.ToCString());
		}
		else
		{
			frame->AddHexBuff(11, tag->size - 11, CSTR("data"), &tagData[11], true);
		}
	}
	else if (tag->tagType == 9)
	{
		Data::ByteBuffer tagData(tag->size);
		this->fd->GetRealData(tag->ofst, tag->size, tagData);
		if (tag->size >= 256 + 11)
		{
			Text::StringBuilderUTF8 sb;
			sb.AppendHexBuff(&tagData[11], 256, ' ', Text::LineBreakType::CRLF);
			frame->AddField(11, tag->size - 11, CSTR("data"), sb.ToCString());
		}
		else
		{
			frame->AddHexBuff(11, tag->size - 11, CSTR("data"), &tagData[11], true);
		}
	}
	else if (tag->tagType == 18)
	{
		Data::ByteBuffer tagData(tag->size);
		this->fd->GetRealData(tag->ofst, tag->size, tagData);
		ParseScriptData(tagData.Arr(), 11, tag->size, 0, frame);
	}
	return frame;
}

Bool IO::FileAnalyse::FLVFileAnalyse::IsError()
{
	return this->fd == 0;
}

Bool IO::FileAnalyse::FLVFileAnalyse::IsParsing()
{
	return this->thread.IsRunning();
}

Bool IO::FileAnalyse::FLVFileAnalyse::TrimPadding(Text::CStringNN outputFile)
{
	return false;
}
