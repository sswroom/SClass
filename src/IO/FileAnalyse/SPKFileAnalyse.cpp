#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "IO/FileAnalyse/SPKFileAnalyse.h"
#include "Manage/Process.h"
#include "Net/SocketFactory.h"
#include "Sync/Thread.h"

void IO::FileAnalyse::SPKFileAnalyse::ParseV1Directory(UInt64 dirOfst, UInt64 dirSize)
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

		IO::FileAnalyse::SPKFileAnalyse::PackInfo *pack = MemAlloc(IO::FileAnalyse::SPKFileAnalyse::PackInfo, 1);
		pack->fileOfst = fileOfst;
		pack->packSize = (UOSInt)fileSize;
		pack->packType = PT_FILE;
		pack->fileName = Text::StrCopyNewC(&buff[ofst + 26], fileNameSize);
		this->packs->Add(pack);
		ofst += 26 + (UOSInt)fileNameSize;
	}
	MemFree(buff);
}

void IO::FileAnalyse::SPKFileAnalyse::ParseV2Directory(UInt64 dirOfst, UInt64 dirSize)
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

	IO::FileAnalyse::SPKFileAnalyse::PackInfo *pack = MemAlloc(IO::FileAnalyse::SPKFileAnalyse::PackInfo, 1);
	pack->fileOfst = dirOfst;
	pack->packSize = (UOSInt)dirSize;
	pack->packType = PT_V2DIRECTORY;
	pack->fileName = 0;
	this->packs->Add(pack);
}

UInt32 __stdcall IO::FileAnalyse::SPKFileAnalyse::ParseThread(void *userObj)
{
	IO::FileAnalyse::SPKFileAnalyse *me = (IO::FileAnalyse::SPKFileAnalyse*)userObj;
	UInt8 buff[256];
	IO::FileAnalyse::SPKFileAnalyse::PackInfo *pack;
	me->threadRunning = true;
	me->threadStarted = true;

	me->fd->GetRealData(0, 256, buff);
	Int32 flags = ReadInt32(&buff[4]);
	UOSInt endOfst;
	UInt64 lastOfst = ReadUInt64(&buff[8]);
	UInt64 lastSize;
	PackType dirType;
	if (flags & 2)
	{
		lastSize = ReadUInt64(&buff[16]);
		endOfst = 24;
		dirType = PT_V2DIRECTORY;
	}
	else
	{
		lastSize = 0;
		endOfst = 16;
		dirType = PT_V1DIRECTORY;
	}
	if (flags & 1)
	{
		UInt32 customSize = ReadUInt32(&buff[endOfst + 4]);
		endOfst += 8 + customSize;
	}

	pack = MemAlloc(IO::FileAnalyse::SPKFileAnalyse::PackInfo, 1);
	pack->fileOfst = 0;
	pack->packSize = endOfst;
	pack->packType = PT_HEADER;
	pack->fileName = 0;
	me->packs->Add(pack);
	if (dirType == PT_V1DIRECTORY)
	{
		me->ParseV1Directory(lastOfst, me->fd->GetDataSize() - lastOfst);

		pack = MemAlloc(IO::FileAnalyse::SPKFileAnalyse::PackInfo, 1);
		pack->fileOfst = lastOfst;
		pack->packSize = (UOSInt)(me->fd->GetDataSize() - lastOfst);
		pack->packType = PT_V1DIRECTORY;
		pack->fileName = 0;
		me->packs->Add(pack);
	}
	else if (dirType == PT_V2DIRECTORY)
	{
		me->ParseV2Directory(lastOfst, lastSize);
	}

	me->threadRunning = false;
	return 0;
}

void IO::FileAnalyse::SPKFileAnalyse::FreePackInfo(IO::FileAnalyse::SPKFileAnalyse::PackInfo *pack)
{
	SDEL_TEXT(pack->fileName);
	MemFree(pack);
}

