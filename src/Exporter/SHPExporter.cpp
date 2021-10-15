#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Exporter/DBFExporter.h"
#include "Exporter/SHPExporter.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "Map/IMapDrawLayer.h"
#include "Math/Point3D.h"
#include "Math/Polyline.h"
#include "Math/Polygon.h"
#include "Math/SRESRIWKTWriter.h"
#include "Text/MyString.h"

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

IO::FileExporter::SupportType Exporter::SHPExporter::IsObjectSupported(IO::ParsedObject *pobj)
{
	if (pobj->GetParserType() != IO::ParserType::MapLayer)
	{
		return IO::FileExporter::SupportType::NotSupported;
	}
	Map::IMapDrawLayer *layer = (Map::IMapDrawLayer *)pobj;
	Map::DrawLayerType layerType = layer->GetLayerType();
	if (layerType == Map::DRAW_LAYER_POINT || layerType == Map::DRAW_LAYER_POINT3D || layerType == Map::DRAW_LAYER_POLYLINE || layerType == Map::DRAW_LAYER_POLYLINE3D || layerType == Map::DRAW_LAYER_POLYGON)
	{
		return IO::FileExporter::SupportType::MultiFiles;
	}
	return IO::FileExporter::SupportType::NotSupported;
}

Bool Exporter::SHPExporter::GetOutputName(UOSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff)
{
	if (index == 0)
	{
		Text::StrConcat(nameBuff, (const UTF8Char*)"ESRI Shapefile");
		Text::StrConcat(fileNameBuff, (const UTF8Char*)"*.shp");
		return true;
	}
	return false;
}

void Exporter::SHPExporter::SetCodePage(UInt32 codePage)
{
	this->codePage = codePage;
}

