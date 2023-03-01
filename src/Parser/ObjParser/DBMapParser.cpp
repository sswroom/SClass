#include "Stdafx.h"
#include "MyMemory.h"
#include "DB/ReadingDB.h"
#include "Map/GPSTrack.h"
#include "Map/MapLayerCollection.h"
#include "Parser/ObjParser/DBMapParser.h"

Parser::ObjParser::DBMapParser::DBMapParser()
{
}

Parser::ObjParser::DBMapParser::~DBMapParser()
{
}

Int32 Parser::ObjParser::DBMapParser::GetName()
{
	return *(Int32*)"DBMA";
}

void Parser::ObjParser::DBMapParser::PrepareSelector(IO::FileSelector *selector, IO::ParserType t)
{
	if (t == IO::ParserType::MapLayer)
	{
		selector->AddFilter(CSTR("*.mdb"), CSTR("MDB map layer"));
	}
}

IO::ParserType Parser::ObjParser::DBMapParser::GetParserType()
{
	return IO::ParserType::MapLayer;
}

IO::ParsedObject *Parser::ObjParser::DBMapParser::ParseObject(IO::ParsedObject *pobj, IO::PackageFile *pkgFile, IO::ParserType targetType)
{
//	DB::ReadingDB *db;
//	DB::DBReader *r;
//	WChar wbuff[512];
	Bool valid = false;
	if (pobj->GetParserType() != IO::ParserType::ReadingDB)
		return 0;
	///////////////////////////
/*	Map::MapLayerCollection *layerColl;
	Data::ArrayList<Text::String *> tableNames;
	OSInt i;
	OSInt j;
	OSInt k;
	OSInt xCol;
	OSInt yCol;
	DB::SharedDBConn *sdb;

	//NEW_CLASS(sdb, DB::SharedDBConn(db
	
	NEW_CLASS(layerColl, Map::MapLayerCollection(pobj->GetSourceNameObj()));
	db = (DB::ReadingDB*)pobj;
	db->QueryTableNames(CSTR_NULL, &tableNames);
	i = 0;
	j = tableNames.GetCount();
	while (i < j)
	{
		r = db->QueryTableData(tableNames.GetItem(i)->ToCString());
		if (r)
		{
			xCol = -1;
			yCol = -1;
			k = r->ColCount();
			while (k-- > 0)
			{
				if (r->GetName(k, wbuff))
				{
					if (Text::StrCompareICase(wbuff, L"EASTING") == 0)
					{
						xCol = k;
					}
					if (Text::StrCompareICase(wbuff, L"NORTHING") == 0)
					{
						yCol = k;
					}
				}
			}

			if (xCol != -1 && yCol != -1)
			{
				////////////////////////////////
			}
			db->CloseReader(r);
		}
	}
	LIST_FREE_STRING(&tableNames);

	if (layerColl->GetCount() > 0)
	{
		return layerColl;
	}
	DEL_CLASS(layerColl);*/
	return 0;
}
