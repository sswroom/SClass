#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Crypto/Hash/HashCreator.h"
#include "IO/ConsoleWriter.h"
#include "Text/MyString.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	UInt8 testBlock[32];
	UInt8 hashVal[32];
	UTF8Char sbuff[65];
	Crypto::Hash::IHash *hash;
	IO::ConsoleWriter console;

	hash = Crypto::Hash::HashCreator::CreateHash(Crypto::Hash::HT_CRC32C);

	UOSInt i = 32;
	while (i-- > 0)
	{
		testBlock[i] = 0;
	}
	hash->Clear();
	hash->Calc(testBlock, 32);
	hash->GetValue(hashVal);
	Text::StrHexBytes(sbuff, hashVal, 4, 0);
	console.WriteLine(sbuff);
	console.WriteLine((const UTF8Char*)"8A9136AA");
	console.WriteLine();

	i = 32;
	while (i-- > 0)
	{
		testBlock[i] = 0xFF;
	}
	hash->Clear();
	hash->Calc(testBlock, 32);
	hash->GetValue(hashVal);
	Text::StrHexBytes(sbuff, hashVal, 4, 0);
	console.WriteLine(sbuff);
	console.WriteLine((const UTF8Char*)"62A8AB43");
	console.WriteLine();

	i = 32;
	while (i-- > 0)
	{
		testBlock[i] = (UInt8)(31 - i);
	}
	hash->Clear();
	hash->Calc(testBlock, 32);
	hash->GetValue(hashVal);
	Text::StrHexBytes(sbuff, hashVal, 4, 0);
	console.WriteLine(sbuff);
	console.WriteLine((const UTF8Char*)"113FDB5C");
	console.WriteLine();

	DEL_CLASS(hash);
	return 0;
}