#include "Stdafx.h"
#include "Crypto/Cert/X509PKCS12.h"
#include "Net/ASN1Util.h"

Crypto::Cert::X509PKCS12::X509PKCS12(NotNullPtr<Text::String> sourceName, Data::ByteArrayR buff) : Crypto::Cert::X509File(sourceName, buff)
{

}

Crypto::Cert::X509PKCS12::X509PKCS12(Text::CString sourceName, Data::ByteArrayR buff) : Crypto::Cert::X509File(sourceName, buff)
{

}

Crypto::Cert::X509PKCS12::~X509PKCS12()
{

}

Crypto::Cert::X509File::FileType Crypto::Cert::X509PKCS12::GetFileType() const
{
	return FileType::PKCS12;
}

void Crypto::Cert::X509PKCS12::ToShortName(Text::StringBuilderUTF8 *sb) const
{
/*	UOSInt len = 0;
	Net::ASN1Util::ItemType itemType = Net::ASN1Util::IT_UNKNOWN;
	const UInt8 *tmpBuff = Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff + this->buffSize, "1.1.2", &len, &itemType);
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

Crypto::Cert::X509File::ValidStatus Crypto::Cert::X509PKCS12::IsValid(Net::SSLEngine *ssl, Crypto::Cert::CertStore *trustStore) const
{
	return Crypto::Cert::X509File::ValidStatus::SignatureInvalid;
}

Net::ASN1Data *Crypto::Cert::X509PKCS12::Clone() const
{
	Crypto::Cert::X509PKCS12 *asn1;
	NEW_CLASS(asn1, Crypto::Cert::X509PKCS12(this->GetSourceNameObj(), this->buff));
	return asn1;
}

void Crypto::Cert::X509PKCS12::ToString(Text::StringBuilderUTF8 *sb) const
{
	if (IsPFX(this->buff.Ptr(), this->buff.PtrEnd(), "1"))
	{
		AppendPFX(this->buff.Ptr(), this->buff.PtrEnd(), "1", sb, CSTR_NULL);
	}
}
