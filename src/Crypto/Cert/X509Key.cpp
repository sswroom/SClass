#include "Stdafx.h"
#include "Crypto/Cert/X509Key.h"
#include "Crypto/Hash/SHA1.h"
#include "Net/ASN1PDUBuilder.h"
#include "Net/ASN1Util.h"

Crypto::Cert::X509Key::X509Key(Text::String *sourceName, const UInt8 *buff, UOSInt buffSize, KeyType keyType) : Crypto::Cert::X509File(sourceName, buff, buffSize)
{
	this->keyType = keyType;
}

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

void Crypto::Cert::X509Key::ToShortName(Text::StringBuilderUTF8 *sb)
{
	sb->Append(KeyTypeGetName(this->keyType));
	sb->AppendChar(' ', 1);
	sb->AppendUOSInt(this->GetKeySizeBits());
	sb->AppendC(UTF8STRC(" bits"));
}

Net::ASN1Data *Crypto::Cert::X509Key::Clone()
{
	Crypto::Cert::X509Key *asn1;
	NEW_CLASS(asn1, Crypto::Cert::X509Key(this->GetSourceNameObj(), this->buff, this->buffSize, this->keyType));
	return asn1;
}

void Crypto::Cert::X509Key::ToString(Text::StringBuilderUTF8 *sb)
{
	Bool found = false;
	if (this->keyType == KeyType::RSA)
	{
		const UInt8 *buff;
		UOSInt buffSize;
		buff = this->GetRSAModulus(&buffSize);
		if (buff)
		{
			if (found) sb->AppendLB(Text::LineBreakType::CRLF);
			found = true;
			sb->AppendC(UTF8STRC("RSA.Modulus = "));
			sb->AppendHexBuff(buff, buffSize, ' ', Text::LineBreakType::None);
		}
		buff = this->GetRSAPublicExponent(&buffSize);
		if (buff)
		{
			if (found) sb->AppendLB(Text::LineBreakType::CRLF);
			found = true;
			sb->AppendC(UTF8STRC("RSA.Public Exponent = "));
			sb->AppendHexBuff(buff, buffSize, ' ', Text::LineBreakType::None);
		}
		buff = this->GetRSAPrivateExponent(&buffSize);
		if (buff)
		{
			if (found) sb->AppendLB(Text::LineBreakType::CRLF);
			found = true;
			sb->AppendC(UTF8STRC("RSA.Private Exponent = "));
			sb->AppendHexBuff(buff, buffSize, ' ', Text::LineBreakType::None);
		}
		buff = this->GetRSAPrime1(&buffSize);
		if (buff)
		{
			if (found) sb->AppendLB(Text::LineBreakType::CRLF);
			found = true;
			sb->AppendC(UTF8STRC("RSA.Prime1 = "));
			sb->AppendHexBuff(buff, buffSize, ' ', Text::LineBreakType::None);
		}
		buff = this->GetRSAPrime2(&buffSize);
		if (buff)
		{
			if (found) sb->AppendLB(Text::LineBreakType::CRLF);
			found = true;
			sb->AppendC(UTF8STRC("RSA.Prime2 = "));
			sb->AppendHexBuff(buff, buffSize, ' ', Text::LineBreakType::None);
		}
		buff = this->GetRSAExponent1(&buffSize);
		if (buff)
		{
			if (found) sb->AppendLB(Text::LineBreakType::CRLF);
			found = true;
			sb->AppendC(UTF8STRC("RSA.Exponent1 = "));
			sb->AppendHexBuff(buff, buffSize, ' ', Text::LineBreakType::None);
		}
		buff = this->GetRSAExponent2(&buffSize);
		if (buff)
		{
			if (found) sb->AppendLB(Text::LineBreakType::CRLF);
			found = true;
			sb->AppendC(UTF8STRC("RSA.Exponent2 = "));
			sb->AppendHexBuff(buff, buffSize, ' ', Text::LineBreakType::None);
		}
		buff = this->GetRSACoefficient(&buffSize);
		if (buff)
		{
			if (found) sb->AppendLB(Text::LineBreakType::CRLF);
			found = true;
			sb->AppendC(UTF8STRC("RSA.Coefficient = "));
			sb->AppendHexBuff(buff, buffSize, ' ', Text::LineBreakType::None);
		}
	}
	else if (this->keyType == KeyType::RSAPublic)
	{
		const UInt8 *buff;
		UOSInt buffSize;
		buff = this->GetRSAModulus(&buffSize);
		if (buff)
		{
			if (found) sb->AppendLB(Text::LineBreakType::CRLF);
			found = true;
			sb->AppendC(UTF8STRC("RSA.Modulus = "));
			sb->AppendHexBuff(buff, buffSize, ' ', Text::LineBreakType::None);
		}
		buff = this->GetRSAPublicExponent(&buffSize);
		if (buff)
		{
			if (found) sb->AppendLB(Text::LineBreakType::CRLF);
			found = true;
			sb->AppendC(UTF8STRC("RSA.Public Exponent = "));
			sb->AppendHexBuff(buff, buffSize, ' ', Text::LineBreakType::None);
		}
	}

	UInt8 keyId[20];
	if (this->GetKeyId(keyId))
	{
		if (found) sb->AppendLB(Text::LineBreakType::CRLF);
		found = true;
		sb->AppendC(UTF8STRC("KeyId = "));
		sb->AppendHexBuff(keyId, 20, ' ', Text::LineBreakType::None);
	}
}

