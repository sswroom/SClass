#include "Stdafx.h"
#include "Crypto/Cert/X509PrivKey.h"
#include "Net/ASN1PDUBuilder.h"
#include "Net/ASN1Util.h"

Crypto::Cert::X509PrivKey::X509PrivKey(NotNullPtr<Text::String> sourceName, Data::ByteArrayR buff) : Crypto::Cert::X509File(sourceName, buff)
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

void Crypto::Cert::X509PrivKey::ToShortName(NotNullPtr<Text::StringBuilderUTF8> sb) const
{
	UOSInt oidLen;
	Net::ASN1Util::ItemType itemType;
	const UInt8 *oidPDU = Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.2.1", oidLen, itemType);
	if (oidPDU == 0 || itemType != Net::ASN1Util::IT_OID)
	{
		return;
	}
	KeyType keyType = KeyTypeFromOID(oidPDU, oidLen, false);
	UOSInt keyLen;
	const UInt8 *keyPDU = Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.3", keyLen, itemType);
	if (keyPDU && itemType == Net::ASN1Util::IT_OCTET_STRING)
	{
		sb->Append(KeyTypeGetName(keyType));
		sb->AppendUTF8Char(' ');
		sb->AppendUOSInt(KeyGetLeng(keyPDU, keyPDU + keyLen, keyType));
		sb->AppendC(UTF8STRC(" bits"));
	}
}

Crypto::Cert::X509File::ValidStatus Crypto::Cert::X509PrivKey::IsValid(NotNullPtr<Net::SSLEngine> ssl, Crypto::Cert::CertStore *trustStore) const
{
	return Crypto::Cert::X509File::ValidStatus::SignatureInvalid;
}

NotNullPtr<Net::ASN1Data> Crypto::Cert::X509PrivKey::Clone() const
{
	NotNullPtr<Crypto::Cert::X509PrivKey> asn1;
	NEW_CLASSNN(asn1, Crypto::Cert::X509PrivKey(this->GetSourceNameObj(), this->buff));
	return asn1;
}

void Crypto::Cert::X509PrivKey::ToString(NotNullPtr<Text::StringBuilderUTF8> sb) const
{
	if (IsPrivateKeyInfo(this->buff.Ptr(), this->buff.PtrEnd(), "1"))
	{
		AppendPrivateKeyInfo(this->buff.Ptr(), this->buff.PtrEnd(), "1", sb);
	}
}

Net::ASN1Names *Crypto::Cert::X509PrivKey::CreateNames() const
{
	return 0;
}

Crypto::Cert::X509File::KeyType Crypto::Cert::X509PrivKey::GetKeyType() const
{
	Net::ASN1Util::ItemType itemType;
	UOSInt keyTypeLen;
	const UInt8 *keyTypeOID = Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.2.1", keyTypeLen, itemType);
	if (keyTypeOID != 0)
	{
		return KeyTypeFromOID(keyTypeOID, keyTypeLen, false);
	}
	return Crypto::Cert::X509File::KeyType::Unknown;
}

Crypto::Cert::X509Key *Crypto::Cert::X509PrivKey::CreateKey() const
{
	KeyType keyType = GetKeyType();
	if (keyType == KeyType::Unknown)
	{
		return 0;
	}
	Net::ASN1Util::ItemType itemType;
	UOSInt keyDataLen;
	const UInt8 *keyData = Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.3", keyDataLen, itemType);
	if (keyData != 0)
	{
		Crypto::Cert::X509Key *key;
		NEW_CLASS(key, Crypto::Cert::X509Key(this->GetSourceNameObj(), Data::ByteArrayR(keyData, keyDataLen), keyType));
		return key;
	}
	return 0;
}

Crypto::Cert::X509PrivKey *Crypto::Cert::X509PrivKey::CreateFromKeyBuff(KeyType keyType, const UInt8 *buff, UOSInt buffSize, Text::String *sourceName)
{
	NotNullPtr<Text::String> mySourceName;
	if (!mySourceName.Set(sourceName))
	{
		mySourceName = Text::String::NewEmpty();
	}
	Net::ASN1PDUBuilder keyPDU;
	keyPDU.BeginSequence();
	keyPDU.AppendInt32(0);
	keyPDU.BeginSequence();
	Text::CString oidStr = KeyTypeGetOID(keyType);
	keyPDU.AppendOIDString(oidStr.v, oidStr.leng);
	keyPDU.AppendNull();
	keyPDU.EndLevel();
	keyPDU.AppendOctetString(buff, buffSize);
	keyPDU.EndLevel();
	Crypto::Cert::X509PrivKey *key;
	NEW_CLASS(key, Crypto::Cert::X509PrivKey(mySourceName, keyPDU.GetArray()));
	return key;
}

Crypto::Cert::X509PrivKey *Crypto::Cert::X509PrivKey::CreateFromKey(NotNullPtr<Crypto::Cert::X509Key> key)
{
	KeyType keyType = key->GetKeyType();
	if (keyType == KeyType::ECDSA)
	{
		ECName ecName = key->GetECName();
		UOSInt keyBuffLen;
		const UInt8 *keyBuff;
		Net::ASN1PDUBuilder keyPDU;
		keyPDU.BeginSequence();
		keyPDU.AppendInt32(0);
		keyPDU.BeginSequence();
		Text::CString oidStr = KeyTypeGetOID(keyType);
		keyPDU.AppendOIDString(oidStr.v, oidStr.leng);
		oidStr = ECNameGetOID(ecName);
		keyPDU.AppendOIDString(oidStr.v, oidStr.leng);
		keyPDU.EndLevel();
		keyPDU.BeginOther(4);
		keyPDU.BeginSequence();
		keyPDU.AppendInt32(1);
		keyBuff = key->GetECPrivate(keyBuffLen);
		keyPDU.AppendOctetStringC(keyBuff, keyBuffLen);
		keyBuff = key->GetECPublic(keyBuffLen);
		if (keyBuff)
		{
			keyPDU.BeginContentSpecific(1);
			keyPDU.AppendBitString(0, keyBuff, keyBuffLen);
			keyPDU.EndLevel();
		}
		keyPDU.EndLevel();
		keyPDU.EndLevel();
		keyPDU.EndLevel();
		Crypto::Cert::X509PrivKey *pkey;
		NEW_CLASS(pkey, Crypto::Cert::X509PrivKey(key->GetSourceNameObj(), keyPDU.GetArray()));
		return pkey;
	}
	else if (keyType == KeyType::RSA)
	{
		return CreateFromKeyBuff(keyType, key->GetASN1Buff(), key->GetASN1BuffSize(), key->GetSourceNameObj().Ptr());
	}
	else
	{
		return 0;
	}
}
