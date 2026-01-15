#include "Stdafx.h"
#include "Data/ByteBuffer.h"
#include "Core/ByteTool_C.h"
#include "IO/FileAnalyse/SHPFileAnalyse.h"
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

void __stdcall IO::FileAnalyse::SHPFileAnalyse::ParseThread(NN<Sync::Thread> thread)
{
	NN<IO::FileAnalyse::SHPFileAnalyse> me = thread->GetUserObj().GetNN<IO::FileAnalyse::SHPFileAnalyse>();
	NN<IO::StreamData> fd;
	UInt64 dataSize;
	UInt64 ofst;
	UInt8 recHdr[12];
	NN<IO::FileAnalyse::SHPFileAnalyse::PackInfo> pack;
	if (!me->fd.SetTo(fd))
	{
		return;
	}
	dataSize = fd->GetDataSize();
	pack = MemAllocNN(PackInfo);
	pack->fileOfst = 0;
	pack->packSize = 100;
	me->packs.Add(pack);
	ofst = 100;
	while (ofst < dataSize && !thread->IsStopping())
	{
		if (fd->GetRealData(ofst, 12, BYTEARR(recHdr)) != 12)
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
		pack = MemAllocNN(PackInfo);
		pack->fileOfst = ofst;
		pack->packSize = 8 + recSize * 2;
		me->packs.Add(pack);
		ofst += pack->packSize;
	}
}

IO::FileAnalyse::SHPFileAnalyse::SHPFileAnalyse(NN<IO::StreamData> fd) : thread(ParseThread, this, CSTR("SHPFileAnalyse"))
{
	UInt8 buff[256];
	this->fd = nullptr;
	this->pauseParsing = false;
	fd->GetRealData(0, 256, BYTEARR(buff));
	if (ReadMInt32(buff) != 9994 || ReadInt32(&buff[28]) != 1000 || (ReadMUInt32(&buff[24]) << 1) != fd->GetDataSize())
	{
		return;
	}
	this->fd = fd->GetPartialData(0, fd->GetDataSize()).Ptr();
	this->thread.Start();
}

IO::FileAnalyse::SHPFileAnalyse::~SHPFileAnalyse()
{
	this->thread.Stop();
	this->fd.Delete();
	this->packs.MemFreeAll();
}

Text::CStringNN IO::FileAnalyse::SHPFileAnalyse::GetFormatName()
{
	return CSTR("ESRI Shapefile");
}

UIntOS IO::FileAnalyse::SHPFileAnalyse::GetFrameCount()
{
	return this->packs.GetCount();
}

Bool IO::FileAnalyse::SHPFileAnalyse::GetFrameName(UIntOS index, NN<Text::StringBuilderUTF8> sb)
{
	NN<IO::FileAnalyse::SHPFileAnalyse::PackInfo> pack;
	if (!this->packs.GetItem(index).SetTo(pack))
		return false;
	sb->AppendU64(pack->fileOfst);
	sb->AppendC(UTF8STRC(": size="));
	sb->AppendUIntOS(pack->packSize);
	return true;
}


UIntOS IO::FileAnalyse::SHPFileAnalyse::GetFrameIndex(UInt64 ofst)
{
	IntOS i = 0;
	IntOS j = (IntOS)this->packs.GetCount() - 1;
	IntOS k;
	NN<PackInfo> pack;
	while (i <= j)
	{
		k = (i + j) >> 1;
		pack = this->packs.GetItemNoCheck((UIntOS)k);
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
			return (UIntOS)k;
		}
	}
	return INVALID_INDEX;
}

Optional<IO::FileAnalyse::FrameDetail> IO::FileAnalyse::SHPFileAnalyse::GetFrameDetail(UIntOS index)
{
	NN<IO::FileAnalyse::FrameDetail> frame;
	UTF8Char sbuff[128];
	UnsafeArray<UTF8Char> sptr;
	NN<IO::FileAnalyse::SHPFileAnalyse::PackInfo> pack;
	NN<IO::StreamData> fd;
	if (!this->packs.GetItem(index).SetTo(pack))
		return nullptr;
	if (!this->fd.SetTo(fd))
		return nullptr;
	
	NEW_CLASSNN(frame, IO::FileAnalyse::FrameDetail(pack->fileOfst, pack->packSize));
	sptr = Text::StrUIntOS(Text::StrConcatC(sbuff, UTF8STRC("Packet ")), index);
	frame->AddHeader(CSTRP(sbuff, sptr));

	Data::ByteBuffer tagData(pack->packSize);
	fd->GetRealData(pack->fileOfst, pack->packSize, tagData);
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
		UIntOS i;
		UIntOS j;
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
			break;
		case 13:
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
			frame->AddFloat(i, 8, CSTR("Zmin"), ReadDouble(&tagData[i]));
			frame->AddFloat(i + 8, 8, CSTR("Zmax"), ReadDouble(&tagData[i + 8]));
			i = pack->packSize - 8 * numPoints;
			j = 0;
			while (j < numPoints)
			{
				frame->AddFloat(i, 8, CSTR("Point.z"), ReadDouble(&tagData[i]));
				i += 8;
				j++;
			}
			break;
		}
	}
	return frame;
}

Bool IO::FileAnalyse::SHPFileAnalyse::IsError()
{
	return this->fd.IsNull();
}

Bool IO::FileAnalyse::SHPFileAnalyse::IsParsing()
{
	return this->thread.IsRunning();
}

Bool IO::FileAnalyse::SHPFileAnalyse::TrimPadding(Text::CStringNN outputFile)
{
	return false;
}
