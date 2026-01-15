#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/PackageFile.h"
#include "Map/TileMapLayer.h"
#include "Map/OSM/OSMLocalTileMap.h"
#include "Parser/ObjParser/OSMMapParser.h"
#include "Text/JSON.h"

Parser::ObjParser::OSMMapParser::OSMMapParser()
{
	this->parsers = nullptr;
}

Parser::ObjParser::OSMMapParser::~OSMMapParser()
{
}

Int32 Parser::ObjParser::OSMMapParser::GetName()
{
	return *(Int32*)"OSMM";
}

void Parser::ObjParser::OSMMapParser::SetParserList(Optional<Parser::ParserList> parsers)
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

Optional<IO::ParsedObject> Parser::ObjParser::OSMMapParser::ParseObject(NN<IO::ParsedObject> pobj, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType)
{
	NN<IO::PackageFile> pkg;
	if (pobj->GetParserType() != IO::ParserType::PackageFile)
		return nullptr;
	pkg = NN<IO::PackageFile>::ConvertFrom(pobj);
	NN<IO::StreamData> fd;
	if (!pkg->OpenStreamData(CSTR("metadata.json")).SetTo(fd))
		return nullptr;
	NN<Parser::ParserList> parsers;
	if (!this->parsers.SetTo(parsers))
		return nullptr;

	UIntOS buffSize = (UIntOS)fd->GetDataSize();
	UInt8 *fileBuff = MemAlloc(UInt8, buffSize + 1);
	fileBuff[fd->GetRealData(0, buffSize, Data::ByteArray(fileBuff, buffSize + 1))] = 0;
	Optional<Text::JSONBase> optfileJSON = Text::JSONBase::ParseJSONStr(Text::CStringNN(fileBuff, buffSize));
	NN<Text::JSONBase> fileJSON;
	MemFree(fileBuff);

	if (!optfileJSON.SetTo(fileJSON))
	{
		fd.Delete();
		return nullptr;
	}
	if (fileJSON->GetType() == Text::JSONType::Object)
	{
		NN<Text::JSONObject> jobj = NN<Text::JSONObject>::ConvertFrom(fileJSON);
		NN<Text::JSONBase> jbase;
		if (jobj->GetObjectValue(CSTR("type")).SetTo(jbase) && jbase->Equals(CSTR("overlay")) && fd->GetShortName().OrEmpty().EndsWith(UTF8STRC("metadata.json")))
		{
			NN<Text::String> name;
			NN<Text::String> format;
			NN<Text::String> sMinZoom;
			NN<Text::String> sMaxZoom;
			UInt32 minZoom;
			UInt32 maxZoom;
			NN<Text::JSONArray> bounds;
			if (jobj->GetObjectString(CSTR("name")).SetTo(name) &&
				jobj->GetObjectString(CSTR("format")).SetTo(format) &&
				jobj->GetObjectString(CSTR("minzoom")).SetTo(sMinZoom) &&
				jobj->GetObjectString(CSTR("maxzoom")).SetTo(sMaxZoom) &&
				jobj->GetObjectArray(CSTR("bounds")).SetTo(bounds) && sMinZoom->ToUInt32(minZoom) && sMaxZoom->ToUInt32(maxZoom) && bounds->GetArrayLength() == 4)
			{
				Math::Coord2DDbl maxCoord;
				Math::Coord2DDbl minCoord;
				Math::Coord2DDbl coord;
				coord = Math::Coord2DDbl(bounds->GetArrayDoubleOrNAN(0), bounds->GetArrayDoubleOrNAN(1));
				maxCoord = Math::Coord2DDbl(bounds->GetArrayDoubleOrNAN(2), bounds->GetArrayDoubleOrNAN(3));
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
	return nullptr;
}
