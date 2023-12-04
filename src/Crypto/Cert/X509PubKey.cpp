#include "Stdafx.h"
#include "Crypto/Cert/X509PubKey.h"
#include "Net/ASN1PDUBuilder.h"
#include "Net/ASN1Util.h"

Crypto::Cert::X509PubKey::X509PubKey(NotNullPtr<Text::String> sourceName, Data::ByteArrayR buff) : Crypto::Cert::X509File(sourceName, buff)
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

void Crypto::Cert::X509PubKey::ToShortName(NotNullPtr<Text::StringBuilderUTF8> sb) const
{
	UOSInt oidLen;
	Net::ASN1Util::ItemType itemType;
	const UInt8 *oidPDU = Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.1.1", oidLen, itemType);
	if (oidPDU == 0 || itemType != Net::ASN1Util::IT_OID)
	{
		return;
	}
	KeyType keyType = KeyTypeFromOID(oidPDU, oidLen, true);
	UOSInt keyLen;
	const UInt8 *keyPDU = Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.2", keyLen, itemType);
	if (keyPDU && itemType == Net::ASN1Util::IT_OCTET_STRING)
	{
		sb->Append(KeyTypeGetName(keyType));
		sb->AppendUTF8Char(' ');
		sb->AppendUOSInt(KeyGetLeng(keyPDU, keyPDU + keyLen, keyType));
		sb->AppendC(UTF8STRC(" bits"));
	}
}

Crypto::Cert::X509File::ValidStatus Crypto::Cert::X509PubKey::IsValid(NotNullPtr<Net::SSLEngine> ssl, Crypto::Cert::CertStore *trustStore) const
{
	return Crypto::Cert::X509File::ValidStatus::SignatureInvalid;
}

NotNullPtr<Net::ASN1Data> Crypto::Cert::X509PubKey::Clone() const
{
	NotNullPtr<Crypto::Cert::X509PubKey> asn1;
	NEW_CLASSNN(asn1, Crypto::Cert::X509PubKey(this->GetSourceNameObj(), this->buff));
	return asn1;
}

void Crypto::Cert::X509PubKey::ToString(NotNullPtr<Text::StringBuilderUTF8> sb) const
{
	if (IsPublicKeyInfo(this->buff.Ptr(), this->buff.PtrEnd(), "1"))
	{
		AppendPublicKeyInfo(this->buff.Ptr(), this->buff.PtrEnd(), "1", sb);
	}
}

Net::ASN1Names *Crypto::Cert::X509PubKey::CreateNames() const
{
	return 0;
}

Crypto::Cert::X509Key *Crypto::Cert::X509PubKey::CreateKey() const
{
	Net::ASN1Util::ItemType itemType;
	UOSInt keyTypeLen;
	UOSInt keyDataLen;
	const UInt8 *keyTypeOID = Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.1.1", keyTypeLen, itemType);
	const UInt8 *keyData = Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.2", keyDataLen, itemType);
	if (keyTypeOID != 0 && keyData != 0)
	{
		Crypto::Cert::X509Key *key;
		if (keyData[0] == 0)
		{
			keyData++;
			keyDataLen--;
		}
		NEW_CLASS(key, Crypto::Cert::X509Key(this->GetSourceNameObj(), Data::ByteArrayR(keyData, keyDataLen), KeyTypeFromOID(keyTypeOID, keyTypeLen, true)));
		return key;
	}
	return 0;
}

Crypto::Cert::X509PubKey *Crypto::Cert::X509PubKey::CreateFromKeyBuff(KeyType keyType, const UInt8 *buff, UOSInt buffSize, NotNullPtr<Text::String> sourceName)
{
	Net::ASN1PDUBuilder keyPDU;
	keyPDU.BeginSequence();
	keyPDU.BeginSequence();
	Text::CString oidStr = KeyTypeGetOID(keyType);
	keyPDU.AppendOIDString(oidStr.v, oidStr.leng);
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
	Crypto::Cert::X509PubKey *key;
	NEW_CLASS(key, Crypto::Cert::X509PubKey(sourceName, keyPDU.GetArray()));
	return key;
}

Crypto::Cert::X509PubKey *Crypto::Cert::X509PubKey::CreateFromKey(NotNullPtr<Crypto::Cert::X509Key> key)
{
	return CreateFromKeyBuff(key->GetKeyType(), key->GetASN1Buff(), key->GetASN1BuffSize(), key->GetSourceNameObj());
}
