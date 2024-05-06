#include "Stdafx.h"
#include "Data/ByteBuffer.h"
#include "Data/ByteTool.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "IO/FileAnalyse/SPKFileAnalyse.h"
#include "Manage/Process.h"
#include "Net/SocketFactory.h"
#include "Sync/SimpleThread.h"

void IO::FileAnalyse::SPKFileAnalyse::ParseV1Directory(UInt64 dirOfst, UInt64 dirSize)
{
	if (dirSize < 26)
		return;
	UOSInt ofst = 0;
	Data::ByteBuffer buff((UOSInt)dirSize);
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

		NN<IO::FileAnalyse::SPKFileAnalyse::PackInfo> pack = MemAllocNN(IO::FileAnalyse::SPKFileAnalyse::PackInfo);
		pack->fileOfst = fileOfst;
		pack->packSize = (UOSInt)fileSize;
		pack->packType = PT_FILE;
		pack->fileName = Text::String::New(&buff[ofst + 26], fileNameSize).Ptr();
		this->packs.Add(pack);
		ofst += 26 + (UOSInt)fileNameSize;
	}
}

void IO::FileAnalyse::SPKFileAnalyse::ParseV2Directory(UInt64 dirOfst, UInt64 dirSize)
{
	if (dirOfst <= 0 || dirSize < 16)
	{
		return;
	}
	while (this->pauseParsing && !this->thread.IsStopping())
	{
		Sync::SimpleThread::Sleep(100);
	}
	if (this->thread.IsStopping())
	{
		return;
	}
	UInt8 buff[16];
	this->fd->GetRealData(dirOfst, 16, BYTEARR(buff));
	this->ParseV2Directory(ReadUInt64(&buff[0]), ReadUInt64(&buff[8]));
	this->ParseV1Directory(dirOfst + 16, dirSize - 16);

	NN<IO::FileAnalyse::SPKFileAnalyse::PackInfo> pack = MemAllocNN(IO::FileAnalyse::SPKFileAnalyse::PackInfo);
	pack->fileOfst = dirOfst;
	pack->packSize = (UOSInt)dirSize;
	pack->packType = PT_V2DIRECTORY;
	pack->fileName = 0;
	this->packs.Add(pack);
}

void __stdcall IO::FileAnalyse::SPKFileAnalyse::ParseThread(NN<Sync::Thread> thread)
{
	NN<IO::FileAnalyse::SPKFileAnalyse> me = thread->GetUserObj().GetNN<IO::FileAnalyse::SPKFileAnalyse>();
	UInt8 buff[256];
	NN<IO::FileAnalyse::SPKFileAnalyse::PackInfo> pack;
	me->fd->GetRealData(0, 256, BYTEARR(buff));
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

	pack = MemAllocNN(IO::FileAnalyse::SPKFileAnalyse::PackInfo);
	pack->fileOfst = 0;
	pack->packSize = endOfst;
	pack->packType = PT_HEADER;
	pack->fileName = 0;
	me->packs.Add(pack);
	if (dirType == PT_V1DIRECTORY)
	{
		me->ParseV1Directory(lastOfst, me->fd->GetDataSize() - lastOfst);

		pack = MemAllocNN(IO::FileAnalyse::SPKFileAnalyse::PackInfo);
		pack->fileOfst = lastOfst;
		pack->packSize = (UOSInt)(me->fd->GetDataSize() - lastOfst);
		pack->packType = PT_V1DIRECTORY;
		pack->fileName = 0;
		me->packs.Add(pack);
	}
	else if (dirType == PT_V2DIRECTORY)
	{
		me->ParseV2Directory(lastOfst, lastSize);
	}
}

void IO::FileAnalyse::SPKFileAnalyse::FreePackInfo(NN<IO::FileAnalyse::SPKFileAnalyse::PackInfo> pack)
{
	SDEL_STRING(pack->fileName);
	MemFreeNN(pack);
}

IO::FileAnalyse::SPKFileAnalyse::SPKFileAnalyse(NN<IO::StreamData> fd) : thread(ParseThread, this, CSTR("SPKFileAnalyse"))
{
	UInt8 buff[256];
	this->fd = 0;
	this->pauseParsing = false;

	fd->GetRealData(0, 256, BYTEARR(buff));
	if (buff[0] != 'S' || buff[1] != 'm' || buff[2] != 'p' || buff[3] != 'f')
	{
		return;
	}
	this->fd = fd->GetPartialData(0, fd->GetDataSize()).Ptr();
	this->thread.Start();
}

IO::FileAnalyse::SPKFileAnalyse::~SPKFileAnalyse()
{
	this->thread.Stop();
	SDEL_CLASS(this->fd);
	this->packs.FreeAll(FreePackInfo);
}

