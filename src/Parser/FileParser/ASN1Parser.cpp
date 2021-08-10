#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/JKSFile.h"
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

void Parser::FileParser::ASN1Parser::PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t)
{
	if (t == IO::ParsedObject::PT_UNKNOWN || t == IO::ParsedObject::PT_ASN1_DATA)
	{
		selector->AddFilter((const UTF8Char*)"*.jks", (const UTF8Char*)"Java Keystore");
	}
}

IO::ParsedObject::ParserType Parser::FileParser::ASN1Parser::GetParserType()
{
	return IO::ParsedObject::PT_ASN1_DATA;
}

IO::ParsedObject *Parser::FileParser::ASN1Parser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType)
{
	if (!fd->IsFullFile())
		return 0;
	Net::ASN1Data *asn1 = 0;
	UInt8 *buff;
	UInt64 fsize = fd->GetDataSize();
	const UTF8Char *fname = fd->GetFullFileName();
	if (Text::StrEndsWithICase(fname, (const UTF8Char*)".JKS") && fsize <= 65536)
	{
		buff = MemAlloc(UInt8, (UOSInt)fsize);
		if (fd->GetRealData(0, (UOSInt)fsize, buff) == fsize && Net::ASN1Util::PDUIsValid(buff, buff + (UOSInt)fsize))
		{
			NEW_CLASS(asn1, Crypto::JKSFile(fname, buff, (UOSInt)fsize));
		}
		MemFree(buff);
	}
	return asn1;
}
