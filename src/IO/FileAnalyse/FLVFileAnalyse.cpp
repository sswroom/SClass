#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "IO/FileAnalyse/FLVFileAnalyse.h"
#include "Sync/Thread.h"
#include "Text/MyStringFloat.h"

UOSInt IO::FileAnalyse::FLVFileAnalyse::ParseScriptDataVal(UInt8 *data, UOSInt ofst, UOSInt endOfst, Text::StringBuilderUTF *sb)
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
			sb->Append(data[ofst + 1]?(const UTF8Char*)"true":(const UTF8Char*)"false");
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
			sb->Append((const UTF8Char*)"[");
			ofst += 5;
			i = 0;
			while (i < arrCnt)
			{
				if (ofst + 2 <= endOfst)
				{
					strLen = ReadMUInt16(&data[ofst]);
					if (i > 0)
					{
						sb->Append((const UTF8Char*)",\r\n\t");
					}
					else
					{
						sb->Append((const UTF8Char*)"\r\n\t");
					}
					if (ofst + 2 + strLen <= endOfst)
					{
						sb->AppendC((UTF8Char*)&data[ofst + 2], strLen);
						ofst += 2 + strLen;
						sb->Append((const UTF8Char*)" = ");
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
			sb->Append((const UTF8Char*)"]");
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

void IO::FileAnalyse::FLVFileAnalyse::ParseScriptData(UInt8 *data, UOSInt ofst, UOSInt endOfst, Text::StringBuilderUTF *sb)
{
	sb->Append((const UTF8Char*)"\r\n");
	ofst = ParseScriptDataVal(data, ofst, endOfst, sb);
	sb->Append((const UTF8Char*)" = ");
	ofst = ParseScriptDataVal(data, ofst, endOfst, sb);
}

UInt32 __stdcall IO::FileAnalyse::FLVFileAnalyse::ParseThread(void *userObj)
{
	IO::FileAnalyse::FLVFileAnalyse *me = (IO::FileAnalyse::FLVFileAnalyse*)userObj;
	UInt64 dataSize;
	UInt64 ofst;
	UInt32 lastSize;
	UInt8 tagHdr[15];
	IO::FileAnalyse::FLVFileAnalyse::FLVTag *tag;
	me->threadRunning = true;
	me->threadStarted = true;
	ofst = me->hdrSize + 4;
	dataSize = me->fd->GetDataSize();
	lastSize = 0;
	while (ofst < dataSize - 11 && !me->threadToStop)
	{
		if (me->fd->GetRealData(ofst - 4, 15, tagHdr) != 15)
			break;
		
		if (ReadMUInt32(tagHdr) != lastSize)
			break;

		lastSize = ReadMUInt24(&tagHdr[5]) + 11;
		if (lastSize <= 11)
		{
			break;
		}
		tag = MemAlloc(IO::FileAnalyse::FLVFileAnalyse::FLVTag, 1);
		tag->ofst = ofst;
		tag->size = lastSize;
		tag->tagType = tagHdr[4] & 0x1f;
		me->tags->Add(tag);
		ofst += lastSize + 4;
	}
	
	me->threadRunning = false;
	return 0;
}

IO::FileAnalyse::FLVFileAnalyse::FLVFileAnalyse(IO::IStreamData *fd)
{
	UInt8 buff[256];
	this->fd = 0;
	this->threadRunning = false;
	this->pauseParsing = false;
	this->threadToStop = false;
	this->threadStarted = false;
	NEW_CLASS(this->tags, Data::SyncArrayList<IO::FileAnalyse::FLVFileAnalyse::FLVTag*>());
	fd->GetRealData(0, 256, buff);
	if (buff[0] != 'F' || buff[1] != 'L' || buff[2] != 'V' || buff[3] != 1)
	{
		return;
	}
	hdrSize = ReadMUInt32(&buff[5]);
	if (hdrSize < 9 || hdrSize > 100)
	{
		return;
	}
	this->fd = fd->GetPartialData(0, fd->GetDataSize());

	Sync::Thread::Create(ParseThread, this);
	while (!this->threadStarted)
	{
		Sync::Thread::Sleep(10);
	}
}

IO::FileAnalyse::FLVFileAnalyse::~FLVFileAnalyse()
{
	if (this->threadRunning)
	{
		this->threadToStop = true;
		while (this->threadRunning)
		{
			Sync::Thread::Sleep(10);
		}
	}

	SDEL_CLASS(this->fd);
	LIST_FREE_FUNC(this->tags, MemFree);
	DEL_CLASS(this->tags);
}

UOSInt IO::FileAnalyse::FLVFileAnalyse::GetFrameCount()
{
	return 1 + this->tags->GetCount();
}

Bool IO::FileAnalyse::FLVFileAnalyse::GetFrameName(UOSInt index, Text::StringBuilderUTF *sb)
{
	if (index == 0)
	{
		sb->Append((const UTF8Char*)"FLV Header");
		return true;
	}
	IO::FileAnalyse::FLVFileAnalyse::FLVTag *tag = this->tags->GetItem(index - 1);
	if (tag == 0)
		return false;
	sb->AppendU64(tag->ofst);
	sb->Append((const UTF8Char*)": Type=");
	sb->AppendU16(tag->tagType);
	sb->Append((const UTF8Char*)", size=");
	sb->AppendUOSInt(tag->size);
	return true;
}

Bool IO::FileAnalyse::FLVFileAnalyse::GetFrameDetail(UOSInt index, Text::StringBuilderUTF *sb)
{
	UInt8 buff[128];
	UInt8 *tagData;
	if (index == 0)
	{
		this->fd->GetRealData(0, this->hdrSize, buff);
		sb->Append((const UTF8Char*)"Version = ");
		sb->AppendU16(buff[3]);
		sb->Append((const UTF8Char*)"\r\nTypeFlagsReserved = ");
		sb->AppendU16((UInt16)(buff[4] >> 3));
		sb->Append((const UTF8Char*)"\r\nTypeFlagsAudio = ");
		sb->AppendU16((UInt16)((buff[4] >> 2) & 1));
		sb->Append((const UTF8Char*)"\r\nTypeFlagsReserved = ");
		sb->AppendU16((UInt16)((buff[4] >> 1) & 1));
		sb->Append((const UTF8Char*)"\r\nTypeFlagsVideo = ");
		sb->AppendU16((UInt16)(buff[4] & 1));
		sb->Append((const UTF8Char*)"\r\nDataOffset = ");
		sb->AppendI32(ReadMInt32(&buff[5]));
		return true;
	}
	IO::FileAnalyse::FLVFileAnalyse::FLVTag *tag = this->tags->GetItem(index - 1);
	if (tag == 0)
		return false;
	sb->Append((const UTF8Char*)"Tag");
	sb->AppendUOSInt(index);
	this->fd->GetRealData(tag->ofst, 11, buff);
	sb->Append((const UTF8Char*)"\r\nReserved = ");
	sb->AppendU16((UInt16)(buff[0] >> 6));
	sb->Append((const UTF8Char*)"\r\nFilter = ");
	sb->AppendU16((UInt16)((buff[0] >> 5) & 1));
	sb->Append((const UTF8Char*)"\r\nTagType = ");
	sb->AppendU16(buff[0] & 0x1f);
	if (tag->tagType == 8)
	{
		sb->Append((const UTF8Char*)" (audio)");
	}
	else if (tag->tagType == 9)
	{
		sb->Append((const UTF8Char*)" (vidio)");
	}
	else if (tag->tagType == 18)
	{
		sb->Append((const UTF8Char*)" (script data)");
	}
	sb->Append((const UTF8Char*)"\r\nDataSize = ");
	sb->AppendU32(ReadMUInt24(&buff[1]));
	sb->Append((const UTF8Char*)"\r\nTimestamp = ");
	sb->AppendU32(ReadMUInt24(&buff[4]));
	sb->Append((const UTF8Char*)"\r\nTimestamp Extended = ");
	sb->AppendU16(buff[7]);
	if (tag->tagType == 8)
	{
		tagData = MemAlloc(UInt8, tag->size);
		this->fd->GetRealData(tag->ofst, tag->size, tagData);

		sb->Append((const UTF8Char*)"\r\n");
		if (tag->size >= 256 + 11)
		{
			sb->AppendHexBuff(tagData, 256, ' ', Text::LBT_CRLF);
		}
		else
		{
			sb->AppendHexBuff(tagData, tag->size - 11, ' ', Text::LBT_CRLF);
		}

		MemFree(tagData);
	}
	else if (tag->tagType == 9)
	{
		tagData = MemAlloc(UInt8, tag->size);
		this->fd->GetRealData(tag->ofst, tag->size, tagData);

		sb->Append((const UTF8Char*)"\r\n");
		if (tag->size >= 256 + 11)
		{
			sb->AppendHexBuff(tagData, 256, ' ', Text::LBT_CRLF);
		}
		else
		{
			sb->AppendHexBuff(tagData, tag->size - 11, ' ', Text::LBT_CRLF);
		}

		MemFree(tagData);
	}
	else if (tag->tagType == 18)
	{
		tagData = MemAlloc(UInt8, tag->size);
		this->fd->GetRealData(tag->ofst, tag->size, tagData);
		ParseScriptData(tagData, 11, tag->size, sb);
		MemFree(tagData);
	}
	return true;
}

Bool IO::FileAnalyse::FLVFileAnalyse::IsError()
{
	return this->fd == 0;
}

Bool IO::FileAnalyse::FLVFileAnalyse::IsParsing()
{
	return this->threadRunning;
}

Bool IO::FileAnalyse::FLVFileAnalyse::TrimPadding(const UTF8Char *outputFile)
{
	return false;
}
