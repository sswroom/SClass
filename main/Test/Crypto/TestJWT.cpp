#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Crypto/Cert/X509PubKey.h"
#include "Crypto/Cert/X509PrivKey.h"
#include "Crypto/Token/JWTHandler.h"
#include "Crypto/Token/JWToken.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "Net/OSSocketFactory.h"
#include "Net/SSLEngineFactory.h"
#include "Parser/FileParser/X509Parser.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/TextBinEnc/Base64Enc.h"

Int32 MyMain(NotNullPtr<Core::IProgControl> progCtrl)
{
	Text::CStringNN keyB64 = CSTR("MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAu1SU1LfVLPHCozMxH2Mo4lgOEePzNm0tRgeLezV6ffAt0gunVTLw7onLRnrq0/IzW7yWR7QkrmBL7jTKEn5u+qKhbwKfBstIs+bMY2Zkp18gnTxKLxoS2tFczGkPLPgizskuemMghRniWaoLcyehkd3qqGElvW/VDL5AaWTg0nLVkjRo9z+40RQzuVaE8AkAFmxZzow3x+VJYKdjykkJ0iT9wCS0DRTXu269V264Vf/3jvredZiKRkgwlL9xNAwxXFg0x/XFw005UWVRIkdgcKWTjpBP2dPwVZ4WWC+9aGVd+Gyn1o0CLelf4rEjGoXbAAEgAqeGUxrcIlbjXfbcmwIDAQAB");
	Text::StringBuilderUTF8 sb;
	Crypto::Token::JWTHandler *jwt;
	Net::OSSocketFactory sockf(false);
	Optional<Net::SSLEngine> ssl = Net::SSLEngineFactory::Create(sockf, true);
	NEW_CLASS(jwt, Crypto::Token::JWTHandler(ssl, Crypto::Token::JWSignature::Algorithm::HS256, UTF8STRC("your-256-bit-secret"), Crypto::Cert::X509Key::KeyType::Unknown));

	Crypto::Token::JWTParam param;
	NotNullPtr<Text::String> s = Text::String::New(UTF8STRC("1234567890"));
	param.SetSubject(s.Ptr());
	s->Release();
	param.SetIssuedAt(1516239022);
	Data::StringMap<const UTF8Char*> payload;
	payload.Put(CSTR("name"), (const UTF8Char*)"John Doe");
	jwt->Generate(sb, &payload, &param);
	if (!sb.Equals(UTF8STRC("eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJuYW1lIjoiSm9obiBEb2UiLCJzdWIiOiIxMjM0NTY3ODkwIiwiaWF0IjoxNTE2MjM5MDIyfQ.3uGPWYtY_HtIcBGz4eUmTtcjZ4HnJZK9Z2uhx0Ks4n8")))
	{
		DEL_CLASS(jwt);
		ssl.Delete();
		return 1;	
	}
	DEL_CLASS(jwt);
	
	Crypto::Token::JWToken *token = Crypto::Token::JWToken::Parse(CSTR("eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJuYW1lIjoiSm9obiBEb2UiLCJzdWIiOiIxMjM0NTY3ODkwIiwiaWF0IjoxNTE2MjM5MDIyfQ.3uGPWYtY_HtIcBGz4eUmTtcjZ4HnJZK9Z2uhx0Ks4n8"), 0);
	if (token == 0)
	{
		ssl.Delete();
		return 2;
	}
	if (!token->SignatureValid(ssl, UTF8STRC("your-256-bit-secret"), Crypto::Cert::X509Key::KeyType::Unknown))
	{
		DEL_CLASS(token);
		ssl.Delete();
		return 3;
	}
	Data::StringMap<Text::String*> *result = token->ParsePayload(param, false, 0);
	if (result == 0)
	{
		DEL_CLASS(token);
		ssl.Delete();
		return 4;
	}
	if (!s.Set(result->Get(CSTR("name"))) || !s->Equals(UTF8STRC("John Doe")))
	{
		token->FreeResult(result);
		DEL_CLASS(token);
		ssl.Delete();
		return 5;
	}
	if (!param.GetSubject().SetTo(s) || !s->Equals(UTF8STRC("1234567890")))
	{
		token->FreeResult(result);
		DEL_CLASS(token);
		ssl.Delete();
		return 6;
	}
	if (param.GetIssuedAt() != 1516239022)
	{
		token->FreeResult(result);
		DEL_CLASS(token);
		ssl.Delete();
		return 7;
	}
	token->FreeResult(result);
	DEL_CLASS(token);

	Text::TextBinEnc::Base64Enc b64;
	UInt8 keyBuff[4096];
	UOSInt keySize;
	keySize = b64.DecodeBin(keyB64.v, keyB64.leng, keyBuff);
	if (keySize == 0)
	{
		ssl.Delete();
		return 8;
	}
	else
	{
		Optional<Crypto::Cert::X509Key> key = 0;
		NotNullPtr<Crypto::Cert::X509Key> nnkey;
		NotNullPtr<Text::String> s = Text::String::New(CSTR("JWTRSAPub.key"));
		Crypto::Cert::X509File *x509;// = Parser::FileParser::X509Parser::ParseBuff(Data::ByteArray(keyBuff, keySize), s);
		NEW_CLASS(x509, Crypto::Cert::X509PubKey(s, Data::ByteArrayR(keyBuff, keySize)));
		s->Release();
		if (x509 == 0)
		{
			ssl.Delete();
			return 9;
		}
		else
		{
			if (x509->GetFileType() == Crypto::Cert::X509File::FileType::PrivateKey)
			{
				Crypto::Cert::X509PrivKey *privKey = (Crypto::Cert::X509PrivKey*)x509;
				key = privKey->CreateKey();
			}
			else if (x509->GetFileType() == Crypto::Cert::X509File::FileType::PublicKey)
			{
				Crypto::Cert::X509PubKey *pubKey = (Crypto::Cert::X509PubKey*)x509;
				key = pubKey->CreateKey();
			}
			else if (x509->GetFileType() == Crypto::Cert::X509File::FileType::Key)
			{
				key = (Crypto::Cert::X509Key*)x509->Clone().Ptr();
			}
			else if (x509->GetFileType() == Crypto::Cert::X509File::FileType::Cert)
			{
				Crypto::Cert::X509Cert *cert = (Crypto::Cert::X509Cert*)x509;
				key = cert->GetNewPublicKey();
			}
			if (key.IsNull())
			{
				DEL_CLASS(x509);
				ssl.Delete();
				return 10;
			}
			DEL_CLASS(x509);
		}
		if (key.SetTo(nnkey) && (nnkey->GetKeyType() == Crypto::Cert::X509Key::KeyType::RSA || nnkey->GetKeyType() == Crypto::Cert::X509Key::KeyType::RSAPublic))
		{
			token = Crypto::Token::JWToken::Parse(CSTR("eyJhbGciOiJSUzI1NiIsInR5cCI6IkpXVCJ9.eyJzdWIiOiIxMjM0NTY3ODkwIiwibmFtZSI6IkpvaG4gRG9lIiwiYWRtaW4iOnRydWUsImlhdCI6MTUxNjIzOTAyMn0.NHVaYe26MbtOYhSKkoKYdFVomg4i8ZJd8_-RU8VNbftc4TSMb4bXP3l3YlNWACwyXPGffz5aXHc6lty1Y2t4SWRqGteragsVdZufDn5BlnJl9pdR_kdVFUsra2rWKEofkZeIC4yWytE58sMIihvo9H1ScmmVwBcQP6XETqYd0aSHp1gOa9RdUPDvoXQ5oqygTqVtxaDr6wUFKrKItgBMzWIdNZ6y7O9E0DhEPTbE9rfBo6KTFsHAZnMg4k68CDp2woYIaXbmYTWcvbzIuHO7_37GT79XdIwkm95QJ7hYC9RiwrV7mesbY4PAahERJawntho0my942XheVLmGwLMBkQ"), 0);
			if (token)
			{
				if (!token->SignatureValid(ssl, nnkey->GetASN1Buff(), nnkey->GetASN1BuffSize(), nnkey->GetKeyType()))
				{
					DEL_CLASS(token);
					key.Delete();
					ssl.Delete();
					return 11;
				}
				DEL_CLASS(token);
			}
		}
		key.Delete();
	}
	ssl.Delete();
	return 0;
}
