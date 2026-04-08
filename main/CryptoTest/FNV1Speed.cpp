#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Crypto/Hash/FNV1.h"
#include "Manage/HiResClock.h"

#define BLOCKSIZE 1048576
#define REPCOUNT 1024

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	Manage::HiResClock *clk;
	UInt8 *testBlock;
	UInt8 hashVal[32];
	Double t;
	IntOS i;
	Crypto::Hash::HashAlgorithm *hash;

	NEW_CLASS(clk, Manage::HiResClock());
	NEW_CLASS(hash, Crypto::Hash::FNV1());
	testBlock = MemAlloc(UInt8, BLOCKSIZE);
	clk->Start();
	i = REPCOUNT;
	while (i-- > 0)
	{
		hash->Calc(testBlock, BLOCKSIZE);
	}
	hash->GetValue(hashVal);
	t = REPCOUNT / clk->GetTimeDiff();
	MemFree(testBlock);
	DEL_CLASS(hash);
	DEL_CLASS(clk);
	printf("Speed: %lf MiB/s\r\n", t);
	return 0;
}
