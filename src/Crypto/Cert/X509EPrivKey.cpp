#include "Stdafx.h"
#include "Crypto/Cert/X509EPrivKey.h"
#include "Net/ASN1PDUBuilder.h"
#include "Net/ASN1Util.h"

Crypto::Cert::X509EPrivKey::X509EPrivKey(NN<Text::String> sourceName, Data::ByteArrayR buff) : Crypto::Cert::X509File(sourceName, buff)
{

}

Crypto::Cert::X509EPrivKey::X509EPrivKey(Text::CStringNN sourceName, Data::ByteArrayR buff) : Crypto::Cert::X509File(sourceName, buff)
{

}

Crypto::Cert::X509EPrivKey::~X509EPrivKey()
{

}

Crypto::Cert::X509File::FileType Crypto::Cert::X509EPrivKey::GetFileType() const
{
	return FileType::EPrivateKey;
}

void Crypto::Cert::X509EPrivKey::ToShortName(NN<Text::StringBuilderUTF8> sb) const
{
	UIntOS oidLen;
	Net::ASN1Util::ItemType itemType;
	UnsafeArray<const UInt8> oidPDU;
	if (!Net::ASN1Util::PDUGetItem(this->buff.Arr(), this->buff.ArrEnd(), "1.2.1", oidLen, itemType).SetTo(oidPDU) || itemType != Net::ASN1Util::IT_OID)
	{
		return;
	}
	KeyType keyType = KeyTypeFromOID(Data::ByteArrayR(oidPDU, oidLen), false);
	UIntOS keyLen;
	UnsafeArray<const UInt8> keyPDU;
	if (Net::ASN1Util::PDUGetItem(this->buff.Arr(), this->buff.ArrEnd(), "1.3", keyLen, itemType).SetTo(keyPDU) && itemType == Net::ASN1Util::IT_OCTET_STRING)
	{
		sb->Append(KeyTypeGetName(keyType));
		sb->AppendUTF8Char(' ');
		sb->AppendUIntOS(KeyGetLeng(keyPDU, keyPDU + keyLen, keyType));
		sb->AppendC(UTF8STRC(" bits"));
	}
}

Crypto::Cert::X509File::ValidStatus Crypto::Cert::X509EPrivKey::IsValid(NN<Net::SSLEngine> ssl, Optional<Crypto::Cert::CertStore> trustStore) const
{
	return Crypto::Cert::X509File::ValidStatus::SignatureInvalid;
}

NN<Net::ASN1Data> Crypto::Cert::X509EPrivKey::Clone() const
{
	NN<Crypto::Cert::X509EPrivKey> asn1;
	NEW_CLASSNN(asn1, Crypto::Cert::X509EPrivKey(this->GetSourceNameObj(), this->buff));
	return asn1;
}

void Crypto::Cert::X509EPrivKey::ToString(NN<Text::StringBuilderUTF8> sb) const
{
	if (IsPrivateKeyInfo(this->buff.Arr(), this->buff.ArrEnd(), "1"))
	{
		AppendPrivateKeyInfo(this->buff.Arr(), this->buff.ArrEnd(), "1", sb);
	}
}

NN<Net::ASN1Names> Crypto::Cert::X509EPrivKey::CreateNames() const
{
	NN<Net::ASN1Names> names;
	NEW_CLASSNN(names, Net::ASN1Names());
	names->SetEncryptedPrivateKey();
	return names;
}
