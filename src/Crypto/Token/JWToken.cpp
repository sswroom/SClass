#include "Stdafx.h"
#include "Crypto/Token/JWToken.h"
#include "Text/JSON.h"
#include "Text/TextBinEnc/Base64Enc.h"

Crypto::Token::JWToken::PayloadMapping Crypto::Token::JWToken::payloadNames[] = 
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

Crypto::Token::JWToken::JWToken(JWSignature::Algorithm alg)
{
	this->alg = alg;
	this->header = 0;
	this->payload = 0;
	this->sign = 0;
	this->signSize = 0;
}

void Crypto::Token::JWToken::SetHeader(Text::CStringNN header)
{
	SDEL_STRING(this->header);
	this->header = Text::String::New(header).Ptr();
}

void Crypto::Token::JWToken::SetPayload(Text::CStringNN payload)
{
	SDEL_STRING(this->payload);
	this->payload = Text::String::New(payload).Ptr();
}

void Crypto::Token::JWToken::SetSignature(const UInt8 *sign, UOSInt signSize)
{
	if (this->sign)
		MemFree(this->sign);
	this->sign = MemAlloc(UInt8, signSize);
	this->signSize = signSize;
	MemCopyNO(this->sign, sign, signSize);
}

Crypto::Token::JWToken::~JWToken()
{
	SDEL_STRING(this->header);
	SDEL_STRING(this->payload);
	if (this->sign)
	{
		MemFree(this->sign);
	}
}

Crypto::Token::JWSignature::Algorithm Crypto::Token::JWToken::GetAlgorithm() const
{
	return this->alg;
}

Text::String *Crypto::Token::JWToken::GetHeader() const
{
	return this->header;
}

Text::String *Crypto::Token::JWToken::GetPayload() const
{
	return this->payload;
}

Crypto::Token::JWToken::VerifyType Crypto::Token::JWToken::GetVerifyType(NotNullPtr<JWTParam> param) const
{
	Text::String *s = param->GetIssuer();
	if (s && s->StartsWith(UTF8STRC("https://login.microsoftonline.com/")))
	{
		return VerifyType::Azure;
	}
	else
	{
		switch (this->alg)
		{
		case Crypto::Token::JWSignature::Algorithm::HS256:
		case Crypto::Token::JWSignature::Algorithm::HS384:
		case Crypto::Token::JWSignature::Algorithm::HS512:
			return VerifyType::Password;
		case Crypto::Token::JWSignature::Algorithm::RS256:
		case Crypto::Token::JWSignature::Algorithm::RS384:
		case Crypto::Token::JWSignature::Algorithm::RS512:
			return VerifyType::Key;
		case Crypto::Token::JWSignature::Algorithm::PS256:
		case Crypto::Token::JWSignature::Algorithm::PS384:
		case Crypto::Token::JWSignature::Algorithm::PS512:
		case Crypto::Token::JWSignature::Algorithm::ES256:
		case Crypto::Token::JWSignature::Algorithm::ES256K:
		case Crypto::Token::JWSignature::Algorithm::ES384:
		case Crypto::Token::JWSignature::Algorithm::ES512:
		case Crypto::Token::JWSignature::Algorithm::EDDSA:
		case Crypto::Token::JWSignature::Algorithm::Unknown:
		default:
			return VerifyType::Unknown;
		}
	}

}

Bool Crypto::Token::JWToken::SignatureValid(Net::SSLEngine *ssl, const UInt8 *key, UOSInt keyLeng, Crypto::Cert::X509Key::KeyType keyType)
{
	Text::TextBinEnc::Base64Enc b64(Text::TextBinEnc::Base64Enc::Charset::URL, true);
	Text::StringBuilderUTF8 sb;
	b64.EncodeBin(sb, this->header->v, this->header->leng);
	sb.AppendUTF8Char('.');
	b64.EncodeBin(sb, this->payload->v, this->payload->leng);
	Crypto::Token::JWSignature sign(ssl, this->alg, key, keyLeng, keyType);
	return sign.VerifyHash(sb.v, sb.leng, this->sign, this->signSize);
}

