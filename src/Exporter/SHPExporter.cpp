#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Exporter/DBFExporter.h"
#include "Exporter/SHPExporter.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "Map/MapDrawLayer.h"
#include "Math/Geometry/PointZ.h"
#include "Math/Geometry/Polyline.h"
#include "Math/Geometry/Polygon.h"
#include "Math/SRESRIWKTWriter.h"
#include "Text/MyString.h"

#include <stdio.h>

Exporter::SHPExporter::SHPExporter()
{
	this->codePage = 0;
}

Exporter::SHPExporter::~SHPExporter()
{
}

Int32 Exporter::SHPExporter::GetName()
{
	return *(Int32*)"SHPE";
}

IO::FileExporter::SupportType Exporter::SHPExporter::IsObjectSupported(NN<IO::ParsedObject> pobj)
{
	if (pobj->GetParserType() != IO::ParserType::MapLayer)
	{
		return IO::FileExporter::SupportType::NotSupported;
	}
	NN<Map::MapDrawLayer> layer = NN<Map::MapDrawLayer>::ConvertFrom(pobj);
	Map::DrawLayerType layerType = layer->GetLayerType();
	if (layerType == Map::DRAW_LAYER_POINT || layerType == Map::DRAW_LAYER_POINT3D || layerType == Map::DRAW_LAYER_POLYLINE || layerType == Map::DRAW_LAYER_POLYLINE3D || layerType == Map::DRAW_LAYER_POLYGON)
	{
		return IO::FileExporter::SupportType::MultiFiles;
	}
	return IO::FileExporter::SupportType::NotSupported;
}

Bool Exporter::SHPExporter::GetOutputName(UOSInt index, UnsafeArray<UTF8Char> nameBuff, UnsafeArray<UTF8Char> fileNameBuff)
{
	if (index == 0)
	{
		Text::StrConcatC(nameBuff, UTF8STRC("ESRI Shapefile"));
		Text::StrConcatC(fileNameBuff, UTF8STRC("*.shp"));
		return true;
	}
	return false;
}

void Exporter::SHPExporter::SetCodePage(UInt32 codePage)
{
	this->codePage = codePage;
}

