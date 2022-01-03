#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Crypto/Cert/X509PrivKey.h"
#include "Crypto/Token/JWTHandler.h"
#include "IO/ConsoleWriter.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "Net/DefaultSSLEngine.h"
#include "Net/OSSocketFactory.h"
#include "Parser/FileParser/X509Parser.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/StringTool.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	UTF8Char sbuff[512];
	IO::ConsoleWriter console;
	Text::StringBuilderUTF8 sb;
	Crypto::Token::JWTHandler *jwt;
	Net::SocketFactory *sockf;
	NEW_CLASS(sockf, Net::OSSocketFactory(false));
	Net::SSLEngine *ssl = Net::DefaultSSLEngine::Create(sockf, true);
	NEW_CLASS(jwt, Crypto::Token::JWTHandler(ssl, Crypto::Token::JWSignature::Algorithm::HS256, (const UInt8*)"your-256-bit-secret", 19));

	Crypto::Token::JWTParam param;
	Text::String *s = Text::String::NewNotNull((const UTF8Char*)"1234567890");
	param.SetSubject(s);
	s->Release();
	param.SetIssuedAt(1516239022);
	Data::StringUTF8Map<const UTF8Char*> payload;
	payload.Put((const UTF8Char*)"name", (const UTF8Char*)"John Doe");
	jwt->Generate(&sb, &payload, &param);
	console.WriteLine(sb.ToString());
	console.WriteLine((const UTF8Char*)"eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJuYW1lIjoiSm9obiBEb2UiLCJzdWIiOiIxMjM0NTY3ODkwIiwiaWF0IjoxNTE2MjM5MDIyfQ.3uGPWYtY_HtIcBGz4eUmTtcjZ4HnJZK9Z2uhx0Ks4n8");
	
	Data::StringUTF8Map<Text::String*> *result = jwt->Parse((const UTF8Char*)"eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJuYW1lIjoiSm9obiBEb2UiLCJzdWIiOiIxMjM0NTY3ODkwIiwiaWF0IjoxNTE2MjM5MDIyfQ.3uGPWYtY_HtIcBGz4eUmTtcjZ4HnJZK9Z2uhx0Ks4n8", &param);
	sb.ClearStr();
	sb.AppendC(UTF8STRC("Payload = "));
	Text::StringTool::BuildString(&sb, result);
	console.WriteLine(sb.ToString());
	sb.ClearStr();
	sb.AppendC(UTF8STRC("Params = "));
	param.ToString(&sb);
	console.WriteLine(sb.ToString());

	jwt->FreeResult(result);

	DEL_CLASS(jwt);

	IO::Path::GetProcessFileName(sbuff);
	IO::Path::AppendPath(sbuff, (const UTF8Char*)"jwtrsa.key");
	if (IO::Path::GetPathType(sbuff) == IO::Path::PathType::File)
	{
		UInt8 keyBuff[4096];
		UOSInt keySize;
		keySize = IO::FileStream::LoadFile(sbuff, keyBuff, 4096);
		if (keySize == 0)
		{
			console.WriteLine((const UTF8Char*)"Error in loading jwtrsa.key file");
		}
		else
		{
			Crypto::Cert::X509Key *key = 0;
			Text::String *s = Text::String::NewNotNull(sbuff);
			Crypto::Cert::X509File *x509 = Parser::FileParser::X509Parser::ParseBuff(keyBuff, keySize, s);
			s->Release();
			if (x509 == 0)
			{
				console.WriteLine((const UTF8Char*)"Error in parsing jwtrsa.key file");
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
					console.WriteLine((const UTF8Char*)"jwtrsa.key is not a key file");
				}
				DEL_CLASS(x509);
			}
			if (key && key->GetKeyType() == Crypto::Cert::X509Key::KeyType::RSA)
			{
				NEW_CLASS(jwt, Crypto::Token::JWTHandler(ssl, Crypto::Token::JWSignature::Algorithm::RS256, key->GetASN1Buff(), key->GetASN1BuffSize()));
				Data::StringUTF8Map<Text::String*> *result = jwt->Parse((const UTF8Char*)"eyJhbGciOiJSUzI1NiIsInR5cCI6IkpXVCJ9.eyJzdWIiOiIxMjM0NTY3ODkwIiwibmFtZSI6IkpvaG4gRG9lIiwiYWRtaW4iOnRydWUsImlhdCI6MTUxNjIzOTAyMn0.NHVaYe26MbtOYhSKkoKYdFVomg4i8ZJd8_-RU8VNbftc4TSMb4bXP3l3YlNWACwyXPGffz5aXHc6lty1Y2t4SWRqGteragsVdZufDn5BlnJl9pdR_kdVFUsra2rWKEofkZeIC4yWytE58sMIihvo9H1ScmmVwBcQP6XETqYd0aSHp1gOa9RdUPDvoXQ5oqygTqVtxaDr6wUFKrKItgBMzWIdNZ6y7O9E0DhEPTbE9rfBo6KTFsHAZnMg4k68CDp2woYIaXbmYTWcvbzIuHO7_37GT79XdIwkm95QJ7hYC9RiwrV7mesbY4PAahERJawntho0my942XheVLmGwLMBkQ", &param);
				sb.ClearStr();
				sb.AppendC(UTF8STRC("Payload = "));
				Text::StringTool::BuildString(&sb, result);
				console.WriteLine(sb.ToString());
				sb.ClearStr();
				sb.AppendC(UTF8STRC("Params = "));
				param.ToString(&sb);
				console.WriteLine(sb.ToString());
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
