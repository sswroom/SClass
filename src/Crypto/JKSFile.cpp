#include "Stdafx.h"
#include "Crypto/JKSFile.h"

Crypto::JKSFile::JKSFile(const UTF8Char *sourceName, const UInt8 *buff, UOSInt buffSize) : Crypto::X509File(sourceName, buff, buffSize)
{

}

Crypto::JKSFile::~JKSFile()
{

}

Crypto::X509File::FileType Crypto::JKSFile::GetFileType()
{
	return FT_JKS;
}

Net::ASN1Data *Crypto::JKSFile::Clone()
{
	Crypto::JKSFile *asn1;
	NEW_CLASS(asn1, Crypto::JKSFile(this->GetSourceNameObj(), this->buff, this->buffSize));
	return asn1;
}

void Crypto::JKSFile::ToString(Text::StringBuilderUTF *sb)
{
}
