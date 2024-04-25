#include "Stdafx.h"
#include "Crypto/Cert/X509Cert.h"
#include "Crypto/Cert/X509Key.h"
#include "Net/ASN1Util.h"
#include "Net/SSLEngine.h"
#include "Text/StringTool.h"

Crypto::Cert::X509Cert::X509Cert(NN<Text::String> sourceName, Data::ByteArrayR buff) : Crypto::Cert::X509File(sourceName, buff)
{

}

Crypto::Cert::X509Cert::X509Cert(Text::CStringNN sourceName, Data::ByteArrayR buff) : Crypto::Cert::X509File(sourceName, buff)
{

}

Crypto::Cert::X509Cert::~X509Cert()
{

}

Bool Crypto::Cert::X509Cert::GetSubjectCN(NN<Text::StringBuilderUTF8> sb) const
{
	UOSInt len = 0;
	Net::ASN1Util::ItemType itemType = Net::ASN1Util::IT_UNKNOWN;
	const UInt8 *tmpBuff;
	if (Net::ASN1Util::PDUGetItemType(this->buff.Ptr(), this->buff.PtrEnd(), "1.1.1") == Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)
	{
		tmpBuff = Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.1.6", len, itemType);
	}
	else
	{
		tmpBuff = Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.1.5", len, itemType);
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

UTF8Char *Crypto::Cert::X509Cert::GetSubjectCN(UTF8Char *sbuff) const
{
	UOSInt len = 0;
	Net::ASN1Util::ItemType itemType = Net::ASN1Util::IT_UNKNOWN;
	const UInt8 *tmpBuff;
	if (Net::ASN1Util::PDUGetItemType(this->buff.Ptr(), this->buff.PtrEnd(), "1.1.1") == Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)
	{
		tmpBuff = Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.1.6", len, itemType);
	}
	else
	{
		tmpBuff = Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.1.5", len, itemType);
	}
	if (tmpBuff != 0 && itemType == Net::ASN1Util::IT_SEQUENCE)
	{
		return NameGetCN(tmpBuff, tmpBuff + len, sbuff);
	}
	else
	{
		return 0;
	}
}

Bool Crypto::Cert::X509Cert::GetIssuerCN(NN<Text::StringBuilderUTF8> sb) const
{
	UOSInt len = 0;
	Net::ASN1Util::ItemType itemType = Net::ASN1Util::IT_UNKNOWN;
	const UInt8 *tmpBuff;
	if (Net::ASN1Util::PDUGetItemType(this->buff.Ptr(), this->buff.PtrEnd(), "1.1.1") == Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)
	{
		tmpBuff = Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.1.4", len, itemType);
	}
	else
	{
		tmpBuff = Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.1.3", len, itemType);
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

void Crypto::Cert::X509Cert::SetDefaultSourceName()
{
	Text::StringBuilderUTF8 sb;
	if (GetSubjectCN(sb))
	{
		sb.AppendC(UTF8STRC(".crt"));
		this->SetSourceName(sb.ToCString());
	}
}

Crypto::Cert::X509File::FileType Crypto::Cert::X509Cert::GetFileType() const
{
	return FileType::Cert;
}

void Crypto::Cert::X509Cert::ToShortName(NN<Text::StringBuilderUTF8> sb) const
{
	this->GetSubjectCN(sb);
}

UOSInt Crypto::Cert::X509Cert::GetCertCount()
{
	return 1;
}

Bool Crypto::Cert::X509Cert::GetCertName(UOSInt index, NN<Text::StringBuilderUTF8> sb)
{
	if (index != 0)
		return false;
	this->GetSubjectCN(sb);
	return true;
}

Optional<Crypto::Cert::X509Cert> Crypto::Cert::X509Cert::GetNewCert(UOSInt index)
{
	if (index != 0)
		return 0;
	return NN<Crypto::Cert::X509Cert>::ConvertFrom(this->Clone());
}

Crypto::Cert::X509File::ValidStatus Crypto::Cert::X509Cert::IsValid(NN<Net::SSLEngine> ssl, Optional<Crypto::Cert::CertStore> trustStore) const
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
	if (!this->GetNotBefore(dt))
	{
		return Crypto::Cert::X509File::ValidStatus::FileFormatInvalid;
	}
	if (dt.ToTicks() > currTime)
	{
		return Crypto::Cert::X509File::ValidStatus::Expired;
	}
	if (!this->GetNotAfter(dt))
	{
		return Crypto::Cert::X509File::ValidStatus::FileFormatInvalid;
	}
	if (dt.ToTicks() < currTime)
	{
		return Crypto::Cert::X509File::ValidStatus::Expired;
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
		if (!this->IsSelfSigned())
		{
			return Crypto::Cert::X509File::ValidStatus::UnknownIssuer;
		}
		NN<Crypto::Cert::X509Key> key;
		if (!key.Set(this->GetNewPublicKey()))
		{
			return Crypto::Cert::X509File::ValidStatus::FileFormatInvalid;
		}
		Bool signValid = ssl->SignatureVerify(key, hashType, signedInfo.payload, signedInfo.payloadSize, signedInfo.signature, signedInfo.signSize);
		key.Delete();
		if (signValid)
		{
			return Crypto::Cert::X509File::ValidStatus::SelfSigned;
		}
		else
		{
			return Crypto::Cert::X509File::ValidStatus::SignatureInvalid;
		}
	}

	NN<Crypto::Cert::X509Key> key;
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

	Data::ArrayList<Text::CString> crlDistributionPoints;
	this->GetCRLDistributionPoints(&crlDistributionPoints);
	//////////////////////////
	// CRL
	return Crypto::Cert::X509File::ValidStatus::Valid;
}

NN<Net::ASN1Data> Crypto::Cert::X509Cert::Clone() const
{
	NN<Crypto::Cert::X509Cert> asn1;
	NEW_CLASSNN(asn1, Crypto::Cert::X509Cert(this->GetSourceNameObj(), this->buff));
	return asn1;
}

Optional<Crypto::Cert::X509Cert> Crypto::Cert::X509Cert::CreateX509Cert() const
{
	NN<Crypto::Cert::X509Cert> asn1;
	NEW_CLASSNN(asn1, Crypto::Cert::X509Cert(this->GetSourceNameObj(), this->buff));
	return asn1;
}

void Crypto::Cert::X509Cert::ToString(NN<Text::StringBuilderUTF8> sb) const
{
	if (IsCertificate(this->buff.Ptr(), this->buff.PtrEnd(), "1"))
	{
		AppendCertificate(this->buff.Ptr(), this->buff.PtrEnd(), "1", sb, CSTR_NULL);
	}
}

NN<Net::ASN1Names> Crypto::Cert::X509Cert::CreateNames() const
{
	NN<Net::ASN1Names> names;
	NEW_CLASSNN(names, Net::ASN1Names());
	return names->SetCertificate();
}

Bool Crypto::Cert::X509Cert::GetIssuerNames(NN<CertNames> names) const
{
	Net::ASN1Util::ItemType itemType;
	UOSInt len;
	const UInt8 *pdu = Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.1.1", len, itemType);
	if (pdu == 0)
	{
		return false;
	}
	if (itemType == Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)
	{
		pdu = Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.1.4", len, itemType);
		if (pdu)
		{
			return NamesGet(pdu, pdu + len, names);
		}
	}
	else
	{
		pdu = Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.1.3", len, itemType);
		if (pdu)
		{
			return NamesGet(pdu, pdu + len, names);
		}
	}
	return false;
}

Bool Crypto::Cert::X509Cert::GetSubjNames(NN<CertNames> names) const
{
	Net::ASN1Util::ItemType itemType;
	UOSInt len;
	const UInt8 *pdu = Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.1.1", len, itemType);
	if (pdu == 0)
	{
		return false;
	}
	if (itemType == Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)
	{
		pdu = Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.1.6", len, itemType);
		if (pdu)
		{
			return NamesGet(pdu, pdu + len, names);
		}
	}
	else
	{
		pdu = Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.1.5", len, itemType);
		if (pdu)
		{
			return NamesGet(pdu, pdu + len, names);
		}
	}
	return false;
}

Bool Crypto::Cert::X509Cert::GetExtensions(NN<CertExtensions> ext) const
{
	Net::ASN1Util::ItemType itemType;
	UOSInt len;
	const UInt8 *pdu = Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.1.1", len, itemType);
	if (pdu == 0)
	{
		return false;
	}
	if (itemType == Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)
	{
		pdu = Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.1.8.1", len, itemType);
		if (pdu)
		{
			return ExtensionsGet(pdu, pdu + len, ext);
		}
	}
	else
	{
		pdu = Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.1.7.1", len, itemType);
		if (pdu)
		{
			return ExtensionsGet(pdu, pdu + len, ext);
		}
	}
	return false;
}

Crypto::Cert::X509Key *Crypto::Cert::X509Cert::GetNewPublicKey() const
{
	Net::ASN1Util::ItemType itemType;
	UOSInt len;
	const UInt8 *pdu = Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.1.1", len, itemType);
	if (pdu == 0)
	{
		return 0;
	}
	if (itemType == Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)
	{
		pdu = Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.1.7", len, itemType);
		if (pdu)
		{
			return PublicKeyGetNew(pdu, pdu + len);
		}
	}
	else
	{
		pdu = Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.1.6", len, itemType);
		if (pdu)
		{
			return PublicKeyGetNew(pdu, pdu + len);
		}
	}
	return 0;
}

Bool Crypto::Cert::X509Cert::GetKeyId(const Data::ByteArray &keyId) const
{
	Crypto::Cert::X509Key *key = GetNewPublicKey();
	if (key == 0)
	{
		return false;
	}
	if (key->GetKeyId(keyId))
	{
		DEL_CLASS(key);
		return true;
	}
	DEL_CLASS(key);
	return false;
}

Bool Crypto::Cert::X509Cert::GetNotBefore(NN<Data::DateTime> dt) const
{
	UOSInt len = 0;
	Net::ASN1Util::ItemType itemType = Net::ASN1Util::IT_UNKNOWN;
	const UInt8 *tmpBuff;
	if (Net::ASN1Util::PDUGetItemType(this->buff.Ptr(), this->buff.PtrEnd(), "1.1.1") == Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)
	{
		tmpBuff = Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.1.5.1", len, itemType);
	}
	else
	{
		tmpBuff = Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.1.4.1", len, itemType);
	}
	if (itemType == Net::ASN1Util::IT_UTCTIME || itemType == Net::ASN1Util::IT_GENERALIZEDTIME)
	{
		return Net::ASN1Util::PDUParseUTCTimeCont(tmpBuff, len, dt);
	}
	return false;
}

Bool Crypto::Cert::X509Cert::GetNotAfter(NN<Data::DateTime> dt) const
{
	UOSInt len = 0;
	Net::ASN1Util::ItemType itemType = Net::ASN1Util::IT_UNKNOWN;
	const UInt8 *tmpBuff;
	if (Net::ASN1Util::PDUGetItemType(this->buff.Ptr(), this->buff.PtrEnd(), "1.1.1") == Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)
	{
		tmpBuff = Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.1.5.2", len, itemType);
	}
	else
	{
		tmpBuff = Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.1.4.2", len, itemType);
	}
	if (itemType == Net::ASN1Util::IT_UTCTIME || itemType == Net::ASN1Util::IT_GENERALIZEDTIME)
	{
		return Net::ASN1Util::PDUParseUTCTimeCont(tmpBuff, len, dt);
	}
	return false;
}

Bool Crypto::Cert::X509Cert::DomainValid(Text::CString domain) const
{
	Crypto::Cert::CertExtensions exts;
	NN<Text::String> s;
	Bool valid = false;
	Crypto::Cert::CertNames subjNames;
	MemClear(&subjNames, sizeof(subjNames));
	if (this->GetSubjNames(subjNames))
	{
		NN<Text::String> cn;
		if (subjNames.commonName.SetTo(cn))
		{
			if (cn->v[0] == '*' && cn->v[1] == '.')
			{
				valid = domain.EqualsICase(&cn->v[2], cn->leng - 2) || domain.EndsWithICase(&cn->v[1], cn->leng - 1);
			}
			else
			{
				valid = domain.EqualsICase(cn);
			}
		}
		Crypto::Cert::CertNames::FreeNames(subjNames);
		if (valid)
		{
			return true;
		}
	}

	MemClear(&exts, sizeof(exts));
	if (this->GetExtensions(exts))
	{
		NN<Data::ArrayListStringNN> strList;
		if (exts.subjectAltName.SetTo(strList))
		{
			Data::ArrayIterator<NN<Text::String>> it = strList->Iterator();
			while (it.HasNext())
			{
				s = it.Next();
				if (s->v[0] == '*' && s->v[1] == '.')
				{
					valid = domain.EqualsICase(&s->v[2], s->leng - 2) || domain.EndsWithICase(&s->v[1], s->leng - 1);
				}
				else
				{
					valid = domain.EqualsICase(s);
				}
				if (valid)
					break;
			}
		}
		Crypto::Cert::CertExtensions::FreeExtensions(exts);
		if (valid)
		{
			return true;
		}
	}
	return false;
}

Bool Crypto::Cert::X509Cert::IsSelfSigned() const
{
	Crypto::Cert::CertNames subjNames;
	Crypto::Cert::CertNames issueNames;
	MemClear(&subjNames, sizeof(subjNames));
	MemClear(&issueNames, sizeof(issueNames));
	Bool ret = false;
	if (this->GetIssuerNames(issueNames) && this->GetSubjNames(subjNames))
	{
		ret = Text::StringTool::Equals(issueNames.commonName, subjNames.commonName);
	}
	Crypto::Cert::CertNames::FreeNames(subjNames);
	Crypto::Cert::CertNames::FreeNames(issueNames);
	return ret;
}

UOSInt Crypto::Cert::X509Cert::GetCRLDistributionPoints(Data::ArrayList<Text::CString> *crlDistributionPoints) const
{
	Net::ASN1Util::ItemType itemType;
	UOSInt len;
	const UInt8 *pdu = Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.1.1", len, itemType);
	if (pdu == 0)
	{
		return 0;
	}
	if (itemType == Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)
	{
		pdu = Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.1.8.1", len, itemType);
		if (pdu)
		{
			return ExtensionsGetCRLDistributionPoints(pdu, pdu + len, crlDistributionPoints);
		}
	}
	else
	{
		pdu = Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.1.7.1", len, itemType);
		if (pdu)
		{
			return ExtensionsGetCRLDistributionPoints(pdu, pdu + len, crlDistributionPoints);
		}
	}
	return 0;

}

const UInt8 *Crypto::Cert::X509Cert::GetIssuerNamesSeq(OutParam<UOSInt> dataLen) const
{
	Net::ASN1Util::ItemType itemType;
	UOSInt len;
	const UInt8 *pdu = Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.1.1", len, itemType);
	if (pdu == 0)
	{
		return 0;
	}
	if (itemType == Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)
	{
		pdu = Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.1.4", len, itemType);
		if (pdu && itemType == Net::ASN1Util::IT_SEQUENCE)
		{
			dataLen.Set(len);
			return pdu;
		}
	}
	else
	{
		pdu = Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.1.3", len, itemType);
		if (pdu && itemType == Net::ASN1Util::IT_SEQUENCE)
		{
			dataLen.Set(len);
			return pdu;
		}
	}
	return 0;
}

const UInt8 *Crypto::Cert::X509Cert::GetSerialNumber(OutParam<UOSInt> dataLen) const
{
	Net::ASN1Util::ItemType itemType;
	UOSInt len;
	const UInt8 *pdu = Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.1.1", len, itemType);
	if (pdu == 0)
	{
		return 0;
	}
	if (itemType == Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)
	{
		pdu = Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.1.2", len, itemType);
		if (pdu && itemType == Net::ASN1Util::IT_INTEGER)
		{
			dataLen.Set(len);
			return pdu;
		}
	}
	else
	{
		pdu = Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.1.1", len, itemType);
		if (pdu && itemType == Net::ASN1Util::IT_INTEGER)
		{
			dataLen.Set(len);
			return pdu;
		}
	}
	return 0;
}
