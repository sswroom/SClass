#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "IO/PackageFile.h"
#include "IO/StmData/ConcatStreamData.h"
#include "IO/StmData/FileData.h"
#include "Parser/FileParser/SZSParser.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"

Parser::FileParser::SZSParser::SZSParser()
{
}

Parser::FileParser::SZSParser::~SZSParser()
{
}

Int32 Parser::FileParser::SZSParser::GetName()
{
	return *(Int32*)"SZSP";
}

void Parser::FileParser::SZSParser::PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t)
{
	if (t == IO::ParsedObject::PT_UNKNOWN || t == IO::ParsedObject::PT_PACKAGE_PARSER)
	{
		selector->AddFilter((const UTF8Char*)"*.szs", (const UTF8Char*)"SZS Package File");
	}
}

IO::ParsedObject::ParserType Parser::FileParser::SZSParser::GetParserType()
{
	return IO::ParsedObject::PT_PACKAGE_PARSER;
}

IO::ParsedObject *Parser::FileParser::SZSParser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType)
{
	UInt8 hdr[16];
	UInt8 fileBuff[272];
	Text::Encoding enc(932);
	UTF8Char u8buff[257];
	OSInt i;
	Int32 fileCnt;
	Int64 ofst;
	Int64 minOfst;
	Int64 fileOfst;
	Int64 fileSize;
	IO::PackageFile *pf = 0;

	fd->GetRealData(0, 16, hdr);
	if (!Text::StrEquals((Char*)hdr, "SZS100__"))
		return 0;
	fileCnt = ReadInt32(&hdr[12]);
	if (fileCnt <= 0)
		return 0;
	NEW_CLASS(pf, IO::PackageFile(fd->GetFullName()));
	ofst = 16;
	minOfst = 16 + 272 * fileCnt;
	i = 0;
	while (i < fileCnt)
	{
		fd->GetRealData(ofst, 272, fileBuff);

		fileOfst = ReadInt64(&fileBuff[256]);
		fileSize = ReadInt64(&fileBuff[264]);
		if (fileOfst < minOfst || fileSize < 0)
		{
			DEL_CLASS(pf);
			return 0;
		}
		enc.UTF8FromBytes(u8buff, fileBuff, 256, 0);
		Text::StrReplace(u8buff, ';', '\\');
		pf->AddData(fd, fileOfst, fileSize, u8buff, 0);

		ofst += 272;
		i++;
	}
	
	return pf;
}
