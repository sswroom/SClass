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

void Parser::FileParser::SPDParser::PrepareSelector(IO::FileSelector *selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::MapLayer)
	{
		selector->AddFilter(CSTR("*.spd"), CSTR("SPD File"));
	}
}

IO::ParserType Parser::FileParser::SPDParser::GetParserType()
{
	return IO::ParserType::MapLayer;
}

IO::ParsedObject *Parser::FileParser::SPDParser::ParseFileHdr(NotNullPtr<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr)
{
	UTF8Char sbuff[1024];
	UTF8Char *sptr;
	UOSInt i;
	if (!fd->IsFullFile())
		return 0;
	sptr = fd->GetFullFileName()->ConcatTo(sbuff);
	i = Text::StrLastIndexOfCharC(sbuff, (UOSInt)(sptr - sbuff), '.');
	if (i == INVALID_INDEX || !Text::StrEqualsICaseC(&sbuff[i], (UOSInt)(sptr - &sbuff[i]), UTF8STRC(".SPD")))
	{
		return 0;
	}
	if (IO::Path::GetPathType(CSTRP(sbuff, sptr)) != IO::Path::PathType::File)
	{
		return 0;
	}
	sptr = Text::StrConcatC(&sbuff[i], UTF8STRC(".sps"));
	if (IO::Path::GetPathType(CSTRP(sbuff, sptr)) != IO::Path::PathType::File)
	{
		return 0;
	}
	sptr = Text::StrConcatC(&sbuff[i], UTF8STRC(".spi"));
	if (IO::Path::GetPathType(CSTRP(sbuff, sptr)) != IO::Path::PathType::File)
	{
		return 0;
	}
	sptr = Text::StrConcatC(&sbuff[i], UTF8STRC(".spb"));
	if (IO::Path::GetPathType(CSTRP(sbuff, sptr)) != IO::Path::PathType::File)
	{
		return 0;
	}

	Map::SPDLayer *layer;
	sbuff[i] = 0;
	NEW_CLASS(layer, Map::SPDLayer(Text::CString(sbuff, i)));
	if (layer->IsError())
	{
		DEL_CLASS(layer);
		return 0;
	}
	return layer;
}
