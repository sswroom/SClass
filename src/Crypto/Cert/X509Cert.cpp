#include "Stdafx.h"
#include "Crypto/Cert/X509Cert.h"
#include "Crypto/Cert/X509Key.h"
#include "Net/ASN1Util.h"
#include "Net/SSLEngine.h"

Crypto::Cert::X509Cert::X509Cert(NotNullPtr<Text::String> sourceName, Data::ByteArrayR buff) : Crypto::Cert::X509File(sourceName, buff)
{

}

Crypto::Cert::X509Cert::X509Cert(Text::CString sourceName, Data::ByteArrayR buff) : Crypto::Cert::X509File(sourceName, buff)
{

}

Crypto::Cert::X509Cert::~X509Cert()
{

}

Bool Crypto::Cert::X509Cert::GetSubjectCN(Text::StringBuilderUTF8 *sb) const
{
	UOSInt len = 0;
	Net::ASN1Util::ItemType itemType = Net::ASN1Util::IT_UNKNOWN;
	const UInt8 *tmpBuff;
	if (Net::ASN1Util::PDUGetItemType(this->buff.Ptr(), this->buff.PtrEnd(), "1.1.1") == Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)
	{
		tmpBuff = Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.1.6", &len, &itemType);
	}
	else
	{
		tmpBuff = Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.1.5", &len, &itemType);
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
		tmpBuff = Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.1.6", &len, &itemType);
	}
	else
	{
		tmpBuff = Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.1.5", &len, &itemType);
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

Bool Crypto::Cert::X509Cert::GetIssuerCN(Text::StringBuilderUTF8 *sb) const
{
	UOSInt len = 0;
	Net::ASN1Util::ItemType itemType = Net::ASN1Util::IT_UNKNOWN;
	const UInt8 *tmpBuff;
	if (Net::ASN1Util::PDUGetItemType(this->buff.Ptr(), this->buff.PtrEnd(), "1.1.1") == Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)
	{
		tmpBuff = Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.1.4", &len, &itemType);
	}
	else
	{
		tmpBuff = Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.1.3", &len, &itemType);
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

Crypto::Cert::X509File::FileType Crypto::Cert::X509Cert::GetFileType() const
{
	return FileType::Cert;
}

void Crypto::Cert::X509Cert::ToShortName(Text::StringBuilderUTF8 *sb) const
{
	this->GetSubjectCN(sb);
}

UOSInt Crypto::Cert::X509Cert::GetCertCount()
{
	return 1;
}

Bool Crypto::Cert::X509Cert::GetCertName(UOSInt index, Text::StringBuilderUTF8 *sb)
{
	if (index != 0)
		return false;
	this->GetSubjectCN(sb);
	return true;
}

Crypto::Cert::X509Cert *Crypto::Cert::X509Cert::GetNewCert(UOSInt index)
{
	if (index != 0)
		return 0;
	return (Crypto::Cert::X509Cert*)this->Clone();
}

Crypto::Cert::X509File::ValidStatus Crypto::Cert::X509Cert::IsValid(Net::SSLEngine *ssl, Crypto::Cert::CertStore *trustStore) const
{
	if (trustStore == 0)
	{
		trustStore = ssl->GetTrustStore();
	}
	Text::StringBuilderUTF8 sb;
	if (!this->GetIssuerCN(&sb))
	{
		return Crypto::Cert::X509File::ValidStatus::FileFormatInvalid;
	}
	Data::DateTime dt;
	Int64 currTime = Data::DateTimeUtil::GetCurrTimeMillis();
	if (!this->GetNotBefore(&dt))
	{
		return Crypto::Cert::X509File::ValidStatus::FileFormatInvalid;
	}
	if (dt.ToTicks() > currTime)
	{
		return Crypto::Cert::X509File::ValidStatus::Expired;
	}
	if (!this->GetNotAfter(&dt))
	{
		return Crypto::Cert::X509File::ValidStatus::FileFormatInvalid;
	}
	if (dt.ToTicks() < currTime)
	{
		return Crypto::Cert::X509File::ValidStatus::Expired;
	}
	SignedInfo signedInfo;
	if (!this->GetSignedInfo(&signedInfo))
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
		if (!this->IsSelfSigned())
		{
			return Crypto::Cert::X509File::ValidStatus::UnknownIssuer;
		}
		Crypto::Cert::X509Key *key = this->GetNewPublicKey();
		if (key == 0)
		{
			return Crypto::Cert::X509File::ValidStatus::FileFormatInvalid;
		}
		Bool signValid = ssl->SignatureVerify(key, hashType, signedInfo.payload, signedInfo.payloadSize, signedInfo.signature, signedInfo.signSize);
		DEL_CLASS(key);
		if (signValid)
		{
			return Crypto::Cert::X509File::ValidStatus::SelfSigned;
		}
		else
		{
			return Crypto::Cert::X509File::ValidStatus::SignatureInvalid;
		}
	}

	Crypto::Cert::X509Key *key = issuer->GetNewPublicKey();
	if (key == 0)
	{
		return Crypto::Cert::X509File::ValidStatus::FileFormatInvalid;
	}
	Bool signValid = ssl->SignatureVerify(key, hashType, signedInfo.payload, signedInfo.payloadSize, signedInfo.signature, signedInfo.signSize);
	DEL_CLASS(key);
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

Net::ASN1Data *Crypto::Cert::X509Cert::Clone() const
{
	Crypto::Cert::X509Cert *asn1;
	NEW_CLASS(asn1, Crypto::Cert::X509Cert(this->GetSourceNameObj(), this->buff));
	return asn1;
}

Crypto::Cert::X509Cert *Crypto::Cert::X509Cert::CreateX509Cert() const
{
	Crypto::Cert::X509Cert *asn1;
	NEW_CLASS(asn1, Crypto::Cert::X509Cert(this->GetSourceNameObj(), this->buff));
	return asn1;
}

void Crypto::Cert::X509Cert::ToString(Text::StringBuilderUTF8 *sb) const
{
	if (IsCertificate(this->buff.Ptr(), this->buff.PtrEnd(), "1"))
	{
		AppendCertificate(this->buff.Ptr(), this->buff.PtrEnd(), "1", sb, CSTR_NULL);
	}
}

Bool Crypto::Cert::X509Cert::GetIssuerNames(CertNames *names) const
{
	Net::ASN1Util::ItemType itemType;
	UOSInt len;
	const UInt8 *pdu = Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.1.1", &len, &itemType);
	if (pdu == 0)
	{
		return false;
	}
	if (itemType == Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)
	{
		pdu = Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.1.4", &len, &itemType);
		if (pdu)
		{
			return NamesGet(pdu, pdu + len, names);
		}
	}
	else
	{
		pdu = Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.1.3", &len, &itemType);
		if (pdu)
		{
			return NamesGet(pdu, pdu + len, names);
		}
	}
	return false;
}

Bool Crypto::Cert::X509Cert::GetSubjNames(CertNames *names) const
{
	Net::ASN1Util::ItemType itemType;
	UOSInt len;
	const UInt8 *pdu = Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.1.1", &len, &itemType);
	if (pdu == 0)
	{
		return false;
	}
	if (itemType == Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)
	{
		pdu = Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.1.6", &len, &itemType);
		if (pdu)
		{
			return NamesGet(pdu, pdu + len, names);
		}
	}
	else
	{
		pdu = Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.1.5", &len, &itemType);
		if (pdu)
		{
			return NamesGet(pdu, pdu + len, names);
		}
	}
	return false;
}

Bool Crypto::Cert::X509Cert::GetExtensions(CertExtensions *ext) const
{
	Net::ASN1Util::ItemType itemType;
	UOSInt len;
	const UInt8 *pdu = Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.1.1", &len, &itemType);
	if (pdu == 0)
	{
		return false;
	}
	if (itemType == Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)
	{
		pdu = Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.1.8.1", &len, &itemType);
		if (pdu)
		{
			return ExtensionsGet(pdu, pdu + len, ext);
		}
	}
	else
	{
		pdu = Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.1.7.1", &len, &itemType);
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
	const UInt8 *pdu = Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.1.1", &len, &itemType);
	if (pdu == 0)
	{
		return 0;
	}
	if (itemType == Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)
	{
		pdu = Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.1.7", &len, &itemType);
		if (pdu)
		{
			return PublicKeyGetNew(pdu, pdu + len);
		}
	}
	else
	{
		pdu = Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.1.6", &len, &itemType);
		if (pdu)
		{
			return PublicKeyGetNew(pdu, pdu + len);
		}
	}
	return 0;

}

Bool Crypto::Cert::X509Cert::GetKeyId(UInt8 *keyId) const
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

Bool Crypto::Cert::X509Cert::GetNotBefore(Data::DateTime *dt) const
{
	UOSInt len = 0;
	Net::ASN1Util::ItemType itemType = Net::ASN1Util::IT_UNKNOWN;
	const UInt8 *tmpBuff;
	if (Net::ASN1Util::PDUGetItemType(this->buff.Ptr(), this->buff.PtrEnd(), "1.1.1") == Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)
	{
		tmpBuff = Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.1.5.1", &len, &itemType);
	}
	else
	{
		tmpBuff = Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.1.4.1", &len, &itemType);
	}
	if (itemType == Net::ASN1Util::IT_UTCTIME || itemType == Net::ASN1Util::IT_GENERALIZEDTIME)
	{
		return Net::ASN1Util::PDUParseUTCTimeCont(tmpBuff, len, dt);
	}
	return false;
}

Bool Crypto::Cert::X509Cert::GetNotAfter(Data::DateTime *dt) const
{
	UOSInt len = 0;
	Net::ASN1Util::ItemType itemType = Net::ASN1Util::IT_UNKNOWN;
	const UInt8 *tmpBuff;
	if (Net::ASN1Util::PDUGetItemType(this->buff.Ptr(), this->buff.PtrEnd(), "1.1.1") == Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)
	{
		tmpBuff = Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.1.5.2", &len, &itemType);
	}
	else
	{
		tmpBuff = Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.1.4.2", &len, &itemType);
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
	UOSInt i;
	UOSInt j;
	Text::String *s;
	Bool valid = false;
	Crypto::Cert::CertNames subjNames;
	MemClear(&subjNames, sizeof(subjNames));
	if (this->GetSubjNames(&subjNames))
	{
		if (subjNames.commonName->v[0] == '*' && subjNames.commonName->v[1] == '.')
		{
			valid = domain.EqualsICase(&subjNames.commonName->v[2], subjNames.commonName->leng - 2) || domain.EndsWithICase(&subjNames.commonName->v[1], subjNames.commonName->leng - 1);
		}
		else
		{
			valid = domain.EqualsICase(subjNames.commonName);
		}
		Crypto::Cert::CertNames::FreeNames(&subjNames);
		if (valid)
		{
			return true;
		}
	}

	MemClear(&exts, sizeof(exts));
	if (this->GetExtensions(&exts))
	{
		if (exts.subjectAltName)
		{
			i = 0;
			j = exts.subjectAltName->GetCount();
			while (i < j)
			{
				s = exts.subjectAltName->GetItem(i);
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
				i++;
			}
		}
		Crypto::Cert::CertExtensions::FreeExtensions(&exts);
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
	if (this->GetIssuerNames(&issueNames) && this->GetSubjNames(&subjNames))
	{
		ret = issueNames.commonName->Equals(subjNames.commonName);
	}
	Crypto::Cert::CertNames::FreeNames(&subjNames);
	Crypto::Cert::CertNames::FreeNames(&issueNames);
	return ret;
}

UOSInt Crypto::Cert::X509Cert::GetCRLDistributionPoints(Data::ArrayList<Text::CString> *crlDistributionPoints) const
{
	Net::ASN1Util::ItemType itemType;
	UOSInt len;
	const UInt8 *pdu = Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.1.1", &len, &itemType);
	if (pdu == 0)
	{
		return 0;
	}
	if (itemType == Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)
	{
		pdu = Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.1.8.1", &len, &itemType);
		if (pdu)
		{
			return ExtensionsGetCRLDistributionPoints(pdu, pdu + len, crlDistributionPoints);
		}
	}
	else
	{
		pdu = Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.1.7.1", &len, &itemType);
		if (pdu)
		{
			return ExtensionsGetCRLDistributionPoints(pdu, pdu + len, crlDistributionPoints);
		}
	}
	return 0;

}

const UInt8 *Crypto::Cert::X509Cert::GetIssuerNamesSeq(UOSInt *dataLen) const
{
	Net::ASN1Util::ItemType itemType;
	UOSInt len;
	const UInt8 *pdu = Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.1.1", &len, &itemType);
	if (pdu == 0)
	{
		return 0;
	}
	if (itemType == Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)
	{
		pdu = Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.1.4", &len, &itemType);
		if (pdu && itemType == Net::ASN1Util::IT_SEQUENCE)
		{
			*dataLen = len;
			return pdu;
		}
	}
	else
	{
		pdu = Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.1.3", &len, &itemType);
		if (pdu && itemType == Net::ASN1Util::IT_SEQUENCE)
		{
			*dataLen = len;
			return pdu;
		}
	}
	return 0;
}

const UInt8 *Crypto::Cert::X509Cert::GetSerialNumber(UOSInt *dataLen) const
{
	Net::ASN1Util::ItemType itemType;
	UOSInt len;
	const UInt8 *pdu = Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.1.1", &len, &itemType);
	if (pdu == 0)
	{
		return 0;
	}
	if (itemType == Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)
	{
		pdu = Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.1.2", &len, &itemType);
		if (pdu && itemType == Net::ASN1Util::IT_INTEGER)
		{
			*dataLen = len;
			return pdu;
		}
	}
	else
	{
		pdu = Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.1.1", &len, &itemType);
		if (pdu && itemType == Net::ASN1Util::IT_INTEGER)
		{
			*dataLen = len;
			return pdu;
		}
	}
	return 0;
}
