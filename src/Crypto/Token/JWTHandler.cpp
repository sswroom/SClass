#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/Hash/HMAC.h"
#include "Crypto/Hash/IHash.h"
#include "Crypto/Hash/SHA256.h"
#include "Crypto/Hash/SHA384.h"
#include "Crypto/Hash/SHA512.h"
#include "Crypto/Token/JWTHandler.h"
#include "Text/EnumFinder.h"
#include "Text/JSON.h"
#include "Text/JSONBuilder.h"
#include "Text/MyString.h"
#include "Text/TextBinEnc/Base64Enc.h"

Crypto::Token::JWTHandler::JWTHandler(Algorithm alg, const UInt8 *privateKey, UOSInt privateKeyLeng)
{
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
	sptr = Text::StrConcat(sptr, GetAlgorithmName(alg));
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
	Crypto::Hash::IHash *hash;
	Crypto::Hash::IHash *ihash;
	switch (alg)
	{
	case HS256:
		NEW_CLASS(ihash, Crypto::Hash::SHA256());
		NEW_CLASS(hash, Crypto::Hash::HMAC(ihash, this->privateKey, this->privateKeyLeng));
		DEL_CLASS(ihash);
		break;
	case HS384:
		NEW_CLASS(ihash, Crypto::Hash::SHA384());
		NEW_CLASS(hash, Crypto::Hash::HMAC(ihash, this->privateKey, this->privateKeyLeng));
		DEL_CLASS(ihash);
		break;
	case HS512:
		NEW_CLASS(ihash, Crypto::Hash::SHA512());
		NEW_CLASS(hash, Crypto::Hash::HMAC(ihash, this->privateKey, this->privateKeyLeng));
		DEL_CLASS(ihash);
		break;
	case PS256:
	case PS384:
	case PS512:
	case RS256:
	case RS384:
	case RS512:
	case ES256:
	case ES256K:
	case ES384:
	case ES512:
	case EDDSA:
	case UNKNOWN:
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

Data::StringUTF8Map<const UTF8Char*> *Crypto::Token::JWTHandler::Parse(const UTF8Char *token, JWTParam *param)
{
	OSInt i1 = Text::StrIndexOf(token, '.');;
	OSInt i2 = Text::StrIndexOf(&token[i1 + 1], '.');
	if (i1 >= 0 && i2 >= 0)
	{
		i2 += i1 + 1;
	}
	OSInt i3 = Text::StrIndexOf(&token[i2 + 1], '.');
	if (i2 < 0 || i3 >= 0)
	{
		return 0;
	}
	Text::TextBinEnc::Base64Enc b64url(Text::TextBinEnc::Base64Enc::CS_URL, true);
	UInt8 *headerBuff = MemAlloc(UInt8, (UOSInt)i1);
	UOSInt headerSize;
	UInt8 *payloadBuff = MemAlloc(UInt8, (UOSInt)(i2 - i1));
	UOSInt payloadSize;
	headerSize = b64url.DecodeBin(token, (UOSInt)i1, headerBuff);
	payloadSize = b64url.DecodeBin(&token[i1 + 1], (UOSInt)(i2 - i1 - 1), payloadBuff);
	Text::JSONBase *hdrJson = Text::JSONBase::ParseJSONStrLen(headerBuff, headerSize);
	Algorithm tokenAlg = UNKNOWN;
	if (hdrJson != 0)
	{
		if (hdrJson->GetJSType() == Text::JSONBase::JST_OBJECT)
		{
			Text::JSONBase *algJson = ((Text::JSONObject*)hdrJson)->GetObjectValue((const UTF8Char*)"alg");
			if (algJson != 0 && algJson->GetJSType() == Text::JSONBase::JST_STRINGUTF8)
			{
				tokenAlg = GetAlgorithmByName(((Text::JSONStringUTF8*)algJson)->GetValue());
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
	Crypto::Hash::IHash *hash;
	Crypto::Hash::IHash *ihash;
	switch (this->alg)
	{
	case HS256:
		NEW_CLASS(ihash, Crypto::Hash::SHA256());
		NEW_CLASS(hash, Crypto::Hash::HMAC(ihash, this->privateKey, this->privateKeyLeng));
		DEL_CLASS(ihash);
		break;
	case HS384:
		NEW_CLASS(ihash, Crypto::Hash::SHA384());
		NEW_CLASS(hash, Crypto::Hash::HMAC(ihash, this->privateKey, this->privateKeyLeng));
		DEL_CLASS(ihash);
		break;
	case HS512:
		NEW_CLASS(ihash, Crypto::Hash::SHA512());
		NEW_CLASS(hash, Crypto::Hash::HMAC(ihash, this->privateKey, this->privateKeyLeng));
		DEL_CLASS(ihash);
		break;
	case PS256:
	case PS384:
	case PS512:
	case RS256:
	case RS384:
	case RS512:
	case ES256:
	case ES256K:
	case ES384:
	case ES512:
	case EDDSA:
	case UNKNOWN:
	default:
		MemFree(headerBuff);
		MemFree(payloadBuff);
		return 0;
	}
	UInt8 hashBuff[128];
	Text::StringBuilderUTF8 sb;
	hash->Calc(token, (UOSInt)i2);
	hash->GetValue(hashBuff);
	b64url.EncodeBin(&sb, hashBuff, hash->GetResultSize());
	if (!sb.EqualsICase(&token[i2 + 1]))
	{
		DEL_CLASS(hash);
		MemFree(headerBuff);
		MemFree(payloadBuff);
		return 0;
	}
	DEL_CLASS(hash);

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
	Data::ArrayList<const UTF8Char*> *vals = result->GetValues();
	LIST_FREE_FUNC(vals, Text::StrDelNew);
	DEL_CLASS(result);
}

Crypto::Token::JWTHandler *Crypto::Token::JWTHandler::CreateHMAC(Algorithm alg, const UInt8 *key, UOSInt keyLeng)
{
	JWTHandler *jwt;
	switch (alg)
	{
	case HS256:
	case HS384:
	case HS512:
		NEW_CLASS(jwt, JWTHandler(alg, key, keyLeng));
		return jwt;
	case PS256:
	case PS384:
	case PS512:
	case RS256:
	case RS384:
	case RS512:
	case ES256:
	case ES256K:
	case ES384:
	case ES512:
	case EDDSA:
	case UNKNOWN:
	default:
		return 0;
	}
}

const UTF8Char *Crypto::Token::JWTHandler::GetAlgorithmName(Algorithm alg)
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
	case UNKNOWN:
	default:
		return (const UTF8Char*)"UNK";
	}
}

Crypto::Token::JWTHandler::Algorithm Crypto::Token::JWTHandler::GetAlgorithmByName(const UTF8Char *name)
{
	Text::EnumFinder<Algorithm> finder((const Char*)name, UNKNOWN);
	finder.Entry("HS256", HS256);
	finder.Entry("HS384", HS384);
	finder.Entry("HS512", HS512);
	finder.Entry("PS256", PS256);
	finder.Entry("PS384", PS384);
	finder.Entry("PS512", PS512);
	finder.Entry("RS256", RS256);
	finder.Entry("RS384", RS384);
	finder.Entry("RS512", RS512);
	finder.Entry("ES256", ES256);
	finder.Entry("ES256K", ES256K);
	finder.Entry("ES384", ES384);
	finder.Entry("ES512", ES512);
	finder.Entry("EdDSA", EDDSA);
	return finder.GetResult();
}
