#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/FastMap.h"
#include "DB/ReadingDB.h"
#include "Map/GPSTrack.h"
#include "Parser/ObjParser/DBITParser.h"

Parser::ObjParser::DBITParser::DBITParser()
{
}

Parser::ObjParser::DBITParser::~DBITParser()
{
}

Int32 Parser::ObjParser::DBITParser::GetName()
{
	return *(Int32*)"DBIT";
}

void Parser::ObjParser::DBITParser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
{
	if (t == IO::ParserType::MapLayer)
	{
		selector->AddFilter(CSTR("*.itm"), CSTR("Taiwan GPS Project File"));
	}
}

IO::ParserType Parser::ObjParser::DBITParser::GetParserType()
{
	return IO::ParserType::MapLayer;
}

IO::ParsedObject *Parser::ObjParser::DBITParser::ParseObject(NN<IO::ParsedObject> pobj, IO::PackageFile *pkgFile, IO::ParserType targetType)
{
	NN<DB::ReadingDB> db;
	NN<DB::DBReader> r;
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	Bool valid = false;
	if (pobj->GetParserType() != IO::ParserType::ReadingDB)
		return 0;
	db = NN<DB::ReadingDB>::ConvertFrom(pobj);
	if (db->QueryTableData(CSTR_NULL, CSTR("IT_TGVLib"), 0, 0, 0, CSTR_NULL, 0).SetTo(r))
	{
		valid = true;
		sptr = r->GetName(0, sbuff);
		if (sptr == 0 || !Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("fUserType")))
			valid = false;
		sptr = r->GetName(1, sbuff);
		if (sptr == 0 || !Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("fDBType")))
			valid = false;
		sptr = r->GetName(2, sbuff);
		if (sptr == 0 || !Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("fObjNo")))
			valid = false;
		sptr = r->GetName(3, sbuff);
		if (sptr == 0 || !Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("fObjFreeNo")))
			valid = false;
		sptr = r->GetName(4, sbuff);
		if (sptr == 0 || !Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("fTypeName")))
			valid = false;
		db->CloseReader(r);
	}
	if (!valid)
		return 0;

	Map::GPSTrack *trk = 0;
	Record *rec;
	Data::FastMap<Int32, Record*> gpsLogMap;
	Data::FastMap<Int32, Record*> wpMap;
	Data::DateTime dt;
	UOSInt i;
	UOSInt j;
	if (db->QueryTableData(CSTR_NULL, CSTR("GPSLog"), 0, 0, 0, CSTR_NULL, 0).SetTo(r))
	{
		Int32 id;
		Int32 times[2];
		Double speed;
		Int64 t;

		while (r->ReadNext())
		{
			id = r->GetInt32(0);
			times[0] = r->GetInt32(3);
			times[1] = r->GetInt32(2);
			speed = r->GetDbl(4) / 1.852;
			t = *(Int64*)times / 10000000LL - 11644473600LL;
			rec = gpsLogMap.Get(id);
			if (rec)
			{
				valid = false;
				break;
			}
			else
			{
				rec = MemAlloc(Record, 1);
				rec->wpId = 0;
				rec->time = t;
				rec->speed = speed;
				rec->gpsLogId = id;
				rec->latitude = 0;
				rec->longitude = 0;
				rec->altitude = 0;
				gpsLogMap.Put(rec->gpsLogId, rec);
			}
		}
		db->CloseReader(r);
	}
	else
	{
		valid = false;
	}
	
	if (db->QueryTableData(CSTR_NULL, CSTR("WP"), 0, 0, 0, CSTR_NULL, 0).SetTo(r))
	{
		Int32 id;
		Int32 gpsLogId;
		Double lat;
		Double lon;
		Double alt;
		while (r->ReadNext())
		{
			id = r->GetInt32(0);
			gpsLogId = r->GetInt32(4);
			lon = r->GetDbl(8);
			lat = r->GetDbl(9);
			alt = r->GetDbl(10);
			rec = gpsLogMap.Get(gpsLogId);
			if (rec == 0)
			{
				valid = false;
				break;
			}
			else if (rec->wpId != 0)
			{
				valid = false;
				break;
			}
			rec->wpId = id;
			rec->latitude = lat;
			rec->longitude = lon;
			rec->altitude = alt;
			wpMap.Put(rec->wpId, rec);
		}
		db->CloseReader(r);
	}
	else
	{
		valid = false;
	}

	if (valid)
	{
		if (db->QueryTableData(CSTR_NULL, CSTR("Line"), 0, 0, 0, CSTR_NULL, 0).SetTo(r))
		{
			Map::GPSTrack::GPSRecord3 gpsRec;
			NEW_CLASS(trk, Map::GPSTrack(pobj->GetSourceNameObj(), true, 0, 0));
			while (r->ReadNext())
			{
				sptr = r->GetStr(2, sbuff, sizeof(sbuff));
				trk->SetTrackName(CSTRP(sbuff, sptr));
				i = (UOSInt)r->GetInt32(9);
				j = (UOSInt)r->GetInt32(10);
				while (i <= j)
				{
					rec = wpMap.Get((Int32)i);
					if (rec)
					{
						gpsRec.recTime = Data::TimeInstant(rec->time, 0);
						gpsRec.pos = Math::Coord2DDbl(rec->longitude, rec->latitude);
						gpsRec.altitude = rec->altitude;
						gpsRec.speed = rec->speed;
						gpsRec.heading = 0;
						gpsRec.valid = true;
						gpsRec.nSateUsed = 0;
						gpsRec.nSateUsedGPS = 0;
						gpsRec.nSateUsedGLO = 0;
						gpsRec.nSateUsedSBAS = 0;
						gpsRec.nSateViewGPS = 0;
						gpsRec.nSateViewGLO = 0;
						gpsRec.nSateViewGA = 0;
						gpsRec.nSateViewQZSS = 0;
						gpsRec.nSateViewBD = 0;
						trk->AddRecord(gpsRec);
					}
					i++;
				}
				trk->NewTrack();
			}
			db->CloseReader(r);
		}
	}

	i = gpsLogMap.GetCount();
	while (i-- > 0)
	{
		MemFree(gpsLogMap.GetItem(i));
	}
	return trk;
}
