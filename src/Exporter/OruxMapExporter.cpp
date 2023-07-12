#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "DB/SQLBuilder.h"
#include "DB/SQLiteFile.h"
#include "Exporter/OruxMapExporter.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "Map/MapDrawLayer.h"
#include "Map/TileMapLayer.h"
#include "Map/OSM/OSMLocalTileMap.h"
#include "Map/OSM/OSMTileMap.h"
#include "Math/Geometry/Polyline.h"
#include "Math/Geometry/Polygon.h"
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
	if (pobj->GetParserType() != IO::ParserType::MapLayer)
	{
		return IO::FileExporter::SupportType::NotSupported;
	}
	Map::MapDrawLayer *layer = (Map::MapDrawLayer *)pobj;
	if (layer->GetObjectClass() == Map::MapDrawLayer::OC_TILE_MAP_LAYER)
	{
		Map::TileMap *tileMap = ((Map::TileMapLayer*)layer)->GetTileMap();
		Map::TileMap::TileType ttype = tileMap->GetTileType();
		if (ttype == Map::TileMap::TT_OSMLOCAL)
		{
			return IO::FileExporter::SupportType::MultiFiles;
		}
		else if (ttype == Map::TileMap::TT_OSM)
		{
			Map::OSM::OSMTileMap *osm = (Map::OSM::OSMTileMap*)tileMap;
			if (osm->HasSPackageFile())
			{
				return IO::FileExporter::SupportType::MultiFiles;
			}
		}
	}
	return IO::FileExporter::SupportType::NotSupported;
}

Bool Exporter::OruxMapExporter::GetOutputName(UOSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff)
{
	if (index == 0)
	{
		Text::StrConcatC(nameBuff, UTF8STRC("OruxMaps"));
		Text::StrConcatC(fileNameBuff, UTF8STRC("*.otrk2.xml"));
		return true;
	}
	return false;
}

