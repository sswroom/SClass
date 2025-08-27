#include "Stdafx.h"
#include "Crypto/Cert/X509CRL.h"
#include "Net/ASN1Util.h"
#include "Net/SSLEngine.h"

Crypto::Cert::X509CRL::X509CRL(NN<Text::String> sourceName, Data::ByteArrayR buff) : Crypto::Cert::X509File(sourceName, buff)
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

void Crypto::Cert::X509CRL::ToShortName(NN<Text::StringBuilderUTF8> sb) const
{
	UOSInt len = 0;
	Net::ASN1Util::ItemType itemType = Net::ASN1Util::IT_UNKNOWN;
	UnsafeArray<const UInt8> tmpBuff;
	if (Net::ASN1Util::PDUGetItem(this->buff.Arr(), this->buff.ArrEnd(), "1.1.3", len, itemType).SetTo(tmpBuff) && itemType == Net::ASN1Util::IT_SEQUENCE)
	{
		NameGetCN(tmpBuff, tmpBuff + len, sb);
	}
	else
	{
		if (Net::ASN1Util::PDUGetItem(this->buff.Arr(), this->buff.ArrEnd(), "1.1.2", len, itemType).SetTo(tmpBuff) && itemType == Net::ASN1Util::IT_SEQUENCE)
		{
			NameGetCN(tmpBuff, tmpBuff + len, sb);
		}
	}
}

