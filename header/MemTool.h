#ifndef _SM_MEMTOOL
#define _SM_MEMTOOL
#ifndef SDEFS_INCLUDED
#include "Stdafx.h"
#endif

/*
void MemFillB(UInt8 *buff, OSInt byteCnt, UInt8 val);
void MemFillW(UInt8 *buff, OSInt wordCnt, UInt16 val);
void MemClear(void *buff, OSInt buffSize);
void MemClearANC(void *buff, OSInt buffSize); //buff 16-byte align, buffSize 16 bytes
void MemClearAC(void *buff, OSInt buffSize); //buff 16-byte align, buffSize 16 bytes

void MemCopyNO(destPtr, srcPtr, len);
vpid MemCopyO(destPtr, srcPtr, len);
void MemCopyAC(destPtr, srcPtr, len);
void MemCopyANC(destPtr, srcPtr, len);
void MemCopyNAC(destPtr, srcPtr, len);
void MemCopyNANC(destPtr, srcPtr, len);
//void MemCopyOAC(destPtr, srcPtr, len);

void MemXOR(const UInt8 *srcBuff1, const UInt8 *srcBuff2, UInt8 *destBuff, OSInt count);
*/

#if !defined(_WIN32) && !defined(__APPLE__)
#define UseSSE42 _UseSSE42
#define UseAVX _UseAVX
#define CPUBrand _CPUBrand
#endif

typedef void (__cdecl *MemClearFunc)(void *buff, UOSInt buffSize);
typedef void (__cdecl *MemCopyFunc)(void *destPtr, const void *srcPtr, UOSInt leng);

extern "C"
{
	void MemFillB(UInt8 *buff, UOSInt byteCnt, UInt8 val);
	void MemFillW(UInt8 *buff, UOSInt wordCnt, UInt16 val);
};

#if defined(HAS_ASM32)
#include <string.h>
#include <memory.h>
#define MemCopyNO(destPtr, srcPtr, len) memmove(destPtr, srcPtr, len)
#define MemCopyO(destPtr, srcPtr, len) memcpy(destPtr, srcPtr, len)
#define MemClear(buff, count) memset(buff, 0, count);

#elif defined(_MSC_VER) || defined(__MINGW32__) || defined(__CYGWIN__)
#include <string.h>
#include <memory.h>
#ifdef CPU_X86_64
#include <intrin.h>
#define MemClear(buff, count) {UOSInt tmp = (count) & 7; UInt8 *buffPtr = (UInt8*)buff; while (tmp-- > 0) {*buffPtr++ = 0;} __stosq((UInt64*)buffPtr, 0, (count) >> 3);}
#else
#define MemClear(buff, count) memset(buff, 0, count);
#endif

#define MemCopyO(destPtr, srcPtr, len) memmove(destPtr, srcPtr, ((len) < 0)?0:(len))
#define MemCopyNO(destPtr, srcPtr, len) memcpy(destPtr, srcPtr, ((len) < 0)?0:(len))
#elif defined(CPU_AVR)
extern "C" void MemCopyNO(void *destPtr, const void *srcPtr, UOSInt len);
#define MemCopyO(destPtr, srcPtr, len) MemCopyNO(destPtr, srcPtr, len)
extern "C" void MemClear(void *buff, UOSInt count);

#define MemClearANC(buff, count) MemClear(buff, count);
#define MemClearAC(buff, count) MemClear(buff, count);
#define MemCopyAC(destPtr, srcPtr, len) MemCopyNO(destPtr, srcPtr, len)
#define MemCopyANC(destPtr, srcPtr, len) MemCopyNO(destPtr, srcPtr, len)
#define MemCopyNAC(destPtr, srcPtr, len) MemCopyNO(destPtr, srcPtr, len)
#define MemCopyNANC(destPtr, srcPtr, len) MemCopyNO(destPtr, srcPtr, len)

