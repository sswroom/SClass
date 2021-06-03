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

void Parser::FileParser::MLHParser::PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t)
{
	if (t == IO::ParsedObject::PT_UNKNOWN || t == IO::ParsedObject::PT_PACKAGE_PARSER)
	{
//		selector->AddFilter(L"*.md5", L"MD5 File");
	}
}

IO::ParsedObject::ParserType Parser::FileParser::MLHParser::GetParserType()
{
	return IO::ParsedObject::PT_PACKAGE_PARSER;
}

typedef struct
{
	UInt8 fileName[16];
	UInt32 storeSize;
	UInt32 decompSize;
	Int32 startOfst;
	Int32 unk;
} MLHFileInfo;

IO::ParsedObject *Parser::FileParser::MLHParser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType)
{
	UInt8 hdr[64];
//	UInt32 fileSize;
	UInt32 fileCnt;
	UInt32 fileOfst;
//	UInt32 isUncompress;
	UOSInt i;
	MLHFileInfo *fileInfo;
	UTF8Char sbuff[17];

	fd->GetRealData(0, 64, hdr);
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
	fd->GetRealData(fileOfst, fileCnt * sizeof(MLHFileInfo), (UInt8*)fileInfo);
	i = 0;
	while (i < fileCnt)
	{
		enc.UTF8FromBytes(sbuff, fileInfo[i].fileName, 16, 0);
		if (fileInfo[i].storeSize == fileInfo[i].decompSize)
		{
			pf->AddData(fd, fileInfo[i].startOfst, fileInfo[i].decompSize, sbuff, 0);
		}
		else
		{
			IO::PackFileItem::CompressInfo compInfo;
			compInfo.decSize = fileInfo[i].decompSize;
			compInfo.compMethod = Data::Compress::Decompressor::CM_MLH;
			compInfo.compFlags = 0;
			compInfo.compExtraSize = 0;
			compInfo.compExtras = 0;
			compInfo.checkMethod = Crypto::Hash::HT_UNKNOWN;
			pf->AddCompData(fd, fileInfo[i].startOfst, fileInfo[i].storeSize, &compInfo, sbuff, 0);
		}
		i++;
	}
	MemFree(fileInfo);

	return pf;
}
