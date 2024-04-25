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

void Parser::ObjParser::OSMMapParser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
{
}

IO::ParserType Parser::ObjParser::OSMMapParser::GetParserType()
{
	return IO::ParserType::MapLayer;
}

IO::ParsedObject *Parser::ObjParser::OSMMapParser::ParseObject(NN<IO::ParsedObject> pobj, IO::PackageFile *pkgFile, IO::ParserType targetType)
{
	NN<IO::PackageFile> pkg;
	if (pobj->GetParserType() != IO::ParserType::PackageFile)
		return 0;
	pkg = NN<IO::PackageFile>::ConvertFrom(pobj);
	NN<IO::StreamData> fd;
	if (!pkg->OpenStreamData(CSTR("metadata.json")).SetTo(fd))
		return 0;
	NN<Parser::ParserList> parsers;
	if (!parsers.Set(this->parsers))
		return 0;

	UOSInt buffSize = (UOSInt)fd->GetDataSize();
	UInt8 *fileBuff = MemAlloc(UInt8, buffSize + 1);
	fileBuff[fd->GetRealData(0, buffSize, Data::ByteArray(fileBuff, buffSize + 1))] = 0;
	Text::JSONBase *fileJSON = Text::JSONBase::ParseJSONStr(Text::CStringNN(fileBuff, buffSize));
	MemFree(fileBuff);

	if (fileJSON == 0)
	{
		fd.Delete();
		return 0;
	}
	if (fileJSON->GetType() == Text::JSONType::Object)
	{
		Text::JSONObject *jobj = (Text::JSONObject*)fileJSON;
		Text::JSONBase *jbase = jobj->GetObjectValue(CSTR("type"));
		if (jbase && jbase->Equals(CSTR("overlay")) && fd->GetShortName().EndsWith(UTF8STRC("metadata.json")))
		{
			NN<Text::String> name;
			NN<Text::String> format;
			NN<Text::String> sMinZoom;
			NN<Text::String> sMaxZoom;
			UInt32 minZoom;
			UInt32 maxZoom;
			Text::JSONArray *bounds = jobj->GetObjectArray(CSTR("bounds"));
			if (jobj->GetObjectString(CSTR("name")).SetTo(name) &&
				jobj->GetObjectString(CSTR("format")).SetTo(format) &&
				jobj->GetObjectString(CSTR("minzoom")).SetTo(sMinZoom) &&
				jobj->GetObjectString(CSTR("maxzoom")).SetTo(sMaxZoom) && bounds && sMinZoom->ToUInt32(minZoom) && sMaxZoom->ToUInt32(maxZoom) && bounds->GetArrayLength() == 4)
			{
				Math::Coord2DDbl maxCoord;
				Math::Coord2DDbl minCoord;
				Math::Coord2DDbl coord;
				coord = Math::Coord2DDbl(bounds->GetArrayDouble(0), bounds->GetArrayDouble(1));
				maxCoord = Math::Coord2DDbl(bounds->GetArrayDouble(2), bounds->GetArrayDouble(3));
				minCoord = coord.Min(maxCoord);
				maxCoord = coord.Max(maxCoord);
				NN<Map::OSM::OSMLocalTileMap> tileMap;
				NN<Map::TileMapLayer> mapLayer;
				NEW_CLASSNN(tileMap, Map::OSM::OSMLocalTileMap(pkg->Clone(), name, format, minZoom, maxZoom, minCoord, maxCoord));
				NEW_CLASSNN(mapLayer, Map::TileMapLayer(tileMap, parsers));
				fd.Delete();
				jobj->EndUse();
				return mapLayer.Ptr();
			}
		}
	}
	fd.Delete();
	fileJSON->EndUse();
	return 0;
}
