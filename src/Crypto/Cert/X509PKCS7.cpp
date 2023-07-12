#include "Stdafx.h"
#include "Crypto/Cert/X509Cert.h"
#include "Crypto/Cert/X509PKCS7.h"
#include "Net/ASN1Util.h"

Crypto::Cert::X509PKCS7::X509PKCS7(Text::String *sourceName, const UInt8 *buff, UOSInt buffSize) : Crypto::Cert::X509File(sourceName, buff, buffSize)
{

}

Crypto::Cert::X509PKCS7::X509PKCS7(Text::CString sourceName, const UInt8 *buff, UOSInt buffSize) : Crypto::Cert::X509File(sourceName, buff, buffSize)
{

}

Crypto::Cert::X509PKCS7::~X509PKCS7()
{

}

Crypto::Cert::X509File::FileType Crypto::Cert::X509PKCS7::GetFileType() const
{
	return FileType::PKCS7;
}

void Crypto::Cert::X509PKCS7::ToShortName(Text::StringBuilderUTF8 *sb) const
{
/*	UOSInt len = 0;
	Net::ASN1Util::ItemType itemType = Net::ASN1Util::IT_UNKNOWN;
	const UInt8 *tmpBuff = Net::ASN1Util::PDUGetItem(this->buff, this->buff + this->buffSize, "1.1.2", &len, &itemType);
	if (tmpBuff != 0 && itemType == Net::ASN1Util::IT_SEQUENCE)
	{
		NameGetCN(tmpBuff, tmpBuff + len, sb);
	}*/
}

UOSInt Crypto::Cert::X509PKCS7::GetCertCount()
{
	Net::ASN1Util::ItemType itemType;
	UOSInt len;
	const UInt8 *certListPDU = Net::ASN1Util::PDUGetItem(this->buff, this->buff + this->buffSize, "1.2.1.4", &len, &itemType);
	if (certListPDU == 0 || itemType != Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)
	{
		return 0;
	}
	return Net::ASN1Util::PDUCountItem(certListPDU, certListPDU + len, 0);
}

Bool Crypto::Cert::X509PKCS7::GetCertName(UOSInt index, Text::StringBuilderUTF8 *sb)
{
	Net::ASN1Util::ItemType itemType;
	UOSInt len;
	const UInt8 *certListPDU = Net::ASN1Util::PDUGetItem(this->buff, this->buff + this->buffSize, "1.2.1.4", &len, &itemType);
	if (certListPDU == 0 || itemType != Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)
	{
		return false;
	}

	Char sbuff[32];
	Text::StrConcat(Text::StrUOSInt(sbuff, index + 1), ".1.1");
	UOSInt itemLen = 0;
	itemType = Net::ASN1Util::IT_UNKNOWN;
	const UInt8 *tmpBuff;
	if (Net::ASN1Util::PDUGetItemType(certListPDU, certListPDU + len, sbuff) == Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)
	{
		Text::StrConcat(Text::StrUOSInt(sbuff, index + 1), ".1.6");
		tmpBuff = Net::ASN1Util::PDUGetItem(certListPDU, certListPDU + len, sbuff, &itemLen, &itemType);
	}
	else
	{
		Text::StrConcat(Text::StrUOSInt(sbuff, index + 1), ".1.5");
		tmpBuff = Net::ASN1Util::PDUGetItem(certListPDU, certListPDU + len, sbuff, &itemLen, &itemType);
	}
	if (tmpBuff != 0 && itemType == Net::ASN1Util::IT_SEQUENCE)
	{
		NameGetCN(tmpBuff, tmpBuff + itemLen, sb);
		return true;
	}
	return false;
}

Crypto::Cert::X509Cert *Crypto::Cert::X509PKCS7::GetNewCert(UOSInt index)
{
	Net::ASN1Util::ItemType itemType;
	UOSInt len;
	const UInt8 *certListPDU = Net::ASN1Util::PDUGetItem(this->buff, this->buff + this->buffSize, "1.2.1.4", &len, &itemType);
	if (certListPDU == 0 || itemType != Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)
	{
		return 0;
	}
	UOSInt ofst;
	Char sbuff[32];
	Text::StrUOSInt(sbuff, index + 1);
	const UInt8 *certPDU = Net::ASN1Util::PDUGetItemRAW(certListPDU, certListPDU + len, sbuff, &len, &ofst);
	if (certPDU)
	{
		Crypto::Cert::X509Cert *cert;
		NEW_CLASS(cert, Crypto::Cert::X509Cert(this->GetSourceNameObj(), certPDU, len + ofst));
		return cert;
	}
	return 0;
}

