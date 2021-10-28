#include "Stdafx.h"
#include "Crypto/Cert/X509PKCS7.h"
#include "Net/ASN1Util.h"

Crypto::Cert::X509PKCS7::X509PKCS7(const UTF8Char *sourceName, const UInt8 *buff, UOSInt buffSize) : Crypto::Cert::X509File(sourceName, buff, buffSize)
{

}

Crypto::Cert::X509PKCS7::~X509PKCS7()
{

}

Crypto::Cert::X509File::FileType Crypto::Cert::X509PKCS7::GetFileType()
{
	return FileType::PKCS7;
}

void Crypto::Cert::X509PKCS7::ToShortName(Text::StringBuilderUTF *sb)
{
/*	UOSInt len = 0;
	Net::ASN1Util::ItemType itemType = Net::ASN1Util::IT_UNKNOWN;
	const UInt8 *tmpBuff = Net::ASN1Util::PDUGetItem(this->buff, this->buff + this->buffSize, "1.1.2", &len, &itemType);
	if (tmpBuff != 0 && itemType == Net::ASN1Util::IT_SEQUENCE)
	{
		NameGetCN(tmpBuff, tmpBuff + len, sb);
	}*/
}

Net::ASN1Data *Crypto::Cert::X509PKCS7::Clone()
{
	Crypto::Cert::X509PKCS7 *asn1;
	NEW_CLASS(asn1, Crypto::Cert::X509PKCS7(this->GetSourceNameObj(), this->buff, this->buffSize));
	return asn1;
}

void Crypto::Cert::X509PKCS7::ToString(Text::StringBuilderUTF *sb)
{
/*	if (IsCertificateRequest(this->buff, this->buff + this->buffSize, "1"))
	{
		AppendCertificateRequest(this->buff, this->buff + this->buffSize, "1", sb);
	}*/
}
