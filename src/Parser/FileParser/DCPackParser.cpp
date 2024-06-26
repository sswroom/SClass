#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "IO/VirtualPackageFileFast.h"
#include "Parser/FileParser/DCPackParser.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"

Parser::FileParser::DCPackParser::DCPackParser()
{
}

Parser::FileParser::DCPackParser::~DCPackParser()
{
}

Int32 Parser::FileParser::DCPackParser::GetName()
{
	return *(Int32*)"DCPP";
}

void Parser::FileParser::DCPackParser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::PackageFile)
	{
		selector->AddFilter(CSTR("*.pac"), CSTR("PAC Package File"));
	}
}

IO::ParserType Parser::FileParser::DCPackParser::GetParserType()
{
	return IO::ParserType::PackageFile;
}

Optional<IO::ParsedObject> Parser::FileParser::DCPackParser::ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr)
{
	UInt32 hdrEnd;
	UInt32 hdrOfst;
	UInt32 fileOfst;
	UInt32 thisOfst;
	UInt32 thisSize;

	UTF8Char name[49];
	UnsafeArray<UTF8Char> sptr;

	if (ReadInt32(&hdr[0]) != 0x41544144 || ReadInt32(&hdr[4]) != 0x504f5424)
	{
		return 0;
	}

	hdrEnd = ReadUInt32(&hdr[56]) << 6;
	fileOfst = 0;
	hdrOfst = 64;
	IO::VirtualPackageFile *pf;
	UInt8 buff[64];
	Text::Encoding enc(932);
	NEW_CLASS(pf, IO::VirtualPackageFileFast(fd->GetFullName()));

	while (hdrOfst < hdrEnd)
	{
		fd->GetRealData(hdrOfst, 64, BYTEARR(buff));
		thisOfst = ReadUInt32(&buff[48]);
		thisSize = ReadUInt32(&buff[56]);
		if (thisOfst != fileOfst || thisOfst != ReadUInt32(&buff[52]))
		{
			DEL_CLASS(pf);
			return 0;
		}
		sptr = enc.UTF8FromBytes(name, buff, 48, 0);
		pf->AddData(fd, thisOfst + hdrEnd, thisSize, IO::PackFileItem::HeaderType::No, CSTRP(name, sptr), 0, 0, 0, 0);

		fileOfst += thisSize;
		hdrOfst += 64;
	}
	return pf;
}
