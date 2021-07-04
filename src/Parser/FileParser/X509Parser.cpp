#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/X509File.h"
#include "Net/ASN1Util.h"
#include "Parser/FileParser/X509Parser.h"
#include "Text/MyString.h"
#include "Text/TextBinEnc/Base64Enc.h"

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

	Crypto::X509File *ret = 0;
	UInt8 buff[4096];
	UInt8 dataBuff[2048];
	UOSInt dataLen;
	fd->GetRealData(0, len, buff);
	if (Text::StrStartsWith(buff, (const UTF8Char*)"-----BEGIN CERTIFICATE-----") && Text::StrStartsWith(&buff[len - 26], (const UTF8Char*)"-----END CERTIFICATE-----\n"))
	{
		Text::TextBinEnc::Base64Enc b64;
		dataLen = b64.DecodeBin(&buff[27], len - 53, dataBuff);
		ret = Crypto::X509File::LoadFile(fd->GetFullFileName(), dataBuff, dataLen, Crypto::X509File::FT_CERT);
	}
	else if (Text::StrStartsWith(buff, (const UTF8Char*)"-----BEGIN RSA PRIVATE KEY-----") && Text::StrStartsWith(&buff[len - 30], (const UTF8Char*)"-----END RSA PRIVATE KEY-----\n"))
	{
		if (Text::StrStartsWith(&buff[32], (const UTF8Char*)"Proc-Type:"))
		{

		}
		else
		{
			Text::TextBinEnc::Base64Enc b64;
			dataLen = b64.DecodeBin(&buff[31], len - 61, dataBuff);
			ret = Crypto::X509File::LoadFile(fd->GetFullFileName(), dataBuff, dataLen, Crypto::X509File::FT_RSA_KEY);
		}
	}
	else if (Text::StrStartsWith(buff, (const UTF8Char*)"-----BEGIN CERTIFICATE REQUEST-----") && Text::StrStartsWith(&buff[len - 34], (const UTF8Char*)"-----END CERTIFICATE REQUEST-----\n"))
	{
		Text::TextBinEnc::Base64Enc b64;
		dataLen = b64.DecodeBin(&buff[35], len - 69, dataBuff);
		ret = Crypto::X509File::LoadFile(fd->GetFullFileName(), dataBuff, dataLen, Crypto::X509File::FT_CERT_REQ);
	}
	return ret;
}
