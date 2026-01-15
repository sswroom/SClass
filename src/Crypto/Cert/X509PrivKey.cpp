#include "Stdafx.h"
#include "Crypto/Cert/X509PrivKey.h"
#include "Net/ASN1PDUBuilder.h"
#include "Net/ASN1Util.h"

Crypto::Cert::X509PrivKey::X509PrivKey(NN<Text::String> sourceName, Data::ByteArrayR buff) : Crypto::Cert::X509File(sourceName, buff)
{

}

Crypto::Cert::X509PrivKey::X509PrivKey(Text::CStringNN sourceName, Data::ByteArrayR buff) : Crypto::Cert::X509File(sourceName, buff)
{

}

Crypto::Cert::X509PrivKey::~X509PrivKey()
{

}

Crypto::Cert::X509File::FileType Crypto::Cert::X509PrivKey::GetFileType() const
{
	return FileType::PrivateKey;
}

void Crypto::Cert::X509PrivKey::ToShortName(NN<Text::StringBuilderUTF8> sb) const
{
	UOSInt oidLen;
	Net::ASN1Util::ItemType itemType;
	UnsafeArray<const UInt8> oidPDU;
	if (!Net::ASN1Util::PDUGetItem(this->buff.Arr(), this->buff.ArrEnd(), "1.2.1", oidLen, itemType).SetTo(oidPDU) || itemType != Net::ASN1Util::IT_OID)
	{
		return;
	}
	KeyType keyType = KeyTypeFromOID(Data::ByteArrayR(oidPDU, oidLen), false);
	UOSInt keyLen;
	UnsafeArray<const UInt8> keyPDU;
	if (Net::ASN1Util::PDUGetItem(this->buff.Arr(), this->buff.ArrEnd(), "1.3", keyLen, itemType).SetTo(keyPDU) && itemType == Net::ASN1Util::IT_OCTET_STRING)
	{
		sb->Append(KeyTypeGetName(keyType));
		sb->AppendUTF8Char(' ');
		sb->AppendUOSInt(KeyGetLeng(keyPDU, keyPDU + keyLen, keyType));
		sb->AppendC(UTF8STRC(" bits"));
	}
}

Crypto::Cert::X509File::ValidStatus Crypto::Cert::X509PrivKey::IsValid(NN<Net::SSLEngine> ssl, Optional<Crypto::Cert::CertStore> trustStore) const
{
	return Crypto::Cert::X509File::ValidStatus::SignatureInvalid;
}

NN<Net::ASN1Data> Crypto::Cert::X509PrivKey::Clone() const
{
	NN<Crypto::Cert::X509PrivKey> asn1;
	NEW_CLASSNN(asn1, Crypto::Cert::X509PrivKey(this->GetSourceNameObj(), this->buff));
	return asn1;
}

void Crypto::Cert::X509PrivKey::ToString(NN<Text::StringBuilderUTF8> sb) const
{
	if (IsPrivateKeyInfo(this->buff.Arr(), this->buff.ArrEnd(), "1"))
	{
		AppendPrivateKeyInfo(this->buff.Arr(), this->buff.ArrEnd(), "1", sb);
	}
}

NN<Net::ASN1Names> Crypto::Cert::X509PrivKey::CreateNames() const
{
	NN<Net::ASN1Names> names;
	NEW_CLASSNN(names, Net::ASN1Names());
	return names;;
}

Crypto::Cert::X509File::KeyType Crypto::Cert::X509PrivKey::GetKeyType() const
{
	Net::ASN1Util::ItemType itemType;
	UOSInt keyTypeLen;
	UnsafeArray<const UInt8> keyTypeOID;
	if (Net::ASN1Util::PDUGetItem(this->buff.Arr(), this->buff.ArrEnd(), "1.2.1", keyTypeLen, itemType).SetTo(keyTypeOID))
	{
		return KeyTypeFromOID(Data::ByteArrayR(keyTypeOID, keyTypeLen), false);
	}
	return Crypto::Cert::X509File::KeyType::Unknown;
}

