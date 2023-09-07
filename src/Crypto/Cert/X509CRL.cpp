#include "Stdafx.h"
#include "Crypto/Cert/X509CRL.h"
#include "Net/ASN1Util.h"
#include "Net/SSLEngine.h"

Crypto::Cert::X509CRL::X509CRL(NotNullPtr<Text::String> sourceName, Data::ByteArrayR buff) : Crypto::Cert::X509File(sourceName, buff)
{

}

Crypto::Cert::X509CRL::X509CRL(Text::CStringNN sourceName, Data::ByteArrayR buff) : Crypto::Cert::X509File(sourceName, buff)
{

}

Crypto::Cert::X509CRL::~X509CRL()
{

}

Crypto::Cert::X509File::FileType Crypto::Cert::X509CRL::GetFileType() const
{
	return FileType::CRL;
}

void Crypto::Cert::X509CRL::ToShortName(NotNullPtr<Text::StringBuilderUTF8> sb) const
{
	UOSInt len = 0;
	Net::ASN1Util::ItemType itemType = Net::ASN1Util::IT_UNKNOWN;
	const UInt8 *tmpBuff = Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.1.3", &len, &itemType);
	if (tmpBuff != 0 && itemType == Net::ASN1Util::IT_SEQUENCE)
	{
		NameGetCN(tmpBuff, tmpBuff + len, sb);
	}
	else
	{
		tmpBuff = Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.1.2", &len, &itemType);
		if (tmpBuff != 0 && itemType == Net::ASN1Util::IT_SEQUENCE)
		{
			NameGetCN(tmpBuff, tmpBuff + len, sb);
		}
	}
}

Crypto::Cert::X509File::ValidStatus Crypto::Cert::X509CRL::IsValid(Net::SSLEngine *ssl, Crypto::Cert::CertStore *trustStore) const
{
	if (trustStore == 0)
	{
		trustStore = ssl->GetTrustStore();
	}
	Text::StringBuilderUTF8 sb;
	if (!this->GetIssuerCN(sb))
	{
		return Crypto::Cert::X509File::ValidStatus::FileFormatInvalid;
	}
	Data::DateTime dt;
	Int64 currTime = Data::DateTimeUtil::GetCurrTimeMillis();
	if (!this->GetThisUpdate(dt))
	{
		return Crypto::Cert::X509File::ValidStatus::FileFormatInvalid;
	}
	if (dt.ToTicks() > currTime)
	{
		return Crypto::Cert::X509File::ValidStatus::Expired;
	}
	if (this->GetNextUpdate(dt))
	{
		if (dt.ToTicks() < currTime)
		{
			return Crypto::Cert::X509File::ValidStatus::Expired;
		}
	}
	SignedInfo signedInfo;
	if (!this->GetSignedInfo(signedInfo))
	{
		return Crypto::Cert::X509File::ValidStatus::FileFormatInvalid;
	}
	Crypto::Hash::HashType hashType = GetAlgHash(signedInfo.algType);
	if (hashType == Crypto::Hash::HashType::Unknown)
	{
		return Crypto::Cert::X509File::ValidStatus::UnsupportedAlgorithm;
	}

	Crypto::Cert::X509Cert *issuer = trustStore->GetCertByCN(sb.ToCString());
	if (issuer == 0)
	{
		return Crypto::Cert::X509File::ValidStatus::UnknownIssuer;
	}
	NotNullPtr<Crypto::Cert::X509Key> key;
	if (!key.Set(issuer->GetNewPublicKey()))
	{
		return Crypto::Cert::X509File::ValidStatus::FileFormatInvalid;
	}
	Bool signValid = ssl->SignatureVerify(key, hashType, signedInfo.payload, signedInfo.payloadSize, signedInfo.signature, signedInfo.signSize);
	key.Delete();
	if (!signValid)
	{
		return Crypto::Cert::X509File::ValidStatus::SignatureInvalid;
	}
	return Crypto::Cert::X509File::ValidStatus::Valid;
}

NotNullPtr<Net::ASN1Data> Crypto::Cert::X509CRL::Clone() const
{
	NotNullPtr<Crypto::Cert::X509CRL> asn1;
	NEW_CLASSNN(asn1, Crypto::Cert::X509CRL(this->GetSourceNameObj(), this->buff));
	return asn1;
}

