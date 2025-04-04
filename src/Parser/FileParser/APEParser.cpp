#include "Stdafx.h"
#include "MyMemory.h"
#include "Parser/FileParser/APEParser.h"
#ifdef ENABLE_APE
#include "Media/APE/APEFile.h"
#endif

Parser::FileParser::APEParser::APEParser()
{
}

Parser::FileParser::APEParser::~APEParser()
{
}

Int32 Parser::FileParser::APEParser::GetName()
{
	return *(Int32*)"APEP";
}

void Parser::FileParser::APEParser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::MediaFile)
	{
		selector->AddFilter(CSTR("*.ape"), CSTR("Monkey's Audio File"));
	}
}

IO::ParserType Parser::FileParser::APEParser::GetParserType()
{
	return IO::ParserType::MediaFile;
}

Optional<IO::ParsedObject> Parser::FileParser::APEParser::ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr)
{
#ifdef ENABLE_APE
	if (*(Int32*)&hdr[0] != *(Int32*)"MAC ")
	{
		return 0;
	}
	
	return Media::APE::APEFile::ParseData(fd);
#else
	return 0;
#endif
}