Optional<Crypto::Cert::X509Key> Crypto::Cert::X509PrivKey::CreateKey() const
{
	KeyType keyType = GetKeyType();
	if (keyType == KeyType::Unknown)
	{
		return nullptr;
	}
	Net::ASN1Util::ItemType itemType;
	UOSInt keyDataLen;
	UnsafeArray<const UInt8> keyData;
	if (Net::ASN1Util::PDUGetItem(this->buff.Arr(), this->buff.ArrEnd(), "1.3", keyDataLen, itemType).SetTo(keyData))
	{
		Crypto::Cert::X509Key *key;
		NEW_CLASS(key, Crypto::Cert::X509Key(this->GetSourceNameObj(), Data::ByteArrayR(keyData, keyDataLen), keyType));
		return key;
	}
	return nullptr;
}

Bool Crypto::Cert::X509PrivKey::GetKeyId(const Data::ByteArray &keyId) const
{
	NN<X509Key> key;
	if (CreateKey().SetTo(key))
	{
		Bool ret = key->GetKeyId(keyId);
		key.Delete();
		return ret;
	}
	return false;
}

NN<Crypto::Cert::X509PrivKey> Crypto::Cert::X509PrivKey::CreateFromKeyBuff(KeyType keyType, UnsafeArray<const UInt8> buff, UOSInt buffSize, Optional<Text::String> sourceName)
{
	NN<Text::String> mySourceName;
	if (!sourceName.SetTo(mySourceName))
	{
		mySourceName = Text::String::NewEmpty();
	}
	Net::ASN1PDUBuilder keyPDU;
	keyPDU.BeginSequence();
	keyPDU.AppendInt32(0);
	keyPDU.BeginSequence();
	Text::CStringNN oidStr = KeyTypeGetOID(keyType);
	keyPDU.AppendOIDString(oidStr);
	keyPDU.AppendNull();
	keyPDU.EndLevel();
	keyPDU.AppendOctetString(buff, buffSize);
	keyPDU.EndLevel();
	NN<Crypto::Cert::X509PrivKey> key;
	NEW_CLASSNN(key, Crypto::Cert::X509PrivKey(mySourceName, keyPDU.GetArray()));
	return key;
}

Optional<Crypto::Cert::X509PrivKey> Crypto::Cert::X509PrivKey::CreateFromKey(NN<Crypto::Cert::X509Key> key)
{
	KeyType keyType = key->GetKeyType();
	if (keyType == KeyType::ECDSA)
	{
		ECName ecName = key->GetECName();
		UOSInt keyBuffLen;
		UnsafeArray<const UInt8> keyBuff;
		Net::ASN1PDUBuilder keyPDU;
		keyPDU.BeginSequence();
		keyPDU.AppendInt32(0);
		keyPDU.BeginSequence();
		Text::CStringNN oidStr = KeyTypeGetOID(keyType);
		keyPDU.AppendOIDString(oidStr);
		oidStr = ECNameGetOID(ecName);
		keyPDU.AppendOIDString(oidStr);
		keyPDU.EndLevel();
		keyPDU.BeginOther(4);
		keyPDU.BeginSequence();
		keyPDU.AppendInt32(1);
		if (!key->GetECPrivate(keyBuffLen).SetTo(keyBuff))
			return nullptr;
		keyPDU.AppendOctetString(keyBuff, keyBuffLen);
		if (key->GetECPublic(keyBuffLen).SetTo(keyBuff))
		{
			keyPDU.BeginContentSpecific(1);
			keyPDU.AppendBitString(0, Data::ByteArrayR(keyBuff, keyBuffLen));
			keyPDU.EndLevel();
		}
		keyPDU.EndLevel();
		keyPDU.EndLevel();
		keyPDU.EndLevel();
		NN<Crypto::Cert::X509PrivKey> pkey;
		NEW_CLASSNN(pkey, Crypto::Cert::X509PrivKey(key->GetSourceNameObj(), keyPDU.GetArray()));
		return pkey;
	}
	else if (keyType == KeyType::RSA)
	{
		return CreateFromKeyBuff(keyType, key->GetASN1Buff(), key->GetASN1BuffSize(), key->GetSourceNameObj().Ptr());
	}
	else
	{
		return nullptr;
	}
}
