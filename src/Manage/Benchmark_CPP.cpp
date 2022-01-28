#include "Stdafx.h"
#include "MyMemory.h"
#include "SIMD.h"

extern "C" void Benchmark_MemCopyTest(UInt8 *buff1, UInt8 *buff2, OSInt buffSize, OSInt loopCnt)
{
	while (loopCnt-- > 0)
	{
		MemCopyANC(buff2, buff1, buffSize);
	}
}

extern "C" void Benchmark_MemWriteTest(UInt8 *buff1, UInt8 *buff2, OSInt buffSize, OSInt loopCnt)
{
	UInt8 *destBuff;
	OSInt cnt;
	Int32x4 zval = PInt32x4Clear();
	while (loopCnt-- > 0)
	{
		destBuff = buff2;
		cnt = buffSize / (16 * 8);
		while (cnt-- > 0)
		{
			PStoreInt32x4NC(&destBuff[0], zval);
			PStoreInt32x4NC(&destBuff[16], zval);
			PStoreInt32x4NC(&destBuff[32], zval);
			PStoreInt32x4NC(&destBuff[48], zval);
			PStoreInt32x4NC(&destBuff[64], zval);
			PStoreInt32x4NC(&destBuff[80], zval);
			PStoreInt32x4NC(&destBuff[96], zval);
			PStoreInt32x4NC(&destBuff[112], zval);
			destBuff += 128;
		}
	}
}

extern "C" void Benchmark_MemReadTest(UInt8 *buff1, UInt8 *buff2, OSInt buffSize, OSInt loopCnt)
{
/*	volatile OSInt *srcBuff;
	OSInt cnt;
	OSInt val;
	while (loopCnt-- > 0)
	{
		srcBuff = (volatile OSInt*)buff1;
		cnt = buffSize / (sizeof(OSInt) * 4);
		while (cnt-- > 0)
		{
			val = srcBuff[0];
			val = srcBuff[1];
			val = srcBuff[2];
			val = srcBuff[3];
			srcBuff += 4;
		}
	}*/

#if defined(SIMD_SIMULATE)
	UInt8 *srcBuff;
#else
	volatile UInt8 *srcBuff;
#endif
	OSInt cnt;
	Int32x4 val;
	while (loopCnt-- > 0)
	{
		srcBuff = buff1;
		cnt = buffSize / (16 * 8);
		while (cnt-- > 0)
		{
			val = PLoadInt32x4A(&srcBuff[0]);
			val = PLoadInt32x4A(&srcBuff[16]);
			val = PLoadInt32x4A(&srcBuff[32]);
			val = PLoadInt32x4A(&srcBuff[48]);
			val = PLoadInt32x4A(&srcBuff[64]);
			val = PLoadInt32x4A(&srcBuff[80]);
			val = PLoadInt32x4A(&srcBuff[96]);
			val = PLoadInt32x4A(&srcBuff[112]);
			srcBuff += 128;
		}
	}
}

