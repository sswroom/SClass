#include "Stdafx.h"
#include "MyMemory.h"
#include "SIMD.h"

extern "C" void MemFillB(UInt8 *buff, OSInt byteCnt, UInt8 val)
{
	while (byteCnt--)
	{
		*buff++ = val;
	}
}

extern "C" void MemFillW(UInt8 *buff, OSInt wordCnt, UInt16 val)
{
	UInt16 *wbuff = (UInt16*)buff;
	while (wordCnt--)
	{
		*wbuff++ = val;
	}
}

#if defined(CPU_X86_64)
extern "C" void MemClearANC_SSE(void *buff, UOSInt buffSize)
{
	Int32x4 zval = PInt32x4Clear();
	UInt8 *buffPtr = (UInt8*)buff;
	buffSize >>= 4;
	while (buffSize-- > 0)
	{
		PStoreInt32x4NC(buffPtr, zval);
		buffPtr += 16;
	}
}

extern "C" void MemClearAC_SSE(void *buff, UOSInt buffSize)
{
	Int32x4 zval = PInt32x4Clear();
	UInt8 *buffPtr = (UInt8*)buff;
	buffSize >>= 4;
	while (buffSize-- > 0)
	{
		PStoreInt32x4(buffPtr, zval);
		buffPtr += 16;
	}
}

extern "C" void MemCopyAC_SSE(void *destPtr, const void *srcPtr, UOSInt leng)
{
	MemCopyNO(destPtr, srcPtr, leng);
}

extern "C" void MemCopyANC_SSE(void *destPtr, const void *srcPtr, UOSInt leng)
{
	const UInt8 *sptr = (const UInt8 *)srcPtr;
	UInt8 *dptr = (UInt8*)destPtr;
	UOSInt i = leng >> 7;
	while (i-- > 0)
	{
		Int32x4 v0 = PLoadInt32x4(&sptr[0]);
		Int32x4 v1 = PLoadInt32x4(&sptr[16]);
		Int32x4 v2 = PLoadInt32x4(&sptr[32]);
		Int32x4 v3 = PLoadInt32x4(&sptr[48]);
		Int32x4 v4 = PLoadInt32x4(&sptr[64]);
		Int32x4 v5 = PLoadInt32x4(&sptr[80]);
		Int32x4 v6 = PLoadInt32x4(&sptr[96]);
		Int32x4 v7 = PLoadInt32x4(&sptr[112]);
		PStoreInt32x4NC(&dptr[0], v0);
		PStoreInt32x4NC(&dptr[16], v1);
		PStoreInt32x4NC(&dptr[32], v2);
		PStoreInt32x4NC(&dptr[48], v3);
		PStoreInt32x4NC(&dptr[64], v4);
		PStoreInt32x4NC(&dptr[80], v5);
		PStoreInt32x4NC(&dptr[96], v6);
		PStoreInt32x4NC(&dptr[112], v7);
		sptr += 128;
		dptr += 128;
	}
	leng = leng & 127;
	i = leng >> 4;
	while (i-- > 0)
	{
		Int32x4 v0 = PLoadInt32x4(&sptr[0]);
		PStoreInt32x4NC(&dptr[0], v0);
		sptr += 16;
		dptr += 16;
	}
	leng &= 15;
	if (leng)
	{
		MemCopyNO(dptr, sptr, leng);
	}
}

extern "C" void MemCopyNAC_SSE(void *destPtr, const void *srcPtr, UOSInt leng)
{
	MemCopyNO(destPtr, srcPtr, leng);
}

extern "C" void MemCopyNANC_SSE(void *destPtr, const void *srcPtr, UOSInt leng)
{
	MemCopyNO(destPtr, srcPtr, leng);
}

extern "C" void MemClearANC_AVX(void *buff, UOSInt buffSize)
{
	Int32x8 zval = PInt32x8Clear();
	UInt8 *buffPtr = (UInt8*)buff;
	buffSize >>= 5;
	while (buffSize-- > 0)
	{
		PStoreInt32x8NC(buffPtr, zval);
		buffPtr += 32;
	}
	PResetAVX();
}

extern "C" void MemClearAC_AVX(void *buff, UOSInt buffSize)
{
	Int32x8 zval = PInt32x8Clear();
	UInt8 *buffPtr = (UInt8*)buff;
	buffSize >>= 5;
	while (buffSize-- > 0)
	{
		PStoreInt32x8(buffPtr, zval);
		buffPtr += 32;
	}
}

extern "C" void MemCopyAC_AVX(void *destPtr, const void *srcPtr, UOSInt leng)
{
	MemCopyNO(destPtr, srcPtr, leng);
}

