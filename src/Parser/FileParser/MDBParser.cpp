#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListStrUTF8.h"
#include "DB/ColDef.h"
#include "DB/DBReader.h"
#include "DB/SharedDBConn.h"
#include "DB/MDBFileConn.h"
#include "Map/ESRI/ESRIMDBLayer.h"
#include "Map/MapLayerCollection.h"
#include "Math/ArcGISPRJParser.h"
#include "Math/CoordinateSystemManager.h"
#include "Parser/FileParser/MDBParser.h"
#include "Text/Encoding.h"

Parser::FileParser::MDBParser::MDBParser()
{
	this->codePage = 0;
	this->log = 0;
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

void Parser::FileParser::MDBParser::SetArcGISPRJParser(Optional<Math::ArcGISPRJParser> prjParser)
{
	this->prjParser = prjParser;
}

void Parser::FileParser::MDBParser::SetLogTool(Optional<IO::LogTool> log)
{
	this->log = log;
}

void Parser::FileParser::MDBParser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::ReadingDB || t == IO::ParserType::MapLayer)
	{
		selector->AddFilter(CSTR("*.mdb"), CSTR("MDB File"));
	}
}

IO::ParserType Parser::FileParser::MDBParser::GetParserType()
{
	return IO::ParserType::ReadingDB;
}

Optional<IO::ParsedObject> Parser::FileParser::MDBParser::ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr)
{
	if (!fd->IsFullFile())
		return 0;
	if (ReadInt32(&hdr[0]) != 0x100)
	{
		return 0;
	}
	if (Text::StrEqualsC(&hdr[4], 15, UTF8STRC("Standard Jet DB")))
	{

	}
	else if (Text::StrEqualsC(&hdr[4], 15, UTF8STRC("Standard ACE DB")))
	{

	}
	else
	{
		return 0;
	}
#ifndef _WIN32_WCE
	NN<DB::MDBFileConn> mdb;
	NN<IO::LogTool> log;
	if (!this->log.SetTo(log))
	{
		return 0;
	}
	NEW_CLASSNN(mdb, DB::MDBFileConn(fd->GetFullFileName()->ToCString(), log, this->codePage, 0, 0));
	if (mdb->GetConnError() != DB::ODBCConn::CE_NONE)
	{
		mdb.Delete();
		return 0;
	}

	Data::ArrayListStringNN tableNames;
	Data::ArrayListStringNN shpTables;
	DB::ColDef colDef(CSTR(""));
	UTF8Char sbuff[128];
	UnsafeArray<UTF8Char> sptr;
	mdb->QueryTableNames(CSTR_NULL, tableNames);
	
	Bool hasSpRef = false;
	UOSInt i = tableNames.GetCount();
	while (i-- > 0)
	{
		NN<Text::String> tableName;
		NN<DB::DBReader> rdr;
		if (tableNames.GetItem(i).SetTo(tableName))
		{
			if (tableName->leng > 0 && tableName->EqualsICase(UTF8STRC("GDB_SpatialRefs")))
				hasSpRef = true;
			if (mdb->QueryTableData(CSTR_NULL, tableName->ToCString(), 0, 0, 0, CSTR_NULL, 0).SetTo(rdr))
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
						if (colDef.GetColName()->EqualsICase(UTF8STRC("OBJECTID")) && colDef.GetColType() == DB::DBUtil::CT_Int32)
						{
							objCol = (OSInt)j;
						}
						else if (colDef.GetColName()->EqualsICase(UTF8STRC("SHAPE")) && (colDef.GetColType() == DB::DBUtil::CT_Binary || colDef.GetColType() == DB::DBUtil::CT_VarUTF8Char))
						{
							shapeCol = (OSInt)j;
						}
					}
					if (objCol != -1 && shapeCol != -1)
					{
						shpTables.Add(tableName);
					}
				}
				mdb->CloseReader(rdr);
			}
		}
	}

	if (shpTables.GetCount())
	{
		Map::MapLayerCollection *lyrColl;
		Optional<Math::CoordinateSystem> csys = 0;
		NN<Math::CoordinateSystem> nncsys;
		DB::SharedDBConn *conn;
		NN<Map::ESRI::ESRIMDBLayer> lyr;
		NN<Math::ArcGISPRJParser> prjParser;
		UInt32 srid = 0;

		if (hasSpRef && this->prjParser.SetTo(prjParser))
		{
			NN<DB::DBReader> rdr;
			if (mdb->QueryTableData(CSTR_NULL, CSTR("GDB_SpatialRefs"), 0, 0, 0, CSTR_NULL, 0).SetTo(rdr))
			{
				if (rdr->ColCount() >= 2)
				{
					if (rdr->GetName(1, sbuff).SetTo(sptr) && Text::StrEqualsICaseC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("SRTEXT")))
					{
						if (rdr->ReadNext())
						{
							Text::StringBuilderUTF8 sb;
							rdr->GetStr(1, sb);
							if (prjParser->ParsePRJBuff(fd->GetFullFileName()->ToCString(), sb.v, sb.GetLength(), 0).SetTo(nncsys))
							{
								srid = nncsys->GetSRID();
								csys = nncsys;
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
		i = shpTables.GetCount();
		while (i-- > 0)
		{
			NEW_CLASSNN(lyr, Map::ESRI::ESRIMDBLayer(conn, srid, fd->GetFullFileName(), Text::String::OrEmpty(shpTables.GetItem(i))->ToCString()));
			
			if (csys.SetTo(nncsys))
			{
				lyr->SetCoordinateSystem(nncsys->Clone());
			}

			lyrColl->Add(lyr);
		}
		csys.Delete();
		conn->UnuseObject();
		tableNames.FreeAll();
		NN<Map::MapDrawLayer> lyr1;
		if (lyrColl->GetCount() == 1 && lyrColl->GetItem(0).SetTo(lyr1))
		{
			lyrColl->RemoveAt(0);
			DEL_CLASS(lyrColl);
			return lyr1;
		}
		else
		{
			return lyrColl;
		}
	}
	else
	{
		tableNames.FreeAll();
		return mdb;
	}
#else
	return 0;
#endif
}
