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

void Parser::FileParser::SMakeParser::PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t)
{
	if (t == IO::ParsedObject::PT_UNKNOWN || t == IO::ParsedObject::PT_SMAKE)
	{
		selector->AddFilter((const UTF8Char*)"SMake.cfg", (const UTF8Char*)"SMake config File");
	}
}

IO::ParsedObject::ParserType Parser::FileParser::SMakeParser::GetParserType()
{
	return IO::ParsedObject::PT_SMAKE;
}

IO::ParsedObject *Parser::FileParser::SMakeParser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType)
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
	NEW_CLASS(smake, IO::SMake(fd->GetFullFileName(), 0, 0));
	if (smake->IsLoadFailed())
	{
		DEL_CLASS(smake);
		return 0;
	}

	return smake;
}
