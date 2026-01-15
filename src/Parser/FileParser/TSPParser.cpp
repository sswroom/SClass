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

void Parser::FileParser::TSPParser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
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

Optional<IO::ParsedObject> Parser::FileParser::TSPParser::ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr)
{
	if (hdr.ReadNI64(0) != *(Int64*)"SmTS____" && hdr.ReadNI64(0) != *(Int64*)"SmTS___A" && hdr.ReadNI64(0) != *(Int64*)"SmTS___B")
	{
		return nullptr;
	}
	Math::TSPFile *pf;
	NEW_CLASS(pf, Math::TSPFile(fd));

	return pf;
}
