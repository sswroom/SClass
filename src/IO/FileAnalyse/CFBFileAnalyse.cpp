#include "Stdafx.h"
#include "Core/ByteTool_C.h"
#include "Data/ArrayListUInt32.h"
#include "IO/FileAnalyse/CFBFileAnalyse.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyStringW.h"

Text::CString IO::FileAnalyse::CFBFileAnalyse::SIDName(UInt32 sid)
{
	switch (sid)
	{
	case 0xfffffffc:
		return CSTR("DIFSECT");
	case 0xfffffffd:
		return CSTR("FATSECT");
	case 0xfffffffe:
		return CSTR("ENDOFCHAIN");
	case 0xffffffff:
		return CSTR("FREESECT");
	default:
		return nullptr;
	}
}

Text::CString IO::FileAnalyse::CFBFileAnalyse::DirTypeName(UInt8 type)
{
	switch (type)
	{
	case 0:
		return CSTR("Empty");
	case 1:
		return CSTR("Storage");
	case 2:
		return CSTR("Stream");
	case 5:
		return CSTR("Root storage");
	default:
		return CSTR("Unknown");
	}
}

Text::CString IO::FileAnalyse::CFBFileAnalyse::ColorName(UInt8 color)
{
	switch (color)
	{
	case 0:
		return CSTR("Red");
	case 1:
		return CSTR("Black");
	default:
		return CSTR("Invalid");
	}
}

void __stdcall IO::FileAnalyse::CFBFileAnalyse::ParseThread(NN<Sync::Thread> thread)
{
	NN<CFBFileAnalyse> me = thread->GetUserObj().GetNN<CFBFileAnalyse>();
	UInt8 header[512];
	NN<IO::StreamData> fd;
	if (!me->fd.SetTo(fd) || fd->GetRealData(0, 512, BYTEARR(header)) != sizeof(header))
	{
		return;
	}
	Data::ByteBuffer sectorTypes(me->sectorCount);
	Data::ArrayListUInt32 fatSectors;
	Data::ByteBuffer sectorBuff(me->sectorSize);
	UInt32 fatSectorCount = ReadLUInt32(&header[44]);
	UInt32 sid;
	UInt32 difatSid = ReadLUInt32(&header[68]);
	UInt32 difatCount = ReadLUInt32(&header[72]);
	UInt32 i;
	UInt32 j;
	sectorTypes.Clear(0, me->sectorCount);

	for (i = 0; i < 109 && fatSectors.GetCount() < fatSectorCount; i++)
	{
		sid = ReadLUInt32(&header[76 + i * 4]);
		if (sid < me->sectorCount && sectorTypes[sid] == 0)
		{
			sectorTypes[sid] = 2;
			fatSectors.Add(sid);
		}
	}
	i = 0;
	while (i < difatCount && i < me->sectorCount && difatSid < me->sectorCount && sectorTypes[difatSid] == 0)
	{
		sectorTypes[difatSid] = 3;
		if (fd->GetRealData((UInt64)(difatSid + 1) * me->sectorSize, me->sectorSize, sectorBuff) != me->sectorSize)
		{
			return;
		}
		j = 0;
		while (j < me->sectorSize / 4 - 1 && fatSectors.GetCount() < fatSectorCount)
		{
			sid = ReadLUInt32(&sectorBuff[j * 4]);
			if (sid < me->sectorCount && sectorTypes[sid] == 0)
			{
				sectorTypes[sid] = 2;
				fatSectors.Add(sid);
			}
			j++;
		}
		difatSid = ReadLUInt32(&sectorBuff[me->sectorSize - 4]);
		i++;
	}

	if (fatSectors.GetCount() > 0)
	{
		Data::ByteBuffer fatData(fatSectors.GetCount() * me->sectorSize);
		i = 0;
		while (i < fatSectors.GetCount())
		{
			sid = fatSectors.GetItem(i);
			if (fd->GetRealData((UInt64)(sid + 1) * me->sectorSize, me->sectorSize, fatData.SubArray((UIntOS)i * me->sectorSize)) != me->sectorSize)
			{
				return;
			}
			i++;
		}

		sid = ReadLUInt32(&header[48]);
		i = 0;
		while (sid < me->sectorCount && i < me->sectorCount && sectorTypes[sid] == 0)
		{
			sectorTypes[sid] = 1;
			UInt64 fatOfst = (UInt64)sid * 4;
			UIntOS fatSectorIndex = (UIntOS)(fatOfst / me->sectorSize);
			UIntOS fatEntryOfst = (UIntOS)(fatOfst % me->sectorSize);
			if (fatSectorIndex >= fatSectors.GetCount())
				break;
			sid = ReadLUInt32(&fatData[fatSectorIndex * me->sectorSize + fatEntryOfst]);
			i++;
		}

		sid = ReadLUInt32(&header[60]);
		UInt32 miniFatCount = ReadLUInt32(&header[64]);
		i = 0;
		while (sid < me->sectorCount && i < miniFatCount && i < me->sectorCount && sectorTypes[sid] == 0)
		{
			sectorTypes[sid] = 4;
			UInt64 fatOfst = (UInt64)sid * 4;
			UIntOS fatSectorIndex = (UIntOS)(fatOfst / me->sectorSize);
			UIntOS fatEntryOfst = (UIntOS)(fatOfst % me->sectorSize);
			if (fatSectorIndex >= fatSectors.GetCount())
				break;
			sid = ReadLUInt32(&fatData[fatSectorIndex * me->sectorSize + fatEntryOfst]);
			i++;
		}
	}

	me->frames.Add(MemAllocNN(FrameInfo));
	NN<FrameInfo> frame = me->frames.GetItemNoCheck(0);
	frame->ofst = 0;
	frame->size = me->sectorSize;
	frame->sectorId = 0xffffffff;
	frame->type = 0;
	for (i = 0; i < me->sectorCount; i++)
	{
		frame = MemAllocNN(FrameInfo);
		frame->ofst = (UInt64)(i + 1) * me->sectorSize;
		frame->size = me->sectorSize;
		frame->sectorId = i;
		frame->type = sectorTypes[i];
		me->frames.Add(frame);
	}
}

