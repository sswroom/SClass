#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Parser/FileParser/SHPParser.h"
#include "Map/SHPData.h"

Parser::FileParser::SHPParser::SHPParser()
{
	this->codePage = 0;
}

Parser::FileParser::SHPParser::~SHPParser()
{
}

Int32 Parser::FileParser::SHPParser::GetName()
{
	return *(Int32*)"SHPP";
}

void Parser::FileParser::SHPParser::SetCodePage(UInt32 codePage)
{
	this->codePage = codePage;
}

void Parser::FileParser::SHPParser::SetArcGISPRJParser(Math::ArcGISPRJParser *prjParser)
{
	this->prjParser = prjParser;
}

void Parser::FileParser::SHPParser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::MapLayer)
	{
		selector->AddFilter(CSTR("*.shp"), CSTR("ESRI Shapefile"));
	}
}

IO::ParserType Parser::FileParser::SHPParser::GetParserType()
{
	return IO::ParserType::PackageFile;
}

IO::ParsedObject *Parser::FileParser::SHPParser::ParseFileHdr(NN<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr)
{
	if (!fd->IsFullFile())
		return 0;
	if (ReadMInt32(hdr) != 9994 || ReadInt32(&hdr[28]) != 1000 || (ReadMUInt32(&hdr[24]) << 1) != fd->GetDataSize())
	{
		return 0;
	}

	Map::SHPData *shp;
	NEW_CLASS(shp, Map::SHPData(hdr, fd, this->codePage, this->prjParser));
	if (shp->IsError())
	{
		DEL_CLASS(shp);
		return 0;
	}
	return shp;
}
