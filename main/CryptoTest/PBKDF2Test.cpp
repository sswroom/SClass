#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Crypto/PBKDF2.h"
#include "Crypto/Hash/HMAC.h"
#include "Crypto/Hash/SHA1.h"
#include "IO/ConsoleWriter.h"
#include "Text/StringBuilderUTF8.h"

void Test(IO::ConsoleWriter *console, const Char *pwd, const Char *salt, UOSInt cnt, UOSInt dkLen)
{
	UInt8 *dk = MemAlloc(UInt8, dkLen);
	Crypto::Hash::HMAC *hmac;
	Crypto::Hash::SHA1 *sha1;
	NEW_CLASS(sha1, Crypto::Hash::SHA1());
	NEW_CLASS(hmac, Crypto::Hash::HMAC(sha1, (const UTF8Char*)pwd, Text::StrCharCnt(pwd)));
	Crypto::PBKDF2::Calc((const UInt8*)salt, Text::StrCharCnt(salt), cnt, dkLen, hmac, dk);
	console->WriteStrC(UTF8STRC("Output =  "));
	Text::StringBuilderUTF8 sb;
	sb.AppendHexBuff(dk, dkLen, 0, Text::LineBreakType::None);
	console->WriteLineC(sb.ToString(), sb.GetLength());
	DEL_CLASS(hmac);
	DEL_CLASS(sha1);
	MemFree(dk);
}

Int32 MyMain(Core::IProgControl *progCtrl)
{
	Text::StringBuilderUTF8 sb;
	IO::ConsoleWriter console;

	//rfc6070
	Test(&console, "password", "salt", 1, 20);
	console.WriteLineC(UTF8STRC("Expected: 0c60c80f961f0e71f3a9b524af6012062fe037a6"));
	console.WriteLine();

	Test(&console, "password", "salt", 2, 20);
	console.WriteLineC(UTF8STRC("Expected: ea6c014dc72d6f8ccd1ed92ace1d41f0d8de8957"));
	console.WriteLine();

	Test(&console, "password", "salt", 4096, 20);
	console.WriteLineC(UTF8STRC("Expected: 4b007901b765489abead49d926f721d065a429c1"));
	console.WriteLine();

	Test(&console, "password", "salt", 16777216, 20);
	console.WriteLineC(UTF8STRC("Expected: eefe3d61cd4da4e4e9945b3d6ba2158c2634e984"));
	console.WriteLine();

	Test(&console, "passwordPASSWORDpassword", "saltSALTsaltSALTsaltSALTsaltSALTsalt", 4096, 25);
	console.WriteLineC(UTF8STRC("Expected: 3d2eec4fe41c849b80c8d83662c0e44a8b291a964cf2f07038"));
	console.WriteLine();
	return 0;
}
