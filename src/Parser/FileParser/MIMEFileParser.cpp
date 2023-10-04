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

void Parser::FileParser::MIMEFileParser::PrepareSelector(NotNullPtr<IO::FileSelector> selector, IO::ParserType t)
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

IO::ParsedObject *Parser::FileParser::MIMEFileParser::ParseFileHdr(NotNullPtr<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr)
{
	NotNullPtr<Text::String> name = fd->GetFullName();
	Text::IMIMEObj *obj;
	if (name->EndsWithICase(UTF8STRC(".eml")))
	{
		obj = Text::MIMEObj::MailMessage::ParseFile(fd);
		return obj;
	}
	return 0;
}
