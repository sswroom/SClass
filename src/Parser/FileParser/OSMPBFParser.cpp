#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/ByteTool_C.h"
#include "IO/StreamData.h"
#include "Map/OSM/OSMData.h"
#include "Parser/FileParser/OSMPBFParser.h"

Parser::FileParser::OSMPBFParser::OSMPBFParser()
{
}

Parser::FileParser::OSMPBFParser::~OSMPBFParser()
{
}

Int32 Parser::FileParser::OSMPBFParser::GetName()
{
	return *(Int32*)"OSMP";
}

void Parser::FileParser::OSMPBFParser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::MediaFile)
	{
		selector->AddFilter(CSTR("*.osm.pbf"), CSTR("OSM PBF File"));
	}
}

IO::ParserType Parser::FileParser::OSMPBFParser::GetParserType()
{
	return IO::ParserType::MapLayer;
}

Optional<IO::ParsedObject> Parser::FileParser::OSMPBFParser::ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr)
{
	if (hdr[4] != 0x0a || hdr[5] != 9 || !Text::StrEqualsC(&hdr[6], 9, UTF8STRC("OSMHeader")))
	{
		return;
	}
	NN<Map::OSM::OSMData> osmData;
	NEW_CLASSNN(osmData, Map::OSM::OSMData(fd->GetFullFileName()));


}
