#include "Stdafx.h"
#include "Crypto/Cert/X509Key.h"
#include "Crypto/Hash/SHA1.h"
#include "Net/ASN1PDUBuilder.h"
#include "Net/ASN1Util.h"

Crypto::Cert::X509Key::X509Key(NotNullPtr<Text::String> sourceName, Data::ByteArrayR buff, KeyType keyType) : Crypto::Cert::X509File(sourceName, buff)
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

void Crypto::Cert::X509Key::ToShortName(NotNullPtr<Text::StringBuilderUTF8> sb) const
{
	sb->Append(KeyTypeGetName(this->keyType));
	sb->AppendUTF8Char(' ');
	sb->AppendUOSInt(this->GetKeySizeBits());
	sb->AppendC(UTF8STRC(" bits"));
}

Crypto::Cert::X509File::ValidStatus Crypto::Cert::X509Key::IsValid(NotNullPtr<Net::SSLEngine> ssl, Crypto::Cert::CertStore *trustStore) const
{
	if (this->keyType == KeyType::Unknown)
	{
		return Crypto::Cert::X509File::ValidStatus::FileFormatInvalid;
	}
	return Crypto::Cert::X509File::ValidStatus::Valid;
}

NotNullPtr<Net::ASN1Data> Crypto::Cert::X509Key::Clone() const
{
	NotNullPtr<Crypto::Cert::X509Key> asn1;
	NEW_CLASSNN(asn1, Crypto::Cert::X509Key(this->GetSourceNameObj(), this->buff, this->keyType));
	return asn1;
}

