#include "Stdafx.h"
#include "Crypto/Cert/X509PrivKey.h"
#include "Net/ASN1PDUBuilder.h"

Crypto::Cert::X509PrivKey::X509PrivKey(const UTF8Char *sourceName, const UInt8 *buff, UOSInt buffSize) : Crypto::Cert::X509File(sourceName, buff, buffSize)
{

}

Crypto::Cert::X509PrivKey::~X509PrivKey()
{

}

Crypto::Cert::X509File::FileType Crypto::Cert::X509PrivKey::GetFileType()
{
	return FT_PRIV_KEY;
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

Crypto::Cert::X509PrivKey *Crypto::Cert::X509PrivKey::CreateFromRSAKey(const UInt8 *buff, UOSInt buffSize)
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
	Crypto::Cert::X509PrivKey *key;
	NEW_CLASS(key, Crypto::Cert::X509PrivKey((const UTF8Char*)"RSAKey", keyPDU.GetBuff(0), keyPDU.GetBuffSize()));
	return key;
}