Data::StringMap<Text::String*> *Crypto::Token::JWToken::ParsePayload(NotNullPtr<JWTParam> param, Bool keepDefault, Text::StringBuilderUTF8 *sbErr)
{
	param->Clear();
	Text::JSONBase *payloadJson = Text::JSONBase::ParseJSONStr(this->payload->ToCString());
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
	Text::StringBuilderUTF8 sb;
	Data::StringMap<Text::String *> *retMap;
	NEW_CLASS(retMap, Data::StringMap<Text::String *>());
	Text::JSONObject *payloadObj = (Text::JSONObject*)payloadJson;
	Text::JSONBase *json;
	Data::ArrayList<Text::String *> objNames;
	payloadObj->GetObjectNames(&objNames);
	Text::String *name;
	Bool isDefault;
	UOSInt i = 0;
	UOSInt j = objNames.GetCount();
	while (i < j)
	{
		name = objNames.GetItem(i);
		json = payloadObj->GetObjectValue(name->ToCString());

		isDefault = true;
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
		else
		{
			isDefault = false;
		}
		if (keepDefault || !isDefault)
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
				json->ToJSONString(sb);
				retMap->Put(name, Text::String::New(sb.ToString(), sb.GetLength()).Ptr());
			}
		}
		i++;
	}
	payloadJson->EndUse();
	return retMap;
}

void Crypto::Token::JWToken::FreeResult(Data::StringMap<Text::String*> *result)
{
	if (result)
	{
		NotNullPtr<const Data::ArrayList<Text::String*>> vals = result->GetValues();
		LIST_FREE_STRING_NO_CLEAR(vals);
		DEL_CLASS(result);
	}
}

void Crypto::Token::JWToken::ToString(NotNullPtr<Text::StringBuilderUTF8> sb) const
{
	Text::TextBinEnc::Base64Enc b64(Text::TextBinEnc::Base64Enc::Charset::URL, true);
	b64.EncodeBin(sb, this->header->v, this->header->leng);
	sb->AppendUTF8Char('.');
	b64.EncodeBin(sb, this->payload->v, this->payload->leng);
	sb->AppendUTF8Char('.');
	b64.EncodeBin(sb, this->sign, this->signSize);
}

Crypto::Token::JWToken *Crypto::Token::JWToken::Generate(JWSignature::Algorithm alg, Text::CStringNN payload, Net::SSLEngine *ssl, const UInt8 *key, UOSInt keyLeng, Crypto::Cert::X509Key::KeyType keyType)
{
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	sptr = Text::StrConcatC(sbuff, UTF8STRC("{\"alg\":\""));
	sptr = JWSignature::AlgorithmGetName(alg).ConcatTo(sptr);
	sptr = Text::StrConcatC(sptr, UTF8STRC("\",\"typ\":\"JWT\"}"));
	Text::StringBuilderUTF8 sb;
	Text::TextBinEnc::Base64Enc b64(Text::TextBinEnc::Base64Enc::Charset::URL, true);
	b64.EncodeBin(sb, sbuff, (UOSInt)(sptr - sbuff));
	sb.AppendUTF8Char('.');

	b64.EncodeBin(sb, payload.v, payload.leng);
	Crypto::Token::JWSignature sign(ssl, alg, key, keyLeng, keyType);
	if (!sign.CalcHash(sb.ToString(), sb.GetLength()))
	{
		return 0;
	}
	JWToken *token;
	NEW_CLASS(token, JWToken(alg));
	token->SetHeader(CSTRP(sbuff, sptr));
	token->SetPayload(payload);
	token->SetSignature(sign.GetSignature(), sign.GetSignatureLen());
	return token;
}