IO::FileAnalyse::SPKFileAnalyse::SPKFileAnalyse(IO::IStreamData *fd)
{
	UInt8 buff[256];
	this->fd = 0;
	this->threadRunning = false;
	this->pauseParsing = false;
	this->threadToStop = false;
	this->threadStarted = false;
	NEW_CLASS(this->extMap, Data::Int32Map<UInt8*>());

	NEW_CLASS(this->packs, Data::SyncArrayList<IO::FileAnalyse::SPKFileAnalyse::PackInfo*>());
	fd->GetRealData(0, 256, buff);
	if (buff[0] != 'S' || buff[1] != 'm' || buff[2] != 'p' || buff[3] != 'f')
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

IO::FileAnalyse::SPKFileAnalyse::~SPKFileAnalyse()
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
	LIST_FREE_FUNC(this->packs, FreePackInfo);
	Data::ArrayList<UInt8*> *extList = this->extMap->GetValues();
	LIST_FREE_FUNC(extList, MemFree);
	DEL_CLASS(this->extMap);
	DEL_CLASS(this->packs);
}

const UTF8Char *IO::FileAnalyse::SPKFileAnalyse::GetFormatName()
{
	return (const UTF8Char*)"SPK";
}

UOSInt IO::FileAnalyse::SPKFileAnalyse::GetFrameCount()
{
	return this->packs->GetCount();
}

Bool IO::FileAnalyse::SPKFileAnalyse::GetFrameName(UOSInt index, Text::StringBuilderUTF *sb)
{
	IO::FileAnalyse::SPKFileAnalyse::PackInfo *pack;
	pack = this->packs->GetItem(index);
	if (pack == 0)
		return false;
	sb->AppendU64(pack->fileOfst);
	sb->Append((const UTF8Char*)": Type=");
	if (pack->packType == PT_HEADER)
	{
		sb->Append((const UTF8Char*)"File header");
	}
	else if (pack->packType == PT_V1DIRECTORY)
	{
		sb->Append((const UTF8Char*)"V1 Directory");
	}
	else if (pack->packType == PT_V2DIRECTORY)
	{
		sb->Append((const UTF8Char*)"V2 Directory");
	}
	else if (pack->packType == PT_FILE)
	{
		sb->Append((const UTF8Char*)"Data Block");
	}
	sb->Append((const UTF8Char*)", size=");
	sb->AppendI32((Int32)pack->packSize);
	if (pack->fileName)
	{
		sb->AppendChar(' ', 1);
		sb->Append(pack->fileName);
	}
	return true;
}

