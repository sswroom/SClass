#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/IStreamData.h"
#include "IO/PackageFile.h"
#include "Parser/FileParser/MMSParser.h"
#include "Text/Encoding.h"

Parser::FileParser::MMSParser::MMSParser()
{
}

Parser::FileParser::MMSParser::~MMSParser()
{
}

Int32 Parser::FileParser::MMSParser::GetName()
{
	return *(Int32*)"MMSP";
}

void Parser::FileParser::MMSParser::PrepareSelector(IO::IFileSelector *selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::PackageFile)
	{
		selector->AddFilter(CSTR("*.mms"), CSTR("MMS File"));
	}
}

IO::ParserType Parser::FileParser::MMSParser::GetParserType()
{
	return IO::ParserType::PackageFile;
}

IO::ParsedObject *Parser::FileParser::MMSParser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType)
{
	IO::PackageFile *pf;
	UInt32 fileCnt;
	UOSInt currOfst;
	UInt8 buff[256];
	UTF8Char sbuff[256];
	UInt8 *ptr;

	fd->GetRealData(0, 256, buff);
	if (buff[0] != 0x8c || buff[1] != 0x80)
		return 0;

	NEW_CLASS(pf, IO::PackageFile(fd->GetFullName()));
	ptr = &buff[2];
	while (*ptr & 0x80)
	{
		if (*ptr == 0x84)
		{
			ptr++;
			ptr++;
			ptr += *ptr;
			ptr++;
			break;
		}
		else if (*ptr == 0x89)
		{
			ptr++;
			ptr += *ptr;
			ptr++;
		}
		else
		{
			ptr++;
			if (*ptr & 0x80)
			{
				ptr++;
			}
			else
			{
				while (*ptr++);
			}
		}
	}

	fileCnt = *ptr++;
	currOfst = (UOSInt)(ptr - buff);

	while (fileCnt-- > 0)
	{
		UInt32 hdrSize;
		UInt32 dataSize;
		fd->GetRealData(currOfst, 4, buff);
		hdrSize = buff[0];
		if (buff[1] & 0x80)
		{
			dataSize = (UInt32)((buff[1] & 0x7f) << 7) | (buff[2] & 0x7f);
			currOfst += 3;
		}
		else
		{
			dataSize = buff[1];
			currOfst += 2;
		}
		fd->GetRealData(currOfst, hdrSize, buff);
		sbuff[0] = (UTF8Char)(0x41 + fileCnt);
		sbuff[1] = 0;
		ptr = buff;
		if (*ptr < hdrSize)
		{
			ptr++;
			if (*ptr & 0x80)
			{
				ptr++;
			}
			else
			{
				while (*ptr++);
			}
			if (*ptr == 0x81)
			{
				ptr += 2;
			}
			if (*ptr == 0x85)
			{
				ptr++;
				Text::StrConcat(sbuff, ptr);
			}
		}
		currOfst += hdrSize;
		pf->AddData(fd, currOfst, dataSize, sbuff, 0);
		currOfst += dataSize;
	}
	return pf;
}
