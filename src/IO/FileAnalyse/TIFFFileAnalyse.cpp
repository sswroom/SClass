#include "Stdafx.h"
#include "Data/ByteOrderLSB.h"
#include "Data/ByteOrderMSB.h"
#include "Data/ByteTool.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "IO/FileAnalyse/TIFFFileAnalyse.h"
#include "Manage/Process.h"
#include "Net/SocketFactory.h"
#include "Sync/Thread.h"

void IO::FileAnalyse::TIFFFileAnalyse::ParseV1Directory(UInt64 dirOfst, UInt64 dirSize)
{
	if (dirSize < 26)
		return;
	UOSInt ofst = 0;
	UInt8 *buff;
	buff = MemAlloc(UInt8, (UOSInt)dirSize);
	this->fd->GetRealData(dirOfst, (UOSInt)dirSize, buff);
	while (dirSize - ofst >= 26)
	{
		UInt64 fileOfst = ReadUInt64(&buff[ofst]);
		UInt64 fileSize = ReadUInt64(&buff[ofst + 8]);
		UInt16 fileNameSize = ReadUInt16(&buff[ofst + 24]);
		if (dirSize - ofst - 26 < fileNameSize)
		{
			break;
		}

		PackInfo *pack = MemAlloc(PackInfo, 1);
		pack->fileOfst = fileOfst;
		pack->packSize = (UOSInt)fileSize;
		pack->packType = PT_FILE;
		pack->fileName = Text::String::New(&buff[ofst + 26], fileNameSize);
		this->packs.Add(pack);
		ofst += 26 + (UOSInt)fileNameSize;
	}
	MemFree(buff);
}

void IO::FileAnalyse::TIFFFileAnalyse::ParseV2Directory(UInt64 dirOfst, UInt64 dirSize)
{
	if (dirOfst <= 0 || dirSize < 16)
	{
		return;
	}
	while (this->pauseParsing && !this->threadToStop)
	{
		Sync::Thread::Sleep(100);
	}
	if (this->threadToStop)
	{
		return;
	}
	UInt8 buff[16];
	this->fd->GetRealData(dirOfst, 16, buff);
	this->ParseV2Directory(ReadUInt64(&buff[0]), ReadUInt64(&buff[8]));
	this->ParseV1Directory(dirOfst + 16, dirSize - 16);

	PackInfo *pack = MemAlloc(PackInfo, 1);
	pack->fileOfst = dirOfst;
	pack->packSize = (UOSInt)dirSize;
	pack->packType = PT_V2DIRECTORY;
	pack->fileName = 0;
	this->packs.Add(pack);
}

UInt32 __stdcall IO::FileAnalyse::TIFFFileAnalyse::ParseThread(void *userObj)
{
	IO::FileAnalyse::TIFFFileAnalyse *me = (IO::FileAnalyse::TIFFFileAnalyse *)userObj;
	UInt8 buff[256];
	PackInfo *pack;
	me->threadRunning = true;
	me->threadStarted = true;

	me->fd->GetRealData(0, 256, buff);
	UInt16 fmt = me->bo->GetUInt16(&buff[2]);
	if (fmt == 42)
	{
	}
	else if (fmt == 43 && me->bo->GetUInt16(&buff[4]) == 8 && me->bo->GetUInt16(&buff[6]) == 0) //BigTIFF
	{
	}
	else
	{
	}
	

	me->threadRunning = false;
	return 0;
}

void IO::FileAnalyse::TIFFFileAnalyse::FreePackInfo(PackInfo *pack)
{
	SDEL_STRING(pack->fileName);
	MemFree(pack);
}

IO::FileAnalyse::TIFFFileAnalyse::TIFFFileAnalyse(IO::StreamData *fd)
{
	UInt8 buff[256];
	this->fd = 0;
	this->bo = 0;
	this->threadRunning = false;
	this->pauseParsing = false;
	this->threadToStop = false;
	this->threadStarted = false;

	fd->GetRealData(0, 256, buff);
	if (*(Int16*)&buff[0] == *(Int16*)"MM")
	{
		NEW_CLASS(this->bo, Data::ByteOrderMSB());
	}
	else if (*(Int16*)&buff[0] == *(Int16*)"II")
	{
		NEW_CLASS(this->bo, Data::ByteOrderLSB());
	}
	else
	{
		return;
	}
	UInt16 fmt = bo->GetUInt16(&buff[2]);
	if (fmt == 42)
	{
	}
	else if (fmt == 43 && bo->GetUInt16(&buff[4]) == 8 && bo->GetUInt16(&buff[6]) == 0) //BigTIFF
	{
	}
	else
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

IO::FileAnalyse::TIFFFileAnalyse::~TIFFFileAnalyse()
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
	SDEL_CLASS(this->bo);
	LIST_FREE_FUNC(&this->packs, FreePackInfo);
}

