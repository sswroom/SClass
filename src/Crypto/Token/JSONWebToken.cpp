#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/Hash/HMAC.h"
#include "Crypto/Hash/IHash.h"
#include "Crypto/Hash/SHA256.h"
#include "Crypto/Hash/SHA384.h"
#include "Crypto/Hash/SHA512.h"
#include "Crypto/Token/JSONWebToken.h"
#include "Text/MyString.h"
#include "Text/TextBinEnc/Base64Enc.h"

Bool Crypto::Token::JSONWebToken::Generate(Text::StringBuilderUTF8 *sb, Algorithm alg, const UTF8Char *payload, const UInt8 *key, UOSInt keySize)
{
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	sptr = Text::StrConcat(sbuff, (const UTF8Char*)"{\"alg\":\"");
	sptr = Text::StrConcat(sptr, GetAlgorithmName(alg));
	sptr = Text::StrConcat(sptr, (const UTF8Char*)"\",\"typ\":\"JWT\"}");
	Text::TextBinEnc::Base64Enc b64(Text::TextBinEnc::Base64Enc::CS_URL, true);
	b64.EncodeBin(sb, sbuff, (UOSInt)(sptr - sbuff));
	sb->AppendChar('.', 1);
	b64.EncodeBin(sb, payload, Text::StrCharCnt(payload));
	Crypto::Hash::IHash *hash;
	Crypto::Hash::IHash *ihash;
	switch (alg)
	{
	case HS256:
		NEW_CLASS(ihash, Crypto::Hash::SHA256());
		NEW_CLASS(hash, Crypto::Hash::HMAC(ihash, key, keySize));
		DEL_CLASS(ihash);
		break;
	case HS384:
		NEW_CLASS(ihash, Crypto::Hash::SHA384());
		NEW_CLASS(hash, Crypto::Hash::HMAC(ihash, key, keySize));
		DEL_CLASS(ihash);
		break;
	case HS512:
		NEW_CLASS(ihash, Crypto::Hash::SHA512());
		NEW_CLASS(hash, Crypto::Hash::HMAC(ihash, key, keySize));
		DEL_CLASS(ihash);
		break;
	default:
		return false;
	}

	hash->Calc(sb->ToString(), sb->GetLength());
	hash->GetValue(sbuff);
	sb->AppendChar('.', 1);
	b64.EncodeBin(sb, sbuff, hash->GetResultSize());
	DEL_CLASS(hash);
	return true;
}

const UTF8Char *Crypto::Token::JSONWebToken::GetAlgorithmName(Algorithm alg)
{
	switch (alg)
	{
	case HS256:
		return (const UTF8Char*)"HS256";
	case HS384:
		return (const UTF8Char*)"HS384";
	case HS512:
		return (const UTF8Char*)"HS512";
	case PS256:
		return (const UTF8Char*)"PS256";
	case PS384:
		return (const UTF8Char*)"PS384";
	case PS512:
		return (const UTF8Char*)"PS512";
	case RS256:
		return (const UTF8Char*)"RS256";
	case RS384:
		return (const UTF8Char*)"RS384";
	case RS512:
		return (const UTF8Char*)"RS512";
	case ES256:
		return (const UTF8Char*)"ES256";
	case ES256K:
		return (const UTF8Char*)"ES256K";
	case ES384:
		return (const UTF8Char*)"ES384";
	case ES512:
		return (const UTF8Char*)"ES512";
	case EDDSA:
		return (const UTF8Char*)"EdDSA";
	default:
		return (const UTF8Char*)"UNK";
	}
}
