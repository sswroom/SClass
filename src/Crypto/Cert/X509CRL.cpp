#include "Stdafx.h"
#include "Crypto/Cert/X509CRL.h"
#include "Net/ASN1Util.h"

Crypto::Cert::X509CRL::X509CRL(Text::String *sourceName, const UInt8 *buff, UOSInt buffSize) : Crypto::Cert::X509File(sourceName, buff, buffSize)
{

}

Crypto::Cert::X509CRL::X509CRL(Text::CString sourceName, const UInt8 *buff, UOSInt buffSize) : Crypto::Cert::X509File(sourceName, buff, buffSize)
{

}

Crypto::Cert::X509CRL::~X509CRL()
{

}

Crypto::Cert::X509File::FileType Crypto::Cert::X509CRL::GetFileType()
{
	return FileType::CRL;
}

void Crypto::Cert::X509CRL::ToShortName(Text::StringBuilderUTF8 *sb)
{
	UOSInt len = 0;
	Net::ASN1Util::ItemType itemType = Net::ASN1Util::IT_UNKNOWN;
	const UInt8 *tmpBuff = Net::ASN1Util::PDUGetItem(this->buff, this->buff + this->buffSize, "1.1.3", &len, &itemType);
	if (tmpBuff != 0 && itemType == Net::ASN1Util::IT_SEQUENCE)
	{
		NameGetCN(tmpBuff, tmpBuff + len, sb);
	}
	else
	{
		tmpBuff = Net::ASN1Util::PDUGetItem(this->buff, this->buff + this->buffSize, "1.1.2", &len, &itemType);
		if (tmpBuff != 0 && itemType == Net::ASN1Util::IT_SEQUENCE)
		{
			NameGetCN(tmpBuff, tmpBuff + len, sb);
		}
	}
}

Crypto::Cert::X509File::ValidStatus Crypto::Cert::X509CRL::IsValid(Net::SSLEngine *ssl, Crypto::Cert::CertStore *trustStore)
{
	return Crypto::Cert::X509File::ValidStatus::SignatureInvalid;
}

Net::ASN1Data *Crypto::Cert::X509CRL::Clone()
{
	Crypto::Cert::X509CRL *asn1;
	NEW_CLASS(asn1, Crypto::Cert::X509CRL(this->GetSourceNameObj(), this->buff, this->buffSize));
	return asn1;
}

void Crypto::Cert::X509CRL::ToString(Text::StringBuilderUTF8 *sb)
{
	if (IsCertificateList(this->buff, this->buff + this->buffSize, "1"))
	{
		AppendCertificateList(this->buff, this->buff + this->buffSize, "1", sb, CSTR_NULL);
	}
}
