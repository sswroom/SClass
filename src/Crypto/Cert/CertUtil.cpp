#include "Stdafx.h"
#include "Crypto/Cert/CertUtil.h"
#include "Net/ASN1Util.h"

Bool Crypto::Cert::CertUtil::AppendNames(Net::ASN1PDUBuilder *builder, const CertNames *names)
{
	Bool found = false;
	builder->BeginSequence();
	if (names->countryName)
	{
		builder->BeginSet();
		builder->BeginSequence();
		builder->AppendOIDString("2.5.4.6");
		builder->AppendPrintableString(names->countryName);
		builder->EndLevel();
		builder->EndLevel();
		found = true;
	}
	if (names->stateOrProvinceName)
	{
		builder->BeginSet();
		builder->BeginSequence();
		builder->AppendOIDString("2.5.4.8");
		builder->AppendUTF8String(names->stateOrProvinceName);
		builder->EndLevel();
		builder->EndLevel();
		found = true;
	}
	if (names->localityName)
	{
		builder->BeginSet();
		builder->BeginSequence();
		builder->AppendOIDString("2.5.4.7");
		builder->AppendUTF8String(names->localityName);
		builder->EndLevel();
		builder->EndLevel();
		found = true;
	}
	if (names->organizationName)
	{
		builder->BeginSet();
		builder->BeginSequence();
		builder->AppendOIDString("2.5.4.10");
		builder->AppendUTF8String(names->organizationName);
		builder->EndLevel();
		builder->EndLevel();
		found = true;
	}
	if (names->organizationUnitName)
	{
		builder->BeginSet();
		builder->BeginSequence();
		builder->AppendOIDString("2.5.4.11");
		builder->AppendUTF8String(names->organizationUnitName);
		builder->EndLevel();
		builder->EndLevel();
		found = true;
	}
	if (names->commonName)
	{
		builder->BeginSet();
		builder->BeginSequence();
		builder->AppendOIDString("2.5.4.3");
		builder->AppendUTF8String(names->commonName);
		builder->EndLevel();
		builder->EndLevel();
		found = true;
	}
	if (names->emailAddress)
	{
		builder->BeginSet();
		builder->BeginSequence();
		builder->AppendOIDString("1.2.840.113549.1.9.1");
		builder->AppendIA5String(names->emailAddress);
		builder->EndLevel();
		builder->EndLevel();
		found = true;
	}
	builder->EndLevel();
	return found;
}

Bool Crypto::Cert::CertUtil::AppendPublicKey(Net::ASN1PDUBuilder *builder, Crypto::Cert::X509Key *key)
{
	if (key->GetKeyType() == Crypto::Cert::X509Key::KeyType::RSA)
	{
		builder->BeginSequence();
		builder->BeginSequence();
		builder->AppendOIDString("1.2.840.113549.1.1.1");
		builder->AppendNull();
		builder->EndLevel();
		Crypto::Cert::X509Key *pubKey = key->CreatePublicKey();
		if (pubKey == 0)
		{
			return false;
		}
		builder->AppendBitStringWith0(pubKey->GetASN1Buff(), pubKey->GetASN1BuffSize());
		builder->EndLevel();
		DEL_CLASS(pubKey);
		return true;
	}
	return false;
}

Bool Crypto::Cert::CertUtil::AppendReqExtensions(Net::ASN1PDUBuilder *builder, const ReqExtensions *ext)
{
	builder->BeginSequence();
	if (ext->subjectAltName)
	{
		builder->BeginSequence();
		builder->AppendOIDString("2.5.29.17");
		builder->BeginOther(Net::ASN1Util::IT_OCTET_STRING);
		builder->BeginSequence();
		UOSInt i = 0;
		UOSInt j = ext->subjectAltName->GetCount();
		const UTF8Char *csptr;
		Net::SocketUtil::AddressInfo addr;
		while (i < j)
		{
			csptr = ext->subjectAltName->GetItem(i);
			if (Net::SocketUtil::GetIPAddr(csptr, &addr))
			{
				if (addr.addrType == Net::AddrType::IPv4)
				{
					builder->AppendOther(0x87, addr.addr, 4);
				}
			}
			else
			{
				builder->AppendOther(0x82, csptr, Text::StrCharCnt(csptr));
			}
			i++;
		}
		builder->EndLevel();
		builder->EndLevel();
		builder->EndLevel();
	}
	builder->EndLevel();
	return true;
}

Bool Crypto::Cert::CertUtil::AppendSign(Net::ASN1PDUBuilder *builder, Net::SSLEngine *ssl, Crypto::Cert::X509Key *key, Crypto::Hash::HashType hashType)
{
	UOSInt itemLen;
	UOSInt itemOfst;
	const UInt8 *item = builder->GetItemRAW("1", &itemLen, &itemOfst);
	if (item == 0)
	{
		return 0;
	}
	if (key->GetKeyType() == Crypto::Cert::X509Key::KeyType::RSA && hashType == Crypto::Hash::HT_SHA256)
	{
		UInt8 signData[256];
		UOSInt signLen;
		if (!ssl->Signature(key, hashType, item, itemOfst + itemLen, signData, &signLen))
		{
			return false;
		}
		builder->BeginSequence();
		builder->AppendOIDString("1.2.840.113549.1.1.11");
		builder->AppendNull();
		builder->EndLevel();
		builder->AppendBitStringWith0(signData, signLen);
		return true;
	}
	else
	{
		return false;
	}
}

Crypto::Cert::X509CertReq *Crypto::Cert::CertUtil::CertReqCreate(Net::SSLEngine *ssl, const CertNames *names, Crypto::Cert::X509Key *key, const ReqExtensions *ext)
{
	Net::ASN1PDUBuilder builder;
	builder.BeginSequence();

	builder.BeginSequence();
	builder.AppendInt32(0);
	if (!AppendNames(&builder, names)) return 0;
	if (!AppendPublicKey(&builder, key)) return 0;
	builder.BeginOther(0xA0);
	if (ext)
	{
		builder.BeginSequence();
		builder.AppendOIDString("1.2.840.113549.1.9.14");
		builder.BeginSet();
		AppendReqExtensions(&builder, ext);
		builder.EndLevel();
		builder.EndLevel();
	}
	builder.EndLevel();
	builder.EndLevel();

	if (!AppendSign(&builder, ssl, key, Crypto::Hash::HT_SHA256)) return 0;
	builder.EndLevel();
	Crypto::Cert::X509CertReq *csr;
	NEW_CLASS(csr, Crypto::Cert::X509CertReq((const UTF8Char*)"CertReq", builder.GetBuff(0), builder.GetBuffSize()));
	return csr;
}