IO::FileAnalyse::CFBFileAnalyse::CFBFileAnalyse(NN<IO::StreamData> fd) : thread(ParseThread, this, CSTR("CFBFileAnalyse"))
{
	UInt8 header[512];
	this->fd = nullptr;
	this->sectorSize = 512;
	this->sectorCount = 0;
	if (fd->GetRealData(0, sizeof(header), BYTEARR(header)) != sizeof(header) ||
		ReadLUInt32(&header[0]) != 0xe011cfd0 || ReadLUInt32(&header[4]) != 0xe11ab1a1)
	{
		return;
	}

	UInt16 sectorShift = ReadLUInt16(&header[30]);
	if (sectorShift != 9 && sectorShift != 12)
	{
		return;
	}
	this->sectorSize = (UIntOS)1 << sectorShift;
	if (fd->GetDataSize() < this->sectorSize)
	{
		return;
	}
	UInt64 sectorCount64 = (fd->GetDataSize() - this->sectorSize) / this->sectorSize;
	if (sectorCount64 > 0xffffffff)
	{
		return;
	}
	this->sectorCount = (UInt32)sectorCount64;
	this->pauseParsing = false;
	this->fd = fd->GetPartialData(0, fd->GetDataSize());
	this->thread.Start();
}

IO::FileAnalyse::CFBFileAnalyse::~CFBFileAnalyse()
{
	this->fd.Delete();
	this->frames.MemFreeAll();
}

Text::CStringNN IO::FileAnalyse::CFBFileAnalyse::GetFormatName()
{
	return CSTR("CFB");
}

