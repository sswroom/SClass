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

void Parser::FileParser::UDPParser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::LogFile)
	{
		selector->AddFilter(CSTR("*.udp"), CSTR("UDP Log File"));
	}
}

IO::ParserType Parser::FileParser::UDPParser::GetParserType()
{
	return IO::ParserType::LogFile;
}

Optional<IO::ParsedObject> Parser::FileParser::UDPParser::ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr)
{
	if (!fd->GetFullName()->EndsWithICase(UTF8STRC("UDP")))
		return 0;

	if (hdr[0] == 0xaa && hdr[1] == 0xbb)
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