Text::CStringNN IO::FileAnalyse::SPKFileAnalyse::GetFormatName()
{
	return CSTR("SPK");
}

UOSInt IO::FileAnalyse::SPKFileAnalyse::GetFrameCount()
{
	return this->packs.GetCount();
}

Bool IO::FileAnalyse::SPKFileAnalyse::GetFrameName(UOSInt index, NN<Text::StringBuilderUTF8> sb)
{
	NN<IO::FileAnalyse::SPKFileAnalyse::PackInfo> pack;
	if (!this->packs.GetItem(index).SetTo(pack))
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

UOSInt IO::FileAnalyse::SPKFileAnalyse::GetFrameIndex(UInt64 ofst)
{
	OSInt i = 0;
	OSInt j = (OSInt)this->packs.GetCount() - 1;
	OSInt k;
	NN<PackInfo> pack;
	while (i <= j)
	{
		k = (i + j) >> 1;
		pack = this->packs.GetItemNoCheck((UOSInt)k);
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

Optional<IO::FileAnalyse::FrameDetail> IO::FileAnalyse::SPKFileAnalyse::GetFrameDetail(UOSInt index)
{
	NN<IO::FileAnalyse::FrameDetail> frame;
	NN<IO::FileAnalyse::SPKFileAnalyse::PackInfo> pack;
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	if (!this->packs.GetItem(index).SetTo(pack))
		return 0;

	NEW_CLASSNN(frame, IO::FileAnalyse::FrameDetail(pack->fileOfst, pack->packSize));
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
		Data::ByteBuffer packBuff(pack->packSize);
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
	}
	else if (pack->packType == PT_V1DIRECTORY)
	{
		Data::ByteBuffer packBuff(pack->packSize);
		this->fd->GetRealData(pack->fileOfst, pack->packSize, packBuff);
		this->GetDetailDirs(packBuff.Ptr(), pack->packSize, 0, frame);
	}
	else if (pack->packType == PT_V2DIRECTORY)
	{
		Data::ByteBuffer packBuff(pack->packSize);
		this->fd->GetRealData(pack->fileOfst, pack->packSize, packBuff);
		frame->AddUInt64(0, CSTR("Prev Directory Offset"), ReadUInt64(&packBuff[0]));
		frame->AddUInt64(8, CSTR("Prev Directory Size"), ReadUInt64(&packBuff[8]));
		this->GetDetailDirs(packBuff.Ptr() + 16, pack->packSize - 16, 16, frame);
	}
	else if (pack->packType == PT_FILE)
	{
		if (pack->packSize <= 64)
		{
			Data::ByteBuffer packBuff(pack->packSize);
			this->fd->GetRealData(pack->fileOfst, pack->packSize, packBuff);
			frame->AddHexBuff(0, CSTR("FileData"), packBuff, true);
		}
		else
		{
			UInt8 buff[32];
			Text::StringBuilderUTF8 sb;
			this->fd->GetRealData(pack->fileOfst, 32, BYTEARR(buff));
			sb.AppendHexBuff(buff, 32, ' ', Text::LineBreakType::CRLF);
			sb.AppendC(UTF8STRC("\r\n...\r\n"));
			this->fd->GetRealData(pack->fileOfst + pack->packSize - 32, 32, BYTEARR(buff));
			sb.AppendHexBuff(buff, 32, ' ', Text::LineBreakType::CRLF);
			frame->AddField(0, pack->packSize, CSTR("FileData"), sb.ToCString());
		}
	}
	return frame;
}

Bool IO::FileAnalyse::SPKFileAnalyse::IsError()
{
	return this->fd == 0;
}

Bool IO::FileAnalyse::SPKFileAnalyse::IsParsing()
{
	return this->thread.IsRunning();
}

Bool IO::FileAnalyse::SPKFileAnalyse::TrimPadding(Text::CStringNN outputFile)
{
	return false;
}

void IO::FileAnalyse::SPKFileAnalyse::GetDetailDirs(UnsafeArray<const UInt8> dirBuff, UOSInt dirSize, UOSInt frameOfst, NN<IO::FileAnalyse::FrameDetail> frame)
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

		frame->AddUInt64(frameOfst, CSTR("File Offset"), fileOfst);
		frame->AddUInt64(frameOfst + 8, CSTR("File Size"), fileSize);
		frame->AddUInt64(frameOfst + 16, CSTR("Reserved"), ReadUInt64(&dirBuff[ofst + 16]));
		frame->AddUInt(frameOfst + 24, 2, CSTR("File Name Size"), fileNameSize);
		frame->AddStrC(frameOfst + 26, fileNameSize, CSTR("File Name"), &dirBuff[ofst + 26]);
		ofst += 26 + (UOSInt)fileNameSize;
		frameOfst += 26 + (UOSInt)fileNameSize;
	}
}
