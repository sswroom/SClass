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

void Parser::FileParser::MD5Parser::SetCodePage(Int32 codePage)
{
	this->codePage = codePage;
}

void Parser::FileParser::MD5Parser::PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t)
{
	if (t == IO::ParsedObject::PT_UNKNOWN || t == IO::ParsedObject::PT_FILE_CHECK)
	{
		selector->AddFilter((const UTF8Char*)"*.md5", (const UTF8Char*)"MD5 File");
	}
}

IO::ParsedObject::ParserType Parser::FileParser::MD5Parser::GetParserType()
{
	return IO::ParsedObject::PT_FILE_CHECK;
}

IO::ParsedObject *Parser::FileParser::MD5Parser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType)
{
	UTF8Char u8buff[512];
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	IO::FileCheck *fchk;
	OSInt i;
	UInt8 chk[20];
	IO::FileCheck::CheckType ctype;
	UOSInt chkSize;

	if (fd->GetFullName() == 0)
	{
		return 0;
	}
	Text::StrConcat(u8buff, fd->GetFullName());
	i = Text::StrLastIndexOf(u8buff, '.');
	if (Text::StrCompareICase(&u8buff[i], (const UTF8Char*)".MD5") == 0)
	{
		ctype = IO::FileCheck::CT_MD5;
		chkSize = 16;
	}
	else if (Text::StrCompareICase(&u8buff[i], (const UTF8Char*)".MD4") == 0)
	{
		ctype = IO::FileCheck::CT_MD4;
		chkSize = 16;
	}
	else if (Text::StrCompareICase(&u8buff[i], (const UTF8Char*)".SHA1") == 0)
	{
		ctype = IO::FileCheck::CT_SHA1;
		chkSize = 20;
	}
	else if (Text::StrCompareICase(&u8buff[i], (const UTF8Char*)".MD5SUM") == 0)
	{
		ctype = IO::FileCheck::CT_MD5;
		chkSize = 16;
	}
	else
	{
		return 0;
	}
	IO::StreamDataStream *stm;
	IO::StreamReader *reader;
	NEW_CLASS(stm, IO::StreamDataStream(fd));
	NEW_CLASS(reader, IO::StreamReader(stm, this->codePage));
	NEW_CLASS(fchk, IO::FileCheck(fd->GetFullName(), ctype));
	while ((sptr = reader->ReadLine(sbuff, 512)) != 0)
	{
		if (sptr - sbuff > (OSInt)(chkSize << 1) + 2)
		{
			if (sbuff[(chkSize << 1)] == ' ' && sbuff[(chkSize << 1) + 1] == '*') //binary mode
			{
				sbuff[chkSize << 1] = 0;
				if (Text::StrHex2Bytes(sbuff, chk) == chkSize)
				{
					fchk->AddEntry(&sbuff[(chkSize << 1) + 2], chk);
				}
			}
			else if (sbuff[(chkSize << 1)] == ' ' && sbuff[(chkSize << 1) + 1] == ' ') //text mode
			{
				sbuff[chkSize << 1] = 0;
				if (Text::StrHex2Bytes(sbuff, chk) == chkSize)
				{
					fchk->AddEntry(&sbuff[(chkSize << 1) + 2], chk);
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
