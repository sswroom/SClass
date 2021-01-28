#include "Stdafx.h"
#include "MyMemory.h"
#include "Parser/FileParser/DCPackParser.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"
#include "IO/PackageFile.h"

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

void Parser::FileParser::DCPackParser::PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t)
{
	if (t == IO::ParsedObject::PT_UNKNOWN || t == IO::ParsedObject::PT_PACKAGE_PARSER)
	{
		selector->AddFilter((const UTF8Char*)"*.pac", (const UTF8Char*)"PAC Package File");
	}
}

IO::ParsedObject::ParserType Parser::FileParser::DCPackParser::GetParserType()
{
	return IO::ParsedObject::PT_PACKAGE_PARSER;
}

IO::ParsedObject *Parser::FileParser::DCPackParser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType)
{
	UInt8 buff[64];
	UInt32 hdrEnd;
	UInt32 hdrOfst;
	UInt32 fileOfst;
	UInt32 thisOfst;
	UInt32 thisSize;

	UTF8Char name[49];
	Text::Encoding enc(932);

	fd->GetRealData(0, 64, buff);
	if (*(Int32*)&buff[0] != 0x41544144 || *(Int32*)&buff[4] != 0x504f5424)
	{
		return 0;
	}

	hdrEnd = *(Int32*)&buff[56] << 6;
	fileOfst = 0;
	hdrOfst = 64;
	IO::PackageFile *pf;
	NEW_CLASS(pf, IO::PackageFile(fd->GetFullName()));

	while (hdrOfst < hdrEnd)
	{
		fd->GetRealData(hdrOfst, 64, buff);
		thisOfst = *(Int32*)&buff[48];
		thisSize = *(Int32*)&buff[56];
		if (thisOfst != fileOfst || thisOfst != *(UInt32*)&buff[52])
		{
			DEL_CLASS(pf);
			return 0;
		}
		enc.UTF8FromBytes(name, buff, 48, 0);
		pf->AddData(fd, thisOfst + hdrEnd, thisSize, name, 0);

		fileOfst += thisSize;
		hdrOfst += 64;
	}
	return pf;
}
