#include "Stdafx.h"
#include "Crypto/Cert/X509Key.h"
#include "Crypto/Hash/SHA1.h"
#include "Net/ASN1PDUBuilder.h"
#include "Net/ASN1Util.h"

Crypto::Cert::X509Key::X509Key(NN<Text::String> sourceName, Data::ByteArrayR buff, KeyType keyType) : Crypto::Cert::X509File(sourceName, buff)
{
	this->keyType = keyType;
}

Crypto::Cert::X509Key::X509Key(Text::CStringNN sourceName, Data::ByteArrayR buff, KeyType keyType) : Crypto::Cert::X509File(sourceName, buff)
{
	this->keyType = keyType;
}

Crypto::Cert::X509Key::~X509Key()
{

}

Crypto::Cert::X509File::FileType Crypto::Cert::X509Key::GetFileType() const
{
	return FileType::Key;
}

void Crypto::Cert::X509Key::ToShortName(NN<Text::StringBuilderUTF8> sb) const
{
	sb->Append(KeyTypeGetName(this->keyType));
	sb->AppendUTF8Char(' ');
	sb->AppendUIntOS(this->GetKeySizeBits());
	sb->AppendC(UTF8STRC(" bits"));
}

Crypto::Cert::X509File::ValidStatus Crypto::Cert::X509Key::IsValid(NN<Net::SSLEngine> ssl, Optional<Crypto::Cert::CertStore> trustStore) const
{
	if (this->keyType == KeyType::Unknown)
	{
		return Crypto::Cert::X509File::ValidStatus::FileFormatInvalid;
	}
	return Crypto::Cert::X509File::ValidStatus::Valid;
}

NN<Net::ASN1Data> Crypto::Cert::X509Key::Clone() const
{
	NN<Crypto::Cert::X509Key> asn1;
	NEW_CLASSNN(asn1, Crypto::Cert::X509Key(this->GetSourceNameObj(), this->buff, this->keyType));
	return asn1;
}