Bool Exporter::SHPExporter::ExportFile(IO::SeekableStream *stm, const UTF8Char *fileName, IO::ParsedObject *pobj, void *param)
{
	UInt8 buff[256];
	UTF8Char fileName2[256];
	if (pobj->GetParserType() != IO::ParserType::MapLayer)
	{
		return false;
	}
	Map::IMapDrawLayer *layer = (Map::IMapDrawLayer *)pobj;
	Map::DrawLayerType layerType = layer->GetLayerType();
	UOSInt fileSize = 100;
	UOSInt recCnt = 0;
	UOSInt i;
	Int32 ilayerType = 0;
	Double xMin = 0;
	Double xMax = 0;
	Double yMin = 0;
	Double yMax = 0;
	Double zMin = 0;
	Double zMax = 0;
	IO::FileStream *shx;
	Data::ArrayListInt64 *objIds;
	Exporter::DBFExporter *exporter;
	void *nameArr;
	void *sess;
	if (layerType != Map::DRAW_LAYER_POINT && layerType != Map::DRAW_LAYER_POINT3D && layerType != Map::DRAW_LAYER_POLYLINE && layerType != Map::DRAW_LAYER_POLYLINE3D && layerType != Map::DRAW_LAYER_POLYGON)
	{
		return false;
	}
	Text::StrConcat(fileName2, fileName);
	IO::Path::ReplaceExt(fileName2, (const UTF8Char*)"shx");
	NEW_CLASS(shx, IO::FileStream(fileName2, IO::FileStream::FileMode::Create, IO::FileStream::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	if (shx->IsError())
	{
		DEL_CLASS(shx);
		return false;
	}
	
	shx->Write(buff, 100);
	stm->Write(buff, 100);
	NEW_CLASS(objIds, Data::ArrayListInt64());
	sess = layer->BeginGetObject();
	layer->GetAllObjectIds(objIds, &nameArr);
	recCnt = objIds->GetCount();

	if (layerType == Map::DRAW_LAYER_POINT)
	{
		Math::Point *pt;
		Double x;
		Double y;
		ilayerType = 1;
		i = 0;
		while (i < recCnt)
		{
			pt = (Math::Point*)layer->GetVectorById(sess, objIds->GetItem(i));
			pt->GetCenter(&x, &y);
			if (i == 0)
			{
				xMin = xMax = x;
				yMin = yMax = y;
			}
			else
			{
				if (x > xMax)
				{
					xMax = x;
				}
				else if (x < xMin)
				{
					xMin = x;
				}
				if (y > yMax)
				{
					yMax = y;
				}
				else if (y < yMin)
				{
					yMin = y;
				}
			}

			WriteMInt32(buff, (Int32)(fileSize >> 1));
			WriteMInt32(&buff[4], 10);
			shx->Write(buff, 8);

			WriteMInt32(buff, (Int32)i);
			*(Int32*)&buff[8] = 1;
			*(Double*)&buff[12] = x;
			*(Double*)&buff[20] = y;
			stm->Write(buff, 28);
			fileSize += 28;


			DEL_CLASS(pt);
			i++;
		}
	}
	else if (layerType == Map::DRAW_LAYER_POINT3D)
	{
		Math::Point3D *pt;
		Double x;
		Double y;
		Double z;
		ilayerType = 11;
		i = 0;
		while (i < recCnt)
		{
			pt = (Math::Point3D*)layer->GetVectorById(sess, objIds->GetItem(i));
			pt->GetCenter3D(&x, &y, &z);
			if (i == 0)
			{
				xMin = xMax = x;
				yMin = yMax = y;
				zMin = zMax = z;
			}
			else
			{
				if (x > xMax)
				{
					xMax = x;
				}
				else if (x < xMin)
				{
					xMin = x;
				}
				if (y > yMax)
				{
					yMax = y;
				}
				else if (y < yMin)
				{
					yMin = y;
				}
				if (z > zMax)
				{
					zMax = z;
				}
				else if (z < zMin)
				{
					zMin = z;
				}
			}

			WriteMInt32(buff, (Int32)(fileSize >> 1));
			WriteMInt32(&buff[4], 18);
			shx->Write(buff, 8);

			WriteMInt32(buff, (Int32)i);
			*(Int32*)&buff[8] = 11;
			*(Double*)&buff[12] = x;
			*(Double*)&buff[20] = y;
			*(Double*)&buff[28] = z;
			*(Double*)&buff[36] = 0;
			stm->Write(buff, 44);
			fileSize += 44;


			DEL_CLASS(pt);
			i++;
		}
	}
	else if (layerType == Map::DRAW_LAYER_POLYLINE)
	{
		ilayerType = 3;

		Math::Polyline *pl;
		Double box[4];
		UOSInt nPtOfst;
		UOSInt nPoint;
		UOSInt nvals[2];
		UInt32 *ptOfsts;
		Double *points;
		
		i = 0;
		while (i < recCnt)
		{
			pl = (Math::Polyline*)layer->GetVectorById(sess, objIds->GetItem(i));
			pl->GetBounds(&box[0], &box[1], &box[2], &box[3]);
			ptOfsts = pl->GetPtOfstList(&nPtOfst);
			points = pl->GetPointList(&nPoint);
			nvals[0] = nPtOfst;
			nvals[1] = nPoint;

			if (i == 0)
			{
				xMin = box[0];
				yMin = box[1];
				xMax = box[2];
				yMax = box[3];
			}
			else
			{
				if (box[2] > xMax)
				{
					xMax = box[2];
				}
				else if (box[0] < xMin)
				{
					xMin = box[0];
				}
				if (box[3] > yMax)
				{
					yMax = box[3];
				}
				else if (box[1] < yMin)
				{
					yMin = box[1];
				}
			}

			WriteMInt32(buff, (Int32)(fileSize >> 1));
			WriteMInt32(&buff[4], (Int32)(22 + 2 * nPtOfst + 8 * nPoint));
			shx->Write(buff, 8);

			WriteMInt32(buff, (Int32)i);
			*(Int32*)&buff[8] = 3;
			stm->Write(buff, 12);
			stm->Write((UInt8*)box, 32);
			stm->Write((UInt8*)nvals, 8);
			fileSize += 52;
			stm->Write((UInt8*)ptOfsts, nPtOfst * 4);
			stm->Write((UInt8*)points, nPoint * 16);
			fileSize += nPtOfst * 4 + nPoint * 16;

			DEL_CLASS(pl);
			i++;
		}
	}
	else if (layerType == Map::DRAW_LAYER_POLYLINE3D)
	{
		ilayerType = 13;

		UOSInt j;
		Math::Polyline3D *pl;
		Double box[4];
		UOSInt nPtOfst;
		UOSInt nPoint;
		Double ranges[2];
		UOSInt nvals[2];
		UInt32 *ptOfsts;
		Double *points;
		Double *alts;
		
		i = 0;
		while (i < recCnt)
		{
			pl = (Math::Polyline3D*)layer->GetVectorById(sess, objIds->GetItem(i));
			pl->GetBounds(&box[0], &box[1], &box[2], &box[3]);
			ptOfsts = pl->GetPtOfstList(&nPtOfst);
			points = pl->GetPointList(&nPoint);
			alts = pl->GetAltitudeList(&nPoint);
			nvals[0] = nPtOfst;
			nvals[1] = nPoint;

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
				xMin = box[0];
				yMin = box[1];
				xMax = box[2];
				yMax = box[3];
				zMin = ranges[0];
				zMax = ranges[1];
			}
			else
			{
				if (box[2] > xMax)
				{
					xMax = box[2];
				}
				else if (box[0] < xMin)
				{
					xMin = box[0];
				}
				if (box[3] > yMax)
				{
					yMax = box[3];
				}
				else if (box[1] < yMin)
				{
					yMin = box[1];
				}
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
			shx->Write(buff, 8);

			WriteMInt32(buff, (Int32)i);
			*(Int32*)&buff[8] = 3;
			stm->Write(buff, 12);
			stm->Write((UInt8*)box, 32);
			stm->Write((UInt8*)nvals, 8);
			fileSize += 52;
			stm->Write((UInt8*)ptOfsts, nPtOfst * 4);
			stm->Write((UInt8*)points, nPoint * 16);
			fileSize += nPtOfst * 4 + nPoint * 16;
			stm->Write((UInt8*)ranges, 16);
			stm->Write((UInt8*)alts, nPoint * 8);
			fileSize += 16 + nPoint * 8;

			DEL_CLASS(pl);
			i++;
		}
	}
	else if (layerType == Map::DRAW_LAYER_POLYGON)
	{
		ilayerType = 5;

		Math::Polygon *pg;
		Double box[4];
		UOSInt nPtOfst;
		UOSInt nPoint;
		UOSInt nvals[2];
		UInt32 *ptOfsts;
		Double *points;
		
		i = 0;
		while (i < recCnt)
		{
			pg = (Math::Polygon*)layer->GetVectorById(sess, objIds->GetItem(i));
			pg->GetBounds(&box[0], &box[1], &box[2], &box[3]);
			ptOfsts = pg->GetPtOfstList(&nPtOfst);
			points = pg->GetPointList(&nPoint);
			nvals[0] = nPtOfst;
			nvals[1] = nPoint;

			if (i == 0)
			{
				xMin = box[0];
				yMin = box[1];
				xMax = box[2];
				yMax = box[3];
			}
			else
			{
				if (box[2] > xMax)
				{
					xMax = box[2];
				}
				else if (box[0] < xMin)
				{
					xMin = box[0];
				}
				if (box[3] > yMax)
				{
					yMax = box[3];
				}
				else if (box[1] < yMin)
				{
					yMin = box[1];
				}
			}

			WriteMInt32(buff, (Int32)(fileSize >> 1));
			WriteMInt32(&buff[4], (Int32)(22 + 2 * nPtOfst + 8 * nPoint));
			shx->Write(buff, 8);

			WriteMInt32(buff, (Int32)i);
			*(Int32*)&buff[8] = 5;
			stm->Write(buff, 12);
			stm->Write((UInt8*)box, 32);
			stm->Write((UInt8*)nvals, 8);
			fileSize += 52;
			stm->Write((UInt8*)ptOfsts, nPtOfst * 4);
			stm->Write((UInt8*)points, nPoint * 16);
			fileSize += nPtOfst * 4 + nPoint * 16;

			DEL_CLASS(pg);
			i++;
		}
	}
	layer->EndGetObject(sess);
	layer->ReleaseNameArr(nameArr);
	DEL_CLASS(objIds);

	WriteMInt32(buff, 9994);
	*(Int32*)&buff[4] = 0;
	*(Int32*)&buff[8] = 0;
	*(Int32*)&buff[12] = 0;
	*(Int32*)&buff[16] = 0;
	*(Int32*)&buff[20] = 0;
	WriteMInt32(&buff[24], (Int32)(fileSize >> 1));
	*(Int32*)&buff[28] = 1000;
	*(Int32*)&buff[32] = ilayerType;
	*(Double*)&buff[36] = xMin;
	*(Double*)&buff[44] = yMin;
	*(Double*)&buff[52] = xMax;
	*(Double*)&buff[60] = yMax;
	*(Double*)&buff[68] = zMin;
	*(Double*)&buff[76] = zMax;
	*(Double*)&buff[84] = 0;
	*(Double*)&buff[92] = 0;
	stm->SeekFromBeginning(0);
	stm->Write(buff, 100);

	WriteMInt32(&buff[24], (Int32)(50 + (recCnt << 2)));
	shx->SeekFromBeginning(0);
	shx->Write(buff, 100);
	DEL_CLASS(shx);

	IO::Path::ReplaceExt(fileName2, (const UTF8Char*)"dbf");
	NEW_CLASS(shx, IO::FileStream(fileName2, IO::FileStream::FileMode::Create, IO::FileStream::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	NEW_CLASS(exporter, Exporter::DBFExporter());
	exporter->SetCodePage(this->codePage);
	exporter->ExportFile(shx, fileName2, layer, 0);
	DEL_CLASS(exporter);
	DEL_CLASS(shx);

	IO::Path::ReplaceExt(fileName2, (const UTF8Char*)"prj");
	Math::CoordinateSystem *csys = layer->GetCoordinateSystem();
	if (csys)
	{
		Char projArr[1024];
		Math::SRESRIWKTWriter wkt;
		Char *cptr = wkt.WriteCSys(csys, projArr, 0, Text::LineBreakType::None);
		NEW_CLASS(shx, IO::FileStream(fileName2, IO::FileStream::FileMode::Create, IO::FileStream::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
		shx->Write((UInt8*)projArr, (UOSInt)(cptr - projArr));
		DEL_CLASS(shx);
	}
	return true;
}
