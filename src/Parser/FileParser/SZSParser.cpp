#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "IO/VirtualPackageFileFast.h"
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

void Parser::FileParser::SZSParser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::PackageFile)
	{
		selector->AddFilter(CSTR("*.szs"), CSTR("SZS Package File"));
	}
}

IO::ParserType Parser::FileParser::SZSParser::GetParserType()
{
	return IO::ParserType::PackageFile;
}

Optional<IO::ParsedObject> Parser::FileParser::SZSParser::ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr)
{
	UInt8 fileBuff[272];
	UTF8Char sbuff[257];
	UnsafeArray<UTF8Char> sptr;
	OSInt i;
	Int32 fileCnt;
	UInt64 ofst;
	UInt64 minOfst;
	UInt64 fileOfst;
	UInt64 fileSize;
	IO::VirtualPackageFile *pf = 0;
	UInt64 fileLen = fd->GetDataSize();

	if (!Text::StrStartsWithC(&hdr[0], 16, UTF8STRC("SZS100__")))
		return 0;
	fileCnt = ReadInt32(&hdr[12]);
	if (fileCnt <= 0)
		return 0;
	Text::Encoding enc(932);
	NEW_CLASS(pf, IO::VirtualPackageFileFast(fd->GetFullName()));
	ofst = 16;
	minOfst = 16 + 272 * (UInt32)fileCnt;
	i = 0;
	while (i < fileCnt)
	{
		fd->GetRealData(ofst, 272, BYTEARR(fileBuff));

		fileOfst = ReadUInt64(&fileBuff[256]);
		fileSize = ReadUInt64(&fileBuff[264]);
		if (fileOfst < minOfst || (fileOfst + fileSize) > fileLen)
		{
			DEL_CLASS(pf);
			return 0;
		}
		sptr = enc.UTF8FromBytes(sbuff, fileBuff, 256, 0);
		Text::StrReplace(sbuff, ';', '\\');
		pf->AddData(fd, fileOfst, fileSize, IO::PackFileItem::HeaderType::No, CSTRP(sbuff, sptr), 0, 0, 0, 0);

		ofst += 272;
		i++;
	}
	
	return pf;
}