void Crypto::Cert::X509Key::ToString(NN<Text::StringBuilderUTF8> sb) const
{
	Bool found = false;
	if (this->keyType == KeyType::RSA)
	{
		UnsafeArray<const UInt8> buff;
		UIntOS buffSize;
		if (this->GetRSAModulus(buffSize).SetTo(buff))
		{
			if (found) sb->AppendLB(Text::LineBreakType::CRLF);
			found = true;
			sb->Append(this->sourceName);
			sb->AppendUTF8Char('.');
			sb->AppendC(UTF8STRC("RSA.Modulus = "));
			sb->AppendHexBuff(buff, buffSize, ' ', Text::LineBreakType::None);
		}
		if (this->GetRSAPublicExponent(buffSize).SetTo(buff))
		{
			if (found) sb->AppendLB(Text::LineBreakType::CRLF);
			found = true;
			sb->Append(this->sourceName);
			sb->AppendUTF8Char('.');
			sb->AppendC(UTF8STRC("RSA.Public Exponent = "));
			sb->AppendHexBuff(buff, buffSize, ' ', Text::LineBreakType::None);
		}
		if (this->GetRSAPrivateExponent(buffSize).SetTo(buff))
		{
			if (found) sb->AppendLB(Text::LineBreakType::CRLF);
			found = true;
			sb->Append(this->sourceName);
			sb->AppendUTF8Char('.');
			sb->AppendC(UTF8STRC("RSA.Private Exponent = "));
			sb->AppendHexBuff(buff, buffSize, ' ', Text::LineBreakType::None);
		}
		if (this->GetRSAPrime1(buffSize).SetTo(buff))
		{
			if (found) sb->AppendLB(Text::LineBreakType::CRLF);
			found = true;
			sb->Append(this->sourceName);
			sb->AppendUTF8Char('.');
			sb->AppendC(UTF8STRC("RSA.Prime1 = "));
			sb->AppendHexBuff(buff, buffSize, ' ', Text::LineBreakType::None);
		}
		if (this->GetRSAPrime2(buffSize).SetTo(buff))
		{
			if (found) sb->AppendLB(Text::LineBreakType::CRLF);
			found = true;
			sb->Append(this->sourceName);
			sb->AppendUTF8Char('.');
			sb->AppendC(UTF8STRC("RSA.Prime2 = "));
			sb->AppendHexBuff(buff, buffSize, ' ', Text::LineBreakType::None);
		}
		if (this->GetRSAExponent1(buffSize).SetTo(buff))
		{
			if (found) sb->AppendLB(Text::LineBreakType::CRLF);
			found = true;
			sb->Append(this->sourceName);
			sb->AppendUTF8Char('.');
			sb->AppendC(UTF8STRC("RSA.Exponent1 = "));
			sb->AppendHexBuff(buff, buffSize, ' ', Text::LineBreakType::None);
		}
		if (this->GetRSAExponent2(buffSize).SetTo(buff))
		{
			if (found) sb->AppendLB(Text::LineBreakType::CRLF);
			found = true;
			sb->Append(this->sourceName);
			sb->AppendUTF8Char('.');
			sb->AppendC(UTF8STRC("RSA.Exponent2 = "));
			sb->AppendHexBuff(buff, buffSize, ' ', Text::LineBreakType::None);
		}
		if (this->GetRSACoefficient(buffSize).SetTo(buff))
		{
			if (found) sb->AppendLB(Text::LineBreakType::CRLF);
			found = true;
			sb->Append(this->sourceName);
			sb->AppendUTF8Char('.');
			sb->AppendC(UTF8STRC("RSA.Coefficient = "));
			sb->AppendHexBuff(buff, buffSize, ' ', Text::LineBreakType::None);
		}
	}
	else if (this->keyType == KeyType::RSAPublic)
	{
		UnsafeArray<const UInt8> buff;
		UIntOS buffSize;
		if (this->GetRSAModulus(buffSize).SetTo(buff))
		{
			if (found) sb->AppendLB(Text::LineBreakType::CRLF);
			found = true;
			sb->Append(this->sourceName);
			sb->AppendUTF8Char('.');
			sb->AppendC(UTF8STRC("RSA.Modulus = "));
			sb->AppendHexBuff(buff, buffSize, ' ', Text::LineBreakType::None);
		}
		if (this->GetRSAPublicExponent(buffSize).SetTo(buff))
		{
			if (found) sb->AppendLB(Text::LineBreakType::CRLF);
			found = true;
			sb->Append(this->sourceName);
			sb->AppendUTF8Char('.');
			sb->AppendC(UTF8STRC("RSA.Public Exponent = "));
			sb->AppendHexBuff(buff, buffSize, ' ', Text::LineBreakType::None);
		}
	}
	else if (this->keyType == KeyType::ECPublic)
	{
		UnsafeArray<const UInt8> buff;
		UIntOS buffSize;
		ECName ecName = this->GetECName();
		if (found) sb->AppendLB(Text::LineBreakType::CRLF);
		found = true;
		sb->Append(this->sourceName);
		sb->AppendUTF8Char('.');
		sb->AppendC(UTF8STRC("EC.Name = "));
		sb->Append(ECNameGetName(ecName));

		if (this->GetECPublic(buffSize).SetTo(buff))
		{
			if (found) sb->AppendLB(Text::LineBreakType::CRLF);
			found = true;
			sb->Append(this->sourceName);
			sb->AppendUTF8Char('.');
			sb->AppendC(UTF8STRC("EC.Public = "));
			sb->AppendHexBuff(buff, buffSize, ' ', Text::LineBreakType::None);
		}
	}
	else if (this->keyType == KeyType::ECDSA)
	{
		UnsafeArray<const UInt8> buff;
		UIntOS buffSize;
		ECName ecName = this->GetECName();
		if (found) sb->AppendLB(Text::LineBreakType::CRLF);
		found = true;
		sb->Append(this->sourceName);
		sb->AppendUTF8Char('.');
		sb->AppendC(UTF8STRC("EC.Name = "));
		sb->Append(ECNameGetName(ecName));

		if (this->GetECPrivate(buffSize).SetTo(buff))
		{
			if (found) sb->AppendLB(Text::LineBreakType::CRLF);
			found = true;
			sb->Append(this->sourceName);
			sb->AppendUTF8Char('.');
			sb->AppendC(UTF8STRC("EC.Private = "));
			sb->AppendHexBuff(buff, buffSize, ' ', Text::LineBreakType::None);
		}
		if (this->GetECPublic(buffSize).SetTo(buff))
		{
			if (found) sb->AppendLB(Text::LineBreakType::CRLF);
			found = true;
			sb->Append(this->sourceName);
			sb->AppendUTF8Char('.');
			sb->AppendC(UTF8STRC("EC.Public = "));
			sb->AppendHexBuff(buff, buffSize, ' ', Text::LineBreakType::None);
		}
	}

	UInt8 keyId[20];
	if (this->GetKeyId(BYTEARR(keyId)))
	{
		if (found) sb->AppendLB(Text::LineBreakType::CRLF);
		found = true;
		sb->Append(this->sourceName);
		sb->AppendUTF8Char('.');
		sb->AppendC(UTF8STRC("KeyId = "));
		sb->AppendHexBuff(keyId, 20, ' ', Text::LineBreakType::None);
	}
}

