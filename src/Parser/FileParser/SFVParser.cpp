#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/FileCheck.h"
#include "IO/Path.h"
#include "IO/StreamDataStream.h"
#include "IO/StreamReader.h"
#include "Parser/FileParser/SFVParser.h"
#include "Text/MyString.h"

Parser::FileParser::SFVParser::SFVParser()
{
}

Parser::FileParser::SFVParser::~SFVParser()
{
}

Int32 Parser::FileParser::SFVParser::GetName()
{
	return *(Int32*)"SFVP";
}

void Parser::FileParser::SFVParser::PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t)
{
	if (t == IO::ParsedObject::PT_UNKNOWN || t == IO::ParsedObject::PT_FILE_CHECK)
	{
		selector->AddFilter((const UTF8Char*)"*.sfv", (const UTF8Char*)"SFV File");
	}
}

IO::ParsedObject::ParserType Parser::FileParser::SFVParser::GetParserType()
{
	return IO::ParsedObject::PT_FILE_CHECK;
}

IO::ParsedObject *Parser::FileParser::SFVParser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType)
{
	UTF8Char u8buff[512];
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	IO::FileCheck *fchk;
	UOSInt i;
	IO::FileCheck::CheckType ctype;
	UInt8 chk[8];
	UOSInt chkSize;

	Text::StrConcat(u8buff, fd->GetFullName());
	i = Text::StrLastIndexOf(u8buff, '.');
	if (i != INVALID_INDEX && Text::StrEqualsICase(&u8buff[i], (const UTF8Char*)".SFV"))
	{
		ctype = IO::FileCheck::CheckType::CRC32;
		chkSize = 4;
	}
	else
	{
		return 0;
	}
	IO::StreamDataStream *stm;
	IO::StreamReader *reader;
	NEW_CLASS(stm, IO::StreamDataStream(fd));
	NEW_CLASS(reader, IO::StreamReader(stm, 65001));
	NEW_CLASS(fchk, IO::FileCheck(fd->GetFullName(), ctype));
	while ((sptr = reader->ReadLine(sbuff, 512)) != 0)
	{
		if (sptr - sbuff > (OSInt)(chkSize << 1) + 2)
		{
			if (sbuff[0] == ';')
			{
			}
			else if (sptr[-(OSInt)(chkSize << 1) - 1] == ' ')
			{
				sptr[-(OSInt)(chkSize << 1) - 1] = 0;
				if (Text::StrHex2Bytes(&sptr[-(OSInt)(chkSize << 1)], chk) == chkSize)
				{
					fchk->AddEntry(sbuff, chk);
				}
			}
		}
		else
		{
		}
	}
	DEL_CLASS(reader);
	DEL_CLASS(stm);
	return fchk;
}
