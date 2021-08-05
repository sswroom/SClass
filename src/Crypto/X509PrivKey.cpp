#include "Stdafx.h"
#include "Crypto/X509PrivKey.h"
#include "Net/ASN1PDUBuilder.h"

Crypto::X509PrivKey::X509PrivKey(const UTF8Char *sourceName, const UInt8 *buff, UOSInt buffSize) : Crypto::X509File(sourceName, buff, buffSize)
{

}

Crypto::X509PrivKey::~X509PrivKey()
{

}

Crypto::X509File::FileType Crypto::X509PrivKey::GetFileType()
{
	return FT_PRIV_KEY;
}

void Crypto::X509PrivKey::ToString(Text::StringBuilderUTF *sb)
{
	if (IsPrivateKeyInfo(this->buff, this->buff + this->buffSize, "1"))
	{
		AppendPrivateKeyInfo(this->buff, this->buff + this->buffSize, "1", sb);
	}
}

Crypto::X509PrivKey *Crypto::X509PrivKey::CreateFromRSAKey(const UInt8 *buff, UOSInt buffSize)
{
	Net::ASN1PDUBuilder keyPDU;
	keyPDU.BeginSequence();
	keyPDU.AppendInt32(0);
	keyPDU.BeginSequence();
	keyPDU.AppendOIDString("1.2.840.113549.1.1.1");
	keyPDU.AppendNull();
	keyPDU.EndLevel();
	keyPDU.AppendOctetString(buff, buffSize);
	keyPDU.EndLevel();
	Crypto::X509PrivKey *key;
	NEW_CLASS(key, Crypto::X509PrivKey((const UTF8Char*)"RSAKey", keyPDU.GetBuff(0), keyPDU.GetBuffSize()));
	return key;
}