#elif defined(__sun__)
#include <string.h>
#define MemCopyO(destPtr, srcPtr, len) memmove(destPtr, srcPtr, len)
#define MemCopyNO(destPtr, srcPtr, len) memcpy(destPtr, srcPtr, len)
#define MemClear(buff, count) memset(buff, 0, count);
#else
#include <memory.h>
//FORCEINLINE void MemCopyO(UnsafeArray<void> destPtr, UnsafeArray<const void> srcPtr, UOSInt len) { memmove(destPtr.Ptr(), srcPtr.Ptr(), len); }
//FORCEINLINE void MemCopyNO(UnsafeArray<void> destPtr, UnsafeArray<const void> srcPtr, UOSInt len) { memcpy(destPtr.Ptr(), srcPtr.Ptr(), len); }
//FORCEINLINE void MemClear(UnsafeArray<void> buff, UOSInt count) { memset(buff.Ptr(), 0, count); }
#define MemCopyO(destPtr, srcPtr, len) memmove(destPtr, srcPtr, len)
#define MemCopyNO(destPtr, srcPtr, len) memcpy(destPtr, srcPtr, len)
#define MemClear(buff, count) memset(buff, 0, count);
#endif
//FORCEINLINE void MemCopyNOShort(UnsafeArray<void> destPtr, UnsafeArray<const void> srcPtr, UOSInt len) { memcpy(destPtr.Ptr(), srcPtr.Ptr(), len); }
#define MemCopyNOShort(destPtr, srcPtr, len) memcpy(destPtr, srcPtr, len)

#if defined(HAS_ASM32)
extern "C"
{
	void MemClearANC(void *buff, UOSInt buffSize); //buff 16-byte align, buffSize 16 bytes
	void MemClearAC(void *buff, UOSInt buffSize); //buff 16-byte align, buffSize 16 bytes
}

FORCEINLINE void MemCopyOAC(void *destPtr, const void *srcPtr, UOSInt leng)
{
	_asm
	{
		mov ecx,leng
		mov esi,srcPtr
		mov edi,destPtr
		cmp esi,edi
		jz mcaexit
		ja mcarlop0
		cmp ecx,128
		jb mcalop

		mov edx,ecx
		shr ecx,7
		ALIGN 16
mcalop5f:
		movups xmm0,mmword ptr [esi]
		movups xmm1,mmword ptr [esi+16]
		movups xmm2,mmword ptr [esi+32]
		movups xmm3,mmword ptr [esi+48]
		movups xmm4,mmword ptr [esi+64]
		movups xmm5,mmword ptr [esi+80]
		movups xmm6,mmword ptr [esi+96]
		movups xmm7,mmword ptr [esi+112]
		movaps xmmword ptr [edi],xmm0
		movaps xmmword ptr [edi+16],xmm1
		movaps xmmword ptr [edi+32],xmm2
		movaps xmmword ptr [edi+48],xmm3
		movaps xmmword ptr [edi+64],xmm4
		movaps xmmword ptr [edi+80],xmm5
		movaps xmmword ptr [edi+96],xmm6
		movaps xmmword ptr [edi+112],xmm7
		lea esi,[esi+128]
		lea edi,[edi+128]
		dec ecx
		jnz mcalop5f

		and edx,0x7f
		jz mcaexit
		mov ecx,edx
		shr ecx,4
		jz mcalop2
		align 16
mcalop5g:
		movups xmm0,mmword ptr [esi]
		movaps mmword ptr [edi],xmm0
		lea esi,[esi+16]
		lea edi,[edi+16]
		dec ecx
		jnz mcalop5g
		sfence
		mov ecx,0xf
		and ecx,edx
		jz mcaexit
		rep movsb
		jmp mcaexit

		align 16
mcalop2:
		mov ecx,edx
		align 16
mcalop:
		mov edx,ecx
		shr ecx,2
		rep movsd
		mov ecx,edx
		and ecx,3
		jz mcaexit
		rep movsb
		align 16
		jmp mcaexit

mcarlop0:
		add esi,ecx
		add edi,ecx
		cmp ecx,128
		jb mcarlop

		mov edx,ecx
		shr ecx,7
		ALIGN 16
mcarlop5f:
		movups xmm0,mmword ptr [esi-128]
		movups xmm1,mmword ptr [esi-112]
		movups xmm2,mmword ptr [esi-96]
		movups xmm3,mmword ptr [esi-80]
		movups xmm4,mmword ptr [esi-64]
		movups xmm5,mmword ptr [esi-48]
		movups xmm6,mmword ptr [esi-32]
		movups xmm7,mmword ptr [esi-16]
		movaps xmmword ptr [edi-128],xmm0
		movaps xmmword ptr [edi-112],xmm1
		movaps xmmword ptr [edi-96],xmm2
		movaps xmmword ptr [edi-80],xmm3
		movaps xmmword ptr [edi-64],xmm4
		movaps xmmword ptr [edi-48],xmm5
		movaps xmmword ptr [edi-32],xmm6
		movaps xmmword ptr [edi-16],xmm7
		lea esi,[esi-128]
		lea edi,[edi-128]
		dec ecx
		jnz mcarlop5f

		and edx,0x7f
		jz mcaexit
		mov ecx,edx
		shr ecx,4
		jz mcarlop2
		align 16
mcarlop5g:
		movups xmm0,mmword ptr [esi-16]
		movaps mmword ptr [edi-16],xmm0
		lea esi,[esi-16]
		lea edi,[edi-16]
		dec ecx
		jnz mcarlop5g
		sfence
		mov ecx,0xf
		and ecx,edx
		jz mcaexit
		std
		rep movsb
		cld
		jmp mcaexit

		align 16
mcarlop2:
		mov ecx,edx
		align 16
mcarlop:
		mov edx,ecx
		shr ecx,2
		std
		rep movsd
		mov ecx,edx
		and ecx,3
		jz mcarexit
		rep movsb
		align 16
mcarexit:
		cld
mcaexit:
	}
}

