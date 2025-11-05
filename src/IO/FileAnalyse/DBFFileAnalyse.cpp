#include "Stdafx.h"
#include "Data/ByteBuffer.h"
#include "Data/ByteTool.h"
#include "IO/FileAnalyse/DBFFileAnalyse.h"
#include "Sync/MutexUsage.h"
#include "Text/StringBuilderUTF8.h"

void __stdcall IO::FileAnalyse::DBFFileAnalyse::ParseThread(NN<Sync::Thread> thread)
{
	NN<IO::FileAnalyse::DBFFileAnalyse> me = thread->GetUserObj().GetNN<IO::FileAnalyse::DBFFileAnalyse>();
	NN<IO::StreamData> fd;
	if (me->fd.SetTo(fd))
	{
		UInt8 buff[256];
		NN<IO::FileAnalyse::DBFFileAnalyse::PackInfo> pack;
		UInt32 val;
		pack = MemAllocNN(IO::FileAnalyse::DBFFileAnalyse::PackInfo);
		pack->fileOfst = 0;
		pack->packSize = 64;
		pack->packType = PackType::TableHeader;
		me->packs.Add(pack);
		fd->GetRealData(0, 64, BYTEARR(buff));
	}
}

IO::FileAnalyse::DBFFileAnalyse::DBFFileAnalyse(NN<IO::StreamData> fd) : thread(ParseThread, this, CSTR("DBFFileAnalyse"))
{
	UInt8 buff[8];
	this->fd = 0;
	this->pauseParsing = false;
	this->imageSize = 0;
	fd->GetRealData(0, 8, BYTEARR(buff));
	if (ReadInt16(buff) != 0x5A4D)
	{
		return;
	}
	this->fd = fd->GetPartialData(0, fd->GetDataSize());
	this->thread.Start();
}

IO::FileAnalyse::DBFFileAnalyse::~DBFFileAnalyse()
{
	this->thread.Stop();
	this->fd.Delete();
	this->packs.MemFreeAll();
}

Text::CStringNN IO::FileAnalyse::DBFFileAnalyse::GetFormatName()
{
	return CSTR("DBF");
}

UOSInt IO::FileAnalyse::DBFFileAnalyse::GetFrameCount()
{
	return this->packs.GetCount();
}

Bool IO::FileAnalyse::DBFFileAnalyse::GetFrameName(UOSInt index, NN<Text::StringBuilderUTF8> sb)
{
	NN<IO::FileAnalyse::DBFFileAnalyse::PackInfo> pack;
	if (!this->packs.GetItem(index).SetTo(pack))
		return false;
	sb->AppendU64(pack->fileOfst);
	sb->AppendC(UTF8STRC(": Type="));
	sb->Append(PackTypeGetName(pack->packType));
	sb->AppendC(UTF8STRC(", size="));
	sb->AppendU64(pack->packSize);
	return true;
}

UOSInt IO::FileAnalyse::DBFFileAnalyse::GetFrameIndex(UInt64 ofst)
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

Optional<IO::FileAnalyse::FrameDetail> IO::FileAnalyse::DBFFileAnalyse::GetFrameDetail(UOSInt index)
{
	NN<IO::FileAnalyse::DBFFileAnalyse::PackInfo> pack;
	NN<IO::StreamData> fd;
	Text::CString vName;
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	if (!this->packs.GetItem(index).SetTo(pack) || !this->fd.SetTo(fd))
		return 0;

	NN<IO::FileAnalyse::FrameDetail> frame;
	NEW_CLASSNN(frame, IO::FileAnalyse::FrameDetail(pack->fileOfst, pack->packSize));
	sptr = PackTypeGetName(pack->packType).ConcatTo(Text::StrConcatC(sbuff, UTF8STRC("Type=")));
	frame->AddText(0, CSTRP(sbuff, sptr));
	sptr = Text::StrUInt64(Text::StrConcatC(sbuff, UTF8STRC("Size=")), pack->packSize);
	frame->AddText(0, CSTRP(sbuff, sptr));

	if (pack->packType == PackType::TableHeader)
	{
		Data::ByteBuffer packBuff((UOSInt)pack->packSize);
		fd->GetRealData(pack->fileOfst, (UOSInt)pack->packSize, packBuff);
		frame->AddHex16(0, CSTR("Magic number"), ReadUInt16(&packBuff[0]));
		frame->AddUInt(2, 2, CSTR("Bytes on last page of file"), ReadUInt16(&packBuff[2]));
		frame->AddUInt(4, 2, CSTR("Pages in file"), ReadUInt16(&packBuff[4]));
		frame->AddUInt(6, 2, CSTR("Relocations"), ReadUInt16(&packBuff[6]));
		frame->AddUInt(8, 2, CSTR("Size of header in paragraphs"), ReadUInt16(&packBuff[8]));
		frame->AddUInt(10, 2, CSTR("Minimum extra paragraphs needed"), ReadUInt16(&packBuff[10]));
		frame->AddUInt(12, 2, CSTR("Maximum extra paragraphs needed"), ReadUInt16(&packBuff[12]));
		frame->AddHex16(14, CSTR("Initial (relative) SS value"), ReadUInt16(&packBuff[14]));
		frame->AddHex16(16, CSTR("Initial SP value"), ReadUInt16(&packBuff[16]));
		frame->AddHex16(18, CSTR("Checksum"), ReadUInt16(&packBuff[18]));
		frame->AddHex16(20, CSTR("Initial IP value"), ReadUInt16(&packBuff[20]));
		frame->AddHex16(22, CSTR("Initial (relative) CS value"), ReadUInt16(&packBuff[22]));
		frame->AddHex16(24, CSTR("File address of relocation table"), ReadUInt16(&packBuff[24]));
		frame->AddUInt(26, 2, CSTR("Overlay number"), ReadUInt16(&packBuff[26]));
		frame->AddUInt(36, 2, CSTR("OEM identifier"), ReadUInt16(&packBuff[36]));
		frame->AddUInt(38, 2, CSTR("OEM information"), ReadUInt16(&packBuff[38]));
		frame->AddHex32(60, CSTR("File address of new exe header"), ReadUInt32(&packBuff[60]));
	}
	return frame;
}

Bool IO::FileAnalyse::DBFFileAnalyse::IsError()
{
	return this->fd.IsNull();
}

Bool IO::FileAnalyse::DBFFileAnalyse::IsParsing()
{
	return this->thread.IsRunning();
}

Bool IO::FileAnalyse::DBFFileAnalyse::TrimPadding(Text::CStringNN outputFile)
{
	return false;
}

Text::CStringNN IO::FileAnalyse::DBFFileAnalyse::PackTypeGetName(PackType packType)
{
	switch (packType)
	{
	case PackType::TableHeader:
		return CSTR("Table Header");
	case PackType::FieldProperties:
		return CSTR("Field Properties");
	case PackType::TableRecord:
		return CSTR("Table Record");
	default:
		return CSTR("Unknown");
	}
}