Bool Exporter::SHPExporter::ExportFile(NN<IO::SeekableStream> stm, Text::CStringNN fileName, NN<IO::ParsedObject> pobj, Optional<ParamData> param)
{
	UInt8 buff[256];
	UTF8Char fileName2[256];
	UnsafeArray<UTF8Char> sptr;
	if (pobj->GetParserType() != IO::ParserType::MapLayer)
	{
		return false;
	}
	NN<Map::MapDrawLayer> layer = NN<Map::MapDrawLayer>::ConvertFrom(pobj);
	Map::DrawLayerType layerType = layer->GetLayerType();
	UOSInt fileSize = 100;
	UOSInt recCnt = 0;
	UOSInt i;
	Int32 ilayerType = 0;
	Math::Coord2DDbl min = Math::Coord2DDbl(0, 0);
	Math::Coord2DDbl max = Math::Coord2DDbl(0, 0);
	Double zMin = 0;
	Double zMax = 0;
	IO::FileStream *shx;
	Map::NameArray *nameArr;
	NN<Map::GetObjectSess> sess;
	if (layerType != Map::DRAW_LAYER_POINT && layerType != Map::DRAW_LAYER_POINT3D && layerType != Map::DRAW_LAYER_POLYLINE && layerType != Map::DRAW_LAYER_POLYLINE3D && layerType != Map::DRAW_LAYER_POLYGON)
	{
		return false;
	}
	fileName.ConcatTo(fileName2);
	sptr = IO::Path::ReplaceExt(fileName2, UTF8STRC("shx"));
	NEW_CLASS(shx, IO::FileStream(CSTRP(fileName2, sptr), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	if (shx->IsError())
	{
		DEL_CLASS(shx);
		return false;
	}
	
	shx->Write(Data::ByteArrayR(buff, 100));
	stm->Write(Data::ByteArrayR(buff, 100));
	Data::ArrayListInt64 objIds;
	sess = layer->BeginGetObject();
	layer->GetAllObjectIds(objIds, &nameArr);
	recCnt = objIds.GetCount();

	if (layerType == Map::DRAW_LAYER_POINT)
	{
		NN<Math::Geometry::Point> pt;
		Math::Coord2DDbl coord;
		ilayerType = 1;
		i = 0;
		while (i < recCnt)
		{
			if (Optional<Math::Geometry::Point>::ConvertFrom(layer->GetNewVectorById(sess, objIds.GetItem(i))).SetTo(pt))
			{
				coord = pt->GetCenter();
				if (i == 0)
				{
					min = max = coord;
				}
				else
				{
					min = min.Min(coord);
					max = max.Max(coord);
				}

				WriteMInt32(buff, (Int32)(fileSize >> 1));
				WriteMInt32(&buff[4], 10);
				shx->Write(Data::ByteArrayR(buff, 8));

				WriteMInt32(buff, (Int32)i);
				*(Int32*)&buff[8] = 1;
				*(Double*)&buff[12] = coord.x;
				*(Double*)&buff[20] = coord.y;
				stm->Write(Data::ByteArrayR(buff, 28));
				fileSize += 28;


				pt.Delete();
			}
			i++;
		}
	}
	else if (layerType == Map::DRAW_LAYER_POINT3D)
	{
		NN<Math::Geometry::PointZ> pt;
		Math::Coord2DDbl coord;
		Math::Vector3 pos;
		ilayerType = 11;
		i = 0;
		while (i < recCnt)
		{
			if (Optional<Math::Geometry::PointZ>::ConvertFrom(layer->GetNewVectorById(sess, objIds.GetItem(i))).SetTo(pt))
			{
				pos = pt->GetPos3D();
				coord = pos.GetXY();
				if (i == 0)
				{
					min = max = coord;
					zMin = zMax = pos.GetZ();
				}
				else
				{
					min = min.Min(coord);
					max = max.Max(coord);
					if (pos.GetZ() > zMax)
					{
						zMax = pos.GetZ();
					}
					else if (pos.GetZ() < zMin)
					{
						zMin = pos.GetZ();
					}
				}

				WriteMInt32(buff, (Int32)(i + 1));
				WriteMInt32(&buff[4], 18);
				shx->Write(Data::ByteArrayR(buff, 8));

				WriteMInt32(buff, (Int32)i);
				*(Int32*)&buff[8] = 11;
				*(Double*)&buff[12] = coord.x;
				*(Double*)&buff[20] = coord.y;
				*(Double*)&buff[28] = pos.GetZ();
				*(Double*)&buff[36] = 0;
				stm->Write(Data::ByteArrayR(buff, 44));
				fileSize += 44;

				pt.Delete();
			}
			i++;
		}
	}
	else if (layerType == Map::DRAW_LAYER_POLYLINE)
	{
		ilayerType = 3;

		NN<Math::Geometry::Polyline> pl;
		Math::RectAreaDbl box;
		UOSInt nPtOfst;
		UOSInt nPoint;
		UInt8 nvals[8];
		UInt32 *ptOfsts;
		Math::Coord2DDbl *points;
		
		i = 0;
		while (i < recCnt)
		{
			Int64 objId = objIds.GetItem(i);
			if (Optional<Math::Geometry::Polyline>::ConvertFrom(layer->GetNewVectorById(sess, objId)).SetTo(pl))
			{
				box = pl->GetBounds();
				nPtOfst = pl->GetCount();
				ptOfsts = MemAlloc(UInt32, nPtOfst);
				nPoint = pl->GetPointCount();
				points = MemAllocA(Math::Coord2DDbl, nPoint);
				pl->FillPointOfstList(points, ptOfsts, 0, 0);
				WriteUInt32(&nvals[0], (UInt32)nPtOfst);
				WriteUInt32(&nvals[4], (UInt32)nPoint);

				if (i == 0)
				{
					min = box.min;
					max = box.max;
				}
				else
				{
					min = min.Min(box.min);
					max = max.Max(box.max);
				}

				WriteMInt32(buff, (Int32)(fileSize >> 1));
				WriteMInt32(&buff[4], (Int32)(22 + 2 * nPtOfst + 8 * nPoint));
				shx->Write(Data::ByteArrayR(buff, 8));

				WriteMInt32(buff, (Int32)(i + 1));
				WriteUInt32(&buff[8], 3);
				stm->Write(Data::ByteArrayR(buff, 12));
				stm->Write(Data::ByteArrayR((UInt8*)&box, 32));
				stm->Write(Data::ByteArrayR(nvals, 8));
				fileSize += 52;
				stm->Write(Data::ByteArrayR((UInt8*)ptOfsts, nPtOfst * 4));
				stm->Write(Data::ByteArrayR((UInt8*)points, nPoint * 16));
				fileSize += nPtOfst * 4 + nPoint * 16;

				MemFreeA(points);
				MemFree(ptOfsts);
				pl.Delete();
			}
			else
			{
				printf("Error in getting object: %lld\r\n", objId);
			}
			i++;
		}
	}
	else if (layerType == Map::DRAW_LAYER_POLYLINE3D)
	{
		ilayerType = 13;

		UOSInt j;
		NN<Math::Geometry::Polyline> pl;
		Math::RectAreaDbl box;
		UOSInt nPtOfst;
		UOSInt nPoint;
		Double ranges[2];
		UInt8 nvals[8];
		UInt32 *ptOfsts;
		Math::Coord2DDbl *points;
		Double *alts;
		
		i = 0;
		while (i < recCnt)
		{
			if (Optional<Math::Geometry::Polyline>::ConvertFrom(layer->GetNewVectorById(sess, objIds.GetItem(i))).SetTo(pl))
			{
				box = pl->GetBounds();
				nPtOfst = pl->GetCount();
				ptOfsts = MemAlloc(UInt32, nPtOfst);
				nPoint = pl->GetPointCount();
				points = MemAllocA(Math::Coord2DDbl, nPoint);
				alts = MemAlloc(Double, nPoint);
				pl->FillPointOfstList(points, ptOfsts, alts, 0);
				WriteUInt32(&nvals[0], (UInt32)nPtOfst);
				WriteUInt32(&nvals[4], (UInt32)nPoint);

				ranges[1] = ranges[0] = alts[0];
				j = nPoint;
				while (j-- > 1)
				{
					if (ranges[1] < alts[j])
						ranges[1] = alts[j];
					if (ranges[0] > alts[j])
						ranges[0] = alts[j];
				}

				if (i == 0)
				{
					min = box.min;
					max = box.max;
					zMin = ranges[0];
					zMax = ranges[1];
				}
				else
				{
					min = min.Min(box.min);
					max = max.Max(box.max);
					if (ranges[1] > zMax)
					{
						zMax = ranges[1];
					}
					else if (ranges[0] < zMin)
					{
						zMin = ranges[1];
					}
				}

				WriteMInt32(buff, (Int32)(fileSize >> 1));
				WriteMInt32(&buff[4], (Int32)(30 + 2 * nPtOfst + 12 * nPoint));
				shx->Write(Data::ByteArrayR(buff, 8));

				WriteMInt32(buff, (Int32)(i + 1));
				WriteUInt32(&buff[8], 3);
				stm->Write(Data::ByteArrayR(buff, 12));
				stm->Write(Data::ByteArrayR((UInt8*)&box, 32));
				stm->Write(Data::ByteArrayR((UInt8*)nvals, 8));
				fileSize += 52;
				stm->Write(Data::ByteArrayR((UInt8*)ptOfsts, nPtOfst * 4));
				stm->Write(Data::ByteArrayR((UInt8*)points, nPoint * 16));
				fileSize += nPtOfst * 4 + nPoint * 16;
				stm->Write(Data::ByteArrayR((UInt8*)ranges, 16));
				stm->Write(Data::ByteArrayR((UInt8*)alts, nPoint * 8));
				fileSize += 16 + nPoint * 8;

				MemFree(alts);
				MemFreeA(points);
				MemFree(ptOfsts);
				pl.Delete();
			}
			i++;
		}
	}
	else if (layerType == Map::DRAW_LAYER_POLYGON)
	{
		ilayerType = 5;

		NN<Math::Geometry::Polygon> pg;
		Math::RectAreaDbl box;
		UOSInt nPtOfst;
		UOSInt nPoint;
		UInt8 nvals[8];
		UInt32 *ptOfsts;
		Math::Coord2DDbl *points;
		
		i = 0;
		while (i < recCnt)
		{
			if (Optional<Math::Geometry::Polygon>::ConvertFrom(layer->GetNewVectorById(sess, objIds.GetItem(i))).SetTo(pg))
			{
				box = pg->GetBounds();
				nPtOfst = pg->GetCount();
				nPoint = pg->GetPointCount();
				ptOfsts = MemAlloc(UInt32, nPtOfst);
				points = MemAllocA(Math::Coord2DDbl, nPoint);
				pg->FillPointOfstList(points, ptOfsts, 0, 0);
				WriteUInt32(&nvals[0], (UInt32)nPtOfst);
				WriteUInt32(&nvals[4], (UInt32)nPoint);

				if (i == 0)
				{
					min = box.min;
					max = box.max;
				}
				else
				{
					min = min.Min(box.min);
					max = max.Max(box.max);
				}

				WriteMInt32(buff, (Int32)(fileSize >> 1));
				WriteMInt32(&buff[4], (Int32)(22 + 2 * nPtOfst + 8 * nPoint));
				shx->Write(Data::ByteArrayR(buff, 8));

				WriteMInt32(buff, (Int32)(i + 1));
				WriteUInt32(&buff[8], 5);
				stm->Write(Data::ByteArrayR(buff, 12));
				stm->Write(Data::ByteArrayR((UInt8*)&box, 32));
				stm->Write(Data::ByteArrayR((UInt8*)nvals, 8));
				fileSize += 52;
				stm->Write(Data::ByteArrayR((UInt8*)ptOfsts, nPtOfst * 4));
				stm->Write(Data::ByteArrayR((UInt8*)points, nPoint * 16));
				fileSize += nPtOfst * 4 + nPoint * 16;
				MemFreeA(points);
				MemFree(ptOfsts);
				pg.Delete();
			}
			i++;
		}
	}
	layer->EndGetObject(sess);
	layer->ReleaseNameArr(nameArr);

	WriteMInt32(buff, 9994);
	*(Int32*)&buff[4] = 0;
	*(Int32*)&buff[8] = 0;
	*(Int32*)&buff[12] = 0;
	*(Int32*)&buff[16] = 0;
	*(Int32*)&buff[20] = 0;
	WriteMInt32(&buff[24], (Int32)(fileSize >> 1));
	*(Int32*)&buff[28] = 1000;
	*(Int32*)&buff[32] = ilayerType;
	*(Double*)&buff[36] = min.x;
	*(Double*)&buff[44] = min.y;
	*(Double*)&buff[52] = max.x;
	*(Double*)&buff[60] = max.y;
	*(Double*)&buff[68] = zMin;
	*(Double*)&buff[76] = zMax;
	*(Double*)&buff[84] = 0;
	*(Double*)&buff[92] = 0;
	stm->SeekFromBeginning(0);
	stm->Write(Data::ByteArrayR(buff, 100));

	WriteMInt32(&buff[24], (Int32)(50 + (recCnt << 2)));
	shx->SeekFromBeginning(0);
	shx->Write(Data::ByteArrayR(buff, 100));
	DEL_CLASS(shx);

	sptr = IO::Path::ReplaceExt(fileName2, UTF8STRC("dbf"));
	{
		IO::FileStream fs(CSTRP(fileName2, sptr), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		Exporter::DBFExporter exporter;
		exporter.SetCodePage(this->codePage);
		exporter.ExportFile(fs, CSTRP(fileName2, sptr), layer, 0);
	}

	sptr = IO::Path::ReplaceExt(fileName2, UTF8STRC("prj"));
	NN<Math::CoordinateSystem> csys = layer->GetCoordinateSystem();
	UTF8Char projArr[1024];
	Math::SRESRIWKTWriter wkt;
	UnsafeArray<UTF8Char> cptr = wkt.WriteCSys(csys, projArr, 0, Text::LineBreakType::None);
	IO::FileStream fs(CSTRP(fileName2, sptr), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	fs.Write(Data::ByteArrayR((UInt8*)projArr, (UOSInt)(cptr - projArr)));
	return true;
}
