#include "Stdafx.h"
#include "Core/Core.h"
#include "Data/ByteTool.h"
#include "Crypto/Hash/HashCreator.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	UInt8 testBlock[32];
	UInt8 hashVal[32];
	Crypto::Hash::IHash *hash;

	hash = Crypto::Hash::HashCreator::CreateHash(Crypto::Hash::HT_CRC16R);

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
	if (ReadMUInt16(hashVal) != 0xCFFF)
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
	if (ReadMUInt16(hashVal) != 0x5AC3)
	{
		DEL_CLASS(hash);
		return 1;
	}

	DEL_CLASS(hash);
	return 0;
}