FORCEINLINE void MemCopyAC(void *destPtr, const void *srcPtr, UOSInt leng)
{
	_asm
	{
		mov ecx,leng
		mov esi,srcPtr
		mov edi,destPtr
		cmp esi,edi
		jz mcaexit
		cmp ecx,128
		jb mcalop

		mov edx,ecx
		shr ecx,7
		ALIGN 16
mcalop5f:
		movups xmm0,mmword ptr [esi]
		movups xmm1,mmword ptr [esi+16]
		movups xmm2,mmword ptr [esi+32]
		movups xmm3,mmword ptr [esi+48]
		movups xmm4,mmword ptr [esi+64]
		movups xmm5,mmword ptr [esi+80]
		movups xmm6,mmword ptr [esi+96]
		movups xmm7,mmword ptr [esi+112]
		movaps xmmword ptr [edi],xmm0
		movaps xmmword ptr [edi+16],xmm1
		movaps xmmword ptr [edi+32],xmm2
		movaps xmmword ptr [edi+48],xmm3
		movaps xmmword ptr [edi+64],xmm4
		movaps xmmword ptr [edi+80],xmm5
		movaps xmmword ptr [edi+96],xmm6
		movaps xmmword ptr [edi+112],xmm7
		lea esi,[esi+128]
		lea edi,[edi+128]
		dec ecx
		jnz mcalop5f

		and edx,0x7f
		jz mcaexit
		mov ecx,edx
		shr ecx,4
		jz mcalop2
		align 16
mcalop5g:
		movups xmm0,mmword ptr [esi]
		movaps mmword ptr [edi],xmm0
		lea esi,[esi+16]
		lea edi,[edi+16]
		dec ecx
		jnz mcalop5g
		sfence
		mov ecx,0xf
		and ecx,edx
		jz mcaexit
		rep movsb
		jmp mcaexit

		align 16
mcalop2:
		mov ecx,edx
		align 16
mcalop:
		mov edx,ecx
		shr ecx,2
		rep movsd
		mov ecx,edx
		and ecx,3
		jz mcaexit
		rep movsb
		align 16
mcaexit:
	}
}

FORCEINLINE void MemCopyANC(void *destPtr, const void *srcPtr, UOSInt leng)
{
	_asm
	{
		mov ecx,leng
		mov esi,srcPtr
		mov edi,destPtr
		cmp esi,edi
		jz mcaexit
		cmp ecx,128
		jb mcalop

		mov edx,ecx
		shr ecx,7
		ALIGN 16
mcalop5f:
		movdqu xmm0,mmword ptr [esi]
		movdqu xmm1,mmword ptr [esi+16]
		movdqu xmm2,mmword ptr [esi+32]
		movdqu xmm3,mmword ptr [esi+48]
		movdqu xmm4,mmword ptr [esi+64]
		movdqu xmm5,mmword ptr [esi+80]
		movdqu xmm6,mmword ptr [esi+96]
		movdqu xmm7,mmword ptr [esi+112]
		movntdq xmmword ptr [edi],xmm0
		movntdq xmmword ptr [edi+16],xmm1
		movntdq xmmword ptr [edi+32],xmm2
		movntdq xmmword ptr [edi+48],xmm3
		movntdq xmmword ptr [edi+64],xmm4
		movntdq xmmword ptr [edi+80],xmm5
		movntdq xmmword ptr [edi+96],xmm6
		movntdq xmmword ptr [edi+112],xmm7
		lea esi,[esi+128]
		lea edi,[edi+128]
		dec ecx
		jnz mcalop5f

		and edx,0x7f
		jz mcaexit
		mov ecx,edx
		shr ecx,4
		jz mcalop2
		align 16
mcalop5g:
		movdqu xmm0,mmword ptr [esi]
		movntdq mmword ptr [edi],xmm0
		lea esi,[esi+16]
		lea edi,[edi+16]
		dec ecx
		jnz mcalop5g
		sfence
		mov ecx,0xf
		and ecx,edx
		jz mcaexit
		rep movsb
		jmp mcaexit

		align 16
mcalop2:
		mov ecx,edx
		align 16
mcalop:
		mov edx,ecx
		shr ecx,2
		rep movsd
		mov ecx,edx
		and ecx,3
		jz mcaexit
		rep movsb
		align 16
mcaexit:
	}
}

