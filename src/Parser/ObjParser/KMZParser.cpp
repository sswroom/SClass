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

void Parser::ObjParser::KMZParser::PrepareSelector(IO::IFileSelector *selector, IO::ParserType t)
{
	if (t == IO::ParserType::MapLayer)
	{
		selector->AddFilter((const UTF8Char*)"*.kmz", (const UTF8Char*)"Zipped KML File");
	}
}

IO::ParserType Parser::ObjParser::KMZParser::GetParserType()
{
	return IO::ParserType::MapLayer;
}

IO::ParsedObject *Parser::ObjParser::KMZParser::ParseObject(IO::ParsedObject *pobj, IO::PackageFile *pkgFile, IO::ParserType targetType)
{
	if (pobj->GetParserType() != IO::ParserType::PackageFile)
		return 0;
	if (this->parsers == 0)
		return 0;
	IO::PackageFile *pkg = (IO::PackageFile*)pobj;
	IO::ParserType pt;
	UTF8Char u8buff[256];
	Data::ArrayList<IO::ParsedObject*> *pobjList;
	IO::IStreamData *fd;
	IO::ParsedObject *pobj2;
	NEW_CLASS(pobjList, Data::ArrayList<IO::ParsedObject*>());
	UOSInt i;
	UOSInt j;
	i = 0;
	j = pkg->GetCount();
	while (i < j)
	{
		pkg->GetItemName(u8buff, i);
		if (Text::StrEndsWith(u8buff, (const UTF8Char*)".kml"))
		{
			fd = pkg->GetItemStmData(i);
			if (fd)
			{
				pobj2 = this->parsers->ParseFile(fd, &pt);
				DEL_CLASS(fd);
				if (pobj2)
				{
					if (pobj->GetSourceNameObj()->EndsWithICase((const UTF8Char*)".kmz"))
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
		NEW_CLASS(mapLyrColl, Map::MapLayerCollection(pobj->GetSourceNameObj(), 0));
		i = 0;
		j = pobjList->GetCount();
		while (i < j)
		{
			pobj2 = pobjList->GetItem(i);
			if (pobj2->GetParserType() == IO::ParserType::MapLayer)
			{
				mapLyrColl->Add((Map::IMapDrawLayer*)pobj2);
			}
			else
			{
				Map::MapLayerCollection *lyrColl = (Map::MapLayerCollection*)pobj2;
				while (lyrColl->GetCount() > 0)
				{
					mapLyrColl->Add(lyrColl->RemoveAt(0));
				}
				DEL_CLASS(pobj2);
			}
			i++;
		}
		DEL_CLASS(pobjList);
		return mapLyrColl;
	}
}
