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

void Parser::FileParser::CIPParser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::MapLayer)
	{
		selector->AddFilter(CSTR("*.cip"), CSTR("CIP File"));
	}
}

IO::ParserType Parser::FileParser::CIPParser::GetParserType()
{
	return IO::ParserType::MapLayer;
}

Optional<IO::ParsedObject> Parser::FileParser::CIPParser::ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr)
{
	UTF8Char sbuff[1024];
	UnsafeArray<UTF8Char> sptr;
	UOSInt i;
	if (!fd->IsFullFile())
		return 0;
	sptr = fd->GetFullFileName()->ConcatTo(sbuff);
	i = Text::StrLastIndexOfCharC(sbuff, (UOSInt)(sptr - sbuff), '.');
	if (i == INVALID_INDEX || !Text::StrEqualsICaseC(&sbuff[i], (UOSInt)(sptr - &sbuff[i]), UTF8STRC(".CIP")))
	{
		return 0;
	}
	if (IO::Path::GetPathType(CSTRP(sbuff, sptr)) != IO::Path::PathType::File)
	{
		return 0;
	}
	sptr = Text::StrConcatC(&sbuff[i], UTF8STRC(".ciu"));
	if (IO::Path::GetPathType(CSTRP(sbuff, sptr)) != IO::Path::PathType::File)
	{
		return 0;
	}
	sptr = Text::StrConcatC(&sbuff[i], UTF8STRC(".cix"));
	if (IO::Path::GetPathType(CSTRP(sbuff, sptr)) != IO::Path::PathType::File)
	{
		return 0;
	}
	sptr = Text::StrConcatC(&sbuff[i], UTF8STRC(".blk"));
	if (IO::Path::GetPathType(CSTRP(sbuff, sptr)) != IO::Path::PathType::File)
	{
		return 0;
	}

	Map::CIPLayer2 *layer;
	sptr = Text::StrConcatC(&sbuff[i], UTF8STRC(".cip"));
	NEW_CLASS(layer, Map::CIPLayer2(CSTRP(sbuff, sptr)));
	if (layer->IsError())
	{
		DEL_CLASS(layer);
		return 0;
	}
	return layer;
}
