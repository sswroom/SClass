#include "Stdafx.h"
#include "MyMemory.h"
#include "Net/ASN1Data.h"
#include "Net/ASN1Util.h"
#include "Parser/FileParser/ASN1Parser.h"
#include "Text/MyString.h"

Parser::FileParser::ASN1Parser::ASN1Parser()
{
}

Parser::FileParser::ASN1Parser::~ASN1Parser()
{
}

Int32 Parser::FileParser::ASN1Parser::GetName()
{
	return *(Int32*)"ASN1";
}

void Parser::FileParser::ASN1Parser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::ASN1Data)
	{
//		selector->AddFilter(CSTR("*.jks"), CSTR("Java Keystore"));
	}
}

IO::ParserType Parser::FileParser::ASN1Parser::GetParserType()
{
	return IO::ParserType::ASN1Data;
}

Optional<IO::ParsedObject> Parser::FileParser::ASN1Parser::ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr)
{
	if (!fd->IsFullFile())
		return nullptr;
	Net::ASN1Data *asn1 = 0;
//	UInt8 *buff;
	UInt64 fsize = fd->GetDataSize();
	NN<Text::String> fname = fd->GetFullFileName();
	if (fname->EndsWithICase(UTF8STRC(".JKS")) && fsize <= 65536)
	{
/*		buff = MemAlloc(UInt8, (UIntOS)fsize);
		if (fd->GetRealData(0, (UIntOS)fsize, buff) == fsize && Net::ASN1Util::PDUIsValid(buff, buff + (UIntOS)fsize))
		{
			NEW_CLASS(asn1, Crypto::JKSFile(fname, buff, (UIntOS)fsize));
		}
		MemFree(buff);*/
	}
	return asn1;
}
