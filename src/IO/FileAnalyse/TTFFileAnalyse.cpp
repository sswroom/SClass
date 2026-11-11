#include "Stdafx.h"
#include "Core/ByteTool_C.h"
#include "Data/ByteBuffer.h"
#include "IO/FileAnalyse/TTFFileAnalyse.h"
#include "Text/StringTool.h"

void __stdcall IO::FileAnalyse::TTFFileAnalyse::ParseThread(NN<Sync::Thread> thread)
{
	NN<IO::FileAnalyse::TTFFileAnalyse> me = thread->GetUserObj().GetNN<IO::FileAnalyse::TTFFileAnalyse>();
	NN<IO::StreamData> fd;
	UInt8 header[12];
	if (!me->fd.SetTo(fd) || fd->GetRealData(0, 12, BYTEARR(header)) != 12)
		return;

	UInt16 tableCount = ReadMUInt16(&header[4]);
	UIntOS directorySize = 12 + (UIntOS)tableCount * 16;
	Data::ByteBuffer directoryBuff(directorySize);
	if (fd->GetRealData(0, directorySize, directoryBuff) != directorySize)
		return;

	NN<PackInfo> pack;
	pack = MemAllocNN(PackInfo);
	pack->fileOfst = 0;
	pack->packSize = directorySize;
	pack->packSizePad = directorySize;
	pack->packType = PackType::Directory;
	WriteNUInt32(pack->tableTag, 0);
	me->packs.Add(pack);

	UIntOS i = 0;
	UIntOS fileSize = fd->GetDataSize();
	while (i < tableCount && !thread->IsStopping())
	{
		UIntOS entryOfst = 12 + i * 16;
		UInt32 tableOffset = ReadMUInt32(&directoryBuff[entryOfst + 8]);
		UInt32 tableLength = ReadMUInt32(&directoryBuff[entryOfst + 12]);
		UIntOS padding = tableLength & 3;
		if (tableLength > 0 && tableOffset >= directorySize && tableOffset <= fileSize && tableLength <= fileSize - tableOffset)
		{
			if (padding)
			{
				padding = 4 - padding;
			}
			pack = MemAllocNN(PackInfo);
			pack->fileOfst = tableOffset;
			pack->packSize = tableLength;
			pack->packSizePad = tableLength + padding;
			pack->packType = PackType::Table;
			WriteNUInt32(pack->tableTag, ReadNUInt32(&directoryBuff[entryOfst]));
			me->packs.Add(pack);
		}
		i++;
	}
}

void __stdcall IO::FileAnalyse::TTFFileAnalyse::FreePackInfo(NN<PackInfo> pack)
{
	MemFreeNN(pack);
}

IO::FileAnalyse::TTFFileAnalyse::TTFFileAnalyse(NN<IO::StreamData> fd) : thread(ParseThread, this, CSTR("TTFFileAnalyse"))
{
	UInt8 header[12];
	this->fd = nullptr;
	UInt64 fileSize = fd->GetDataSize();
	if (fileSize < 12 || fd->GetRealData(0, 12, BYTEARR(header)) != 12)
		return;

	UInt32 sfntVersion = ReadMUInt32(&header[0]);
	if (sfntVersion != 0x00010000 && sfntVersion != 0x4f54544f && sfntVersion != 0x74727565 && sfntVersion != 0x74797031)
		return;
	UInt16 tableCount = ReadMUInt16(&header[4]);
	UInt64 directorySize = 12 + (UInt64)tableCount * 16;
	if (directorySize > fileSize)
		return;

	this->fd = fd->GetPartialData(0, fileSize);
	this->thread.Start();
}

IO::FileAnalyse::TTFFileAnalyse::~TTFFileAnalyse()
{
	this->thread.Stop();
	this->fd.Delete();
	this->packs.FreeAll(FreePackInfo);
}

Text::CStringNN IO::FileAnalyse::TTFFileAnalyse::GetFormatName()
{
	return CSTR("TTF");
}

