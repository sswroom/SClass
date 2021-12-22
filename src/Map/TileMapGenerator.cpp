#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/FileStream.h"
#include "IO/MemoryStream.h"
#include "IO/Path.h"
#include "IO/StreamReader.h"
#include "IO/StreamWriter.h"
#include "Map/ScaledMapView.h"
#include "Map/TileMapGenerator.h"
#include "Math/Math.h"
#include "Media/Resizer/LanczosResizerH8_8.h"
#include "Sync/MutexUsage.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"

void Map::TileMapGenerator::InitMapView(Map::MapView *view, Int32 x, Int32 y, UInt32 scale)
{
	view->UpdateSize((this->imgSize * this->osSize), (this->imgSize * this->osSize));
	view->SetDPI((Math::UOSInt2Double(this->osSize) * 96.0), 96.0);
	view->SetCenterXY((x + 0.5) * this->imgSize * scale / 2000 / 283464, (y + 0.5) * this->imgSize * scale / 2000 / 283464);
}

UTF8Char *Map::TileMapGenerator::GenFileName(UTF8Char *sbuff, Int32 x, Int32 y, UInt32 scale, const UTF8Char *ext)
{
	UTF8Char *sptr;
	sptr = Text::StrConcat(sbuff, this->tileDir);
	if (sptr[-1] != IO::Path::PATH_SEPERATOR)
	{
		*sptr++ = IO::Path::PATH_SEPERATOR;
	}
	sptr = Text::StrUInt32(sptr, scale);
	*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr = Text::StrInt32(sptr, x >> 5);
	sptr = Text::StrConcat(sptr, (const UTF8Char*)"_");
	sptr = Text::StrInt32(sptr, y >> 5);
	IO::Path::CreateDirectory(sbuff);
	*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr = Text::StrInt32(sptr, x);
	sptr = Text::StrConcat(sptr, (const UTF8Char*)"_");
	sptr = Text::StrInt32(sptr, y);
	sptr = Text::StrConcat(sptr, ext);
	return sptr;
}

