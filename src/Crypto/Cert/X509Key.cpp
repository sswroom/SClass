#include "Stdafx.h"
#include "Crypto/Cert/X509Key.h"
#include "Net/ASN1Util.h"

Crypto::Cert::X509Key::X509Key(const UTF8Char *sourceName, const UInt8 *buff, UOSInt buffSize, KeyType keyType) : Crypto::Cert::X509File(sourceName, buff, buffSize)
{
	this->keyType = keyType;
}

Crypto::Cert::X509Key::~X509Key()
{

}

Crypto::Cert::X509File::FileType Crypto::Cert::X509Key::GetFileType()
{
	return FT_KEY;
}

void Crypto::Cert::X509Key::ToShortName(Text::StringBuilderUTF *sb)
{
	sb->Append(KeyTypeGetName(this->keyType));
	sb->AppendChar(' ', 1);
	sb->AppendUOSInt(KeyGetLeng(this->buff, this->buff + this->buffSize, this->keyType));
	sb->Append((const UTF8Char*)" bits");
}

Net::ASN1Data *Crypto::Cert::X509Key::Clone()
{
	Crypto::Cert::X509Key *asn1;
	NEW_CLASS(asn1, Crypto::Cert::X509Key(this->GetSourceNameObj(), this->buff, this->buffSize, this->keyType));
	return asn1;
}

void Crypto::Cert::X509Key::ToString(Text::StringBuilderUTF *sb)
{
}

Crypto::Cert::X509File::KeyType Crypto::Cert::X509Key::GetKeyType()
{
	return this->keyType;
}