extern "C" void MemCopyANC_AVX(void *destPtr, const void *srcPtr, UOSInt leng)
{
	const UInt8 *sptr = (const UInt8 *)srcPtr;
	UInt8 *dptr = (UInt8*)destPtr;
	UOSInt i = leng >> 8;
	while (i-- > 0)
	{
		Int32x8 v0 = PLoadInt32x8(&sptr[0]);
		Int32x8 v1 = PLoadInt32x8(&sptr[32]);
		Int32x8 v2 = PLoadInt32x8(&sptr[64]);
		Int32x8 v3 = PLoadInt32x8(&sptr[96]);
		Int32x8 v4 = PLoadInt32x8(&sptr[128]);
		Int32x8 v5 = PLoadInt32x8(&sptr[160]);
		Int32x8 v6 = PLoadInt32x8(&sptr[192]);
		Int32x8 v7 = PLoadInt32x8(&sptr[224]);
		PStoreInt32x8NC(&dptr[0], v0);
		PStoreInt32x8NC(&dptr[32], v1);
		PStoreInt32x8NC(&dptr[64], v2);
		PStoreInt32x8NC(&dptr[96], v3);
		PStoreInt32x8NC(&dptr[128], v4);
		PStoreInt32x8NC(&dptr[160], v5);
		PStoreInt32x8NC(&dptr[192], v6);
		PStoreInt32x8NC(&dptr[224], v7);
		sptr += 256;
		dptr += 256;
	}
	PResetAVX();
	leng = leng & 255;
	i = leng >> 4;
	while (i-- > 0)
	{
		Int32x4 v0 = PLoadInt32x4(&sptr[0]);
		PStoreInt32x4NC(&dptr[0], v0);
		sptr += 16;
		dptr += 16;
	}
	leng &= 15;
	if (leng)
	{
		MemCopyNO(dptr, sptr, leng);
	}
}

extern "C" void MemCopyNAC_AVX(void *destPtr, const void *srcPtr, UOSInt leng)
{
	MemCopyNO(destPtr, srcPtr, leng);
}

extern "C" void MemCopyNANC_AVX(void *destPtr, const void *srcPtr, UOSInt leng)
{
	MemCopyNO(destPtr, srcPtr, leng);
}

extern "C" void MemClearANC_AMDSSE(void *buff, UOSInt buffSize)
{
	Int32x4 zval = PInt32x4Clear();
	UInt8 *buffPtr = (UInt8*)buff;
	buffSize >>= 4;
	while (buffSize-- > 0)
	{
		PStoreInt32x4NC(buffPtr, zval);
		buffPtr += 16;
	}
}

extern "C" void MemClearAC_AMDSSE(void *buff, UOSInt buffSize)
{
	Int32x4 zval = PInt32x4Clear();
	UInt8 *buffPtr = (UInt8*)buff;
	buffSize >>= 4;
	while (buffSize-- > 0)
	{
		PStoreInt32x4(buffPtr, zval);
		buffPtr += 16;
	}
}

extern "C" void MemCopyAC_AMDSSE(void *destPtr, const void *srcPtr, UOSInt leng)
{
	MemCopyNO(destPtr, srcPtr, leng);
}

extern "C" void MemCopyANC_AMDSSE(void *destPtr, const void *srcPtr, UOSInt leng)
{
	MemCopyANC_SSE(destPtr, srcPtr, leng);
}

extern "C" void MemCopyNAC_AMDSSE(void *destPtr, const void *srcPtr, UOSInt leng)
{
	MemCopyNO(destPtr, srcPtr, leng);
}

extern "C" void MemCopyNANC_AMDSSE(void *destPtr, const void *srcPtr, UOSInt leng)
{
	MemCopyNANC_SSE(destPtr, srcPtr, leng);
}

extern "C" void MemClearANC_AMDAVX(void *buff, UOSInt buffSize)
{
	Int32x8 zval = PInt32x8Clear();
	UInt8 *buffPtr = (UInt8*)buff;
	buffSize >>= 5;
	while (buffSize-- > 0)
	{
		PStoreInt32x8(buffPtr, zval);
		buffPtr += 32;
	}
}

extern "C" void MemClearAC_AMDAVX(void *buff, UOSInt buffSize)
{
	Int32x8 zval = PInt32x8Clear();
	UInt8 *buffPtr = (UInt8*)buff;
	buffSize >>= 5;
	while (buffSize-- > 0)
	{
		PStoreInt32x8(buffPtr, zval);
		buffPtr += 32;
	}
}

extern "C" void MemCopyAC_AMDAVX(void *destPtr, const void *srcPtr, UOSInt leng)
{
	MemCopyNO(destPtr, srcPtr, leng);
}

extern "C" void MemCopyANC_AMDAVX(void *destPtr, const void *srcPtr, UOSInt leng)
{
	MemCopyANC_AVX(destPtr, srcPtr, leng);
}

extern "C" void MemCopyNAC_AMDAVX(void *destPtr, const void *srcPtr, UOSInt leng)
{
	MemCopyNO(destPtr, srcPtr, leng);
}

extern "C" void MemCopyNANC_AMDAVX(void *destPtr, const void *srcPtr, UOSInt leng)
{
	MemCopyNANC_AVX(destPtr, srcPtr, leng);
}
#elif defined(HAS_ASM32)
extern "C" void MemClearANC(void *buff, UOSInt buffSize)
{
	Int32x4 zval = PInt32x4Clear();
	UInt8 *buffPtr = (UInt8*)buff;
	buffSize >>= 4;
	while (buffSize-- > 0)
	{
		PStoreInt32x4NC(buffPtr, zval);
		buffPtr += 16;
	}
}

extern "C" void MemClearAC(void *buff, UOSInt buffSize)
{
	Int32x4 zval = PInt32x4Clear();
	UInt8 *buffPtr = (UInt8*)buff;
	buffSize >>= 4;
	while (buffSize-- > 0)
	{
		PStoreInt32x4A(buffPtr, zval);
		buffPtr += 16;
	}
}

#endif
