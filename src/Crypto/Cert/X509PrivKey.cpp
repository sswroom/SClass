#include "Stdafx.h"
#include "Crypto/Cert/X509PrivKey.h"
#include "Net/ASN1PDUBuilder.h"
#include "Net/ASN1Util.h"

Crypto::Cert::X509PrivKey::X509PrivKey(const UTF8Char *sourceName, const UInt8 *buff, UOSInt buffSize) : Crypto::Cert::X509File(sourceName, buff, buffSize)
{

}

Crypto::Cert::X509PrivKey::~X509PrivKey()
{

}

Crypto::Cert::X509File::FileType Crypto::Cert::X509PrivKey::GetFileType()
{
	return FileType::PrivateKey;
}

void Crypto::Cert::X509PrivKey::ToShortName(Text::StringBuilderUTF *sb)
{
	UOSInt oidLen;
	Net::ASN1Util::ItemType itemType;
	const UInt8 *oidPDU = Net::ASN1Util::PDUGetItem(this->buff, this->buff + this->buffSize, "1.2.1", &oidLen, &itemType);
	if (oidPDU == 0 || itemType != Net::ASN1Util::IT_OID)
	{
		return;
	}
	KeyType keyType = KeyTypeFromOID(oidPDU, oidLen);
	UOSInt keyLen;
	const UInt8 *keyPDU = Net::ASN1Util::PDUGetItem(this->buff, this->buff + this->buffSize, "1.3", &keyLen, &itemType);
	if (keyPDU && itemType == Net::ASN1Util::IT_OCTET_STRING)
	{
		sb->Append(KeyTypeGetName(keyType));
		sb->AppendChar(' ', 1);
		sb->AppendUOSInt(KeyGetLeng(keyPDU, keyPDU + keyLen, keyType));
		sb->Append((const UTF8Char*)" bits");
	}
}

Net::ASN1Data *Crypto::Cert::X509PrivKey::Clone()
{
	Crypto::Cert::X509PrivKey *asn1;
	NEW_CLASS(asn1, Crypto::Cert::X509PrivKey(this->GetSourceNameObj(), this->buff, this->buffSize));
	return asn1;
}

void Crypto::Cert::X509PrivKey::ToString(Text::StringBuilderUTF *sb)
{
	if (IsPrivateKeyInfo(this->buff, this->buff + this->buffSize, "1"))
	{
		AppendPrivateKeyInfo(this->buff, this->buff + this->buffSize, "1", sb);
	}
}

Crypto::Cert::X509PrivKey *Crypto::Cert::X509PrivKey::CreateFromKeyBuff(KeyType keyType, const UInt8 *buff, UOSInt buffSize)
{
	Net::ASN1PDUBuilder keyPDU;
	keyPDU.BeginSequence();
	keyPDU.AppendInt32(0);
	keyPDU.BeginSequence();
	keyPDU.AppendOIDString(KeyTypeGetOID(keyType));
	keyPDU.AppendNull();
	keyPDU.EndLevel();
	keyPDU.AppendOctetString(buff, buffSize);
	keyPDU.EndLevel();
	Crypto::Cert::X509PrivKey *key;
	NEW_CLASS(key, Crypto::Cert::X509PrivKey((const UTF8Char*)"PrivKey", keyPDU.GetBuff(0), keyPDU.GetBuffSize()));
	return key;
}

Crypto::Cert::X509PrivKey *Crypto::Cert::X509PrivKey::CreateFromKey(Crypto::Cert::X509Key *key)
{
	Net::ASN1PDUBuilder keyPDU;
	keyPDU.BeginSequence();
	keyPDU.AppendInt32(0);
	keyPDU.BeginSequence();
	keyPDU.AppendOIDString(KeyTypeGetOID(key->GetKeyType()));
	keyPDU.AppendNull();
	keyPDU.EndLevel();
	keyPDU.AppendOctetString(key->GetASN1Buff(), key->GetASN1BuffSize());
	keyPDU.EndLevel();
	Crypto::Cert::X509PrivKey *privKey;
	NEW_CLASS(privKey, Crypto::Cert::X509PrivKey(key->GetSourceNameObj(), keyPDU.GetBuff(0), keyPDU.GetBuffSize()));
	return privKey;
}
