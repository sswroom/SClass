#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteBuffer.h"
#include "IO/VirtualPackageFileFast.h"
#include "Parser/FileParser/X13Parser.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"

Parser::FileParser::X13Parser::X13Parser()
{
}

Parser::FileParser::X13Parser::~X13Parser()
{
}

Int32 Parser::FileParser::X13Parser::GetName()
{
	//From Hooligans
	return *(Int32*)"X13P";
}

void Parser::FileParser::X13Parser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::MapLayer)
	{
		selector->AddFilter(CSTR("*.x13"), CSTR("X13 (Hooligans) Package File"));
	}
}

IO::ParserType Parser::FileParser::X13Parser::GetParserType()
{
	return IO::ParserType::PackageFile;
}

Optional<IO::ParsedObject> Parser::FileParser::X13Parser::ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr)
{
	UOSInt buffOfst;
	UTF8Char name[49];
	UnsafeArray<UTF8Char> sptr;

	if (ReadUInt32(&hdr[0]) != 0x4B434150 || (ReadUInt32(&hdr[4]) + ReadUInt32(&hdr[8])) != fd->GetDataSize() || (ReadUInt32(&hdr[8]) & 63) != 0)
	{
		return 0;
	}

	IO::VirtualPackageFile *pf;
	Text::Encoding enc;
	Data::ByteBuffer recHdrs(ReadUInt32(&hdr[8]));
	fd->GetRealData(ReadUInt32(&hdr[4]), ReadUInt32(&hdr[8]), recHdrs);
	NEW_CLASS(pf, IO::VirtualPackageFileFast(fd->GetFullName()));
	buffOfst = 0;
	while (buffOfst < ReadUInt32(&hdr[8]))
	{
		sptr = enc.UTF8FromBytes(name, &recHdrs[buffOfst], 48, 0);
		pf->AddData(fd, ReadUInt32(&recHdrs[buffOfst + 56]), ReadUInt32(&recHdrs[buffOfst + 60]), IO::PackFileItem::HeaderType::No, CSTRP(name, sptr), 0, 0, 0, 0);
		buffOfst += 64;
	}
	return pf;
}
