#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "IO/FileStream.h"
#include "IO/FileAnalyse/MapsforgeFileAnalyse.h"
#include "Sync/SimpleThread.h"
#include "Text/MyStringFloat.h"
#include "Text/MyStringW.h"
#include "Text/XLSUtil.h"

// https://github.com/mapsforge/mapsforge/blob/master/docs/Specification-Binary-Map-File.md
void __stdcall IO::FileAnalyse::MapsforgeFileAnalyse::ParseThread(NN<Sync::Thread> thread)
{
	NN<IO::FileAnalyse::MapsforgeFileAnalyse> me = thread->GetUserObj().GetNN<IO::FileAnalyse::MapsforgeFileAnalyse>();
	UInt8 readBuff[4096];
	UOSInt readSize;
	NN<IO::FileAnalyse::MapsforgeFileAnalyse::PackInfo> pack;
	NN<IO::StreamData> fd;
	if (me->fd.SetTo(fd))
	{
		readSize = fd->GetRealData(0, 256, BYTEARR(readBuff));
		if (readSize != 256 || !Text::StrStartsWithC(readBuff, 256, UTF8STRC("mapsforge binary OSM")))
		{
			return;
		}
		UInt32 headerSize = ReadMUInt32(&readBuff[20]);
		UInt64 fileSize = ReadMUInt64(&readBuff[28]);
		if (fileSize == fd->GetDataSize() && headerSize >= 73 && headerSize < fileSize)
		{
			pack = MemAllocNN(PackInfo);
			pack->fileOfst = 0;
			pack->packSize = headerSize;
			pack->packType = 0;
			me->packs.Add(pack);
		}

/*		


		while (!thread->IsStopping())
		{
			if (me->pauseParsing)
			{
				Sync::SimpleThread::Sleep(100);
			}
			else
			{
				pack = MemAllocNN(PackInfo);
				pack->fileOfst = readOfst;
				pack->packSize = 4096;
				pack->packType = ReadUInt16(readBuff);
				me->packs.Add(pack);
				readOfst += 4096;
			}
		}*/
	}
}

UOSInt IO::FileAnalyse::MapsforgeFileAnalyse::ReadVBEU(UnsafeArray<UInt8> buff, UOSInt ofst, OutParam<UInt64> v)
{
	UInt64 tmpV = 0;
	UOSInt sh = 0;
	UInt8 b;
	while (true)
	{
		b = buff[ofst];
		ofst++;
		tmpV = tmpV | ((UInt64)(b & 0x7f) << sh);
		if ((b & 0x80) == 0)
			break;
		sh += 7;
	}
	v.Set(tmpV);
	return ofst;
}

IO::FileAnalyse::MapsforgeFileAnalyse::MapsforgeFileAnalyse(NN<IO::StreamData> fd) : thread(ParseThread, this, CSTR("MapsforgeFileAnalyse"))
{
	UInt8 buff[256];
	this->fd = 0;
	this->pauseParsing = false;
	fd->GetRealData(0, 256, BYTEARR(buff));
	if (!Text::StrStartsWithC(buff, 256, UTF8STRC("mapsforge binary OSM")))
	{
		return;
	}
	this->fd = fd->GetPartialData(0, fd->GetDataSize());
	this->thread.Start();
}

IO::FileAnalyse::MapsforgeFileAnalyse::~MapsforgeFileAnalyse()
{
	this->thread.Stop();
	this->fd.Delete();
	this->packs.MemFreeAll();
}

Text::CStringNN IO::FileAnalyse::MapsforgeFileAnalyse::GetFormatName()
{
	return CSTR("MFO");
}

UOSInt IO::FileAnalyse::MapsforgeFileAnalyse::GetFrameCount()
{
	return this->packs.GetCount();
}

