#include "Stdafx.h"
#include "Crypto/Cert/X509PubKey.h"
#include "Net/ASN1PDUBuilder.h"
#include "Net/ASN1Util.h"

Crypto::Cert::X509PubKey::X509PubKey(Text::String *sourceName, const UInt8 *buff, UOSInt buffSize) : Crypto::Cert::X509File(sourceName, buff, buffSize)
{

}

Crypto::Cert::X509PubKey::X509PubKey(const UTF8Char *sourceName, const UInt8 *buff, UOSInt buffSize) : Crypto::Cert::X509File(sourceName, buff, buffSize)
{

}

Crypto::Cert::X509PubKey::~X509PubKey()
{

}

Crypto::Cert::X509File::FileType Crypto::Cert::X509PubKey::GetFileType()
{
	return FileType::PublicKey;
}

void Crypto::Cert::X509PubKey::ToShortName(Text::StringBuilderUTF8 *sb)
{
	UOSInt oidLen;
	Net::ASN1Util::ItemType itemType;
	const UInt8 *oidPDU = Net::ASN1Util::PDUGetItem(this->buff, this->buff + this->buffSize, "1.1.1", &oidLen, &itemType);
	if (oidPDU == 0 || itemType != Net::ASN1Util::IT_OID)
	{
		return;
	}
	KeyType keyType = KeyTypeFromOID(oidPDU, oidLen, true);
	UOSInt keyLen;
	const UInt8 *keyPDU = Net::ASN1Util::PDUGetItem(this->buff, this->buff + this->buffSize, "1.2", &keyLen, &itemType);
	if (keyPDU && itemType == Net::ASN1Util::IT_OCTET_STRING)
	{
		sb->Append(KeyTypeGetName(keyType));
		sb->AppendChar(' ', 1);
		sb->AppendUOSInt(KeyGetLeng(keyPDU, keyPDU + keyLen, keyType));
		sb->AppendC(UTF8STRC(" bits"));
	}
}

Net::ASN1Data *Crypto::Cert::X509PubKey::Clone()
{
	Crypto::Cert::X509PubKey *asn1;
	NEW_CLASS(asn1, Crypto::Cert::X509PubKey(this->GetSourceNameObj(), this->buff, this->buffSize));
	return asn1;
}

void Crypto::Cert::X509PubKey::ToString(Text::StringBuilderUTF8 *sb)
{
	if (IsPublicKeyInfo(this->buff, this->buff + this->buffSize, "1"))
	{
		AppendPublicKeyInfo(this->buff, this->buff + this->buffSize, "1", sb);
	}
}

Crypto::Cert::X509Key *Crypto::Cert::X509PubKey::CreateKey()
{
	Net::ASN1Util::ItemType itemType;
	UOSInt keyTypeLen;
	UOSInt keyDataLen;
	const UInt8 *keyTypeOID = Net::ASN1Util::PDUGetItem(this->buff, this->buff + this->buffSize, "1.1.1", &keyTypeLen, &itemType);
	const UInt8 *keyData = Net::ASN1Util::PDUGetItem(this->buff, this->buff + this->buffSize, "1.2", &keyDataLen, &itemType);
	if (keyTypeOID != 0 && keyData != 0)
	{
		Crypto::Cert::X509Key *key;
		if (keyData[0] == 0)
		{
			keyData++;
			keyDataLen--;
		}
		NEW_CLASS(key, Crypto::Cert::X509Key(this->GetSourceNameObj(), keyData, keyDataLen, KeyTypeFromOID(keyTypeOID, keyTypeLen, true)));
		return key;
	}
	return 0;
}

Crypto::Cert::X509PubKey *Crypto::Cert::X509PubKey::CreateFromKeyBuff(KeyType keyType, const UInt8 *buff, UOSInt buffSize, Text::String *sourceName)
{
	if (sourceName == 0)
	{
		sourceName = Text::String::NewEmpty();
	}
	Net::ASN1PDUBuilder keyPDU;
	keyPDU.BeginSequence();
	keyPDU.BeginSequence();
	Text::CString oidStr = KeyTypeGetOID(keyType);
	keyPDU.AppendOIDString(oidStr.v, oidStr.leng);
	keyPDU.AppendNull();
	keyPDU.EndLevel();
	if (keyType == KeyType::RSAPublic)
	{
		UInt8 *tmpBuff = MemAlloc(UInt8, buffSize + 1);
		MemCopyNO(&tmpBuff[1], buff, buffSize);
		tmpBuff[0] = 0;
		keyPDU.AppendBitString(tmpBuff, buffSize + 1);
		MemFree(tmpBuff);
	}
	else
	{
		keyPDU.AppendBitString(buff, buffSize);
	}
	keyPDU.EndLevel();
	Crypto::Cert::X509PubKey *key;
	NEW_CLASS(key, Crypto::Cert::X509PubKey(sourceName, keyPDU.GetBuff(0), keyPDU.GetBuffSize()));
	return key;
}

Crypto::Cert::X509PubKey *Crypto::Cert::X509PubKey::CreateFromKey(Crypto::Cert::X509Key *key)
{
	return CreateFromKeyBuff(key->GetKeyType(), key->GetASN1Buff(), key->GetASN1BuffSize(), key->GetSourceNameObj());
}
