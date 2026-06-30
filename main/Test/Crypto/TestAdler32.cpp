#include "Stdafx.h"
#include "Core/Core.h"
#include "Core/ByteTool_C.h"
#include "Crypto/Hash/Adler32.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	UInt8 testBlock[32];
	UInt8 hashVal[32];
	NN<Crypto::Hash::HashAlgorithm> hash;

	NEW_CLASSNN(hash, Crypto::Hash::Adler32());

	UIntOS i = 32;
	while (i-- > 0)
	{
		testBlock[i] = 0;
	}
	hash->Clear();
	hash->Calc(testBlock, 32);
	hash->GetValue(hashVal);
	if (ReadMUInt32(hashVal) != 0x00200001)
	{
		hash.Delete();
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
		hash.Delete();
		return 2;
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
		hash.Delete();
		return 3;
	}

	hash->Clear();
	hash->Calc(testBlock, 31);
	hash->GetValue(hashVal);
	if (ReadMUInt32(hashVal) != 0x28CF01F1)
	{
		hash.Delete();
		return 4;
	}

	hash.Delete();
	return 0;
}
