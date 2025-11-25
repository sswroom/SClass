#include "Stdafx.h"
#include "Core/Core.h"
#include "Core/ByteTool_C.h"
#include "Crypto/Hash/HashCreator.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	UInt8 testBlock[32];
	UInt8 hashVal[32];
	NN<Crypto::Hash::HashAlgorithm> hash;

	if (!Crypto::Hash::HashCreator::CreateHash(Crypto::Hash::HashType::CRC16R).SetTo(hash))
		return 1;

	UOSInt i = 32;
	while (i-- > 0)
	{
		testBlock[i] = 0;
	}
	hash->Clear();
	hash->Calc(testBlock, 32);
	hash->GetValue(hashVal);
	if (ReadMUInt16(hashVal) != 0x6BFE)
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
	if (ReadMUInt16(hashVal) != 0xCFFF)
	{
		hash.Delete();
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
	if (ReadMUInt16(hashVal) != 0x5AC3)
	{
		hash.Delete();
		return 1;
	}

	hash.Delete();
	return 0;
}
