#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/VirtualPackageFileFast.h"
#include "Parser/FileParser/MRGParser.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"

Parser::FileParser::MRGParser::MRGParser()
{
}

Parser::FileParser::MRGParser::~MRGParser()
{
}

Int32 Parser::FileParser::MRGParser::GetName()
{
	return *(Int32*)"MRGP";
}

void Parser::FileParser::MRGParser::PrepareSelector(NotNullPtr<IO::FileSelector> selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::PackageFile)
	{
		selector->AddFilter(CSTR("*.mrg"), CSTR("MRG Package File"));
	}
}

IO::ParserType Parser::FileParser::MRGParser::GetParserType()
{
	return IO::ParserType::PackageFile;
}

IO::ParsedObject *Parser::FileParser::MRGParser::ParseFileHdr(NotNullPtr<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr)
{
	UInt32 startOfst;
	UInt64 currOfst;
	UInt32 currSize;
	UInt32 hdrOfst;
	UInt8 rec[76];
	UTF8Char name[65];
	UTF8Char *sptr;

	if (ReadUInt32(&hdr[0]) != 0x3067726D || ReadUInt32(&hdr[4]) != 0x31)
	{
		return 0;
	}

	startOfst = ReadUInt32(&hdr[8]);
	currOfst = startOfst;
	hdrOfst = 16;
	Text::Encoding enc(932);
	IO::VirtualPackageFile *pf;
	NEW_CLASS(pf, IO::VirtualPackageFileFast(fd->GetFullName()));

	while (hdrOfst < startOfst)
	{
		fd->GetRealData(hdrOfst, 76, BYTEARR(rec));
		if (*(Int32*)&rec[68] != 0 || *(Int32*)&rec[72] != 0)
		{
			DEL_CLASS(pf);
			return 0;
		}
		sptr = enc.UTF8FromBytes(name, rec, 64, 0);
		currSize = *(UInt32*)&rec[64];
		pf->AddData(fd, currOfst, currSize, IO::PackFileItem::HeaderType::No, CSTRP(name, sptr), 0, 0, 0, 0);
		currOfst += currSize;
		hdrOfst += 76;
	}

	return pf;
}