FORCEINLINE void MemCopyNAC(void *destPtr, const void *srcPtr, UOSInt leng)
{
	_asm
	{
		mov ecx,leng
		mov esi,srcPtr
		mov edi,destPtr
		cmp esi,edi
		jz mcpexit
		cmp ecx,144
		jb mcplop

		mov edx,ecx
		mov eax,0xf
		and eax,edi
		jz mcplop5a
		mov ecx,16
		sub ecx,eax
		sub edx,ecx
		rep movsb
		align 16
mcplop5a:
		mov ecx,edx
		shr ecx,7
		ALIGN 16
mcplop5f:
		movups xmm0,mmword ptr [esi]
		movups xmm1,mmword ptr [esi+16]
		movups xmm2,mmword ptr [esi+32]
		movups xmm3,mmword ptr [esi+48]
		movups xmm4,mmword ptr [esi+64]
		movups xmm5,mmword ptr [esi+80]
		movups xmm6,mmword ptr [esi+96]
		movups xmm7,mmword ptr [esi+112]
		movaps xmmword ptr [edi],xmm0
		movaps xmmword ptr [edi+16],xmm1
		movaps xmmword ptr [edi+32],xmm2
		movaps xmmword ptr [edi+48],xmm3
		movaps xmmword ptr [edi+64],xmm4
		movaps xmmword ptr [edi+80],xmm5
		movaps xmmword ptr [edi+96],xmm6
		movaps xmmword ptr [edi+112],xmm7
		lea esi,[esi+128]
		lea edi,[edi+128]
		dec ecx
		jnz mcplop5f

		and edx,0x7f
		jz mcpexit
		mov ecx,edx
		shr ecx,4
		jz mcplop2
		align 16
mcplop5g:
		movups xmm0,mmword ptr [esi]
		movaps mmword ptr [edi],xmm0
		lea esi,[esi+16]
		lea edi,[edi+16]
		dec ecx
		jnz mcplop5g
		sfence
		mov ecx,0xf
		and ecx,edx
		jz mcpexit
		rep movsb
		jmp mcpexit

		align 16
mcplop2:
		mov ecx,edx
		align 16
mcplop:
		mov edx,ecx
		shr ecx,2
		rep movsd
		mov ecx,edx
		and ecx,3
		jz mcpexit
		rep movsb
		align 16
mcpexit:
	}
}

