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

Crypto::Token::JWTHandler::PayloadMapping Crypto::Token::JWTHandler::payloadNames[] = 
{
//https://www.iana.org/assignments/jwt/jwt.xhtml#claims
// client_id

//https://learn.microsoft.com/en-us/azure/active-directory/develop/id-token-claims-reference
//https://learn.microsoft.com/en-us/azure/active-directory/develop/access-token-claims-reference
//https://login.microsoftonline.com/common/discovery/v2.0/keys
	{UTF8STRC("address"), UTF8STRC("Preferred postal address")},
	{UTF8STRC("acr"), UTF8STRC("Authentication Context Class Reference")},
	{UTF8STRC("act"), UTF8STRC("Actor")},
	{UTF8STRC("aio"), UTF8STRC("An internal claim that's used to record data for token reuse. Should be ignored.")},
	{UTF8STRC("amr"), UTF8STRC("Authentication Methods References")},
	{UTF8STRC("appid"), UTF8STRC("The application ID of the client using the token")},
	{UTF8STRC("appidacr"), UTF8STRC("Indicates authentication method of the client")},
	{UTF8STRC("at_hash"), UTF8STRC("Access Token hash value")},
	{UTF8STRC("attest"), UTF8STRC("Attestation level as defined in SHAKEN framework")},
	{UTF8STRC("aud"), UTF8STRC("Audience")},
	{UTF8STRC("auth_time"), UTF8STRC("Time when the authentication occurred")},
	{UTF8STRC("azp"), UTF8STRC("Authorized party - the party to which the ID Token was issued")},
	{UTF8STRC("azpacr"), UTF8STRC("A replacement for appidacr. Indicates the authentication method of the client")},
	{UTF8STRC("birthdate"), UTF8STRC("Birthday")},
	{UTF8STRC("c_hash"), UTF8STRC("Code hash value")},
	{UTF8STRC("client_id"), UTF8STRC("Client Identifier")},
	{UTF8STRC("cnf"), UTF8STRC("Confirmation")},
	{UTF8STRC("dest"), UTF8STRC("Destination Identity String")},
	{UTF8STRC("email"), UTF8STRC("Preferred e-mail address")},
	{UTF8STRC("email_verified"), UTF8STRC("True if the e-mail address has been verified; otherwise false")},
	{UTF8STRC("events"), UTF8STRC("Security Events")},
	{UTF8STRC("exp"), UTF8STRC("Expiration Time")},
	{UTF8STRC("family_name"), UTF8STRC("Surname(s) or last name(s)")},
	{UTF8STRC("gender"), UTF8STRC("Gender")},
	{UTF8STRC("given_name"), UTF8STRC("Given name(s) or first name(s)")},
	{UTF8STRC("iat"), UTF8STRC("Issued At")},
	{UTF8STRC("idp"), UTF8STRC("Records the identity provider that authenticated the subject of the token.")},
	{UTF8STRC("iss"), UTF8STRC("Issuer")},
	{UTF8STRC("jti"), UTF8STRC("JWT ID")},
	{UTF8STRC("locale"), UTF8STRC("Locale")},
	{UTF8STRC("middle_name"), UTF8STRC("Middle name(s)")},
	{UTF8STRC("mky"), UTF8STRC("Media Key Fingerprint String")},
	{UTF8STRC("name"), UTF8STRC("Full name")},
	{UTF8STRC("nbf"), UTF8STRC("Not Before")},
	{UTF8STRC("nickname"), UTF8STRC("Casual name")},
	{UTF8STRC("nonce"), UTF8STRC("Value used to associate a Client session with an ID Token (MAY also be used for nonce values in other applications of JWTs)")},
	{UTF8STRC("oid"), UTF8STRC("The immutable identifier for an object, in this case, a user account")},
	{UTF8STRC("orig"), UTF8STRC("Originating Identity String")},
	{UTF8STRC("origid"), UTF8STRC("Originating Identifier as defined in SHAKEN framework")},
	{UTF8STRC("phone_number"), UTF8STRC("Preferred telephone number")},
	{UTF8STRC("phone_number_verified"), UTF8STRC("True if the phone number has been verified; otherwise false")},
	{UTF8STRC("picture"), UTF8STRC("Profile picture URL")},
	{UTF8STRC("preferred_username"), UTF8STRC("Shorthand name by which the End-User wishes to be referred to")},
	{UTF8STRC("profile"), UTF8STRC("Profile page URL")},
	{UTF8STRC("rh"), UTF8STRC("An internal claim used to revalidate tokens. Should be ignored.")},
	{UTF8STRC("rph"), UTF8STRC("Resource Priority Header Authorization")},
	{UTF8STRC("roles"), UTF8STRC("The set of roles that were assigned to the user who is logging in.")},
	{UTF8STRC("scope"), UTF8STRC("Scope Values")},
	{UTF8STRC("scp"), UTF8STRC("The set of scopes exposed by the application for which the client application has requested (and received) consent")},
	{UTF8STRC("sid"), UTF8STRC("Session ID")},
	{UTF8STRC("sip_callid"), UTF8STRC("SIP Call-Id header field value")},
	{UTF8STRC("sip_cseq_num"), UTF8STRC("SIP CSeq numeric header field parameter value")},
	{UTF8STRC("sip_date"), UTF8STRC("SIP Date header field value")},
	{UTF8STRC("sip_from_tag"), UTF8STRC("SIP From tag header field parameter value")},
	{UTF8STRC("sip_via_branch"), UTF8STRC("SIP Via branch header field parameter value")},
	{UTF8STRC("sub"), UTF8STRC("Subject")},
	{UTF8STRC("sub_jwk"), UTF8STRC("Public key used to check the signature of an ID Token")},
	{UTF8STRC("sub_jwk"), UTF8STRC("Public key used to check the signature of an ID Token")},
	{UTF8STRC("tid"), UTF8STRC("Represents the tenant that the user is signing in to")},
	{UTF8STRC("toe"), UTF8STRC("Time of Event")},
	{UTF8STRC("txn"), UTF8STRC("Transaction Identifier")},
	{UTF8STRC("unique_name"), UTF8STRC("Provides a human readable value that identifies the subject of the token")},
	{UTF8STRC("uti"), UTF8STRC("Token identifier claim, equivalent to jti in the JWT specification")},
	{UTF8STRC("ver"), UTF8STRC("Version")},
	{UTF8STRC("vot"), UTF8STRC("Vector of Trust value")},
	{UTF8STRC("vtm"), UTF8STRC("Vector of Trust trustmark URL")},
	{UTF8STRC("website"), UTF8STRC("Web page or blog URL")},
	{UTF8STRC("zoneinfo"), UTF8STRC("Time zone")},
};

