#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/Token/JWTParam.h"
#include "Text/MyString.h"

Crypto::Token::JWTParam::JWTParam()
{
	this->iss = nullptr;
	this->sub = nullptr;
	this->aud = nullptr;
	this->exp = 0;
	this->nbf = 0;
	this->iat = 0;
	this->jti = nullptr;
}

Crypto::Token::JWTParam::~JWTParam()
{
	OPTSTR_DEL(this->iss);
	OPTSTR_DEL(this->sub);
	OPTSTR_DEL(this->aud);
	OPTSTR_DEL(this->jti);
}

void Crypto::Token::JWTParam::Clear()
{
	OPTSTR_DEL(this->iss);
	OPTSTR_DEL(this->sub);
	OPTSTR_DEL(this->aud);
	this->exp = 0;
	this->nbf = 0;
	this->iat = 0;
	OPTSTR_DEL(this->jti);
	this->iss = nullptr;
	this->sub = nullptr;
	this->aud = nullptr;
	this->jti = nullptr;
}

void Crypto::Token::JWTParam::SetIssuer(Optional<Text::String> issuer)
{
	OPTSTR_DEL(this->iss);
	this->iss = Text::String::CopyOrNull(issuer);
}

Optional<Text::String> Crypto::Token::JWTParam::GetIssuer() const
{
	return this->iss;
}

Bool Crypto::Token::JWTParam::IsIssuerValid(UnsafeArray<const UTF8Char> issuer, UIntOS issuerLen) const
{
	NN<Text::String> s;
	return !this->iss.SetTo(s) || s->Equals(issuer, issuerLen);
}

void Crypto::Token::JWTParam::SetSubject(Optional<Text::String> subject)
{
	OPTSTR_DEL(this->sub);
	this->sub = Text::String::CopyOrNull(subject);
}

Optional<Text::String> Crypto::Token::JWTParam::GetSubject() const
{
	return this->sub;
}

void Crypto::Token::JWTParam::SetAudience(Optional<Text::String> audience)
{
	OPTSTR_DEL(this->aud);
	this->aud = Text::String::CopyOrNull(audience);
}

Optional<Text::String> Crypto::Token::JWTParam::GetAudience() const
{
	return this->aud;
}

void Crypto::Token::JWTParam::SetExpirationTime(Int64 t)
{
	this->exp = t;
}

Int64 Crypto::Token::JWTParam::GetExpirationTime() const
{
	return this->exp;
}

void Crypto::Token::JWTParam::SetNotBefore(Int64 t)
{
	this->nbf = t;
}

Int64 Crypto::Token::JWTParam::GetNotBefore() const
{
	return this->nbf;
}

void Crypto::Token::JWTParam::SetIssuedAt(Int64 t)
{
	this->iat = t;
}

Int64 Crypto::Token::JWTParam::GetIssuedAt() const
{
	return this->iat;
}

void Crypto::Token::JWTParam::SetJWTId(Optional<Text::String> id)
{
	OPTSTR_DEL(this->jti);
	this->jti = Text::String::CopyOrNull(id);
}

Optional<Text::String> Crypto::Token::JWTParam::GetJWTId() const
{
	return this->jti;
}

Bool Crypto::Token::JWTParam::IsExpired(Data::Timestamp ts) const
{
	Int64 t = ts.ToUnixTimestamp();
	return t < this->nbf || t >= this->exp;
}

void Crypto::Token::JWTParam::ToString(NN<Text::StringBuilderUTF8> sb) const
{
	NN<Text::String> s;
	Bool found = false;
	sb->AppendC(UTF8STRC("Params ["));
	if (this->iss.SetTo(s))
	{
		if (found) sb->AppendC(UTF8STRC(", "));
		sb->AppendC(UTF8STRC("iss="));
		sb->Append(s);
		found = true;
	}
	if (this->sub.SetTo(s))
	{
		if (found) sb->AppendC(UTF8STRC(", "));
		sb->AppendC(UTF8STRC("sub="));
		sb->Append(s);
		found = true;
	}
	if (this->aud.SetTo(s))
	{
		if (found) sb->AppendC(UTF8STRC(", "));
		sb->AppendC(UTF8STRC("aud="));
		sb->Append(s);
		found = true;
	}
	if (this->exp != 0)
	{
		if (found) sb->AppendC(UTF8STRC(", "));
		sb->AppendC(UTF8STRC("exp="));
		sb->AppendI64(this->exp);
		found = true;
	}
	if (this->nbf != 0)
	{
		if (found) sb->AppendC(UTF8STRC(", "));
		sb->AppendC(UTF8STRC("nbf="));
		sb->AppendI64(this->nbf);
		found = true;
	}
	if (this->iat != 0)
	{
		if (found) sb->AppendC(UTF8STRC(", "));
		sb->AppendC(UTF8STRC("iat="));
		sb->AppendI64(this->iat);
		found = true;
	}
	if (this->jti.SetTo(s))
	{
		if (found) sb->AppendC(UTF8STRC(", "));
		sb->AppendC(UTF8STRC("jti="));
		sb->Append(s);
		found = true;
	}
	sb->AppendC(UTF8STRC("]"));
}
