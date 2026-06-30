#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Crypto/Hash/SHA1.h"
#include "Manage/HiResClock.h"

#define BLOCKSIZE 1048576
#define REPCOUNT 1024

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	NN<Manage::HiResClock> clk;
	UnsafeArray<UInt8> testBlock;
	UInt8 hashVal[32];
	Double t;
	IntOS i;
	NN<Crypto::Hash::HashAlgorithm> hash;

	NEW_CLASSNN(clk, Manage::HiResClock());
	NEW_CLASSNN(hash, Crypto::Hash::SHA1());
	testBlock = MemAllocArr(UInt8, BLOCKSIZE);
	clk->Start();
	i = REPCOUNT;
	while (i-- > 0)
	{
		hash->Calc(testBlock, BLOCKSIZE);
	}
	hash->GetValue(hashVal);
	t = REPCOUNT / clk->GetTimeDiff();
	MemFreeArr(testBlock);
	hash.Delete();
	clk.Delete();
	printf("Speed: %lf (MiB/s)\r\n", t);
	return 0;
}
