#include "Stdafx.h"
#include "Crypto/Cert/X509CertReq.h"
#include "Crypto/Cert/X509Key.h"
#include "Net/ASN1Util.h"
#include "Net/SSLEngine.h"

Crypto::Cert::X509CertReq::X509CertReq(Text::String *sourceName, const UInt8 *buff, UOSInt buffSize) : Crypto::Cert::X509File(sourceName, buff, buffSize)
{

}

Crypto::Cert::X509CertReq::X509CertReq(Text::CString sourceName, const UInt8 *buff, UOSInt buffSize) : Crypto::Cert::X509File(sourceName, buff, buffSize)
{

}

Crypto::Cert::X509CertReq::~X509CertReq()
{

}

Crypto::Cert::X509File::FileType Crypto::Cert::X509CertReq::GetFileType()
{
	return FileType::CertRequest;
}

void Crypto::Cert::X509CertReq::ToShortName(Text::StringBuilderUTF8 *sb)
{
	UOSInt len = 0;
	Net::ASN1Util::ItemType itemType = Net::ASN1Util::IT_UNKNOWN;
	const UInt8 *tmpBuff = Net::ASN1Util::PDUGetItem(this->buff, this->buff + this->buffSize, "1.1.2", &len, &itemType);
	if (tmpBuff != 0 && itemType == Net::ASN1Util::IT_SEQUENCE)
	{
		NameGetCN(tmpBuff, tmpBuff + len, sb);
	}
}

Crypto::Cert::X509File::ValidStatus Crypto::Cert::X509CertReq::IsValid(Net::SSLEngine *ssl, Crypto::Cert::CertStore *trustStore)
{
	SignedInfo signedInfo;
	if (!this->GetSignedInfo(&signedInfo))
	{
		return Crypto::Cert::X509File::ValidStatus::FileFormatInvalid;		
	}
	Crypto::Hash::HashType hashType = GetAlgHash(signedInfo.algType);
	if (hashType == Crypto::Hash::HT_UNKNOWN)
	{
		return Crypto::Cert::X509File::ValidStatus::UnsupportedAlgorithm;
	}
	Crypto::Cert::X509Key *key = this->GetNewPublicKey();
	if (key == 0)
	{
		return Crypto::Cert::X509File::ValidStatus::FileFormatInvalid;
	}
	Bool valid = ssl->SignatureVerify(key, hashType, signedInfo.payload, signedInfo.payloadSize, signedInfo.signature, signedInfo.signSize);
	DEL_CLASS(key);
	if (valid)
	{
		return Crypto::Cert::X509File::ValidStatus::Valid;
	}
	else
	{
		return Crypto::Cert::X509File::ValidStatus::SignatureInvalid;
	}
}

Net::ASN1Data *Crypto::Cert::X509CertReq::Clone()
{
	Crypto::Cert::X509CertReq *asn1;
	NEW_CLASS(asn1, Crypto::Cert::X509CertReq(this->GetSourceNameObj(), this->buff, this->buffSize));
	return asn1;
}

void Crypto::Cert::X509CertReq::ToString(Text::StringBuilderUTF8 *sb)
{
	if (IsCertificateRequest(this->buff, this->buff + this->buffSize, "1"))
	{
		AppendCertificateRequest(this->buff, this->buff + this->buffSize, "1", sb);
	}
}

Bool Crypto::Cert::X509CertReq::GetNames(CertNames *names)
{
	UOSInt itemLen;
	Net::ASN1Util::ItemType itemType;
	const UInt8 *namesPDU = Net::ASN1Util::PDUGetItem(this->buff, this->buff + this->buffSize, "1.1.2", &itemLen, &itemType);
	if (namesPDU)
	{
		return NamesGet(namesPDU, namesPDU + itemLen, names);
	}
	return false;
}

Bool Crypto::Cert::X509CertReq::GetExtensions(CertExtensions *ext)
{
	UOSInt itemLen;
	Net::ASN1Util::ItemType itemType;
	const UInt8 *extPDU = Net::ASN1Util::PDUGetItem(this->buff, this->buff + this->buffSize, "1.1.4.1", &itemLen, &itemType);
	if (extPDU && itemType == Net::ASN1Util::IT_SEQUENCE)
	{
		UOSInt oidLen;
		const UInt8 *oid = Net::ASN1Util::PDUGetItem(extPDU, extPDU + itemLen, "1", &oidLen, &itemType);
		if (oid && Net::ASN1Util::OIDEqualsText(oid, oidLen, UTF8STRC("1.2.840.113549.1.9.14"))) //extensionRequest
		{
			UOSInt extSeqSize;
			const UInt8 *extSeq = Net::ASN1Util::PDUGetItem(extPDU, extPDU + itemLen, "2.1", &extSeqSize, &itemType);
			if (extSeq && itemType == Net::ASN1Util::IT_SEQUENCE)
			{
				return ExtensionsGet(extSeq, extSeq + extSeqSize, ext);
			}
		}
	}
	return false;
}

Crypto::Cert::X509Key *Crypto::Cert::X509CertReq::GetNewPublicKey()
{
	UOSInt itemLen;
	Net::ASN1Util::ItemType itemType;
	const UInt8 *keyPDU = Net::ASN1Util::PDUGetItem(this->buff, this->buff + this->buffSize, "1.1.3", &itemLen, &itemType);
	if (keyPDU)
	{
		return PublicKeyGetNew(keyPDU, keyPDU + itemLen);
	}
	return 0;
}

Bool Crypto::Cert::X509CertReq::GetKeyId(UInt8 *keyId)
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