Bool IO::FileAnalyse::SPKFileAnalyse::GetFrameDetail(UOSInt index, Text::StringBuilderUTF *sb)
{
	IO::FileAnalyse::SPKFileAnalyse::PackInfo *pack;
	UInt8 *packBuff;
	pack = this->packs->GetItem(index);
	if (pack == 0)
		return false;

	sb->AppendU64(pack->fileOfst);
	sb->Append((const UTF8Char*)": Type=");
	if (pack->packType == PT_HEADER)
	{
		sb->Append((const UTF8Char*)"File header");
	}
	else if (pack->packType == PT_V1DIRECTORY)
	{
		sb->Append((const UTF8Char*)"V1 Directory");
	}
	else if (pack->packType == PT_V2DIRECTORY)
	{
		sb->Append((const UTF8Char*)"V2 Directory");
	}
	else if (pack->packType == PT_FILE)
	{
		sb->Append((const UTF8Char*)"Data Block");
	}
	sb->Append((const UTF8Char*)", size=");
	sb->AppendI32((Int32)pack->packSize);

	if (pack->fileName)
	{
		sb->Append((const UTF8Char*)"\r\nFile Name = ");
		sb->Append(pack->fileName);
	}

	if (pack->packType == PT_HEADER)
	{
		UInt32 flags;
		UOSInt endOfst;
		packBuff = MemAlloc(UInt8, pack->packSize);
		this->fd->GetRealData(pack->fileOfst, pack->packSize, packBuff);

		sb->Append((const UTF8Char*)"\r\nFlags = 0x");
		sb->AppendHex32V(flags = ReadUInt32(&packBuff[4]));
		sb->Append((const UTF8Char*)"\r\nLast Directory Offset = ");
		sb->AppendI64(ReadInt64(&packBuff[8]));
		endOfst = 16;
		if (flags & 2)
		{
			sb->Append((const UTF8Char*)"\r\nLast Directory Size = ");
			sb->AppendI64(ReadInt64(&packBuff[16]));
			endOfst = 24;
		}
		if (flags & 1)
		{
			Int32 customType;
			UInt32 customSize;
			sb->Append((const UTF8Char*)"\r\nCustom Type = ");
			sb->AppendI32(customType = ReadInt32(&packBuff[endOfst]));
			sb->Append((const UTF8Char*)"\r\nCustom Size = ");
			sb->AppendU32(customSize = ReadUInt32(&packBuff[endOfst + 4]));
			if (customType == 1)
			{
				UOSInt customOfst;
				UInt8 urlCnt;
				UInt8 urlI;
				sb->Append((const UTF8Char*)"\r\n-OSM Tile:");
				sb->Append((const UTF8Char*)"\r\n-Number of URL = ");
				sb->AppendU16(urlCnt = packBuff[endOfst + 8]);
				urlI = 0;
				customOfst = 1;
				while (urlI < urlCnt)
				{
					sb->Append((const UTF8Char*)"\r\n-URL");
					sb->AppendU16(urlI);
					sb->Append((const UTF8Char*)" = ");
					sb->AppendC(&packBuff[endOfst + 8 + customOfst + 1], packBuff[endOfst + 8 + customOfst]);
					customOfst += (UOSInt)packBuff[endOfst + 8 + customOfst] + 1;
					urlI++;
				}
			}
			else
			{
				sb->Append((const UTF8Char*)"\r\nCustom data = ");
				sb->AppendHexBuff(&packBuff[endOfst + 8], customSize, ' ', Text::LBT_NONE);
			}

		}
		MemFree(packBuff);
	}
	else if (pack->packType == PT_V1DIRECTORY)
	{
		packBuff = MemAlloc(UInt8, pack->packSize);
		this->fd->GetRealData(pack->fileOfst, pack->packSize, packBuff);
		this->GetDetailDirs(packBuff, pack->packSize, sb);
		MemFree(packBuff);
	}
	else if (pack->packType == PT_V2DIRECTORY)
	{
		packBuff = MemAlloc(UInt8, pack->packSize);
		this->fd->GetRealData(pack->fileOfst, pack->packSize, packBuff);
		sb->Append((const UTF8Char*)"\r\nPrev Directory Offset = ");
		sb->AppendI64(ReadInt64(&packBuff[0]));
		sb->Append((const UTF8Char*)"\r\nPrev Directory Size = ");
		sb->AppendI64(ReadInt64(&packBuff[8]));
		this->GetDetailDirs(packBuff + 16, pack->packSize - 16, sb);
		MemFree(packBuff);
	}
	else if (pack->packType == PT_FILE)
	{
		if (pack->packSize <= 64)
		{
			packBuff = MemAlloc(UInt8, pack->packSize);
			this->fd->GetRealData(pack->fileOfst, pack->packSize, packBuff);
			sb->Append((const UTF8Char*)"\r\n");
			sb->AppendHexBuff(packBuff, pack->packSize, ' ', Text::LBT_CRLF);
			MemFree(packBuff);
		}
		else
		{
			UInt8 buff[32];
			sb->Append((const UTF8Char*)"\r\n");
			this->fd->GetRealData(pack->fileOfst, 32, buff);
			sb->AppendHexBuff(buff, 32, ' ', Text::LBT_CRLF);
			sb->Append((const UTF8Char*)"\r\n...\r\n");
			this->fd->GetRealData(pack->fileOfst + pack->packSize - 32, 32, buff);
			sb->AppendHexBuff(buff, 32, ' ', Text::LBT_CRLF);
		}
	}
	return true;
}

