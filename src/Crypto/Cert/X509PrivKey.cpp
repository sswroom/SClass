#include "Stdafx.h"
#include "Crypto/Cert/X509PrivKey.h"
#include "Net/ASN1PDUBuilder.h"
#include "Net/ASN1Util.h"

Crypto::Cert::X509PrivKey::X509PrivKey(Text::String *sourceName, const UInt8 *buff, UOSInt buffSize) : Crypto::Cert::X509File(sourceName, buff, buffSize)
{

}

Crypto::Cert::X509PrivKey::X509PrivKey(Text::CString sourceName, const UInt8 *buff, UOSInt buffSize) : Crypto::Cert::X509File(sourceName, buff, buffSize)
{

}

Crypto::Cert::X509PrivKey::~X509PrivKey()
{

}

Crypto::Cert::X509File::FileType Crypto::Cert::X509PrivKey::GetFileType()
{
	return FileType::PrivateKey;
}

void Crypto::Cert::X509PrivKey::ToShortName(Text::StringBuilderUTF8 *sb)
{
	UOSInt oidLen;
	Net::ASN1Util::ItemType itemType;
	const UInt8 *oidPDU = Net::ASN1Util::PDUGetItem(this->buff, this->buff + this->buffSize, "1.2.1", &oidLen, &itemType);
	if (oidPDU == 0 || itemType != Net::ASN1Util::IT_OID)
	{
		return;
	}
	KeyType keyType = KeyTypeFromOID(oidPDU, oidLen, false);
	UOSInt keyLen;
	const UInt8 *keyPDU = Net::ASN1Util::PDUGetItem(this->buff, this->buff + this->buffSize, "1.3", &keyLen, &itemType);
	if (keyPDU && itemType == Net::ASN1Util::IT_OCTET_STRING)
	{
		sb->Append(KeyTypeGetName(keyType));
		sb->AppendUTF8Char(' ');
		sb->AppendUOSInt(KeyGetLeng(keyPDU, keyPDU + keyLen, keyType));
		sb->AppendC(UTF8STRC(" bits"));
	}
}

Crypto::Cert::X509File::ValidStatus Crypto::Cert::X509PrivKey::IsValid(Net::SSLEngine *ssl, Crypto::Cert::CertStore *trustStore)
{
	return Crypto::Cert::X509File::ValidStatus::SignatureInvalid;
}

Net::ASN1Data *Crypto::Cert::X509PrivKey::Clone()
{
	Crypto::Cert::X509PrivKey *asn1;
	NEW_CLASS(asn1, Crypto::Cert::X509PrivKey(this->GetSourceNameObj(), this->buff, this->buffSize));
	return asn1;
}

void Crypto::Cert::X509PrivKey::ToString(Text::StringBuilderUTF8 *sb)
{
	if (IsPrivateKeyInfo(this->buff, this->buff + this->buffSize, "1"))
	{
		AppendPrivateKeyInfo(this->buff, this->buff + this->buffSize, "1", sb);
	}
}

Crypto::Cert::X509Key *Crypto::Cert::X509PrivKey::CreateKey()
{
	Net::ASN1Util::ItemType itemType;
	UOSInt keyTypeLen;
	UOSInt keyDataLen;
	const UInt8 *keyTypeOID = Net::ASN1Util::PDUGetItem(this->buff, this->buff + this->buffSize, "1.2.1", &keyTypeLen, &itemType);
	const UInt8 *keyData = Net::ASN1Util::PDUGetItem(this->buff, this->buff + this->buffSize, "1.3", &keyDataLen, &itemType);
	if (keyTypeOID != 0 && keyData != 0)
	{
		Crypto::Cert::X509Key *key;
		NEW_CLASS(key, Crypto::Cert::X509Key(this->GetSourceNameObj(), keyData, keyDataLen, KeyTypeFromOID(keyTypeOID, keyTypeLen, false)));
		return key;
	}
	return 0;
}

Crypto::Cert::X509PrivKey *Crypto::Cert::X509PrivKey::CreateFromKeyBuff(KeyType keyType, const UInt8 *buff, UOSInt buffSize, Text::String *sourceName)
{
	if (sourceName == 0)
	{
		sourceName = Text::String::NewEmpty();
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
	NEW_CLASS(key, Crypto::Cert::X509PrivKey(sourceName, keyPDU.GetBuff(0), keyPDU.GetBuffSize()));
	return key;
}

Crypto::Cert::X509PrivKey *Crypto::Cert::X509PrivKey::CreateFromKey(Crypto::Cert::X509Key *key)
{
	return CreateFromKeyBuff(key->GetKeyType(), key->GetASN1Buff(), key->GetASN1BuffSize(), key->GetSourceNameObj());
}