NN<Net::ASN1Names> Crypto::Cert::X509Key::CreateNames() const
{
	NN<Net::ASN1Names> names;
	NEW_CLASSNN(names, Net::ASN1Names());
	switch (this->keyType)
	{
	case KeyType::RSA:
		return names->SetRSAPrivateKey();
	case KeyType::RSAPublic:
		return names->SetRSAPublicKey();
	default:
	case KeyType::DSA:
	case KeyType::ECDSA:
	case KeyType::ECPublic:
	case KeyType::ED25519:
	case KeyType::Unknown:
		return names;
	}
}

Crypto::Cert::X509File::KeyType Crypto::Cert::X509Key::GetKeyType() const
{
	return this->keyType;
}

UIntOS Crypto::Cert::X509Key::GetKeySizeBits() const
{
	return KeyGetLeng(this->buff.Arr(), this->buff.ArrEnd(), this->keyType);
}

Bool Crypto::Cert::X509Key::IsPrivateKey() const
{
	switch (this->keyType)
	{
	case KeyType::DSA:
	case KeyType::ECDSA:
	case KeyType::ED25519:
	case KeyType::RSA:
		return true;
	case KeyType::RSAPublic:
	case KeyType::ECPublic:
	case KeyType::Unknown:
	default:
		return false;
	}
}

Optional<Crypto::Cert::X509Key> Crypto::Cert::X509Key::ExtractPublicKey() const
{
	if (this->keyType == KeyType::RSAPublic)
	{
		return NN<Crypto::Cert::X509Key>::ConvertFrom(this->Clone());
	}
	else if (this->keyType == KeyType::RSA)
	{
		UIntOS modulusSize;
		UnsafeArray<const UInt8> modulus;
		UIntOS publicExponentSize;
		UnsafeArray<const UInt8> publicExponent;
		if (!this->GetRSAModulus(modulusSize).SetTo(modulus)) return nullptr;
		if (!this->GetRSAPublicExponent(publicExponentSize).SetTo(publicExponent)) return nullptr;
		return CreateRSAPublicKey(this->GetSourceNameObj()->ToCString(), Data::ByteArrayR(modulus, modulusSize), Data::ByteArrayR(publicExponent, publicExponentSize));
	}
	else if (this->keyType == KeyType::ECPublic)
	{
		return NN<Crypto::Cert::X509Key>::ConvertFrom(this->Clone());
	}
	else
	{
		return nullptr;
	}
}

