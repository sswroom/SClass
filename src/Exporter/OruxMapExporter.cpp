#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteBuffer.h"
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

IO::FileExporter::SupportType Exporter::OruxMapExporter::IsObjectSupported(NotNullPtr<IO::ParsedObject> pobj)
{
	if (pobj->GetParserType() != IO::ParserType::MapLayer)
	{
		return IO::FileExporter::SupportType::NotSupported;
	}
	NotNullPtr<Map::MapDrawLayer> layer = NotNullPtr<Map::MapDrawLayer>::ConvertFrom(pobj);
	if (layer->GetObjectClass() == Map::MapDrawLayer::OC_TILE_MAP_LAYER)
	{
		NotNullPtr<Map::TileMap> tileMap = NotNullPtr<Map::TileMapLayer>::ConvertFrom(layer)->GetTileMap();
		Map::TileMap::TileType ttype = tileMap->GetTileType();
		if (ttype == Map::TileMap::TT_OSMLOCAL)
		{
			return IO::FileExporter::SupportType::MultiFiles;
		}
		else if (ttype == Map::TileMap::TT_OSM)
		{
			NotNullPtr<Map::OSM::OSMTileMap> osm = NotNullPtr<Map::OSM::OSMTileMap>::ConvertFrom(tileMap);
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

Bool Exporter::OruxMapExporter::ExportFile(NotNullPtr<IO::SeekableStream> stm, Text::CStringNN fileName, NotNullPtr<IO::ParsedObject> pobj, Optional<ParamData> param)
{
	UTF8Char fileName2[512];
	UTF8Char u8fileName[512];
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	if (pobj->GetParserType() != IO::ParserType::MapLayer)
	{
		return false;
	}
	NotNullPtr<Map::MapDrawLayer> layer = NotNullPtr<Map::MapDrawLayer>::ConvertFrom(pobj);
	if (layer->GetObjectClass() != Map::MapDrawLayer::OC_TILE_MAP_LAYER)
	{
		return false;
	}
	NotNullPtr<Map::TileMap> tileMap = NotNullPtr<Map::TileMapLayer>::ConvertFrom(layer)->GetTileMap();
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
			NotNullPtr<IO::StreamData> fd;
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
			NotNullPtr<Map::OSM::OSMLocalTileMap> osm = NotNullPtr<Map::OSM::OSMLocalTileMap>::ConvertFrom(tileMap);
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
				if (osm->GetTileBounds(level, minX, minY, maxX, maxY))
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
					osm->GetTileImageIDs(level, Math::RectAreaDbl(Math::Coord2DDbl(minLon, minLat), Math::Coord2DDbl(maxLon, maxLat)), &imgIds);
					Optional<DB::DBTransaction> sess = db->BeginTransaction();
					NotNullPtr<DB::DBTransaction> thisSess;
					j = imgIds.GetCount();
					while (j-- > 0)
					{
						Math::Coord2D<Int32> tileId = imgIds.GetItem(j);
						if (osm->LoadTileImageData(level, tileId, bounds, true, it).SetTo(fd))
						{
							UOSInt imgSize = (UOSInt)fd->GetDataSize();
							Data::ByteBuffer imgBuff(imgSize);
							fd->GetRealData(0, imgSize, imgBuff);
							sql.Clear();
							sql.AppendCmdC(CSTR("insert into tiles (x, y, z, image) values ("));
							sql.AppendInt32(tileId.x - minX);
							sql.AppendCmdC(CSTR(", "));
							sql.AppendInt32(tileId.y - minY);
							sql.AppendCmdC(CSTR(", "));
							sql.AppendInt32((Int32)(UInt32)level);
							sql.AppendCmdC(CSTR(", "));
							sql.AppendBinary(imgBuff.Ptr(), imgSize);
							sql.AppendCmdC(CSTR(")"));
							db->ExecuteNonQuery(sql.ToCString());
							fd.Delete();
							if (j != 0 && (j & 0x3fff) == 0)
							{
								if (sess.SetTo(thisSess))
								{
									db->Commit(thisSess);
								}
								sess = db->BeginTransaction();
							}
						}
					}
					if (sess.SetTo(thisSess))
					{
						db->Commit(thisSess);
					}
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
		NotNullPtr<Map::OSM::OSMTileMap> osm = NotNullPtr<Map::OSM::OSMTileMap>::ConvertFrom(tileMap);
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
