#include "Stdafx.h"
#include "Crypto/Cert/X509Cert.h"
#include "Crypto/Cert/X509Key.h"
#include "Net/ASN1Util.h"

Crypto::Cert::X509Cert::X509Cert(Text::String *sourceName, const UInt8 *buff, UOSInt buffSize) : Crypto::Cert::X509File(sourceName, buff, buffSize)
{

}

Crypto::Cert::X509Cert::X509Cert(Text::CString sourceName, const UInt8 *buff, UOSInt buffSize) : Crypto::Cert::X509File(sourceName, buff, buffSize)
{

}

Crypto::Cert::X509Cert::~X509Cert()
{

}

void Crypto::Cert::X509Cert::GetSubjectCN(Text::StringBuilderUTF8 *sb)
{
	UOSInt len = 0;
	Net::ASN1Util::ItemType itemType = Net::ASN1Util::IT_UNKNOWN;
	const UInt8 *tmpBuff;
	if (Net::ASN1Util::PDUGetItemType(this->buff, this->buff + this->buffSize, "1.1.1") == Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)
	{
		tmpBuff = Net::ASN1Util::PDUGetItem(this->buff, this->buff + this->buffSize, "1.1.6", &len, &itemType);
	}
	else
	{
		tmpBuff = Net::ASN1Util::PDUGetItem(this->buff, this->buff + this->buffSize, "1.1.5", &len, &itemType);
	}
	if (tmpBuff != 0 && itemType == Net::ASN1Util::IT_SEQUENCE)
	{
		NameGetCN(tmpBuff, tmpBuff + len, sb);
	}
}

Crypto::Cert::X509File::FileType Crypto::Cert::X509Cert::GetFileType()
{
	return FileType::Cert;
}

void Crypto::Cert::X509Cert::ToShortName(Text::StringBuilderUTF8 *sb)
{
	this->GetSubjectCN(sb);
}