Bool Crypto::Cert::X509Key::GetKeyId(const Data::ByteArray &keyId) const
{
	if (keyId.GetSize() < 20)
	{
		return false;
	}
	NN<Crypto::Cert::X509Key> pubKey;
	if (this->ExtractPublicKey().SetTo(pubKey))
	{
		Crypto::Hash::SHA1 sha1;
		sha1.Calc(pubKey->GetASN1Buff(), pubKey->GetASN1BuffSize());
		sha1.GetValue(keyId.Arr());
		pubKey.Delete();
		return true;
	}
	return false;
}

UIntOS Crypto::Cert::X509Key::GetDataBlockSize() const
{
	return GetKeySizeBits() / 8;
}

UnsafeArrayOpt<const UInt8> Crypto::Cert::X509Key::GetRSAModulus(OptOut<UIntOS> size) const
{
	if (this->keyType == KeyType::RSA)
	{
		return Net::ASN1Util::PDUGetItem(this->buff.Arr(), this->buff.ArrEnd(), "1.2", size, 0);
	}
	else if (this->keyType == KeyType::RSAPublic)
	{
		return Net::ASN1Util::PDUGetItem(this->buff.Arr(), this->buff.ArrEnd(), "1.1", size, 0);
	}
	else
	{
		return nullptr;
	}
}

UnsafeArrayOpt<const UInt8> Crypto::Cert::X509Key::GetRSAPublicExponent(OptOut<UIntOS> size) const
{
	if (this->keyType == KeyType::RSA)
	{
		return Net::ASN1Util::PDUGetItem(this->buff.Arr(), this->buff.ArrEnd(), "1.3", size, 0);
	}
	else if (this->keyType == KeyType::RSAPublic)
	{
		return Net::ASN1Util::PDUGetItem(this->buff.Arr(), this->buff.ArrEnd(), "1.2", size, 0);
	}
	else
	{
		return nullptr;
	}
}

UnsafeArrayOpt<const UInt8> Crypto::Cert::X509Key::GetRSAPrivateExponent(OptOut<UIntOS> size) const
{
	if (this->keyType != KeyType::RSA) return nullptr;
	return Net::ASN1Util::PDUGetItem(this->buff.Arr(), this->buff.ArrEnd(), "1.4", size, 0);
}

UnsafeArrayOpt<const UInt8> Crypto::Cert::X509Key::GetRSAPrime1(OptOut<UIntOS> size) const
{
	if (this->keyType != KeyType::RSA) return nullptr;
	return Net::ASN1Util::PDUGetItem(this->buff.Arr(), this->buff.ArrEnd(), "1.5", size, 0);
}

UnsafeArrayOpt<const UInt8> Crypto::Cert::X509Key::GetRSAPrime2(OptOut<UIntOS> size) const
{
	if (this->keyType != KeyType::RSA) return nullptr;
	return Net::ASN1Util::PDUGetItem(this->buff.Arr(), this->buff.ArrEnd(), "1.6", size, 0);
}

UnsafeArrayOpt<const UInt8> Crypto::Cert::X509Key::GetRSAExponent1(OptOut<UIntOS> size) const
{
	// e1 = d % p
	// d = private Exponent
	// p = prime1
	if (this->keyType != KeyType::RSA) return nullptr;
	return Net::ASN1Util::PDUGetItem(this->buff.Arr(), this->buff.ArrEnd(), "1.7", size, 0);
}

UnsafeArrayOpt<const UInt8> Crypto::Cert::X509Key::GetRSAExponent2(OptOut<UIntOS> size) const
{
	// e2 = d % q
	// d = private Exponent
	// p = prime2
	if (this->keyType != KeyType::RSA) return nullptr;
	return Net::ASN1Util::PDUGetItem(this->buff.Arr(), this->buff.ArrEnd(), "1.8", size, 0);
}

UnsafeArrayOpt<const UInt8> Crypto::Cert::X509Key::GetRSACoefficient(OptOut<UIntOS> size) const
{
	if (this->keyType != KeyType::RSA) return nullptr;
	return Net::ASN1Util::PDUGetItem(this->buff.Arr(), this->buff.ArrEnd(), "1.9", size, 0);
}

