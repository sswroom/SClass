#include "Stdafx.h"
#include "MyMemory.h"
#include "Net/ASN1Util.h"
#include "Parser/FileParser/X509Parser.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/TextBinEnc/Base64Enc.h"

#include <stdio.h>

Parser::FileParser::X509Parser::X509Parser()
{
}

Parser::FileParser::X509Parser::~X509Parser()
{
}

Int32 Parser::FileParser::X509Parser::GetName()
{
	return *(Int32*)"X509";
}

void Parser::FileParser::X509Parser::PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t)
{
	if (t == IO::ParsedObject::PT_UNKNOWN || t == IO::ParsedObject::PT_X509_FILE)
	{
		selector->AddFilter((const UTF8Char*)"*.x13", (const UTF8Char*)"X13 (Hooligans) Package File");
	}
}

IO::ParsedObject::ParserType Parser::FileParser::X509Parser::GetParserType()
{
	return IO::ParsedObject::PT_X509_FILE;
}

IO::ParsedObject *Parser::FileParser::X509Parser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType)
{
	UInt64 len = fd->GetDataSize();
	if (len > 4096 || (targetType != IO::ParsedObject::PT_UNKNOWN && targetType != IO::ParsedObject::PT_X509_FILE))
	{
		return 0;
	}
	UInt8 buff[4096];
	UInt8 dataBuff[2048];
	UOSInt dataLen;
	fd->GetRealData(0, len, buff);
	if (Text::StrStartsWith(buff, (const UTF8Char*)"-----BEGIN CERTIFICATE-----") && Text::StrStartsWith(&buff[len - 26], (const UTF8Char*)"-----END CERTIFICATE-----\n"))
	{
		Text::TextBinEnc::Base64Enc b64;
		dataLen = b64.DecodeBin(&buff[27], len - 53, dataBuff);
		Text::StringBuilderUTF8 sb;
		Bool succ = Net::ASN1Util::PDUToString(dataBuff, dataBuff + dataLen, &sb, 0);
		printf("%s\r\n", sb.ToString());
		if (!succ)
		{
			sb.ClearStr();
			sb.AppendHexBuff(dataBuff, dataLen, ' ', Text::LBT_CRLF);
			printf("%s\r\n", sb.ToString());
		}
	}
	return 0;
}
