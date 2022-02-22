#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "IO/PackageFile.h"
#include "Parser/FileParser/SM2MPXParser.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"

Parser::FileParser::SM2MPXParser::SM2MPXParser()
{
}

Parser::FileParser::SM2MPXParser::~SM2MPXParser()
{
}

Int32 Parser::FileParser::SM2MPXParser::GetName()
{
	return *(Int32*)"SM2M";
}

void Parser::FileParser::SM2MPXParser::PrepareSelector(IO::IFileSelector *selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::PackageFile)
	{
		selector->AddFilter(CSTR("*.sm2"), CSTR("SM2 Package File"));
		selector->AddFilter(CSTR("*.mpx"), CSTR("MPX Package File"));
	}
}

IO::ParserType Parser::FileParser::SM2MPXParser::GetParserType()
{
	return IO::ParserType::PackageFile;
}

IO::ParsedObject *Parser::FileParser::SM2MPXParser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType)
{
	UInt8 hdr[32];
	UInt8 rec[20];

	Text::Encoding enc(932);
	UTF8Char name[13];
	UTF8Char *sptr;

	fd->GetRealData(0, 32, hdr);
	if (ReadUInt32(&hdr[0]) != 0x4D324D53 || ReadUInt32(&hdr[4]) != 0x30315850)
	{
		return 0;
	}

	UInt32 cnt = ReadUInt32(&hdr[8]);
	UInt32 endOfst = ReadUInt32(&hdr[12]);
	UInt32 hdrOfst = 32;
	UInt32 lastOfst;
	if (cnt * 20 != endOfst - 32 || ReadUInt32(&hdr[28]) != 32)
	{
		return 0;
	}

	UInt32 thisOfst;
	UInt32 thisSize;
	lastOfst = endOfst;
	IO::PackageFile *pf;
	NEW_CLASS(pf, IO::PackageFile(fd->GetFullName()));
	while (hdrOfst < endOfst)
	{
		fd->GetRealData(hdrOfst, 20, rec);
		thisOfst = ReadUInt32(&rec[12]);
		thisSize = ReadUInt32(&rec[16]);
		if (thisOfst < lastOfst)
		{
			DEL_CLASS(pf);
			return 0;
		}

		sptr = enc.UTF8FromBytes(name, rec, 12, 0);
		pf->AddData(fd, thisOfst, thisSize, CSTRP(name, sptr), 0);

		lastOfst = thisOfst + thisSize;
		hdrOfst += 20;
	}

	return pf;
}