Text::CString IO::FileAnalyse::TIFFFileAnalyse::GetFormatName()
{
	return CSTR("TIFF");
}

UOSInt IO::FileAnalyse::TIFFFileAnalyse::GetFrameCount()
{
	return this->packs.GetCount();
}

Bool IO::FileAnalyse::TIFFFileAnalyse::GetFrameName(UOSInt index, Text::StringBuilderUTF8 *sb)
{
	PackInfo *pack;
	pack = this->packs.GetItem(index);
	if (pack == 0)
		return false;
	sb->AppendU64(pack->fileOfst);
	sb->AppendC(UTF8STRC(": Type="));
	if (pack->packType == PT_HEADER)
	{
		sb->AppendC(UTF8STRC("File header"));
	}
	else if (pack->packType == PT_V1DIRECTORY)
	{
		sb->AppendC(UTF8STRC("V1 Directory"));
	}
	else if (pack->packType == PT_V2DIRECTORY)
	{
		sb->AppendC(UTF8STRC("V2 Directory"));
	}
	else if (pack->packType == PT_FILE)
	{
		sb->AppendC(UTF8STRC("Data Block"));
	}
	sb->AppendC(UTF8STRC(", size="));
	sb->AppendI32((Int32)pack->packSize);
	if (pack->fileName)
	{
		sb->AppendUTF8Char(' ');
		sb->Append(pack->fileName);
	}
	return true;
}

