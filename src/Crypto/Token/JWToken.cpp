#include "Stdafx.h"
#include "Crypto/Token/JWToken.h"
#include "Text/JSON.h"
#include "Text/JSText.h"
#include "Text/TextBinEnc/Base64Enc.h"

Crypto::Token::JWToken::PayloadMapping Crypto::Token::JWToken::payloadNames[] = 
{
//https://www.iana.org/assignments/jwt/jwt.xhtml#claims
// client_id

//https://learn.microsoft.com/en-us/azure/active-directory/develop/id-token-claims-reference
//https://learn.microsoft.com/en-us/azure/active-directory/develop/access-token-claims-reference
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
	this->header = nullptr;
	this->payload = nullptr;
	this->sign = nullptr;
	this->signSize = 0;
}

void Crypto::Token::JWToken::SetHeader(Text::CStringNN header)
{
	OPTSTR_DEL(this->header);
	this->header = Text::String::New(header).Ptr();
}

void Crypto::Token::JWToken::SetPayload(Text::CStringNN payload)
{
	OPTSTR_DEL(this->payload);
	this->payload = Text::String::New(payload).Ptr();
}

void Crypto::Token::JWToken::SetSignature(UnsafeArray<const UInt8> sign, UIntOS signSize)
{
	UnsafeArray<UInt8> nnsign;
	if (this->sign.SetTo(nnsign))
		MemFreeArr(nnsign);
	this->sign = nnsign = MemAllocArr(UInt8, signSize);
	this->signSize = signSize;
	MemCopyNO(nnsign.Ptr(), sign.Ptr(), signSize);
}

Crypto::Token::JWToken::~JWToken()
{
	OPTSTR_DEL(this->header);
	OPTSTR_DEL(this->payload);
	UnsafeArray<UInt8> nnsign;
	if (this->sign.SetTo(nnsign))
	{
		MemFreeArr(nnsign);
	}
}

Crypto::Token::JWSignature::Algorithm Crypto::Token::JWToken::GetAlgorithm() const
{
	return this->alg;
}

Optional<Text::String> Crypto::Token::JWToken::GetHeader() const
{
	return this->header;
}

Optional<Text::String> Crypto::Token::JWToken::GetPayload() const
{
	return this->payload;
}

