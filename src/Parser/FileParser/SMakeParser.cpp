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

void Parser::FileParser::SMakeParser::PrepareSelector(IO::IFileSelector *selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::Smake)
	{
		selector->AddFilter((const UTF8Char*)"SMake.cfg", (const UTF8Char*)"SMake config File");
	}
}

IO::ParserType Parser::FileParser::SMakeParser::GetParserType()
{
	return IO::ParserType::Smake;
}

IO::ParsedObject *Parser::FileParser::SMakeParser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType)
{
	if (!fd->IsFullFile())
	{
		return 0;
	}
	if (!Text::StrEquals(fd->GetShortName(), (const UTF8Char*)"SMake.cfg"))
	{
		return 0;
	}
	IO::SMake *smake;
	NEW_CLASS(smake, IO::SMake(fd->GetFullFileName()->v, 0, 0));
	if (smake->IsLoadFailed())
	{
		DEL_CLASS(smake);
		return 0;
	}

	return smake;
}
