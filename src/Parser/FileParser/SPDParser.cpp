#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "IO/Path.h"
#include "Map/SPDLayer.h"
#include "Parser/FileParser/SPDParser.h"

Parser::FileParser::SPDParser::SPDParser()
{
}

Parser::FileParser::SPDParser::~SPDParser()
{
}

Int32 Parser::FileParser::SPDParser::GetName()
{
	return *(Int32*)"SPDP";
}

void Parser::FileParser::SPDParser::PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t)
{
	if (t == IO::ParsedObject::PT_UNKNOWN || t == IO::ParsedObject::PT_MAP_LAYER_PARSER)
	{
		selector->AddFilter((const UTF8Char*)"*.spd", (const UTF8Char*)"SPD File");
	}
}

IO::ParsedObject::ParserType Parser::FileParser::SPDParser::GetParserType()
{
	return IO::ParsedObject::PT_MAP_LAYER_PARSER;
}

IO::ParsedObject *Parser::FileParser::SPDParser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType)
{
	UTF8Char sbuff[1024];
	OSInt i;
	if (!fd->IsFullFile())
		return 0;
	Text::StrConcat(sbuff, fd->GetFullFileName());
	i = Text::StrLastIndexOf(sbuff, '.');
	if (Text::StrCompareICase(&sbuff[i], (const UTF8Char*)".SPD") != 0)
	{
		return 0;
	}
	if (IO::Path::GetPathType(sbuff) != IO::Path::PT_FILE)
	{
		return 0;
	}
	Text::StrConcat(&sbuff[i], (const UTF8Char*)".sps");
	if (IO::Path::GetPathType(sbuff) != IO::Path::PT_FILE)
	{
		return 0;
	}
	Text::StrConcat(&sbuff[i], (const UTF8Char*)".spi");
	if (IO::Path::GetPathType(sbuff) != IO::Path::PT_FILE)
	{
		return 0;
	}
	Text::StrConcat(&sbuff[i], (const UTF8Char*)".spb");
	if (IO::Path::GetPathType(sbuff) != IO::Path::PT_FILE)
	{
		return 0;
	}

	Map::SPDLayer *layer;
	sbuff[i] = 0;
	NEW_CLASS(layer, Map::SPDLayer(sbuff));
	if (layer->IsError())
	{
		DEL_CLASS(layer);
		return 0;
	}
	return layer;
}
