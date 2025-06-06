#include "Stdafx.h"
#include "Core/Core.h"
#include "Crypto/Hash/CRC32R.h"
#include "Data/ByteTool.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	UInt8 testBlock[] = "123456789";
	UInt8 testBlock2[] = "12345678901234567";
	UInt8 hashVal[32];
	Crypto::Hash::CRC32R hash;

	hash.GetValue(hashVal);
	if (ReadMUInt32(hashVal) != 0x00000000)
	{
		return 1;
	}

	hash.Calc(testBlock, sizeof(testBlock) - 1);
	hash.GetValue(hashVal);
	if (ReadMUInt32(hashVal) != 0xCBF43926)
	{
		return 1;
	}

	hash.Clear();
	hash.Calc(testBlock2, sizeof(testBlock2) - 1);
	hash.GetValue(hashVal);
	if (ReadMUInt32(hashVal) != 0x3FA43360)
	{
		return 1;
	}

	return 0;
}
