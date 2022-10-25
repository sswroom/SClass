#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "IO/PackageFile.h"
#include "Parser/FileParser/NFPParser.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"

Parser::FileParser::NFPParser::NFPParser()
{
}

Parser::FileParser::NFPParser::~NFPParser()
{
}

Int32 Parser::FileParser::NFPParser::GetName()
{
	return *(Int32*)"NFPP";
}

void Parser::FileParser::NFPParser::PrepareSelector(IO::IFileSelector *selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::PackageFile)
	{
		selector->AddFilter(CSTR("*.nfp"), CSTR("NFP Package File"));
	}
}

IO::ParserType Parser::FileParser::NFPParser::GetParserType()
{
	return IO::ParserType::PackageFile;
}

typedef struct
{
	UInt8 fileName[12];
	UInt32 fileSize;
	Int64 reserved;
	UInt32 startOfst;
	UInt32 storeSize;
} NFPFileInfo;

IO::ParsedObject *Parser::FileParser::NFPParser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType)
{
	UInt8 hdr[64];
	UInt32 fileCnt;
	UInt32 fileOfst;
	UOSInt i;
	NFPFileInfo *fileInfo;
	UTF8Char sbuff[13];
	UTF8Char *sptr;

	fd->GetRealData(0, 64, hdr);
	if (!Text::StrStartsWithC(hdr, 64, UTF8STRC("NFP2.0 (c)NOBORI 1997-2002")))
		return 0;

	fileCnt = ReadUInt32(&hdr[52]);
	fileOfst = ReadUInt32(&hdr[56]);
//	dataOfst = ReadInt32(&hdr[60]);
	Text::Encoding enc(932);
	IO::PackageFile *pf;
	NEW_CLASS(pf, IO::PackageFile(fd->GetFullName()));

	fileInfo = MemAlloc(NFPFileInfo, fileCnt);
	fd->GetRealData(fileOfst, fileCnt * sizeof(NFPFileInfo), (UInt8*)fileInfo);
	i = 0;
	while (i < fileCnt)
	{
		sptr = enc.UTF8FromBytes(sbuff, fileInfo[i].fileName, 12, 0);
		pf->AddData(fd, fileInfo[i].startOfst, fileInfo[i].fileSize, CSTRP(sbuff, sptr), Data::Timestamp(0, 0));
		i++;
	}
	MemFree(fileInfo);

	return pf;
}
