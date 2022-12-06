#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "IO/PackageFile.h"
#include "Parser/FileParser/MAIPackParser.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"

Parser::FileParser::MAIPackParser::MAIPackParser()
{
}

Parser::FileParser::MAIPackParser::~MAIPackParser()
{
}

Int32 Parser::FileParser::MAIPackParser::GetName()
{
	return *(Int32*)"MAIP";
}

void Parser::FileParser::MAIPackParser::PrepareSelector(IO::IFileSelector *selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::PackageFile)
	{
		selector->AddFilter(CSTR("*.mai"), CSTR("MAI Package File"));
	}
}

IO::ParserType Parser::FileParser::MAIPackParser::GetParserType()
{
	return IO::ParserType::PackageFile;
}

IO::ParsedObject *Parser::FileParser::MAIPackParser::ParseFileHdr(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr)
{
	UInt8 recbuff[24];
	UInt32 hdrEnd;
	UInt32 hdrOfst;
	UInt32 fileOfst;
	UInt32 thisOfst;
	UInt32 thisSize;

	UTF8Char name[17];
	UTF8Char *sptr;

	if (ReadUInt32(&hdr[0]) != 0x0A49414D || ReadUInt32(&hdr[4]) != fd->GetDataSize() || ReadUInt32(&hdr[12]) != 0x100)
	{
		return 0;
	}

	hdrEnd = ReadUInt32(&hdr[8]) * 24 + 16;
	fileOfst = hdrEnd;
	hdrOfst = 16;
	IO::PackageFile *pf;
	Text::Encoding enc(932);
	NEW_CLASS(pf, IO::PackageFile(fd->GetFullName()));

	while (hdrOfst < hdrEnd)
	{
		fd->GetRealData(hdrOfst, 24, recbuff);
		thisOfst = ReadUInt32(&recbuff[16]);
		thisSize = ReadUInt32(&recbuff[20]);
		if (thisOfst != fileOfst)
		{
			DEL_CLASS(pf);
			return 0;
		}
		sptr = enc.UTF8FromBytes(name, recbuff, 16, 0);
		pf->AddData(fd, thisOfst, thisSize, CSTRP(name, sptr), 0);

		fileOfst += thisSize;
		hdrOfst += 24;
	}
	return pf;
}