UnsafeArrayOpt<const UInt8> Crypto::Cert::X509Key::GetECPrivate(OptOut<UIntOS> size) const
{
	if (this->keyType == KeyType::ECDSA)
	{
		return Net::ASN1Util::PDUGetItem(this->buff.Arr(), this->buff.ArrEnd(), "1.2", size, 0);
	}
	else
	{
		return nullptr;
	}
}
UnsafeArrayOpt<const UInt8> Crypto::Cert::X509Key::GetECPublic(OptOut<UIntOS> size) const
{
	Net::ASN1Util::ItemType itemType;
	UIntOS itemLen;
	UnsafeArray<const UInt8> itemPDU;
	if (this->keyType == KeyType::ECPublic)
	{
		if (Net::ASN1Util::PDUGetItem(this->buff.Arr(), this->buff.ArrEnd(), "1.2", itemLen, itemType).SetTo(itemPDU)  && itemType == Net::ASN1Util::IT_BIT_STRING)
		{
			size.Set(itemLen - 1);
			return itemPDU + 1;
		}
		return nullptr;
	}
	else if (this->keyType == KeyType::ECDSA)
	{
		if (Net::ASN1Util::PDUGetItem(this->buff.Arr(), this->buff.ArrEnd(), "1.3", itemLen, itemType).SetTo(itemPDU) && itemType == Net::ASN1Util::IT_CONTEXT_SPECIFIC_1)
		{
			if (Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, "1", itemLen, itemType).SetTo(itemPDU) && itemType == Net::ASN1Util::IT_BIT_STRING)
			{
				size.Set(itemLen - 1);
				return itemPDU + 1;
			}
			return nullptr;
		}
		if (Net::ASN1Util::PDUGetItem(this->buff.Arr(), this->buff.ArrEnd(), "1.4", itemLen, itemType).SetTo(itemPDU) && itemType == Net::ASN1Util::IT_CONTEXT_SPECIFIC_1)
		{
			if (Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, "1", itemLen, itemType).SetTo(itemPDU) && itemType == Net::ASN1Util::IT_BIT_STRING)
			{
				size.Set(itemLen - 1);
				return itemPDU + 1;
			}
			return nullptr;
		}
		return nullptr;
	}
	else
	{
		return nullptr;
	}
}

Crypto::Cert::X509File::ECName Crypto::Cert::X509Key::GetECName() const
{
	if (this->keyType == KeyType::ECPublic)
	{
		Net::ASN1Util::ItemType itemType;
		UIntOS size;
		UnsafeArray<const UInt8> itemPDU;
		if (Net::ASN1Util::PDUGetItem(this->buff.Arr(), this->buff.ArrEnd(), "1.1.2", size, itemType).SetTo(itemPDU) && itemType == Net::ASN1Util::IT_OID)
		{
			return ECNameFromOID(Data::ByteArrayR(itemPDU, size));
		}
	}
	else if (this->keyType == KeyType::ECDSA)
	{
		Net::ASN1Util::ItemType itemType;
		UIntOS size;
		UnsafeArray<const UInt8> itemPDU;
		if (Net::ASN1Util::PDUGetItem(this->buff.Arr(), this->buff.ArrEnd(), "1.3", size, itemType).SetTo(itemPDU) && itemType == Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)
		{
			if (Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + size, "1", size, itemType).SetTo(itemPDU) && itemType == Net::ASN1Util::IT_OID)
			{
				return ECNameFromOID(Data::ByteArrayR(itemPDU, size));
			}
		}
	}
	return ECName::Unknown;
}

