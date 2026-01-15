#include "Stdafx.h"
#include "Crypto/Cert/X509CertReq.h"
#include "Crypto/Cert/X509Key.h"
#include "Net/ASN1Util.h"
#include "Net/SSLEngine.h"

Crypto::Cert::X509CertReq::X509CertReq(NN<Text::String> sourceName, Data::ByteArrayR buff) : Crypto::Cert::X509File(sourceName, buff)
{

}

Crypto::Cert::X509CertReq::X509CertReq(Text::CStringNN sourceName, Data::ByteArrayR buff) : Crypto::Cert::X509File(sourceName, buff)
{

}

Crypto::Cert::X509CertReq::~X509CertReq()
{

}

Crypto::Cert::X509File::FileType Crypto::Cert::X509CertReq::GetFileType() const
{
	return FileType::CertRequest;
}

void Crypto::Cert::X509CertReq::ToShortName(NN<Text::StringBuilderUTF8> sb) const
{
	UIntOS len = 0;
	Net::ASN1Util::ItemType itemType = Net::ASN1Util::IT_UNKNOWN;
	UnsafeArray<const UInt8> tmpBuff;
	if (Net::ASN1Util::PDUGetItem(this->buff.Arr(), this->buff.ArrEnd(), "1.1.2", len, itemType).SetTo(tmpBuff) && itemType == Net::ASN1Util::IT_SEQUENCE)
	{
		NameGetCN(tmpBuff, tmpBuff + len, sb);
	}
}

Crypto::Cert::X509File::ValidStatus Crypto::Cert::X509CertReq::IsValid(NN<Net::SSLEngine> ssl, Optional<Crypto::Cert::CertStore> trustStore) const
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
	NN<Crypto::Cert::X509Key> key;;
	if (!this->GetNewPublicKey().SetTo(key))
	{
		return Crypto::Cert::X509File::ValidStatus::FileFormatInvalid;
	}
	Bool valid = ssl->SignatureVerify(key, hashType, Data::ByteArrayR(signedInfo.payload, signedInfo.payloadSize), Data::ByteArrayR(signedInfo.signature, signedInfo.signSize));
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

NN<Net::ASN1Data> Crypto::Cert::X509CertReq::Clone() const
{
	NN<Crypto::Cert::X509CertReq> asn1;
	NEW_CLASSNN(asn1, Crypto::Cert::X509CertReq(this->GetSourceNameObj(), this->buff));
	return asn1;
}

void Crypto::Cert::X509CertReq::ToString(NN<Text::StringBuilderUTF8> sb) const
{
	if (IsCertificateRequest(this->buff.Arr(), this->buff.ArrEnd(), "1"))
	{
		AppendCertificateRequest(this->buff.Arr(), this->buff.ArrEnd(), "1", sb);
	}
}

NN<Net::ASN1Names> Crypto::Cert::X509CertReq::CreateNames() const
{
	NN<Net::ASN1Names> names;
	NEW_CLASSNN(names, Net::ASN1Names());
	return names->SetCertificationRequest();
}

Bool Crypto::Cert::X509CertReq::GetNames(NN<CertNames> names) const
{
	UIntOS itemLen;
	Net::ASN1Util::ItemType itemType;
	UnsafeArray<const UInt8> namesPDU;
	if (Net::ASN1Util::PDUGetItem(this->buff.Arr(), this->buff.ArrEnd(), "1.1.2", itemLen, itemType).SetTo(namesPDU))
	{
		return NamesGet(namesPDU, namesPDU + itemLen, names);
	}
	return false;
}

Bool Crypto::Cert::X509CertReq::GetExtensions(NN<CertExtensions> ext) const
{
	UIntOS itemLen;
	Net::ASN1Util::ItemType itemType;
	UnsafeArray<const UInt8> extPDU;
	if (Net::ASN1Util::PDUGetItem(this->buff.Arr(), this->buff.ArrEnd(), "1.1.4.1", itemLen, itemType).SetTo(extPDU) && itemType == Net::ASN1Util::IT_SEQUENCE)
	{
		UIntOS oidLen;
		UnsafeArray<const UInt8> oid;
		if (Net::ASN1Util::PDUGetItem(extPDU, extPDU + itemLen, "1", oidLen, itemType).SetTo(oid) && Net::ASN1Util::OIDEqualsText(Data::ByteArrayR(oid, oidLen), CSTR("1.2.840.113549.1.9.14"))) //extensionRequest
		{
			UIntOS extSeqSize;
			UnsafeArray<const UInt8> extSeq;
			if (Net::ASN1Util::PDUGetItem(extPDU, extPDU + itemLen, "2.1", extSeqSize, itemType).SetTo(extSeq) && itemType == Net::ASN1Util::IT_SEQUENCE)
			{
				return ExtensionsGet(extSeq, extSeq + extSeqSize, ext);
			}
		}
	}
	return false;
}

Optional<Crypto::Cert::X509Key> Crypto::Cert::X509CertReq::GetNewPublicKey() const
{
	UIntOS itemLen;
	Net::ASN1Util::ItemType itemType;
	UnsafeArray<const UInt8> keyPDU;
	if (Net::ASN1Util::PDUGetItem(this->buff.Arr(), this->buff.ArrEnd(), "1.1.3", itemLen, itemType).SetTo(keyPDU))
	{
		return PublicKeyGetNew(keyPDU, keyPDU + itemLen);
	}
	return nullptr;
}

Bool Crypto::Cert::X509CertReq::GetKeyId(const Data::ByteArray &keyId) const
{
	NN<Crypto::Cert::X509Key> key;
	if (!GetNewPublicKey().SetTo(key))
	{
		return false;
	}
	if (key->GetKeyId(keyId))
	{
		key.Delete();
		return true;
	}
	key.Delete();
	return false;
}
