#include "Stdafx.h"
#include "Crypto/Cert/X509PubKey.h"
#include "Net/ASN1PDUBuilder.h"
#include "Net/ASN1Util.h"

Crypto::Cert::X509PubKey::X509PubKey(NN<Text::String> sourceName, Data::ByteArrayR buff) : Crypto::Cert::X509File(sourceName, buff)
{

}

Crypto::Cert::X509PubKey::X509PubKey(Text::CStringNN sourceName, Data::ByteArrayR buff) : Crypto::Cert::X509File(sourceName, buff)
{

}

Crypto::Cert::X509PubKey::~X509PubKey()
{

}

Crypto::Cert::X509File::FileType Crypto::Cert::X509PubKey::GetFileType() const
{
	return FileType::PublicKey;
}

void Crypto::Cert::X509PubKey::ToShortName(NN<Text::StringBuilderUTF8> sb) const
{
	UOSInt oidLen;
	Net::ASN1Util::ItemType itemType;
	UnsafeArray<const UInt8> oidPDU;
	if (!Net::ASN1Util::PDUGetItem(this->buff.Arr(), this->buff.ArrEnd(), "1.1.1", oidLen, itemType).SetTo(oidPDU) || itemType != Net::ASN1Util::IT_OID)
	{
		return;
	}
	KeyType keyType = KeyTypeFromOID(Data::ByteArrayR(oidPDU, oidLen), true);
	UOSInt keyLen;
	UnsafeArray<const UInt8> keyPDU;
	if (Net::ASN1Util::PDUGetItem(this->buff.Arr(), this->buff.ArrEnd(), "1.2", keyLen, itemType).SetTo(keyPDU) && itemType == Net::ASN1Util::IT_OCTET_STRING)
	{
		sb->Append(KeyTypeGetName(keyType));
		sb->AppendUTF8Char(' ');
		sb->AppendUOSInt(KeyGetLeng(keyPDU, keyPDU + keyLen, keyType));
		sb->AppendC(UTF8STRC(" bits"));
	}
}

Crypto::Cert::X509File::ValidStatus Crypto::Cert::X509PubKey::IsValid(NN<Net::SSLEngine> ssl, Optional<Crypto::Cert::CertStore> trustStore) const
{
	return Crypto::Cert::X509File::ValidStatus::SignatureInvalid;
}

NN<Net::ASN1Data> Crypto::Cert::X509PubKey::Clone() const
{
	NN<Crypto::Cert::X509PubKey> asn1;
	NEW_CLASSNN(asn1, Crypto::Cert::X509PubKey(this->GetSourceNameObj(), this->buff));
	return asn1;
}

void Crypto::Cert::X509PubKey::ToString(NN<Text::StringBuilderUTF8> sb) const
{
	if (IsPublicKeyInfo(this->buff.Arr(), this->buff.ArrEnd(), "1"))
	{
		AppendPublicKeyInfo(this->buff.Arr(), this->buff.ArrEnd(), "1", sb);
	}
}

NN<Net::ASN1Names> Crypto::Cert::X509PubKey::CreateNames() const
{
	NN<Net::ASN1Names> names;
	NEW_CLASSNN(names, Net::ASN1Names());
	return names;
}

Optional<Crypto::Cert::X509Key> Crypto::Cert::X509PubKey::CreateKey() const
{
	Net::ASN1Util::ItemType itemType;
	UOSInt keyTypeLen;
	UOSInt keyDataLen;
	UnsafeArray<const UInt8> keyTypeOID;
	UnsafeArray<const UInt8> keyData;
	if (Net::ASN1Util::PDUGetItem(this->buff.Arr(), this->buff.ArrEnd(), "1.1.1", keyTypeLen, itemType).SetTo(keyTypeOID) &&
		Net::ASN1Util::PDUGetItem(this->buff.Arr(), this->buff.ArrEnd(), "1.2", keyDataLen, itemType).SetTo(keyData))
	{
		NN<Crypto::Cert::X509Key> key;
		if (keyData[0] == 0)
		{
			keyData++;
			keyDataLen--;
		}
		NEW_CLASSNN(key, Crypto::Cert::X509Key(this->GetSourceNameObj(), Data::ByteArrayR(keyData, keyDataLen), KeyTypeFromOID(Data::ByteArrayR(keyTypeOID, keyTypeLen), true)));
		return key;
	}
	return 0;
}

NN<Crypto::Cert::X509PubKey> Crypto::Cert::X509PubKey::CreateFromKeyBuff(KeyType keyType, const UInt8 *buff, UOSInt buffSize, NN<Text::String> sourceName)
{
	Net::ASN1PDUBuilder keyPDU;
	keyPDU.BeginSequence();
	keyPDU.BeginSequence();
	Text::CStringNN oidStr = KeyTypeGetOID(keyType);
	keyPDU.AppendOIDString(oidStr);
	keyPDU.AppendNull();
	keyPDU.EndLevel();
	if (keyType == KeyType::RSAPublic)
	{
		keyPDU.AppendBitString(0, buff, buffSize);
	}
	else
	{
		keyPDU.AppendBitString(0, buff, buffSize);
	}
	keyPDU.EndLevel();
	NN<Crypto::Cert::X509PubKey> key;
	NEW_CLASSNN(key, Crypto::Cert::X509PubKey(sourceName, keyPDU.GetArray()));
	return key;
}

NN<Crypto::Cert::X509PubKey> Crypto::Cert::X509PubKey::CreateFromKey(NN<Crypto::Cert::X509Key> key)
{
	return CreateFromKeyBuff(key->GetKeyType(), key->GetASN1Buff(), key->GetASN1BuffSize(), key->GetSourceNameObj());
}