Crypto::Cert::X509File::ValidStatus Crypto::Cert::X509CRL::IsValid(NN<Net::SSLEngine> ssl, Optional<Crypto::Cert::CertStore> trustStore) const
{
	NN<Crypto::Cert::CertStore> trusts;
	if (!trustStore.SetTo(trusts))
	{
		trusts = ssl->GetTrustStore();
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

	NN<Crypto::Cert::X509Cert> issuer;
	if (!trusts->GetCertByCN(sb.ToCString()).SetTo(issuer))
	{
		return Crypto::Cert::X509File::ValidStatus::UnknownIssuer;
	}
	NN<Crypto::Cert::X509Key> key;
	if (!issuer->GetNewPublicKey().SetTo(key))
	{
		return Crypto::Cert::X509File::ValidStatus::FileFormatInvalid;
	}
	Bool signValid = ssl->SignatureVerify(key, hashType, Data::ByteArrayR(signedInfo.payload, signedInfo.payloadSize), Data::ByteArrayR(signedInfo.signature, signedInfo.signSize));
	key.Delete();
	if (!signValid)
	{
		return Crypto::Cert::X509File::ValidStatus::SignatureInvalid;
	}
	return Crypto::Cert::X509File::ValidStatus::Valid;
}

NN<Net::ASN1Data> Crypto::Cert::X509CRL::Clone() const
{
	NN<Crypto::Cert::X509CRL> asn1;
	NEW_CLASSNN(asn1, Crypto::Cert::X509CRL(this->GetSourceNameObj(), this->buff));
	return asn1;
}

void Crypto::Cert::X509CRL::ToString(NN<Text::StringBuilderUTF8> sb) const
{
	if (IsCertificateList(this->buff.Arr(), this->buff.ArrEnd(), "1"))
	{
		AppendCertificateList(this->buff.Arr(), this->buff.ArrEnd(), "1", sb, nullptr);
	}
}

NN<Net::ASN1Names> Crypto::Cert::X509CRL::CreateNames() const
{
	NN<Net::ASN1Names> names;
	NEW_CLASSNN(names, Net::ASN1Names());
	return names;
//	return names->SetCertificateList();*/
}

Bool Crypto::Cert::X509CRL::HasVersion() const
{
	UOSInt itemLen;
	Net::ASN1Util::ItemType itemType;
	if (Net::ASN1Util::PDUGetItem(this->buff.Arr(), this->buff.ArrEnd(), "1.1.1", itemLen, itemType).NotNull() && itemType == Net::ASN1Util::IT_INTEGER)
	{
		return true;
	}
	return false;
}

Bool Crypto::Cert::X509CRL::GetIssuerCN(NN<Text::StringBuilderUTF8> sb) const
{
	UOSInt len = 0;
	Net::ASN1Util::ItemType itemType = Net::ASN1Util::IT_UNKNOWN;
	UnsafeArrayOpt<const UInt8> tmpBuff;
	UnsafeArray<const UInt8> nntmpBuff;
	if (this->HasVersion())
	{
		tmpBuff = Net::ASN1Util::PDUGetItem(this->buff.Arr(), this->buff.ArrEnd(), "1.1.3", len, itemType);
	}
	else
	{
		tmpBuff = Net::ASN1Util::PDUGetItem(this->buff.Arr(), this->buff.ArrEnd(), "1.1.2", len, itemType);
	}
	if (tmpBuff.SetTo(nntmpBuff) && itemType == Net::ASN1Util::IT_SEQUENCE)
	{
		return NameGetCN(nntmpBuff, nntmpBuff + len, sb);
	}
	else
	{
		return false;
	}
}

Bool Crypto::Cert::X509CRL::GetThisUpdate(NN<Data::DateTime> dt) const
{
	Net::ASN1Util::ItemType itemType;
	UOSInt itemLen;
	UnsafeArrayOpt<const UInt8> itemPDU;
	UnsafeArray<const UInt8> nnitemPDU;
	if (this->HasVersion())
	{
		itemPDU = Net::ASN1Util::PDUGetItem(this->buff.Arr(), this->buff.ArrEnd(), "1.1.4", itemLen, itemType);
	}
	else
	{
		itemPDU = Net::ASN1Util::PDUGetItem(this->buff.Arr(), this->buff.ArrEnd(), "1.1.3", itemLen, itemType);
	}
	if (itemPDU.SetTo(nnitemPDU) && (itemType == Net::ASN1Util::IT_UTCTIME || itemType == Net::ASN1Util::IT_GENERALIZEDTIME))
	{
		return Net::ASN1Util::PDUParseUTCTimeCont(Data::ByteArrayR(nnitemPDU, itemLen), dt);
	}
	else
	{
		return false;
	}
}

Bool Crypto::Cert::X509CRL::GetNextUpdate(NN<Data::DateTime> dt) const
{
	Net::ASN1Util::ItemType itemType;
	UOSInt itemLen;
	UnsafeArrayOpt<const UInt8> itemPDU;
	UnsafeArray<const UInt8> nnitemPDU;
	if (this->HasVersion())
	{
		itemPDU = Net::ASN1Util::PDUGetItem(this->buff.Arr(), this->buff.ArrEnd(), "1.1.5", itemLen, itemType);
	}
	else
	{
		itemPDU = Net::ASN1Util::PDUGetItem(this->buff.Arr(), this->buff.ArrEnd(), "1.1.4", itemLen, itemType);
	}
	if (itemPDU.SetTo(nnitemPDU) && (itemType == Net::ASN1Util::IT_UTCTIME || itemType == Net::ASN1Util::IT_GENERALIZEDTIME))
	{
		return Net::ASN1Util::PDUParseUTCTimeCont(Data::ByteArrayR(nnitemPDU, itemLen), dt);
	}
	else
	{
		return false;
	}
}

Bool Crypto::Cert::X509CRL::IsRevoked(NN<Crypto::Cert::X509Cert> cert) const
{
	UOSInt snLen;
	UnsafeArray<const UInt8> sn;
	if (!cert->GetSerialNumber(snLen).SetTo(sn))
		return true;
	UTF8Char sbuff[32];
	Net::ASN1Util::ItemType itemType;
	UOSInt itemLen;
	UnsafeArrayOpt<const UInt8> itemPDU;
	UnsafeArray<const UInt8> nnitemPDU;
	UOSInt i;
	if (this->HasVersion())
	{
		itemPDU = Net::ASN1Util::PDUGetItem(this->buff.Arr(), this->buff.ArrEnd(), "1.1.5", itemLen, itemType);
		i = 5;
	}
	else
	{
		itemPDU = Net::ASN1Util::PDUGetItem(this->buff.Arr(), this->buff.ArrEnd(), "1.1.4", itemLen, itemType);
		i = 4;
	}
	if (!itemPDU.SetTo(nnitemPDU))
	{
		return false;
	}
	if (itemType != Net::ASN1Util::IT_SEQUENCE)
	{
		i++;
		Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("1.1.")), i);
		itemPDU = Net::ASN1Util::PDUGetItem(this->buff.Arr(), this->buff.ArrEnd(), (const Char*)sbuff, itemLen, itemType);
		if (!itemPDU.SetTo(nnitemPDU) || itemType != Net::ASN1Util::IT_SEQUENCE)
		{
			return false;
		}
	}
	if (itemPDU.SetTo(nnitemPDU))
	{
		i = 0;
		while (true)
		{
			UnsafeArray<const UInt8> subitemPDU;
			UOSInt subitemLen;
			Text::StrConcatC(Text::StrUOSInt(sbuff, ++i), UTF8STRC(".1"));
			if (!Net::ASN1Util::PDUGetItem(nnitemPDU, nnitemPDU + itemLen, (const Char*)sbuff, subitemLen, itemType).SetTo(subitemPDU))
				break;
			if (itemType == Net::ASN1Util::IT_INTEGER && Text::StrEqualsC(subitemPDU, subitemLen, sn, snLen))
			{
				return true;
			}
		}
	}
	return false;
}