UOSInt Crypto::Cert::X509Cert::GetCertCount()
{
	return 0;
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

Net::ASN1Data *Crypto::Cert::X509Cert::Clone()
{
	Crypto::Cert::X509Cert *asn1;
	NEW_CLASS(asn1, Crypto::Cert::X509Cert(this->GetSourceNameObj(), this->buff, this->buffSize));
	return asn1;
}

Crypto::Cert::X509Cert *Crypto::Cert::X509Cert::CreateX509Cert()
{
	Crypto::Cert::X509Cert *asn1;
	NEW_CLASS(asn1, Crypto::Cert::X509Cert(this->GetSourceNameObj(), this->buff, this->buffSize));
	return asn1;
}

void Crypto::Cert::X509Cert::ToString(Text::StringBuilderUTF8 *sb)
{
	if (IsCertificate(this->buff, this->buff + this->buffSize, "1"))
	{
		AppendCertificate(this->buff, this->buff + this->buffSize, "1", sb, CSTR_NULL);
	}
}

Bool Crypto::Cert::X509Cert::GetIssueNames(CertNames *names)
{
	Net::ASN1Util::ItemType itemType;
	UOSInt len;
	const UInt8 *pdu = Net::ASN1Util::PDUGetItem(this->buff, this->buff + this->buffSize, "1.1.1", &len, &itemType);
	if (pdu == 0)
	{
		return false;
	}
	if (itemType == Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)
	{
		pdu = Net::ASN1Util::PDUGetItem(this->buff, this->buff + this->buffSize, "1.1.4", &len, &itemType);
		if (pdu)
		{
			return NamesGet(pdu, pdu + len, names);
		}
	}
	else
	{
		pdu = Net::ASN1Util::PDUGetItem(this->buff, this->buff + this->buffSize, "1.1.3", &len, &itemType);
		if (pdu)
		{
			return NamesGet(pdu, pdu + len, names);
		}
	}
	return false;
}

Bool Crypto::Cert::X509Cert::GetSubjNames(CertNames *names)
{
	Net::ASN1Util::ItemType itemType;
	UOSInt len;
	const UInt8 *pdu = Net::ASN1Util::PDUGetItem(this->buff, this->buff + this->buffSize, "1.1.1", &len, &itemType);
	if (pdu == 0)
	{
		return false;
	}
	if (itemType == Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)
	{
		pdu = Net::ASN1Util::PDUGetItem(this->buff, this->buff + this->buffSize, "1.1.6", &len, &itemType);
		if (pdu)
		{
			return NamesGet(pdu, pdu + len, names);
		}
	}
	else
	{
		pdu = Net::ASN1Util::PDUGetItem(this->buff, this->buff + this->buffSize, "1.1.5", &len, &itemType);
		if (pdu)
		{
			return NamesGet(pdu, pdu + len, names);
		}
	}
	return false;
}

Bool Crypto::Cert::X509Cert::GetExtensions(CertExtensions *ext)
{
	Net::ASN1Util::ItemType itemType;
	UOSInt len;
	const UInt8 *pdu = Net::ASN1Util::PDUGetItem(this->buff, this->buff + this->buffSize, "1.1.1", &len, &itemType);
	if (pdu == 0)
	{
		return false;
	}
	if (itemType == Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)
	{
		pdu = Net::ASN1Util::PDUGetItem(this->buff, this->buff + this->buffSize, "1.1.8.1", &len, &itemType);
		if (pdu)
		{
			return ExtensionsGet(pdu, pdu + len, ext);
		}
	}
	else
	{
		pdu = Net::ASN1Util::PDUGetItem(this->buff, this->buff + this->buffSize, "1.1.7.1", &len, &itemType);
		if (pdu)
		{
			return ExtensionsGet(pdu, pdu + len, ext);
		}
	}
	return false;
}

Crypto::Cert::X509Key *Crypto::Cert::X509Cert::GetNewPublicKey()
{
	Net::ASN1Util::ItemType itemType;
	UOSInt len;
	const UInt8 *pdu = Net::ASN1Util::PDUGetItem(this->buff, this->buff + this->buffSize, "1.1.1", &len, &itemType);
	if (pdu == 0)
	{
		return 0;
	}
	if (itemType == Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)
	{
		pdu = Net::ASN1Util::PDUGetItem(this->buff, this->buff + this->buffSize, "1.1.7", &len, &itemType);
		if (pdu)
		{
			return PublicKeyGetNew(pdu, pdu + len);
		}
	}
	else
	{
		pdu = Net::ASN1Util::PDUGetItem(this->buff, this->buff + this->buffSize, "1.1.6", &len, &itemType);
		if (pdu)
		{
			return PublicKeyGetNew(pdu, pdu + len);
		}
	}
	return 0;

}

Bool Crypto::Cert::X509Cert::GetKeyId(UInt8 *keyId)
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

Bool Crypto::Cert::X509Cert::GetNotBefore(Data::DateTime *dt)
{
	UOSInt len = 0;
	Net::ASN1Util::ItemType itemType = Net::ASN1Util::IT_UNKNOWN;
	const UInt8 *tmpBuff;
	if (Net::ASN1Util::PDUGetItemType(this->buff, this->buff + this->buffSize, "1.1.1") == Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)
	{
		tmpBuff = Net::ASN1Util::PDUGetItem(this->buff, this->buff + this->buffSize, "1.1.5.1", &len, &itemType);
	}
	else
	{
		tmpBuff = Net::ASN1Util::PDUGetItem(this->buff, this->buff + this->buffSize, "1.1.4.1", &len, &itemType);
	}
	if (itemType == Net::ASN1Util::IT_UTCTIME)
	{
		return Net::ASN1Util::PDUParseUTCTimeCont(tmpBuff, len, dt);
	}
	return false;
}

Bool Crypto::Cert::X509Cert::GetNotAfter(Data::DateTime *dt)
{
	UOSInt len = 0;
	Net::ASN1Util::ItemType itemType = Net::ASN1Util::IT_UNKNOWN;
	const UInt8 *tmpBuff;
	if (Net::ASN1Util::PDUGetItemType(this->buff, this->buff + this->buffSize, "1.1.1") == Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)
	{
		tmpBuff = Net::ASN1Util::PDUGetItem(this->buff, this->buff + this->buffSize, "1.1.5.2", &len, &itemType);
	}
	else
	{
		tmpBuff = Net::ASN1Util::PDUGetItem(this->buff, this->buff + this->buffSize, "1.1.4.2", &len, &itemType);
	}
	if (itemType == Net::ASN1Util::IT_UTCTIME)
	{
		return Net::ASN1Util::PDUParseUTCTimeCont(tmpBuff, len, dt);
	}
	return false;
}

Bool Crypto::Cert::X509Cert::DomainValid(Text::CString domain)
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

Bool Crypto::Cert::X509Cert::IsSelfSigned()
{
	Crypto::Cert::CertNames subjNames;
	Crypto::Cert::CertNames issueNames;
	MemClear(&subjNames, sizeof(subjNames));
	MemClear(&issueNames, sizeof(issueNames));
	Bool ret = false;
	if (this->GetIssueNames(&issueNames) && this->GetSubjNames(&subjNames))
	{
		ret = issueNames.commonName->Equals(subjNames.commonName);
	}
	Crypto::Cert::CertNames::FreeNames(&subjNames);
	Crypto::Cert::CertNames::FreeNames(&issueNames);
	return ret;
}

const UInt8 *Crypto::Cert::X509Cert::GetIssueNamesSeq(UOSInt *dataLen)
{
	Net::ASN1Util::ItemType itemType;
	UOSInt len;
	const UInt8 *pdu = Net::ASN1Util::PDUGetItem(this->buff, this->buff + this->buffSize, "1.1.1", &len, &itemType);
	if (pdu == 0)
	{
		return 0;
	}
	if (itemType == Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)
	{
		pdu = Net::ASN1Util::PDUGetItem(this->buff, this->buff + this->buffSize, "1.1.4", &len, &itemType);
		if (pdu && itemType == Net::ASN1Util::IT_SEQUENCE)
		{
			*dataLen = len;
			return pdu;
		}
	}
	else
	{
		pdu = Net::ASN1Util::PDUGetItem(this->buff, this->buff + this->buffSize, "1.1.3", &len, &itemType);
		if (pdu && itemType == Net::ASN1Util::IT_SEQUENCE)
		{
			*dataLen = len;
			return pdu;
		}
	}
	return 0;
}

const UInt8 *Crypto::Cert::X509Cert::GetSerialNumber(UOSInt *dataLen)
{
	Net::ASN1Util::ItemType itemType;
	UOSInt len;
	const UInt8 *pdu = Net::ASN1Util::PDUGetItem(this->buff, this->buff + this->buffSize, "1.1.1", &len, &itemType);
	if (pdu == 0)
	{
		return 0;
	}
	if (itemType == Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)
	{
		pdu = Net::ASN1Util::PDUGetItem(this->buff, this->buff + this->buffSize, "1.1.2", &len, &itemType);
		if (pdu && itemType == Net::ASN1Util::IT_INTEGER)
		{
			*dataLen = len;
			return pdu;
		}
	}
	else
	{
		pdu = Net::ASN1Util::PDUGetItem(this->buff, this->buff + this->buffSize, "1.1.1", &len, &itemType);
		if (pdu && itemType == Net::ASN1Util::IT_INTEGER)
		{
			*dataLen = len;
			return pdu;
		}
	}
	return 0;
}
