#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/Hash/HMAC.h"
#include "Crypto/Hash/HashAlgorithm.h"
#include "Crypto/Hash/SHA256.h"
#include "Crypto/Hash/SHA384.h"
#include "Crypto/Hash/SHA512.h"
#include "Crypto/Token/JWTHandler.h"
#include "Crypto/Token/JWToken.h"
#include "Text/JSON.h"
#include "Text/JSONBuilder.h"
#include "Text/MyString.h"
#include "Text/TextBinEnc/Base64Enc.h"

Crypto::Token::JWTHandler::JWTHandler(Optional<Net::SSLEngine> ssl, JWSignature::Algorithm alg, UnsafeArray<const UInt8> key, UIntOS keyLeng, Crypto::Cert::X509Key::KeyType keyType)
{
	this->ssl = ssl;
	this->alg = alg;
	this->key = MemAllocArr(UInt8, keyLeng);
	this->keyLeng = keyLeng;
	this->keyType = keyType;
	MemCopyNO(&this->key[0], key.Ptr(), this->keyLeng);
}

Crypto::Token::JWTHandler::~JWTHandler()
{
	MemFreeArr(this->key);
}

Bool Crypto::Token::JWTHandler::Generate(NN<Text::StringBuilderUTF8> sb, NN<Data::StringMapObj<const UTF8Char*>> payload, Optional<JWTParam> param)
{
	NN<Data::ArrayListObj<Optional<Text::String>>> keys = payload->GetKeys();
	NN<Text::String> key;
	UIntOS i;
	UIntOS j;
	NN<JWTParam> nnparam;
	NN<Text::String> s;
	Text::JSONBuilder json(Text::JSONBuilder::OT_OBJECT);
	i = 0;
	j = keys->GetCount();
	while (i < j)
	{
		key = Text::String::OrEmpty(keys->GetItem(i));
		json.ObjectAddStrUTF8(key->ToCString(), payload->Get(key));
		i++;
	}
	if (param.SetTo(nnparam))
	{
		if (nnparam->GetIssuer().SetTo(s))
		{
			json.ObjectAddStr(CSTR("iss"), s);
		}
		if (nnparam->GetSubject().SetTo(s))
		{
			json.ObjectAddStr(CSTR("sub"), s);
		}
		if (nnparam->GetAudience().SetTo(s))
		{
			json.ObjectAddStr(CSTR("aud"), s);
		}
		if (nnparam->GetExpirationTime() != 0)
		{
			json.ObjectAddInt64(CSTR("exp"), nnparam->GetExpirationTime());
		}
		if (nnparam->GetNotBefore() != 0)
		{
			json.ObjectAddInt64(CSTR("nbf"), nnparam->GetNotBefore());
		}
		if (nnparam->GetIssuedAt() != 0)
		{
			json.ObjectAddInt64(CSTR("iat"), nnparam->GetIssuedAt());
		}
		if (nnparam->GetJWTId().SetTo(s))
		{
			json.ObjectAddStr(CSTR("jti"), s);
		}
	}
	NN<Crypto::Token::JWToken> token;
	if (!Crypto::Token::JWToken::Generate(alg, json.Build(), this->ssl, this->key, this->keyLeng, this->keyType).SetTo(token))
	{
		return false;
	}
	token->ToString(sb);
	token.Delete();
	return true;
}

