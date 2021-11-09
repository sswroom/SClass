#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListStrUTF8.h"
#include "DB/ColDef.h"
#include "DB/DBReader.h"
#include "DB/SharedDBConn.h"
#include "DB/MDBFile.h"
#include "Map/ESRI/ESRIMDBLayer.h"
#include "Map/MapLayerCollection.h"
#include "Math/CoordinateSystemManager.h"
#include "Parser/FileParser/MDBParser.h"
#include "Text/Encoding.h"

Parser::FileParser::MDBParser::MDBParser()
{
	this->codePage = 0;
}

Parser::FileParser::MDBParser::~MDBParser()
{
}

Int32 Parser::FileParser::MDBParser::GetName()
{
	return *(Int32*)"MDBP";
}

void Parser::FileParser::MDBParser::SetCodePage(UInt32 codePage)
{
	this->codePage = codePage;
}

void Parser::FileParser::MDBParser::PrepareSelector(IO::IFileSelector *selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::ReadingDB || t == IO::ParserType::MapLayer)
	{
		selector->AddFilter((const UTF8Char*)"*.mdb", (const UTF8Char*)"MDB File");
	}
}

IO::ParserType Parser::FileParser::MDBParser::GetParserType()
{
	return IO::ParserType::ReadingDB;
}

IO::ParsedObject *Parser::FileParser::MDBParser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType)
{
	UInt8 buff[32];
	if (!fd->IsFullFile())
		return 0;
	if (fd->GetRealData(0, 32, buff) != 32)
	{
		return 0;
	}
	if (*(Int32*)&buff[0] != 0x100)
	{
		return 0;
	}
	if (Text::StrEquals(&buff[4], (const UTF8Char*)"Standard Jet DB"))
	{

	}
	else if (Text::StrEquals(&buff[4], (const UTF8Char*)"Standard ACE DB"))
	{

	}
	else
	{
		return 0;
	}
#ifndef _WIN32_WCE
	DB::MDBFile *mdb;
	NEW_CLASS(mdb, DB::MDBFile(fd->GetFullFileName(), 0, this->codePage, 0, 0));
	if (mdb->GetConnError() != DB::ODBCConn::CE_NONE)
	{
		DEL_CLASS(mdb);
		return 0;
	}

	Data::ArrayListStrUTF8 *tableNames;
	Data::ArrayListStrUTF8 *shpTables;
	DB::ColDef *colDef;
	NEW_CLASS(tableNames, Data::ArrayListStrUTF8());
	NEW_CLASS(shpTables, Data::ArrayListStrUTF8());
	NEW_CLASS(colDef, DB::ColDef((const UTF8Char*)""));
	UTF8Char sbuff[128];
	mdb->GetTableNames(tableNames);
	
	Bool hasSpRef = false;
	UOSInt i = tableNames->GetCount();
	while (i-- > 0)
	{
		const UTF8Char *tableName = tableNames->GetItem(i);
		DB::DBReader *rdr = mdb->GetTableData(tableName, 0, 0, 0, 0, 0);
		if (tableName && Text::StrEqualsICase(tableName, (const UTF8Char*)"GDB_SpatialRefs"))
		{
			hasSpRef = true;
		}

		if (rdr)
		{
			if (rdr->ColCount() >= 2)
			{
				OSInt objCol = -1;
				OSInt shapeCol = -1;
				UOSInt j;
				j = rdr->ColCount();
				while (j-- > 0)
				{
					rdr->GetColDef(j, colDef);
					if (Text::StrEqualsICase(colDef->GetColName(), (const UTF8Char*)"OBJECTID") && colDef->GetColType() == DB::DBUtil::CT_Int32)
					{
						objCol = (OSInt)j;
					}
					else if (Text::StrEqualsICase(colDef->GetColName(), (const UTF8Char*)"SHAPE") && (colDef->GetColType() == DB::DBUtil::CT_Binary || colDef->GetColType() == DB::DBUtil::CT_VarChar))
					{
						shapeCol = (OSInt)j;
					}
				}
				if (objCol != -1 && shapeCol != -1)
				{
					shpTables->Add(tableName);
				}
			}
			mdb->CloseReader(rdr);
		}
	}

	if (shpTables->GetCount())
	{
		Map::MapLayerCollection *lyrColl;
		Math::CoordinateSystem *csys = 0;
		DB::SharedDBConn *conn;
		Map::ESRI::ESRIMDBLayer *lyr;
		UInt32 srid = 0;

		if (hasSpRef)
		{
			DB::DBReader *rdr = mdb->GetTableData((const UTF8Char*)"GDB_SpatialRefs", 0, 0, 0, 0, 0);
			if (rdr)
			{
				if (rdr->ColCount() >= 2)
				{
					if (rdr->GetName(1, sbuff) && Text::StrEqualsICase(sbuff, (const UTF8Char*)"SRTEXT"))
					{
						if (rdr->ReadNext())
						{
							Text::StringBuilderUTF8 sb;
							rdr->GetStr(1, &sb);
							csys = Math::CoordinateSystemManager::ParsePRJBuff(fd->GetFullFileName(), (Char*)sb.ToString(), 0);
							if (csys)
							{
								srid = csys->GetSRID();
							}
						}
					}
				}
				mdb->CloseReader(rdr);
			}
		}
		NEW_CLASS(conn, DB::SharedDBConn(mdb));
		NEW_CLASS(lyrColl, Map::MapLayerCollection(fd->GetFullName(), 0));

		UOSInt i;
		i = shpTables->GetCount();
		while (i-- > 0)
		{
			NEW_CLASS(lyr, Map::ESRI::ESRIMDBLayer(conn, srid, fd->GetFullFileName(), shpTables->GetItem(i)));
			
			if (csys)
			{
				lyr->SetCoordinateSystem(csys->Clone());
			}

			lyrColl->Add(lyr);
		}
		SDEL_CLASS(csys);
		conn->UnuseObject();
		DEL_CLASS(colDef);
		DEL_CLASS(shpTables);
		DEL_CLASS(tableNames);
		if (lyrColl->GetCount() == 1)
		{
			Map::IMapDrawLayer *lyr = lyrColl->GetItem(0);
			lyrColl->RemoveAt(0);
			DEL_CLASS(lyrColl);
			return lyr;
		}
		else
		{
			return lyrColl;
		}
	}
	else
	{
		DEL_CLASS(colDef);
		DEL_CLASS(shpTables);
		DEL_CLASS(tableNames);

		return mdb;
	}
#else
	return 0;
#endif
}
