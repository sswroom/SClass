#include "Stdafx.h"
#include "MyMemory.h"
#include "DB/ColDef.h"
#include "DB/SharedReadingDB.h"
#include "IO/PackageFile.h"
#include "Map/FileGDB.h"
#include "Map/FileGDBLayer.h"
#include "Map/MapLayerCollection.h"
#include "Math/CoordinateSystemManager.h"
#include "Parser/ParserList.h"
#include "Parser/ObjParser/FileGDBParser.h"

Parser::ObjParser::FileGDBParser::FileGDBParser()
{
}

Parser::ObjParser::FileGDBParser::~FileGDBParser()
{
}

Int32 Parser::ObjParser::FileGDBParser::GetName()
{
	return *(Int32*)"FGDB";
}

void Parser::ObjParser::FileGDBParser::PrepareSelector(IO::IFileSelector *selector, IO::ParserType t)
{
	if (t == IO::ParserType::ReadingDB)
	{
		selector->AddFilter((const UTF8Char*)"*.gdb", (const UTF8Char*)"File Geodatabase File");
	}
}

IO::ParserType Parser::ObjParser::FileGDBParser::GetParserType()
{
	return IO::ParserType::ReadingDB;
}

IO::ParsedObject *Parser::ObjParser::FileGDBParser::ParseObject(IO::ParsedObject *pobj, IO::PackageFile *pkgFile, IO::ParserType targetType)
{
	if (pobj->GetParserType() != IO::ParserType::PackageFile)
		return 0;
	IO::PackageFile *pkg = (IO::PackageFile*)pobj;
	if (!Text::StrEndsWithICase(pkg->GetSourceNameObj(), (const UTF8Char*)".GDB"))
	{
		return 0;
	}
#if defined(CPU_X86_64)
#if !defined(_MSC_VER) || ((_MSC_VER / 10) == 191)
	Map::FileGDB *fileGDB;
	NEW_CLASS(fileGDB, Map::FileGDB(pkg->GetSourceNameObj()));
	if (!fileGDB->IsError())
	{
		Data::ArrayListStrUTF8 *tableNames;
		Data::ArrayListStrUTF8 *shpTables;
		DB::ColDef *colDef;
		NEW_CLASS(tableNames, Data::ArrayListStrUTF8());
		NEW_CLASS(shpTables, Data::ArrayListStrUTF8());
		NEW_CLASS(colDef, DB::ColDef((const UTF8Char*)""));
		fileGDB->GetTableNames(tableNames);

		UOSInt i = tableNames->GetCount();
		while (i-- > 0)
		{
			const UTF8Char *tableName = tableNames->GetItem(i);
			DB::DBReader *rdr = fileGDB->GetTableData(tableName, 0, 0, 0);
			if (rdr)
			{
				if (rdr->ColCount() >= 2)
				{
					UOSInt objCol = (UOSInt)-1;
					UOSInt shapeCol = (UOSInt)-1;
					UOSInt j;
					j = rdr->ColCount();
					while (j-- > 0)
					{
						rdr->GetColDef(j, colDef);
						if (colDef->IsPK() && colDef->GetColType() == DB::DBUtil::CT_Int32)
						{
							objCol = j;
						}
						else if (colDef->GetColType() == DB::DBUtil::CT_Vector)
						{
							shapeCol = j;
						}
					}
					if (objCol != (UOSInt)-1 && shapeCol != (UOSInt)-1)
					{
						shpTables->Add(tableName);
					}
				}
				fileGDB->CloseReader(rdr);
			}
		}

		if (shpTables->GetCount())
		{
			Map::MapLayerCollection *lyrColl;
			Math::CoordinateSystem *csys = 0;
			DB::SharedReadingDB *conn;
			Map::FileGDBLayer *lyr;

			NEW_CLASS(conn, DB::SharedReadingDB(fileGDB));
			NEW_CLASS(lyrColl, Map::MapLayerCollection(pkg->GetSourceNameObj(), 0));

			UOSInt i;
			i = shpTables->GetCount();
			while (i-- > 0)
			{
				NEW_CLASS(lyr, Map::FileGDBLayer(conn, pkg->GetSourceNameObj(), shpTables->GetItem(i)));

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

			return fileGDB;
		}

		return fileGDB;
	}
	DEL_CLASS(fileGDB);
#endif
#endif
	return 0;
}
