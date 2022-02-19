#include "Stdafx.h"
#include "MyMemory.h"
#include "Parser/FileParser/MRGParser.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"
#include "IO/PackageFile.h"

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

void Parser::FileParser::MRGParser::PrepareSelector(IO::IFileSelector *selector, IO::ParserType t)
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

IO::ParsedObject *Parser::FileParser::MRGParser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType)
{
	UInt32 hdr[4];
	UInt32 startOfst;
	UInt64 currOfst;
	UInt32 currSize;
	UInt32 hdrOfst;
	UInt8 rec[76];
	UTF8Char name[65];

	fd->GetRealData(0, 16, (UInt8*)hdr);
	if (hdr[0] != 0x3067726D || hdr[1] != 0x31)
	{
		return 0;
	}

	startOfst = hdr[2];
	currOfst = startOfst;
	hdrOfst = 16;
	Text::Encoding enc(932);
	IO::PackageFile *pf;
	NEW_CLASS(pf, IO::PackageFile(fd->GetFullName()));

	while (hdrOfst < startOfst)
	{
		fd->GetRealData(hdrOfst, 76, rec);
		if (*(Int32*)&rec[68] != 0 || *(Int32*)&rec[72] != 0)
		{
			DEL_CLASS(pf);
			return 0;
		}
		enc.UTF8FromBytes(name, rec, 64, 0);
		currSize = *(UInt32*)&rec[64];
		pf->AddData(fd, currOfst, currSize, name, 0);
		currOfst += currSize;
		hdrOfst += 76;
	}

	return pf;
}
