#include "Stdafx.h"
#include "Crypto/X509RSAKey.h"

Crypto::X509RSAKey::X509RSAKey(const UTF8Char *sourceName, const UInt8 *buff, UOSInt buffSize) : Crypto::X509File(sourceName, buff, buffSize)
{

}

Crypto::X509RSAKey::~X509RSAKey()
{

}

Crypto::X509File::FileType Crypto::X509RSAKey::GetFileType()
{
	return FT_RSA_KEY;
}

Net::ASN1Data *Crypto::X509RSAKey::Clone()
{
	Crypto::X509RSAKey *asn1;
	NEW_CLASS(asn1, Crypto::X509RSAKey(this->GetSourceNameObj(), this->buff, this->buffSize));
	return asn1;
}

void Crypto::X509RSAKey::ToString(Text::StringBuilderUTF *sb)
{
}
