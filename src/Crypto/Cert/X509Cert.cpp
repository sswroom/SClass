#include "Stdafx.h"
#include "Crypto/Cert/X509Cert.h"
#include "Net/ASN1Util.h"

Crypto::Cert::X509Cert::X509Cert(const UTF8Char *sourceName, const UInt8 *buff, UOSInt buffSize) : Crypto::Cert::X509File(sourceName, buff, buffSize)
{

}

Crypto::Cert::X509Cert::~X509Cert()
{

}

void Crypto::Cert::X509Cert::GetSubjectCN(Text::StringBuilderUTF *sb)
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

void Crypto::Cert::X509Cert::ToShortName(Text::StringBuilderUTF *sb)
{
	this->GetSubjectCN(sb);
}

Net::ASN1Data *Crypto::Cert::X509Cert::Clone()
{
	Crypto::Cert::X509Cert *asn1;
	NEW_CLASS(asn1, Crypto::Cert::X509Cert(this->GetSourceNameObj(), this->buff, this->buffSize));
	return asn1;
}

void Crypto::Cert::X509Cert::ToString(Text::StringBuilderUTF *sb)
{
	if (IsCertificate(this->buff, this->buff + this->buffSize, "1"))
	{
		AppendCertificate(this->buff, this->buff + this->buffSize, "1", sb);
	}
}
