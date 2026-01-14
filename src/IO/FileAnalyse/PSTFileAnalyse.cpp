#include "Stdafx.h"
#include "Data/ByteBuffer.h"
#include "Core/ByteTool_C.h"
#include "IO/FileAnalyse/PSTFileAnalyse.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"

void __stdcall IO::FileAnalyse::PSTFileAnalyse::ParseThread(NN<Sync::Thread> thread)
{
	NN<IO::FileAnalyse::PSTFileAnalyse> me = thread->GetUserObj().GetNN<IO::FileAnalyse::PSTFileAnalyse>();
	NN<IO::StreamData> fd;
//	UInt64 dataSize;
//	UInt64 ofst;
//	UInt32 lastSize;
//	UOSInt readSize;
	UInt8 buff[256];
	NN<PackItem> item;
	if (!me->fd.SetTo(fd))
	{
		return;
	}
	if (fd->GetRealData(0, 256, BYTEARR(buff)) != 256)
	{
		return;
	}
	if (buff[0] != '!' || buff[1] != 'B' || buff[2] != 'D' || buff[3] != 'N' || buff[8] != 0x53 || buff[9] != 0x4d)
	{
		return;
	}
	UInt16 wVer = ReadUInt16(&buff[10]);
	me->unicode = wVer >= 23;
	if (me->unicode)
	{
		item = MemAllocNN(IO::FileAnalyse::PSTFileAnalyse::PackItem);
		item->ofst = 0;
		item->size = 564;
		item->packType = PackType::Header;
		me->items.Add(item);
	}
	else
	{
		item = MemAllocNN(IO::FileAnalyse::PSTFileAnalyse::PackItem);
		item->ofst = 0;
		item->size = 516;
		item->packType = PackType::Header;
		me->items.Add(item);
	}

/*	while (ofst < dataSize && !thread->IsStopping())
	{
		readSize = fd->GetRealData(ofst, 12, BYTEARR(buff));
		if (readSize < 4)
			break;
		
		if (readSize < 12)
			break;
		lastSize = ReadMUInt32(&buff[4]);

		item = MemAllocNN(IO::FileAnalyse::PSTFileAnalyse::PackItem);
		item->ofst = ofst + 4;
		item->size = lastSize + 12;
		item->packType = (PackType)ReadInt32(&buff[8]);
		me->items.Add(item);
		ofst += lastSize + 12;
	}*/
}

IO::FileAnalyse::PSTFileAnalyse::PSTFileAnalyse(NN<IO::StreamData> fd) : thread(ParseThread, this, CSTR("PSTFileAnalyse"))
{
	UInt8 buff[256];
	this->fd = nullptr;
	this->pauseParsing = false;
	this->unicode = false;
	fd->GetRealData(0, 256, BYTEARR(buff));
	if (buff[0] != '!' || buff[1] != 'B' || buff[2] != 'D' || buff[3] != 'N' || buff[8] != 0x53 || buff[9] != 0x4d)
	{
		return;
	}
	this->fd = fd->GetPartialData(0, fd->GetDataSize());
	this->thread.Start();
}

IO::FileAnalyse::PSTFileAnalyse::~PSTFileAnalyse()
{
	this->thread.Stop();
	this->fd.Delete();
	this->items.MemFreeAll();
}

Text::CStringNN IO::FileAnalyse::PSTFileAnalyse::GetFormatName()
{
	return CSTR("PST");
}

UOSInt IO::FileAnalyse::PSTFileAnalyse::GetFrameCount()
{
	return this->items.GetCount();
}

Bool IO::FileAnalyse::PSTFileAnalyse::GetFrameName(UOSInt index, NN<Text::StringBuilderUTF8> sb)
{
	NN<IO::FileAnalyse::PSTFileAnalyse::PackItem> item;
	if (!this->items.GetItem(index).SetTo(item))
		return false;
	sb->AppendU64(item->ofst);
	sb->AppendC(UTF8STRC(": Type="));
	sb->Append(PackTypeGetName(item->packType));
	sb->AppendC(UTF8STRC(", size="));
	sb->AppendUOSInt(item->size);
	return true;
}


