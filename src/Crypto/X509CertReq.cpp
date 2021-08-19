#include "Stdafx.h"
#include "Crypto/X509CertReq.h"

Crypto::X509CertReq::X509CertReq(const UTF8Char *sourceName, const UInt8 *buff, UOSInt buffSize) : Crypto::X509File(sourceName, buff, buffSize)
{

}

Crypto::X509CertReq::~X509CertReq()
{

}

Crypto::X509File::FileType Crypto::X509CertReq::GetFileType()
{
	return FT_CERT_REQ;
}

Net::ASN1Data *Crypto::X509CertReq::Clone()
{
	Crypto::X509CertReq *asn1;
	NEW_CLASS(asn1, Crypto::X509CertReq(this->GetSourceNameObj(), this->buff, this->buffSize));
	return asn1;
}

void Crypto::X509CertReq::ToString(Text::StringBuilderUTF *sb)
{
}
