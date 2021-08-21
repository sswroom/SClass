#include "Stdafx.h"
#include "Crypto/Cert/X509RSAKey.h"

Crypto::Cert::X509RSAKey::X509RSAKey(const UTF8Char *sourceName, const UInt8 *buff, UOSInt buffSize) : Crypto::Cert::X509File(sourceName, buff, buffSize)
{

}

Crypto::Cert::X509RSAKey::~X509RSAKey()
{

}

Crypto::Cert::X509File::FileType Crypto::Cert::X509RSAKey::GetFileType()
{
	return FT_RSA_KEY;
}

Net::ASN1Data *Crypto::Cert::X509RSAKey::Clone()
{
	Crypto::Cert::X509RSAKey *asn1;
	NEW_CLASS(asn1, Crypto::Cert::X509RSAKey(this->GetSourceNameObj(), this->buff, this->buffSize));
	return asn1;
}

void Crypto::Cert::X509RSAKey::ToString(Text::StringBuilderUTF *sb)
{
}