void Map::TileMapGenerator::AppendDBFile(IO::Writer *writer, Int32 x, Int32 y, UInt32 scale, Int32 xOfst, Int32 yOfst)
{
	UTF8Char sbuff2[512];
	IO::FileStream *sfs;
	IO::StreamReader *reader;
	UTF8Char *sptr;

	Int64 id = ((Int64)x) << 32 | (UInt32)y;
	Bool generating;
	while (true)
	{
		Sync::MutexUsage mutUsage(dbMut);
		generating = this->dbGenList->SortedIndexOf(id) >= 0;
		mutUsage.EndUse();
		if (!generating)
			break;
		dbEvt->Wait(10);
	}

	GenFileName(sbuff2, x, y, scale, (const UTF8Char*)".db");
	NEW_CLASS(sfs, IO::FileStream(sbuff2, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	if (!sfs->IsError())
	{
		NEW_CLASS(reader, IO::StreamReader(sfs, 65001));
		sptr = Text::StrConcat(sbuff2, (const UTF8Char*)"3,");
		sptr = Text::StrInt32(sptr, xOfst);
		sptr = Text::StrConcat(sptr, (const UTF8Char*)",");
		sptr = Text::StrInt32(sptr, yOfst);
		writer->WriteLine(sbuff2);

		while ((sptr = reader->ReadLine(sbuff2, 509)) != 0)
		{
			reader->GetLastLineBreak(sptr);
			writer->Write(sbuff2);
		}
		DEL_CLASS(reader);
	}
	DEL_CLASS(sfs);
}

Bool Map::TileMapGenerator::GenerateDBFile(Int32 x, Int32 y, UInt32 scale, Map::MapScheduler *mapSch)
{
	UTF8Char sbuff[512];
	Map::MapConfig2TGen::DrawParam params;
	Media::DrawImage *dimg2;
	Bool isLayerEmpty;
	GenFileName(sbuff, x, y, scale, (const UTF8Char*)".db");

	Int64 id = ((Int64)x) << 32 | (UInt32)y;
	Bool generating;
	Sync::MutexUsage mutUsage(dbMut);
	generating = this->dbGenList->SortedIndexOf(id) >= 0;
	if (generating)
	{
		mutUsage.EndUse();
		return true;
	}
	if (IO::Path::GetPathType(sbuff) == IO::Path::PathType::File)
	{
		mutUsage.EndUse();
		return true;
	}
	this->dbGenList->SortedInsert(id);
	mutUsage.EndUse();

	Map::ScaledMapView view(this->imgSize, this->imgSize, 0, 0, scale);
	InitMapView(&view, x, y, scale);

	params.tileX = x;
	params.tileY = y;
	params.labelType = 0;
	params.dbStream = 0;

	dimg2 = geng->CreateImage32(16, 16, Media::AT_NO_ALPHA);
	dimg2->SetHDPI(96.0 * Math::UOSInt2Double(this->osSize));
	dimg2->SetVDPI(96.0 * Math::UOSInt2Double(this->osSize));
	mcfg->DrawMap(dimg2, &view, &isLayerEmpty, mapSch, resizer, sbuff, &params);
	mutUsage.BeginUse();
	this->dbGenList->RemoveAt((UOSInt)this->dbGenList->SortedIndexOf(id));
	dbEvt->Set();
	mutUsage.EndUse();
	geng->DeleteImage(dimg2);
	return true;
}

Map::TileMapGenerator::TileMapGenerator(Map::MapConfig2TGen *mcfg, Media::DrawEngine *geng, const UTF8Char *tileDir, UOSInt osSize)
{
	this->imgSize = 512;
	this->geng = geng;
	this->mcfg = mcfg;
	this->osSize = osSize;

	this->tileDir = Text::StrCopyNew(tileDir);
	NEW_CLASS(this->dbGenList, Data::ArrayListInt64());
	NEW_CLASS(this->dbMut, Sync::Mutex());
	NEW_CLASS(this->dbEvt, Sync::Event(true, (const UTF8Char*)"Map.TileMapGenerator.dbEvt"));
	NEW_CLASS(this->resizer, Media::Resizer::LanczosResizerH8_8(3, 3, Media::AT_NO_ALPHA));
}

Map::TileMapGenerator::~TileMapGenerator()
{
	Text::StrDelNew(this->tileDir);
	DEL_CLASS(this->dbGenList);
	DEL_CLASS(this->dbEvt);
	DEL_CLASS(this->dbMut);
	DEL_CLASS(this->resizer);
}

Int64 Map::TileMapGenerator::GetTileID(Double lat, Double lon, UInt32 scale, UInt32 imgSize)
{
	Int32 x;
	Int32 y;
	x = (::Int32)(lon * 2000 * 283464 / (Double)scale) / (Int32)imgSize;
	y = (::Int32)(lat * 2000 * 283464 / (Double)scale) / (Int32)imgSize;
	return (((Int64)x) << 32) | (0xffffffffLL & (Int64)y);
}

Bool Map::TileMapGenerator::GenerateTile(Int64 tileId, UInt32 scale, Map::MapScheduler *mapSch)
{
	UTF8Char sbuff2[512];
	IO::FileStream *dfs;
	IO::MemoryStream *mstm;
	IO::StreamWriter *writer;

	Int32 x = (Int32)(tileId >> 32);
	Int32 y = (Int32)(tileId & 0xffffffffLL);
	GenerateDBFile(x, y, scale, mapSch);
	GenerateDBFile(x - 1, y, scale, mapSch);
	GenerateDBFile(x + 1, y, scale, mapSch);
	GenerateDBFile(x, y - 1, scale, mapSch);
	GenerateDBFile(x, y + 1, scale, mapSch);

	GenFileName(sbuff2, x, y, scale, (const UTF8Char*)".png");
	if (IO::Path::GetPathType(sbuff2) == IO::Path::PathType::File)
		return true;

	NEW_CLASS(mstm, IO::MemoryStream(1048576, (const UTF8Char*)"Map.TileMapGenerator.GenerateTile"));
	NEW_CLASS(writer, IO::StreamWriter(mstm, 65001));
	writer->WriteSignature();

	AppendDBFile(writer, x, y, scale, 0, 0);
	AppendDBFile(writer, x + 1, y, scale, (Int32)this->imgSize, 0);
	AppendDBFile(writer, x - 1, y, scale, (Int32)-this->imgSize, 0);
	AppendDBFile(writer, x, y + 1, scale, 0, (Int32)-this->imgSize);
	AppendDBFile(writer, x, y - 1, scale, 0, (Int32)this->imgSize);
	DEL_CLASS(writer);
	mstm->SeekFromBeginning(0);
	
	Media::DrawImage *dimg;
	Media::DrawImage *dimg2;
	Bool isLayerEmpty;
	Map::MapConfig2TGen::DrawParam params;
	params.tileX = x;
	params.tileY = y;
	params.labelType = 2;
	params.dbStream = mstm;

	Map::ScaledMapView view(this->imgSize, this->imgSize, 0, 0, scale);
	InitMapView(&view, x, y, scale);

	dimg = this->geng->CreateImage32(this->imgSize, this->imgSize, Media::AT_NO_ALPHA);
	if (this->osSize == 1)
	{
		dimg->SetHDPI(96);
		dimg->SetVDPI(96);
		mcfg->DrawMap(dimg, &view, &isLayerEmpty, mapSch, resizer, 0, &params);
	}
	else
	{
		dimg2 = this->geng->CreateImage32((this->imgSize * this->osSize), (this->imgSize * this->osSize), Media::AT_NO_ALPHA);
		dimg2->SetHDPI(96.0 * Math::UOSInt2Double(this->osSize));
		dimg2->SetVDPI(96.0 * Math::UOSInt2Double(this->osSize));
		mcfg->DrawMap(dimg2, &view, &isLayerEmpty, mapSch, resizer, 0, &params);
		
		Bool revOrder;
		UInt8 *imgPtr = dimg2->GetImgBits(&revOrder);
		resizer->Resize(imgPtr, (Int32)(this->imgSize * 4 * this->osSize), (Int32)(this->imgSize * this->osSize), (Int32)(this->imgSize * this->osSize), 0, 0, dimg->GetImgBits(&revOrder), this->imgSize * 4, this->imgSize, this->imgSize);
		geng->DeleteImage(dimg2);
	}
	DEL_CLASS(mstm);

	NEW_CLASS(dfs, IO::FileStream(sbuff2, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	dimg->SavePng(dfs);
	geng->DeleteImage(dimg);
	DEL_CLASS(dfs);

	return true;
}

Bool Map::TileMapGenerator::GenerateTileArea(Double lat1, Double lon1, Double lat2, Double lon2, UInt32 scale, Map::MapScheduler *mapSch)
{
	Int64 id = GetTileID(lat1, lon1, scale, this->imgSize);
	Int32 i;
	Int32 j;
	Int32 tmp;
	Int32 x1 = (Int32)(id >> 32);
	Int32 y1 = (Int32)(id & 0xffffffff);
	id = GetTileID(lat2, lon2, scale, this->imgSize);
	Int32 x2 = (Int32)(id >> 32);
	Int32 y2 = (Int32)(id & 0xffffffff);
	if (x1 > x2)
	{
		tmp = x1;
		x1 = x2;
		x2 = tmp;
	}
	if (y1 > y2)
	{
		tmp = y1;
		y1 = y2;
		y2 = tmp;
	}

	j = y1;
	while (j <= y2)
	{
		i = x1;
		while (i <= x2)
		{
			GenerateTile((((Int64)i) << 32) | (0xffffffffLL & (Int64)j), scale, mapSch);
			i++;
		}
		j++;
	}
	return true;
}