UIntOS IO::FileAnalyse::TTFFileAnalyse::GetFrameCount()
{
	return this->packs.GetCount();
}

Bool IO::FileAnalyse::TTFFileAnalyse::GetFrameName(UIntOS index, NN<Text::StringBuilderUTF8> sb)
{
	NN<PackInfo> pack;
	if (!this->packs.GetItem(index).SetTo(pack))
		return false;
	sb->AppendU64(pack->fileOfst);
	sb->AppendC(UTF8STRC(": "));
	if (pack->packType == PackType::Directory)
	{
		sb->AppendC(UTF8STRC("Offset table and table directory"));
	}
	else
	{
		if (Text::StringTool::IsTextASCII(Data::ByteArrayR(pack->tableTag, 4)))
		{
			sb->AppendC(UTF8STRC("Table tag="));
			sb->AppendC(pack->tableTag, 4);
		}
		else
		{
			sb->AppendC(UTF8STRC("Table tag=0x"));
			sb->AppendHex32(ReadMUInt32(pack->tableTag));
		}
	}
	sb->AppendC(UTF8STRC(", size="));
	sb->AppendUIntOS(pack->packSize);
	return true;
}

UIntOS IO::FileAnalyse::TTFFileAnalyse::GetFrameIndex(UInt64 ofst)
{
	UIntOS i = 0;
	UIntOS count = this->packs.GetCount();
	while (i < count)
	{
		NN<PackInfo> pack = this->packs.GetItemNoCheck(i);
		if (ofst >= pack->fileOfst && ofst - pack->fileOfst < pack->packSizePad)
			return i;
		i++;
	}
	return INVALID_INDEX;
}