FORCEINLINE void MemCopyNANC(void *destPtr, const void *srcPtr, UOSInt leng)
{
	_asm
	{
		mov ecx,leng
		mov esi,srcPtr
		mov edi,destPtr
		cmp esi,edi
		jz mcpexit
		cmp ecx,144
		jb mcplop

		mov edx,ecx
		mov eax,0xf
		and eax,edi
		jz mcplop5a
		mov ecx,16
		sub ecx,eax
		sub edx,ecx
		rep movsb
		align 16
mcplop5a:
		mov ecx,edx
		shr ecx,7
		ALIGN 16
mcplop5f:
		movdqu xmm0,mmword ptr [esi]
		movdqu xmm1,mmword ptr [esi+16]
		movdqu xmm2,mmword ptr [esi+32]
		movdqu xmm3,mmword ptr [esi+48]
		movdqu xmm4,mmword ptr [esi+64]
		movdqu xmm5,mmword ptr [esi+80]
		movdqu xmm6,mmword ptr [esi+96]
		movdqu xmm7,mmword ptr [esi+112]
		movntdq xmmword ptr [edi],xmm0
		movntdq xmmword ptr [edi+16],xmm1
		movntdq xmmword ptr [edi+32],xmm2
		movntdq xmmword ptr [edi+48],xmm3
		movntdq xmmword ptr [edi+64],xmm4
		movntdq xmmword ptr [edi+80],xmm5
		movntdq xmmword ptr [edi+96],xmm6
		movntdq xmmword ptr [edi+112],xmm7
		lea esi,[esi+128]
		lea edi,[edi+128]
		dec ecx
		jnz mcplop5f

		and edx,0x7f
		jz mcpexit
		mov ecx,edx
		shr ecx,4
		jz mcplop2
		align 16
mcplop5g:
		movdqu xmm0,mmword ptr [esi]
		movntdq mmword ptr [edi],xmm0
		lea esi,[esi+16]
		lea edi,[edi+16]
		dec ecx
		jnz mcplop5g
		sfence
		mov ecx,0xf
		and ecx,edx
		jz mcpexit
		rep movsb
		jmp mcpexit

		align 16
mcplop2:
		mov ecx,edx
		align 16
mcplop:
		mov edx,ecx
		shr ecx,2
		rep movsd
		mov ecx,edx
		and ecx,3
		jz mcpexit
		rep movsb
		align 16
mcpexit:
	}
}
#elif defined(CPU_X86_64)
extern MemClearFunc MemClearANC; //buff 16-byte align, buffSize 16 bytes
extern MemClearFunc MemClearAC; //buff 16-byte align, buffSize 16 bytes
extern MemCopyFunc MemCopyAC;
extern MemCopyFunc MemCopyANC;
extern MemCopyFunc MemCopyNAC;
extern MemCopyFunc MemCopyNANC;
//extern MemCopyFunc MemCopyOAC;

#if !defined(_MSC_VER) && !defined(__MINGW32__)
extern "C"
{
	void MemCopyNAC_SSE(void *destPtr, const void *srcPtr, UOSInt leng);
}
#undef MemCopyNOShort
#define MemCopyNOShort(destPtr, srcPtr, len) MemCopyNAC_SSE(destPtr, srcPtr, len)
#endif
#else
#define MemClearANC(buff, count) MemClear(buff, count);
#define MemClearAC(buff, count) MemClear(buff, count);
#define MemCopyAC(destPtr, srcPtr, len) MemCopyNO(destPtr, srcPtr, len)
#define MemCopyANC(destPtr, srcPtr, len) MemCopyNO(destPtr, srcPtr, len)
#define MemCopyNAC(destPtr, srcPtr, len) MemCopyNO(destPtr, srcPtr, len)
#define MemCopyNANC(destPtr, srcPtr, len) MemCopyNO(destPtr, srcPtr, len)
//#define MemCopyOAC(destPtr, srcPtr, len) MemCopyO(destPtr, srcPtr, len)
#endif


FORCEINLINE void MemXOR(const UInt8 *srcBuff1, const UInt8 *srcBuff2, UInt8 *destBuff, UOSInt count)
{
	while (count >= (UOSInt)sizeof(UOSInt))
	{
		*(UOSInt*)destBuff = (*(UOSInt*)srcBuff1) ^ (*(UOSInt*)srcBuff2);
		srcBuff1 += sizeof(UOSInt);
		srcBuff2 += sizeof(UOSInt);
		destBuff += sizeof(UOSInt);
		count -= sizeof(UOSInt);
	}
	while (count > 0)
	{
		*destBuff = (*srcBuff1) ^ (*srcBuff2);
		destBuff++;
		srcBuff1++;
		srcBuff2++;
		count--;
	}
}

void MemTool_Init();

typedef void (CALLBACKFUNC MemCopyBFunc)(UInt8 *destPtr, const UInt8 *srcPtr, UOSInt size);