NN<Crypto::Cert::X509Key> Crypto::Cert::X509Key::FromECPublicKey(Data::ByteArrayR buff, Data::ByteArrayR paramOID)
{
	Net::ASN1PDUBuilder pdu;
	pdu.BeginSequence();
	pdu.BeginSequence();
	pdu.AppendOIDString(CSTR("1.2.840.10045.2.1"));
	pdu.AppendOID(paramOID);
	pdu.EndLevel();
	pdu.AppendBitString(0, buff);
	pdu.EndLevel();
	NN<X509Key> key;
	NEW_CLASSNN(key, X509Key(CSTR("ECPublic.key"), pdu.GetArray(), KeyType::ECPublic));
	return key;
}

NN<Crypto::Cert::X509Key> Crypto::Cert::X509Key::CreateRSAPublicKey(Text::CStringNN name, Data::ByteArrayR modulus, Data::ByteArrayR publicExponent)
{
	Net::ASN1PDUBuilder builder;
	builder.BeginSequence();
	builder.AppendOther(Net::ASN1Util::IT_INTEGER, modulus.Arr(), modulus.GetSize());
	builder.AppendOther(Net::ASN1Util::IT_INTEGER, publicExponent.Arr(), publicExponent.GetSize());
	builder.EndLevel();
	NN<Crypto::Cert::X509Key> key;
	NEW_CLASSNN(key, Crypto::Cert::X509Key(name, builder.GetArray(), KeyType::RSAPublic));
	return key;
}

Optional<Crypto::Cert::X509Key> Crypto::Cert::X509Key::FromRSAKey(NN<RSAKey> key)
{
	UIntOS modulusSize;
	UnsafeArray<const UInt8> modulus;
	UIntOS publicExponentSize;
	UnsafeArray<const UInt8> publicExponent;
	if (!key->GetRSAModulus(modulusSize).SetTo(modulus) || !key->GetRSAPublicExponent(publicExponentSize).SetTo(publicExponent))
	{
		return nullptr;
	}
	UIntOS privateExponentSize;
	UnsafeArray<const UInt8> privateExponent;
	UIntOS prime1Size;
	UnsafeArray<const UInt8> prime1;
	UIntOS prime2Size;
	UnsafeArray<const UInt8> prime2;
	UIntOS exponent1Size;
	UnsafeArray<const UInt8> exponent1;
	UIntOS exponent2Size;
	UnsafeArray<const UInt8> exponent2;
	UIntOS coefficientSize;
	UnsafeArray<const UInt8> coefficient;

	Net::ASN1PDUBuilder pdu;
	if (!key->GetRSAPrivateExponent(privateExponentSize).SetTo(privateExponent) ||
		!key->GetRSAPrime1(prime1Size).SetTo(prime1) ||
		!key->GetRSAPrime2(prime2Size).SetTo(prime2) ||
		!key->GetRSAExponent1(exponent1Size).SetTo(exponent1) ||
		!key->GetRSAExponent2(exponent2Size).SetTo(exponent2) ||
		!key->GetRSACoefficient(coefficientSize).SetTo(coefficient))
	{
		pdu.BeginSequence();
		pdu.AppendInteger(modulus, modulusSize);
		pdu.AppendInteger(publicExponent, publicExponentSize);
		pdu.EndLevel();
		return NEW_CLASS_D(X509Key(CSTR("RSAPublic.key"), pdu.GetArray(), KeyType::RSAPublic));
	}
	else
	{
		pdu.BeginSequence();
		pdu.AppendInt32(0);
		pdu.AppendInteger(modulus, modulusSize);
		pdu.AppendInteger(publicExponent, publicExponentSize);
		pdu.AppendInteger(privateExponent, privateExponentSize);
		pdu.AppendInteger(prime1, prime1Size);
		pdu.AppendInteger(prime2, prime2Size);
		pdu.AppendInteger(exponent1, exponent1Size);
		pdu.AppendInteger(exponent2, exponent2Size);
		pdu.AppendInteger(coefficient, coefficientSize);
		pdu.EndLevel();
		return NEW_CLASS_D(X509Key(CSTR("RSAPrivate.key"), pdu.GetArray(), KeyType::RSA));
	}
}
