#include "Stdafx.h"
#include "Core/Core.h"
#include "Data/ByteTool.h"
#include "Crypto/Hash/Adler32.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	UInt8 testBlock[32];
	UInt8 hashVal[32];
	Crypto::Hash::IHash *hash;

	NEW_CLASS(hash, Crypto::Hash::Adler32());

	UOSInt i = 32;
	while (i-- > 0)
	{
		testBlock[i] = 0;
	}
	hash->Clear();
	hash->Calc(testBlock, 32);
	hash->GetValue(hashVal);
	if (ReadMUInt32(hashVal) != 0x00200001)
	{
		DEL_CLASS(hash);
		return 1;
	}

	i = 32;
	while (i-- > 0)
	{
		testBlock[i] = 0xFF;
	}
	hash->Clear();
	hash->Calc(testBlock, 32);
	hash->GetValue(hashVal);
	if (ReadMUInt32(hashVal) != 0x0E2E1FE1)
	{
		DEL_CLASS(hash);
		return 1;
	}

	i = 32;
	while (i-- > 0)
	{
		testBlock[i] = (UInt8)(31 - i);
	}
	hash->Clear();
	hash->Calc(testBlock, 32);
	hash->GetValue(hashVal);
	if (ReadMUInt32(hashVal) != 0x2AC001F1)
	{
		DEL_CLASS(hash);
		return 1;
	}

	hash->Clear();
	hash->Calc(testBlock, 31);
	hash->GetValue(hashVal);
	if (ReadMUInt32(hashVal) != 0x28CF01F1)
	{
		DEL_CLASS(hash);
		return 1;
	}

	DEL_CLASS(hash);
	return 0;
}
