#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/Hash/HMAC.h"
#include "Crypto/Hash/IHash.h"
#include "Crypto/Hash/SHA256.h"
#include "Crypto/Hash/SHA384.h"
#include "Crypto/Hash/SHA512.h"
#include "Crypto/Token/JWTHandler.h"
#include "Text/JSON.h"
#include "Text/JSONBuilder.h"
#include "Text/MyString.h"
#include "Text/TextBinEnc/Base64Enc.h"

Crypto::Token::JWTHandler::JWTHandler(Net::SSLEngine *ssl, JWSignature::Algorithm alg, const UInt8 *privateKey, UOSInt privateKeyLeng)
{
	this->ssl = ssl;
	this->alg = alg;
	this->privateKey = MemAlloc(UInt8, privateKeyLeng);
	this->privateKeyLeng = privateKeyLeng;
	MemCopyNO(this->privateKey, privateKey, this->privateKeyLeng);
}

Crypto::Token::JWTHandler::~JWTHandler()
{
	MemFree(this->privateKey);
}

Bool Crypto::Token::JWTHandler::Generate(Text::StringBuilderUTF8 *sb, Data::StringUTF8Map<const UTF8Char*> *payload, JWTParam *param)
{
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	sptr = Text::StrConcat(sbuff, (const UTF8Char*)"{\"alg\":\"");
	sptr = Text::StrConcat(sptr, JWSignature::AlgorithmGetName(alg));
	sptr = Text::StrConcat(sptr, (const UTF8Char*)"\",\"typ\":\"JWT\"}");
	Text::TextBinEnc::Base64Enc b64(Text::TextBinEnc::Base64Enc::CS_URL, true);
	b64.EncodeBin(sb, sbuff, (UOSInt)(sptr - sbuff));
	sb->AppendChar('.', 1);
	Text::StringBuilderUTF8 sbJson;
	Text::JSONBuilder *json;
	Data::ArrayList<const UTF8Char*> *keys = payload->GetKeys();
	const UTF8Char *key;
	UOSInt i;
	UOSInt j;
	NEW_CLASS(json, Text::JSONBuilder(&sbJson, Text::JSONBuilder::OT_OBJECT));
	i = 0;
	j = keys->GetCount();
	while (i < j)
	{
		key = keys->GetItem(i);
		json->ObjectAddStrUTF8(key, payload->Get(key));
		i++;
	}
	if (param != 0)
	{
		if (param->GetIssuer() != 0)
		{
			json->ObjectAddStrUTF8((const UTF8Char*)"iss", param->GetIssuer());
		}
		if (param->GetSubject() != 0)
		{
			json->ObjectAddStrUTF8((const UTF8Char*)"sub", param->GetSubject());
		}
		if (param->GetAudience() != 0)
		{
			json->ObjectAddStrUTF8((const UTF8Char*)"aud", param->GetAudience());
		}
		if (param->GetExpirationTime() != 0)
		{
			json->ObjectAddInt64((const UTF8Char*)"exp", param->GetExpirationTime());
		}
		if (param->GetNotBefore() != 0)
		{
			json->ObjectAddInt64((const UTF8Char*)"nbf", param->GetNotBefore());
		}
		if (param->GetIssuedAt() != 0)
		{
			json->ObjectAddInt64((const UTF8Char*)"iat", param->GetIssuedAt());
		}
		if (param->GetJWTId() != 0)
		{
			json->ObjectAddStrUTF8((const UTF8Char*)"jti", param->GetJWTId());
		}
	}
	DEL_CLASS(json);
	b64.EncodeBin(sb, sbJson.ToString(), sbJson.GetLength());
	Crypto::Token::JWSignature sign(this->ssl, this->alg, this->privateKey, this->privateKeyLeng);
	if (!sign.CalcHash(sb->ToString(), sb->GetLength()))
	{
		return false;
	}
	sb->AppendChar('.', 1);
	sign.GetHashB64(sb);
	return true;
}

