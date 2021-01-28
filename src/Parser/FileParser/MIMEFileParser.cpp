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

void Parser::FileParser::MIMEFileParser::PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t)
{
	if (t == IO::ParsedObject::PT_UNKNOWN || t == IO::ParsedObject::PT_MIME_OBJECT)
	{
		selector->AddFilter((const UTF8Char*)"*.eml", (const UTF8Char*)"Email File");
	}
}

IO::ParsedObject::ParserType Parser::FileParser::MIMEFileParser::GetParserType()
{
	return IO::ParsedObject::PT_MIME_OBJECT;
}

IO::ParsedObject *Parser::FileParser::MIMEFileParser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType)
{
	const UTF8Char *name = fd->GetFullName();
	Text::IMIMEObj *obj;
	if (Text::StrEndsWithICase(name, (const UTF8Char*)".eml"))
	{
		obj = Text::MIMEObj::MailMessage::ParseFile(fd);
		return obj;
	}
	return 0;
}
