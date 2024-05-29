#include "Stdafx.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "Map/TileMapOruxWriter.h"
#include "Map/OSM/OSMTileMap.h"
#include "Sync/MutexUsage.h"

Map::TileMapOruxWriter::TileMapOruxWriter(Text::CStringNN fileName, UOSInt minLev, UOSInt maxLev, Math::RectAreaDbl bounds)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	this->minLev = minLev;
	this->maxLev = maxLev;
	this->bounds = bounds;
	this->levels = MemAlloc(LevelInfo, maxLev - minLev + 1);
	sptr = fileName.ConcatTo(sbuff);
	UOSInt i = Text::StrLastIndexOfCharC(sbuff, (UOSInt)(sptr - sbuff), IO::Path::PATH_SEPERATOR);
	sptr = Text::StrConcatC(&sbuff[i + 1], UTF8STRC("OruxMapsImages.db"));
	NEW_CLASSNN(this->db, DB::SQLiteFile(CSTRP(sbuff, sptr)));
	this->sess = 0;
	if (!this->db->IsError())
	{
		DB::SQLBuilder sql(db->GetSQLType(), db->IsAxisAware(), db->GetTzQhr());
		db->ExecuteNonQuery(CSTR("CREATE TABLE android_metadata (locale TEXT)"));
		db->ExecuteNonQuery(CSTR("CREATE TABLE tiles (x int, y int, z int, image blob, PRIMARY KEY (x,y,z))"));
		db->ExecuteNonQuery(CSTR("delete from android_metadata"));
		db->ExecuteNonQuery(CSTR("delete from tiles"));
		sql.Clear();
		sql.AppendCmdC(CSTR("insert into android_metadata (locale) values ("));
		sql.AppendStrUTF8((const UTF8Char*)"zh_TW");//loc->shortName);
		sql.AppendCmdC(CSTR(")"));
		db->ExecuteNonQuery(sql.ToCString());

		sptr = fileName.Substring(i + 1).ConcatTo(sbuff);
		i = Text::StrIndexOfCharC(sbuff, (UOSInt)(sptr - sbuff), '.');
		if (i >= 0)
		{
			sptr = &sbuff[i];
			*sptr = 0;
		}

		IO::FileStream fs(fileName, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		Text::StringBuilderUTF8 sbXML;
		sbXML.AppendC(UTF8STRC("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"));
		sbXML.AppendC(UTF8STRC("<OruxTracker xmlns=\"http://oruxtracker.com/app/res/calibration\"\n"));
		sbXML.AppendC(UTF8STRC(" versionCode=\"3.0\">\n"));
		sbXML.AppendC(UTF8STRC("<MapCalibration layers=\"true\" layerLevel=\"0\">\n"));
		sbXML.AppendC(UTF8STRC("<MapName><![CDATA["));
		sbXML.AppendP(sbuff, sptr);
		sbXML.AppendC(UTF8STRC("]]></MapName>\n"));
		UOSInt level = minLev;
		while (level <= maxLev)
		{
			Int32 minX = Map::OSM::OSMTileMap::Lon2TileX(bounds.min.x, level);
			Int32 minY = Map::OSM::OSMTileMap::Lat2TileY(bounds.max.y, level);
			Int32 maxX = Map::OSM::OSMTileMap::Lon2TileX(bounds.max.x, level);
			Int32 maxY = Map::OSM::OSMTileMap::Lat2TileY(bounds.min.y, level);
			Double minLon = Map::OSM::OSMTileMap::TileX2Lon(minX, level);
			Double maxLon = Map::OSM::OSMTileMap::TileX2Lon(maxX + 1, level);
			Double minLat = Map::OSM::OSMTileMap::TileY2Lat(maxY + 1, level);
			Double maxLat = Map::OSM::OSMTileMap::TileY2Lat(minY, level);
			this->levels[level - minLev].minX = minX;
			this->levels[level - minLev].minY = minY;

			sbXML.AppendC(UTF8STRC("<OruxTracker  versionCode=\"2.1\">\n"));

			sbXML.AppendC(UTF8STRC("<MapCalibration layers=\"false\" layerLevel=\""));
			sbXML.AppendUOSInt(level);
			sbXML.AppendC(UTF8STRC("\">\n"));

			sbXML.AppendC(UTF8STRC("<MapName><![CDATA["));
			sbXML.AppendP(sbuff, sptr);
			sbXML.AppendC(UTF8STRC("]]></MapName>\n"));

			sbXML.AppendC(UTF8STRC("<MapChunks xMax=\""));
			sbXML.AppendI32(maxX - minX + 1);
			sbXML.AppendC(UTF8STRC("\" yMax=\""));
			sbXML.AppendI32(maxY - minY + 1);
			sbXML.AppendC(UTF8STRC("\" datum=\"WGS84\" projection=\"Mercator\" img_height=\"256\" img_width=\"256\" file_name=\""));
			sbXML.AppendP(sbuff, sptr);
			sbXML.AppendC(UTF8STRC("\" />\n"));

			sbXML.AppendC(UTF8STRC("<MapDimensions height=\""));
			sbXML.AppendI32((maxY - minY + 1) * 256);
			sbXML.AppendC(UTF8STRC("\" width=\""));
			sbXML.AppendI32((maxX - minX + 1) * 256);
			sbXML.AppendC(UTF8STRC("\" />\n"));

			sbXML.AppendC(UTF8STRC("<MapBounds minLat=\""));
			sbXML.AppendDouble(minLat);
			sbXML.AppendC(UTF8STRC("\" maxLat=\""));
			sbXML.AppendDouble(maxLat);
			sbXML.AppendC(UTF8STRC("\" minLon=\""));
			sbXML.AppendDouble(minLon);
			sbXML.AppendC(UTF8STRC("\" maxLon=\""));
			sbXML.AppendDouble(maxLon);
			sbXML.AppendC(UTF8STRC("\" />\n"));

			sbXML.AppendC(UTF8STRC("<CalibrationPoints>\n"));

			sbXML.AppendC(UTF8STRC("<CalibrationPoint corner=\"TL\" lon=\""));
			sbXML.AppendDouble(minLon);
			sbXML.AppendC(UTF8STRC("\" lat=\""));
			sbXML.AppendDouble(maxLat);
			sbXML.AppendC(UTF8STRC("\" />\n"));

			sbXML.AppendC(UTF8STRC("<CalibrationPoint corner=\"BR\" lon=\""));
			sbXML.AppendDouble(maxLon);
			sbXML.AppendC(UTF8STRC("\" lat=\""));
			sbXML.AppendDouble(minLat);
			sbXML.AppendC(UTF8STRC("\" />\n"));

			sbXML.AppendC(UTF8STRC("<CalibrationPoint corner=\"TR\" lon=\""));
			sbXML.AppendDouble(maxLon);
			sbXML.AppendC(UTF8STRC("\" lat=\""));
			sbXML.AppendDouble(maxLat);
			sbXML.AppendC(UTF8STRC("\" />\n"));

			sbXML.AppendC(UTF8STRC("<CalibrationPoint corner=\"BL\" lon=\""));
			sbXML.AppendDouble(minLon);
			sbXML.AppendC(UTF8STRC("\" lat=\""));
			sbXML.AppendDouble(minLat);
			sbXML.AppendC(UTF8STRC("\" />\n"));

			sbXML.AppendC(UTF8STRC("</CalibrationPoints>\n"));
			sbXML.AppendC(UTF8STRC("</MapCalibration>\n"));
			sbXML.AppendC(UTF8STRC("</OruxTracker>\n"));

			level++;
		}
		sbXML.AppendC(UTF8STRC("</MapCalibration>\n"));
		sbXML.AppendC(UTF8STRC("</OruxTracker>\n"));
		fs.WriteCont(sbXML.v, sbXML.leng);
		this->sess = this->db->BeginTransaction();
	}
}

