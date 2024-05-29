#include "Stdafx.h"
#include "Crypto/Cert/X509PKCS12.h"
#include "Net/ASN1Util.h"

Crypto::Cert::X509PKCS12::X509PKCS12(NN<Text::String> sourceName, Data::ByteArrayR buff) : Crypto::Cert::X509File(sourceName, buff)
{

}

Crypto::Cert::X509PKCS12::X509PKCS12(Text::CStringNN sourceName, Data::ByteArrayR buff) : Crypto::Cert::X509File(sourceName, buff)
{

}

Crypto::Cert::X509PKCS12::~X509PKCS12()
{

}

Crypto::Cert::X509File::FileType Crypto::Cert::X509PKCS12::GetFileType() const
{
	return FileType::PKCS12;
}

void Crypto::Cert::X509PKCS12::ToShortName(NN<Text::StringBuilderUTF8> sb) const
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

Bool Crypto::Cert::X509PKCS12::GetCertName(UOSInt index, NN<Text::StringBuilderUTF8> sb)
{
	return false;
}

Optional<Crypto::Cert::X509Cert> Crypto::Cert::X509PKCS12::GetNewCert(UOSInt index)
{
	return 0;
}

Crypto::Cert::X509File::ValidStatus Crypto::Cert::X509PKCS12::IsValid(NN<Net::SSLEngine> ssl, Optional<Crypto::Cert::CertStore> trustStore) const
{
	return Crypto::Cert::X509File::ValidStatus::SignatureInvalid;
}

NN<Net::ASN1Data> Crypto::Cert::X509PKCS12::Clone() const
{
	NN<Crypto::Cert::X509PKCS12> asn1;
	NEW_CLASSNN(asn1, Crypto::Cert::X509PKCS12(this->GetSourceNameObj(), this->buff));
	return asn1;
}

void Crypto::Cert::X509PKCS12::ToString(NN<Text::StringBuilderUTF8> sb) const
{
	if (IsPFX(this->buff.Arr(), this->buff.ArrEnd(), "1"))
	{
		AppendPFX(this->buff.Arr(), this->buff.ArrEnd(), "1", sb, CSTR_NULL);
	}
}

NN<Net::ASN1Names> Crypto::Cert::X509PKCS12::CreateNames() const
{
	NN<Net::ASN1Names> names;
	NEW_CLASSNN(names, Net::ASN1Names());
	return names->SetPFX();
}