Bool Exporter::OruxMapExporter::ExportFile(IO::SeekableStream *stm, Text::CString fileName, IO::ParsedObject *pobj, void *param)
{
	UTF8Char fileName2[512];
	UTF8Char u8fileName[512];
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	if (pobj->GetParserType() != IO::ParserType::MapLayer)
	{
		return false;
	}
	Map::MapDrawLayer *layer = (Map::MapDrawLayer*)pobj;
	if (layer->GetObjectClass() != Map::MapDrawLayer::OC_TILE_MAP_LAYER)
	{
		return false;
	}
	Map::TileMap *tileMap = ((Map::TileMapLayer*)layer)->GetTileMap();
	Map::TileMap::TileType ttype = tileMap->GetTileType();
	Text::UTF8Writer *writer;
	UOSInt i;
	UOSInt j;
	UOSInt level;
	NotNullPtr<Text::String> s;
//	const UTF8Char *csptr;
	Int32 minX;
	Int32 minY;
	Int32 maxX;
	Int32 maxY;
	Double minLat;
	Double minLon;
	Double maxLat;
	Double maxLon;
	Math::RectAreaDbl bounds;
	Bool succ;
	i = fileName.LastIndexOf(IO::Path::PATH_SEPERATOR);
	sptr = Text::StrConcatC(fileName2, &fileName.v[i + 1], fileName.leng - i - 1);
	i = Text::StrLastIndexOfCharC(fileName2, (UOSInt)(sptr - fileName2), '.');
	if (i != INVALID_INDEX)
	{
		fileName2[i] = 0;
		sptr = &fileName2[i];
	}
	if (Text::StrEndsWithC(fileName2, (UOSInt)(sptr - fileName2), UTF8STRC(".otrk2")))
	{
		sptr -= 6;
		*sptr = 0;
	}

	if (ttype == Map::TileMap::TT_OSMLOCAL)
	{
		DB::SQLiteFile *db;
		s = Text::XML::ToNewXMLText(fileName2);
		succ = false;

		sptr = fileName.ConcatTo(u8fileName);
		sptr = IO::Path::AppendPath(u8fileName, sptr, CSTR("OruxMapsImages.db"));
		NEW_CLASS(db, DB::SQLiteFile(CSTRP(u8fileName, sptr)));
		if (!db->IsError())
		{
			Data::ArrayList<Math::Coord2D<Int32>> imgIds;
			Map::TileMap::ImageType it;
			IO::StreamData *fd;
			DB::SQLBuilder sql(db->GetSQLType(), db->IsAxisAware(), db->GetTzQhr());
			db->ExecuteNonQuery(CSTR("CREATE TABLE android_metadata (locale TEXT)"));
			db->ExecuteNonQuery(CSTR("CREATE TABLE tiles (x int, y int, z int, image blob, PRIMARY KEY (x,y,z))"));
			db->ExecuteNonQuery(CSTR("delete from android_metadata"));
			db->ExecuteNonQuery(CSTR("delete from tiles"));
			Text::Locale::LocaleEntry *loc = Text::Locale::GetLocaleEntry(Text::EncodingFactory::GetSystemLCID());
			if (loc)
			{
				sql.Clear();
				sql.AppendCmdC(CSTR("insert into android_metadata (locale) values ("));
				sql.AppendStrUTF8((const UTF8Char*)"zh_TW");//loc->shortName);
				sql.AppendCmdC(CSTR(")"));
				db->ExecuteNonQuery(sql.ToCString());
			}
			succ = true;
			Map::OSM::OSMLocalTileMap *osm = (Map::OSM::OSMLocalTileMap*)tileMap;
			NEW_CLASS(writer, Text::UTF8Writer(stm));
			writer->WriteStrC(UTF8STRC("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"));
			writer->WriteStrC(UTF8STRC("<OruxTracker xmlns=\"http://oruxtracker.com/app/res/calibration\"\n"));
			writer->WriteStrC(UTF8STRC(" versionCode=\"3.0\">\n"));
			writer->WriteStrC(UTF8STRC("<MapCalibration layers=\"true\" layerLevel=\"0\">\n"));
			writer->WriteStrC(UTF8STRC("<MapName><![CDATA["));
			writer->WriteStrC(s->v, s->leng);
			writer->WriteStrC(UTF8STRC("]]></MapName>\n"));
			level = 0;
			while (level <= 18)
			{
				if (osm->GetTileBounds(level, &minX, &minY, &maxX, &maxY))
				{
					minLon = Map::OSM::OSMTileMap::TileX2Lon(minX, level);
					maxLon = Map::OSM::OSMTileMap::TileX2Lon(maxX + 1, level);
					minLat = Map::OSM::OSMTileMap::TileY2Lat(maxY + 1, level);
					maxLat = Map::OSM::OSMTileMap::TileY2Lat(minY, level);
					writer->WriteStrC(UTF8STRC("<OruxTracker  versionCode=\"2.1\">\n"));

					writer->WriteStrC(UTF8STRC("<MapCalibration layers=\"false\" layerLevel=\""));
					sptr = Text::StrUOSInt(sbuff, level);
					writer->WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
					writer->WriteStrC(UTF8STRC("\">\n"));

					writer->WriteStrC(UTF8STRC("<MapName><![CDATA["));
					writer->WriteStrC(s->v, s->leng);
					writer->WriteStrC(UTF8STRC("]]></MapName>\n"));

					writer->WriteStrC(UTF8STRC("<MapChunks xMax=\""));
					sptr = Text::StrInt32(sbuff, maxX - minX + 1);
					writer->WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
					writer->WriteStrC(UTF8STRC("\" yMax=\""));
					sptr = Text::StrInt32(sbuff, maxY - minY + 1);
					writer->WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
					writer->WriteStrC(UTF8STRC("\" datum=\"WGS84\" projection=\"Mercator\" img_height=\"256\" img_width=\"256\" file_name=\""));
					writer->WriteStrC(s->v, s->leng);
					writer->WriteStrC(UTF8STRC("\" />\n"));

					writer->WriteStrC(UTF8STRC("<MapDimensions height=\""));
					sptr = Text::StrInt32(sbuff, (maxY - minY + 1) * 256);
					writer->WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
					writer->WriteStrC(UTF8STRC("\" width=\""));
					sptr = Text::StrInt32(sbuff, (maxX - minX + 1) * 256);
					writer->WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
					writer->WriteStrC(UTF8STRC("\" />\n"));

					writer->WriteStrC(UTF8STRC("<MapBounds minLat=\""));
					sptr = Text::StrDouble(sbuff, minLat);
					writer->WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
					writer->WriteStrC(UTF8STRC("\" maxLat=\""));
					sptr = Text::StrDouble(sbuff, maxLat);
					writer->WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
					writer->WriteStrC(UTF8STRC("\" minLon=\""));
					sptr = Text::StrDouble(sbuff, minLon);
					writer->WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
					writer->WriteStrC(UTF8STRC("\" maxLon=\""));
					sptr = Text::StrDouble(sbuff, maxLon);
					writer->WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
					writer->WriteStrC(UTF8STRC("\" />\n"));

					writer->WriteStrC(UTF8STRC("<CalibrationPoints>\n"));

					writer->WriteStrC(UTF8STRC("<CalibrationPoint corner=\"TL\" lon=\""));
					sptr = Text::StrDouble(sbuff, minLon);
					writer->WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
					writer->WriteStrC(UTF8STRC("\" lat=\""));
					sptr = Text::StrDouble(sbuff, maxLat);
					writer->WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
					writer->WriteStrC(UTF8STRC("\" />\n"));

					writer->WriteStrC(UTF8STRC("<CalibrationPoint corner=\"BR\" lon=\""));
					sptr = Text::StrDouble(sbuff, maxLon);
					writer->WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
					writer->WriteStrC(UTF8STRC("\" lat=\""));
					sptr = Text::StrDouble(sbuff, minLat);
					writer->WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
					writer->WriteStrC(UTF8STRC("\" />\n"));

					writer->WriteStrC(UTF8STRC("<CalibrationPoint corner=\"TR\" lon=\""));
					sptr = Text::StrDouble(sbuff, maxLon);
					writer->WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
					writer->WriteStrC(UTF8STRC("\" lat=\""));
					sptr = Text::StrDouble(sbuff, maxLat);
					writer->WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
					writer->WriteStrC(UTF8STRC("\" />\n"));

					writer->WriteStrC(UTF8STRC("<CalibrationPoint corner=\"BL\" lon=\""));
					sptr = Text::StrDouble(sbuff, minLon);
					writer->WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
					writer->WriteStrC(UTF8STRC("\" lat=\""));
					sptr = Text::StrDouble(sbuff, minLat);
					writer->WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
					writer->WriteStrC(UTF8STRC("\" />\n"));

					writer->WriteStrC(UTF8STRC("</CalibrationPoints>\n"));
					writer->WriteStrC(UTF8STRC("</MapCalibration>\n"));
					writer->WriteStrC(UTF8STRC("</OruxTracker>\n"));
				
					imgIds.Clear();
					osm->GetTileImageIDs(level, Math::RectAreaDbl(Math::Coord2DDbl(minLon, maxLat), Math::Coord2DDbl(maxLon, minLat)), &imgIds);
					void *sess = db->BeginTransaction();
					j = imgIds.GetCount();
					while (j-- > 0)
					{
						Math::Coord2D<Int32> tileId = imgIds.GetItem(j);
						fd = osm->LoadTileImageData(level, tileId, &bounds, true, &it);
						if (fd)
						{
							UOSInt imgSize = (UOSInt)fd->GetDataSize();
							UInt8 *imgBuff = MemAlloc(UInt8, imgSize);
							fd->GetRealData(0, imgSize, imgBuff);
							sql.Clear();
							sql.AppendCmdC(CSTR("insert into tiles (x, y, z, image) values ("));
							sql.AppendInt32(tileId.x - minX);
							sql.AppendCmdC(CSTR(", "));
							sql.AppendInt32(tileId.y - minY);
							sql.AppendCmdC(CSTR(", "));
							sql.AppendInt32((Int32)(UInt32)level);
							sql.AppendCmdC(CSTR(", "));
							sql.AppendBinary(imgBuff, imgSize);
							sql.AppendCmdC(CSTR(")"));
							db->ExecuteNonQuery(sql.ToCString());
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

				level++;
			}
			s->Release();
			writer->WriteStrC(UTF8STRC("</MapCalibration>\n"));
			writer->WriteStrC(UTF8STRC("</OruxTracker>\n"));
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
