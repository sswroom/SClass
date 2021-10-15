#include "Stdafx.h"
#include "Crypto/Cert/CertUtil.h"

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
		UInt8 *keyBuff = MemAlloc(UInt8, pubKey->GetASN1BuffSize() + 1);
		keyBuff[0] = 0;
		MemCopyNO(&keyBuff[1], pubKey->GetASN1Buff(), pubKey->GetASN1BuffSize());
		builder->AppendBitString(keyBuff, pubKey->GetASN1BuffSize() + 1);
		MemFree(keyBuff);
		DEL_CLASS(pubKey);
		builder->EndLevel();
		return true;
	}
	return false;
}

Bool Crypto::Cert::CertUtil::AppendSign(Net::ASN1PDUBuilder *builder, Crypto::Cert::X509Key *key, Crypto::Hash::HashType hashType)
{
	/////////////////////////////
	return false;
}

Crypto::Cert::X509CertReq *Crypto::Cert::CertUtil::CertReqCreate(const CertNames *names, Crypto::Cert::X509Key *key)
{
	Net::ASN1PDUBuilder builder;
	builder.BeginSequence();

	builder.BeginSequence();
	builder.AppendInt32(0);
	if (!AppendNames(&builder, names)) return 0;
	if (!AppendPublicKey(&builder, key)) return 0;
	builder.BeginOther(0xA0);
	builder.EndLevel();
	builder.EndLevel();

	if (!AppendSign(&builder, key, Crypto::Hash::HT_SHA256)) return 0;
	builder.EndLevel();
	return 0;
}
