#ifndef _SM_MYMEMORY
#define _SM_MYMEMORY

/*
void NEW_CLASS(variable, className);
void DEL_CLASS(variable);

T *MemAlloc(T, cnt);
T *MemAllocA(T, cnt);
T *MemAllocA64(T, cnt);
void MemFree(ptr);
void MemFreeA(ptr);
void MemFreeA64(ptr);

void MemInit();
void MemDeinit();
void MemPtrChk(void *ptr);
void MemSetBreakPoint(OSInt address);
void MemSetBreakPoint(OSInt address, OSInt size);
void MemSetLogFile(const UTF8Char *logFile);
void MemLock();
void MemUnlock();
void MemClear(void *buff, OSInt size);
Int32 MemCheckError();
Int32 MemCountBlks();
void MemIncCounter(void *ptr);
void MemDecCounter(void *ptr);

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

void MemXOR(const UInt8 *srcBuff1, const UInt8 *srcBuff2, UInt8 *destBuff, OSInt count);
*/

#if !defined(_WIN32) && !defined(__APPLE__)
#define UseAVX _UseAVX
#define CPUBrand _CPUBrand
#endif

#if  0
#define NEW_CLASS(variable, className) {variable = new className;MemPtrChk(variable);MemIncCounter(variable);}
#define DEL_CLASS(variable) {delete variable;MemDecCounter(variable);}
#else
#define NEW_CLASS(variable, className) {variable = new className;MemPtrChk(variable);}
#define DEL_CLASS(variable) {delete variable;}
#endif
#define SDEL_CLASS(variable) if (variable) {DEL_CLASS(variable); variable = 0;}

#define MemAlloc(T, cnt) (T*)MAlloc(sizeof(T) * (cnt))
#define MemAllocA(T, cnt) (T*)MAllocA64(sizeof(T) * (cnt))
#define MemAllocA64(T, cnt) (T*)MAllocA64(sizeof(T) * (cnt))
#define MemFreeA64(ptr) MemFreeA(ptr)

void MemPtrChk(void *ptr);
void MemInit();
void MemSetBreakPoint(OSInt address);
void MemSetBreakPoint(OSInt address, OSInt size);
void MemSetLogFile(const UTF8Char *logFile);
void MemLock();
void MemUnlock();
void *MAlloc(OSInt size);
void *MAllocA(OSInt size);
void *MAllocA64(OSInt size);
void MemFree(void *ptr);
void MemFreeA(void *ptr);
void MemDeinit();
Int32 MemCheckError();
Int32 MemCountBlks();
void MemIncCounter(void *ptr);
void MemDecCounter(void *ptr);

typedef void (__cdecl *MemClearFunc)(void *buff, OSInt buffSize);
typedef void (__cdecl *MemCopyFunc)(void *destPtr, const void *srcPtr, OSInt leng);

extern "C"
{
	void MemFillB(UInt8 *buff, OSInt byteCnt, UInt8 val);
	void MemFillW(UInt8 *buff, OSInt wordCnt, UInt16 val);
};

#if defined(HAS_ASM32)
#include <string.h>
#include <memory.h>
#define MemCopyNO(destPtr, srcPtr, len) memmove(destPtr, srcPtr, len)
#define MemCopyO(destPtr, srcPtr, len) memcpy(destPtr, srcPtr, len)
#define MemClear(buff, count) memset(buff, 0, count);

#elif defined(_MSC_VER) || defined(__MINGW32__)
#include <string.h>
#include <memory.h>
#ifdef CPU_X86_64
#include <intrin.h>
#define MemClear(buff, count) {OSInt tmp = (count) & 7; UInt8 *buffPtr = (UInt8*)buff; while (tmp-- > 0) {*buffPtr++ = 0;} __stosq((UInt64*)buffPtr, 0, (count) >> 3);}
#else
#define MemClear(buff, count) memset(buff, 0, count);
#endif

#define MemCopyO(destPtr, srcPtr, len) memmove(destPtr, srcPtr, ((len) < 0)?0:(len))
#define MemCopyNO(destPtr, srcPtr, len) memcpy(destPtr, srcPtr, ((len) < 0)?0:(len))
#elif defined(CPU_AVR)
extern "C" void MemCopyNO(void *destPtr, const void *srcPtr, OSInt len);
#define MemCopyO(destPtr, srcPtr, len) MemCopyNO(destPtr, srcPtr, len)
extern "C" void MemClear(void *buff, OSInt count);

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
#define MemCopyO(destPtr, srcPtr, len) memmove(destPtr, srcPtr, len)
#define MemCopyNO(destPtr, srcPtr, len) memcpy(destPtr, srcPtr, len)
#define MemClear(buff, count) memset(buff, 0, count);
#endif

#if defined(HAS_ASM32)
extern "C"
{
	void MemClearANC(void *buff, OSInt buffSize); //buff 16-byte align, buffSize 16 bytes
	void MemClearAC(void *buff, OSInt buffSize); //buff 16-byte align, buffSize 16 bytes
}

FORCEINLINE void MemCopyAC(void *destPtr, const void *srcPtr, OSInt leng)
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

FORCEINLINE void MemCopyANC(void *destPtr, const void *srcPtr, OSInt leng)
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

FORCEINLINE void MemCopyNAC(void *destPtr, const void *srcPtr, OSInt leng)
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

FORCEINLINE void MemCopyNANC(void *destPtr, const void *srcPtr, OSInt leng)
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
#else
#define MemClearANC(buff, count) MemClear(buff, count);
#define MemClearAC(buff, count) MemClear(buff, count);
#define MemCopyAC(destPtr, srcPtr, len) MemCopyNO(destPtr, srcPtr, len)
#define MemCopyANC(destPtr, srcPtr, len) MemCopyNO(destPtr, srcPtr, len)
#define MemCopyNAC(destPtr, srcPtr, len) MemCopyNO(destPtr, srcPtr, len)
#define MemCopyNANC(destPtr, srcPtr, len) MemCopyNO(destPtr, srcPtr, len)
#endif


FORCEINLINE void MemXOR(const UInt8 *srcBuff1, const UInt8 *srcBuff2, UInt8 *destBuff, OSInt count)
{
	while (count >= (OSInt)sizeof(OSInt))
	{
		*(OSInt*)destBuff = (*(OSInt*)srcBuff1) ^ (*(OSInt*)srcBuff2);
		srcBuff1 += sizeof(OSInt);
		srcBuff2 += sizeof(OSInt);
		destBuff += sizeof(OSInt);
		count -= sizeof(OSInt);
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

#endif
