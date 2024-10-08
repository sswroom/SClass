#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "IO/StreamReader.h"
#include "IO/StreamDataStream.h"
#include "IO/Path.h"
#include "IO/FileCheck.h"
#include "Parser/FileParser/MD5Parser.h"

Parser::FileParser::MD5Parser::MD5Parser()
{
	this->codePage = 0;
}

Parser::FileParser::MD5Parser::~MD5Parser()
{
}

Int32 Parser::FileParser::MD5Parser::GetName()
{
	return *(Int32*)"MD5P";
}

void Parser::FileParser::MD5Parser::SetCodePage(UInt32 codePage)
{
	this->codePage = codePage;
}

void Parser::FileParser::MD5Parser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::FileCheck)
	{
		selector->AddFilter(CSTR("*.md5"), CSTR("MD5 File"));
	}
}

IO::ParserType Parser::FileParser::MD5Parser::GetParserType()
{
	return IO::ParserType::FileCheck;
}

Optional<IO::ParsedObject> Parser::FileParser::MD5Parser::ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	IO::FileCheck *fchk;
	UInt8 chk[20];
	Crypto::Hash::HashType ctype;
	UOSInt chkSize;
	NN<Text::String> fullName = fd->GetFullName();

	if (fullName->EndsWithICase(UTF8STRC(".MD5")))
	{
		ctype = Crypto::Hash::HashType::MD5;
		chkSize = 16;
	}
	else if (fullName->EndsWithICase(UTF8STRC(".MD4")))
	{
		ctype = Crypto::Hash::HashType::MD4;
		chkSize = 16;
	}
	else if (fullName->EndsWithICase(UTF8STRC(".SHA1")))
	{
		ctype = Crypto::Hash::HashType::SHA1;
		chkSize = 20;
	}
	else if (fullName->EndsWithICase(UTF8STRC(".MD5SUM")))
	{
		ctype = Crypto::Hash::HashType::MD5;
		chkSize = 16;
	}
	else
	{
		return 0;
	}
	IO::StreamDataStream stm(fd);
	IO::StreamReader reader(stm, this->codePage);
	NEW_CLASS(fchk, IO::FileCheck(fullName, ctype));
	while (reader.ReadLine(sbuff, 512).SetTo(sptr))
	{
		if (sptr - sbuff > (OSInt)(chkSize << 1) + 2)
		{
			if (sbuff[(chkSize << 1)] == ' ' && sbuff[(chkSize << 1) + 1] == '*') //binary mode
			{
				sbuff[chkSize << 1] = 0;
				if (Text::StrHex2Bytes(sbuff, chk) == chkSize)
				{
					fchk->AddEntry(CSTRP(&sbuff[(chkSize << 1) + 2], sptr), chk);
				}
			}
			else if (sbuff[(chkSize << 1)] == ' ' && sbuff[(chkSize << 1) + 1] == ' ') //text mode
			{
				sbuff[chkSize << 1] = 0;
				if (Text::StrHex2Bytes(sbuff, chk) == chkSize)
				{
					fchk->AddEntry(CSTRP(&sbuff[(chkSize << 1) + 2], sptr), chk);
				}
			}
		}
		else
		{
		}
	}
	return fchk;
}
