#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Manage/HiResClock.h"
#include "Text/MyString.h"
#include <memory.h>
#include <stdio.h>

#ifdef _WIN32_WCE
#define COPYCNT 0x7fff8
#else
#define COPYCNT 15
#endif
#define LOOPCNT 10485760

void mtest(UOSInt copyCnt)
{
	UInt8 *buff1;
	UInt8 *buff2;
	Manage::HiResClock *clk;
	UOSInt i;
	Double t1;
	Double t2;
	Double t3;
	Double t4;

	buff1 = MemAllocA64(UInt8, copyCnt);
	buff2 = MemAllocA64(UInt8, copyCnt);
	NEW_CLASS(clk, Manage::HiResClock());
	clk->Start();
	i = LOOPCNT;
	while (i-- > 0)
	{
		memcpy(buff2, buff1, copyCnt);
	}
	t1 = clk->GetTimeDiff();

	clk->Start();
	i = LOOPCNT;
	while (i-- > 0)
	{
		MemCopyNOShort(buff2, buff1, copyCnt);
	}
	t2 = clk->GetTimeDiff();

	clk->Start();
	i = LOOPCNT;
	while (i-- > 0)
	{
		memcpy(buff2, buff1, copyCnt);
	}
	t3 = clk->GetTimeDiff();

	clk->Start();
	i = LOOPCNT;
	while (i-- > 0)
	{
		MemCopyNOShort(buff2, buff1, copyCnt);
	}
	t4 = clk->GetTimeDiff();

	printf("t1 = %lf, t2 = %lf, t3 = %lf, t4 = %lf\n", t1, t2, t3, t4);
	DEL_CLASS(clk);
	MemFreeA(buff1);
	MemFreeA(buff2);
}

void MemTest2(UOSInt cnt)
{
	UTF8Char sbuff1[64];
	UTF8Char sbuff2[64];
	UTF8Char *sptr;
	if (cnt < 64)
	{
		sptr = sbuff1;
		while (cnt-- > 0)
		{
			sptr = Text::StrConcat(sptr, (UTF8Char*)"0");
		}
		memcpy(sbuff2, sbuff1, (UOSInt)(sptr - sbuff1));
		MemCopyNAC(sbuff2, sbuff1, (UOSInt)(sptr - sbuff1));
		MemCopyAC(sbuff2, sbuff1, (UOSInt)(sptr - sbuff1));
		MemCopyNANC(sbuff2, sbuff1, (UOSInt)(sptr - sbuff1));
		MemCopyANC(sbuff2, sbuff1, (UOSInt)(sptr - sbuff1));
	}
	else
	{
		UTF8Char *srcPtr = MemAllocA64(UTF8Char, cnt);
		UTF8Char *destPtr = MemAllocA64(UTF8Char, cnt);
//		printf("1\r\n");
		MemClearAC(srcPtr, cnt);
//		printf("2\r\n");
		memcpy(destPtr, srcPtr, cnt);
//		printf("3\r\n");
		MemCopyNAC(destPtr, srcPtr, cnt);
//		printf("4\r\n");
		MemCopyAC(destPtr, srcPtr, cnt);
//		printf("5\r\n");
		MemCopyNANC(destPtr, srcPtr, cnt);
//		printf("6\r\n");
		MemCopyANC(destPtr, srcPtr, cnt);
//		printf("7\r\n");
		MemCopyNOShort(destPtr, srcPtr, cnt);
		MemFreeA(srcPtr);
		MemFreeA(destPtr);
	}
}

Int32 MyMain(NN<Core::IProgControl> progCtrl)
{
	mtest(COPYCNT);
	UOSInt i = 0;
	while (i < 1024)
	{
		printf("i = %d\r\n", (UInt32)i);
		MemTest2(i++);
	}
//	MemTest2(257);
	return 0;
}
