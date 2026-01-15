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

void Parser::FileParser::SMakeParser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
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

Optional<IO::ParsedObject> Parser::FileParser::SMakeParser::ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr)
{
	if (!fd->IsFullFile())
	{
		return nullptr;
	}
	if (!fd->GetShortName().OrEmpty().Equals(UTF8STRC("SMake.cfg")))
	{
		return nullptr;
	}
	IO::SMake *smake;
	NEW_CLASS(smake, IO::SMake(fd->GetFullFileName()->ToCString(), 0, nullptr));
	if (smake->IsLoadFailed())
	{
		DEL_CLASS(smake);
		return nullptr;
	}

	return smake;
}
