#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "IO/PackageFile.h"
#include "Parser/FileParser/MajiroArcParser.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"

Parser::FileParser::MajiroArcParser::MajiroArcParser()
{
}

Parser::FileParser::MajiroArcParser::~MajiroArcParser()
{
}

Int32 Parser::FileParser::MajiroArcParser::GetName()
{
	return *(Int32*)"MARC";
}

void Parser::FileParser::MajiroArcParser::PrepareSelector(IO::IFileSelector *selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::PackageFile)
	{
		selector->AddFilter(CSTR("*.arc"), CSTR("ARC Package File"));
	}
}

IO::ParserType Parser::FileParser::MajiroArcParser::GetParserType()
{
	return IO::ParserType::PackageFile;
}

typedef struct
{
	Char idChars[16];
	UInt32 fileCnt;
	UInt32 fileNameOfst;
	UInt32 fileNameEndOfst;
} ARCHeader;

typedef struct
{
	UInt8 fileCheck[8];
	UInt32 recOfst;
	UInt32 recSize;
} ARCRecord;

IO::ParsedObject *Parser::FileParser::MajiroArcParser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType)
{
	ARCHeader hdr;
	ARCRecord *recBuff;
	UInt8 *fileNameBuff;
	UInt8 *fileNamePtr;
	UInt8 *fileNamePtr2;
	UTF8Char sbuff[256];
	UInt32 i;
	Text::Encoding enc(932);

	fd->GetRealData(0, 28, (UInt8*)&hdr);
	if (!Text::StrEquals(hdr.idChars, "MajiroArcV3.000"))
	{
		return 0;
	}
	fileNameBuff = MemAlloc(UInt8, hdr.fileNameEndOfst - hdr.fileNameOfst);
	fd->GetRealData(hdr.fileNameOfst, hdr.fileNameEndOfst - hdr.fileNameOfst, fileNameBuff);
	recBuff = MemAlloc(ARCRecord, hdr.fileCnt);
	fd->GetRealData(28, sizeof(ARCRecord) * hdr.fileCnt, (UInt8*)recBuff);

	IO::PackageFile *pf;
	NEW_CLASS(pf, IO::PackageFile(fd->GetFullName()));
	fileNamePtr = fileNameBuff;
	i = 0;
	
	while (i < hdr.fileCnt)
	{
		fileNamePtr2 = fileNamePtr;
		while (*fileNamePtr2++);
		enc.UTF8FromBytes(sbuff, fileNamePtr, (UOSInt)(fileNamePtr2 - fileNamePtr - 1), 0);
		pf->AddData(fd, recBuff[i].recOfst, recBuff[i].recSize, sbuff, 0);

		fileNamePtr = fileNamePtr2;
		i++;
	}
	MemFree(fileNameBuff);
	MemFree(recBuff);
	return pf;
}