Bool IO::FileAnalyse::MapsforgeFileAnalyse::GetFrameName(UOSInt index, NN<Text::StringBuilderUTF8> sb)
{
	NN<IO::FileAnalyse::MapsforgeFileAnalyse::PackInfo> pack;
	if (!this->packs.GetItem(index).SetTo(pack))
		return false;
	sb->AppendU64(pack->fileOfst);
	sb->AppendC(UTF8STRC(": Num="));
	sb->AppendUOSInt(index);
	sb->AppendC(UTF8STRC(", Type=0x"));
	sb->AppendHex16(pack->packType);
	switch (pack->packType)
	{
	case 0:
		sb->AppendC(UTF8STRC(" (File Header)"));
		break;
	}
	return true;
}

UOSInt IO::FileAnalyse::MapsforgeFileAnalyse::GetFrameIndex(UInt64 ofst)
{
	UOSInt index = (UOSInt)(ofst >> 12);
	if (index >= this->packs.GetCount())
	{
		return INVALID_INDEX;
	}
	return index;
}

Optional<IO::FileAnalyse::FrameDetail> IO::FileAnalyse::MapsforgeFileAnalyse::GetFrameDetail(UOSInt index)
{
	NN<IO::FileAnalyse::FrameDetail> frame;
	NN<IO::FileAnalyse::MapsforgeFileAnalyse::PackInfo> pack;
	UnsafeArray<UInt8> packBuff;
	NN<IO::StreamData> fd;
	UTF8Char sbuff[128];
	UnsafeArray<UTF8Char> sptr;
	UOSInt ofst;
	UOSInt nextOfst;
	UInt64 v;
	UInt8 flags;
	UOSInt cnt;
	UOSInt i;
	if (!this->packs.GetItem(index).SetTo(pack) || !this->fd.SetTo(fd))
		return 0;

	packBuff = MemAllocArr(UInt8, pack->packSize);
	NEW_CLASSNN(frame, IO::FileAnalyse::FrameDetail(pack->fileOfst, pack->packSize));
	fd->GetRealData(pack->fileOfst, pack->packSize, Data::ByteArray(packBuff, pack->packSize));
	switch (pack->packType)
	{
	case 0x0:
		frame->AddField(0, 20, CSTR("Magic byte"), Text::CStringNN(packBuff, 20));
		frame->AddUInt(20, 4, CSTR("Header size"), ReadMUInt32(&packBuff[20]));
		frame->AddUInt(24, 4, CSTR("File version"), ReadMUInt32(&packBuff[24]));
		frame->AddUInt64(28, CSTR("File size"), ReadMUInt64(&packBuff[28]));
		sptr = Data::Timestamp(ReadMInt64(&packBuff[36]), Data::DateTimeUtil::GetLocalTzQhr()).ToString(sbuff);
		frame->AddUInt64Name(36, 8, CSTR("Date of creation"), ReadMUInt64(&packBuff[36]), CSTRP(sbuff, sptr));
		frame->AddFloat(44, 4, CSTR("Min Lat"), ReadMInt32(&packBuff[44]) * 0.000001);
		frame->AddFloat(48, 4, CSTR("Min Lon"), ReadMInt32(&packBuff[48]) * 0.000001);
		frame->AddFloat(52, 4, CSTR("Max Lat"), ReadMInt32(&packBuff[52]) * 0.000001);
		frame->AddFloat(56, 4, CSTR("Max Lon"), ReadMInt32(&packBuff[56]) * 0.000001);
		frame->AddUInt(60, 2, CSTR("Tile size"), ReadMUInt16(&packBuff[60]));
		ofst = 62;
		nextOfst = ReadVBEU(packBuff, ofst, v);
		frame->AddField(ofst, nextOfst + v - ofst, CSTR("Projection"), Text::CStringNN(&packBuff[nextOfst], (UOSInt)v));
		ofst = nextOfst + (UOSInt)v;
		flags = packBuff[ofst];
		frame->AddBit(ofst, CSTR("existence of debug information"), flags, 7);
		frame->AddBit(ofst, CSTR("existence of the map start position field"), flags, 6);
		frame->AddBit(ofst, CSTR("existence of the start zoom level field"), flags, 5);
		frame->AddBit(ofst, CSTR("existence of the language(s) preference field"), flags, 4);
		frame->AddBit(ofst, CSTR("existence of the comment field"), flags, 3);
		frame->AddBit(ofst, CSTR("existence of the created by field"), flags, 2);
		frame->AddBit(ofst, CSTR("reserved for future use"), flags, 1);
		frame->AddBit(ofst, CSTR("reserved for future use"), flags, 0);
		ofst++;
		if (flags & 0x40)
		{
			frame->AddFloat(ofst, 4, CSTR("Map Start Lat"), ReadMInt32(&packBuff[ofst]) * 0.000001);
			frame->AddFloat(ofst + 4, 4, CSTR("Map Start Lon"), ReadMInt32(&packBuff[ofst + 4]) * 0.000001);
			ofst += 8;
		}
		if (flags & 0x20)
		{
			frame->AddUInt(ofst, 1, CSTR("Start zoom level"), packBuff[ofst]);
			ofst++;
		}
		if (flags & 0x10)
		{
			nextOfst = ReadVBEU(packBuff, ofst, v);
			frame->AddField(ofst, nextOfst + v - ofst, CSTR("Language(s) preference"), Text::CStringNN(&packBuff[nextOfst], (UOSInt)v));
			ofst = nextOfst + (UOSInt)v;
		}
		if (flags & 0x8)
		{
			nextOfst = ReadVBEU(packBuff, ofst, v);
			frame->AddField(ofst, nextOfst + v - ofst, CSTR("Comment"), Text::CStringNN(&packBuff[nextOfst], (UOSInt)v));
			ofst = nextOfst + (UOSInt)v;
		}
		if (flags & 0x4)
		{
			nextOfst = ReadVBEU(packBuff, ofst, v);
			frame->AddField(ofst, nextOfst + v - ofst, CSTR("Created by"), Text::CStringNN(&packBuff[nextOfst], (UOSInt)v));
			ofst = nextOfst + (UOSInt)v;
		}
		cnt = ReadMUInt16(&packBuff[ofst]);
		frame->AddUInt(ofst, 2, CSTR("POI tags count"), cnt);
		ofst += 2;
		i = 0;
		while (i < cnt)
		{
			nextOfst = ReadVBEU(packBuff, ofst, v);
			sptr = Text::StrUOSInt(sbuff, i);
			frame->AddField(ofst, nextOfst + v - ofst, CSTR("POI tag ID"), CSTRP(sbuff, sptr));
			frame->AddField(ofst, nextOfst + v - ofst, CSTR("POI tag"), Text::CStringNN(&packBuff[nextOfst], (UOSInt)v));
			ofst = nextOfst + (UOSInt)v;
			i++;
		}

		cnt = ReadMUInt16(&packBuff[ofst]);
		frame->AddUInt(ofst, 2, CSTR("Way tags count"), cnt);
		ofst += 2;
		i = 0;
		while (i < cnt)
		{
			nextOfst = ReadVBEU(packBuff, ofst, v);
			sptr = Text::StrUOSInt(sbuff, i);
			frame->AddField(ofst, nextOfst + v - ofst, CSTR("Way tag ID"), CSTRP(sbuff, sptr));
			frame->AddField(ofst, nextOfst + v - ofst, CSTR("Way tag"), Text::CStringNN(&packBuff[nextOfst], (UOSInt)v));
			ofst = nextOfst + (UOSInt)v;
			i++;
		}
		break;
	}
	MemFreeArr(packBuff);
	return frame;
}

Bool IO::FileAnalyse::MapsforgeFileAnalyse::IsError()
{
	return this->fd.IsNull();
}

Bool IO::FileAnalyse::MapsforgeFileAnalyse::IsParsing()
{
	return this->thread.IsRunning();
}

Bool IO::FileAnalyse::MapsforgeFileAnalyse::TrimPadding(Text::CStringNN outputFile)
{
	return false;
}
