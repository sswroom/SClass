#include "Stdafx.h"
#include "Crypto/Cert/X509CertReq.h"
#include "Net/ASN1Util.h"

Crypto::Cert::X509CertReq::X509CertReq(const UTF8Char *sourceName, const UInt8 *buff, UOSInt buffSize) : Crypto::Cert::X509File(sourceName, buff, buffSize)
{

}

Crypto::Cert::X509CertReq::~X509CertReq()
{

}

Crypto::Cert::X509File::FileType Crypto::Cert::X509CertReq::GetFileType()
{
	return FileType::CertRequest;
}

void Crypto::Cert::X509CertReq::ToShortName(Text::StringBuilderUTF *sb)
{
	UOSInt len = 0;
	Net::ASN1Util::ItemType itemType = Net::ASN1Util::IT_UNKNOWN;
	const UInt8 *tmpBuff = Net::ASN1Util::PDUGetItem(this->buff, this->buff + this->buffSize, "1.1.2", &len, &itemType);
	if (tmpBuff != 0 && itemType == Net::ASN1Util::IT_SEQUENCE)
	{
		NameGetCN(tmpBuff, tmpBuff + len, sb);
	}
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

Bool Crypto::Cert::X509CertReq::GetNames(CertNames *names)
{
	UOSInt itemLen;
	Net::ASN1Util::ItemType itemType;
	const UInt8 *namesPDU = Net::ASN1Util::PDUGetItem(this->buff, this->buff + this->buffSize, "1.1.2", &itemLen, &itemType);
	if (namesPDU)
	{
		return NamesGet(namesPDU, namesPDU + itemLen, names);
	}
	return false;
}
