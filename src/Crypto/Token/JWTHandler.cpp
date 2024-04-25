#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/Hash/HMAC.h"
#include "Crypto/Hash/IHash.h"
#include "Crypto/Hash/SHA256.h"
#include "Crypto/Hash/SHA384.h"
#include "Crypto/Hash/SHA512.h"
#include "Crypto/Token/JWTHandler.h"
#include "Crypto/Token/JWToken.h"
#include "Text/JSON.h"
#include "Text/JSONBuilder.h"
#include "Text/MyString.h"
#include "Text/TextBinEnc/Base64Enc.h"

Crypto::Token::JWTHandler::JWTHandler(Optional<Net::SSLEngine> ssl, JWSignature::Algorithm alg, const UInt8 *key, UOSInt keyLeng, Crypto::Cert::X509Key::KeyType keyType)
{
	this->ssl = ssl;
	this->alg = alg;
	this->key = MemAlloc(UInt8, keyLeng);
	this->keyLeng = keyLeng;
	this->keyType = keyType;
	MemCopyNO(this->key, key, this->keyLeng);
}

Crypto::Token::JWTHandler::~JWTHandler()
{
	if (this->key)
		MemFree(this->key);
}

Bool Crypto::Token::JWTHandler::Generate(NN<Text::StringBuilderUTF8> sb, Data::StringMap<const UTF8Char*> *payload, JWTParam *param)
{
	NN<Data::ArrayList<Text::String*>> keys = payload->GetKeys();
	Text::String *key;
	UOSInt i;
	UOSInt j;
	NN<Text::String> s;
	Text::JSONBuilder json(Text::JSONBuilder::OT_OBJECT);
	i = 0;
	j = keys->GetCount();
	while (i < j)
	{
		key = keys->GetItem(i);
		json.ObjectAddStrUTF8(key->ToCString(), payload->Get(key));
		i++;
	}
	if (param != 0)
	{
		if (param->GetIssuer().SetTo(s))
		{
			json.ObjectAddStr(CSTR("iss"), s);
		}
		if (param->GetSubject().SetTo(s))
		{
			json.ObjectAddStr(CSTR("sub"), s);
		}
		if (param->GetAudience().SetTo(s))
		{
			json.ObjectAddStr(CSTR("aud"), s);
		}
		if (param->GetExpirationTime() != 0)
		{
			json.ObjectAddInt64(CSTR("exp"), param->GetExpirationTime());
		}
		if (param->GetNotBefore() != 0)
		{
			json.ObjectAddInt64(CSTR("nbf"), param->GetNotBefore());
		}
		if (param->GetIssuedAt() != 0)
		{
			json.ObjectAddInt64(CSTR("iat"), param->GetIssuedAt());
		}
		if (param->GetJWTId().SetTo(s))
		{
			json.ObjectAddStr(CSTR("jti"), s);
		}
	}
	Crypto::Token::JWToken *token = Crypto::Token::JWToken::Generate(alg, json.Build(), this->ssl, this->key, this->keyLeng, this->keyType);
	if (token == 0)
	{
		return false;
	}
	token->ToString(sb);
	DEL_CLASS(token);
	return true;
}

