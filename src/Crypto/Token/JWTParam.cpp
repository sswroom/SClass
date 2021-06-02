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
	SDEL_TEXT(this->iss);
	SDEL_TEXT(this->sub);
	SDEL_TEXT(this->aud);
	SDEL_TEXT(this->jti);
}

void Crypto::Token::JWTParam::Clear()
{
	SDEL_TEXT(this->iss);
	SDEL_TEXT(this->sub);
	SDEL_TEXT(this->aud);
	this->exp = 0;
	this->nbf = 0;
	this->iat = 0;
	SDEL_TEXT(this->jti);
}

void Crypto::Token::JWTParam::SetIssuer(const UTF8Char *issuer)
{
	SDEL_TEXT(this->iss);
	this->iss = SCOPY_TEXT(issuer);
}

const UTF8Char *Crypto::Token::JWTParam::GetIssuer()
{
	return this->iss;
}

Bool Crypto::Token::JWTParam::IsIssuerValid(const UTF8Char *issuer)
{
	return this->iss == 0 || Text::StrEquals(this->iss, issuer);
}

void Crypto::Token::JWTParam::SetSubject(const UTF8Char *subject)
{
	SDEL_TEXT(this->sub);
	this->sub = SCOPY_TEXT(subject);
}

const UTF8Char *Crypto::Token::JWTParam::GetSubject()
{
	return this->sub;
}

void Crypto::Token::JWTParam::SetAudience(const UTF8Char *audience)
{
	SDEL_TEXT(this->aud);
	this->aud = SCOPY_TEXT(audience);
}

const UTF8Char *Crypto::Token::JWTParam::GetAudience()
{
	return this->aud;
}

void Crypto::Token::JWTParam::SetExpirationTime(Int64 t)
{
	this->exp = t;
}

Int64 Crypto::Token::JWTParam::GetExpirationTime()
{
	return this->exp;
}

void Crypto::Token::JWTParam::SetNotBefore(Int64 t)
{
	this->nbf = t;
}

Int64 Crypto::Token::JWTParam::GetNotBefore()
{
	return this->nbf;
}

void Crypto::Token::JWTParam::SetIssuedAt(Int64 t)
{
	this->iat = t;
}

Int64 Crypto::Token::JWTParam::GetIssuedAt()
{
	return this->iat;
}

void Crypto::Token::JWTParam::SetJWTId(const UTF8Char *id)
{
	SDEL_TEXT(this->jti);
	this->jti = SCOPY_TEXT(id);
}

const UTF8Char *Crypto::Token::JWTParam::GetJWTId()
{
	return this->jti;
}

void Crypto::Token::JWTParam::ToString(Text::StringBuilderUTF *sb)
{
	Bool found = false;
	sb->Append((const UTF8Char*)"Params [");
	if (this->iss != 0)
	{
		if (found) sb->Append((const UTF8Char*)", ");
		sb->Append((const UTF8Char*)"iss=");
		sb->Append(this->iss);
		found = true;
	}
	if (this->sub != 0)
	{
		if (found) sb->Append((const UTF8Char*)", ");
		sb->Append((const UTF8Char*)"sub=");
		sb->Append(this->sub);
		found = true;
	}
	if (this->aud != 0)
	{
		if (found) sb->Append((const UTF8Char*)", ");
		sb->Append((const UTF8Char*)"aud=");
		sb->Append(this->aud);
		found = true;
	}
	if (this->exp != 0)
	{
		if (found) sb->Append((const UTF8Char*)", ");
		sb->Append((const UTF8Char*)"exp=");
		sb->AppendI64(this->exp);
		found = true;
	}
	if (this->nbf != 0)
	{
		if (found) sb->Append((const UTF8Char*)", ");
		sb->Append((const UTF8Char*)"nbf=");
		sb->AppendI64(this->nbf);
		found = true;
	}
	if (this->iat != 0)
	{
		if (found) sb->Append((const UTF8Char*)", ");
		sb->Append((const UTF8Char*)"iat=");
		sb->AppendI64(this->iat);
		found = true;
	}
	if (this->jti != 0)
	{
		if (found) sb->Append((const UTF8Char*)", ");
		sb->Append((const UTF8Char*)"jti=");
		sb->Append(this->jti);
		found = true;
	}
	sb->Append((const UTF8Char*)"]");
}
