#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteBuffer.h"
#include "Data/ByteTool.h"
#include "IO/PackageFile.h"
#include "Parser/FileParser/SEGPackParser.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"

Parser::FileParser::SEGPackParser::SEGPackParser()
{
}

Parser::FileParser::SEGPackParser::~SEGPackParser()
{
}

Int32 Parser::FileParser::SEGPackParser::GetName()
{
	return *(Int32*)"SEGP";
}

void Parser::FileParser::SEGPackParser::PrepareSelector(NotNullPtr<IO::FileSelector> selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::PackageFile)
	{
		selector->AddFilter(CSTR("*.seg"), CSTR("SEG Package File"));
	}
}

IO::ParserType Parser::FileParser::SEGPackParser::GetParserType()
{
	return IO::ParserType::PackageFile;
}

IO::ParsedObject *Parser::FileParser::SEGPackParser::ParseFileHdr(NotNullPtr<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr)
{
	UTF8Char name[64];
	UTF8Char *sptr;
	UInt32 buffOfst;
	UInt32 fileOfst;

	if (!fd->GetFullName()->EndsWithICase(UTF8STRC("SEG")))
		return 0;

	UInt32 hdrSize = ReadUInt32(&hdr[0]);
	if (hdrSize == 0 || hdrSize >= (fd->GetDataSize() - 4) || hdrSize > 1048576)
	{
		return 0;
	}

	IO::PackageFile *pf;
	Text::Encoding enc(932);

	NEW_CLASS(pf, IO::PackageFile(fd->GetFullName()));
	Data::ByteBuffer buff(hdrSize);
	fd->GetRealData(4, hdrSize, buff);
	
	buffOfst = 0;
	fileOfst = 4 + hdrSize;
	while (buffOfst < hdrSize)
	{
		UInt32 packSize = ReadUInt32(&buff[buffOfst]);
		UInt32 thisOfst = ReadUInt32(&buff[buffOfst + 8]);
		UInt32 thisSize = ReadUInt32(&buff[buffOfst + 12]);
		if (*(Int32*)&buff[buffOfst + 4] != 0 || packSize > 80 || packSize <= 17 || thisOfst != fileOfst || packSize + buffOfst > hdrSize)
		{
			DEL_CLASS(pf);
			return 0;
		}
		if (buff[buffOfst + packSize - 1] != 0)
		{
			DEL_CLASS(pf);
			return 0;
		}
		if (Text::StrCharCnt((Char*)&buff[buffOfst + 16]) != packSize - 17)
		{
			DEL_CLASS(pf);
			return 0;
		}
		
		sptr = enc.UTF8FromBytes(name, &buff[buffOfst + 16], packSize - 17, 0);
		pf->AddData(fd, thisOfst, thisSize, CSTRP(name, sptr), 0, 0, 0);

		fileOfst += thisSize;
		buffOfst += packSize;
	}
	return pf;
}
