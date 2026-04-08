#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Crypto/Encrypt/Base64.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	Crypto::Encrypt::Base64 *b64;
	Text::StringBuilderUTF8 sb;
	UInt8 decBuff[256];
	UTF8Char sendBuff[640];
	UTF8Char recvBuff[256];
	UIntOS decSize;
	UInt8 buff[] = "BgBbhVNOBAAAAAwAAACvvFANx8YJRAoAAAAyAI7XUA08lwlECgAAADIAQhZRDXCdCUQKAAAAMgCeK1ENNN4JRAoAAAAyAFMYUQ2eKApECgAAADIA+s9QDU8pCkQKAAAAMgDbmFAN5v0JRAoAAAAyAIuZUA2prglECgAAADIA+LBQDXl1CUQKAAAAMgBA91ANoGUJRAoAAAAyAMsmUQ00bQlECgAAADIA/7tQDQHYCUQKAAAAMgA=";
	UInt8 buff2[] = "AAAAANsHCBcLKBpLwFAN3AAKRJg6AAA=";
	NEW_CLASS(b64, Crypto::Encrypt::Base64());
	decSize = b64->Decrypt(buff, sizeof(buff) - 1, decBuff);
	Text::StrHexBytes(sendBuff, decBuff, decSize, ' ');
	sb.AppendC(UTF8STRC("Send Buff:\r\n"));
	sb.AppendHexBuff(Data::ByteArrayR(decBuff, decSize), ' ', Text::LineBreakType::CRLF);
	printf("%s\r\n\r\n", sb.ToString().Ptr());
	decSize = b64->Decrypt(buff2, sizeof(buff2) - 1, decBuff);
	Text::StrHexBytes(recvBuff, decBuff, decSize, ' ');
	sb.ClearStr();
	sb.AppendC(UTF8STRC("Recv Buff:\r\n"));
	sb.AppendHexBuff(Data::ByteArrayR(decBuff, decSize), ' ', Text::LineBreakType::CRLF);
	printf("%s\r\n\r\n", sb.ToString().Ptr());

	DEL_CLASS(b64);
	return 0;
}
