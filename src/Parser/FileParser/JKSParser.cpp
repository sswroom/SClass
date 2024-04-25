#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/Cert/X509Cert.h"
#include "Data/ByteBuffer.h"
#include "Data/ByteTool.h"
#include "IO/VirtualPackageFileFast.h"
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

void Parser::FileParser::JKSParser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
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

IO::ParsedObject *Parser::FileParser::JKSParser::ParseFileHdr(NN<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr)
{
	UInt64 fileSize = fd->GetDataSize();
	if (ReadMUInt32(&hdr[0]) != 0xFEEDFEED)
	{
		return 0;
	}
	UInt32 version = ReadMUInt32(&hdr[4]);
	if (version > 2)
	{
		return 0;
	}
	UInt8 buff[256];
	Data::ByteBuffer cerBuff;
	Text::StringBuilderUTF8 sb;
	IO::VirtualPackageFile *pkg;
	NEW_CLASS(pkg, IO::VirtualPackageFileFast(fd->GetFullFileName()));
	UInt32 cnt = ReadMUInt32(&hdr[8]);
	UInt64 ofst = 12;
	UOSInt readSize;
	UOSInt i = 0;
	while (i < cnt && ofst < fileSize)
	{
		readSize = fd->GetRealData(ofst, 256, BYTEARR(buff));
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
		if (cerBuff.GetSize() < certLen)
		{
			cerBuff.ChangeSize(certLen);
		}
		NN<Text::String> s = Text::String::New(sb.ToCString());
		fd->GetRealData(ofst + 20 + aliasLen + certTypeLen, certLen, cerBuff);
		Crypto::Cert::X509Cert *cert = (Crypto::Cert::X509Cert*)Parser::FileParser::X509Parser::ParseBuff(cerBuff.WithSize(certLen), s);
		s->Release();
		if (cert)
		{
			pkg->AddObject(cert, Text::CStringNN(&buff[6], aliasLen), Data::Timestamp(ts, 0), 0, 0, 0);
		}
		else
		{
			printf("JKS: Error in parsing cert file\r\n");
		}
		ofst += 20 + aliasLen + certTypeLen + certLen;
		i++;
	}
	return pkg;
}