Crypto::Token::JWToken::VerifyType Crypto::Token::JWToken::GetVerifyType(NN<JWTParam> param) const
{
	NN<Text::String> s;
	if (param->GetIssuer().SetTo(s) && s->StartsWith(UTF8STRC("https://login.microsoftonline.com/")))
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

Bool Crypto::Token::JWToken::SignatureValid(Optional<Net::SSLEngine> ssl, UnsafeArray<const UInt8> key, UIntOS keyLeng, Crypto::Cert::X509Key::KeyType keyType)
{
	NN<Text::String> nnheader;
	NN<Text::String> nnpayload;
	UnsafeArray<UInt8> nnsign;
	if (!this->header.SetTo(nnheader) || !this->payload.SetTo(nnpayload) || !this->sign.SetTo(nnsign))
		return false;
	Text::TextBinEnc::Base64Enc b64(Text::TextBinEnc::Base64Enc::Charset::URL, true);
	Text::StringBuilderUTF8 sb;
	b64.EncodeBin(sb, nnheader->v.Ptr(), nnheader->leng);
	sb.AppendUTF8Char('.');
	b64.EncodeBin(sb, nnpayload->v.Ptr(), nnpayload->leng);
	Crypto::Token::JWSignature sign(ssl, this->alg, key, keyLeng, keyType);
	return sign.VerifyHash(sb.v, sb.leng, nnsign, this->signSize);
}

Optional<Data::StringMapObj<Text::String*>> Crypto::Token::JWToken::ParsePayload(NN<JWTParam> param, Bool keepDefault, Optional<Text::StringBuilderUTF8> sbErr)
{
	param->Clear();
	NN<Text::JSONBase> payloadJson;
	NN<Text::String> nnpayload;
	NN<Text::StringBuilderUTF8> nnsb;
	if (!this->payload.SetTo(nnpayload) || !Text::JSONBase::ParseJSONStr(nnpayload->ToCString()).SetTo(payloadJson))
	{
		if (sbErr.SetTo(nnsb)) nnsb->AppendC(UTF8STRC("Payload cannot be parsed with JSON"));
		return nullptr;
	}
	if (payloadJson->GetType() != Text::JSONType::Object)
	{
		payloadJson->EndUse();
		if (sbErr.SetTo(nnsb)) nnsb->AppendC(UTF8STRC("Payload is not JSON object"));
		return nullptr;
	}
	Text::StringBuilderUTF8 sb;
	Data::StringMapObj<Text::String *> *retMap;
	NEW_CLASS(retMap, Data::StringMapObj<Text::String *>());
	NN<Text::JSONObject> payloadObj = NN<Text::JSONObject>::ConvertFrom(payloadJson);
	NN<Text::JSONBase> json;
	Data::ArrayListNN<Text::String> objNames;
	payloadObj->GetObjectNames(objNames);
	NN<Text::String> name;
	Bool isDefault;
	Data::ArrayIterator<NN<Text::String>> it = objNames.Iterator();
	while (it.HasNext())
	{
		name = it.Next();

		isDefault = true;
		if (name->Equals(UTF8STRC("iss")))
		{
			if (payloadObj->GetObjectValue(name->ToCString()).SetTo(json) && json->GetType() == Text::JSONType::String)
			{
				param->SetIssuer(NN<Text::JSONString>::ConvertFrom(json)->GetValue());
			}
		}
		else if (name->Equals(UTF8STRC("sub")))
		{
			if (payloadObj->GetObjectValue(name->ToCString()).SetTo(json) && json->GetType() == Text::JSONType::String)
			{
				param->SetSubject(NN<Text::JSONString>::ConvertFrom(json)->GetValue());
			}
		}
		else if (name->Equals(UTF8STRC("aud")))
		{
			if (payloadObj->GetObjectValue(name->ToCString()).SetTo(json) && json->GetType() == Text::JSONType::String)
			{
				param->SetAudience(NN<Text::JSONString>::ConvertFrom(json)->GetValue());
			}
		}
		else if (name->Equals(UTF8STRC("exp")))
		{
			if (payloadObj->GetObjectValue(name->ToCString()).SetTo(json) && json->GetType() == Text::JSONType::Number)
			{
				param->SetExpirationTime(NN<Text::JSONNumber>::ConvertFrom(json)->GetAsInt64());
			}
		}
		else if (name->Equals(UTF8STRC("nbf")))
		{
			if (payloadObj->GetObjectValue(name->ToCString()).SetTo(json) && json->GetType() == Text::JSONType::Number)
			{
				param->SetNotBefore(NN<Text::JSONNumber>::ConvertFrom(json)->GetAsInt64());
			}
		}
		else if (name->Equals(UTF8STRC("iat")))
		{
			if (payloadObj->GetObjectValue(name->ToCString()).SetTo(json) && json->GetType() == Text::JSONType::Number)
			{
				param->SetIssuedAt(NN<Text::JSONNumber>::ConvertFrom(json)->GetAsInt64());
			}
		}
		else if (name->Equals(UTF8STRC("jti")))
		{
			if (payloadObj->GetObjectValue(name->ToCString()).SetTo(json) && json->GetType() == Text::JSONType::String)
			{
				param->SetJWTId(NN<Text::JSONString>::ConvertFrom(json)->GetValue());
			}
		}
		else
		{
			isDefault = false;
		}
		if (keepDefault || !isDefault)
		{
			if (!payloadObj->GetObjectValue(name->ToCString()).SetTo(json))
			{
				retMap->PutNN(name, 0);
			}
			else if (json->GetType() == Text::JSONType::String)
			{
				retMap->PutNN(name, NN<Text::JSONString>::ConvertFrom(json)->GetValue()->Clone().Ptr());
			}
			else
			{
				sb.ClearStr();
				json->ToJSONString(sb);
				retMap->PutNN(name, Text::String::New(sb.ToString(), sb.GetLength()).Ptr());
			}
		}
	}
	payloadJson->EndUse();
	return retMap;
}

void Crypto::Token::JWToken::FreeResult(NN<Data::StringMapObj<Text::String*>> result)
{
	NN<const Data::ArrayListObj<Text::String*>> vals = result->GetValues();
	LIST_FREE_STRING_NO_CLEAR(vals);
	result.Delete();
}

void Crypto::Token::JWToken::ToString(NN<Text::StringBuilderUTF8> sb) const
{
	NN<Text::String> nnheader;
	NN<Text::String> nnpayload;
	UnsafeArray<UInt8> nnsign;
	if (!this->header.SetTo(nnheader) || !this->payload.SetTo(nnpayload) || !this->sign.SetTo(nnsign))
		return;
	Text::TextBinEnc::Base64Enc b64(Text::TextBinEnc::Base64Enc::Charset::URL, true);
	b64.EncodeBin(sb, nnheader->v, nnheader->leng);
	sb->AppendUTF8Char('.');
	b64.EncodeBin(sb, nnpayload->v, nnpayload->leng);
	sb->AppendUTF8Char('.');
	b64.EncodeBin(sb, nnsign, this->signSize);
}

Optional<Crypto::Token::JWToken> Crypto::Token::JWToken::Generate(JWSignature::Algorithm alg, Text::CStringNN payload, Optional<Net::SSLEngine> ssl, UnsafeArray<const UInt8> key, UIntOS keyLeng, Crypto::Cert::X509Key::KeyType keyType)
{
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	sptr = Text::StrConcatC(sbuff, UTF8STRC("{\"alg\":\""));
	sptr = JWSignature::AlgorithmGetName(alg).ConcatTo(sptr);
	sptr = Text::StrConcatC(sptr, UTF8STRC("\",\"typ\":\"JWT\"}"));
	Text::StringBuilderUTF8 sb;
	Text::TextBinEnc::Base64Enc b64(Text::TextBinEnc::Base64Enc::Charset::URL, true);
	b64.EncodeBin(sb, sbuff, (UIntOS)(sptr - sbuff));
	sb.AppendUTF8Char('.');
	b64.EncodeBin(sb, payload.v, payload.leng);
	Crypto::Token::JWSignature sign(ssl, alg, key, keyLeng, keyType);
	if (!sign.CalcHash(sb.ToString(), sb.GetLength()))
	{
		return nullptr;
	}
	JWToken *token;
	NEW_CLASS(token, JWToken(alg));
	token->SetHeader(CSTRP(sbuff, sptr));
	token->SetPayload(payload);
	token->SetSignature(sign.GetSignature(), sign.GetSignatureLen());
	return token;
}

Optional<Crypto::Token::JWToken> Crypto::Token::JWToken::GenerateRSA(JWSignature::Algorithm alg, Text::CStringNN payload, Optional<Net::SSLEngine> ssl, Text::CStringNN keyId, NN<Crypto::Cert::X509PrivKey> key)
{
	NN<Net::SSLEngine> nnssl;
	if (!ssl.SetTo(nnssl))
	{
		return nullptr;
	}
	Text::StringBuilderUTF8 sbHeader;
	NN<Text::String> s;
	Crypto::Hash::HashType hash;
	if (alg == JWSignature::Algorithm::RS256)
	{
		sbHeader.Append(CSTR("{\"alg\":\"RS256\",\"typ\":\"JWT\",\"kid\":"));
		hash = Crypto::Hash::HashType::SHA256;
	}
	else if (alg == JWSignature::Algorithm::RS384)
	{
		sbHeader.Append(CSTR("{\"alg\":\"RS256\",\"typ\":\"JWT\",\"kid\":"));
		hash = Crypto::Hash::HashType::SHA384;
	}
	else if (alg == JWSignature::Algorithm::RS512)
	{
		sbHeader.Append(CSTR("{\"alg\":\"RS512\",\"typ\":\"JWT\",\"kid\":"));
		hash = Crypto::Hash::HashType::SHA512;
	}
	else
	{
		return nullptr;
	}
	s = Text::JSText::ToNewJSTextDQuote(keyId.v);
	sbHeader.Append(s);
	s->Release();
	sbHeader.Append(CSTR("}"));
	UInt8 signData[1024];
	UIntOS signLen;
	Text::StringBuilderUTF8 sb;
	Text::TextBinEnc::Base64Enc b64(Text::TextBinEnc::Base64Enc::Charset::URL, true);
	b64.EncodeBin(sb, sbHeader.v, sbHeader.leng);
	sb.AppendUTF8Char('.');
	b64.EncodeBin(sb, payload.v, payload.leng);
	NN<Crypto::Cert::X509Key> k;
	if (!key->CreateKey().SetTo(k))
		return nullptr;
	Bool succ = nnssl->Signature(k, hash, sb.ToByteArray(), signData, signLen);
	k.Delete();
	if (!succ)
		return nullptr;
	JWToken *token;
	NEW_CLASS(token, JWToken(alg));
	token->SetHeader(sbHeader.ToCString());
	token->SetPayload(payload);
	token->SetSignature(signData, signLen);
	return token;
}

Optional<Crypto::Token::JWToken> Crypto::Token::JWToken::Parse(Text::CStringNN token, Optional<Text::StringBuilderUTF8> sbErr)
{
	NN<Text::StringBuilderUTF8> nnsb;
	UIntOS i1 = token.IndexOf('.');;
	if (i1 == INVALID_INDEX)
	{
		if (sbErr.SetTo(nnsb)) nnsb->AppendC(UTF8STRC("Token format error: no . found"));
		return nullptr;
	}
	UIntOS i2 = token.IndexOf('.', i1 + 1);
	if (i2 == INVALID_INDEX)
	{
		if (sbErr.SetTo(nnsb)) nnsb->AppendC(UTF8STRC("Token format error: Only 1 . found"));
		return nullptr;
	}
	if (token.IndexOf('.', i2 + 1) != INVALID_INDEX)
	{
		if (sbErr.SetTo(nnsb)) nnsb->AppendC(UTF8STRC("Token format error: More than 2 . found"));
		return nullptr;
	}
	Text::TextBinEnc::Base64Enc b64url(Text::TextBinEnc::Base64Enc::Charset::URL, true);
	UInt8 *headerBuff = MemAlloc(UInt8, i1 + 1);
	UIntOS headerSize;
	UInt8 *payloadBuff = MemAlloc(UInt8, (i2 - i1));
	UIntOS payloadSize;
	UInt8 *signBuff = MemAlloc(UInt8, (token.leng - i2));
	UIntOS signSize;
	headerSize = b64url.DecodeBin(Text::CStringNN(token.v, i1), headerBuff);
	payloadSize = b64url.DecodeBin(Text::CStringNN(token.v + i1 + 1, i2 - i1 - 1), payloadBuff);
	signSize = b64url.DecodeBin(Text::CStringNN(token.v + i2 + 1, token.leng - i2 - 1), signBuff);
	headerBuff[headerSize] = 0;
	payloadBuff[payloadSize] = 0;
	Crypto::Token::JWSignature::Algorithm alg;
	NN<Text::JSONBase> json;
	if (!Text::JSONBase::ParseJSONStr(Text::CStringNN(headerBuff, headerSize)).SetTo(json))
	{
		if (sbErr.SetTo(nnsb)) nnsb->AppendC(UTF8STRC("Token format error: header is not JSON"));
		MemFree(headerBuff);
		MemFree(payloadBuff);
		MemFree(signBuff);
		return nullptr;
	}
	else if (json->GetType() != Text::JSONType::Object)
	{
		if (sbErr.SetTo(nnsb)) nnsb->AppendC(UTF8STRC("Token format error: header JSON is not object"));
		json->EndUse();
		MemFree(headerBuff);
		MemFree(payloadBuff);
		MemFree(signBuff);
		return nullptr;
	}
	NN<Text::String> sAlg;
	if (!json->GetValueString(CSTR("alg")).SetTo(sAlg))
	{
		if (sbErr.SetTo(nnsb)) nnsb->AppendC(UTF8STRC("Token format error: alg is not found"));
		json->EndUse();
		MemFree(headerBuff);
		MemFree(payloadBuff);
		MemFree(signBuff);
		return nullptr;
	}
	alg = JWSignature::AlgorithmGetByName(sAlg->v);
	if (alg == JWSignature::Algorithm::Unknown)
	{
		if (sbErr.SetTo(nnsb)) nnsb->AppendC(UTF8STRC("Token format error: alg is not supported"));
		json->EndUse();
		MemFree(headerBuff);
		MemFree(payloadBuff);
		MemFree(signBuff);
		return nullptr;
	}
	json->EndUse();
	if (!Text::JSONBase::ParseJSONStr(Text::CStringNN(payloadBuff, payloadSize)).SetTo(json))
	{
		if (sbErr.SetTo(nnsb)) nnsb->AppendC(UTF8STRC("Token format error: payload is not JSON"));
		MemFree(headerBuff);
		MemFree(payloadBuff);
		MemFree(signBuff);
		return nullptr;
	}
	else if (json->GetType() != Text::JSONType::Object)
	{
		if (sbErr.SetTo(nnsb)) nnsb->AppendC(UTF8STRC("Token format error: payload JSON is not object"));
		json->EndUse();
		MemFree(headerBuff);
		MemFree(payloadBuff);
		MemFree(signBuff);
		return nullptr;
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

Text::CStringNN Crypto::Token::JWToken::PayloadName(Text::CStringNN key)
{
	IntOS i = 0;
	IntOS j = (sizeof(payloadNames) / sizeof(payloadNames[0])) - 1;
	IntOS k;
	IntOS l;
	while (i <= j)
	{
		k = (i + j) >> 1;
		l = key.CompareToFast(Text::CStringNN(payloadNames[k].key, payloadNames[k].keyLen));
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
			return Text::CStringNN(payloadNames[k].name, payloadNames[k].nameLen);
		}
	}
	return key;
}

Text::CStringNN Crypto::Token::JWToken::VerifyTypeGetName(VerifyType val)
{
	switch (val)
	{
	default:
	case VerifyType::Unknown:
		return CSTR("Unknown");
	case VerifyType::Azure:
		return CSTR("Azure");
	case VerifyType::Password:
		return CSTR("Password");
	case VerifyType::Key:
		return CSTR("Key");
	}
}
