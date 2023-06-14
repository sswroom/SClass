#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "IO/FileAnalyse/SHPFileAnalyse.h"
#include "Sync/SimpleThread.h"
#include "Sync/Thread.h"
#include "Text/Encoding.h"
#include "Text/StringBuilderUTF8.h"

Text::CString IO::FileAnalyse::SHPFileAnalyse::ShapeTypeGetName(UInt32 shapeType)
{
	switch (shapeType)
	{
	case 0:
		return CSTR("Null Shape");
	case 1:
		return CSTR("Point");
	case 3:
		return CSTR("PolyLine");
	case 5:
		return CSTR("Polygon");
	case 8:
		return CSTR("MultiPoint");
	case 11:
		return CSTR("PointZ");
	case 13:
		return CSTR("PolyLineZ");
	case 15:
		return CSTR("PolygonZ");
	case 18:
		return CSTR("MultiPointZ");
	case 21:
		return CSTR("PointM");
	case 23:
		return CSTR("PolyLineM");
	case 25:
		return CSTR("PolygonM");
	case 28:
		return CSTR("MultiPointM");
	case 31:
		return CSTR("MultiPatch");
	default:
		return CSTR("Unknown");
	}
}

UInt32 __stdcall IO::FileAnalyse::SHPFileAnalyse::ParseThread(void *userObj)
{
	IO::FileAnalyse::SHPFileAnalyse *me = (IO::FileAnalyse::SHPFileAnalyse*)userObj;
	UInt64 dataSize;
	UInt64 ofst;
	UInt8 recHdr[12];
	IO::FileAnalyse::SHPFileAnalyse::PackInfo *pack;
	me->threadRunning = true;
	me->threadStarted = true;
	dataSize = me->fd->GetDataSize();
	pack = MemAlloc(PackInfo, 1);
	pack->fileOfst = 0;
	pack->packSize = 100;
	me->packs.Add(pack);
	ofst = 100;
	while (ofst < dataSize && !me->threadToStop)
	{
		if (me->fd->GetRealData(ofst, 12, recHdr) != 12)
			break;
		
		UInt32 recSize = ReadMUInt32(&recHdr[4]);
		if (recSize < 4)
		{
			break;
		}
		if (ofst + 8 + recSize * 2 > dataSize)
		{
			break;
		}
		pack = MemAlloc(PackInfo, 1);
		pack->fileOfst = ofst;
		pack->packSize = 8 + recSize * 2;
		me->packs.Add(pack);
		ofst += pack->packSize;
	}
	
	me->threadRunning = false;
	return 0;
}

IO::FileAnalyse::SHPFileAnalyse::SHPFileAnalyse(IO::StreamData *fd)
{
	UInt8 buff[256];
	this->fd = 0;
	this->threadRunning = false;
	this->pauseParsing = false;
	this->threadToStop = false;
	this->threadStarted = false;
	fd->GetRealData(0, 256, buff);
	if (ReadMInt32(buff) != 9994 || ReadInt32(&buff[28]) != 1000 || (ReadMUInt32(&buff[24]) << 1) != fd->GetDataSize())
	{
		return;
	}
	this->fd = fd->GetPartialData(0, fd->GetDataSize());

	Sync::Thread::Create(ParseThread, this);
	while (!this->threadStarted)
	{
		Sync::SimpleThread::Sleep(10);
	}
}

IO::FileAnalyse::SHPFileAnalyse::~SHPFileAnalyse()
{
	if (this->threadRunning)
	{
		this->threadToStop = true;
		while (this->threadRunning)
		{
			Sync::SimpleThread::Sleep(10);
		}
	}

	SDEL_CLASS(this->fd);
	LIST_FREE_FUNC(&this->packs, MemFree);
}

Text::CString IO::FileAnalyse::SHPFileAnalyse::GetFormatName()
{
	return CSTR("ESRI Shapefile");
}

UOSInt IO::FileAnalyse::SHPFileAnalyse::GetFrameCount()
{
	return this->packs.GetCount();
}

Bool IO::FileAnalyse::SHPFileAnalyse::GetFrameName(UOSInt index, Text::StringBuilderUTF8 *sb)
{
	IO::FileAnalyse::SHPFileAnalyse::PackInfo *pack = this->packs.GetItem(index);
	Text::CString name;
	if (pack == 0)
		return false;
	sb->AppendU64(pack->fileOfst);
	sb->AppendC(UTF8STRC(": size="));
	sb->AppendUOSInt(pack->packSize);
	return true;
}


UOSInt IO::FileAnalyse::SHPFileAnalyse::GetFrameIndex(UInt64 ofst)
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

