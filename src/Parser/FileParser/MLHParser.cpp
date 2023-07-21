#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "IO/PackageFile.h"
#include "Parser/FileParser/MLHParser.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"

Parser::FileParser::MLHParser::MLHParser()
{
}

Parser::FileParser::MLHParser::~MLHParser()
{
}

Int32 Parser::FileParser::MLHParser::GetName()
{
	return *(Int32*)"MLHP";
}

void Parser::FileParser::MLHParser::PrepareSelector(IO::FileSelector *selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::PackageFile)
	{
//		selector->AddFilter(L"*.md5", L"MD5 File");
	}
}

IO::ParserType Parser::FileParser::MLHParser::GetParserType()
{
	return IO::ParserType::PackageFile;
}

typedef struct
{
	UInt8 fileName[16];
	UInt32 storeSize;
	UInt32 decompSize;
	UInt32 startOfst;
	Int32 unk;
} MLHFileInfo;

IO::ParsedObject *Parser::FileParser::MLHParser::ParseFileHdr(NotNullPtr<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr)
{
//	UInt32 fileSize;
	UInt32 fileCnt;
	UInt32 fileOfst;
//	UInt32 isUncompress;
	UOSInt i;
	MLHFileInfo *fileInfo;
	UTF8Char sbuff[17];
	UTF8Char *sptr;

	if (!Text::StrEquals((const Char*)hdr, "MLH ENCODE 1.04  (C) MAEHASHI"))
		return 0;

//	fileSize = ReadUInt32(&hdr[32]);
	fileCnt = ReadUInt32(&hdr[36]);
	fileOfst = ReadUInt32(&hdr[40]);
//	isUncompress = ReadUInt32(&hdr[44]);

	Text::Encoding enc(932);
	IO::PackageFile *pf;
	NEW_CLASS(pf, IO::PackageFile(fd->GetFullName()));

	fileInfo = MemAlloc(MLHFileInfo, fileCnt);
	fd->GetRealData(fileOfst, fileCnt * sizeof(MLHFileInfo), Data::ByteArray((UInt8*)fileInfo, fileCnt * sizeof(MLHFileInfo)));
	i = 0;
	while (i < fileCnt)
	{
		sptr = enc.UTF8FromBytes(sbuff, fileInfo[i].fileName, 16, 0);
		if (fileInfo[i].storeSize == fileInfo[i].decompSize)
		{
			pf->AddData(fd, fileInfo[i].startOfst, fileInfo[i].decompSize, CSTRP(sbuff, sptr), Data::Timestamp(0));
		}
		else
		{
			IO::PackFileItem::CompressInfo compInfo;
			compInfo.decSize = fileInfo[i].decompSize;
			compInfo.compMethod = Data::Compress::Decompressor::CM_MLH;
			compInfo.compFlags = 0;
			compInfo.compExtraSize = 0;
			compInfo.compExtras = 0;
			compInfo.checkMethod = Crypto::Hash::HashType::Unknown;
			pf->AddCompData(fd, fileInfo[i].startOfst, fileInfo[i].storeSize, &compInfo, CSTRP(sbuff, sptr), Data::Timestamp(0));
		}
		i++;
	}
	MemFree(fileInfo);

	return pf;
}
