#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/PackageFile.h"
#include "Map/TileMapLayer.h"
#include "Map/OSM/OSMLocalTileMap.h"
#include "Parser/ObjParser/OSMMapParser.h"
#include "Text/JSON.h"

Parser::ObjParser::OSMMapParser::OSMMapParser()
{
	this->parsers = 0;
}

Parser::ObjParser::OSMMapParser::~OSMMapParser()
{
}

Int32 Parser::ObjParser::OSMMapParser::GetName()
{
	return *(Int32*)"OSMM";
}

void Parser::ObjParser::OSMMapParser::SetParserList(Parser::ParserList *parsers)
{
	this->parsers = parsers;
}

void Parser::ObjParser::OSMMapParser::PrepareSelector(NotNullPtr<IO::FileSelector> selector, IO::ParserType t)
{
}

IO::ParserType Parser::ObjParser::OSMMapParser::GetParserType()
{
	return IO::ParserType::MapLayer;
}

IO::ParsedObject *Parser::ObjParser::OSMMapParser::ParseObject(NotNullPtr<IO::ParsedObject> pobj, IO::PackageFile *pkgFile, IO::ParserType targetType)
{
	NotNullPtr<IO::PackageFile> pkg;
	if (pobj->GetParserType() != IO::ParserType::PackageFile)
		return 0;
	pkg = NotNullPtr<IO::PackageFile>::ConvertFrom(pobj);
	IO::StreamData *fd = pkg->OpenStreamData(CSTR("metadata.json"));
	if (fd == 0)
		return 0;

	UOSInt buffSize = (UOSInt)fd->GetDataSize();
	UInt8 *fileBuff = MemAlloc(UInt8, buffSize + 1);
	fileBuff[fd->GetRealData(0, buffSize, Data::ByteArray(fileBuff, buffSize + 1))] = 0;
	Text::JSONBase *fileJSON = Text::JSONBase::ParseJSONStr(Text::CStringNN(fileBuff, buffSize));
	MemFree(fileBuff);

	if (fileJSON == 0)
	{
		DEL_CLASS(fd);
		return 0;
	}
	if (fileJSON->GetType() == Text::JSONType::Object)
	{
		Text::JSONObject *jobj = (Text::JSONObject*)fileJSON;
		Text::JSONBase *jbase = jobj->GetObjectValue(CSTR("type"));
		if (jbase && jbase->Equals(CSTR("overlay")) && fd->GetShortName().EndsWith(UTF8STRC("metadata.json")))
		{
			NotNullPtr<Text::String> name;
			NotNullPtr<Text::String> format;
			Text::String *sMinZoom = jobj->GetObjectString(CSTR("minzoom"));
			Text::String *sMaxZoom = jobj->GetObjectString(CSTR("maxzoom"));
			UInt32 minZoom;
			UInt32 maxZoom;
			Text::JSONArray *bounds = jobj->GetObjectArray(CSTR("bounds"));
			if (name.Set(jobj->GetObjectString(CSTR("name"))) && format.Set(jobj->GetObjectString(CSTR("format"))) && sMinZoom && sMaxZoom && bounds && sMinZoom->ToUInt32(minZoom) && sMaxZoom->ToUInt32(maxZoom) && bounds->GetArrayLength() == 4)
			{
				Math::Coord2DDbl maxCoord;
				Math::Coord2DDbl minCoord;
				Math::Coord2DDbl coord;
				coord = Math::Coord2DDbl(bounds->GetArrayDouble(0), bounds->GetArrayDouble(1));
				maxCoord = Math::Coord2DDbl(bounds->GetArrayDouble(2), bounds->GetArrayDouble(3));
				minCoord = coord.Min(maxCoord);
				maxCoord = coord.Max(maxCoord);
				NotNullPtr<Map::OSM::OSMLocalTileMap> tileMap;
				NotNullPtr<Map::TileMapLayer> mapLayer;
				NEW_CLASSNN(tileMap, Map::OSM::OSMLocalTileMap(pkg->Clone(), name, format, minZoom, maxZoom, minCoord, maxCoord));
				NEW_CLASSNN(mapLayer, Map::TileMapLayer(tileMap, this->parsers));
				DEL_CLASS(fd);
				jobj->EndUse();
				return mapLayer.Ptr();
			}
		}
	}
	DEL_CLASS(fd);
	fileJSON->EndUse();
	return 0;
}
