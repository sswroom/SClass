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

Crypto::Token::JWTHandler::JWTHandler(Net::SSLEngine *ssl, JWSignature::Algorithm alg, const UInt8 *key, UOSInt keyLeng, Crypto::Cert::X509Key::KeyType keyType)
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

Bool Crypto::Token::JWTHandler::Generate(NotNullPtr<Text::StringBuilderUTF8> sb, Data::StringMap<const UTF8Char*> *payload, JWTParam *param)
{
	Text::StringBuilderUTF8 sbJson;
	NotNullPtr<Data::ArrayList<Text::String*>> keys = payload->GetKeys();
	Text::String *key;
	UOSInt i;
	UOSInt j;
	{
		Text::JSONBuilder json(sbJson, Text::JSONBuilder::OT_OBJECT);
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
			if (param->GetIssuer() != 0)
			{
				json.ObjectAddStr(CSTR("iss"), param->GetIssuer());
			}
			if (param->GetSubject() != 0)
			{
				json.ObjectAddStr(CSTR("sub"), param->GetSubject());
			}
			if (param->GetAudience() != 0)
			{
				json.ObjectAddStr(CSTR("aud"), param->GetAudience());
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
			if (param->GetJWTId() != 0)
			{
				json.ObjectAddStr(CSTR("jti"), param->GetJWTId());
			}
		}
	}
	Crypto::Token::JWToken *token = Crypto::Token::JWToken::Generate(alg, sbJson.ToCString(), this->ssl, this->key, this->keyLeng, this->keyType);
	if (token == 0)
	{
		return false;
	}
	token->ToString(sb);
	DEL_CLASS(token);
	return true;
}

