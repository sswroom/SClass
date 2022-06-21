#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/Cert/X509Cert.h"
#include "Data/ByteTool.h"
#include "IO/PackageFile.h"
#include "Parser/FileParser/JKSParser.h"
#include "Parser/FileParser/X509Parser.h"

#include <stdio.h>

Parser::FileParser::JKSParser::JKSParser()
{
}

Parser::FileParser::JKSParser::~JKSParser()
{
}

Int32 Parser::FileParser::JKSParser::GetName()
{
	return *(Int32*)"JKSP";
}

void Parser::FileParser::JKSParser::PrepareSelector(IO::IFileSelector *selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::SectorData)
	{
		selector->AddFilter(CSTR("*.jks"), CSTR("Java KeyStore"));
	}
}

IO::ParserType Parser::FileParser::JKSParser::GetParserType()
{
	return IO::ParserType::PackageFile;
}

IO::ParsedObject *Parser::FileParser::JKSParser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType)
{
	UInt8 buff[128];
	UInt64 fileSize = fd->GetDataSize();
	if (fd->GetRealData(0, 12, buff) < 12)
		return 0;
	if (ReadMUInt32(buff) != 0xFEEDFEED)
	{
		return 0;
	}
	UInt32 version = ReadMUInt32(&buff[4]);
	if (version > 2)
	{
		return 0;
	}
	UInt8 *cerBuff = 0;
	UOSInt cerBuffSize = 0;
	Text::StringBuilderUTF8 sb;
	IO::PackageFile *pkg;
	NEW_CLASS(pkg, IO::PackageFile(fd->GetFullFileName()));
	UInt32 cnt = ReadMUInt32(&buff[8]);
	UInt64 ofst = 12;
	UOSInt readSize;
	UOSInt i = 0;
	while (i < cnt && ofst < fileSize)
	{
		readSize = fd->GetRealData(ofst, 128, buff);
		if (readSize < 25)
		{
			printf("JKS: readSize < 25\r\n");
			break;
		}
		UInt32 tag = ReadMUInt32(buff);
		if (tag != 2)
		{
			printf("JKS: Unsupported tag: %d\r\n", tag);
			break;
		}

		UOSInt aliasLen = ReadMUInt16(&buff[4]);
		Int64 ts = ReadMInt64(&buff[6 + aliasLen]);
		UOSInt certTypeLen = ReadMUInt16(&buff[14 + aliasLen]);
		UInt32 certLen = ReadMUInt32(&buff[16 + aliasLen + certTypeLen]);
		if (certTypeLen != 5 || !Text::StrEqualsC(&buff[16 + aliasLen], certTypeLen, UTF8STRC("X.509")))
		{
			printf("JKS: Unknown cert Type\r\n");
			break;
		}
		sb.ClearStr();
		sb.AppendC(&buff[6], aliasLen);
		sb.AppendC(UTF8STRC(".cer"));
		if (cerBuffSize < certLen)
		{
			cerBuffSize = certLen;
			if (cerBuff)
			{
				MemFree(cerBuff);
			}
			cerBuff = MemAlloc(UInt8, cerBuffSize);
		}
		Text::String *s = Text::String::New(sb.ToCString());
		fd->GetRealData(ofst + 20 + aliasLen + certTypeLen, certLen, cerBuff);
		Crypto::Cert::X509Cert *cert = (Crypto::Cert::X509Cert*)Parser::FileParser::X509Parser::ParseBuff(cerBuff, certLen, s);
		s->Release();
		if (cert)
		{
			pkg->AddObject(cert, Text::CString(&buff[6], aliasLen), ts);
		}
		else
		{
			printf("JKS: Error in parsing cert file\r\n");
		}
		ofst += 20 + aliasLen + certTypeLen + certLen;
		i++;
	}
	if (cerBuff)
	{
		MemFree(cerBuff);
	}
	return pkg;
}
