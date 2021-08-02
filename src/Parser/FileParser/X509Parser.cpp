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
	if (t == IO::ParsedObject::PT_UNKNOWN || t == IO::ParsedObject::PT_ASN1_DATA)
	{
		selector->AddFilter((const UTF8Char*)"*.crt", (const UTF8Char*)"X.509 Certification File");
		selector->AddFilter((const UTF8Char*)"*.p12", (const UTF8Char*)"PKCS 12 KeyStore File");
	}
}

IO::ParsedObject::ParserType Parser::FileParser::X509Parser::GetParserType()
{
	return IO::ParsedObject::PT_ASN1_DATA;
}

IO::ParsedObject *Parser::FileParser::X509Parser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType)
{
	UInt64 len = fd->GetDataSize();
	if (len > 4096 || (targetType != IO::ParsedObject::PT_UNKNOWN && targetType != IO::ParsedObject::PT_ASN1_DATA))
	{
		return 0;
	}

	Crypto::X509File *ret = 0;
	UInt8 buff[4096];
	UInt8 dataBuff[2048];
	UOSInt dataLen;
	fd->GetRealData(0, (UOSInt)len, buff);
	if (Text::StrStartsWith(buff, (const UTF8Char*)"-----BEGIN CERTIFICATE-----") && Text::StrStartsWith(&buff[len - 26], (const UTF8Char*)"-----END CERTIFICATE-----\n"))
	{
		Text::TextBinEnc::Base64Enc b64;
		dataLen = b64.DecodeBin(&buff[27], (UOSInt)len - 53, dataBuff);
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
			dataLen = b64.DecodeBin(&buff[31], (UOSInt)len - 61, dataBuff);
			ret = Crypto::X509File::LoadFile(fd->GetFullFileName(), dataBuff, dataLen, Crypto::X509File::FT_RSA_KEY);
		}
	}
	else if (Text::StrStartsWith(buff, (const UTF8Char*)"-----BEGIN PRIVATE KEY-----") && Text::StrStartsWith(&buff[len - 26], (const UTF8Char*)"-----END PRIVATE KEY-----\n"))
	{
		Text::TextBinEnc::Base64Enc b64;
		dataLen = b64.DecodeBin(&buff[27], (UOSInt)len - 53, dataBuff);
		ret = Crypto::X509File::LoadFile(fd->GetFullFileName(), dataBuff, dataLen, Crypto::X509File::FT_PRIV_KEY);
	}
	else if (Text::StrStartsWith(buff, (const UTF8Char*)"-----BEGIN CERTIFICATE REQUEST-----") && Text::StrStartsWith(&buff[len - 34], (const UTF8Char*)"-----END CERTIFICATE REQUEST-----\n"))
	{
		Text::TextBinEnc::Base64Enc b64;
		dataLen = b64.DecodeBin(&buff[35], (UOSInt)len - 69, dataBuff);
		ret = Crypto::X509File::LoadFile(fd->GetFullFileName(), dataBuff, dataLen, Crypto::X509File::FT_CERT_REQ);
	}
	else
	{
		const UTF8Char *name = fd->GetShortName();
		if (!Text::StrEndsWithICase(name, (const UTF8Char*)".P12") && !Text::StrEndsWithICase(name, (const UTF8Char*)".DER"))
		{
			return 0;
		}
		ret = Crypto::X509File::LoadFile(fd->GetFullFileName(), buff, (UOSInt)len, Crypto::X509File::FT_CERT);
	}
	return ret;
}