void Crypto::Cert::X509CRL::ToString(NotNullPtr<Text::StringBuilderUTF8> sb) const
{
	if (IsCertificateList(this->buff.Ptr(), this->buff.PtrEnd(), "1"))
	{
		AppendCertificateList(this->buff.Ptr(), this->buff.PtrEnd(), "1", sb, CSTR_NULL);
	}
}

Bool Crypto::Cert::X509CRL::HasVersion() const
{
	UOSInt itemLen;
	Net::ASN1Util::ItemType itemType;
	if (Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.1.1", &itemLen, &itemType) != 0 && itemType == Net::ASN1Util::IT_INTEGER)
	{
		return true;
	}
	return false;
}

Bool Crypto::Cert::X509CRL::GetIssuerCN(NotNullPtr<Text::StringBuilderUTF8> sb) const
{
	UOSInt len = 0;
	Net::ASN1Util::ItemType itemType = Net::ASN1Util::IT_UNKNOWN;
	const UInt8 *tmpBuff;
	if (this->HasVersion())
	{
		tmpBuff = Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.1.3", &len, &itemType);
	}
	else
	{
		tmpBuff = Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.1.2", &len, &itemType);
	}
	if (tmpBuff != 0 && itemType == Net::ASN1Util::IT_SEQUENCE)
	{
		return NameGetCN(tmpBuff, tmpBuff + len, sb);
	}
	else
	{
		return false;
	}
}

Bool Crypto::Cert::X509CRL::GetThisUpdate(NotNullPtr<Data::DateTime> dt) const
{
	Net::ASN1Util::ItemType itemType;
	UOSInt itemLen;
	const UInt8 *itemPDU;
	if (this->HasVersion())
	{
		itemPDU = Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.1.4", &itemLen, &itemType);
	}
	else
	{
		itemPDU = Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.1.3", &itemLen, &itemType);
	}
	if (itemPDU != 0 && (itemType == Net::ASN1Util::IT_UTCTIME || itemType == Net::ASN1Util::IT_GENERALIZEDTIME))
	{
		return Net::ASN1Util::PDUParseUTCTimeCont(itemPDU, itemLen, dt);
	}
	else
	{
		return false;
	}
}

Bool Crypto::Cert::X509CRL::GetNextUpdate(NotNullPtr<Data::DateTime> dt) const
{
	Net::ASN1Util::ItemType itemType;
	UOSInt itemLen;
	const UInt8 *itemPDU;
	if (this->HasVersion())
	{
		itemPDU = Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.1.5", &itemLen, &itemType);
	}
	else
	{
		itemPDU = Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.1.4", &itemLen, &itemType);
	}
	if (itemPDU != 0 && (itemType == Net::ASN1Util::IT_UTCTIME || itemType == Net::ASN1Util::IT_GENERALIZEDTIME))
	{
		return Net::ASN1Util::PDUParseUTCTimeCont(itemPDU, itemLen, dt);
	}
	else
	{
		return false;
	}
}

Bool Crypto::Cert::X509CRL::IsRevoked(NotNullPtr<Crypto::Cert::X509Cert> cert) const
{
	UOSInt snLen;
	const UInt8 *sn = cert->GetSerialNumber(snLen);
	if (sn == 0)
		return true;
	UTF8Char sbuff[32];
	Net::ASN1Util::ItemType itemType;
	UOSInt itemLen;
	const UInt8 *itemPDU;
	UOSInt i;
	if (this->HasVersion())
	{
		itemPDU = Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.1.5", &itemLen, &itemType);
		i = 5;
	}
	else
	{
		itemPDU = Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.1.4", &itemLen, &itemType);
		i = 4;
	}
	if (itemPDU == 0)
	{
		return false;
	}
	if (itemType != Net::ASN1Util::IT_SEQUENCE)
	{
		i++;
		Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("1.1.")), i);
		itemPDU = Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), (const Char*)sbuff, &itemLen, &itemType);
		if (itemPDU == 0 || itemType != Net::ASN1Util::IT_SEQUENCE)
		{
			return false;
		}
	}
	i = 0;
	while (true)
	{
		const UInt8 *subitemPDU;
		UOSInt subitemLen;
		Text::StrConcatC(Text::StrUOSInt(sbuff, ++i), UTF8STRC(".1"));
		subitemPDU = Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, (const Char*)sbuff, &subitemLen, &itemType);
		if (subitemPDU == 0)
			break;
		if (itemType == Net::ASN1Util::IT_INTEGER && Text::StrEqualsC(subitemPDU, subitemLen, sn, snLen))
		{
			return true;
		}
	}
	return false;
}
