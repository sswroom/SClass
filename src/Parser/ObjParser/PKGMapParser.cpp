#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/PackageFile.h"
#include "Map/MapLayerCollection.h"
#include "Parser/FileParser/GUIImgParser.h"
#include "Parser/FileParser/TIFFParser.h"
#include "Parser/FileParser/XMLParser.h"
#include "Parser/ObjParser/PKGMapParser.h"

Parser::ObjParser::PKGMapParser::PKGMapParser()
{
	this->encFact = nullptr;
}

Parser::ObjParser::PKGMapParser::~PKGMapParser()
{
}

Int32 Parser::ObjParser::PKGMapParser::GetName()
{
	return *(Int32*)"PMAP";
}

void Parser::ObjParser::PKGMapParser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
{
}

IO::ParserType Parser::ObjParser::PKGMapParser::GetParserType()
{
	return IO::ParserType::MapLayer;
}

Optional<IO::ParsedObject> Parser::ObjParser::PKGMapParser::ParseObject(NN<IO::ParsedObject> pobj, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType)
{
	NN<IO::PackageFile> pkg;
	if (pobj->GetParserType() != IO::ParserType::PackageFile)
		return nullptr;
	pkg = NN<IO::PackageFile>::ConvertFrom(pobj);
	Data::ArrayListNN<Map::MapDrawLayer> layers;
	ParsePackage(pkg, layers, this->encFact);
	if (layers.GetCount() == 0)
		return nullptr;
	if (layers.GetCount() == 1)
		return layers.GetItemNoCheck(0);
	NN<Map::MapLayerCollection> mapColl;
	NEW_CLASSNN(mapColl, Map::MapLayerCollection(pobj->GetSourceNameObj(), 0));
	UOSInt i = 0;
	UOSInt j = layers.GetCount();
	while (i < j)
	{
		mapColl->Add(layers.GetItemNoCheck(i));
		i++;
	}
	mapColl->ReorderLayers();
	return mapColl;
}

void Parser::ObjParser::PKGMapParser::ParsePackage(NN<IO::PackageFile> pkg, NN<Data::ArrayListNN<Map::MapDrawLayer>> layers, Optional<Text::EncodingFactory> encFact)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	NN<IO::PackageFile> subPkg;
	Bool needRelease;
	IO::PackageFile::PackObjectType pot;
	NN<IO::StreamData> fd;
	UOSInt i = 0;
	UOSInt j = pkg->GetCount();
	while (i < j)
	{
		pot = pkg->GetItemType(i);
		if (pot == IO::PackageFile::PackObjectType::PackageFileType && j == 1)
		{
			if (pkg->GetItemPack(i, needRelease).SetTo(subPkg))
			{
				ParsePackage(subPkg, layers, encFact);
				if (needRelease) subPkg.Delete();
			}
		}
		else if (pot == IO::PackageFile::PackObjectType::StreamData)
		{
			if (pkg->GetItemName(sbuff, i).SetTo(sptr))
			{
				if (CSTRP(sbuff, sptr).EndsWith(CSTR(".tif")) && pkg->GetItemStmDataNew(i).SetTo(fd))
				{
					NN<IO::ParsedObject> pobj;
					Parser::FileParser::TIFFParser tiffParser;
					if (tiffParser.ParseFile(fd, pkg, IO::ParserType::MapLayer).SetTo(pobj))
					{
						if (pobj->GetParserType() == IO::ParserType::MapLayer)
							layers->Add(NN<Map::MapDrawLayer>::ConvertFrom(pobj));
						else
							pobj.Delete();
					}
					else
					{
						Parser::FileParser::GUIImgParser imgParser;
						if (imgParser.ParseFile(fd, pkg, IO::ParserType::MapLayer).SetTo(pobj))
						{
							if (pobj->GetParserType() == IO::ParserType::MapLayer)
								layers->Add(NN<Map::MapDrawLayer>::ConvertFrom(pobj));
							else
								pobj.Delete();
						}
					}
					fd.Delete();
				}
				else if (CSTRP(sbuff, sptr).EndsWith(CSTR(".gml")) && pkg->GetItemStmDataNew(i).SetTo(fd))
				{
					NN<IO::ParsedObject> pobj;
					Parser::FileParser::XMLParser xmlParser;
					xmlParser.SetEncFactory(encFact);
					if (xmlParser.ParseFile(fd, pkg, IO::ParserType::MapLayer).SetTo(pobj))
					{
						if (pobj->GetParserType() == IO::ParserType::MapLayer)
							layers->Add(NN<Map::MapDrawLayer>::ConvertFrom(pobj));
						else
							pobj.Delete();
					}
				}
			}
		}
		i++;
	}
}