UIntOS IO::FileAnalyse::CFBFileAnalyse::GetFrameCount()
{
	return this->frames.GetCount();
}

Bool IO::FileAnalyse::CFBFileAnalyse::GetFrameName(UIntOS index, NN<Text::StringBuilderUTF8> sb)
{
	NN<FrameInfo> frame;
	if (!this->frames.GetItem(index).SetTo(frame))
		return false;
	sb->AppendU64(frame->ofst);
	if (index == 0)
	{
		sb->AppendC(UTF8STRC(": Compound File Header"));
		return true;
	}
	sb->AppendC(UTF8STRC(": Sector "));
	sb->AppendU32(frame->sectorId);
	sb->AppendC(UTF8STRC(", "));
	switch (frame->type)
	{
	case 1:
		sb->AppendC(UTF8STRC("Directory"));
		break;
	case 2:
		sb->AppendC(UTF8STRC("FAT"));
		break;
	case 3:
		sb->AppendC(UTF8STRC("DIFAT"));
		break;
	case 4:
		sb->AppendC(UTF8STRC("Mini FAT"));
		break;
	default:
		sb->AppendC(UTF8STRC("Data"));
		break;
	}
	return true;
}

UIntOS IO::FileAnalyse::CFBFileAnalyse::GetFrameIndex(UInt64 ofst)
{
	if (ofst < this->sectorSize)
		return 0;
	UInt64 sectorId = ofst / this->sectorSize - 1;
	if (sectorId >= this->sectorCount)
		return INVALID_INDEX;
	return (UIntOS)sectorId + 1;
}

