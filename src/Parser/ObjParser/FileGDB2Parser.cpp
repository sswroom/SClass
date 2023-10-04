#include "Stdafx.h"
#include "MyMemory.h"
#include "DB/SharedReadingDB.h"
#include "IO/PackageFile.h"
#include "IO/Path.h"
#include "Map/FileGDBLayer.h"
#include "Map/MapLayerCollection.h"
#include "Map/ESRI/FileGDBDir.h"
#include "Parser/ObjParser/FileGDB2Parser.h"

Parser::ObjParser::FileGDB2Parser::FileGDB2Parser()
{
}

Parser::ObjParser::FileGDB2Parser::~FileGDB2Parser()
{
}

Int32 Parser::ObjParser::FileGDB2Parser::GetName()
{
	return *(Int32*)"FGDB";
}

void Parser::ObjParser::FileGDB2Parser::SetArcGISPRJParser(Math::ArcGISPRJParser *prjParser)
{
	this->prjParser = prjParser;
}

void Parser::ObjParser::FileGDB2Parser::PrepareSelector(NotNullPtr<IO::FileSelector> selector, IO::ParserType t)
{
	if (t == IO::ParserType::ReadingDB || t == IO::ParserType::Unknown)
	{
		selector->AddFilter(CSTR("*.gdb"), CSTR("File Geodatabase File"));
	}
}

IO::ParserType Parser::ObjParser::FileGDB2Parser::GetParserType()
{
	return IO::ParserType::ReadingDB;
}

IO::ParsedObject *Parser::ObjParser::FileGDB2Parser::ParseObject(IO::ParsedObject *pobj, IO::PackageFile *pkgFile, IO::ParserType targetType)
{
	if (pobj->GetParserType() != IO::ParserType::PackageFile)
		return 0;
	IO::PackageFile *relObj = 0;
	IO::PackageFile *pkg = (IO::PackageFile*)pobj;
	if (pkg->GetCount() == 1 && pkg->GetItemType(0) == IO::PackageFile::PackObjectType::PackageFileType)
	{
		relObj = pkg->GetItemPackNew(0);
		pkg = relObj;
	}
	UOSInt index = pkg->GetItemIndex(CSTR("a00000001.gdbtable"));
	if (index == INVALID_INDEX)
	{
		SDEL_CLASS(relObj);
		return 0;
	}
	NotNullPtr<Map::ESRI::FileGDBDir> fgdb;
	if (!fgdb.Set(Map::ESRI::FileGDBDir::OpenDir(pkg, this->prjParser)))
	{
		SDEL_CLASS(relObj);
		return 0;
	}
	if (targetType == IO::ParserType::MapLayer || targetType == IO::ParserType::Unknown)
	{
		NotNullPtr<DB::DBReader> r;
		if (r.Set(fgdb->QueryTableData(CSTR_NULL, CSTR("GDB_Items"), 0, 0, 0, CSTR_NULL, 0)))
		{
			Data::ArrayList<Text::String*> layers;
			Text::String *layerName;
			while (r->ReadNext())
			{
				if (!r->IsNull(6) && !r->IsNull(7))
				{
					layerName = r->GetNewStr(3); //or 4?
					if (layerName)
					{
						layers.Add(layerName);
					}
				}
			}
			fgdb->CloseReader(r);
			if (layers.GetCount() > 0)
			{
				UTF8Char sbuff[512];
				UTF8Char *sptr;
				Map::MapLayerCollection *layerColl;
				NotNullPtr<Map::FileGDBLayer> layer;
				DB::SharedReadingDB *db;
				UOSInt i;
				UOSInt j;
				NEW_CLASS(db, DB::SharedReadingDB(fgdb));
				sptr = pobj->GetSourceNameObj()->ConcatTo(sbuff);
				i = Text::StrLastIndexOfC(sbuff, (UOSInt)(sptr - sbuff), IO::Path::PATH_SEPERATOR);
				NEW_CLASS(layerColl, Map::MapLayerCollection(CSTRP(sbuff, sptr), CSTRP(&sbuff[i + 1], sptr)));
				i = 0;
				j = layers.GetCount();
				while (i < j)
				{
					layerName = layers.GetItem(i);
					NEW_CLASSNN(layer, Map::FileGDBLayer(db, layerName->ToCString(), layerName->ToCString(), this->prjParser));
					layerColl->Add(layer);
					layerName->Release();
					i++;
				}
				db->UnuseObject();
				SDEL_CLASS(relObj);
				return layerColl;
			}
		}
	}
	SDEL_CLASS(relObj);
	return fgdb.Ptr();
}
