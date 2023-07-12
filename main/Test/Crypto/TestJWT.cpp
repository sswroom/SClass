#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Crypto/Cert/X509PrivKey.h"
#include "Crypto/Token/JWTHandler.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "Net/OSSocketFactory.h"
#include "Net/SSLEngineFactory.h"
#include "Parser/FileParser/X509Parser.h"
#include "Text/StringBuilderUTF8.h"

Int32 MyMain(NotNullPtr<Core::IProgControl> progCtrl)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	Text::StringBuilderUTF8 sb;
	Crypto::Token::JWTHandler *jwt;
	Net::SocketFactory *sockf;
	NEW_CLASS(sockf, Net::OSSocketFactory(false));
	Net::SSLEngine *ssl = Net::SSLEngineFactory::Create(sockf, true);
	NEW_CLASS(jwt, Crypto::Token::JWTHandler(ssl, Crypto::Token::JWSignature::Algorithm::HS256, UTF8STRC("your-256-bit-secret")));

	Crypto::Token::JWTParam param;
	Text::String *s = Text::String::New(UTF8STRC("1234567890"));
	param.SetSubject(s);
	s->Release();
	param.SetIssuedAt(1516239022);
	Data::StringMap<const UTF8Char*> payload;
	payload.Put(CSTR("name"), (const UTF8Char*)"John Doe");
	jwt->Generate(&sb, &payload, &param);
	if (!sb.Equals(UTF8STRC("eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJuYW1lIjoiSm9obiBEb2UiLCJzdWIiOiIxMjM0NTY3ODkwIiwiaWF0IjoxNTE2MjM5MDIyfQ.3uGPWYtY_HtIcBGz4eUmTtcjZ4HnJZK9Z2uhx0Ks4n8")))
	{
		DEL_CLASS(jwt);
		SDEL_CLASS(ssl);
		DEL_CLASS(sockf);
		return 1;	
	}
	
	Data::StringMap<Text::String*> *result = jwt->Parse((const UTF8Char*)"eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJuYW1lIjoiSm9obiBEb2UiLCJzdWIiOiIxMjM0NTY3ODkwIiwiaWF0IjoxNTE2MjM5MDIyfQ.3uGPWYtY_HtIcBGz4eUmTtcjZ4HnJZK9Z2uhx0Ks4n8", &param);
	s = result->Get(CSTR("name"));
	if (s == 0 || !s->Equals(UTF8STRC("John Doe")))
	{
		jwt->FreeResult(result);
		DEL_CLASS(jwt);
		SDEL_CLASS(ssl);
		DEL_CLASS(sockf);
		return 1;
	}
	s = param.GetSubject();
	if (s == 0 || !s->Equals(UTF8STRC("1234567890")))
	{
		jwt->FreeResult(result);
		DEL_CLASS(jwt);
		SDEL_CLASS(ssl);
		DEL_CLASS(sockf);
		return 1;
	}
	if (param.GetIssuedAt() != 1516239022)
	{
		jwt->FreeResult(result);
		DEL_CLASS(jwt);
		SDEL_CLASS(ssl);
		DEL_CLASS(sockf);
		return 1;
	}
	jwt->FreeResult(result);
	DEL_CLASS(jwt);

	sptr = IO::Path::GetProcessFileName(sbuff);
	sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("jwtrsa.key"));
	if (IO::Path::GetPathType(CSTRP(sbuff, sptr)) == IO::Path::PathType::File)
	{
		UInt8 keyBuff[4096];
		UOSInt keySize;
		keySize = IO::FileStream::LoadFile(CSTRP(sbuff, sptr), keyBuff, 4096);
		if (keySize == 0)
		{
			SDEL_CLASS(ssl);
			DEL_CLASS(sockf);
			return 1;
		}
		else
		{
			Crypto::Cert::X509Key *key = 0;
			Text::String *s = Text::String::New(sbuff, (UOSInt)(sptr - sbuff));
			Crypto::Cert::X509File *x509 = Parser::FileParser::X509Parser::ParseBuff(keyBuff, keySize, s);
			s->Release();
			if (x509 == 0)
			{
				SDEL_CLASS(ssl);
				DEL_CLASS(sockf);
				return 1;
			}
			else
			{
				if (x509->GetFileType() == Crypto::Cert::X509File::FileType::PrivateKey)
				{
					Crypto::Cert::X509PrivKey *privKey = (Crypto::Cert::X509PrivKey*)x509;
					key = privKey->CreateKey();
				}
				else if (x509->GetFileType() == Crypto::Cert::X509File::FileType::Key)
				{
					key = (Crypto::Cert::X509Key*)x509->Clone();
				}
				if (key == 0)
				{
					DEL_CLASS(x509);
					SDEL_CLASS(ssl);
					DEL_CLASS(sockf);
					return 1;
				}
				DEL_CLASS(x509);
			}
			if (key && key->GetKeyType() == Crypto::Cert::X509Key::KeyType::RSA)
			{
				NEW_CLASS(jwt, Crypto::Token::JWTHandler(ssl, Crypto::Token::JWSignature::Algorithm::RS256, key->GetASN1Buff(), key->GetASN1BuffSize()));
				Data::StringMap<Text::String*> *result = jwt->Parse((const UTF8Char*)"eyJhbGciOiJSUzI1NiIsInR5cCI6IkpXVCJ9.eyJzdWIiOiIxMjM0NTY3ODkwIiwibmFtZSI6IkpvaG4gRG9lIiwiYWRtaW4iOnRydWUsImlhdCI6MTUxNjIzOTAyMn0.NHVaYe26MbtOYhSKkoKYdFVomg4i8ZJd8_-RU8VNbftc4TSMb4bXP3l3YlNWACwyXPGffz5aXHc6lty1Y2t4SWRqGteragsVdZufDn5BlnJl9pdR_kdVFUsra2rWKEofkZeIC4yWytE58sMIihvo9H1ScmmVwBcQP6XETqYd0aSHp1gOa9RdUPDvoXQ5oqygTqVtxaDr6wUFKrKItgBMzWIdNZ6y7O9E0DhEPTbE9rfBo6KTFsHAZnMg4k68CDp2woYIaXbmYTWcvbzIuHO7_37GT79XdIwkm95QJ7hYC9RiwrV7mesbY4PAahERJawntho0my942XheVLmGwLMBkQ", &param);
				jwt->FreeResult(result);

				DEL_CLASS(jwt);
			}
			SDEL_CLASS(key);
		}
	}
	SDEL_CLASS(ssl);
	DEL_CLASS(sockf);
	return 0;
}