Crypto::Token::JWToken *Crypto::Token::JWToken::Parse(Text::CStringNN token, Text::StringBuilderUTF8 *sbErr)
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
	UInt8 *headerBuff = MemAlloc(UInt8, i1 + 1);
	UOSInt headerSize;
	UInt8 *payloadBuff = MemAlloc(UInt8, (i2 - i1));
	UOSInt payloadSize;
	UInt8 *signBuff = MemAlloc(UInt8, (token.leng - i2));
	UOSInt signSize;
	headerSize = b64url.DecodeBin(token.v, i1, headerBuff);
	payloadSize = b64url.DecodeBin(&token.v[i1 + 1], i2 - i1 - 1, payloadBuff);
	signSize = b64url.DecodeBin(&token.v[i2 + 1], token.leng - i2 - 1, signBuff);
	headerBuff[headerSize] = 0;
	payloadBuff[payloadSize] = 0;
	Crypto::Token::JWSignature::Algorithm alg;
	Text::JSONBase *json = Text::JSONBase::ParseJSONStr(Text::CString(headerBuff, headerSize));
	if (json == 0)
	{
		if (sbErr) sbErr->AppendC(UTF8STRC("Token format error: header is not JSON"));
		MemFree(headerBuff);
		MemFree(payloadBuff);
		MemFree(signBuff);
		return 0;
	}
	else if (json->GetType() != Text::JSONType::Object)
	{
		if (sbErr) sbErr->AppendC(UTF8STRC("Token format error: header JSON is not object"));
		json->EndUse();
		MemFree(headerBuff);
		MemFree(payloadBuff);
		MemFree(signBuff);
		return 0;
	}
	Text::String *sAlg = json->GetValueString(CSTR("alg"));
	if (sAlg == 0)
	{
		if (sbErr) sbErr->AppendC(UTF8STRC("Token format error: alg is not found"));
		json->EndUse();
		MemFree(headerBuff);
		MemFree(payloadBuff);
		MemFree(signBuff);
		return 0;
	}
	alg = JWSignature::AlgorithmGetByName(sAlg->v);
	if (alg == JWSignature::Algorithm::Unknown)
	{
		if (sbErr) sbErr->AppendC(UTF8STRC("Token format error: alg is not supported"));
		json->EndUse();
		MemFree(headerBuff);
		MemFree(payloadBuff);
		MemFree(signBuff);
		return 0;
	}
	json->EndUse();
	json = Text::JSONBase::ParseJSONStr(Text::CString(payloadBuff, payloadSize));
	if (json == 0)
	{
		if (sbErr) sbErr->AppendC(UTF8STRC("Token format error: payload is not JSON"));
		MemFree(headerBuff);
		MemFree(payloadBuff);
		MemFree(signBuff);
		return 0;
	}
	else if (json->GetType() != Text::JSONType::Object)
	{
		if (sbErr) sbErr->AppendC(UTF8STRC("Token format error: payload JSON is not object"));
		json->EndUse();
		MemFree(headerBuff);
		MemFree(payloadBuff);
		MemFree(signBuff);
		return 0;
	}
	json->EndUse();
	JWToken *jwt;
	NEW_CLASS(jwt, JWToken(alg));
	jwt->SetHeader(Text::CStringNN(headerBuff, headerSize));
	jwt->SetPayload(Text::CStringNN(payloadBuff, payloadSize));
	jwt->SetSignature(signBuff, signSize);

	MemFree(headerBuff);
	MemFree(payloadBuff);
	MemFree(signBuff);
	return jwt;
}

Text::CString Crypto::Token::JWToken::PayloadName(Text::CString key)
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

Text::CString Crypto::Token::JWToken::VerifyTypeGetName(VerifyType val)
{
	switch (val)
	{
	case VerifyType::Unknown:
		return CSTR("Unknown");
	case VerifyType::Azure:
		return CSTR("Azure");
	case VerifyType::Password:
		return CSTR("Password");
	case VerifyType::Key:
		return CSTR("Key");
	default:
		return CSTR_NULL;
	}
}
