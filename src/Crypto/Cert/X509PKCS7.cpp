#include "Stdafx.h"
#include "Crypto/Cert/X509Cert.h"
#include "Crypto/Cert/X509PKCS7.h"
#include "Net/ASN1Util.h"

Crypto::Cert::X509PKCS7::X509PKCS7(NN<Text::String> sourceName, Data::ByteArrayR buff) : Crypto::Cert::X509File(sourceName, buff)
{

}

Crypto::Cert::X509PKCS7::X509PKCS7(Text::CStringNN sourceName, Data::ByteArrayR buff) : Crypto::Cert::X509File(sourceName, buff)
{

}

Crypto::Cert::X509PKCS7::~X509PKCS7()
{

}

Crypto::Cert::X509File::FileType Crypto::Cert::X509PKCS7::GetFileType() const
{
	return FileType::PKCS7;
}

void Crypto::Cert::X509PKCS7::ToShortName(NN<Text::StringBuilderUTF8> sb) const
{
/*	UIntOS len = 0;
	Net::ASN1Util::ItemType itemType = Net::ASN1Util::IT_UNKNOWN;
	const UInt8 *tmpBuff = Net::ASN1Util::PDUGetItem(this->buff, this->buff.ArrEnd(), "1.1.2", &len, &itemType);
	if (tmpBuff != 0 && itemType == Net::ASN1Util::IT_SEQUENCE)
	{
		NameGetCN(tmpBuff, tmpBuff + len, sb);
	}*/
}

UIntOS Crypto::Cert::X509PKCS7::GetCertCount()
{
	Net::ASN1Util::ItemType itemType;
	UIntOS len;
	UnsafeArray<const UInt8> certListPDU;
	if (!Net::ASN1Util::PDUGetItem(this->buff.Arr(), this->buff.ArrEnd(), "1.2.1.4", len, itemType).SetTo(certListPDU) || itemType != Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)
	{
		return 0;
	}
	return Net::ASN1Util::PDUCountItem(certListPDU, certListPDU + len, nullptr);
}

Bool Crypto::Cert::X509PKCS7::GetCertName(UIntOS index, NN<Text::StringBuilderUTF8> sb)
{
	Net::ASN1Util::ItemType itemType;
	UIntOS len;
	UnsafeArray<const UInt8> certListPDU;
	if (!Net::ASN1Util::PDUGetItem(this->buff.Arr(), this->buff.ArrEnd(), "1.2.1.4", len, itemType).SetTo(certListPDU) || itemType != Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)
	{
		return false;
	}

	Char sbuff[32];
	Text::StrConcat(Text::StrUIntOS(sbuff, index + 1), ".1.1");
	UIntOS itemLen = 0;
	itemType = Net::ASN1Util::IT_UNKNOWN;
	UnsafeArrayOpt<const UInt8> tmpBuff;
	UnsafeArray<const UInt8> nntmpBuff;
	if (Net::ASN1Util::PDUGetItemType(certListPDU, certListPDU + len, sbuff) == Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)
	{
		Text::StrConcat(Text::StrUIntOS(sbuff, index + 1), ".1.6");
		tmpBuff = Net::ASN1Util::PDUGetItem(certListPDU, certListPDU + len, sbuff, itemLen, itemType);
	}
	else
	{
		Text::StrConcat(Text::StrUIntOS(sbuff, index + 1), ".1.5");
		tmpBuff = Net::ASN1Util::PDUGetItem(certListPDU, certListPDU + len, sbuff, itemLen, itemType);
	}
	if (tmpBuff.SetTo(nntmpBuff) && itemType == Net::ASN1Util::IT_SEQUENCE)
	{
		NameGetCN(nntmpBuff, nntmpBuff + itemLen, sb);
		return true;
	}
	return false;
}

Optional<Crypto::Cert::X509Cert> Crypto::Cert::X509PKCS7::GetNewCert(UIntOS index)
{
	Net::ASN1Util::ItemType itemType;
	UIntOS len;
	UnsafeArray<const UInt8> certListPDU;
	if (!Net::ASN1Util::PDUGetItem(this->buff.Arr(), this->buff.ArrEnd(), "1.2.1.4", len, itemType).SetTo(certListPDU) || itemType != Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)
	{
		return nullptr;
	}
	UIntOS ofst;
	Char sbuff[32];
	Text::StrUIntOS(sbuff, index + 1);
	UnsafeArray<const UInt8> certPDU;
	if (Net::ASN1Util::PDUGetItemRAW(certListPDU, certListPDU + len, sbuff, len, ofst).SetTo(certPDU))
	{
		NN<Crypto::Cert::X509Cert> cert;
		NEW_CLASSNN(cert, Crypto::Cert::X509Cert(this->GetSourceNameObj(), Data::ByteArrayR(certPDU, len + ofst)));
		return cert;
	}
	return nullptr;
}