Crypto::Cert::X509File::KeyType Crypto::Cert::X509Key::GetKeyType()
{
	return this->keyType;
}

UOSInt Crypto::Cert::X509Key::GetKeySizeBits()
{
	return KeyGetLeng(this->buff, this->buff + this->buffSize, this->keyType);
}

Bool Crypto::Cert::X509Key::IsPrivateKey()
{
	switch (this->keyType)
	{
	case KeyType::DSA:
	case KeyType::ECDSA:
	case KeyType::ED25519:
	case KeyType::RSA:
		return true;
	case KeyType::RSAPublic:
	case KeyType::Unknown:
	default:
		return false;
	}
}

Crypto::Cert::X509Key *Crypto::Cert::X509Key::CreatePublicKey()
{
	if (this->keyType == KeyType::RSAPublic)
	{
		return (Crypto::Cert::X509Key*)this->Clone();
	}
	else if (this->keyType == KeyType::RSA)
	{
		Net::ASN1PDUBuilder builder;
		UOSInt buffSize;
		const UInt8 *buff;
		builder.BeginSequence();
		if ((buff = this->GetRSAModulus(&buffSize)) == 0) return 0;
		builder.AppendOther(Net::ASN1Util::IT_INTEGER, buff, buffSize);
		if ((buff = this->GetRSAPublicExponent(&buffSize)) == 0) return 0;
		builder.AppendOther(Net::ASN1Util::IT_INTEGER, buff, buffSize);
		builder.EndLevel();
		Crypto::Cert::X509Key *key;
		NEW_CLASS(key, Crypto::Cert::X509Key(this->GetSourceNameObj(), builder.GetBuff(&buffSize), builder.GetBuffSize(), KeyType::RSAPublic));
		return key;
	}
	else
	{
		return 0;
	}
}

Bool Crypto::Cert::X509Key::GetKeyId(UInt8 *keyId)
{
	Crypto::Cert::X509Key *pubKey = this->CreatePublicKey();
	if (pubKey)
	{
		Crypto::Hash::SHA1 sha1;
		sha1.Calc(pubKey->GetASN1Buff(), pubKey->GetASN1BuffSize());
		sha1.GetValue(keyId);
		DEL_CLASS(pubKey);
		return true;
	}
	return false;
}

const UInt8 *Crypto::Cert::X509Key::GetRSAModulus(UOSInt *size)
{
	if (this->keyType == KeyType::RSA)
	{
		return Net::ASN1Util::PDUGetItem(this->buff, this->buff + this->buffSize, "1.2", size, 0);
	}
	else if (this->keyType == KeyType::RSAPublic)
	{
		return Net::ASN1Util::PDUGetItem(this->buff, this->buff + this->buffSize, "1.1", size, 0);
	}
	else
	{
		return 0;
	}
}

const UInt8 *Crypto::Cert::X509Key::GetRSAPublicExponent(UOSInt *size)
{
	if (this->keyType == KeyType::RSA)
	{
		return Net::ASN1Util::PDUGetItem(this->buff, this->buff + this->buffSize, "1.3", size, 0);
	}
	else if (this->keyType == KeyType::RSAPublic)
	{
		return Net::ASN1Util::PDUGetItem(this->buff, this->buff + this->buffSize, "1.2", size, 0);
	}
	else
	{
		return 0;
	}
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
