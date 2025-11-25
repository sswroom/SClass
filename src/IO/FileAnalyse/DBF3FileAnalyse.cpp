#include "Stdafx.h"
#include "Data/ByteBuffer.h"
#include "Core/ByteTool_C.h"
#include "IO/FileAnalyse/DBF3FileAnalyse.h"
#include "Sync/MutexUsage.h"
#include "Text/StringBuilderUTF8.h"

void __stdcall IO::FileAnalyse::DBF3FileAnalyse::ParseThread(NN<Sync::Thread> thread)
{
	NN<IO::FileAnalyse::DBF3FileAnalyse> me = thread->GetUserObj().GetNN<IO::FileAnalyse::DBF3FileAnalyse>();
	NN<IO::StreamData> fd;
	if (me->fd.SetTo(fd))
	{
		UInt8 buff[256];
		fd->GetRealData(0, 32, BYTEARR(buff));
		UInt32 nRec = ReadUInt32(&buff[4]);
		UInt32 startOfst = ReadUInt16(&buff[8]);
		UInt32 recLen = ReadUInt16(&buff[10]);
		NN<DBFCol> col;
		NN<IO::FileAnalyse::DBF3FileAnalyse::PackInfo> pack;
		pack = MemAllocNN(IO::FileAnalyse::DBF3FileAnalyse::PackInfo);
		pack->fileOfst = 0;
		pack->packSize = 32;
		pack->packType = PackType::TableHeader;
		me->packs.Add(pack);
		UOSInt currColOfst = 1;
		UInt64 ofst = 32;
		while (!thread->IsStopping() && ofst < startOfst)
		{
			fd->GetRealData(ofst, 32, BYTEARR(buff));
			if (buff[0] == 13)
			{
				pack = MemAllocNN(IO::FileAnalyse::DBF3FileAnalyse::PackInfo);
				pack->fileOfst = ofst;
				pack->packSize = 1;
				pack->packType = PackType::Terminator;
				me->packs.Add(pack);
				break;
			}
			else
			{
				col = MemAllocNN(DBFCol);
				col->name = Text::String::New(buff, 11);
				col->name->RTrim();
				col->type = buff[11];
				col->colSize = buff[16];
				col->colDP = buff[17];
				col->colOfst = currColOfst;
				currColOfst += col->colSize;
				me->cols.Add(col);

				pack = MemAllocNN(IO::FileAnalyse::DBF3FileAnalyse::PackInfo);
				pack->fileOfst = ofst;
				pack->packSize = 32;
				pack->packType = PackType::FieldSubrecords;
				me->packs.Add(pack);
				ofst += 32;
			}
		}
		ofst = startOfst;
		UOSInt i = 0;
		while (i < nRec)
		{
			pack = MemAllocNN(IO::FileAnalyse::DBF3FileAnalyse::PackInfo);
			pack->fileOfst = ofst;
			pack->packSize = recLen;
			pack->packType = PackType::TableRecord;
			me->packs.Add(pack);
			ofst += recLen;
			i++;
		}
		pack = MemAllocNN(IO::FileAnalyse::DBF3FileAnalyse::PackInfo);
		pack->fileOfst = ofst;
		pack->packSize = 1;
		pack->packType = PackType::Terminator;
		me->packs.Add(pack);
	}
}

void __stdcall IO::FileAnalyse::DBF3FileAnalyse::FreeCol(NN<DBFCol> col)
{
	col->name->Release();
	MemFreeNN(col);
}

IO::FileAnalyse::DBF3FileAnalyse::DBF3FileAnalyse(NN<IO::StreamData> fd) : thread(ParseThread, this, CSTR("DBF3FileAnalyse"))
{
	UInt8 buff[32];
	this->fd = 0;
	this->pauseParsing = false;
	fd->GetRealData(0, 32, BYTEARR(buff));
	if (buff[0] != 0x3)
	{
		return;
	}
	UInt32 nRec = ReadUInt32(&buff[4]);
	UInt32 startOfst = ReadUInt16(&buff[8]);
	UInt32 recLen = ReadUInt16(&buff[10]);
	if (fd->GetDataSize() != nRec * recLen + startOfst + 1)
	{
		return;
	}
	this->fd = fd->GetPartialData(0, fd->GetDataSize());
	this->thread.Start();
}

IO::FileAnalyse::DBF3FileAnalyse::~DBF3FileAnalyse()
{
	this->thread.Stop();
	this->fd.Delete();
	this->packs.MemFreeAll();
	this->cols.FreeAll(FreeCol);
}

Text::CStringNN IO::FileAnalyse::DBF3FileAnalyse::GetFormatName()
{
	return CSTR("DBF3");
}

UOSInt IO::FileAnalyse::DBF3FileAnalyse::GetFrameCount()
{
	return this->packs.GetCount();
}

Bool IO::FileAnalyse::DBF3FileAnalyse::GetFrameName(UOSInt index, NN<Text::StringBuilderUTF8> sb)
{
	NN<IO::FileAnalyse::DBF3FileAnalyse::PackInfo> pack;
	if (!this->packs.GetItem(index).SetTo(pack))
		return false;
	sb->AppendU64(pack->fileOfst);
	sb->AppendC(UTF8STRC(": Type="));
	sb->Append(PackTypeGetName(pack->packType));
	sb->AppendC(UTF8STRC(", size="));
	sb->AppendU64(pack->packSize);
	return true;
}

UOSInt IO::FileAnalyse::DBF3FileAnalyse::GetFrameIndex(UInt64 ofst)
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

