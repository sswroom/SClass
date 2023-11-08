#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/PackageFile.h"
#include "Map/MapLayerCollection.h"
#include "Parser/ParserList.h"
#include "Parser/ObjParser/KMZParser.h"

Parser::ObjParser::KMZParser::KMZParser()
{
	this->parsers = 0;
}

Parser::ObjParser::KMZParser::~KMZParser()
{
}

Int32 Parser::ObjParser::KMZParser::GetName()
{
	return *(Int32*)"KMZP";
}

void Parser::ObjParser::KMZParser::SetParserList(Parser::ParserList *parsers)
{
	this->parsers = parsers;
}

void Parser::ObjParser::KMZParser::PrepareSelector(NotNullPtr<IO::FileSelector> selector, IO::ParserType t)
{
	if (t == IO::ParserType::MapLayer)
	{
		selector->AddFilter(CSTR("*.kmz"), CSTR("Zipped KML File"));
	}
}

IO::ParserType Parser::ObjParser::KMZParser::GetParserType()
{
	return IO::ParserType::MapLayer;
}

IO::ParsedObject *Parser::ObjParser::KMZParser::ParseObject(NotNullPtr<IO::ParsedObject> pobj, IO::PackageFile *pkgFile, IO::ParserType targetType)
{
	if (pobj->GetParserType() != IO::ParserType::PackageFile)
		return 0;
	if (this->parsers == 0)
		return 0;
	NotNullPtr<IO::PackageFile> pkg = NotNullPtr<IO::PackageFile>::ConvertFrom(pobj);
	IO::ParserType pt;
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	Data::ArrayList<IO::ParsedObject*> *pobjList;
	IO::ParsedObject *pobj2;
	NEW_CLASS(pobjList, Data::ArrayList<IO::ParsedObject*>());
	UOSInt i;
	UOSInt j;
	i = 0;
	j = pkg->GetCount();
	while (i < j)
	{
		sptr = pkg->GetItemName(sbuff, i);
		if (Text::StrEndsWithC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC(".kml")))
		{
			NotNullPtr<IO::StreamData> fd;
			if (fd.Set(pkg->GetItemStmDataNew(i)))
			{
				pobj2 = this->parsers->ParseFile(fd, &pt);
				fd.Delete();
				if (pobj2)
				{
					if (pobj->GetSourceNameObj()->EndsWithICase(UTF8STRC(".kmz")))
					{
						pobj2->SetSourceName(pobj->GetSourceNameObj());
					}
					if (pt == IO::ParserType::MapLayer)
					{
						pobjList->Add(pobj2);
					}
					else
					{
						DEL_CLASS(pobj2);
					}
				}
			}
		}
		i++;
	}
	if (pobjList->GetCount() == 0)
	{
		DEL_CLASS(pobjList);
		return 0;
	}
	else if (pobjList->GetCount() == 1)
	{
		pobj2 = pobjList->GetItem(0);
		DEL_CLASS(pobjList);
		return pobj2;
	}
	else
	{
		Map::MapLayerCollection *mapLyrColl;
		NotNullPtr<IO::ParsedObject> nnpobj2;
		NotNullPtr<Map::MapDrawLayer> layer;
		NEW_CLASS(mapLyrColl, Map::MapLayerCollection(pobj->GetSourceNameObj(), 0));
		i = 0;
		j = pobjList->GetCount();
		while (i < j)
		{
			if (nnpobj2.Set(pobjList->GetItem(i)))
			{
				if (nnpobj2->GetParserType() == IO::ParserType::MapLayer)
				{
					mapLyrColl->Add(NotNullPtr<Map::MapDrawLayer>::ConvertFrom(nnpobj2));
				}
				else
				{
					Map::MapLayerCollection *lyrColl = (Map::MapLayerCollection*)nnpobj2.Ptr();
					while (lyrColl->GetCount() > 0)
					{
						if (layer.Set(lyrColl->RemoveAt(0)))
							mapLyrColl->Add(layer);
					}
					nnpobj2.Delete();
				}
			}
			i++;
		}
		DEL_CLASS(pobjList);
		return mapLyrColl;
	}
}
