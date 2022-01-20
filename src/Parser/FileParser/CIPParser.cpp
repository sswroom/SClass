#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "IO/Path.h"
#include "Map/CIPLayer2.h"
#include "Parser/FileParser/CIPParser.h"

Parser::FileParser::CIPParser::CIPParser()
{
}

Parser::FileParser::CIPParser::~CIPParser()
{
}

Int32 Parser::FileParser::CIPParser::GetName()
{
	return *(Int32*)"CIPP";
}

void Parser::FileParser::CIPParser::PrepareSelector(IO::IFileSelector *selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::MapLayer)
	{
		selector->AddFilter((const UTF8Char*)"*.cip", (const UTF8Char*)"CIP File");
	}
}

IO::ParserType Parser::FileParser::CIPParser::GetParserType()
{
	return IO::ParserType::MapLayer;
}

IO::ParsedObject *Parser::FileParser::CIPParser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType)
{
	UTF8Char sbuff[1024];
	UOSInt i;
	if (!fd->IsFullFile())
		return 0;
	fd->GetFullFileName()->ConcatTo(sbuff);
	i = Text::StrLastIndexOfChar(sbuff, '.');
	if (i == INVALID_INDEX || Text::StrCompareICase(&sbuff[i], (const UTF8Char*)".CIP") != 0)
	{
		return 0;
	}
	if (IO::Path::GetPathType(sbuff) != IO::Path::PathType::File)
	{
		return 0;
	}
	Text::StrConcatC(&sbuff[i], UTF8STRC(".ciu"));
	if (IO::Path::GetPathType(sbuff) != IO::Path::PathType::File)
	{
		return 0;
	}
	Text::StrConcatC(&sbuff[i], UTF8STRC(".cix"));
	if (IO::Path::GetPathType(sbuff) != IO::Path::PathType::File)
	{
		return 0;
	}
	Text::StrConcatC(&sbuff[i], UTF8STRC(".blk"));
	if (IO::Path::GetPathType(sbuff) != IO::Path::PathType::File)
	{
		return 0;
	}

	Map::CIPLayer2 *layer;
	Text::StrConcatC(&sbuff[i], UTF8STRC(".cip"));
	NEW_CLASS(layer, Map::CIPLayer2(sbuff));
	if (layer->IsError())
	{
		DEL_CLASS(layer);
		return 0;
	}
	return layer;
}
