#include "Stdafx.h"
#include "Crypto/Token/JWToken.h"
#include "Text/JSON.h"
#include "Text/TextBinEnc/Base64Enc.h"

Crypto::Token::JWToken::JWToken(JWSignature::Algorithm alg)
{
	this->alg = alg;
	this->header = 0;
	this->payload = 0;
	this->sign = 0;
	this->signSize = 0;
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

void Crypto::Token::JWToken::SetHeader(Text::CStringNN header)
{
	SDEL_STRING(this->header);
	this->header = Text::String::New(header).Ptr();
}

Text::String *Crypto::Token::JWToken::GetHeader() const
{
	return this->header;
}

void Crypto::Token::JWToken::SetPayload(Text::CStringNN payload)
{
	SDEL_STRING(this->payload);
	this->payload = Text::String::New(payload).Ptr();
}

Text::String *Crypto::Token::JWToken::GetPayload() const
{
	return this->payload;
}

void Crypto::Token::JWToken::SetSignature(const UInt8 *sign, UOSInt signSize)
{
	if (this->sign)
		MemFree(this->sign);
	this->sign = MemAlloc(UInt8, signSize);
	this->signSize = signSize;
	MemCopyNO(this->sign, sign, signSize);
}

Bool Crypto::Token::JWToken::SignatureValid(Net::SSLEngine *ssl, const UInt8 *key, UOSInt keyLeng)
{
	///////////////////////////////
	return false;
}

void Crypto::Token::JWToken::ToString(NotNullPtr<Text::StringBuilderUTF8> sb) const
{
	if (this->header == 0 || this->payload == 0 || this->sign == 0)
	{
		return;
	}
	Text::TextBinEnc::Base64Enc b64(Text::TextBinEnc::Base64Enc::Charset::URL, true);
	b64.EncodeBin(sb, this->header->v, this->header->leng);
	sb->AppendUTF8Char('.');
	b64.EncodeBin(sb, this->payload->v, this->payload->leng);
	sb->AppendUTF8Char('.');
	b64.EncodeBin(sb, this->sign, this->signSize);
}

Crypto::Token::JWToken *Crypto::Token::JWToken::Generate(JWSignature::Algorithm alg, Text::CStringNN payload, Net::SSLEngine *ssl, const UInt8 *privateKey, UOSInt privateKeyLeng)
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
	Crypto::Token::JWSignature sign(ssl, alg, privateKey, privateKeyLeng);
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