Map::TileMapOruxWriter::~TileMapOruxWriter()
{
	NN<DB::DBTransaction> sess;
	if (this->sess.SetTo(sess))
	{
		this->db->Commit(sess);
		this->sess = 0;
	}
	this->db.Delete();
	MemFree(this->levels);
}

void Map::TileMapOruxWriter::BeginLevel(UOSInt level)
{
}

void Map::TileMapOruxWriter::AddX(Int32 x)
{
}

void Map::TileMapOruxWriter::AddImage(UOSInt level, Int32 x, Int32 y, Data::ByteArrayR imgData, Map::TileMap::ImageType imgType)
{
	DB::SQLBuilder sql(this->db->GetSQLType(), this->db->IsAxisAware(), this->db->GetTzQhr());
	sql.AppendCmdC(CSTR("insert into tiles (x, y, z, image) values ("));
	sql.AppendInt32(x - this->levels[level - minLev].minX);
	sql.AppendCmdC(CSTR(", "));
	sql.AppendInt32(y - this->levels[level - minLev].minY);
	sql.AppendCmdC(CSTR(", "));
	sql.AppendInt32((Int32)(UInt32)level);
	sql.AppendCmdC(CSTR(", "));
	sql.AppendBinary(imgData.Arr(), imgData.GetSize());
	sql.AppendCmdC(CSTR(")"));
	Sync::MutexUsage mutUsage(this->mut);
	this->db->ExecuteNonQuery(sql.ToCString());
	this->imgCount++;
	if (this->imgCount > 0x3fff)
	{
		NN<DB::DBTransaction> sess;
		if (this->sess.SetTo(sess))
		{
			this->db->Commit(sess);
		}
		this->sess = this->db->BeginTransaction();
		this->imgCount = 0;
	}
}
