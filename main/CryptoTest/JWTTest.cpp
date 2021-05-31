#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Crypto/Token/JSONWebToken.h"
#include "IO/ConsoleWriter.h"
#include "Text/StringBuilderUTF8.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	IO::ConsoleWriter console;
	Text::StringBuilderUTF8 sb;
	Crypto::Token::JSONWebToken::Generate(&sb, Crypto::Token::JSONWebToken::HS256, (const UTF8Char*)"{\"sub\":\"1234567890\",\"name\":\"John Doe\",\"iat\":1516239022}", (const UInt8*)"your-256-bit-secret", 19);
	console.WriteLine(sb.ToString());
	return 0;
}
