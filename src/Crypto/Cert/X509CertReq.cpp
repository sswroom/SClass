#include "Stdafx.h"
#include "Crypto/Cert/X509CertReq.h"
#include "Crypto/Cert/X509Key.h"
#include "Net/ASN1Util.h"
#include "Net/SSLEngine.h"

Crypto::Cert::X509CertReq::X509CertReq(NotNullPtr<Text::String> sourceName, Data::ByteArrayR buff) : Crypto::Cert::X509File(sourceName, buff)
{

}

Crypto::Cert::X509CertReq::X509CertReq(Text::CString sourceName, Data::ByteArrayR buff) : Crypto::Cert::X509File(sourceName, buff)
{

}

Crypto::Cert::X509CertReq::~X509CertReq()
{

}

Crypto::Cert::X509File::FileType Crypto::Cert::X509CertReq::GetFileType() const
{
	return FileType::CertRequest;
}

void Crypto::Cert::X509CertReq::ToShortName(NotNullPtr<Text::StringBuilderUTF8> sb) const
{
	UOSInt len = 0;
	Net::ASN1Util::ItemType itemType = Net::ASN1Util::IT_UNKNOWN;
	const UInt8 *tmpBuff = Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.1.2", &len, &itemType);
	if (tmpBuff != 0 && itemType == Net::ASN1Util::IT_SEQUENCE)
	{
		NameGetCN(tmpBuff, tmpBuff + len, sb);
	}
}

Crypto::Cert::X509File::ValidStatus Crypto::Cert::X509CertReq::IsValid(Net::SSLEngine *ssl, Crypto::Cert::CertStore *trustStore) const
{
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
	NotNullPtr<Crypto::Cert::X509Key> key;;
	if (!key.Set(this->GetNewPublicKey()))
	{
		return Crypto::Cert::X509File::ValidStatus::FileFormatInvalid;
	}
	Bool valid = ssl->SignatureVerify(key, hashType, signedInfo.payload, signedInfo.payloadSize, signedInfo.signature, signedInfo.signSize);
	key.Delete();
	if (valid)
	{
		return Crypto::Cert::X509File::ValidStatus::Valid;
	}
	else
	{
		return Crypto::Cert::X509File::ValidStatus::SignatureInvalid;
	}
}

NotNullPtr<Net::ASN1Data> Crypto::Cert::X509CertReq::Clone() const
{
	NotNullPtr<Crypto::Cert::X509CertReq> asn1;
	NEW_CLASSNN(asn1, Crypto::Cert::X509CertReq(this->GetSourceNameObj(), this->buff));
	return asn1;
}

void Crypto::Cert::X509CertReq::ToString(NotNullPtr<Text::StringBuilderUTF8> sb) const
{
	if (IsCertificateRequest(this->buff.Ptr(), this->buff.PtrEnd(), "1"))
	{
		AppendCertificateRequest(this->buff.Ptr(), this->buff.PtrEnd(), "1", sb);
	}
}

Bool Crypto::Cert::X509CertReq::GetNames(CertNames *names) const
{
	UOSInt itemLen;
	Net::ASN1Util::ItemType itemType;
	const UInt8 *namesPDU = Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.1.2", &itemLen, &itemType);
	if (namesPDU)
	{
		return NamesGet(namesPDU, namesPDU + itemLen, names);
	}
	return false;
}

Bool Crypto::Cert::X509CertReq::GetExtensions(CertExtensions *ext) const
{
	UOSInt itemLen;
	Net::ASN1Util::ItemType itemType;
	const UInt8 *extPDU = Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.1.4.1", &itemLen, &itemType);
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

Crypto::Cert::X509Key *Crypto::Cert::X509CertReq::GetNewPublicKey() const
{
	UOSInt itemLen;
	Net::ASN1Util::ItemType itemType;
	const UInt8 *keyPDU = Net::ASN1Util::PDUGetItem(this->buff.Ptr(), this->buff.PtrEnd(), "1.1.3", &itemLen, &itemType);
	if (keyPDU)
	{
		return PublicKeyGetNew(keyPDU, keyPDU + itemLen);
	}
	return 0;
}

Bool Crypto::Cert::X509CertReq::GetKeyId(const Data::ByteArray &keyId) const
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
