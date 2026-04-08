#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Crypto/Hash/HMAC.h"
#include "Crypto/Hash/SHA1.h"
#include "IO/Console.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	UTF8Char sbuff[128];
	UnsafeArray<UTF8Char> sptr;
    UInt8 Message_Digest[20];
	Text::StringBuilderUTF8 sb;

	Crypto::Hash::SHA1 sha;
	Crypto::Hash::HMAC hmac(sha, (UInt8*)"secret", 6);
	hmac.Calc((UInt8*)"The quick brown fox jumped over the lazy dog.", 45);
	hmac.GetValue(Message_Digest);
	sptr = Text::StrHexBytes(sbuff, Message_Digest, 20, 0);
	sb.AppendToLower(sbuff, (UIntOS)(sptr - sbuff));
	IO::Console::PrintStrO(sb.ToString());

	return 0;
}
