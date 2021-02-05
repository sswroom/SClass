#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "DB/SQLBuilder.h"
#include "DB/SQLiteFile.h"
#include "Exporter/OruxMapExporter.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "Map/IMapDrawLayer.h"
#include "Map/TileMapLayer.h"
#include "Map/OSM/OSMLocalTileMap.h"
#include "Map/OSM/OSMTileMap.h"
#include "Math/Point3D.h"
#include "Math/Polyline.h"
#include "Math/Polygon.h"
#include "Text/Locale.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/UTF8Writer.h"
#include "Text/XML.h"

Exporter::OruxMapExporter::OruxMapExporter()
{
}

Exporter::OruxMapExporter::~OruxMapExporter()
{
}

Int32 Exporter::OruxMapExporter::GetName()
{
	return *(Int32*)"ORUX";
}

IO::FileExporter::SupportType Exporter::OruxMapExporter::IsObjectSupported(IO::ParsedObject *pobj)
{
	if (pobj->GetParserType() != IO::ParsedObject::PT_MAP_LAYER_PARSER)
	{
		return IO::FileExporter::ST_NOT_SUPPORTED;
	}
	Map::IMapDrawLayer *layer = (Map::IMapDrawLayer *)pobj;
	if (layer->GetObjectClass() == Map::IMapDrawLayer::OC_TILE_MAP_LAYER)
	{
		Map::TileMap *tileMap = ((Map::TileMapLayer*)layer)->GetTileMap();
		Map::TileMap::TileType ttype = tileMap->GetTileType();
		if (ttype == Map::TileMap::TT_OSMLOCAL)
		{
			return IO::FileExporter::ST_MULTI_FILES;
		}
		else if (ttype == Map::TileMap::TT_OSM)
		{
			Map::OSM::OSMTileMap *osm = (Map::OSM::OSMTileMap*)tileMap;
			if (osm->HasSPackageFile())
			{
				return IO::FileExporter::ST_MULTI_FILES;
			}
		}
	}
	return IO::FileExporter::ST_NOT_SUPPORTED;
}

Bool Exporter::OruxMapExporter::GetOutputName(OSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff)
{
	if (index == 0)
	{
		Text::StrConcat(nameBuff, (const UTF8Char*)"OruxMaps");
		Text::StrConcat(fileNameBuff, (const UTF8Char*)"*.otrk2.xml");
		return true;
	}
	return false;
}

