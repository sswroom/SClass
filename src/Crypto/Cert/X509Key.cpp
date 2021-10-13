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
	return FileType::Key;
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

const UInt8 *Crypto::Cert::X509Key::GetRSAModulus(UOSInt *size)
{
	if (this->keyType != KeyType::RSA) return 0;
	return Net::ASN1Util::PDUGetItem(this->buff, this->buff + this->buffSize, "1.2", size, 0);
}

const UInt8 *Crypto::Cert::X509Key::GetRSAPublicExponent(UOSInt *size)
{
	if (this->keyType != KeyType::RSA) return 0;
	return Net::ASN1Util::PDUGetItem(this->buff, this->buff + this->buffSize, "1.3", size, 0);
}

const UInt8 *Crypto::Cert::X509Key::GetRSAPrivateExponent(UOSInt *size)
{
	if (this->keyType != KeyType::RSA) return 0;
	return Net::ASN1Util::PDUGetItem(this->buff, this->buff + this->buffSize, "1.4", size, 0);
}

const UInt8 *Crypto::Cert::X509Key::GetRSAPrime1(UOSInt *size)
{
	if (this->keyType != KeyType::RSA) return 0;
	return Net::ASN1Util::PDUGetItem(this->buff, this->buff + this->buffSize, "1.5", size, 0);
}

const UInt8 *Crypto::Cert::X509Key::GetRSAPrime2(UOSInt *size)
{
	if (this->keyType != KeyType::RSA) return 0;
	return Net::ASN1Util::PDUGetItem(this->buff, this->buff + this->buffSize, "1.6", size, 0);
}

const UInt8 *Crypto::Cert::X509Key::GetRSAExponent1(UOSInt *size)
{
	if (this->keyType != KeyType::RSA) return 0;
	return Net::ASN1Util::PDUGetItem(this->buff, this->buff + this->buffSize, "1.7", size, 0);
}

const UInt8 *Crypto::Cert::X509Key::GetRSAExponent2(UOSInt *size)
{
	if (this->keyType != KeyType::RSA) return 0;
	return Net::ASN1Util::PDUGetItem(this->buff, this->buff + this->buffSize, "1.8", size, 0);
}

const UInt8 *Crypto::Cert::X509Key::GetRSACoefficient(UOSInt *size)
{
	if (this->keyType != KeyType::RSA) return 0;
	return Net::ASN1Util::PDUGetItem(this->buff, this->buff + this->buffSize, "1.9", size, 0);
}
