#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/SMake.h"
#include "Parser/FileParser/SMakeParser.h"
#include "Text/MyString.h"

Parser::FileParser::SMakeParser::SMakeParser()
{
}

Parser::FileParser::SMakeParser::~SMakeParser()
{
}

Int32 Parser::FileParser::SMakeParser::GetName()
{
	return *(Int32*)"TXTP";
}

void Parser::FileParser::SMakeParser::PrepareSelector(IO::FileSelector *selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::Smake)
	{
		selector->AddFilter(CSTR("SMake.cfg"), CSTR("SMake config File"));
	}
}

IO::ParserType Parser::FileParser::SMakeParser::GetParserType()
{
	return IO::ParserType::Smake;
}

IO::ParsedObject *Parser::FileParser::SMakeParser::ParseFileHdr(IO::StreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr)
{
	if (!fd->IsFullFile())
	{
		return 0;
	}
	if (!fd->GetShortName().Equals(UTF8STRC("SMake.cfg")))
	{
		return 0;
	}
	IO::SMake *smake;
	NEW_CLASS(smake, IO::SMake(fd->GetFullFileName()->ToCString(), 0, 0));
	if (smake->IsLoadFailed())
	{
		DEL_CLASS(smake);
		return 0;
	}

	return smake;
}