Crypto::Cert::X509File::ValidStatus Crypto::Cert::X509PKCS7::IsValid(NN<Net::SSLEngine> ssl, Optional<Crypto::Cert::CertStore> trustStore) const
{
	return Crypto::Cert::X509File::ValidStatus::SignatureInvalid;
}

NN<Net::ASN1Data> Crypto::Cert::X509PKCS7::Clone() const
{
	NN<Crypto::Cert::X509PKCS7> asn1;
	NEW_CLASSNN(asn1, Crypto::Cert::X509PKCS7(this->GetSourceNameObj(), this->buff));
	return asn1;
}

void Crypto::Cert::X509PKCS7::ToString(NN<Text::StringBuilderUTF8> sb) const
{
	if (IsContentInfo(this->buff.Arr(), this->buff.ArrEnd(), "1"))
	{
		AppendContentInfo(this->buff.Arr(), this->buff.ArrEnd(), "1", sb, nullptr, Crypto::Cert::X509File::ContentDataType::Unknown);
	}
}

NN<Net::ASN1Names> Crypto::Cert::X509PKCS7::CreateNames() const
{
	NN<Net::ASN1Names> names;
	NEW_CLASSNN(names, Net::ASN1Names());
	return names->SetPKCS7ContentInfo();
}

Bool Crypto::Cert::X509PKCS7::IsSignData() const
{
	UIntOS itemLen;
	Net::ASN1Util::ItemType itemType;
	UnsafeArray<const UInt8> itemPDU;
	if (!Net::ASN1Util::PDUGetItem(this->buff.Arr(), this->buff.ArrEnd(), "1.1", itemLen, itemType).SetTo(itemPDU) || itemType != Net::ASN1Util::IT_OID)
	{
		return false;
	}
	return Net::ASN1Util::OIDEqualsText(Data::ByteArrayR(itemPDU, itemLen), CSTR("1.2.840.113549.1.7.2"));
}

Crypto::Hash::HashType Crypto::Cert::X509PKCS7::GetDigestType() const
{
	if (!this->IsSignData())
	{
		return Crypto::Hash::HashType::Unknown;
	}
	UIntOS itemLen;
	Net::ASN1Util::ItemType itemType;
	UnsafeArray<const UInt8> itemPDU;
	if (Net::ASN1Util::PDUGetItem(this->buff.Arr(), this->buff.ArrEnd(), "1.2.1.5.1.3.1", itemLen, itemType).SetTo(itemPDU) && itemType == Net::ASN1Util::IT_OID)
	{
		return HashTypeFromOID(Data::ByteArrayR(itemPDU, itemLen));
	}
	return Crypto::Hash::HashType::Unknown;
}

UnsafeArrayOpt<const UInt8> Crypto::Cert::X509PKCS7::GetMessageDigest(OutParam<UIntOS> digestSize) const
{
	Net::ASN1Util::ItemType itemType;
	UIntOS itemLen;
	UnsafeArray<const UInt8> itemPDU;
	UIntOS subitemLen;
	UnsafeArray<const UInt8> subitemPDU;
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	if (Net::ASN1Util::PDUGetItem(this->buff.Arr(), this->buff.ArrEnd(), "1.2.1.5.1.4", itemLen, itemType).SetTo(itemPDU) && itemType == Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)
	{
		UIntOS i = 0;
		while (true)
		{
			sptr = Text::StrUIntOS(sbuff, ++i);
			Text::StrConcatC(sptr, UTF8STRC(".1"));
			if (!Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, (const Char*)sbuff, subitemLen, itemType).SetTo(subitemPDU))
			{
				break;
			}
			else if (itemType == Net::ASN1Util::IT_OID && Net::ASN1Util::OIDEqualsText(Data::ByteArrayR(subitemPDU, subitemLen), CSTR("1.2.840.113549.1.9.4")))
			{
				Text::StrConcatC(sptr, UTF8STRC(".2.1"));
				if (Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, (const Char*)sbuff, subitemLen, itemType).SetTo(subitemPDU) &&
					itemType == Net::ASN1Util::IT_OCTET_STRING)
				{
					digestSize.Set(subitemLen);
					return subitemPDU;
				}
			}
		}
	}
	return nullptr;
}

UnsafeArrayOpt<const UInt8> Crypto::Cert::X509PKCS7::GetEncryptedDigest(OutParam<UIntOS> signSize) const
{
	Net::ASN1Util::ItemType itemType;
	UIntOS itemLen;
	UnsafeArray<const UInt8> itemPDU;
	if (Net::ASN1Util::PDUGetItem(this->buff.Arr(), this->buff.ArrEnd(), "1.2.1.5.1.6", itemLen, itemType).SetTo(itemPDU) && itemType == Net::ASN1Util::IT_OCTET_STRING)
	{
		signSize.Set(itemLen);
		return itemPDU;
	}
	return nullptr;
}
