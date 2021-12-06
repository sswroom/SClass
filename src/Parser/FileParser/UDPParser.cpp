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

void Parser::FileParser::UDPParser::PrepareSelector(IO::IFileSelector *selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::LogFile)
	{
		selector->AddFilter((const UTF8Char*)"*.udp", (const UTF8Char*)"UDP Log File");
	}
}

IO::ParserType Parser::FileParser::UDPParser::GetParserType()
{
	return IO::ParserType::LogFile;
}

IO::ParsedObject *Parser::FileParser::UDPParser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType)
{
	UInt8 buff[256];
	if (!fd->GetFullName()->EndsWithICase((const UTF8Char*)"UDP"))
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
