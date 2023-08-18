#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Net/OSSocketFactory.h"
#include "Net/SSLEngineFactory.h"
#include "Parser/FileParser/X509Parser.h"

#include <stdio.h>

const Char key[] = 
	"-----BEGIN EC PRIVATE KEY-----\r\n"
	"MIGkAgEBBDBNK0jwKqqf8zkM+Z2l++9r8bzdTS/XCoB4N1J07dPxpByyJyGbhvIy\r\n"
	"1kLvY2gIvlmgBwYFK4EEACKhZANiAAQvPxAK2RhvH/k5inDa9oMxUZPvvb9fq8G3\r\n"
	"9dKW1tS+ywhejnKeu/48HXAXgx2g6qMJjEPpcTy/DaYm12r3GTaRzOBQmxSItStk\r\n"
	"lpQg5vf23Fc9fFrQ9AnQKrb1dgTkoxQ=\r\n"
	"-----END EC PRIVATE KEY-----\r\n";

Int32 MyMain(NotNullPtr<Core::IProgControl> progCtrl)
{
	Int32 ret = 1;
	UInt8 signData[2048];
	UOSInt signLen;
	Net::OSSocketFactory sockf(true);
	Net::SSLEngine *ssl = Net::SSLEngineFactory::Create(sockf, true);
	Crypto::Cert::X509File *x509 = 0;
	if (ssl)
	{
		NotNullPtr<Text::String> fileName = Text::String::New(UTF8STRC("Temp.key"));
		x509 = Parser::FileParser::X509Parser::ParseBuff(Data::ByteArrayR((const UInt8*)key, sizeof(key) - 1), fileName);
		fileName->Release();
	}
	NotNullPtr<Crypto::Cert::X509Key> key;
	if (x509 && x509->GetFileType() == Crypto::Cert::X509File::FileType::Key && key.Set((Crypto::Cert::X509Key*)x509))
	{
		if (!ssl->Signature(key, Crypto::Hash::HashType::SHA256, UTF8STRC("123456"), signData, &signLen))
		{
			printf("Error in generating signature\r\n");
		}
		else
		{
			Text::StringBuilderUTF8 sb;
			sb.AppendHexBuff(signData, signLen, ' ', Text::LineBreakType::CRLF);
			printf("Signature:\r\n%s\r\n", sb.ToString());
			
			if (ssl->SignatureVerify(key, Crypto::Hash::HashType::SHA256, UTF8STRC("123456"), signData, signLen))
			{
				printf("Signature verify success\r\n");
				ret = 0;
			}
			else
			{
				printf("Error in verifying signature, len = %d\r\n", (UInt32)signLen);
			}
		}
	}
	else
	{
		printf("Error in parsing private key\r\n");
	}
	SDEL_CLASS(x509);
	SDEL_CLASS(ssl);
	return ret;
}
