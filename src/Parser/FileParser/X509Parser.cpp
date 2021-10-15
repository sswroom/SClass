#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/Cert/X509Cert.h"
#include "Crypto/Cert/X509CertReq.h"
#include "Crypto/Cert/X509File.h"
#include "Crypto/Cert/X509Key.h"
#include "Crypto/Cert/X509PrivKey.h"
#include "Crypto/Cert/X509PubKey.h"
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

void Parser::FileParser::X509Parser::PrepareSelector(IO::IFileSelector *selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::ASN1Data)
	{
		selector->AddFilter((const UTF8Char*)"*.crt", (const UTF8Char*)"X.509 Certification File");
		selector->AddFilter((const UTF8Char*)"*.p12", (const UTF8Char*)"PKCS 12 KeyStore File");
		selector->AddFilter((const UTF8Char*)"*.pem", (const UTF8Char*)"PEM File");
	}
}

IO::ParserType Parser::FileParser::X509Parser::GetParserType()
{
	return IO::ParserType::ASN1Data;
}

IO::ParsedObject *Parser::FileParser::X509Parser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType)
{
	UInt64 len = fd->GetDataSize();
	if (len > 4096 || (targetType != IO::ParserType::Unknown && targetType != IO::ParserType::ASN1Data))
	{
		return 0;
	}

	UInt8 buff[4096];
	fd->GetRealData(0, (UOSInt)len, buff);
	return ParseBuff(buff, (UOSInt)len, fd->GetFullFileName());
}

Crypto::Cert::X509File *Parser::FileParser::X509Parser::ParseBuff(const UInt8 *buff, UOSInt buffSize, const UTF8Char *fileName)
{
	Crypto::Cert::X509File *ret = 0;
	UInt8 dataBuff[2048];
	UOSInt dataLen;
	UOSInt lbSize;
	if (buff[buffSize - 2] == 13 && buff[buffSize - 1] == 10)
	{
		lbSize = 2;
	}
	else if (buff[buffSize - 1] == 10)
	{
		lbSize = 1;
	}
	else
	{
		return 0;
	}
	if (Text::StrStartsWith(buff, (const UTF8Char*)"-----BEGIN CERTIFICATE-----") && Text::StrStartsWith(&buff[buffSize - 25 - lbSize], (const UTF8Char*)"-----END CERTIFICATE-----"))
	{
		Text::TextBinEnc::Base64Enc b64;
		dataLen = b64.DecodeBin(&buff[27], buffSize - 52 - lbSize, dataBuff);
		NEW_CLASS(ret, Crypto::Cert::X509Cert(fileName, dataBuff, dataLen));
	}
	else if (Text::StrStartsWith(buff, (const UTF8Char*)"-----BEGIN RSA PRIVATE KEY-----") && Text::StrStartsWith(&buff[buffSize - 29 - lbSize], (const UTF8Char*)"-----END RSA PRIVATE KEY-----"))
	{
		if (Text::StrStartsWith(&buff[31 + lbSize], (const UTF8Char*)"Proc-Type:"))
		{

		}
		else
		{
			Text::TextBinEnc::Base64Enc b64;
			dataLen = b64.DecodeBin(&buff[31], buffSize - 60 - lbSize, dataBuff);
			NEW_CLASS(ret, Crypto::Cert::X509Key(fileName, dataBuff, dataLen, Crypto::Cert::X509File::KeyType::RSA));
		}
	}
	else if (Text::StrStartsWith(buff, (const UTF8Char*)"-----BEGIN DSA PRIVATE KEY-----") && Text::StrStartsWith(&buff[buffSize - 29 - lbSize], (const UTF8Char*)"-----END DSA PRIVATE KEY-----"))
	{
		Text::TextBinEnc::Base64Enc b64;
		dataLen = b64.DecodeBin(&buff[31], buffSize - 60 - lbSize, dataBuff);
		NEW_CLASS(ret, Crypto::Cert::X509Key(fileName, dataBuff, dataLen, Crypto::Cert::X509File::KeyType::DSA));
	}
	else if (Text::StrStartsWith(buff, (const UTF8Char*)"-----BEGIN EC PRIVATE KEY-----") && Text::StrStartsWith(&buff[buffSize - 28 - lbSize], (const UTF8Char*)"-----END EC PRIVATE KEY-----"))
	{
		Text::TextBinEnc::Base64Enc b64;
		dataLen = b64.DecodeBin(&buff[30], buffSize - 58 - lbSize, dataBuff);
		NEW_CLASS(ret, Crypto::Cert::X509Key(fileName, dataBuff, dataLen, Crypto::Cert::X509File::KeyType::ECDSA));
	}
	else if (Text::StrStartsWith(buff, (const UTF8Char*)"-----BEGIN PRIVATE KEY-----") && Text::StrStartsWith(&buff[buffSize - 25 - lbSize], (const UTF8Char*)"-----END PRIVATE KEY-----"))
	{
		Text::TextBinEnc::Base64Enc b64;
		dataLen = b64.DecodeBin(&buff[27], buffSize - 52 - lbSize, dataBuff);
		NEW_CLASS(ret, Crypto::Cert::X509PrivKey(fileName, dataBuff, dataLen));
	}
	else if (Text::StrStartsWith(buff, (const UTF8Char*)"-----BEGIN PUBLIC KEY-----") && Text::StrStartsWith(&buff[buffSize - 24 - lbSize], (const UTF8Char*)"-----END PUBLIC KEY-----"))
	{
		Text::TextBinEnc::Base64Enc b64;
		dataLen = b64.DecodeBin(&buff[26], buffSize - 50 - lbSize, dataBuff);
		NEW_CLASS(ret, Crypto::Cert::X509PubKey(fileName, dataBuff, dataLen));
	}
	else if (Text::StrStartsWith(buff, (const UTF8Char*)"-----BEGIN CERTIFICATE REQUEST-----") && Text::StrStartsWith(&buff[buffSize - 33 - lbSize], (const UTF8Char*)"-----END CERTIFICATE REQUEST-----"))
	{
		Text::TextBinEnc::Base64Enc b64;
		dataLen = b64.DecodeBin(&buff[35], buffSize - 68 - lbSize, dataBuff);
		NEW_CLASS(ret, Crypto::Cert::X509CertReq(fileName, dataBuff, dataLen));
	}
	else
	{
		if (!Text::StrEndsWithICase(fileName, (const UTF8Char*)".P12") && !Text::StrEndsWithICase(fileName, (const UTF8Char*)".DER"))
		{
			return 0;
		}
		NEW_CLASS(ret, Crypto::Cert::X509Cert(fileName, buff, buffSize));
	}
	return ret;
}
