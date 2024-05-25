#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Text/IMIMEObj.h"
#include "Text/MyString.h"
#include "Text/MIMEObj/MailMessage.h"
#include "Parser/FileParser/MIMEFileParser.h"

Parser::FileParser::MIMEFileParser::MIMEFileParser()
{
}

Parser::FileParser::MIMEFileParser::~MIMEFileParser()
{
}

Int32 Parser::FileParser::MIMEFileParser::GetName()
{
	return *(Int32*)"MIME";
}

void Parser::FileParser::MIMEFileParser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::MIMEObject)
	{
		selector->AddFilter(CSTR("*.eml"), CSTR("Email File"));
	}
}

IO::ParserType Parser::FileParser::MIMEFileParser::GetParserType()
{
	return IO::ParserType::MIMEObject;
}

Optional<IO::ParsedObject> Parser::FileParser::MIMEFileParser::ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr)
{
	NN<Text::String> name = fd->GetFullName();
	if (name->EndsWithICase(UTF8STRC(".eml")))
	{
		return Text::MIMEObj::MailMessage::ParseFile(fd);
	}
	return 0;
}
