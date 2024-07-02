#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/FileStream.h"
#include "IO/MemoryStream.h"
#include "IO/Path.h"
#include "Map/ScaledMapView.h"
#include "Map/TileMapGenerator.h"
#include "Math/Math.h"
#include "Media/Resizer/LanczosResizerH8_8.h"
#include "Sync/MutexUsage.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"
#include "Text/UTF8Reader.h"
#include "Text/UTF8Writer.h"

void Map::TileMapGenerator::InitMapView(Map::MapView *view, Int32 x, Int32 y, UInt32 scale)
{
	view->UpdateSize(Math::Size2DDbl(UOSInt2Double(this->imgSize * this->osSize), UOSInt2Double(this->imgSize * this->osSize)));
	view->SetDPI((UOSInt2Double(this->osSize) * 96.0), 96.0);
	view->SetCenterXY(Math::Coord2DDbl((x + 0.5) * this->imgSize * scale / 2000 / 283464, (y + 0.5) * this->imgSize * scale / 2000 / 283464));
}

UnsafeArray<UTF8Char> Map::TileMapGenerator::GenFileName(UnsafeArray<UTF8Char> sbuff, Int32 x, Int32 y, UInt32 scale, Text::CStringNN ext)
{
	UnsafeArray<UTF8Char> sptr;
	sptr = Text::StrConcat(sbuff, this->tileDir);
	if (sptr[-1] != IO::Path::PATH_SEPERATOR)
	{
		*sptr++ = IO::Path::PATH_SEPERATOR;
	}
	sptr = Text::StrUInt32(sptr, scale);
	*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr = Text::StrInt32(sptr, x >> 5);
	sptr = Text::StrConcatC(sptr, UTF8STRC("_"));
	sptr = Text::StrInt32(sptr, y >> 5);
	IO::Path::CreateDirectory(CSTRP(sbuff, sptr));
	*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr = Text::StrInt32(sptr, x);
	sptr = Text::StrConcatC(sptr, UTF8STRC("_"));
	sptr = Text::StrInt32(sptr, y);
	sptr = ext.ConcatTo(sptr);
	return sptr;
}

void Map::TileMapGenerator::AppendDBFile(IO::Writer *writer, Int32 x, Int32 y, UInt32 scale, Int32 xOfst, Int32 yOfst)
{
	UTF8Char sbuff2[512];
	UnsafeArray<UTF8Char> sptr;

	Int64 id = ((Int64)x) << 32 | (UInt32)y;
	Bool generating;
	while (true)
	{
		Sync::MutexUsage mutUsage(this->dbMut);
		generating = this->dbGenList.SortedIndexOf(id) >= 0;
		mutUsage.EndUse();
		if (!generating)
			break;
		this->dbEvt.Wait(10);
	}

	sptr = GenFileName(sbuff2, x, y, scale, CSTR(".db"));
	IO::FileStream sfs(CSTRP(sbuff2, sptr), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	if (!sfs.IsError())
	{
		Text::UTF8Reader reader(sfs);
		sptr = Text::StrConcatC(sbuff2, UTF8STRC("3,"));
		sptr = Text::StrInt32(sptr, xOfst);
		sptr = Text::StrConcatC(sptr, UTF8STRC(","));
		sptr = Text::StrInt32(sptr, yOfst);
		writer->WriteLine(CSTRP(sbuff2, sptr));

		while (reader.ReadLine(sbuff2, 509).SetTo(sptr))
		{
			sptr = reader.GetLastLineBreak(sptr);
			writer->Write(CSTRP(sbuff2, sptr));
		}
	}
}

Bool Map::TileMapGenerator::GenerateDBFile(Int32 x, Int32 y, UInt32 scale, Map::MapScheduler *mapSch)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	Map::MapConfig2TGen::DrawParam params;
	NN<Media::DrawImage> dimg2;
	Bool isLayerEmpty;
	sptr = GenFileName(sbuff, x, y, scale, CSTR(".db"));

	Int64 id = ((Int64)x) << 32 | (UInt32)y;
	Bool generating;
	Sync::MutexUsage mutUsage(this->dbMut);
	generating = this->dbGenList.SortedIndexOf(id) >= 0;
	if (generating)
	{
		return true;
	}
	if (IO::Path::GetPathType(CSTRP(sbuff, sptr)) == IO::Path::PathType::File)
	{
		return true;
	}
	this->dbGenList.SortedInsert(id);
	mutUsage.EndUse();

	Map::ScaledMapView view(Math::Size2DDbl(this->imgSize, this->imgSize), Math::Coord2DDbl(0, 0), scale, false);
	InitMapView(&view, x, y, scale);

	params.tileX = x;
	params.tileY = y;
	params.labelType = 0;
	params.dbStream = 0;

	if (geng->CreateImage32(Math::Size2D<UOSInt>(16, 16), Media::AT_NO_ALPHA).SetTo(dimg2))
	{
		dimg2->SetHDPI(96.0 * UOSInt2Double(this->osSize));
		dimg2->SetVDPI(96.0 * UOSInt2Double(this->osSize));
		mcfg->DrawMap(dimg2, view, &isLayerEmpty, mapSch, resizer, CSTRP(sbuff, sptr), &params);
		mutUsage.BeginUse();
		this->dbGenList.RemoveAt((UOSInt)this->dbGenList.SortedIndexOf(id));
		this->dbEvt.Set();
		mutUsage.EndUse();
		geng->DeleteImage(dimg2);
		return true;
	}
	else
	{
		return false;
	}
}

