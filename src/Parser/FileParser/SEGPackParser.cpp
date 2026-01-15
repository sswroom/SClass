#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteBuffer.h"
#include "Core/ByteTool_C.h"
#include "IO/VirtualPackageFileFast.h"
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

void Parser::FileParser::SEGPackParser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
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

Optional<IO::ParsedObject> Parser::FileParser::SEGPackParser::ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr)
{
	UTF8Char name[64];
	UnsafeArray<UTF8Char> sptr;
	UInt32 buffOfst;
	UInt32 fileOfst;

	if (!fd->GetFullName()->EndsWithICase(UTF8STRC("SEG")))
		return nullptr;

	UInt32 hdrSize = ReadUInt32(&hdr[0]);
	if (hdrSize == 0 || hdrSize >= (fd->GetDataSize() - 4) || hdrSize > 1048576)
	{
		return nullptr;
	}

	IO::VirtualPackageFile *pf;
	Text::Encoding enc(932);

	NEW_CLASS(pf, IO::VirtualPackageFileFast(fd->GetFullName()));
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
			return nullptr;
		}
		if (buff[buffOfst + packSize - 1] != 0)
		{
			DEL_CLASS(pf);
			return nullptr;
		}
		if (Text::StrCharCnt(&buff[buffOfst + 16]) != packSize - 17)
		{
			DEL_CLASS(pf);
			return nullptr;
		}
		
		sptr = enc.UTF8FromBytes(name, &buff[buffOfst + 16], packSize - 17, 0);
		pf->AddData(fd, thisOfst, thisSize, IO::PackFileItem::HeaderType::No, CSTRP(name, sptr), 0, 0, 0, 0);

		fileOfst += thisSize;
		buffOfst += packSize;
	}
	return pf;
}
