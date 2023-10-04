#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/TSPFile.h"
#include "Parser/FileParser/TSPParser.h"

Parser::FileParser::TSPParser::TSPParser()
{
}

Parser::FileParser::TSPParser::~TSPParser()
{
}

Int32 Parser::FileParser::TSPParser::GetName()
{
	return *(Int32*)"TSPP";
}

void Parser::FileParser::TSPParser::PrepareSelector(NotNullPtr<IO::FileSelector> selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::ReadingDB)
	{
		selector->AddFilter(CSTR("*.tsp"), CSTR("Total Station Point File"));
	}
}

IO::ParserType Parser::FileParser::TSPParser::GetParserType()
{
	return IO::ParserType::ReadingDB;
}

IO::ParsedObject *Parser::FileParser::TSPParser::ParseFileHdr(NotNullPtr<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr)
{
	if (*(Int64*)hdr != *(Int64*)"SmTS____" && *(Int64*)hdr != *(Int64*)"SmTS___A" && *(Int64*)hdr != *(Int64*)"SmTS___B")
	{
		return 0;
	}
	Math::TSPFile *pf;
	NEW_CLASS(pf, Math::TSPFile(fd));

	return pf;
}