Optional<IO::FileAnalyse::FrameDetail> IO::FileAnalyse::TTFFileAnalyse::GetFrameDetail(UIntOS index)
{
	NN<IO::StreamData> fd;
	NN<PackInfo> pack;
	if (!this->fd.SetTo(fd) || !this->packs.GetItem(index).SetTo(pack))
		return nullptr;

	NN<IO::FileAnalyse::FrameDetail> frame;
	UInt8 paddingBytes[3] = {0};
	UIntOS padding = pack->packSizePad - pack->packSize;
	NEW_CLASSNN(frame, IO::FileAnalyse::FrameDetail(pack->fileOfst, pack->packSizePad));
	if (pack->packType == PackType::Directory)
	{
		Data::ByteBuffer directoryBuff(pack->packSize);
		if (fd->GetRealData(pack->fileOfst, pack->packSize, directoryBuff) != pack->packSize)
		{
			frame.Delete();
			return nullptr;
		}
		frame->AddHeader(CSTR("SFNT offset table and table directory"));
		frame->AddUInt(0, 4, CSTR("SFNT version"), ReadMUInt32(&directoryBuff[0]));
		UInt16 tableCount = ReadMUInt16(&directoryBuff[4]);
		frame->AddUInt(4, 2, CSTR("Number of tables"), tableCount);
		frame->AddUInt(6, 2, CSTR("Search range"), ReadMUInt16(&directoryBuff[6]));
		frame->AddUInt(8, 2, CSTR("Entry selector"), ReadMUInt16(&directoryBuff[8]));
		frame->AddUInt(10, 2, CSTR("Range shift"), ReadMUInt16(&directoryBuff[10]));

		UIntOS i = 0;
		while (i < tableCount)
		{
			UIntOS entryOfst = 12 + i * 16;
			frame->AddStrC(entryOfst, 4, CSTR("Table tag"), &directoryBuff[entryOfst]);
			frame->AddUInt(entryOfst + 4, 4, CSTR("Checksum"), ReadMUInt32(&directoryBuff[entryOfst + 4]));
			frame->AddUInt(entryOfst + 8, 4, CSTR("Offset"), ReadMUInt32(&directoryBuff[entryOfst + 8]));
			frame->AddUInt(entryOfst + 12, 4, CSTR("Length"), ReadMUInt32(&directoryBuff[entryOfst + 12]));
			i++;
		}
	}
	else
	{
		Text::StringBuilderUTF8 sb;
		if (Text::StringTool::IsTextASCII(Data::ByteArrayR(pack->tableTag, 4)))
		{
			sb.AppendC(UTF8STRC("Table data, tag="));
			sb.AppendC(pack->tableTag, 4);
		}
		else
		{
			sb.AppendC(UTF8STRC("Table data, tag=0x"));
			sb.AppendHex32(ReadMUInt32(pack->tableTag));
		}
		frame->AddHeader(sb.ToCString());
		Data::ByteBuffer tableBuff(pack->packSize);
		if (fd->GetRealData(pack->fileOfst, pack->packSize, tableBuff) != pack->packSize)
		{
			frame.Delete();
			return nullptr;
		}
		if (Text::StrEqualsC(pack->tableTag, 4, UTF8STRC("head")) && pack->packSize >= 54)
		{
			frame->AddUInt(0, 4, CSTR("Version"), ReadMUInt32(&tableBuff[0]));
			frame->AddUInt(4, 4, CSTR("Font revision"), ReadMUInt32(&tableBuff[4]));
			frame->AddHex32(8, CSTR("Checksum adjustment"), ReadMUInt32(&tableBuff[8]));
			frame->AddHex32(12, CSTR("Magic number"), ReadMUInt32(&tableBuff[12]));
			frame->AddUInt(16, 2, CSTR("Flags"), ReadMUInt16(&tableBuff[16]));
			frame->AddUInt(18, 2, CSTR("Units per em"), ReadMUInt16(&tableBuff[18]));
			sb.ClearStr();
			sb.AppendTSNoZone(Data::Timestamp::FromEpochSec(ReadMInt64(&tableBuff[20]) - 2082844800, Data::DateTimeUtil::GetLocalTzQhr()));
			frame->AddHex64Name(20, CSTR("Created"), ReadMUInt64(&tableBuff[20]), sb.ToCString());
			sb.ClearStr();
			sb.AppendTSNoZone(Data::Timestamp::FromEpochSec(ReadMInt64(&tableBuff[28]) - 2082844800, Data::DateTimeUtil::GetLocalTzQhr()));
			frame->AddHex64Name(28, CSTR("Modified"), ReadMUInt64(&tableBuff[28]), sb.ToCString());
			frame->AddInt(36, 2, CSTR("X minimum"), ReadMInt16(&tableBuff[36]));
			frame->AddInt(38, 2, CSTR("Y minimum"), ReadMInt16(&tableBuff[38]));
			frame->AddInt(40, 2, CSTR("X maximum"), ReadMInt16(&tableBuff[40]));
			frame->AddInt(42, 2, CSTR("Y maximum"), ReadMInt16(&tableBuff[42]));
			frame->AddUInt(44, 2, CSTR("Mac style"), ReadMUInt16(&tableBuff[44]));
			frame->AddUInt(46, 2, CSTR("Lowest recommended PPEM"), ReadMUInt16(&tableBuff[46]));
			frame->AddInt(48, 2, CSTR("Font direction hint"), ReadMInt16(&tableBuff[48]));
			frame->AddInt(50, 2, CSTR("Location format"), ReadMInt16(&tableBuff[50]));
			frame->AddInt(52, 2, CSTR("Glyph data format"), ReadMInt16(&tableBuff[52]));
		}
		else if (Text::StrEqualsC(pack->tableTag, 4, UTF8STRC("hhea")) && pack->packSize >= 36)
		{
			frame->AddUInt(0, 4, CSTR("Version"), ReadMUInt32(&tableBuff[0]));
			frame->AddInt(4, 2, CSTR("Ascender"), ReadMInt16(&tableBuff[4]));
			frame->AddInt(6, 2, CSTR("Descender"), ReadMInt16(&tableBuff[6]));
			frame->AddInt(8, 2, CSTR("Line gap"), ReadMInt16(&tableBuff[8]));
			frame->AddUInt(10, 2, CSTR("Maximum advance width"), ReadMUInt16(&tableBuff[10]));
			frame->AddInt(12, 2, CSTR("Minimum left side bearing"), ReadMInt16(&tableBuff[12]));
			frame->AddInt(14, 2, CSTR("Minimum right side bearing"), ReadMInt16(&tableBuff[14]));
			frame->AddInt(16, 2, CSTR("Maximum extent"), ReadMInt16(&tableBuff[16]));
			frame->AddInt(18, 2, CSTR("Caret slope rise"), ReadMInt16(&tableBuff[18]));
			frame->AddInt(20, 2, CSTR("Caret slope run"), ReadMInt16(&tableBuff[20]));
			frame->AddInt(22, 2, CSTR("Caret offset"), ReadMInt16(&tableBuff[22]));
			frame->AddHexBuff(24, 8, CSTR("Reserved"), &tableBuff[24], false);
			frame->AddInt(32, 2, CSTR("Metric data format"), ReadMInt16(&tableBuff[32]));
			frame->AddUInt(34, 2, CSTR("Number of horizontal metrics"), ReadMUInt16(&tableBuff[34]));
		}
		else if (Text::StrEqualsC(pack->tableTag, 4, UTF8STRC("maxp")) && pack->packSize >= 6)
		{
			UInt32 version = ReadMUInt32(&tableBuff[0]);
			frame->AddUInt(0, 4, CSTR("Version"), version);
			frame->AddUInt(4, 2, CSTR("Number of glyphs"), ReadMUInt16(&tableBuff[4]));
			if (version == 0x00010000 && pack->packSize >= 32)
			{
				frame->AddUInt(6, 2, CSTR("Maximum points in a non-composite glyph"), ReadMUInt16(&tableBuff[6]));
				frame->AddUInt(8, 2, CSTR("Maximum contours in a non-composite glyph"), ReadMUInt16(&tableBuff[8]));
				frame->AddUInt(10, 2, CSTR("Maximum points in a composite glyph"), ReadMUInt16(&tableBuff[10]));
				frame->AddUInt(12, 2, CSTR("Maximum contours in a composite glyph"), ReadMUInt16(&tableBuff[12]));
				frame->AddUInt(14, 2, CSTR("Maximum zones"), ReadMUInt16(&tableBuff[14]));
				frame->AddUInt(16, 2, CSTR("Maximum twilight points"), ReadMUInt16(&tableBuff[16]));
				frame->AddUInt(18, 2, CSTR("Maximum storage"), ReadMUInt16(&tableBuff[18]));
				frame->AddUInt(20, 2, CSTR("Maximum function definitions"), ReadMUInt16(&tableBuff[20]));
				frame->AddUInt(22, 2, CSTR("Maximum instruction definitions"), ReadMUInt16(&tableBuff[22]));
				frame->AddUInt(24, 2, CSTR("Maximum stack elements"), ReadMUInt16(&tableBuff[24]));
				frame->AddUInt(26, 2, CSTR("Maximum instruction size"), ReadMUInt16(&tableBuff[26]));
				frame->AddUInt(28, 2, CSTR("Maximum component elements"), ReadMUInt16(&tableBuff[28]));
				frame->AddUInt(30, 2, CSTR("Maximum component depth"), ReadMUInt16(&tableBuff[30]));
			}
		}
		else if (Text::StrEqualsC(pack->tableTag, 4, UTF8STRC("OS/2")) && pack->packSize >= 2)
		{
			UInt16 version = ReadMUInt16(&tableBuff[0]);
			frame->AddUInt(0, 2, CSTR("Version"), version);
			if (pack->packSize >= 4) frame->AddInt(2, 2, CSTR("Average character width"), ReadMInt16(&tableBuff[2]));
			if (pack->packSize >= 6) frame->AddUInt(4, 2, CSTR("Weight class"), ReadMUInt16(&tableBuff[4]));
			if (pack->packSize >= 8) frame->AddUInt(6, 2, CSTR("Width class"), ReadMUInt16(&tableBuff[6]));
			if (pack->packSize >= 10) frame->AddHex16(8, CSTR("Embedding and licensing flags"), ReadMUInt16(&tableBuff[8]));
			if (pack->packSize >= 12) frame->AddInt(10, 2, CSTR("Subscript X size"), ReadMInt16(&tableBuff[10]));
			if (pack->packSize >= 14) frame->AddInt(12, 2, CSTR("Subscript Y size"), ReadMInt16(&tableBuff[12]));
			if (pack->packSize >= 16) frame->AddInt(14, 2, CSTR("Subscript X offset"), ReadMInt16(&tableBuff[14]));
			if (pack->packSize >= 18) frame->AddInt(16, 2, CSTR("Subscript Y offset"), ReadMInt16(&tableBuff[16]));
			if (pack->packSize >= 20) frame->AddInt(18, 2, CSTR("Superscript X size"), ReadMInt16(&tableBuff[18]));
			if (pack->packSize >= 22) frame->AddInt(20, 2, CSTR("Superscript Y size"), ReadMInt16(&tableBuff[20]));
			if (pack->packSize >= 24) frame->AddInt(22, 2, CSTR("Superscript X offset"), ReadMInt16(&tableBuff[22]));
			if (pack->packSize >= 26) frame->AddInt(24, 2, CSTR("Superscript Y offset"), ReadMInt16(&tableBuff[24]));
			if (pack->packSize >= 28) frame->AddInt(26, 2, CSTR("Strikeout size"), ReadMInt16(&tableBuff[26]));
			if (pack->packSize >= 30) frame->AddInt(28, 2, CSTR("Strikeout position"), ReadMInt16(&tableBuff[28]));
			if (pack->packSize >= 32) frame->AddInt(30, 2, CSTR("Family class"), ReadMInt16(&tableBuff[30]));
			if (pack->packSize >= 42) frame->AddHexBuff(32, 10, CSTR("PANOSE classification"), &tableBuff[32], false);
			if (pack->packSize >= 46) frame->AddHex32(42, CSTR("Unicode range 1"), ReadMUInt32(&tableBuff[42]));
			if (pack->packSize >= 50) frame->AddHex32(46, CSTR("Unicode range 2"), ReadMUInt32(&tableBuff[46]));
			if (pack->packSize >= 54) frame->AddHex32(50, CSTR("Unicode range 3"), ReadMUInt32(&tableBuff[50]));
			if (pack->packSize >= 58) frame->AddHex32(54, CSTR("Unicode range 4"), ReadMUInt32(&tableBuff[54]));
			if (pack->packSize >= 62) frame->AddStrC(58, 4, CSTR("Vendor ID"), &tableBuff[58]);
			if (pack->packSize >= 64) frame->AddHex16(62, CSTR("Selection flags"), ReadMUInt16(&tableBuff[62]));
			if (pack->packSize >= 66) frame->AddUInt(64, 2, CSTR("First character index"), ReadMUInt16(&tableBuff[64]));
			if (pack->packSize >= 68) frame->AddUInt(66, 2, CSTR("Last character index"), ReadMUInt16(&tableBuff[66]));
			if (pack->packSize >= 70) frame->AddInt(68, 2, CSTR("Typographic ascender"), ReadMInt16(&tableBuff[68]));
			if (pack->packSize >= 72) frame->AddInt(70, 2, CSTR("Typographic descender"), ReadMInt16(&tableBuff[70]));
			if (pack->packSize >= 74) frame->AddInt(72, 2, CSTR("Typographic line gap"), ReadMInt16(&tableBuff[72]));
			if (pack->packSize >= 76) frame->AddUInt(74, 2, CSTR("Windows ascent"), ReadMUInt16(&tableBuff[74]));
			if (pack->packSize >= 78) frame->AddUInt(76, 2, CSTR("Windows descent"), ReadMUInt16(&tableBuff[76]));
			if (version >= 1 && pack->packSize >= 82) frame->AddHex32(78, CSTR("Code page range 1"), ReadMUInt32(&tableBuff[78]));
			if (version >= 1 && pack->packSize >= 86) frame->AddHex32(82, CSTR("Code page range 2"), ReadMUInt32(&tableBuff[82]));
			if (version >= 2 && pack->packSize >= 88) frame->AddInt(86, 2, CSTR("Height of lowercase x"), ReadMInt16(&tableBuff[86]));
			if (version >= 2 && pack->packSize >= 90) frame->AddInt(88, 2, CSTR("Height of uppercase H"), ReadMInt16(&tableBuff[88]));
			if (version >= 2 && pack->packSize >= 92) frame->AddUInt(90, 2, CSTR("Default character"), ReadMUInt16(&tableBuff[90]));
			if (version >= 2 && pack->packSize >= 94) frame->AddUInt(92, 2, CSTR("Break character"), ReadMUInt16(&tableBuff[92]));
			if (version >= 2 && pack->packSize >= 96) frame->AddUInt(94, 2, CSTR("Maximum context"), ReadMUInt16(&tableBuff[94]));
			if (version >= 5 && pack->packSize >= 98) frame->AddUInt(96, 2, CSTR("Lower optical point size"), ReadMUInt16(&tableBuff[96]));
			if (version >= 5 && pack->packSize >= 100) frame->AddUInt(98, 2, CSTR("Upper optical point size"), ReadMUInt16(&tableBuff[98]));
		}
		else if (Text::StrEqualsC(pack->tableTag, 4, UTF8STRC("VDMX")))
		{
			if (pack->packSize < 6)
			{
				frame->AddHeader(CSTR("Truncated VDMX header"));
				frame->AddHexBuff(0, CSTR("Table data"), tableBuff, true);
			}
			else
			{
				UInt16 version = ReadMUInt16(&tableBuff[0]);
				UInt16 numRecs = ReadMUInt16(&tableBuff[2]);
				UInt16 numRatios = ReadMUInt16(&tableBuff[4]);
				frame->AddUInt(0, 2, CSTR("Version"), version);
				frame->AddUInt(2, 2, CSTR("Number of VDMX groups"), numRecs);
				frame->AddUInt(4, 2, CSTR("Number of ratio records"), numRatios);

				UIntOS ratioArrayOfst = 6;
				UIntOS ratioArraySize = (UIntOS)numRatios * 4;
				UIntOS offsetArrayOfst = ratioArrayOfst + ratioArraySize;
				if (ratioArraySize <= pack->packSize - ratioArrayOfst &&
					(UIntOS)numRecs <= (pack->packSize - offsetArrayOfst) / ((UIntOS)numRatios == 0 ? 1 : (UIntOS)numRatios * 2))
				{
					UIntOS i = 0;
					while (i < numRatios)
					{
						UIntOS ratioOfst = ratioArrayOfst + i * 4;
						Text::StringBuilderUTF8 fieldName;
						fieldName.AppendC(UTF8STRC("Character set, ratio "));
						fieldName.AppendUIntOS(i);
						frame->AddUInt(ratioOfst, 1, fieldName.ToCString(), tableBuff[ratioOfst]);
						fieldName.ClearStr();
						fieldName.AppendC(UTF8STRC("X ratio, record "));
						fieldName.AppendUIntOS(i);
						frame->AddUInt(ratioOfst + 1, 1, fieldName.ToCString(), tableBuff[ratioOfst + 1]);
						fieldName.ClearStr();
						fieldName.AppendC(UTF8STRC("Y start ratio, record "));
						fieldName.AppendUIntOS(i);
						frame->AddUInt(ratioOfst + 2, 1, fieldName.ToCString(), tableBuff[ratioOfst + 2]);
						fieldName.ClearStr();
						fieldName.AppendC(UTF8STRC("Y end ratio, record "));
						fieldName.AppendUIntOS(i);
						frame->AddUInt(ratioOfst + 3, 1, fieldName.ToCString(), tableBuff[ratioOfst + 3]);
						i++;
					}

					UIntOS offsetCount = (UIntOS)numRatios * numRecs;
					i = 0;
					while (i < offsetCount)
					{
						UIntOS offsetOfst = offsetArrayOfst + i * 2;
						UInt16 groupOfst = ReadMUInt16(&tableBuff[offsetOfst]);
						Text::StringBuilderUTF8 fieldName;
						fieldName.AppendC(UTF8STRC("VDMX group offset "));
						fieldName.AppendUIntOS(i);
						frame->AddUInt(offsetOfst, 2, fieldName.ToCString(), groupOfst);

						if (groupOfst >= offsetArrayOfst + offsetCount * 2 && groupOfst <= pack->packSize && pack->packSize - groupOfst >= 4)
						{
							UInt16 groupRecordCount = ReadMUInt16(&tableBuff[groupOfst]);
							UIntOS availableSize = pack->packSize - groupOfst - 4;
							Text::StringBuilderUTF8 groupName;
							groupName.AppendC(UTF8STRC("VDMX group "));
							groupName.AppendUIntOS(i);
							frame->AddHeader(groupName.ToCString());
							frame->AddUInt(groupOfst, 2, CSTR("Number of entries"), groupRecordCount);
							frame->AddUInt(groupOfst + 2, 1, CSTR("Starting pixel size"), tableBuff[groupOfst + 2]);
							frame->AddUInt(groupOfst + 3, 1, CSTR("Ending pixel size"), tableBuff[groupOfst + 3]);
							UIntOS availableRecords = availableSize / 5;
							if (availableRecords > groupRecordCount)
								availableRecords = groupRecordCount;
							UIntOS j = 0;
							while (j < availableRecords)
							{
								UIntOS recordOfst = groupOfst + 4 + j * 5;
								frame->AddUInt(recordOfst, 1, CSTR("Pixel size"), tableBuff[recordOfst]);
								frame->AddInt(recordOfst + 1, 2, CSTR("Maximum y value"), ReadMInt16(&tableBuff[recordOfst + 1]));
								frame->AddInt(recordOfst + 3, 2, CSTR("Minimum y value"), ReadMInt16(&tableBuff[recordOfst + 3]));
								j++;
							}
							if (availableRecords < groupRecordCount)
								frame->AddHeader(CSTR("Truncated VDMX group entries"));
						}
						i++;
					}
				}
				else
				{
					frame->AddHeader(CSTR("Truncated VDMX ratio or offset arrays"));
					frame->AddHexBuff(0, CSTR("Table data"), tableBuff, true);
				}
			}
		}
		else if (Text::StrEqualsC(pack->tableTag, 4, UTF8STRC("LTSH")) && pack->packSize >= 4)
		{
			UInt16 version = ReadMUInt16(&tableBuff[0]);
			UInt16 glyphCount = ReadMUInt16(&tableBuff[2]);
			frame->AddUInt(0, 2, CSTR("Version"), version);
			frame->AddUInt(2, 2, CSTR("Number of glyphs"), glyphCount);
			if ((UIntOS)glyphCount <= pack->packSize - 4)
			{
				UIntOS i = 0;
				while (i < glyphCount)
				{
					Text::StringBuilderUTF8 fieldName;
					fieldName.AppendC(UTF8STRC("Minimum PPEM, glyph "));
					fieldName.AppendUIntOS(i);
					frame->AddUInt(4 + i, 1, fieldName.ToCString(), tableBuff[4 + i]);
					i++;
				}
			}
			else
			{
				frame->AddHeader(CSTR("Truncated LTSH glyph data"));
				frame->AddHexBuff(0, CSTR("Table data"), tableBuff, true);
			}
		}
		else if (Text::StrEqualsC(pack->tableTag, 4, UTF8STRC("hmtx")))
		{
			NN<PackInfo> hheaPack;
			NN<PackInfo> maxpPack;
			Optional<PackInfo> hheaOpt = nullptr;
			Optional<PackInfo> maxpOpt = nullptr;
			UIntOS packCnt = this->packs.GetCount();
			UIntOS i = 0;
			while (i < packCnt && (!hheaOpt.SetTo(hheaPack) || !maxpOpt.SetTo(maxpPack)))
			{
				NN<PackInfo> tablePack = this->packs.GetItemNoCheck(i);
				if (tablePack->packType == PackType::Table)
				{
					if (Text::StrEqualsC(tablePack->tableTag, 4, UTF8STRC("hhea")))
						hheaOpt = tablePack;
					else if (Text::StrEqualsC(tablePack->tableTag, 4, UTF8STRC("maxp")))
						maxpOpt = tablePack;
				}
				i++;
			}

			UInt8 metricCountBuff[2];
			UInt8 glyphCountBuff[2];
			UInt16 metricCount = 0;
			UInt16 glyphCount = 0;
			if (hheaOpt.SetTo(hheaPack) && maxpOpt.SetTo(maxpPack) &&
				hheaPack->packSize >= 36 && maxpPack->packSize >= 6 &&
				fd->GetRealData(hheaPack->fileOfst + 34, 2, BYTEARR(metricCountBuff)) == 2 &&
				fd->GetRealData(maxpPack->fileOfst + 4, 2, BYTEARR(glyphCountBuff)) == 2)
			{
				metricCount = ReadMUInt16(metricCountBuff);
				glyphCount = ReadMUInt16(glyphCountBuff);
			}

			if (metricCount > 0 && metricCount <= glyphCount &&
				(UIntOS)metricCount * 4 + ((UIntOS)glyphCount - metricCount) * 2 <= pack->packSize)
			{
				frame->AddHeader(CSTR("Horizontal metrics"));
				i = 0;
				while (i < metricCount)
				{
					UIntOS entryOfst = i * 4;
					Text::StringBuilderUTF8 fieldName;
					fieldName.AppendC(UTF8STRC("Advance width, glyph "));
					fieldName.AppendUIntOS(i);
					frame->AddUInt(entryOfst, 2, fieldName.ToCString(), ReadMUInt16(&tableBuff[entryOfst]));
					fieldName.ClearStr();
					fieldName.AppendC(UTF8STRC("Left side bearing, glyph "));
					fieldName.AppendUIntOS(i);
					frame->AddInt(entryOfst + 2, 2, fieldName.ToCString(), ReadMInt16(&tableBuff[entryOfst + 2]));
					i++;
				}
				while (i < glyphCount)
				{
					UIntOS entryOfst = (UIntOS)metricCount * 4 + (i - metricCount) * 2;
					Text::StringBuilderUTF8 fieldName;
					fieldName.AppendC(UTF8STRC("Left side bearing, glyph "));
					fieldName.AppendUIntOS(i);
					frame->AddInt(entryOfst, 2, fieldName.ToCString(), ReadMInt16(&tableBuff[entryOfst]));
					i++;
				}
			}
			else
			{
				frame->AddHeader(CSTR("Unable to decode horizontal metrics (missing or invalid hhea/maxp data)"));
				frame->AddHexBuff(0, CSTR("Table data"), tableBuff, true);
			}
		}
	}
	if (padding > 0)
	{
		frame->AddHexBuff(pack->packSize, CSTR("Padding"), Data::ByteArrayR(paddingBytes, padding), false);
	}
	return frame;
}

Bool IO::FileAnalyse::TTFFileAnalyse::IsError()
{
	return this->fd.IsNull();
}

Bool IO::FileAnalyse::TTFFileAnalyse::IsParsing()
{
	return this->thread.IsRunning();
}

Bool IO::FileAnalyse::TTFFileAnalyse::TrimPadding(Text::CStringNN outputFile)
{
	return false;
}
