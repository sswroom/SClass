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

void Parser::FileParser::ASN1Parser::PrepareSelector(IO::IFileSelector *selector, IO::ParserType t)
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

IO::ParsedObject *Parser::FileParser::ASN1Parser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType)
{
	if (!fd->IsFullFile())
		return 0;
	Net::ASN1Data *asn1 = 0;
//	UInt8 *buff;
	UInt64 fsize = fd->GetDataSize();
	Text::String *fname = fd->GetFullFileName();
	if (fname->EndsWithICase(UTF8STRC(".JKS")) && fsize <= 65536)
	{
/*		buff = MemAlloc(UInt8, (UOSInt)fsize);
		if (fd->GetRealData(0, (UOSInt)fsize, buff) == fsize && Net::ASN1Util::PDUIsValid(buff, buff + (UOSInt)fsize))
		{
			NEW_CLASS(asn1, Crypto::JKSFile(fname, buff, (UOSInt)fsize));
		}
		MemFree(buff);*/
	}
	return asn1;
}
