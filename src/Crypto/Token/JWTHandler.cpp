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
	sptr = Text::StrConcatC(sbuff, UTF8STRC("{\"alg\":\""));
	sptr = JWSignature::AlgorithmGetName(alg).ConcatTo(sptr);
	sptr = Text::StrConcatC(sptr, UTF8STRC("\",\"typ\":\"JWT\"}"));
	Text::TextBinEnc::Base64Enc b64(Text::TextBinEnc::Base64Enc::Charset::URL, true);
	b64.EncodeBin(sb, sbuff, (UOSInt)(sptr - sbuff));
	sb->AppendUTF8Char('.');
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
			json->ObjectAddStr((const UTF8Char*)"iss", param->GetIssuer());
		}
		if (param->GetSubject() != 0)
		{
			json->ObjectAddStr((const UTF8Char*)"sub", param->GetSubject());
		}
		if (param->GetAudience() != 0)
		{
			json->ObjectAddStr((const UTF8Char*)"aud", param->GetAudience());
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
			json->ObjectAddStr((const UTF8Char*)"jti", param->GetJWTId());
		}
	}
	DEL_CLASS(json);
	b64.EncodeBin(sb, sbJson.ToString(), sbJson.GetLength());
	Crypto::Token::JWSignature sign(this->ssl, this->alg, this->privateKey, this->privateKeyLeng);
	if (!sign.CalcHash(sb->ToString(), sb->GetLength()))
	{
		return false;
	}
	sb->AppendUTF8Char('.');
	sign.GetHashB64(sb);
	return true;
}

Data::StringMap<Text::String*> *Crypto::Token::JWTHandler::Parse(const UTF8Char *token, JWTParam *param)
{
	UOSInt tokenLen = Text::StrCharCnt(token);
	UOSInt i1 = Text::StrIndexOfCharC(token, tokenLen, '.');;
	UOSInt i2 = Text::StrIndexOfCharC(&token[i1 + 1], tokenLen - i1 - 1, '.');
	if (i1 != INVALID_INDEX && i2 != INVALID_INDEX)
	{
		i2 += i1 + 1;
	}
	UOSInt i3 = Text::StrIndexOfCharC(&token[i2 + 1], tokenLen - i2 - 1, '.');
	if (i2 == INVALID_INDEX || i3 != INVALID_INDEX)
	{
		return 0;
	}
	Text::TextBinEnc::Base64Enc b64url(Text::TextBinEnc::Base64Enc::Charset::URL, true);
	UInt8 *headerBuff = MemAlloc(UInt8, i1);
	UOSInt headerSize;
	UInt8 *payloadBuff = MemAlloc(UInt8, (i2 - i1));
	UOSInt payloadSize;
	headerSize = b64url.DecodeBin(token, i1, headerBuff);
	payloadSize = b64url.DecodeBin(&token[i1 + 1], i2 - i1 - 1, payloadBuff);
	Text::JSONBase *hdrJson = Text::JSONBase::ParseJSONBytes(headerBuff, headerSize);
	JWSignature::Algorithm tokenAlg = JWSignature::Algorithm::Unknown;
	if (hdrJson != 0)
	{
		if (hdrJson->GetType() == Text::JSONType::Object)
		{
			Text::JSONBase *algJson = ((Text::JSONObject*)hdrJson)->GetObjectValue(UTF8STRC("alg"));
			if (algJson != 0 && algJson->GetType() == Text::JSONType::String)
			{
				tokenAlg = JWSignature::AlgorithmGetByName(((Text::JSONString*)algJson)->GetValue()->v);
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
	if (!sb.Equals(&token[i2 + 1], tokenLen - i2 - 1))
	{
		MemFree(headerBuff);
		MemFree(payloadBuff);
		return 0;
	}

	param->Clear();
	Text::JSONBase *payloadJson = Text::JSONBase::ParseJSONBytes(payloadBuff, payloadSize);
	MemFree(headerBuff);
	MemFree(payloadBuff);
	if (payloadJson == 0)
	{
		return 0;
	}
	if (payloadJson->GetType() != Text::JSONType::Object)
	{
		payloadJson->EndUse();
		return 0;
	}
	Data::StringMap<Text::String *> *retMap;
	NEW_CLASS(retMap, Data::StringMap<Text::String *>());
	Text::JSONObject *payloadObj = (Text::JSONObject*)payloadJson;
	Text::JSONBase *json;
	Data::ArrayList<Text::String *> objNames;
	payloadObj->GetObjectNames(&objNames);
	Text::String *name;
	UOSInt i = 0;
	UOSInt j = objNames.GetCount();
	while (i < j)
	{
		name = objNames.GetItem(i);
		json = payloadObj->GetObjectValue(name->v, name->leng);
		if (name->Equals(UTF8STRC("iss")))
		{
			if (json != 0 && json->GetType() == Text::JSONType::String)
			{
				param->SetIssuer(((Text::JSONString*)json)->GetValue());
			}
		}
		else if (name->Equals(UTF8STRC("sub")))
		{
			if (json != 0 && json->GetType() == Text::JSONType::String)
			{
				param->SetSubject(((Text::JSONString*)json)->GetValue());
			}
		}
		else if (name->Equals(UTF8STRC("aud")))
		{
			if (json != 0 && json->GetType() == Text::JSONType::String)
			{
				param->SetAudience(((Text::JSONString*)json)->GetValue());
			}
		}
		else if (name->Equals(UTF8STRC("exp")))
		{
			if (json != 0 && json->GetType() == Text::JSONType::Number)
			{
				param->SetExpirationTime((long)((Text::JSONNumber*)json)->GetValue());
			}
		}
		else if (name->Equals(UTF8STRC("nbf")))
		{
			if (json != 0 && json->GetType() == Text::JSONType::Number)
			{
				param->SetNotBefore((long)((Text::JSONNumber*)json)->GetValue());
			}
		}
		else if (name->Equals(UTF8STRC("iat")))
		{
			if (json != 0 && json->GetType() == Text::JSONType::Number)
			{
				param->SetIssuedAt((long)((Text::JSONNumber*)json)->GetValue());
			}
		}
		else if (name->Equals(UTF8STRC("jti")))
		{
			if (json != 0 && json->GetType() == Text::JSONType::String)
			{
				param->SetJWTId(((Text::JSONString*)json)->GetValue());
			}
		}
		else
		{
			if (json == 0)
			{
				retMap->Put(name, 0);
			}
			else if (json->GetType() == Text::JSONType::String)
			{
				retMap->Put(name, SCOPY_STRING(((Text::JSONString*)json)->GetValue()));
			}
			else
			{
				sb.ClearStr();
				json->ToJSONString(&sb);
				retMap->Put(name, Text::String::New(sb.ToString(), sb.GetLength()));
			}
		}
		i++;
	}
	payloadJson->EndUse();
	return retMap;	
}

void Crypto::Token::JWTHandler::FreeResult(Data::StringMap<Text::String*> *result)
{
	if (result)
	{
		Data::ArrayList<Text::String*>* vals = result->GetValues();
		LIST_FREE_STRING(vals);
		DEL_CLASS(result);
	}
}