void Crypto::Cert::X509Key::ToString(NotNullPtr<Text::StringBuilderUTF8> sb) const
{
	Bool found = false;
	if (this->keyType == KeyType::RSA)
	{
		const UInt8 *buff;
		UOSInt buffSize;
		buff = this->GetRSAModulus(buffSize);
		if (buff)
		{
			if (found) sb->AppendLB(Text::LineBreakType::CRLF);
			found = true;
			sb->Append(this->sourceName);
			sb->AppendUTF8Char('.');
			sb->AppendC(UTF8STRC("RSA.Modulus = "));
			sb->AppendHexBuff(buff, buffSize, ' ', Text::LineBreakType::None);
		}
		buff = this->GetRSAPublicExponent(buffSize);
		if (buff)
		{
			if (found) sb->AppendLB(Text::LineBreakType::CRLF);
			found = true;
			sb->Append(this->sourceName);
			sb->AppendUTF8Char('.');
			sb->AppendC(UTF8STRC("RSA.Public Exponent = "));
			sb->AppendHexBuff(buff, buffSize, ' ', Text::LineBreakType::None);
		}
		buff = this->GetRSAPrivateExponent(buffSize);
		if (buff)
		{
			if (found) sb->AppendLB(Text::LineBreakType::CRLF);
			found = true;
			sb->Append(this->sourceName);
			sb->AppendUTF8Char('.');
			sb->AppendC(UTF8STRC("RSA.Private Exponent = "));
			sb->AppendHexBuff(buff, buffSize, ' ', Text::LineBreakType::None);
		}
		buff = this->GetRSAPrime1(buffSize);
		if (buff)
		{
			if (found) sb->AppendLB(Text::LineBreakType::CRLF);
			found = true;
			sb->Append(this->sourceName);
			sb->AppendUTF8Char('.');
			sb->AppendC(UTF8STRC("RSA.Prime1 = "));
			sb->AppendHexBuff(buff, buffSize, ' ', Text::LineBreakType::None);
		}
		buff = this->GetRSAPrime2(buffSize);
		if (buff)
		{
			if (found) sb->AppendLB(Text::LineBreakType::CRLF);
			found = true;
			sb->Append(this->sourceName);
			sb->AppendUTF8Char('.');
			sb->AppendC(UTF8STRC("RSA.Prime2 = "));
			sb->AppendHexBuff(buff, buffSize, ' ', Text::LineBreakType::None);
		}
		buff = this->GetRSAExponent1(buffSize);
		if (buff)
		{
			if (found) sb->AppendLB(Text::LineBreakType::CRLF);
			found = true;
			sb->Append(this->sourceName);
			sb->AppendUTF8Char('.');
			sb->AppendC(UTF8STRC("RSA.Exponent1 = "));
			sb->AppendHexBuff(buff, buffSize, ' ', Text::LineBreakType::None);
		}
		buff = this->GetRSAExponent2(buffSize);
		if (buff)
		{
			if (found) sb->AppendLB(Text::LineBreakType::CRLF);
			found = true;
			sb->Append(this->sourceName);
			sb->AppendUTF8Char('.');
			sb->AppendC(UTF8STRC("RSA.Exponent2 = "));
			sb->AppendHexBuff(buff, buffSize, ' ', Text::LineBreakType::None);
		}
		buff = this->GetRSACoefficient(buffSize);
		if (buff)
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
		const UInt8 *buff;
		UOSInt buffSize;
		buff = this->GetRSAModulus(buffSize);
		if (buff)
		{
			if (found) sb->AppendLB(Text::LineBreakType::CRLF);
			found = true;
			sb->Append(this->sourceName);
			sb->AppendUTF8Char('.');
			sb->AppendC(UTF8STRC("RSA.Modulus = "));
			sb->AppendHexBuff(buff, buffSize, ' ', Text::LineBreakType::None);
		}
		buff = this->GetRSAPublicExponent(buffSize);
		if (buff)
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
		const UInt8 *buff;
		UOSInt buffSize;
		ECName ecName = this->GetECName();
		if (found) sb->AppendLB(Text::LineBreakType::CRLF);
		found = true;
		sb->Append(this->sourceName);
		sb->AppendUTF8Char('.');
		sb->AppendC(UTF8STRC("EC.Name = "));
		sb->Append(ECNameGetName(ecName));

		buff = this->GetECPublic(buffSize);
		if (buff)
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
		const UInt8 *buff;
		UOSInt buffSize;
		ECName ecName = this->GetECName();
		if (found) sb->AppendLB(Text::LineBreakType::CRLF);
		found = true;
		sb->Append(this->sourceName);
		sb->AppendUTF8Char('.');
		sb->AppendC(UTF8STRC("EC.Name = "));
		sb->Append(ECNameGetName(ecName));

		buff = this->GetECPrivate(buffSize);
		if (buff)
		{
			if (found) sb->AppendLB(Text::LineBreakType::CRLF);
			found = true;
			sb->Append(this->sourceName);
			sb->AppendUTF8Char('.');
			sb->AppendC(UTF8STRC("EC.Private = "));
			sb->AppendHexBuff(buff, buffSize, ' ', Text::LineBreakType::None);
		}
		buff = this->GetECPublic(buffSize);
		if (buff)
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

Net::ASN1Names *Crypto::Cert::X509Key::CreateNames() const
{
	Net::ASN1Names *names;
	NEW_CLASS(names, Net::ASN1Names());
	switch (this->keyType)
	{
	case KeyType::RSA:
		return names->SetRSAPrivateKey().Ptr();
	case KeyType::RSAPublic:
		return names->SetRSAPublicKey().Ptr();
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

UOSInt Crypto::Cert::X509Key::GetKeySizeBits() const
{
	return KeyGetLeng(this->buff.Ptr(), this->buff.PtrEnd(), this->keyType);
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

Crypto::Cert::X509Key *Crypto::Cert::X509Key::CreatePublicKey() const
{
	if (this->keyType == KeyType::RSAPublic)
	{
		return (Crypto::Cert::X509Key*)this->Clone().Ptr();
	}
	else if (this->keyType == KeyType::RSA)
	{
		Net::ASN1PDUBuilder builder;
		UOSInt buffSize;
		const UInt8 *buff;
		builder.BeginSequence();
		if ((buff = this->GetRSAModulus(buffSize)) == 0) return 0;
		builder.AppendOther(Net::ASN1Util::IT_INTEGER, buff, buffSize);
		if ((buff = this->GetRSAPublicExponent(buffSize)) == 0) return 0;
		builder.AppendOther(Net::ASN1Util::IT_INTEGER, buff, buffSize);
		builder.EndLevel();
		Crypto::Cert::X509Key *key;
		NEW_CLASS(key, Crypto::Cert::X509Key(this->GetSourceNameObj(), builder.GetArray(), KeyType::RSAPublic));
		return key;
	}
	else if (this->keyType == KeyType::ECPublic)
	{
		return (Crypto::Cert::X509Key*)this->Clone().Ptr();
	}
	else
	{
		return 0;
	}
}

Bool Crypto::Cert::X509Key::GetKeyId(const Data::ByteArray &keyId) const
{
	if (keyId.GetSize() < 20)
	{
		return false;
	}
	Crypto::Cert::X509Key *pubKey = this->CreatePublicKey();
	if (pubKey)
	{
		Crypto::Hash::SHA1 sha1;
		sha1.Calc(pubKey->GetASN1Buff(), pubKey->GetASN1BuffSize());
		sha1.GetValue(keyId.Ptr());
		DEL_CLASS(pubKey);
		return true;
	}
	return false;
}

const UInt8 *Crypto::Cert::X509Key::GetRSAModulus(OptOut<UOSInt> size) const
{
	if (this->keyType == KeyType::RSA)
	{
		return Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.2", size, 0);
	}
	else if (this->keyType == KeyType::RSAPublic)
	{
		return Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.1", size, 0);
	}
	else
	{
		return 0;
	}
}

const UInt8 *Crypto::Cert::X509Key::GetRSAPublicExponent(OptOut<UOSInt> size) const
{
	if (this->keyType == KeyType::RSA)
	{
		return Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.3", size, 0);
	}
	else if (this->keyType == KeyType::RSAPublic)
	{
		return Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.2", size, 0);
	}
	else
	{
		return 0;
	}
}

const UInt8 *Crypto::Cert::X509Key::GetRSAPrivateExponent(OptOut<UOSInt> size) const
{
	if (this->keyType != KeyType::RSA) return 0;
	return Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.4", size, 0);
}

const UInt8 *Crypto::Cert::X509Key::GetRSAPrime1(OptOut<UOSInt> size) const
{
	if (this->keyType != KeyType::RSA) return 0;
	return Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.5", size, 0);
}

const UInt8 *Crypto::Cert::X509Key::GetRSAPrime2(OptOut<UOSInt> size) const
{
	if (this->keyType != KeyType::RSA) return 0;
	return Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.6", size, 0);
}

const UInt8 *Crypto::Cert::X509Key::GetRSAExponent1(OptOut<UOSInt> size) const
{
	if (this->keyType != KeyType::RSA) return 0;
	return Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.7", size, 0);
}

const UInt8 *Crypto::Cert::X509Key::GetRSAExponent2(OptOut<UOSInt> size) const
{
	if (this->keyType != KeyType::RSA) return 0;
	return Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.8", size, 0);
}

const UInt8 *Crypto::Cert::X509Key::GetRSACoefficient(OptOut<UOSInt> size) const
{
	if (this->keyType != KeyType::RSA) return 0;
	return Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.9", size, 0);
}

const UInt8 *Crypto::Cert::X509Key::GetECPrivate(OptOut<UOSInt> size) const
{
	if (this->keyType == KeyType::ECDSA)
	{
		return Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.2", size, 0);
	}
	else
	{
		return 0;
	}
}
const UInt8 *Crypto::Cert::X509Key::GetECPublic(OptOut<UOSInt> size) const
{
	Net::ASN1Util::ItemType itemType;
	UOSInt itemLen;
	const UInt8 *itemPDU;
	if (this->keyType == KeyType::ECPublic)
	{
		itemPDU = Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.2", itemLen, itemType);
		if (itemPDU != 0 && itemType == Net::ASN1Util::IT_BIT_STRING)
		{
			size.Set(itemLen - 1);
			return itemPDU + 1;
		}
		return 0;
	}
	else if (this->keyType == KeyType::ECDSA)
	{
		itemPDU = Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.3", itemLen, itemType);
		if (itemPDU != 0 && itemType == Net::ASN1Util::IT_CONTEXT_SPECIFIC_1)
		{
			itemPDU = Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, "1", itemLen, itemType);
			if (itemPDU != 0 && itemType == Net::ASN1Util::IT_BIT_STRING)
			{
				size.Set(itemLen - 1);
				return itemPDU + 1;
			}
			return 0;
		}
		itemPDU = Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.4", itemLen, itemType);
		if (itemPDU != 0 && itemType == Net::ASN1Util::IT_CONTEXT_SPECIFIC_1)
		{
			itemPDU = Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, "1", itemLen, itemType);
			if (itemPDU != 0 && itemType == Net::ASN1Util::IT_BIT_STRING)
			{
				size.Set(itemLen - 1);
				return itemPDU + 1;
			}
			return 0;
		}
		return 0;
	}
	else
	{
		return 0;
	}
}

Crypto::Cert::X509File::ECName Crypto::Cert::X509Key::GetECName() const
{
	if (this->keyType == KeyType::ECPublic)
	{
		Net::ASN1Util::ItemType itemType;
		UOSInt size;
		const UInt8 *itemPDU = Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.1.2", size, itemType);
		if (itemPDU != 0 && itemType == Net::ASN1Util::IT_OID)
		{
			return ECNameFromOID(itemPDU, size);
		}
	}
	else if (this->keyType == KeyType::ECDSA)
	{
		Net::ASN1Util::ItemType itemType;
		UOSInt size;
		const UInt8 *itemPDU = Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.3", size, itemType);
		if (itemPDU != 0 && itemType == Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)
		{
			itemPDU = Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + size, "1", size, itemType);
			if (itemPDU != 0 && itemType == Net::ASN1Util::IT_OID)
			{
				return ECNameFromOID(itemPDU, size);
			}
		}
	}
	return ECName::Unknown;
}

Crypto::Cert::X509Key *Crypto::Cert::X509Key::FromECPublicKey(const UInt8 *buff, UOSInt buffSize, const UInt8 *paramOID, UOSInt oidLen)
{
	Net::ASN1PDUBuilder pdu;
	pdu.BeginSequence();
	pdu.BeginSequence();
	pdu.AppendOIDString(UTF8STRC("1.2.840.10045.2.1"));
	pdu.AppendOID(paramOID, oidLen);
	pdu.EndLevel();
	pdu.AppendBitString(0, buff, buffSize);
	pdu.EndLevel();
	return NEW_CLASS_D(X509Key(CSTR("ECPublic.key"), pdu.GetArray(), KeyType::ECPublic));
}