Crypto::Cert::X509File::ValidStatus Crypto::Cert::X509PKCS7::IsValid(Net::SSLEngine *ssl, Crypto::Cert::CertStore *trustStore) const
{
	return Crypto::Cert::X509File::ValidStatus::SignatureInvalid;
}

Net::ASN1Data *Crypto::Cert::X509PKCS7::Clone() const
{
	Crypto::Cert::X509PKCS7 *asn1;
	NEW_CLASS(asn1, Crypto::Cert::X509PKCS7(this->GetSourceNameObj(), this->buff, this->buffSize));
	return asn1;
}

void Crypto::Cert::X509PKCS7::ToString(Text::StringBuilderUTF8 *sb) const
{
	if (IsContentInfo(this->buff, this->buff + this->buffSize, "1"))
	{
		AppendContentInfo(this->buff, this->buff + this->buffSize, "1", sb, CSTR_NULL, Crypto::Cert::X509File::ContentDataType::Unknown);
	}
}

Bool Crypto::Cert::X509PKCS7::IsSignData() const
{
	UOSInt itemLen;
	Net::ASN1Util::ItemType itemType;
	const UInt8 *itemPDU = Net::ASN1Util::PDUGetItem(this->buff, this->buff + this->buffSize, "1.1", &itemLen, &itemType);
	if (itemPDU == 0 || itemType != Net::ASN1Util::IT_OID)
	{
		return false;
	}
	return Net::ASN1Util::OIDEqualsText(itemPDU, itemLen, UTF8STRC("1.2.840.113549.1.7.2"));
}

Crypto::Hash::HashType Crypto::Cert::X509PKCS7::GetDigestType() const
{
	if (!this->IsSignData())
	{
		return Crypto::Hash::HashType::Unknown;
	}
	UOSInt itemLen;
	Net::ASN1Util::ItemType itemType;
	const UInt8 *itemPDU = Net::ASN1Util::PDUGetItem(this->buff, this->buff + this->buffSize, "1.2.1.5.1.3.1", &itemLen, &itemType);
	if (itemPDU && itemType == Net::ASN1Util::IT_OID)
	{
		return HashTypeFromOID(itemPDU, itemLen);
	}
	return Crypto::Hash::HashType::Unknown;
}

const UInt8 *Crypto::Cert::X509PKCS7::GetMessageDigest(UOSInt *digestSize) const
{
	Net::ASN1Util::ItemType itemType;
	UOSInt itemLen;
	const UInt8 *itemPDU;
	UOSInt subitemLen;
	const UInt8 *subitemPDU;
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	if ((itemPDU = Net::ASN1Util::PDUGetItem(this->buff, this->buff + this->buffSize, "1.2.1.5.1.4", &itemLen, &itemType)) != 0 && itemType == Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)
	{
		UOSInt i = 0;
		while (true)
		{
			sptr = Text::StrUOSInt(sbuff, ++i);
			Text::StrConcatC(sptr, UTF8STRC(".1"));
			if ((subitemPDU = Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, (const Char*)sbuff, &subitemLen, &itemType)) == 0)
			{
				break;
			}
			else if (itemType == Net::ASN1Util::IT_OID && Net::ASN1Util::OIDEqualsText(subitemPDU, subitemLen, UTF8STRC("1.2.840.113549.1.9.4")))
			{
				Text::StrConcatC(sptr, UTF8STRC(".2.1"));
				if ((subitemPDU = Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, (const Char*)sbuff, &subitemLen, &itemType)) != 0 &&
					itemType == Net::ASN1Util::IT_OCTET_STRING)
				{
					*digestSize = subitemLen;
					return subitemPDU;
				}
			}
		}
	}
	return 0;
}

const UInt8 *Crypto::Cert::X509PKCS7::GetEncryptedDigest(UOSInt *signSize) const
{
	Net::ASN1Util::ItemType itemType;
	UOSInt itemLen;
	const UInt8 *itemPDU;
	if ((itemPDU = Net::ASN1Util::PDUGetItem(this->buff, this->buff + this->buffSize, "1.2.1.5.1.6", &itemLen, &itemType)) != 0 && itemType == Net::ASN1Util::IT_OCTET_STRING)
	{
		*signSize = itemLen;
		return itemPDU;
	}
	return 0;
}
