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

void Parser::FileParser::SHPParser::PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t)
{
	if (t == IO::ParsedObject::PT_UNKNOWN || t == IO::ParsedObject::PT_MAP_LAYER_PARSER)
	{
		selector->AddFilter((const UTF8Char*)"*.shp", (const UTF8Char*)"ESRI Shapefile");
	}
}

IO::ParsedObject::ParserType Parser::FileParser::SHPParser::GetParserType()
{
	return IO::ParsedObject::PT_PACKAGE_PARSER;
}

IO::ParsedObject *Parser::FileParser::SHPParser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType)
{
	UInt8 shpBuff[100];
	if (!fd->IsFullFile())
		return 0;
	if (fd->GetRealData(0, 100, shpBuff) != 100)
		return 0;
	if (ReadMInt32(shpBuff) != 9994 || ReadInt32(&shpBuff[28]) != 1000 || (ReadMUInt32(&shpBuff[24]) << 1) != fd->GetDataSize())
	{
		return 0;
	}

	Map::SHPData *shp;
	NEW_CLASS(shp, Map::SHPData(shpBuff, fd, this->codePage));
	if (shp->IsError())
	{
		DEL_CLASS(shp);
		return 0;
	}
	return shp;
}
