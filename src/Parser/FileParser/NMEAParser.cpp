#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/GPSNMEA.h"
#include "IO/StreamDataStream.h"
#include "Parser/FileParser/NMEAParser.h"

Parser::FileParser::NMEAParser::NMEAParser()
{
}

Parser::FileParser::NMEAParser::~NMEAParser()
{
}

Int32 Parser::FileParser::NMEAParser::GetName()
{
	return *(Int32*)"NMEA";
}

void Parser::FileParser::NMEAParser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::MapLayer)
	{
		selector->AddFilter(CSTR("*.nmea"), CSTR("NMEA File"));
		selector->AddFilter(CSTR("*.log"), CSTR("Olympus GPS Log"));
	}
}

IO::ParserType Parser::FileParser::NMEAParser::GetParserType()
{
	return IO::ParserType::MapLayer;
}

Optional<IO::ParsedObject> Parser::FileParser::NMEAParser::ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr)
{
	if (fd->GetFullName()->EndsWithICase(UTF8STRC(".LOG")))
	{
		if (!Text::StrStartsWithC(&hdr[0], 32, UTF8STRC("@Olympus/")) || hdr[20] != 13 || hdr[21] != 10)
			return 0;
	}
	else
	{
		return 0;
	}

	IO::StreamDataStream stm(fd, 22, fd->GetDataSize() - 22);
	return IO::GPSNMEA::NMEA2Track(stm, fd->GetFullName()->ToCString());
}
