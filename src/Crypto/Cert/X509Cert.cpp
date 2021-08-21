#include "Stdafx.h"
#include "Crypto/Cert/X509Cert.h"

Crypto::Cert::X509Cert::X509Cert(const UTF8Char *sourceName, const UInt8 *buff, UOSInt buffSize) : Crypto::Cert::X509File(sourceName, buff, buffSize)
{

}

Crypto::Cert::X509Cert::~X509Cert()
{

}

Crypto::Cert::X509File::FileType Crypto::Cert::X509Cert::GetFileType()
{
	return FT_CERT;
}

Net::ASN1Data *Crypto::Cert::X509Cert::Clone()
{
	Crypto::Cert::X509Cert *asn1;
	NEW_CLASS(asn1, Crypto::Cert::X509Cert(this->GetSourceNameObj(), this->buff, this->buffSize));
	return asn1;
}

void Crypto::Cert::X509Cert::ToString(Text::StringBuilderUTF *sb)
{
	if (IsCertificate(this->buff, this->buff + this->buffSize, "1"))
	{
		AppendCertificate(this->buff, this->buff + this->buffSize, "1", sb);
	}
}