UOSInt IO::FileAnalyse::PSTFileAnalyse::GetFrameIndex(UInt64 ofst)
{
	OSInt i = 0;
	OSInt j = (OSInt)this->items.GetCount() - 1;
	OSInt k;
	NN<PackItem> pack;
	while (i <= j)
	{
		k = (i + j) >> 1;
		pack = this->items.GetItemNoCheck((UOSInt)k);
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

Optional<IO::FileAnalyse::FrameDetail> IO::FileAnalyse::PSTFileAnalyse::GetFrameDetail(UOSInt index)
{
	UTF8Char sbuff[128];
	UnsafeArray<UTF8Char> sptr;
	NN<IO::FileAnalyse::FrameDetail> frame;
	NN<IO::FileAnalyse::PSTFileAnalyse::PackItem> item;
	NN<IO::StreamData> fd;
	UOSInt i;
	UOSInt j;
	UInt32 v32;
	if (!this->items.GetItem(index).SetTo(item))
		return nullptr;
	if (!this->fd.SetTo(fd))
		return nullptr;
	NEW_CLASSNN(frame, IO::FileAnalyse::FrameDetail(item->ofst, item->size));
	if (item->packType == PackType::Header)
	{
		Data::ByteBuffer packData(item->size);
		fd->GetRealData(item->ofst, item->size, packData);
		frame->AddStrC(0, 4, CSTR("dwMagic"), &packData[0]);
		frame->AddHex32(4, CSTR("dwCRCPartial"), ReadUInt32(&packData[4]));
		frame->AddHex16(8, CSTR("wMagicClient"), ReadUInt16(&packData[8]));
		frame->AddUInt(10, 2, CSTR("wVer"), ReadUInt16(&packData[10]));
		frame->AddUInt(12, 2, CSTR("wVerClient"), ReadUInt16(&packData[12]));
		frame->AddUInt(14, 1, CSTR("bPlatformCreate"), packData[14]);
		frame->AddUInt(15, 1, CSTR("bPlatformAccess"), packData[15]);
		frame->AddUInt(16, 4, CSTR("dwReserved1"), ReadUInt32(&packData[16]));
		frame->AddUInt(20, 4, CSTR("dwReserved2"), ReadUInt32(&packData[20]));
		if (this->unicode)
		{
			frame->AddHex64(24, CSTR("bidUnused"), ReadUInt64(&packData[24]));
			frame->AddHex64(32, CSTR("bidNextP"), ReadUInt64(&packData[32]));
			frame->AddHex32(40, CSTR("dwUnique"), ReadUInt32(&packData[40]));
			i = 0;
			j = 32;
			while (i < j)
			{
				sptr = Text::StrConcatC(Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("rgnid[")), i), UTF8STRC("]"));
				v32 = ReadUInt32(&packData[44 + i * 4]);
				frame->AddHex32Name(44 + i * 4, CSTRP(sbuff, sptr), v32, NIDTypeGetName(v32));
				i++;
			}
			frame->AddUInt64(172, CSTR("qwUnused"), ReadUInt64(&packData[172]));
			frame->AddUInt(180, 4, CSTR("ROOT.dwReserved"), ReadUInt32(&packData[180]));
			frame->AddUInt64(184, CSTR("ROOT.ibFileEof"), ReadUInt64(&packData[184]));
			frame->AddUInt64(192, CSTR("ROOT.ibAMapLast"), ReadUInt64(&packData[192]));
			frame->AddUInt64(200, CSTR("ROOT.cbAMapFree"), ReadUInt64(&packData[200]));
			frame->AddUInt64(208, CSTR("ROOT.cbPMapFree"), ReadUInt64(&packData[208]));
			frame->AddUInt64(216, CSTR("ROOT.BREFNBT.bid"), ReadUInt64(&packData[216]));
			frame->AddUInt64(224, CSTR("ROOT.BREFNBT.ib"), ReadUInt64(&packData[224]));
			frame->AddUInt64(232, CSTR("ROOT.BREFBBT.bid"), ReadUInt64(&packData[232]));
			frame->AddUInt64(240, CSTR("ROOT.BREFBBT.ib"), ReadUInt64(&packData[240]));
			frame->AddUInt(248, 1, CSTR("ROOT.fAMapValid"), packData[248]);
			frame->AddUInt(249, 1, CSTR("ROOT.bReserved"), packData[249]);
			frame->AddUInt(250, 2, CSTR("ROOT.wReserved"), ReadUInt16(&packData[250]));
			frame->AddUInt(252, 4, CSTR("dwAlign"), ReadUInt32(&packData[252]));
			frame->AddHexBuff(256, CSTR("rgbFM"), packData.SubArray(256, 128), true);
			frame->AddHexBuff(384, CSTR("rgbFP"), packData.SubArray(384, 128), true);
			frame->AddHex8(512, CSTR("bSentinel"), packData[512]);
			frame->AddHex8(513, CSTR("bCryptMethod"), packData[513]);
			frame->AddHex16(514, CSTR("rgbReserved"), ReadUInt16(&packData[514]));
			frame->AddHex64(516, CSTR("bidNextB"), ReadUInt64(&packData[516]));
			frame->AddHex32(524, CSTR("dwCRCFull"), ReadUInt32(&packData[524]));
			frame->AddUInt(528, 3, CSTR("rgbReserved2"), ReadUInt24(&packData[528]));
			frame->AddUInt(531, 1, CSTR("bReserved"), packData[531]);
			frame->AddHexBuff(532, CSTR("rgbReserved3"), packData.SubArray(532, 32), true);
		}
		else
		{
			frame->AddHex32(24, CSTR("bidNextB"), ReadUInt32(&packData[24]));
			frame->AddHex32(28, CSTR("bidNextP"), ReadUInt32(&packData[28]));
			frame->AddHex32(32, CSTR("dwUnique"), ReadUInt32(&packData[32]));
			i = 0;
			j = 32;
			while (i < j)
			{
				sptr = Text::StrConcatC(Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("rgnid[")), i), UTF8STRC("]"));
				v32 = ReadUInt32(&packData[36 + i * 4]);
				frame->AddHex32Name(36 + i * 4, CSTRP(sbuff, sptr), v32, NIDTypeGetName(v32));
				i++;
			}
			frame->AddUInt(164, 4, CSTR("ROOT.dwReserved"), ReadUInt32(&packData[164]));
			frame->AddUInt(168, 4, CSTR("ROOT.ibFileEof"), ReadUInt32(&packData[168]));
			frame->AddUInt(172, 4, CSTR("ROOT.ibAMapLast"), ReadUInt32(&packData[172]));
			frame->AddUInt(176, 4, CSTR("ROOT.cbAMapFree"), ReadUInt32(&packData[176]));
			frame->AddUInt(180, 4, CSTR("ROOT.cbPMapFree"), ReadUInt32(&packData[180]));
			frame->AddUInt(184, 4, CSTR("ROOT.BREFNBT.bid"), ReadUInt32(&packData[184]));
			frame->AddUInt(188, 4, CSTR("ROOT.BREFNBT.ib"), ReadUInt32(&packData[188]));
			frame->AddUInt(192, 4, CSTR("ROOT.BREFBBT.bid"), ReadUInt32(&packData[192]));
			frame->AddUInt(196, 4, CSTR("ROOT.BREFBBT.ib"), ReadUInt32(&packData[196]));
			frame->AddUInt(200, 1, CSTR("ROOT.fAMapValid"), packData[200]);
			frame->AddUInt(201, 1, CSTR("ROOT.bReserved"), packData[201]);
			frame->AddUInt(202, 2, CSTR("ROOT.wReserved"), ReadUInt16(&packData[202]));
			frame->AddUInt(204, 4, CSTR("dwAlign"), ReadUInt32(&packData[204]));
			frame->AddHexBuff(208, CSTR("rgbFM"), packData.SubArray(208, 128), true);
			frame->AddHexBuff(336, CSTR("rgbFP"), packData.SubArray(336, 128), true);
			frame->AddHex8(464, CSTR("bSentinel"), packData[464]);
			frame->AddHex8(465, CSTR("bCryptMethod"), packData[465]);
			frame->AddHex16(466, CSTR("rgbReserved"), ReadUInt16(&packData[466]));
			frame->AddHex64(468, CSTR("ullReserved"), ReadUInt64(&packData[468]));
			frame->AddHex32(476, CSTR("dwReserved"), ReadUInt32(&packData[476]));
			frame->AddUInt(480, 3, CSTR("rgbReserved2"), ReadUInt24(&packData[480]));
			frame->AddUInt(483, 1, CSTR("bReserved"), packData[483]);
			frame->AddHexBuff(484, CSTR("rgbReserved3"), packData.SubArray(484, 32), true);
		}
	}
	return frame;
}

Bool IO::FileAnalyse::PSTFileAnalyse::IsError()
{
	return this->fd.IsNull();
}

Bool IO::FileAnalyse::PSTFileAnalyse::IsParsing()
{
	return this->thread.IsRunning();
}

Bool IO::FileAnalyse::PSTFileAnalyse::TrimPadding(Text::CStringNN outputFile)
{
	return false;
}

Text::CStringNN IO::FileAnalyse::PSTFileAnalyse::PackTypeGetName(PackType packType)
{
	switch (packType)
	{
	case PackType::Header:
		return CSTR("Header");
	default:
		return CSTR("Unknown");
	}
}

Text::CString IO::FileAnalyse::PSTFileAnalyse::NIDTypeGetName(UInt32 nidType)
{
	switch (nidType)
	{
	case 0x400:
		return CSTR("NID_TYPE_NORMAL_FOLDER");
	case 0x4000:
		return CSTR("NID_TYPE_SEARCH_FOLDER");
	case 0x8000:
		return CSTR("NID_TYPE_ASSOC_MESSAGE");
	case 0x10000:
		return CSTR("NID_TYPE_NORMAL_MESSAGE");
	default:
		return nullptr;
	}
}
