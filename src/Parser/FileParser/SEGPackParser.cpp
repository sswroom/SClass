#include "Stdafx.h"
#include "MyMemory.h"
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

void Parser::FileParser::SEGPackParser::PrepareSelector(IO::IFileSelector *selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::PackageFile)
	{
		selector->AddFilter((const UTF8Char*)"*.seg", (const UTF8Char*)"SEG Package File");
	}
}

IO::ParserType Parser::FileParser::SEGPackParser::GetParserType()
{
	return IO::ParserType::PackageFile;
}

IO::ParsedObject *Parser::FileParser::SEGPackParser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType)
{
	UInt32 hdr[1];
	UTF8Char name[64];
	UInt32 buffOfst;
	UInt32 fileOfst;
	Text::Encoding enc(932);

	if (!fd->GetFullName()->EndsWithICase((const UTF8Char*)"SEG"))
		return 0;

	fd->GetRealData(0, 4, (UInt8*)hdr);
	if (hdr[0] == 0 || hdr[0] >= (fd->GetDataSize() - 4) || hdr[0] > 1048576)
	{
		return 0;
	}

	UInt8 *buff;
	IO::PackageFile *pf;

	NEW_CLASS(pf, IO::PackageFile(fd->GetFullName()));
	buff = MemAlloc(UInt8, hdr[0]);
	fd->GetRealData(4, hdr[0], buff);
	
	buffOfst = 0;
	fileOfst = 4 + hdr[0];
	while (buffOfst < hdr[0])
	{
		UInt32 packSize = ReadUInt32(&buff[buffOfst]);
		UInt32 thisOfst = ReadUInt32(&buff[buffOfst + 8]);
		UInt32 thisSize = ReadUInt32(&buff[buffOfst + 12]);
		if (*(Int32*)&buff[buffOfst + 4] != 0 || packSize > 80 || packSize <= 17 || thisOfst != fileOfst || packSize + buffOfst > hdr[0])
		{
			MemFree(buff);
			DEL_CLASS(pf);
			return 0;
		}
		if (buff[buffOfst + packSize - 1] != 0)
		{
			MemFree(buff);
			DEL_CLASS(pf);
			return 0;
		}
		if (Text::StrCharCnt((Char*)&buff[buffOfst + 16]) != packSize - 17)
		{
			MemFree(buff);
			DEL_CLASS(pf);
			return 0;
		}
		
		enc.UTF8FromBytes(name, &buff[buffOfst + 16], packSize - 17, 0);
		pf->AddData(fd, thisOfst, thisSize, name, 0);

		fileOfst += thisSize;
		buffOfst += packSize;
	}

	MemFree(buff);
	return pf;
}
