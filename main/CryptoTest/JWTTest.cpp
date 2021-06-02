#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Crypto/Token/JWTHandler.h"
#include "IO/ConsoleWriter.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/StringTool.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	IO::ConsoleWriter console;
	Text::StringBuilderUTF8 sb;
	Crypto::Token::JWTHandler *jwt = Crypto::Token::JWTHandler::CreateHMAC(Crypto::Token::JWTHandler::HS256, (const UInt8*)"your-256-bit-secret", 19);
	if (jwt)
	{
		Crypto::Token::JWTParam param;
		param.SetSubject((const UTF8Char*)"1234567890");
		param.SetIssuedAt(1516239022);
		Data::StringUTF8Map<const UTF8Char*> payload;
		payload.Put((const UTF8Char*)"name", (const UTF8Char*)"John Doe");
		jwt->Generate(&sb, &payload, &param);
		console.WriteLine(sb.ToString());
		console.WriteLine((const UTF8Char*)"eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJuYW1lIjoiSm9obiBEb2UiLCJzdWIiOiIxMjM0NTY3ODkwIiwiaWF0IjoxNTE2MjM5MDIyfQ.3uGPWYtY_HtIcBGz4eUmTtcjZ4HnJZK9Z2uhx0Ks4n8");
		
		Data::StringUTF8Map<const UTF8Char*> *result = jwt->Parse((const UTF8Char*)"eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJuYW1lIjoiSm9obiBEb2UiLCJzdWIiOiIxMjM0NTY3ODkwIiwiaWF0IjoxNTE2MjM5MDIyfQ.3uGPWYtY_HtIcBGz4eUmTtcjZ4HnJZK9Z2uhx0Ks4n8", &param);
		sb.ClearStr();
		sb.Append((const UTF8Char*)"Payload = ");
		Text::StringTool::BuildString(&sb, result);
		console.WriteLine(sb.ToString());
		sb.ClearStr();
		sb.Append((const UTF8Char*)"Params = ");
		param.ToString(&sb);
		console.WriteLine(sb.ToString());

		jwt->FreeResult(result);

		DEL_CLASS(jwt);
	}
	return 0;
}