Bool Exporter::OruxMapExporter::ExportFile(IO::SeekableStream *stm, const UTF8Char *fileName, IO::ParsedObject *pobj, void *param)
{
	UTF8Char fileName2[512];
	UTF8Char u8fileName[512];
	UTF8Char sbuff[256];
	if (pobj->GetParserType() != IO::ParsedObject::PT_MAP_LAYER_PARSER)
	{
		return false;
	}
	Map::IMapDrawLayer *layer = (Map::IMapDrawLayer*)pobj;
	if (layer->GetObjectClass() != Map::IMapDrawLayer::OC_TILE_MAP_LAYER)
	{
		return false;
	}
	Map::TileMap *tileMap = ((Map::TileMapLayer*)layer)->GetTileMap();
	Map::TileMap::TileType ttype = tileMap->GetTileType();
	Text::UTF8Writer *writer;
	OSInt i;
	OSInt j;
	const UTF8Char *csptr;
	Int32 minX;
	Int32 minY;
	Int32 maxX;
	Int32 maxY;
	Int32 x;
	Int32 y;
	Double minLat;
	Double minLon;
	Double maxLat;
	Double maxLon;
	Double boundsXY[4];
	Bool succ;
	i = Text::StrLastIndexOf(fileName, IO::Path::PATH_SEPERATOR);
	Text::StrConcat(fileName2, &fileName[i + 1]);
	i = Text::StrLastIndexOf(fileName2, '.');
	if (i >= 0)
	{
		fileName2[i] = 0;
	}
	if (Text::StrEndsWith(fileName2, (const UTF8Char*)".otrk2"))
	{
		fileName2[Text::StrCharCnt(fileName2) - 6] = 0;
	}

	if (ttype == Map::TileMap::TT_OSMLOCAL)
	{
		DB::SQLiteFile *db;
		csptr = Text::XML::ToNewXMLText(fileName2);
		succ = false;

		Text::StrConcat(u8fileName, fileName);
		IO::Path::AppendPath(u8fileName, (const UTF8Char*)"OruxMapsImages.db");
		NEW_CLASS(db, DB::SQLiteFile(u8fileName));
		if (!db->IsError())
		{
			Data::ArrayList<Int64> imgIds;
			Map::TileMap::ImageType it;
			IO::IStreamData *fd;
			DB::SQLBuilder sql(db->GetSvrType(), db->GetTzQhr());
			db->ExecuteNonQuery((const UTF8Char*)"CREATE TABLE android_metadata (locale TEXT)");
			db->ExecuteNonQuery((const UTF8Char*)"CREATE TABLE tiles (x int, y int, z int, image blob, PRIMARY KEY (x,y,z))");
			db->ExecuteNonQuery((const UTF8Char*)"delete from android_metadata");
			db->ExecuteNonQuery((const UTF8Char*)"delete from tiles");
			Text::Locale::LocaleEntry *loc = Text::Locale::GetLocaleEntry(Text::EncodingFactory::GetSystemLCID());
			if (loc)
			{
				sql.Clear();
				sql.AppendCmd((const UTF8Char*)"insert into android_metadata (locale) values (");
				sql.AppendStrUTF8((const UTF8Char*)"zh_TW");//loc->shortName);
				sql.AppendCmd((const UTF8Char*)")");
				db->ExecuteNonQuery(sql.ToString());
			}
			succ = true;
			Map::OSM::OSMLocalTileMap *osm = (Map::OSM::OSMLocalTileMap*)tileMap;
			NEW_CLASS(writer, Text::UTF8Writer(stm));
			writer->Write((const UTF8Char*)"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
			writer->Write((const UTF8Char*)"<OruxTracker xmlns=\"http://oruxtracker.com/app/res/calibration\"\n");
			writer->Write((const UTF8Char*)" versionCode=\"3.0\">\n");
			writer->Write((const UTF8Char*)"<MapCalibration layers=\"true\" layerLevel=\"0\">\n");
			writer->Write((const UTF8Char*)"<MapName><![CDATA[");
			writer->Write(csptr);
			writer->Write((const UTF8Char*)"]]></MapName>\n");
			i = 0;
			while (i <= 18)
			{
				if (osm->GetTileBounds(i, &minX, &minY, &maxX, &maxY))
				{
					minLon = Map::OSM::OSMTileMap::TileX2Lon(minX, i);
					maxLon = Map::OSM::OSMTileMap::TileX2Lon(maxX + 1, i);
					minLat = Map::OSM::OSMTileMap::TileY2Lat(maxY + 1, i);
					maxLat = Map::OSM::OSMTileMap::TileY2Lat(minY, i);
					writer->Write((const UTF8Char*)"<OruxTracker  versionCode=\"2.1\">\n");

					writer->Write((const UTF8Char*)"<MapCalibration layers=\"false\" layerLevel=\"");
					Text::StrOSInt(sbuff, i);
					writer->Write(sbuff);
					writer->Write((const UTF8Char*)"\">\n");

					writer->Write((const UTF8Char*)"<MapName><![CDATA[");
					writer->Write(csptr);
					writer->Write((const UTF8Char*)"]]></MapName>\n");

					writer->Write((const UTF8Char*)"<MapChunks xMax=\"");
					Text::StrInt32(sbuff, maxX - minX + 1);
					writer->Write(sbuff);
					writer->Write((const UTF8Char*)"\" yMax=\"");
					Text::StrInt32(sbuff, maxY - minY + 1);
					writer->Write(sbuff);
					writer->Write((const UTF8Char*)"\" datum=\"WGS84\" projection=\"Mercator\" img_height=\"256\" img_width=\"256\" file_name=\"");
					writer->Write(csptr);
					writer->Write((const UTF8Char*)"\" />\n");

					writer->Write((const UTF8Char*)"<MapDimensions height=\"");
					Text::StrInt32(sbuff, (maxY - minY + 1) * 256);
					writer->Write(sbuff);
					writer->Write((const UTF8Char*)"\" width=\"");
					Text::StrInt32(sbuff, (maxX - minX + 1) * 256);
					writer->Write(sbuff);
					writer->Write((const UTF8Char*)"\" />\n");

					writer->Write((const UTF8Char*)"<MapBounds minLat=\"");
					Text::StrDouble(sbuff, minLat);
					writer->Write(sbuff);
					writer->Write((const UTF8Char*)"\" maxLat=\"");
					Text::StrDouble(sbuff, maxLat);
					writer->Write(sbuff);
					writer->Write((const UTF8Char*)"\" minLon=\"");
					Text::StrDouble(sbuff, minLon);
					writer->Write(sbuff);
					writer->Write((const UTF8Char*)"\" maxLon=\"");
					Text::StrDouble(sbuff, maxLon);
					writer->Write(sbuff);
					writer->Write((const UTF8Char*)"\" />\n");

					writer->Write((const UTF8Char*)"<CalibrationPoints>\n");

					writer->Write((const UTF8Char*)"<CalibrationPoint corner=\"TL\" lon=\"");
					Text::StrDouble(sbuff, minLon);
					writer->Write(sbuff);
					writer->Write((const UTF8Char*)"\" lat=\"");
					Text::StrDouble(sbuff, maxLat);
					writer->Write(sbuff);
					writer->Write((const UTF8Char*)"\" />\n");

					writer->Write((const UTF8Char*)"<CalibrationPoint corner=\"BR\" lon=\"");
					Text::StrDouble(sbuff, maxLon);
					writer->Write(sbuff);
					writer->Write((const UTF8Char*)"\" lat=\"");
					Text::StrDouble(sbuff, minLat);
					writer->Write(sbuff);
					writer->Write((const UTF8Char*)"\" />\n");

					writer->Write((const UTF8Char*)"<CalibrationPoint corner=\"TR\" lon=\"");
					Text::StrDouble(sbuff, maxLon);
					writer->Write(sbuff);
					writer->Write((const UTF8Char*)"\" lat=\"");
					Text::StrDouble(sbuff, maxLat);
					writer->Write(sbuff);
					writer->Write((const UTF8Char*)"\" />\n");

					writer->Write((const UTF8Char*)"<CalibrationPoint corner=\"BL\" lon=\"");
					Text::StrDouble(sbuff, minLon);
					writer->Write(sbuff);
					writer->Write((const UTF8Char*)"\" lat=\"");
					Text::StrDouble(sbuff, minLat);
					writer->Write(sbuff);
					writer->Write((const UTF8Char*)"\" />\n");

					writer->Write((const UTF8Char*)"</CalibrationPoints>\n");
					writer->Write((const UTF8Char*)"</MapCalibration>\n");
					writer->Write((const UTF8Char*)"</OruxTracker>\n");
				
					imgIds.Clear();
					osm->GetImageIDs(i, minLon, maxLat, maxLon, minLat, &imgIds);
					void *sess = db->BeginTransaction();
					j = imgIds.GetCount();
					while (j-- > 0)
					{
						fd = osm->LoadTileImageData(i, imgIds.GetItem(j), boundsXY, true, &x, &y, &it);
						if (fd)
						{
							OSInt imgSize = (OSInt)fd->GetDataSize();
							UInt8 *imgBuff = MemAlloc(UInt8, imgSize);
							fd->GetRealData(0, imgSize, imgBuff);
							sql.Clear();
							sql.AppendCmd((const UTF8Char*)"insert into tiles (x, y, z, image) values (");
							sql.AppendInt32(x - minX);
							sql.AppendCmd((const UTF8Char*)", ");
							sql.AppendInt32(y - minY);
							sql.AppendCmd((const UTF8Char*)", ");
							sql.AppendInt32((Int32)i);
							sql.AppendCmd((const UTF8Char*)", ");
							sql.AppendBinary(imgBuff, imgSize);
							sql.AppendCmd((const UTF8Char*)")");
							db->ExecuteNonQuery(sql.ToString());
							MemFree(imgBuff);
							DEL_CLASS(fd);
							if (j != 0 && (j & 0x3fff) == 0)
							{
								db->Commit(sess);
								sess = db->BeginTransaction();
							}
						}
					}
					db->Commit(sess);
				}

				i++;
			}
			Text::XML::FreeNewText(csptr);
			writer->Write((const UTF8Char*)"</MapCalibration>\n");
			writer->Write((const UTF8Char*)"</OruxTracker>\n");
			DEL_CLASS(writer);
		}
		DEL_CLASS(db);
		return succ;
	}
	else if (ttype == Map::TileMap::TT_OSM)
	{
		Map::OSM::OSMTileMap *osm = (Map::OSM::OSMTileMap*)tileMap;
		if (!osm->HasSPackageFile())
		{
			return false;
		}
		NEW_CLASS(writer, Text::UTF8Writer(stm));
		////////////////////////////////////////////
		DEL_CLASS(writer);
		return true;
	}
	return false;
}