Map::TileMapGenerator::TileMapGenerator(Map::MapConfig2TGen *mcfg, NN<Media::DrawEngine> geng, const UTF8Char *tileDir, UOSInt osSize)
{
	this->imgSize = 512;
	this->geng = geng;
	this->mcfg = mcfg;
	this->osSize = osSize;

	this->tileDir = Text::StrCopyNew(tileDir).Ptr();
	NEW_CLASS(this->resizer, Media::Resizer::LanczosResizerH8_8(3, 3, Media::AT_NO_ALPHA));
}

Map::TileMapGenerator::~TileMapGenerator()
{
	Text::StrDelNew(this->tileDir);
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
	UnsafeArray<UTF8Char> sptr;

	Int32 x = (Int32)(tileId >> 32);
	Int32 y = (Int32)(tileId & 0xffffffffLL);
	GenerateDBFile(x, y, scale, mapSch);
	GenerateDBFile(x - 1, y, scale, mapSch);
	GenerateDBFile(x + 1, y, scale, mapSch);
	GenerateDBFile(x, y - 1, scale, mapSch);
	GenerateDBFile(x, y + 1, scale, mapSch);

	sptr = GenFileName(sbuff2, x, y, scale, CSTR(".png"));
	if (IO::Path::GetPathType(CSTRP(sbuff2, sptr)) == IO::Path::PathType::File)
		return true;

	IO::MemoryStream mstm(1048576);
	{
		Text::UTF8Writer writer(mstm);
		writer.WriteSignature();

		AppendDBFile(&writer, x, y, scale, 0, 0);
		AppendDBFile(&writer, x + 1, y, scale, (Int32)this->imgSize, 0);
		AppendDBFile(&writer, x - 1, y, scale, (Int32)-this->imgSize, 0);
		AppendDBFile(&writer, x, y + 1, scale, 0, (Int32)-this->imgSize);
		AppendDBFile(&writer, x, y - 1, scale, 0, (Int32)this->imgSize);
	}
	mstm.SeekFromBeginning(0);
	
	NN<Media::DrawImage> dimg;
	NN<Media::DrawImage> dimg2;
	Bool isLayerEmpty;
	Map::MapConfig2TGen::DrawParam params;
	params.tileX = x;
	params.tileY = y;
	params.labelType = 2;
	params.dbStream = &mstm;

	Map::ScaledMapView view(Math::Size2DDbl(this->imgSize, this->imgSize), Math::Coord2DDbl(0, 0), scale, false);
	InitMapView(&view, x, y, scale);

	if (this->geng->CreateImage32(Math::Size2D<UOSInt>(this->imgSize, this->imgSize), Media::AT_NO_ALPHA).SetTo(dimg))
	{
		if (this->osSize == 1)
		{
			dimg->SetHDPI(96);
			dimg->SetVDPI(96);
			mcfg->DrawMap(dimg, view, &isLayerEmpty, mapSch, resizer, CSTR_NULL, &params);
		}
		else
		{
			if (this->geng->CreateImage32(Math::Size2D<UOSInt>((this->imgSize * this->osSize), (this->imgSize * this->osSize)), Media::AT_NO_ALPHA).SetTo(dimg2))
			{
				dimg2->SetHDPI(96.0 * UOSInt2Double(this->osSize));
				dimg2->SetVDPI(96.0 * UOSInt2Double(this->osSize));
				mcfg->DrawMap(dimg2, view, &isLayerEmpty, mapSch, resizer, CSTR_NULL, &params);
				
				Bool revOrder;
				UnsafeArray<UInt8> imgPtr;
				UnsafeArray<UInt8> dimgPtr;
				if (dimg2->GetImgBits(revOrder).SetTo(imgPtr) && dimg->GetImgBits(revOrder).SetTo(dimgPtr))
				{
					resizer->Resize(imgPtr, (Int32)(this->imgSize * 4 * this->osSize), (Int32)(this->imgSize * this->osSize), (Int32)(this->imgSize * this->osSize), 0, 0, dimgPtr, this->imgSize * 4, this->imgSize, this->imgSize);
					dimg2->GetImgBitsEnd(false);
					dimg->GetImgBitsEnd(true);
				}
				geng->DeleteImage(dimg2);
			}
		}

		{
			IO::FileStream dfs(CSTRP(sbuff2, sptr), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
			dimg->SavePng(dfs);
			geng->DeleteImage(dimg);
		}
		return true;
	}
	else
	{
		return false;
	}
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
