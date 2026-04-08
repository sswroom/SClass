#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Crypto/Hash/Adler32.h"
#include "IO/Console.h"
#include "Text/MyString.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	UInt8 testBlock[] = "Wikipedia";
	UInt8 hashVal[32];
	UTF8Char sbuff[65];
	Crypto::Hash::HashAlgorithm *hash;

	NEW_CLASS(hash, Crypto::Hash::Adler32());

	hash->GetValue(hashVal);
	Text::StrHexBytes(sbuff, hashVal, 4, 0);
	IO::Console::PrintStrO(sbuff);
	IO::Console::PrintStrO(U8STR("\n00000001\n\n"));

	hash->Calc(testBlock, sizeof(testBlock) - 1);
	hash->GetValue(hashVal);
	Text::StrHexBytes(sbuff, hashVal, 4, 0);
	IO::Console::PrintStrO(sbuff);
	IO::Console::PrintStrO(U8STR("\n11E60398\n"));

	DEL_CLASS(hash);
	return 0;
}
