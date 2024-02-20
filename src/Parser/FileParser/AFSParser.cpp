#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteBuffer.h"
#include "Data/ByteTool.h"
#include "IO/StreamData.h"
#include "IO/VirtualPackageFileFast.h"
#include "Parser/FileParser/AFSParser.h"
#include "Text/Encoding.h"

Parser::FileParser::AFSParser::AFSParser()
{
}

Parser::FileParser::AFSParser::~AFSParser()
{
}

Int32 Parser::FileParser::AFSParser::GetName()
{
	return *(Int32*)"AFSP";
}

void Parser::FileParser::AFSParser::PrepareSelector(NotNullPtr<IO::FileSelector> selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::PackageFile)
	{
		selector->AddFilter(CSTR("*.afs"), CSTR("AFS ADX ADPCM Package"));
	}
}

IO::ParserType Parser::FileParser::AFSParser::GetParserType()
{
	return IO::ParserType::PackageFile;
}

IO::ParsedObject *Parser::FileParser::AFSParser::ParseFileHdr(NotNullPtr<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr)
{
	IO::VirtualPackageFile *pf;
	UTF8Char sbuff[9];
	UTF8Char *namePtr;
	UInt32 fileCnt;
	UInt32 i;
	UInt32 ofst;
	UInt32 leng;

	if (*(Int32*)hdr != *(Int32*)"AFS")
		return 0;
	sbuff[0] = 'A';
	sbuff[1] = 'F';
	sbuff[2] = 'S';
	sbuff[3] = '_';
	sbuff[4] = '0';
	sbuff[5] = '0';
	sbuff[6] = '0';
	sbuff[7] = '0';
	sbuff[8] = 0;

	fileCnt = ReadUInt32(&hdr[4]);
	Data::ByteBuffer buff2(fileCnt << 3);
	NEW_CLASS(pf, IO::VirtualPackageFileFast(fd->GetFullName()));
	fd->GetRealData(8, fileCnt << 3, buff2);
	i = 0;
	while (i < fileCnt)
	{
		namePtr = &sbuff[8];
		while (++*--namePtr > '9')
		{
			*namePtr = '0';
		}

		ofst = ReadUInt32(&buff2[i << 3]);
		leng = ReadUInt32(&buff2[(i << 3) + 4]);
		if (ofst == 0 || leng == 0)
		{
			DEL_CLASS(pf);
			return 0;
		}
		pf->AddData(fd, ofst, leng, IO::PackFileItem::HeaderType::No, {sbuff, 8}, 0, 0, 0, 0);
		i++;
	}
	return pf;
}
