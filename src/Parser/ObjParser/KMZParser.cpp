#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/PackageFile.h"
#include "Map/MapLayerCollection.h"
#include "Parser/ParserList.h"
#include "Parser/FileParser/XMLParser.h"
#include "Parser/ObjParser/KMZParser.h"

Parser::ObjParser::KMZParser::KMZParser()
{
	this->browser = nullptr;
	this->parsers = nullptr;
	this->encFact = nullptr;
}

Parser::ObjParser::KMZParser::~KMZParser()
{
}

Int32 Parser::ObjParser::KMZParser::GetName()
{
	return *(Int32*)"KMZP";
}

void Parser::ObjParser::KMZParser::SetWebBrowser(Optional<Net::WebBrowser> browser)
{
	this->browser = browser;
}

void Parser::ObjParser::KMZParser::SetParserList(Optional<Parser::ParserList> parsers)
{
	this->parsers = parsers;
}

void Parser::ObjParser::KMZParser::SetEncFactory(Optional<Text::EncodingFactory> encFact)
{
	this->encFact = encFact;
}

void Parser::ObjParser::KMZParser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
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

Optional<IO::ParsedObject> Parser::ObjParser::KMZParser::ParseObject(NN<IO::ParsedObject> pobj, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType)
{
	NN<Parser::ParserList> parsers;
	if (pobj->GetParserType() != IO::ParserType::PackageFile || (targetType != IO::ParserType::MapLayer && targetType != IO::ParserType::Unknown))
		return nullptr;
	if (!this->parsers.SetTo(parsers))
		return nullptr;

	NN<IO::PackageFile> pf = NN<IO::PackageFile>::ConvertFrom(pobj);
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	UOSInt ui = pf->GetCount();
	while (ui-- > 0)
	{
		if (pf->GetItemName(sbuff, ui).SetTo(sptr) && Text::StrEndsWithC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC(".kml")) && pf->GetItemType(ui) == IO::PackageFile::PackObjectType::StreamData)
		{
			NN<IO::StreamData> stmData;
			Optional<IO::ParsedObject> pobj = nullptr;
			NN<IO::ParsedObject> nnpobj;
			if (pf->GetItemStmDataNew(ui).SetTo(stmData))
			{
				Parser::FileParser::XMLParser xmlParser;
				xmlParser.SetParserList(this->parsers);
				xmlParser.SetEncFactory(this->encFact);
				xmlParser.SetWebBrowser(this->browser);
				pobj = xmlParser.ParseFile(stmData, pf, IO::ParserType::MapLayer);
				stmData.Delete();
			}
			if (pobj.SetTo(nnpobj))
			{
				if (nnpobj->GetParserType() == IO::ParserType::MapLayer)
				{
					return nnpobj;
				}
				else
				{
					nnpobj.Delete();
				}
			}
		}
	}

/*	NN<IO::PackageFile> pkg = NN<IO::PackageFile>::ConvertFrom(pobj);
	//UTF8Char sbuff[256];
	//UnsafeArray<UTF8Char> sptr;
	Data::ArrayListNN<IO::ParsedObject> pobjList;
	NN<IO::ParsedObject> pobj2;
	UOSInt i;
	UOSInt j;
	i = 0;
	j = pkg->GetCount();
	while (i < j)
	{
		if (pkg->GetItemName(sbuff, i).SetTo(sptr) && Text::StrEndsWithC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC(".kml")))
		{
			NN<IO::StreamData> fd;
			if (pkg->GetItemStmDataNew(i).SetTo(fd))
			{
				if (parsers->ParseFile(fd).SetTo(pobj2))
				{
					if (pobj->GetSourceNameObj()->EndsWithICase(UTF8STRC(".kmz")))
					{
						pobj2->SetSourceName(pobj->GetSourceNameObj());
					}
					if (pobj2->GetParserType() == IO::ParserType::MapLayer)
					{
						pobjList.Add(pobj2);
					}
					else
					{
						pobj2.Delete();
					}
				}
				fd.Delete();
			}
		}
		i++;
	}
	if (pobjList.GetCount() == 0)
	{
		return 0;
	}
	else if (pobjList.GetCount() == 1)
	{
		return pobjList.GetItem(0).OrNull();
	}
	else
	{
		Map::MapLayerCollection *mapLyrColl;
		NN<IO::ParsedObject> nnpobj2;
		NN<Map::MapDrawLayer> layer;
		NEW_CLASS(mapLyrColl, Map::MapLayerCollection(pobj->GetSourceNameObj(), 0));
		i = 0;
		j = pobjList.GetCount();
		while (i < j)
		{
			nnpobj2 = pobjList.GetItemNoCheck(i);
			if (nnpobj2->GetParserType() == IO::ParserType::MapLayer)
			{
				mapLyrColl->Add(NN<Map::MapDrawLayer>::ConvertFrom(nnpobj2));
			}
			else
			{
				NN<Map::MapLayerCollection> lyrColl = NN<Map::MapLayerCollection>::ConvertFrom(nnpobj2);
				while (lyrColl->GetCount() > 0)
				{
					if (lyrColl->RemoveAt(0).SetTo(layer))
						mapLyrColl->Add(layer);
				}
				nnpobj2.Delete();
			}
			i++;
		}
		return mapLyrColl;
	}*/
	return nullptr;
}