IO::FileAnalyse::FrameDetail *IO::FileAnalyse::SHPFileAnalyse::GetFrameDetail(UOSInt index)
{
	IO::FileAnalyse::FrameDetail *frame;
	UTF8Char sbuff[128];
	UTF8Char *sptr;
	UInt8 *tagData;
	IO::FileAnalyse::SHPFileAnalyse::PackInfo *pack = this->packs.GetItem(index);
	if (pack == 0)
		return 0;
	
	NEW_CLASS(frame, IO::FileAnalyse::FrameDetail(pack->fileOfst, (UInt32)pack->packSize));
	sptr = Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("Packet ")), index);
	frame->AddHeader(CSTRP(sbuff, sptr));

	tagData = MemAlloc(UInt8, pack->packSize);
	this->fd->GetRealData(pack->fileOfst, pack->packSize, tagData);
	if (index == 0)
	{
		frame->AddUInt(0, 4, CSTR("File Code"), ReadMUInt32(&tagData[0]));
		frame->AddUInt(4, 4, CSTR("Unused"), ReadMUInt32(&tagData[4]));
		frame->AddUInt(8, 4, CSTR("Unused"), ReadMUInt32(&tagData[8]));
		frame->AddUInt(12, 4, CSTR("Unused"), ReadMUInt32(&tagData[12]));
		frame->AddUInt(16, 4, CSTR("Unused"), ReadMUInt32(&tagData[16]));
		frame->AddUInt(20, 4, CSTR("Unused"), ReadMUInt32(&tagData[20]));
		frame->AddUInt(24, 4, CSTR("File Length"), ReadMUInt32(&tagData[24]));
		frame->AddUInt(28, 4, CSTR("Version"), ReadUInt32(&tagData[28]));
		frame->AddUIntName(32, 4, CSTR("Shape Type"), ReadUInt32(&tagData[32]), ShapeTypeGetName(ReadUInt32(&tagData[32])));
		frame->AddFloat(36, 8, CSTR("Xmin"), ReadDouble(&tagData[36]));
		frame->AddFloat(44, 8, CSTR("Ymin"), ReadDouble(&tagData[44]));
		frame->AddFloat(52, 8, CSTR("Xmax"), ReadDouble(&tagData[52]));
		frame->AddFloat(60, 8, CSTR("Ymax"), ReadDouble(&tagData[60]));
		frame->AddFloat(68, 8, CSTR("Zmin"), ReadDouble(&tagData[68]));
		frame->AddFloat(76, 8, CSTR("Zmax"), ReadDouble(&tagData[76]));
		frame->AddFloat(84, 8, CSTR("Mmin"), ReadDouble(&tagData[84]));
		frame->AddFloat(92, 8, CSTR("Mmax"), ReadDouble(&tagData[92]));
	}
	else
	{
		UInt32 numParts;
		UInt32 numPoints;
		UOSInt i;
		UOSInt j;
		frame->AddUInt(0, 4, CSTR("Record Number"), ReadMUInt32(&tagData[0]));
		frame->AddUInt(4, 4, CSTR("Content Length (WORD)"), ReadMUInt32(&tagData[4]));
		UInt32 shapeType = ReadUInt32(&tagData[8]);
		frame->AddUIntName(8, 4, CSTR("Shape Type"), shapeType, ShapeTypeGetName(shapeType));
		switch (shapeType)
		{
		case 0:
			break;
		case 3:
		case 5:
			frame->AddFloat(12, 8, CSTR("Xmin"), ReadDouble(&tagData[12]));
			frame->AddFloat(20, 8, CSTR("Ymin"), ReadDouble(&tagData[20]));
			frame->AddFloat(28, 8, CSTR("Xmax"), ReadDouble(&tagData[28]));
			frame->AddFloat(36, 8, CSTR("Ymax"), ReadDouble(&tagData[36]));
			numParts = ReadUInt32(&tagData[44]);
			numPoints = ReadUInt32(&tagData[48]);
			frame->AddUInt(44, 4, CSTR("NumParts"), numParts);
			frame->AddUInt(48, 4, CSTR("NumPoints"), numPoints);
			i = 52;
			j = 0;
			while (j < numParts)
			{
				frame->AddUInt(i, 4, CSTR("Part/PtIndex"), ReadUInt32(&tagData[i]));
				i += 4;
				j++;
			}
			j = 0;
			while (j < numPoints)
			{
				frame->AddFloat(i, 8, CSTR("Point.x"), ReadDouble(&tagData[i]));
				frame->AddFloat(i + 8, 8, CSTR("Point.y"), ReadDouble(&tagData[i + 8]));
				i += 16;
				j++;
			}
		}
	}
	MemFree(tagData);
	return frame;
}

Bool IO::FileAnalyse::SHPFileAnalyse::IsError()
{
	return this->fd == 0;
}

Bool IO::FileAnalyse::SHPFileAnalyse::IsParsing()
{
	return this->threadRunning;
}

Bool IO::FileAnalyse::SHPFileAnalyse::TrimPadding(Text::CString outputFile)
{
	return false;
}