void __stdcall MyMemCopy0(UInt8 *destPtr, const UInt8 *srcPtr, UOSInt size);
void __stdcall MyMemCopy1(UInt8 *destPtr, const UInt8 *srcPtr, UOSInt size);
void __stdcall MyMemCopy2(UInt8 *destPtr, const UInt8 *srcPtr, UOSInt size);
void __stdcall MyMemCopy3(UInt8 *destPtr, const UInt8 *srcPtr, UOSInt size);
void __stdcall MyMemCopy4(UInt8 *destPtr, const UInt8 *srcPtr, UOSInt size);
void __stdcall MyMemCopy5(UInt8 *destPtr, const UInt8 *srcPtr, UOSInt size);
void __stdcall MyMemCopy6(UInt8 *destPtr, const UInt8 *srcPtr, UOSInt size);
void __stdcall MyMemCopy7(UInt8 *destPtr, const UInt8 *srcPtr, UOSInt size);
void __stdcall MyMemCopy8(UInt8 *destPtr, const UInt8 *srcPtr, UOSInt size);
void __stdcall MyMemCopy9(UInt8 *destPtr, const UInt8 *srcPtr, UOSInt size);
void __stdcall MyMemCopy10(UInt8 *destPtr, const UInt8 *srcPtr, UOSInt size);
void __stdcall MyMemCopy11(UInt8 *destPtr, const UInt8 *srcPtr, UOSInt size);
void __stdcall MyMemCopy12(UInt8 *destPtr, const UInt8 *srcPtr, UOSInt size);
void __stdcall MyMemCopy13(UInt8 *destPtr, const UInt8 *srcPtr, UOSInt size);
void __stdcall MyMemCopy14(UInt8 *destPtr, const UInt8 *srcPtr, UOSInt size);
void __stdcall MyMemCopy15(UInt8 *destPtr, const UInt8 *srcPtr, UOSInt size);
void __stdcall MyMemCopy16(UInt8 *destPtr, const UInt8 *srcPtr, UOSInt size);
void __stdcall MyMemCopy17(UInt8 *destPtr, const UInt8 *srcPtr, UOSInt size);
void __stdcall MyMemCopy18(UInt8 *destPtr, const UInt8 *srcPtr, UOSInt size);
void __stdcall MyMemCopy19(UInt8 *destPtr, const UInt8 *srcPtr, UOSInt size);
void __stdcall MyMemCopy20(UInt8 *destPtr, const UInt8 *srcPtr, UOSInt size);
void __stdcall MyMemCopy21(UInt8 *destPtr, const UInt8 *srcPtr, UOSInt size);
void __stdcall MyMemCopy22(UInt8 *destPtr, const UInt8 *srcPtr, UOSInt size);
void __stdcall MyMemCopy23(UInt8 *destPtr, const UInt8 *srcPtr, UOSInt size);
void __stdcall MyMemCopy24(UInt8 *destPtr, const UInt8 *srcPtr, UOSInt size);
void __stdcall MyMemCopy25(UInt8 *destPtr, const UInt8 *srcPtr, UOSInt size);
void __stdcall MyMemCopy26(UInt8 *destPtr, const UInt8 *srcPtr, UOSInt size);
void __stdcall MyMemCopy27(UInt8 *destPtr, const UInt8 *srcPtr, UOSInt size);
void __stdcall MyMemCopy28(UInt8 *destPtr, const UInt8 *srcPtr, UOSInt size);
void __stdcall MyMemCopy29(UInt8 *destPtr, const UInt8 *srcPtr, UOSInt size);
void __stdcall MyMemCopy30(UInt8 *destPtr, const UInt8 *srcPtr, UOSInt size);
void __stdcall MyMemCopy31(UInt8 *destPtr, const UInt8 *srcPtr, UOSInt size);

static MemCopyBFunc MemCopyArr[32] = {
	MyMemCopy0,
	MyMemCopy1,
	MyMemCopy2,
	MyMemCopy3,
	MyMemCopy4,
	MyMemCopy5,
	MyMemCopy6,
	MyMemCopy7,
	MyMemCopy8,
	MyMemCopy9,
	MyMemCopy10,
	MyMemCopy11,
	MyMemCopy12,
	MyMemCopy13,
	MyMemCopy14,
	MyMemCopy15,
	MyMemCopy16,
	MyMemCopy17,
	MyMemCopy18,
	MyMemCopy19,
	MyMemCopy20,
	MyMemCopy21,
	MyMemCopy22,
	MyMemCopy23,
	MyMemCopy24,
	MyMemCopy25,
	MyMemCopy26,
	MyMemCopy27,
	MyMemCopy28,
	MyMemCopy29,
	MyMemCopy30,
	MyMemCopy31
};

FORCEINLINE void MyMemCopy(void *destPtr, const void *srcPtr, UOSInt size)
{
	if (size < 32)
	{
		MemCopyArr[size]((UInt8*)destPtr, (const UInt8*)srcPtr, size);
	}
	else
	{
		memcpy(destPtr, srcPtr, size);
	}
}
#endif
