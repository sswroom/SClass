#include "Stdafx.h"
#include "Crypto/Cert/X509PKCS12.h"
#include "Net/ASN1Util.h"

Crypto::Cert::X509PKCS12::X509PKCS12(Text::String *sourceName, const UInt8 *buff, UOSInt buffSize) : Crypto::Cert::X509File(sourceName, buff, buffSize)
{

}

Crypto::Cert::X509PKCS12::X509PKCS12(Text::CString sourceName, const UInt8 *buff, UOSInt buffSize) : Crypto::Cert::X509File(sourceName, buff, buffSize)
{

}

Crypto::Cert::X509PKCS12::~X509PKCS12()
{

}

Crypto::Cert::X509File::FileType Crypto::Cert::X509PKCS12::GetFileType()
{
	return FileType::PKCS12;
}

void Crypto::Cert::X509PKCS12::ToShortName(Text::StringBuilderUTF8 *sb)
{
/*	UOSInt len = 0;
	Net::ASN1Util::ItemType itemType = Net::ASN1Util::IT_UNKNOWN;
	const UInt8 *tmpBuff = Net::ASN1Util::PDUGetItem(this->buff, this->buff + this->buffSize, "1.1.2", &len, &itemType);
	if (tmpBuff != 0 && itemType == Net::ASN1Util::IT_SEQUENCE)
	{
		NameGetCN(tmpBuff, tmpBuff + len, sb);
	}*/
}

UOSInt Crypto::Cert::X509PKCS12::GetCertCount()
{
	return 0;
}

Bool Crypto::Cert::X509PKCS12::GetCertName(UOSInt index, Text::StringBuilderUTF8 *sb)
{
	return false;
}

Crypto::Cert::X509Cert *Crypto::Cert::X509PKCS12::GetNewCert(UOSInt index)
{
	return 0;
}

Net::ASN1Data *Crypto::Cert::X509PKCS12::Clone()
{
	Crypto::Cert::X509PKCS12 *asn1;
	NEW_CLASS(asn1, Crypto::Cert::X509PKCS12(this->GetSourceNameObj(), this->buff, this->buffSize));
	return asn1;
}

void Crypto::Cert::X509PKCS12::ToString(Text::StringBuilderUTF8 *sb)
{
/*	if (IsCertificateRequest(this->buff, this->buff + this->buffSize, "1"))
	{
		AppendCertificateRequest(this->buff, this->buff + this->buffSize, "1", sb);
	}*/
}
