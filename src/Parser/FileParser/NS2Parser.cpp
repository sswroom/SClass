#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteBuffer.h"
#include "Data/ByteTool.h"
#include "IO/VirtualPackageFileFast.h"
#include "IO/StmData/ConcatStreamData.h"
#include "IO/StmData/FileData.h"
#include "Parser/FileParser/NS2Parser.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"

Parser::FileParser::NS2Parser::NS2Parser()
{
}

Parser::FileParser::NS2Parser::~NS2Parser()
{
}

Int32 Parser::FileParser::NS2Parser::GetName()
{
	return *(Int32*)"NS2P";
}

void Parser::FileParser::NS2Parser::PrepareSelector(NotNullPtr<IO::FileSelector> selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::PackageFile)
	{
		selector->AddFilter(CSTR("*.ns2"), CSTR("NS2 Package File"));
	}
}

IO::ParserType Parser::FileParser::NS2Parser::GetParserType()
{
	return IO::ParserType::PackageFile;
}

IO::ParsedObject *Parser::FileParser::NS2Parser::ParseFileHdr(NotNullPtr<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr)
{
	UInt32 hdrSize;
	UInt64 fileOfst;
	UInt32 fileSize;
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	UOSInt i;
	UOSInt j;

	if (hdr[4] != '"')
		return 0;
	i = 5;
	while (i < 32)
	{
		if (hdr[i] == '"')
			break;
		i++;
	}
	if (i >= 32)
		return 0;
	if (hdr[i + 5] != '"')
		return 0;

	IO::VirtualPackageFile *pf;
	Text::Encoding enc(932);
	hdrSize = ReadUInt32(&hdr[0]);
	Data::ByteBuffer hdrBuff(hdrSize);
	fd->GetRealData(0, hdrSize, hdrBuff);
	NEW_CLASS(pf, IO::VirtualPackageFileFast(fd->GetFullName()));
	
	i = 4;
	fileOfst = hdrSize;
	while ((UInt32)i < hdrSize)
	{
		if (hdrBuff[i] != '"')
			break;

		i++;
		j = i;
		while ((UInt32)i < hdrSize)
		{
			if (hdrBuff[i] == '"')
				break;
			i++;
		}
		if ((UInt32)i >= hdrSize - 4)
			break;
		sptr = enc.UTF8FromBytes(sbuff, &hdrBuff[j], i - j, 0);
		fileSize = ReadUInt32(&hdrBuff[i + 1]);
		pf->AddData(fd, fileOfst, fileSize, IO::PackFileItem::HeaderType::No, 0, CSTRP(sbuff, sptr), 0, 0, 0, 0);
		fileOfst += fileSize;
		i += 5;
	}
	return pf;
}
