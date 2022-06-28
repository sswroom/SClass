#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/Token/JWTParam.h"
#include "Text/MyString.h"

Crypto::Token::JWTParam::JWTParam()
{
	this->iss = 0;
	this->sub = 0;
	this->aud = 0;
	this->exp = 0;
	this->nbf = 0;
	this->iat = 0;
	this->jti = 0;
}

Crypto::Token::JWTParam::~JWTParam()
{
	SDEL_STRING(this->iss);
	SDEL_STRING(this->sub);
	SDEL_STRING(this->aud);
	SDEL_STRING(this->jti);
}

void Crypto::Token::JWTParam::Clear()
{
	SDEL_STRING(this->iss);
	SDEL_STRING(this->sub);
	SDEL_STRING(this->aud);
	this->exp = 0;
	this->nbf = 0;
	this->iat = 0;
	SDEL_STRING(this->jti);
}

void Crypto::Token::JWTParam::SetIssuer(Text::String *issuer)
{
	SDEL_STRING(this->iss);
	this->iss = SCOPY_STRING(issuer);
}

Text::String *Crypto::Token::JWTParam::GetIssuer() const
{
	return this->iss;
}

Bool Crypto::Token::JWTParam::IsIssuerValid(const UTF8Char *issuer, UOSInt issuerLen) const
{
	return this->iss == 0 || this->iss->Equals(issuer, issuerLen);
}

void Crypto::Token::JWTParam::SetSubject(Text::String *subject)
{
	SDEL_STRING(this->sub);
	this->sub = SCOPY_STRING(subject);
}

Text::String *Crypto::Token::JWTParam::GetSubject() const
{
	return this->sub;
}

void Crypto::Token::JWTParam::SetAudience(Text::String *audience)
{
	SDEL_STRING(this->aud);
	this->aud = SCOPY_STRING(audience);
}

Text::String *Crypto::Token::JWTParam::GetAudience() const
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

void Crypto::Token::JWTParam::SetJWTId(Text::String *id)
{
	SDEL_STRING(this->jti);
	this->jti = SCOPY_STRING(id);
}

Text::String *Crypto::Token::JWTParam::GetJWTId() const
{
	return this->jti;
}

void Crypto::Token::JWTParam::ToString(Text::StringBuilderUTF8 *sb) const
{
	Bool found = false;
	sb->AppendC(UTF8STRC("Params ["));
	if (this->iss != 0)
	{
		if (found) sb->AppendC(UTF8STRC(", "));
		sb->AppendC(UTF8STRC("iss="));
		sb->Append(this->iss);
		found = true;
	}
	if (this->sub != 0)
	{
		if (found) sb->AppendC(UTF8STRC(", "));
		sb->AppendC(UTF8STRC("sub="));
		sb->Append(this->sub);
		found = true;
	}
	if (this->aud != 0)
	{
		if (found) sb->AppendC(UTF8STRC(", "));
		sb->AppendC(UTF8STRC("aud="));
		sb->Append(this->aud);
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
	if (this->jti != 0)
	{
		if (found) sb->AppendC(UTF8STRC(", "));
		sb->AppendC(UTF8STRC("jti="));
		sb->Append(this->jti);
		found = true;
	}
	sb->AppendC(UTF8STRC("]"));
}