Data::StringUTF8Map<const UTF8Char*> *Crypto::Token::JWTHandler::Parse(const UTF8Char *token, JWTParam *param)
{
	UOSInt i1 = Text::StrIndexOf(token, '.');;
	UOSInt i2 = Text::StrIndexOf(&token[i1 + 1], '.');
	if (i1 != INVALID_INDEX && i2 != INVALID_INDEX)
	{
		i2 += i1 + 1;
	}
	UOSInt i3 = Text::StrIndexOf(&token[i2 + 1], '.');
	if (i2 == INVALID_INDEX || i3 != INVALID_INDEX)
	{
		return 0;
	}
	Text::TextBinEnc::Base64Enc b64url(Text::TextBinEnc::Base64Enc::CS_URL, true);
	UInt8 *headerBuff = MemAlloc(UInt8, i1);
	UOSInt headerSize;
	UInt8 *payloadBuff = MemAlloc(UInt8, (i2 - i1));
	UOSInt payloadSize;
	headerSize = b64url.DecodeBin(token, i1, headerBuff);
	payloadSize = b64url.DecodeBin(&token[i1 + 1], i2 - i1 - 1, payloadBuff);
	Text::JSONBase *hdrJson = Text::JSONBase::ParseJSONStrLen(headerBuff, headerSize);
	JWSignature::Algorithm tokenAlg = JWSignature::Algorithm::Unknown;
	if (hdrJson != 0)
	{
		if (hdrJson->GetJSType() == Text::JSONBase::JST_OBJECT)
		{
			Text::JSONBase *algJson = ((Text::JSONObject*)hdrJson)->GetObjectValue((const UTF8Char*)"alg");
			if (algJson != 0 && algJson->GetJSType() == Text::JSONBase::JST_STRINGUTF8)
			{
				tokenAlg = JWSignature::AlgorithmGetByName(((Text::JSONStringUTF8*)algJson)->GetValue());
			}

		}
		hdrJson->EndUse();
	}
	if (this->alg != tokenAlg)
	{
		MemFree(headerBuff);
		MemFree(payloadBuff);
		return 0;
	}
	Crypto::Token::JWSignature sign(this->ssl, this->alg, this->privateKey, this->privateKeyLeng);
	if (!sign.CalcHash(token, (UOSInt)i2))
	{
		MemFree(headerBuff);
		MemFree(payloadBuff);
		return 0;
	}
	Text::StringBuilderUTF8 sb;
	sign.GetHashB64(&sb);
	if (!sb.Equals(&token[i2 + 1]))
	{
		MemFree(headerBuff);
		MemFree(payloadBuff);
		return 0;
	}

	param->Clear();
	Text::JSONBase *payloadJson = Text::JSONBase::ParseJSONStrLen(payloadBuff, payloadSize);
	MemFree(headerBuff);
	MemFree(payloadBuff);
	if (payloadJson == 0)
	{
		return 0;
	}
	if (payloadJson->GetJSType() != Text::JSONBase::JST_OBJECT)
	{
		payloadJson->EndUse();
		return 0;
	}
	Data::StringUTF8Map<const UTF8Char *> *retMap;
	NEW_CLASS(retMap, Data::StringUTF8Map<const UTF8Char *>());
	Text::JSONObject *payloadObj = (Text::JSONObject*)payloadJson;
	Text::JSONBase *json;
	Data::ArrayList<const UTF8Char *> objNames;
	payloadObj->GetObjectNames(&objNames);
	const UTF8Char *name;
	UOSInt i = 0;
	UOSInt j = objNames.GetCount();
	while (i < j)
	{
		name = objNames.GetItem(i);
		json = payloadObj->GetObjectValue(name);
		if (Text::StrEquals(name, (const UTF8Char*)"iss"))
		{
			if (json != 0 && json->GetJSType() == Text::JSONBase::JST_STRINGUTF8)
			{
				param->SetIssuer(((Text::JSONStringUTF8*)json)->GetValue());
			}
		}
		else if (Text::StrEquals(name, (const UTF8Char*)"sub"))
		{
			if (json != 0 && json->GetJSType() == Text::JSONBase::JST_STRINGUTF8)
			{
				param->SetSubject(((Text::JSONStringUTF8*)json)->GetValue());
			}
		}
		else if (Text::StrEquals(name, (const UTF8Char*)"aud"))
		{
			if (json != 0 && json->GetJSType() == Text::JSONBase::JST_STRINGUTF8)
			{
				param->SetAudience(((Text::JSONStringUTF8*)json)->GetValue());
			}
		}
		else if (Text::StrEquals(name, (const UTF8Char*)"exp"))
		{
			if (json != 0 && json->GetJSType() == Text::JSONBase::JST_NUMBER)
			{
				param->SetExpirationTime((long)((Text::JSONNumber*)json)->GetValue());
			}
		}
		else if (Text::StrEquals(name, (const UTF8Char*)"nbf"))
		{
			if (json != 0 && json->GetJSType() == Text::JSONBase::JST_NUMBER)
			{
				param->SetNotBefore((long)((Text::JSONNumber*)json)->GetValue());
			}
		}
		else if (Text::StrEquals(name, (const UTF8Char*)"iat"))
		{
			if (json != 0 && json->GetJSType() == Text::JSONBase::JST_NUMBER)
			{
				param->SetIssuedAt((long)((Text::JSONNumber*)json)->GetValue());
			}
		}
		else if (Text::StrEquals(name, (const UTF8Char*)"jti"))
		{
			if (json != 0 && json->GetJSType() == Text::JSONBase::JST_STRINGUTF8)
			{
				param->SetJWTId(((Text::JSONStringUTF8*)json)->GetValue());
			}
		}
		else
		{
			if (json == 0)
			{
				retMap->Put(name, 0);
			}
			else if (json->GetJSType() == Text::JSONBase::JST_STRINGUTF8)
			{
				retMap->Put(name, SCOPY_TEXT(((Text::JSONStringUTF8*)json)->GetValue()));
			}
			else
			{
				sb.ClearStr();
				json->ToJSONString(&sb);
				retMap->Put(name, Text::StrCopyNew(sb.ToString()));
			}
		}
		i++;
	}
	payloadJson->EndUse();
	return retMap;	
}

void Crypto::Token::JWTHandler::FreeResult(Data::StringUTF8Map<const UTF8Char*> *result)
{
	if (result)
	{
		Data::ArrayList<const UTF8Char*>* vals = result->GetValues();
		LIST_FREE_FUNC(vals, Text::StrDelNew);
		DEL_CLASS(result);
	}
}