UOSInt IO::FileAnalyse::TIFFFileAnalyse::GetFrameIndex(UInt64 ofst)
{
	OSInt i = 0;
	OSInt j = (OSInt)this->packs.GetCount() - 1;
	OSInt k;
	PackInfo *pack;
	while (i <= j)
	{
		k = (i + j) >> 1;
		pack = this->packs.GetItem((UOSInt)k);
		if (ofst < pack->fileOfst)
		{
			j = k - 1;
		}
		else if (ofst >= pack->fileOfst + pack->packSize)
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

IO::FileAnalyse::FrameDetail *IO::FileAnalyse::TIFFFileAnalyse::GetFrameDetail(UOSInt index)
{
	IO::FileAnalyse::FrameDetail *frame;
	PackInfo *pack;
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	UInt8 *packBuff;
	pack = this->packs.GetItem(index);
	if (pack == 0)
		return 0;

	NEW_CLASS(frame, IO::FileAnalyse::FrameDetail(pack->fileOfst, (UInt32)pack->packSize));
	if (pack->packType == PT_HEADER)
	{
		frame->AddText(0, CSTR("Type=File header"));
	}
	else if (pack->packType == PT_V1DIRECTORY)
	{
		frame->AddText(0, CSTR("Type=V1 Directory"));
	}
	else if (pack->packType == PT_V2DIRECTORY)
	{
		frame->AddText(0, CSTR("Type=V2 Directory"));
	}
	else if (pack->packType == PT_FILE)
	{
		frame->AddText(0, CSTR("Type=Data Block"));
	}
	sptr = Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("Size=")), pack->packSize);
	frame->AddText(0, CSTRP(sbuff, sptr));

	if (pack->fileName)
	{
		Text::StringBuilderUTF8 sb;
		sb.AppendC(UTF8STRC("File Name="));
		sb.Append(pack->fileName);
		frame->AddText(0, sb.ToCString());
	}

	if (pack->packType == PT_HEADER)
	{
		UInt32 flags;
		UOSInt endOfst;
		packBuff = MemAlloc(UInt8, pack->packSize);
		this->fd->GetRealData(pack->fileOfst, pack->packSize, packBuff);

		frame->AddHex32(4, CSTR("Flags"), flags = ReadUInt32(&packBuff[4]));
		frame->AddUInt64(8, CSTR("Last Directory Offset"), ReadUInt64(&packBuff[8]));
		endOfst = 16;
		if (flags & 2)
		{
			frame->AddUInt64(16, CSTR("Last Directory Size"), ReadUInt64(&packBuff[16]));
			endOfst = 24;
		}
		if (flags & 1)
		{
			Int32 customType;
			UInt32 customSize;
			frame->AddInt(endOfst, 4, CSTR("Custom Type"), customType = ReadInt32(&packBuff[endOfst]));
			frame->AddUInt(endOfst + 4, 4, CSTR("Custom Size"), customSize = ReadUInt32(&packBuff[endOfst + 4]));
			if (customType == 1)
			{
				UOSInt customOfst;
				UInt8 urlCnt;
				UInt8 urlI;
				frame->AddText(endOfst + 8, CSTR("-OSM Tile:"));
				frame->AddUInt(endOfst + 8, 1, CSTR("-Number of URL"), urlCnt = packBuff[endOfst + 8]);
				urlI = 0;
				customOfst = 1;
				while (urlI < urlCnt)
				{
					sptr = Text::StrUInt16(Text::StrConcatC(sbuff, UTF8STRC("-Length")), urlI);
					frame->AddUInt(endOfst + 8 + customOfst, 1, CSTRP(sbuff, sptr), packBuff[endOfst + 8 + customOfst]);
					sptr = Text::StrUInt16(Text::StrConcatC(sbuff, UTF8STRC("-URL")), urlI);
					frame->AddStrC(endOfst + 8 + customOfst + 1, packBuff[endOfst + 8 + customOfst], CSTRP(sbuff, sptr), &packBuff[endOfst + 8 + customOfst + 1]);
					customOfst += (UOSInt)packBuff[endOfst + 8 + customOfst] + 1;
					urlI++;
				}
			}
			else
			{
				frame->AddHexBuff(endOfst + 8, customSize, CSTR("Custom data"), &packBuff[endOfst + 8], false);
			}
		}
		MemFree(packBuff);
	}
	else if (pack->packType == PT_V1DIRECTORY)
	{
		packBuff = MemAlloc(UInt8, pack->packSize);
		this->fd->GetRealData(pack->fileOfst, pack->packSize, packBuff);
		MemFree(packBuff);
	}
	else if (pack->packType == PT_V2DIRECTORY)
	{
		packBuff = MemAlloc(UInt8, pack->packSize);
		this->fd->GetRealData(pack->fileOfst, pack->packSize, packBuff);
		frame->AddUInt64(0, CSTR("Prev Directory Offset"), ReadUInt64(&packBuff[0]));
		frame->AddUInt64(8, CSTR("Prev Directory Size"), ReadUInt64(&packBuff[8]));
		MemFree(packBuff);
	}
	else if (pack->packType == PT_FILE)
	{
		if (pack->packSize <= 64)
		{
			packBuff = MemAlloc(UInt8, pack->packSize);
			this->fd->GetRealData(pack->fileOfst, pack->packSize, packBuff);
			frame->AddHexBuff(0, pack->packSize, CSTR("FileData"), packBuff, true);
			MemFree(packBuff);
		}
		else
		{
			UInt8 buff[32];
			Text::StringBuilderUTF8 sb;
			this->fd->GetRealData(pack->fileOfst, 32, buff);
			sb.AppendHexBuff(buff, 32, ' ', Text::LineBreakType::CRLF);
			sb.AppendC(UTF8STRC("\r\n...\r\n"));
			this->fd->GetRealData(pack->fileOfst + pack->packSize - 32, 32, buff);
			sb.AppendHexBuff(buff, 32, ' ', Text::LineBreakType::CRLF);
			frame->AddField(0, pack->packSize, CSTR("FileData"), sb.ToCString());
		}
	}
	return frame;
}

Bool IO::FileAnalyse::TIFFFileAnalyse::IsError()
{
	return this->fd == 0;
}

Bool IO::FileAnalyse::TIFFFileAnalyse::IsParsing()
{
	return this->threadRunning;
}

Bool IO::FileAnalyse::TIFFFileAnalyse::TrimPadding(Text::CString outputFile)
{
	return false;
}
