#include "Stdafx.h"
#include "MyMemory.h"
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

void Parser::ObjParser::DBITParser::PrepareSelector(IO::IFileSelector *selector, IO::ParserType t)
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

IO::ParsedObject *Parser::ObjParser::DBITParser::ParseObject(IO::ParsedObject *pobj, IO::PackageFile *pkgFile, IO::ParserType targetType)
{
	DB::ReadingDB *db;
	DB::DBReader *r;
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	Bool valid = false;
	if (pobj->GetParserType() != IO::ParserType::ReadingDB)
		return 0;
	db = (DB::ReadingDB*)pobj;
	r = db->GetTableData((const UTF8Char*)"IT_TGVLib", 0, 0, 0, CSTR_NULL, 0);
	if (r)
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
	Data::Int32Map<Record*> *gpsLogMap;
	Data::Int32Map<Record*> *wpMap;
	Data::ArrayList<Record*> *recList;
	Data::DateTime *dt;
	UOSInt i;
	UOSInt j;
	NEW_CLASS(dt, Data::DateTime());
	NEW_CLASS(gpsLogMap, Data::Int32Map<Record*>());
	NEW_CLASS(wpMap, Data::Int32Map<Record*>());
	r = db->GetTableData((const UTF8Char*)"GPSLog", 0, 0, 0, CSTR_NULL, 0);
	if (r)
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
			rec = gpsLogMap->Get(id);
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
				gpsLogMap->Put(rec->gpsLogId, rec);
			}
		}
		db->CloseReader(r);
	}
	else
	{
		valid = false;
	}
	
	r = db->GetTableData((const UTF8Char*)"WP", 0, 0, 0, CSTR_NULL, 0);
	if (r)
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
			rec = gpsLogMap->Get(gpsLogId);
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
			wpMap->Put(rec->wpId, rec);
		}
		db->CloseReader(r);
	}
	else
	{
		valid = false;
	}

	if (valid)
	{
		r = db->GetTableData((const UTF8Char*)"Line", 0, 0, 0, CSTR_NULL, 0);
		if (r)
		{
			Map::GPSTrack::GPSRecord2 gpsRec;
			NEW_CLASS(trk, Map::GPSTrack(pobj->GetSourceNameObj(), true, 0, 0));
			while (r->ReadNext())
			{
				sptr = r->GetStr(2, sbuff, sizeof(sbuff));
				trk->SetTrackName(CSTRP(sbuff, sptr));
				i = (UOSInt)r->GetInt32(9);
				j = (UOSInt)r->GetInt32(10);
				while (i <= j)
				{
					rec = wpMap->Get((Int32)i);
					if (rec)
					{
						gpsRec.utcTimeTicks = rec->time * 1000LL;
						gpsRec.lat = rec->latitude;
						gpsRec.lon = rec->longitude;
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
						trk->AddRecord(&gpsRec);
					}
					i++;
				}
				trk->NewTrack();
			}
			db->CloseReader(r);
		}
	}

	recList = gpsLogMap->GetValues();
	i = recList->GetCount();
	while (i-- > 0)
	{
		MemFree(recList->GetItem(i));
	}
	DEL_CLASS(wpMap);
	DEL_CLASS(gpsLogMap);
	DEL_CLASS(dt);
	return trk;
}
