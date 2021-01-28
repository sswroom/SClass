#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/UDPFileLog.h"
#include "Parser/FileParser/UDPParser.h"

Parser::FileParser::UDPParser::UDPParser()
{
}

Parser::FileParser::UDPParser::~UDPParser()
{
}

Int32 Parser::FileParser::UDPParser::GetName()
{
	return *(Int32*)"UDPP";
}

void Parser::FileParser::UDPParser::PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t)
{
	if (t == IO::ParsedObject::PT_UNKNOWN || t == IO::ParsedObject::PT_LOG_FILE)
	{
		selector->AddFilter((const UTF8Char*)"*.udp", (const UTF8Char*)"UDP Log File");
	}
}

IO::ParsedObject::ParserType Parser::FileParser::UDPParser::GetParserType()
{
	return IO::ParsedObject::PT_LOG_FILE;
}

IO::ParsedObject *Parser::FileParser::UDPParser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType)
{
	UInt8 buff[256];
	OSInt i = Text::StrLastIndexOf(fd->GetFullName(), '.');
	if (i < 0)
		return 0;
	if (Text::StrCompareICase(&(fd->GetFullName())[i + 1], (const UTF8Char*)"UDP") != 0)
		return 0;

	fd->GetRealData(0, 256, buff);
	if (buff[0] == 0xaa && buff[1] == 0xbb)
	{
		IO::UDPLog *log;
		NEW_CLASS(log, IO::UDPFileLog(fd));
		return log;
	}
	else
	{
		return 0;
	}
}
