#include "Stdafx.h"
#include "Core/Core.h"
#include "Data/ByteTool.h"
#include "Crypto/Hash/HashCreator.h"

Int32 MyMain(NotNullPtr<Core::IProgControl> progCtrl)
{
	UInt8 testBlock[32];
	UInt8 hashVal[32];
	Crypto::Hash::IHash *hash;

	hash = Crypto::Hash::HashCreator::CreateHash(Crypto::Hash::HashType::CRC32C);

	UOSInt i = 32;
	while (i-- > 0)
	{
		testBlock[i] = 0;
	}
	hash->Clear();
	hash->Calc(testBlock, 32);
	hash->GetValue(hashVal);
	if (ReadMUInt32(hashVal) != 0x8A9136AA)
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
	if (ReadMUInt32(hashVal) != 0x62A8AB43)
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
	if (ReadMUInt32(hashVal) != 0x113FDB5C)
	{
		DEL_CLASS(hash);
		return 1;
	}

	DEL_CLASS(hash);
	return 0;
}