Crypto::Token::JWTHandler::JWTHandler(Net::SSLEngine *ssl, JWSignature::Algorithm alg, const UInt8 *key, UOSInt keyLeng)
{
	this->ssl = ssl;
	this->alg = alg;
	this->key = MemAlloc(UInt8, keyLeng);
	this->keyLeng = keyLeng;
	MemCopyNO(this->key, key, this->keyLeng);
}

Crypto::Token::JWTHandler::JWTHandler(Net::SSLEngine *ssl)
{
	this->ssl = ssl;
	this->alg = JWSignature::Algorithm::HS512;
	this->key = 0;
	this->keyLeng = 0;
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
	Crypto::Token::JWToken *token = Crypto::Token::JWToken::Generate(alg, sbJson.ToCString(), this->ssl, this->key, this->keyLeng);
	if (token == 0)
	{
		return false;
	}
	token->ToString(sb);
	DEL_CLASS(token);
	return true;
}

Data::StringMap<Text::String*> *Crypto::Token::JWTHandler::Parse(Text::CString token, NotNullPtr<JWTParam> param, Bool ignoreSignValid, Text::StringBuilderUTF8 *sbErr)
{
	UOSInt i1 = token.IndexOf('.');;
	if (i1 == INVALID_INDEX)
	{
		if (sbErr) sbErr->AppendC(UTF8STRC("Token format error: no . found"));
		return 0;
	}
	UOSInt i2 = token.IndexOf('.', i1 + 1);
	if (i2 == INVALID_INDEX)
	{
		if (sbErr) sbErr->AppendC(UTF8STRC("Token format error: Only 1 . found"));
		return 0;
	}
	if (token.IndexOf('.', i2 + 1) != INVALID_INDEX)
	{
		if (sbErr) sbErr->AppendC(UTF8STRC("Token format error: More than 2 . found"));
		return 0;
	}
	Text::TextBinEnc::Base64Enc b64url(Text::TextBinEnc::Base64Enc::Charset::URL, true);
	UInt8 *headerBuff = MemAlloc(UInt8, i1);
	UOSInt headerSize;
	UInt8 *payloadBuff = MemAlloc(UInt8, (i2 - i1));
	UOSInt payloadSize;
	headerSize = b64url.DecodeBin(token.v, i1, headerBuff);
	payloadSize = b64url.DecodeBin(&token.v[i1 + 1], i2 - i1 - 1, payloadBuff);
	Text::JSONBase *hdrJson = Text::JSONBase::ParseJSONBytes(headerBuff, headerSize);
	JWSignature::Algorithm tokenAlg = JWSignature::Algorithm::Unknown;
	if (hdrJson != 0)
	{
		if (hdrJson->GetType() == Text::JSONType::Object)
		{
			Text::JSONBase *algJson = ((Text::JSONObject*)hdrJson)->GetObjectValue(CSTR("alg"));
			if (algJson != 0 && algJson->GetType() == Text::JSONType::String)
			{
				tokenAlg = JWSignature::AlgorithmGetByName(((Text::JSONString*)algJson)->GetValue()->v);
			}

		}
		hdrJson->EndUse();
	}
	Text::StringBuilderUTF8 sb;
	if (!ignoreSignValid)
	{
		Crypto::Token::JWSignature sign(this->ssl, tokenAlg, this->key, this->keyLeng);
		if (!sign.CalcHash(token.v, (UOSInt)i2))
		{
			MemFree(headerBuff);
			MemFree(payloadBuff);
			if (sbErr) sbErr->AppendC(UTF8STRC("Error in calculating hash"));
			return 0;
		}
		sign.GetHashB64(sb);
		if (!sb.Equals(&token.v[i2 + 1], token.leng - i2 - 1))
		{
			MemFree(headerBuff);
			MemFree(payloadBuff);
			if (sbErr) sbErr->AppendC(UTF8STRC("Hash not match"));
			return 0;
		}
	}

	param->Clear();
	Text::JSONBase *payloadJson = Text::JSONBase::ParseJSONBytes(payloadBuff, payloadSize);
	MemFree(headerBuff);
	MemFree(payloadBuff);
	if (payloadJson == 0)
	{
		if (sbErr) sbErr->AppendC(UTF8STRC("Payload cannot be parsed with JSON"));
		return 0;
	}
	if (payloadJson->GetType() != Text::JSONType::Object)
	{
		payloadJson->EndUse();
		if (sbErr) sbErr->AppendC(UTF8STRC("Payload is not JSON object"));
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
		json = payloadObj->GetObjectValue(name->ToCString());

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
			json->ToJSONString(sb);
			retMap->Put(name, Text::String::New(sb.ToString(), sb.GetLength()).Ptr());
		}

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
				param->SetExpirationTime(((Text::JSONNumber*)json)->GetAsInt64());
			}
		}
		else if (name->Equals(UTF8STRC("nbf")))
		{
			if (json != 0 && json->GetType() == Text::JSONType::Number)
			{
				param->SetNotBefore(((Text::JSONNumber*)json)->GetAsInt64());
			}
		}
		else if (name->Equals(UTF8STRC("iat")))
		{
			if (json != 0 && json->GetType() == Text::JSONType::Number)
			{
				param->SetIssuedAt(((Text::JSONNumber*)json)->GetAsInt64());
			}
		}
		else if (name->Equals(UTF8STRC("jti")))
		{
			if (json != 0 && json->GetType() == Text::JSONType::String)
			{
				param->SetJWTId(((Text::JSONString*)json)->GetValue());
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
		NotNullPtr<const Data::ArrayList<Text::String*>> vals = result->GetValues();
		LIST_FREE_STRING_NO_CLEAR(vals);
		DEL_CLASS(result);
	}
}

Text::CString Crypto::Token::JWTHandler::PayloadName(Text::CString key)
{
	OSInt i = 0;
	OSInt j = (sizeof(payloadNames) / sizeof(payloadNames[0])) - 1;
	OSInt k;
	OSInt l;
	while (i <= j)
	{
		k = (i + j) >> 1;
		l = key.CompareToFast(Text::CString(payloadNames[k].key, payloadNames[k].keyLen));
		if (l > 0)
		{
			i = k + 1;
		}
		else if (l < 0)
		{
			j = k - 1;
		}
		else
		{
			return Text::CString(payloadNames[k].name, payloadNames[k].nameLen);
		}
	}
	return key;
}
