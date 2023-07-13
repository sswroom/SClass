#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/FileCheck.h"
#include "IO/Path.h"
#include "IO/StreamDataStream.h"
#include "Parser/FileParser/SFVParser.h"
#include "Text/MyString.h"
#include "Text/UTF8Reader.h"

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

void Parser::FileParser::SFVParser::PrepareSelector(IO::FileSelector *selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::FileCheck)
	{
		selector->AddFilter(CSTR("*.sfv"), CSTR("SFV File"));
	}
}

IO::ParserType Parser::FileParser::SFVParser::GetParserType()
{
	return IO::ParserType::FileCheck;
}

IO::ParsedObject *Parser::FileParser::SFVParser::ParseFileHdr(IO::StreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	IO::FileCheck *fchk;
	Crypto::Hash::HashType ctype;
	UInt8 chk[8];
	UOSInt chkSize;

	if (fd->GetFullName()->EndsWithICase(UTF8STRC(".SFV")))
	{
		ctype = Crypto::Hash::HashType::CRC32;
		chkSize = 4;
	}
	else
	{
		return 0;
	}
	IO::StreamDataStream stm(fd);
	Text::UTF8Reader reader(stm);
	NEW_CLASS(fchk, IO::FileCheck(fd->GetFullName(), ctype));
	while ((sptr = reader.ReadLine(sbuff, 512)) != 0)
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
					fchk->AddEntry(CSTRP(sbuff, &sptr[-(OSInt)(chkSize << 1) - 1]), chk);
				}
			}
		}
		else
		{
		}
	}
	return fchk;
}