Optional<IO::FileAnalyse::FrameDetail> IO::FileAnalyse::CFBFileAnalyse::GetFrameDetail(UIntOS index)
{
	NN<IO::StreamData> fd;
	NN<FrameInfo> frameInfo;
	NN<IO::FileAnalyse::FrameDetail> frame;
	if (!this->fd.SetTo(fd) || !this->frames.GetItem(index).SetTo(frameInfo))
		return nullptr;

	Data::ByteBuffer buff(frameInfo->size);
	if (fd->GetRealData(frameInfo->ofst, frameInfo->size, buff) != frameInfo->size)
		return nullptr;
	NEW_CLASSNN(frame, IO::FileAnalyse::FrameDetail(frameInfo->ofst, frameInfo->size));
	if (index == 0)
	{
		frame->AddHexBuff(0, 8, CSTR("Signature"), &buff[0], false);
		frame->AddHexBuff(8, 16, CSTR("Header CLSID"), &buff[8], false);
		frame->AddHex16(24, CSTR("Minor Version"), ReadLUInt16(&buff[24]));
		frame->AddUInt(26, 2, CSTR("Major Version"), ReadLUInt16(&buff[26]));
		frame->AddHex16Name(28, CSTR("Byte Order"), ReadLUInt16(&buff[28]), (ReadLUInt16(&buff[28]) == 0xfffe) ? CSTR("Little endian") : CSTR("Invalid"));
		frame->AddUInt(30, 2, CSTR("Sector Shift"), ReadLUInt16(&buff[30]));
		frame->AddUInt(32, 2, CSTR("Mini Sector Shift"), ReadLUInt16(&buff[32]));
		frame->AddHexBuff(34, 6, CSTR("Reserved"), &buff[34], false);
		frame->AddUInt(40, 4, CSTR("Number of Directory Sectors"), ReadLUInt32(&buff[40]));
		frame->AddUInt(44, 4, CSTR("Number of FAT Sectors"), ReadLUInt32(&buff[44]));
		frame->AddUIntName(48, 4, CSTR("First Directory Sector Location"), ReadLUInt32(&buff[48]), SIDName(ReadLUInt32(&buff[48])));
		frame->AddUInt(52, 4, CSTR("Transaction Signature Number"), ReadLUInt32(&buff[52]));
		frame->AddUInt(56, 4, CSTR("Mini Stream Cutoff Size"), ReadLUInt32(&buff[56]));
		frame->AddUIntName(60, 4, CSTR("First Mini FAT Sector Location"), ReadLUInt32(&buff[60]), SIDName(ReadLUInt32(&buff[60])));
		frame->AddUInt(64, 4, CSTR("Number of Mini FAT Sectors"), ReadLUInt32(&buff[64]));
		frame->AddUIntName(68, 4, CSTR("First DIFAT Sector Location"), ReadLUInt32(&buff[68]), SIDName(ReadLUInt32(&buff[68])));
		frame->AddUInt(72, 4, CSTR("Number of DIFAT Sectors"), ReadLUInt32(&buff[72]));
		UIntOS i = 0;
		while (i < 109)
		{
			UTF8Char nameBuff[48];
			UnsafeArray<UTF8Char> nameEnd = Text::StrUIntOS(Text::StrConcatC(nameBuff, UTF8STRC("DIFAT Entry ")), i);
			frame->AddUIntName(76 + i * 4, 4, CSTRP(nameBuff, nameEnd), ReadLUInt32(&buff[76 + i * 4]), SIDName(ReadLUInt32(&buff[76 + i * 4])));
			i++;
		}
		if (frameInfo->size > 512)
		{
			frame->AddHexBuff(512, frameInfo->size - 512, CSTR("Header Sector Padding"), &buff[512], true);
		}
	}
	else if (frameInfo->type == 1)
	{
		UTF8Char nameBuff[256];
		UnsafeArray<UTF8Char> nameEnd;
		UIntOS entryOfst = 0;
		UInt32 entryId = 0;
		UInt64 t;
		while (entryOfst + 128 <= frameInfo->size)
		{
			UInt16 nameSize = ReadLUInt16(&buff[entryOfst + 64]);
			UInt8 entryType = buff[entryOfst + 66];
			if (entryId > 0)
				frame->AddFieldSeperstor(entryOfst, CSTR("Directory Entry"));
			if (entryType != 0 && nameSize >= 2 && nameSize <= 64 && (nameSize & 1) == 0)
			{
				nameEnd = Text::StrUTF16_UTF8C(nameBuff, (const UTF16Char*)&buff[entryOfst], (nameSize / 2) - 1);
				frame->AddField(entryOfst, 64, CSTR("Directory Entry Name"), CSTRP(nameBuff, nameEnd));
			}
			else
			{
				frame->AddHexBuff(entryOfst, 64, CSTR("Directory Entry Name"), &buff[entryOfst], false);
			}
			frame->AddUInt( entryOfst + 64, 2, CSTR("Directory Entry Name Length"), nameSize);
			frame->AddUIntName(entryOfst + 66, 1, CSTR("Object Type"), entryType, DirTypeName(entryType));
			frame->AddUIntName(entryOfst + 67, 1, CSTR("Color Flag"), buff[entryOfst + 67], ColorName(buff[entryOfst + 67]));
			frame->AddUIntName(entryOfst + 68, 4, CSTR("Left Sibling ID"), ReadLUInt32(&buff[entryOfst + 68]), SIDName(ReadLUInt32(&buff[entryOfst + 68])));
			frame->AddUIntName(entryOfst + 72, 4, CSTR("Right Sibling ID"), ReadLUInt32(&buff[entryOfst + 72]), SIDName(ReadLUInt32(&buff[entryOfst + 72])));
			frame->AddUIntName(entryOfst + 76, 4, CSTR("Child ID"), ReadLUInt32(&buff[entryOfst + 76]), SIDName(ReadLUInt32(&buff[entryOfst + 76])));
			frame->AddHexBuff(entryOfst + 80, 16, CSTR("CLSID"), &buff[entryOfst + 80], false);
			frame->AddHex32(entryOfst + 96, CSTR("State Bits"), ReadLUInt32(&buff[entryOfst + 96]));
			t = ReadLUInt64(&buff[entryOfst + 100]);
			if (t == 0)
			{
				frame->AddHex64(entryOfst + 100, CSTR("Creation Time"), t);
			}
			else
			{
				nameEnd = Data::Timestamp::FromFILETIME(&buff[entryOfst + 100], Data::DateTimeUtil::GetLocalTzQhr()).ToStringNoZone(nameBuff);
				frame->AddHex64Name(entryOfst + 100, CSTR("Creation Time"), t, CSTRP(nameBuff, nameEnd));
			}
			t = ReadLUInt64(&buff[entryOfst + 108]);
			if (t == 0)
			{
				frame->AddHex64(entryOfst + 108, CSTR("Modified Time"), t);
			}
			else
			{
				nameEnd = Data::Timestamp::FromFILETIME(&buff[entryOfst + 108], Data::DateTimeUtil::GetLocalTzQhr()).ToStringNoZone(nameBuff);
				frame->AddHex64Name(entryOfst + 108, CSTR("Modified Time"), t, CSTRP(nameBuff, nameEnd));
			}
			frame->AddUIntName(entryOfst + 116, 4, CSTR("Starting Sector Location"), ReadLUInt32(&buff[entryOfst + 116]), SIDName(ReadLUInt32(&buff[entryOfst + 116])));
			frame->AddUInt64(entryOfst + 120, CSTR("Stream Size"), ReadLUInt64(&buff[entryOfst + 120]));
			entryOfst += 128;
			entryId++;
		}
	}
	else if (frameInfo->type == 2 || frameInfo->type == 4)
	{
		UIntOS i = 0;
		while (i < frameInfo->size / 4)
		{
			UInt32 value = ReadLUInt32(&buff[i * 4]);
			UTF8Char nameBuff[48];
			UnsafeArray<UTF8Char> namePtr;
			if (frameInfo->type == 2)
				namePtr = Text::StrConcatC(nameBuff, UTF8STRC("FAT Entry "));
			else
				namePtr = Text::StrConcatC(nameBuff, UTF8STRC("Mini FAT Entry "));
			UnsafeArray<UTF8Char> nameEnd = Text::StrUIntOS(namePtr, (UIntOS)frameInfo->sectorId * (frameInfo->size / 4) + i);
			frame->AddUIntName(i * 4, 4, CSTRP(nameBuff, nameEnd), value, SIDName(value));
			i++;
		}
	}
	else if (frameInfo->type == 3)
	{
		UIntOS i = 0;
		while (i < frameInfo->size / 4 - 1)
		{
			UInt32 sid = ReadLUInt32(&buff[i * 4]);
			UTF8Char nameBuff[48];
			UnsafeArray<UTF8Char> nameEnd = Text::StrUIntOS(Text::StrConcatC(nameBuff, UTF8STRC("FAT Sector ID ")), i);
			frame->AddUIntName(i * 4, 4, CSTRP(nameBuff, nameEnd), sid, SIDName(sid));
			i++;
		}
		frame->AddUIntName(frameInfo->size - 4, 4, CSTR("Next DIFAT Sector Location"), ReadLUInt32(&buff[frameInfo->size - 4]), SIDName(ReadLUInt32(&buff[frameInfo->size - 4])));
	}
	else
	{
		frame->AddHexBuff(0, frameInfo->size, CSTR("Sector Data"), &buff[0], true);
	}
	return frame;
}

Bool IO::FileAnalyse::CFBFileAnalyse::IsError()
{
	return this->fd.IsNull();
}

Bool IO::FileAnalyse::CFBFileAnalyse::IsParsing()
{
	return this->thread.IsRunning();
}

Bool IO::FileAnalyse::CFBFileAnalyse::TrimPadding(Text::CStringNN outputFile)
{
	return false;
}
