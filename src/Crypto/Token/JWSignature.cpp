#include "Stdafx.h"
#include "Crypto/Cert/X509Key.h"
#include "Crypto/Hash/HMAC.h"
#include "Crypto/Hash/SHA256.h"
#include "Crypto/Hash/SHA384.h"
#include "Crypto/Hash/SHA512.h"
#include "Crypto/Token/JWSignature.h"
#include "Text/EnumFinder.h"
#include "Text/TextBinEnc/Base64Enc.h"

Crypto::Token::JWSignature::JWSignature(Net::SSLEngine *ssl, Algorithm alg, const UInt8 *privateKey, UOSInt privateKeyLeng)
{
	this->ssl = ssl;
	this->alg = alg;
	this->hashValSize = 0;
	this->privateKey = MemAlloc(UInt8, privateKeyLeng);
	MemCopyNO(this->privateKey, privateKey, privateKeyLeng);
	this->privateKeyLeng = privateKeyLeng;
}

Crypto::Token::JWSignature::~JWSignature()
{
	MemFree(this->privateKey);
}

Bool Crypto::Token::JWSignature::CalcHash(const UInt8 *buff, UOSInt buffSize)
{
	Crypto::Hash::IHash *hash;
	Crypto::Hash::IHash *ihash;
	switch (alg)
	{
	case Algorithm::HS256:
		NEW_CLASS(ihash, Crypto::Hash::SHA256());
		NEW_CLASS(hash, Crypto::Hash::HMAC(ihash, this->privateKey, this->privateKeyLeng));
		DEL_CLASS(ihash);
		break;
	case Algorithm::HS384:
		NEW_CLASS(ihash, Crypto::Hash::SHA384());
		NEW_CLASS(hash, Crypto::Hash::HMAC(ihash, this->privateKey, this->privateKeyLeng));
		DEL_CLASS(ihash);
		break;
	case Algorithm::HS512:
		NEW_CLASS(ihash, Crypto::Hash::SHA512());
		NEW_CLASS(hash, Crypto::Hash::HMAC(ihash, this->privateKey, this->privateKeyLeng));
		DEL_CLASS(ihash);
		break;
	case Algorithm::RS256:
	case Algorithm::RS384:
	case Algorithm::RS512:
		{
			if (this->ssl == 0)
			{
				return false;
			}
			Crypto::Cert::X509Key *key;
			Bool succ = false;
			NEW_CLASS(key, Crypto::Cert::X509Key((const UTF8Char*)"rsakey", this->privateKey, this->privateKeyLeng, Crypto::Cert::X509Key::KeyType::RSA));
			if (alg == Algorithm::RS256)
				succ = this->ssl->Signature(key, Crypto::Hash::HT_SHA256, buff, buffSize, this->hashVal, &this->hashValSize);
			else if (alg == Algorithm::RS384)
				succ = this->ssl->Signature(key, Crypto::Hash::HT_SHA384, buff, buffSize, this->hashVal, &this->hashValSize);
			if (alg == Algorithm::RS512)
				succ = this->ssl->Signature(key, Crypto::Hash::HT_SHA512, buff, buffSize, this->hashVal, &this->hashValSize);
			DEL_CLASS(key);
			return succ;
		}
		break;
	case Algorithm::PS256:
	case Algorithm::PS384:
	case Algorithm::PS512:
	case Algorithm::ES256:
	case Algorithm::ES256K:
	case Algorithm::ES384:
	case Algorithm::ES512:
	case Algorithm::EDDSA:
	case Algorithm::Unknown:
	default:
		return false;
	}

	hash->Calc(buff, buffSize);
	hash->GetValue(this->hashVal);
	this->hashValSize = hash->GetResultSize();
	DEL_CLASS(hash);
	return true;
}

Bool Crypto::Token::JWSignature::GetHashB64(Text::StringBuilderUTF8 *sb)
{
	if (this->hashValSize == 0)
	{
		return false;
	}
	Text::TextBinEnc::Base64Enc b64(Text::TextBinEnc::Base64Enc::Charset::URL, true);
	b64.EncodeBin(sb, this->hashVal, this->hashValSize);
	return true;
}

Text::CString Crypto::Token::JWSignature::AlgorithmGetName(Algorithm alg)
{
	switch (alg)
	{
	case Algorithm::HS256:
		return {UTF8STRC("HS256")};
	case Algorithm::HS384:
		return {UTF8STRC("HS384")};
	case Algorithm::HS512:
		return {UTF8STRC("HS512")};
	case Algorithm::PS256:
		return {UTF8STRC("PS256")};
	case Algorithm::PS384:
		return {UTF8STRC("PS384")};
	case Algorithm::PS512:
		return {UTF8STRC("PS512")};
	case Algorithm::RS256:
		return {UTF8STRC("RS256")};
	case Algorithm::RS384:
		return {UTF8STRC("RS384")};
	case Algorithm::RS512:
		return {UTF8STRC("RS512")};
	case Algorithm::ES256:
		return {UTF8STRC("ES256")};
	case Algorithm::ES256K:
		return {UTF8STRC("ES256K")};
	case Algorithm::ES384:
		return {UTF8STRC("ES384")};
	case Algorithm::ES512:
		return {UTF8STRC("ES512")};
	case Algorithm::EDDSA:
		return {UTF8STRC("EdDSA")};
	case Algorithm::Unknown:
	default:
		return {UTF8STRC("UNK")};
	}
}

Crypto::Token::JWSignature::Algorithm Crypto::Token::JWSignature::AlgorithmGetByName(const UTF8Char *name)
{
	Text::EnumFinder<Algorithm> finder((const Char*)name, Algorithm::Unknown);
	finder.Entry("HS256", Algorithm::HS256);
	finder.Entry("HS384", Algorithm::HS384);
	finder.Entry("HS512", Algorithm::HS512);
	finder.Entry("PS256", Algorithm::PS256);
	finder.Entry("PS384", Algorithm::PS384);
	finder.Entry("PS512", Algorithm::PS512);
	finder.Entry("RS256", Algorithm::RS256);
	finder.Entry("RS384", Algorithm::RS384);
	finder.Entry("RS512", Algorithm::RS512);
	finder.Entry("ES256", Algorithm::ES256);
	finder.Entry("ES256K", Algorithm::ES256K);
	finder.Entry("ES384", Algorithm::ES384);
	finder.Entry("ES512", Algorithm::ES512);
	finder.Entry("EdDSA", Algorithm::EDDSA);
	return finder.GetResult();
}