UOSInt IO::FileAnalyse::SPKFileAnalyse::GetFrameIndex(UInt64 ofst)
{
	OSInt i = 0;
	OSInt j = (OSInt)this->packs->GetCount() - 1;
	OSInt k;
	PackInfo *pack;
	while (i <= j)
	{
		k = (i + j) >> 1;
		pack = this->packs->GetItem((UOSInt)k);
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

IO::FileAnalyse::FrameDetail *IO::FileAnalyse::SPKFileAnalyse::GetFrameDetail(UOSInt index)
{
	IO::FileAnalyse::FrameDetail *frame;
	IO::FileAnalyse::SPKFileAnalyse::PackInfo *pack;
	UInt8 *packBuff;
	pack = this->packs->GetItem(index);
	if (pack == 0)
		return 0;

	NEW_CLASS(frame, IO::FileAnalyse::FrameDetail(pack->fileOfst, pack->packSize));
	return frame;
/*	
	sb->AppendU64(pack->fileOfst);
	sb->Append((const UTF8Char*)": Type=");
	if (pack->packType == PT_HEADER)
	{
		sb->Append((const UTF8Char*)"File header");
	}
	else if (pack->packType == PT_V1DIRECTORY)
	{
		sb->Append((const UTF8Char*)"V1 Directory");
	}
	else if (pack->packType == PT_V2DIRECTORY)
	{
		sb->Append((const UTF8Char*)"V2 Directory");
	}
	else if (pack->packType == PT_FILE)
	{
		sb->Append((const UTF8Char*)"Data Block");
	}
	sb->Append((const UTF8Char*)", size=");
	sb->AppendI32((Int32)pack->packSize);

	if (pack->fileName)
	{
		sb->Append((const UTF8Char*)"\r\nFile Name = ");
		sb->Append(pack->fileName);
	}

	if (pack->packType == PT_HEADER)
	{
		UInt32 flags;
		UOSInt endOfst;
		packBuff = MemAlloc(UInt8, pack->packSize);
		this->fd->GetRealData(pack->fileOfst, pack->packSize, packBuff);

		sb->Append((const UTF8Char*)"\r\nFlags = 0x");
		sb->AppendHex32V(flags = ReadUInt32(&packBuff[4]));
		sb->Append((const UTF8Char*)"\r\nLast Directory Offset = ");
		sb->AppendI64(ReadInt64(&packBuff[8]));
		endOfst = 16;
		if (flags & 2)
		{
			sb->Append((const UTF8Char*)"\r\nLast Directory Size = ");
			sb->AppendI64(ReadInt64(&packBuff[16]));
			endOfst = 24;
		}
		if (flags & 1)
		{
			Int32 customType;
			UInt32 customSize;
			sb->Append((const UTF8Char*)"\r\nCustom Type = ");
			sb->AppendI32(customType = ReadInt32(&packBuff[endOfst]));
			sb->Append((const UTF8Char*)"\r\nCustom Size = ");
			sb->AppendU32(customSize = ReadUInt32(&packBuff[endOfst + 4]));
			if (customType == 1)
			{
				UOSInt customOfst;
				UInt8 urlCnt;
				UInt8 urlI;
				sb->Append((const UTF8Char*)"\r\n-OSM Tile:");
				sb->Append((const UTF8Char*)"\r\n-Number of URL = ");
				sb->AppendU16(urlCnt = packBuff[endOfst + 8]);
				urlI = 0;
				customOfst = 1;
				while (urlI < urlCnt)
				{
					sb->Append((const UTF8Char*)"\r\n-URL");
					sb->AppendU16(urlI);
					sb->Append((const UTF8Char*)" = ");
					sb->AppendC(&packBuff[endOfst + 8 + customOfst + 1], packBuff[endOfst + 8 + customOfst]);
					customOfst += (UOSInt)packBuff[endOfst + 8 + customOfst] + 1;
					urlI++;
				}
			}
			else
			{
				sb->Append((const UTF8Char*)"\r\nCustom data = ");
				sb->AppendHexBuff(&packBuff[endOfst + 8], customSize, ' ', Text::LBT_NONE);
			}

		}
		MemFree(packBuff);
	}
	else if (pack->packType == PT_V1DIRECTORY)
	{
		packBuff = MemAlloc(UInt8, pack->packSize);
		this->fd->GetRealData(pack->fileOfst, pack->packSize, packBuff);
		this->GetDetailDirs(packBuff, pack->packSize, sb);
		MemFree(packBuff);
	}
	else if (pack->packType == PT_V2DIRECTORY)
	{
		packBuff = MemAlloc(UInt8, pack->packSize);
		this->fd->GetRealData(pack->fileOfst, pack->packSize, packBuff);
		sb->Append((const UTF8Char*)"\r\nPrev Directory Offset = ");
		sb->AppendI64(ReadInt64(&packBuff[0]));
		sb->Append((const UTF8Char*)"\r\nPrev Directory Size = ");
		sb->AppendI64(ReadInt64(&packBuff[8]));
		this->GetDetailDirs(packBuff + 16, pack->packSize - 16, sb);
		MemFree(packBuff);
	}
	else if (pack->packType == PT_FILE)
	{
		if (pack->packSize <= 64)
		{
			packBuff = MemAlloc(UInt8, pack->packSize);
			this->fd->GetRealData(pack->fileOfst, pack->packSize, packBuff);
			sb->Append((const UTF8Char*)"\r\n");
			sb->AppendHexBuff(packBuff, pack->packSize, ' ', Text::LBT_CRLF);
			MemFree(packBuff);
		}
		else
		{
			UInt8 buff[32];
			sb->Append((const UTF8Char*)"\r\n");
			this->fd->GetRealData(pack->fileOfst, 32, buff);
			sb->AppendHexBuff(buff, 32, ' ', Text::LBT_CRLF);
			sb->Append((const UTF8Char*)"\r\n...\r\n");
			this->fd->GetRealData(pack->fileOfst + pack->packSize - 32, 32, buff);
			sb->AppendHexBuff(buff, 32, ' ', Text::LBT_CRLF);
		}
	}
	return true;*/
}

Bool IO::FileAnalyse::SPKFileAnalyse::IsError()
{
	return this->fd == 0;
}

Bool IO::FileAnalyse::SPKFileAnalyse::IsParsing()
{
	return this->threadRunning;
}

Bool IO::FileAnalyse::SPKFileAnalyse::TrimPadding(const UTF8Char *outputFile)
{
	return false;
}

void IO::FileAnalyse::SPKFileAnalyse::GetDetailDirs(const UInt8 *dirBuff, UOSInt dirSize, Text::StringBuilderUTF *sb)
{
	UOSInt ofst = 0;
	while (dirSize - ofst >= 26)
	{
		UInt64 fileOfst = ReadUInt64(&dirBuff[ofst]);
		UInt64 fileSize = ReadUInt64(&dirBuff[ofst + 8]);
		UInt16 fileNameSize = ReadUInt16(&dirBuff[ofst + 24]);
		if (dirSize - ofst - 26 < fileNameSize)
		{
			break;
		}

		sb->Append((const UTF8Char*)"\r\nFile Ofst = ");
		sb->AppendU64(fileOfst);
		sb->Append((const UTF8Char*)", Size = ");
		sb->AppendU64(fileSize);
		sb->Append((const UTF8Char*)", Name = ");
		sb->AppendC(&dirBuff[ofst + 26], fileNameSize);
		ofst += 26 + (UOSInt)fileNameSize;
	}
}
