#include "Stdafx.h"
#include "Crypto/Cert/X509CertReq.h"

Crypto::Cert::X509CertReq::X509CertReq(const UTF8Char *sourceName, const UInt8 *buff, UOSInt buffSize) : Crypto::Cert::X509File(sourceName, buff, buffSize)
{

}

Crypto::Cert::X509CertReq::~X509CertReq()
{

}

Crypto::Cert::X509File::FileType Crypto::Cert::X509CertReq::GetFileType()
{
	return FT_CERT_REQ;
}

Net::ASN1Data *Crypto::Cert::X509CertReq::Clone()
{
	Crypto::Cert::X509CertReq *asn1;
	NEW_CLASS(asn1, Crypto::Cert::X509CertReq(this->GetSourceNameObj(), this->buff, this->buffSize));
	return asn1;
}

void Crypto::Cert::X509CertReq::ToString(Text::StringBuilderUTF *sb)
{
	if (IsCertificateRequest(this->buff, this->buff + this->buffSize, "1"))
	{
		AppendCertificateRequest(this->buff, this->buff + this->buffSize, "1", sb);
	}
}