Optional<IO::FileAnalyse::FrameDetail> IO::FileAnalyse::DBF3FileAnalyse::GetFrameDetail(UOSInt index)
{
	NN<IO::FileAnalyse::DBF3FileAnalyse::PackInfo> pack;
	NN<IO::StreamData> fd;
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
		Text::StringBuilderUTF8 sb;
		Data::ByteBuffer packBuff((UOSInt)pack->packSize);
		fd->GetRealData(pack->fileOfst, (UOSInt)pack->packSize, packBuff);
		frame->AddHex8(0, CSTR("DBF File Type"), packBuff[0]);
		sb.AppendU32(1900 + packBuff[1])->AppendUTF8Char('-')->AppendU16(packBuff[2])->AppendUTF8Char('-')->AppendU16(packBuff[3]);
		frame->AddField(1, 3, CSTR("Last update"), sb.ToCString());
		frame->AddUInt(4, 4, CSTR("Number of records in file"), ReadUInt32(&packBuff[4]));
		frame->AddUInt(8, 2, CSTR("Position of first data record"), ReadUInt16(&packBuff[8]));
		frame->AddUInt(10, 2, CSTR("Length of one data record, including delete flag"), ReadUInt16(&packBuff[10]));
		frame->AddHexBuff(12, CSTR("Reserved"), packBuff.SubArray(12, 16), false);
		frame->AddHex8(28, CSTR("Table flags"), packBuff[28]);
		frame->AddBit(28, CSTR("file has a structural .cdx"), packBuff[28], 0);
		frame->AddBit(28, CSTR("file has a Memo field"), packBuff[28], 1);
		frame->AddBit(28, CSTR("file is a database (.dbc)"), packBuff[28], 2);
		frame->AddHex8(29, CSTR("Code page mark"), packBuff[29]);
		frame->AddHexBuff(30, CSTR("Reserved"), packBuff.SubArray(30, 2), false);
	}
	else if (pack->packType == PackType::FieldSubrecords)
	{
		Data::ByteBuffer packBuff((UOSInt)pack->packSize);
		fd->GetRealData(pack->fileOfst, (UOSInt)pack->packSize, packBuff);
		frame->AddStrS(0, 11, CSTR("Field name"), &packBuff[0]);
		frame->AddStrC(11, 1, CSTR("Field type"), &packBuff[11]);
		frame->AddUInt(12, 4, CSTR("Displacement of field in record"), ReadUInt32(&packBuff[12]));
		frame->AddUInt(16, 1, CSTR("Length of field (in bytes)"), packBuff[16]);
		frame->AddUInt(17, 1, CSTR("Number of decimal places"), ReadUInt16(&packBuff[8]));
		frame->AddHex8(18, CSTR("Field flags"), packBuff[18]);
		frame->AddBit(18, CSTR("System Column (not visible to user)"), packBuff[18], 0);
		frame->AddBit(18, CSTR("Column can store null values"), packBuff[18], 1);
		frame->AddBit(18, CSTR("Binary column (for CHAR and MEMO only) "), packBuff[18], 2);
		frame->AddBit(18, CSTR("Column is autoincrementing"), packBuff[18], 3);
		frame->AddUInt(19, 4, CSTR("Value of autoincrement Next value"), ReadUInt32(&packBuff[19]));
		frame->AddUInt(23, 1, CSTR("Value of autoincrement Step value"), packBuff[23]);
		frame->AddHexBuff(24, CSTR("Reserved"), packBuff.SubArray(24, 8), false);
	}
	else if (pack->packType == PackType::TableRecord)
	{
		Data::ByteBuffer packBuff((UOSInt)pack->packSize);
		fd->GetRealData(pack->fileOfst, (UOSInt)pack->packSize, packBuff);
		frame->AddField(0, 1, CSTR("Record deleted"), (packBuff[0] == '*')?CSTR("Yes"):CSTR("No"));
		UOSInt i = 0;
		UOSInt j = this->cols.GetCount();
		NN<DBFCol> col;
		while (i < j)
		{
			col = this->cols.GetItemNoCheck(i);
			frame->AddStrS(col->colOfst, col->colSize, col->name->ToCString(), &packBuff[col->colOfst]);
			i++;
		}
	}
	else if (pack->packType == PackType::Terminator)
	{
		Data::ByteBuffer packBuff((UOSInt)pack->packSize);
		fd->GetRealData(pack->fileOfst, (UOSInt)pack->packSize, packBuff);
		frame->AddHex8(0, CSTR("Terminator"), packBuff[0]);
	}
	return frame;
}

Bool IO::FileAnalyse::DBF3FileAnalyse::IsError()
{
	return this->fd.IsNull();
}

Bool IO::FileAnalyse::DBF3FileAnalyse::IsParsing()
{
	return this->thread.IsRunning();
}

Bool IO::FileAnalyse::DBF3FileAnalyse::TrimPadding(Text::CStringNN outputFile)
{
	return false;
}

Text::CStringNN IO::FileAnalyse::DBF3FileAnalyse::PackTypeGetName(PackType packType)
{
	switch (packType)
	{
	case PackType::TableHeader:
		return CSTR("File Header");
	case PackType::FieldSubrecords:
		return CSTR("Field Subrecords Structure");
	case PackType::Terminator:
		return CSTR("Terminator");
	case PackType::TableRecord:
		return CSTR("Table Record");
	default:
		return CSTR("Unknown");
	}
}
