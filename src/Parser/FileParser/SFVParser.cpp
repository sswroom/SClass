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

void Parser::FileParser::SFVParser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
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

Optional<IO::ParsedObject> Parser::FileParser::SFVParser::ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
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
		return nullptr;
	}
	IO::StreamDataStream stm(fd);
	Text::UTF8Reader reader(stm);
	NEW_CLASS(fchk, IO::FileCheck(fd->GetFullName(), ctype));
	while (reader.ReadLine(sbuff, 512).SetTo(sptr))
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
