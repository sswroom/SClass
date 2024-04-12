#include "Stdafx.h"
#include <stdlib.h>

#undef HAS_ASM32
#ifdef HAS_ASM32
#define F1(x, y, z) _asm {mov eax,x} _asm {xor eax,y} _asm {xor eax,z}
#define F2(x, y, z) _asm {push edi} _asm {mov eax,x} _asm {mov edi,eax} _asm {and eax,y} _asm {not edi} _asm {and edi,z} _asm {or eax,edi} _asm {pop edi}
#define F3(x, y, z) _asm {mov eax,y} _asm {not eax} _asm{or eax,x} _asm {xor eax,z}
#define F4(x, y, z) _asm {push edi} _asm {mov eax,z} _asm {mov edi,eax} _asm {and eax,x} _asm {not edi} _asm {and edi,y} _asm {or eax,edi} _asm {pop edi}

#define HASHSTEP(f, w, x, y, z, data, data2, s) f(x, y, z) _asm {add w, eax} _asm{add w, dword ptr [edi + data * 4]} _asm {add w, data2} _asm {rol w, s}
#define a ebx
#define b ecx
#define c esi
#define d edx

extern "C" void RMD128_CalcBlock(UInt32 *keys, const UInt8 *block)
{
	UInt32 firstVals[4];
	_asm
	{
		mov edi,keys
		mov ebx,dword ptr [edi]
		mov ecx,dword ptr [edi+4]
		mov esi,dword ptr [edi+8]
		mov edx,dword ptr [edi+12]
		mov edi,block
	}

	HASHSTEP(F1, a, b, c, d, 0,  0x00000000, 11);
	HASHSTEP(F1, d, a, b, c, 1,  0x00000000, 14);
	HASHSTEP(F1, c, d, a, b, 2,  0x00000000, 15);
	HASHSTEP(F1, b, c, d, a, 3,  0x00000000, 12);
	HASHSTEP(F1, a, b, c, d, 4,  0x00000000, 5);
	HASHSTEP(F1, d, a, b, c, 5,  0x00000000, 8);
	HASHSTEP(F1, c, d, a, b, 6,  0x00000000, 7);
	HASHSTEP(F1, b, c, d, a, 7,  0x00000000, 9);
	HASHSTEP(F1, a, b, c, d, 8,  0x00000000, 11);
	HASHSTEP(F1, d, a, b, c, 9,  0x00000000, 13);
	HASHSTEP(F1, c, d, a, b, 10, 0x00000000, 14);
	HASHSTEP(F1, b, c, d, a, 11, 0x00000000, 15);
	HASHSTEP(F1, a, b, c, d, 12, 0x00000000, 6);
	HASHSTEP(F1, d, a, b, c, 13, 0x00000000, 7);
	HASHSTEP(F1, c, d, a, b, 14, 0x00000000, 9);
	HASHSTEP(F1, b, c, d, a, 15, 0x00000000, 8);

	HASHSTEP(F2, a, b, c, d, 7,  0x5A827999, 7);
	HASHSTEP(F2, d, a, b, c, 4,  0x5A827999, 6);
	HASHSTEP(F2, c, d, a, b, 13, 0x5A827999, 8);
	HASHSTEP(F2, b, c, d, a, 1,  0x5A827999, 13);
	HASHSTEP(F2, a, b, c, d, 10, 0x5A827999, 11);
	HASHSTEP(F2, d, a, b, c, 6,  0x5A827999, 9);
	HASHSTEP(F2, c, d, a, b, 15, 0x5A827999, 7);
	HASHSTEP(F2, b, c, d, a, 3,  0x5A827999, 15);
	HASHSTEP(F2, a, b, c, d, 12, 0x5A827999, 7);
	HASHSTEP(F2, d, a, b, c, 0,  0x5A827999, 12);
	HASHSTEP(F2, c, d, a, b, 9,  0x5A827999, 15);
	HASHSTEP(F2, b, c, d, a, 5,  0x5A827999, 9);
	HASHSTEP(F2, a, b, c, d, 2,  0x5A827999, 11);
	HASHSTEP(F2, d, a, b, c, 14, 0x5A827999, 7);
	HASHSTEP(F2, c, d, a, b, 11, 0x5A827999, 13);
	HASHSTEP(F2, b, c, d, a, 8,  0x5A827999, 12);

	HASHSTEP(F3, a, b, c, d, 3,  0x6ED9EBA1, 11);
	HASHSTEP(F3, d, a, b, c, 10, 0x6ED9EBA1, 13);
	HASHSTEP(F3, c, d, a, b, 14, 0x6ED9EBA1, 6);
	HASHSTEP(F3, b, c, d, a, 4,  0x6ED9EBA1, 7);
	HASHSTEP(F3, a, b, c, d, 9,  0x6ED9EBA1, 14);
	HASHSTEP(F3, d, a, b, c, 15, 0x6ED9EBA1, 9);
	HASHSTEP(F3, c, d, a, b, 8,  0x6ED9EBA1, 13);
	HASHSTEP(F3, b, c, d, a, 1,  0x6ED9EBA1, 15);
	HASHSTEP(F3, a, b, c, d, 2,  0x6ED9EBA1, 14);
	HASHSTEP(F3, d, a, b, c, 7,  0x6ED9EBA1, 8);
	HASHSTEP(F3, c, d, a, b, 0,  0x6ED9EBA1, 13);
	HASHSTEP(F3, b, c, d, a, 6,  0x6ED9EBA1, 6);
	HASHSTEP(F3, a, b, c, d, 13, 0x6ED9EBA1, 5);
	HASHSTEP(F3, d, a, b, c, 11, 0x6ED9EBA1, 12);
	HASHSTEP(F3, c, d, a, b, 5,  0x6ED9EBA1, 7);
	HASHSTEP(F3, b, c, d, a, 12, 0x6ED9EBA1, 5);

	HASHSTEP(F4, a, b, c, d, 1,  0x8F1BBCDC, 11);
	HASHSTEP(F4, d, a, b, c, 9,  0x8F1BBCDC, 12);
	HASHSTEP(F4, c, d, a, b, 11, 0x8F1BBCDC, 14);
	HASHSTEP(F4, b, c, d, a, 10, 0x8F1BBCDC, 15);
	HASHSTEP(F4, a, b, c, d, 0,  0x8F1BBCDC, 14);
	HASHSTEP(F4, d, a, b, c, 8,  0x8F1BBCDC, 15);
	HASHSTEP(F4, c, d, a, b, 12, 0x8F1BBCDC, 9);
	HASHSTEP(F4, b, c, d, a, 4,  0x8F1BBCDC, 8);
	HASHSTEP(F4, a, b, c, d, 13, 0x8F1BBCDC, 9);
	HASHSTEP(F4, d, a, b, c, 3,  0x8F1BBCDC, 14);
	HASHSTEP(F4, c, d, a, b, 7,  0x8F1BBCDC, 5);
	HASHSTEP(F4, b, c, d, a, 15, 0x8F1BBCDC, 6);
	HASHSTEP(F4, a, b, c, d, 14, 0x8F1BBCDC, 8);
	HASHSTEP(F4, d, a, b, c, 5,  0x8F1BBCDC, 6);
	HASHSTEP(F4, c, d, a, b, 6,  0x8F1BBCDC, 5);
	HASHSTEP(F4, b, c, d, a, 2,  0x8F1BBCDC, 12);

	_asm
	{
		mov dword ptr firstVals[0],ebx
		mov dword ptr firstVals[4],ecx
		mov dword ptr firstVals[8],esi
		mov dword ptr firstVals[12],edx
	}
/*	A' = h0;
	B' = h1;
	C' = h2;
	D' = h3;
	E' = h4;


	T = rol_s'(j)(A' + f(79-j, B', C', D') + X[r'(j)] + K'(j)) + E';
	A = E';
	E' = D';
	D' = rol_10(C');
	C' = B';
	B' = T;
*/
	_asm
	{
		mov edi,keys
		mov ebx,dword ptr [edi]
		mov ecx,dword ptr [edi+4]
		mov esi,dword ptr [edi+8]
		mov edx,dword ptr [edi+12]
		mov edi,block
	}

	HASHSTEP(F4, a, b, c, d, 5,  0x50A28BE6, 8);
	HASHSTEP(F4, d, a, b, c, 14, 0x50A28BE6, 9);
	HASHSTEP(F4, c, d, a, b, 7,  0x50A28BE6, 9);
	HASHSTEP(F4, b, c, d, a, 0,  0x50A28BE6, 11);
	HASHSTEP(F4, a, b, c, d, 9,  0x50A28BE6, 13);
	HASHSTEP(F4, d, a, b, c, 2,  0x50A28BE6, 15);
	HASHSTEP(F4, c, d, a, b, 11, 0x50A28BE6, 15);
	HASHSTEP(F4, b, c, d, a, 4,  0x50A28BE6, 5);
	HASHSTEP(F4, a, b, c, d, 13, 0x50A28BE6, 7);
	HASHSTEP(F4, d, a, b, c, 6,  0x50A28BE6, 7);
	HASHSTEP(F4, c, d, a, b, 15, 0x50A28BE6, 8);
	HASHSTEP(F4, b, c, d, a, 8,  0x50A28BE6, 11);
	HASHSTEP(F4, a, b, c, d, 1,  0x50A28BE6, 14);
	HASHSTEP(F4, d, a, b, c, 10, 0x50A28BE6, 14);
	HASHSTEP(F4, c, d, a, b, 3,  0x50A28BE6, 12);
	HASHSTEP(F4, b, c, d, a, 12, 0x50A28BE6, 6);

	HASHSTEP(F3, a, b, c, d, 6,  0x5C4DD124, 9);
	HASHSTEP(F3, d, a, b, c, 11, 0x5C4DD124, 13);
	HASHSTEP(F3, c, d, a, b, 3,  0x5C4DD124, 15);
	HASHSTEP(F3, b, c, d, a, 7,  0x5C4DD124, 7);
	HASHSTEP(F3, a, b, c, d, 0,  0x5C4DD124, 12);
	HASHSTEP(F3, d, a, b, c, 13, 0x5C4DD124, 8);
	HASHSTEP(F3, c, d, a, b, 5,  0x5C4DD124, 9);
	HASHSTEP(F3, b, c, d, a, 10, 0x5C4DD124, 11);
	HASHSTEP(F3, a, b, c, d, 14, 0x5C4DD124, 7);
	HASHSTEP(F3, d, a, b, c, 15, 0x5C4DD124, 7);
	HASHSTEP(F3, c, d, a, b, 8,  0x5C4DD124, 12);
	HASHSTEP(F3, b, c, d, a, 12, 0x5C4DD124, 7);
	HASHSTEP(F3, a, b, c, d, 4,  0x5C4DD124, 6);
	HASHSTEP(F3, d, a, b, c, 9,  0x5C4DD124, 15);
	HASHSTEP(F3, c, d, a, b, 1,  0x5C4DD124, 13);
	HASHSTEP(F3, b, c, d, a, 2,  0x5C4DD124, 11);

	HASHSTEP(F2, a, b, c, d, 15, 0x6D703EF3, 9);
	HASHSTEP(F2, d, a, b, c, 5,  0x6D703EF3, 7);
	HASHSTEP(F2, c, d, a, b, 1,  0x6D703EF3, 15);
	HASHSTEP(F2, b, c, d, a, 3,  0x6D703EF3, 11);
	HASHSTEP(F2, a, b, c, d, 7,  0x6D703EF3, 8);
	HASHSTEP(F2, d, a, b, c, 14, 0x6D703EF3, 6);
	HASHSTEP(F2, c, d, a, b, 6,  0x6D703EF3, 6);
	HASHSTEP(F2, b, c, d, a, 9,  0x6D703EF3, 14);
	HASHSTEP(F2, a, b, c, d, 11, 0x6D703EF3, 12);
	HASHSTEP(F2, d, a, b, c, 8,  0x6D703EF3, 13);
	HASHSTEP(F2, c, d, a, b, 12, 0x6D703EF3, 5);
	HASHSTEP(F2, b, c, d, a, 2,  0x6D703EF3, 14);
	HASHSTEP(F2, a, b, c, d, 10, 0x6D703EF3, 13);
	HASHSTEP(F2, d, a, b, c, 0,  0x6D703EF3, 13);
	HASHSTEP(F2, c, d, a, b, 4,  0x6D703EF3, 7);
	HASHSTEP(F2, b, c, d, a, 13, 0x6D703EF3, 5);

	HASHSTEP(F1, a, b, c, d, 8,  0x00000000, 15);
	HASHSTEP(F1, d, a, b, c, 6,  0x00000000, 5);
	HASHSTEP(F1, c, d, a, b, 4,  0x00000000, 8);
	HASHSTEP(F1, b, c, d, a, 1,  0x00000000, 11);
	HASHSTEP(F1, a, b, c, d, 3,  0x00000000, 14);
	HASHSTEP(F1, d, a, b, c, 11, 0x00000000, 14);
	HASHSTEP(F1, c, d, a, b, 15, 0x00000000, 6);
	HASHSTEP(F1, b, c, d, a, 0,  0x00000000, 14);
	HASHSTEP(F1, a, b, c, d, 5,  0x00000000, 6);
	HASHSTEP(F1, d, a, b, c, 12, 0x00000000, 9);
	HASHSTEP(F1, c, d, a, b, 2,  0x00000000, 12);
	HASHSTEP(F1, b, c, d, a, 13, 0x00000000, 9);
	HASHSTEP(F1, a, b, c, d, 9,  0x00000000, 12);
	HASHSTEP(F1, d, a, b, c, 7,  0x00000000, 5);
	HASHSTEP(F1, c, d, a, b, 10, 0x00000000, 15);
	HASHSTEP(F1, b, c, d, a, 14, 0x00000000, 8);

/*
        T := h1 + C + D';
		h1 := h2 + D + A';
        h2 := h3 + A + B';
		h3 := h0 + B + C';
		h0 := T;*/

	_asm
	{
		mov edi,keys

		add esi, dword ptr [edi]
		add esi, firstVals[4]
		add edx, dword ptr [edi+4]
		add edx, firstVals[8]
		add ebx, dword ptr [edi+8]
		add ebx, firstVals[12]
		add ecx, dword ptr [edi+12]
		add ecx, firstVals[0]
		mov dword ptr [edi], edx
		mov dword ptr [edi+4], ebx
		mov dword ptr [edi+8], ecx
		mov dword ptr [edi+12], esi
	}
}

#else
#if 0 && (defined(HAS_GCCASM32) || defined(HAS_GCCASM64))
template<class T>
inline T _rotl(T x, UInt8 r)
{
	asm("roll %1,%0" : "+r" (x) : "c" (r));
	return x;
}
#else
#define _rotl(a, b) (((a) << (b)) | ((a) >> (32 - b)));
#endif

#define F1(x, y, z) (x ^ y ^ z)
#define F2(x, y, z) ((x & y) | (~x & z))
#define F3(x, y, z) ((x | ~y) ^ z)
#define F4(x, y, z) ((x & z) | (y & ~z))
#define HASHSTEP(f, w, x, y, z, data, data2, s) {w = _rotl(w + f(x, y, z) + *(UInt32*)&block[data * 4] + data2, s);}

extern "C" void RMD128_CalcBlock(UInt32 *keys, const UInt8 *block)
{
	UInt32 aVal;
	UInt32 bVal;
	UInt32 cVal;
	UInt32 dVal;
	UInt32 aVal2;
	UInt32 bVal2;
	UInt32 cVal2;
	UInt32 dVal2;
	aVal = aVal2 = keys[0];
	bVal = bVal2 = keys[1];
	cVal = cVal2 = keys[2];
	dVal = dVal2 = keys[3];

/*	A = h0;
	B = h1;
	C = h2;
	D = h3;

	T = rol_s(j)(A + f(j, B, C, D) + X[r(j)] + K(j)) + E;
	A = E;
	E = D;
	D = rol_10(C);
	C = B;
	B = T;*/
#define a aVal
#define b bVal
#define c cVal
#define d dVal

	HASHSTEP(F1, a, b, c, d, 0,  0x00000000, 11);
	HASHSTEP(F1, d, a, b, c, 1,  0x00000000, 14);
	HASHSTEP(F1, c, d, a, b, 2,  0x00000000, 15);
	HASHSTEP(F1, b, c, d, a, 3,  0x00000000, 12);
	HASHSTEP(F1, a, b, c, d, 4,  0x00000000, 5);
	HASHSTEP(F1, d, a, b, c, 5,  0x00000000, 8);
	HASHSTEP(F1, c, d, a, b, 6,  0x00000000, 7);
	HASHSTEP(F1, b, c, d, a, 7,  0x00000000, 9);
	HASHSTEP(F1, a, b, c, d, 8,  0x00000000, 11);
	HASHSTEP(F1, d, a, b, c, 9,  0x00000000, 13);
	HASHSTEP(F1, c, d, a, b, 10, 0x00000000, 14);
	HASHSTEP(F1, b, c, d, a, 11, 0x00000000, 15);
	HASHSTEP(F1, a, b, c, d, 12, 0x00000000, 6);
	HASHSTEP(F1, d, a, b, c, 13, 0x00000000, 7);
	HASHSTEP(F1, c, d, a, b, 14, 0x00000000, 9);
	HASHSTEP(F1, b, c, d, a, 15, 0x00000000, 8);

	HASHSTEP(F2, a, b, c, d, 7,  0x5A827999, 7);
	HASHSTEP(F2, d, a, b, c, 4,  0x5A827999, 6);
	HASHSTEP(F2, c, d, a, b, 13, 0x5A827999, 8);
	HASHSTEP(F2, b, c, d, a, 1,  0x5A827999, 13);
	HASHSTEP(F2, a, b, c, d, 10, 0x5A827999, 11);
	HASHSTEP(F2, d, a, b, c, 6,  0x5A827999, 9);
	HASHSTEP(F2, c, d, a, b, 15, 0x5A827999, 7);
	HASHSTEP(F2, b, c, d, a, 3,  0x5A827999, 15);
	HASHSTEP(F2, a, b, c, d, 12, 0x5A827999, 7);
	HASHSTEP(F2, d, a, b, c, 0,  0x5A827999, 12);
	HASHSTEP(F2, c, d, a, b, 9,  0x5A827999, 15);
	HASHSTEP(F2, b, c, d, a, 5,  0x5A827999, 9);
	HASHSTEP(F2, a, b, c, d, 2,  0x5A827999, 11);
	HASHSTEP(F2, d, a, b, c, 14, 0x5A827999, 7);
	HASHSTEP(F2, c, d, a, b, 11, 0x5A827999, 13);
	HASHSTEP(F2, b, c, d, a, 8,  0x5A827999, 12);

	HASHSTEP(F3, a, b, c, d, 3,  0x6ED9EBA1, 11);
	HASHSTEP(F3, d, a, b, c, 10, 0x6ED9EBA1, 13);
	HASHSTEP(F3, c, d, a, b, 14, 0x6ED9EBA1, 6);
	HASHSTEP(F3, b, c, d, a, 4,  0x6ED9EBA1, 7);
	HASHSTEP(F3, a, b, c, d, 9,  0x6ED9EBA1, 14);
	HASHSTEP(F3, d, a, b, c, 15, 0x6ED9EBA1, 9);
	HASHSTEP(F3, c, d, a, b, 8,  0x6ED9EBA1, 13);
	HASHSTEP(F3, b, c, d, a, 1,  0x6ED9EBA1, 15);
	HASHSTEP(F3, a, b, c, d, 2,  0x6ED9EBA1, 14);
	HASHSTEP(F3, d, a, b, c, 7,  0x6ED9EBA1, 8);
	HASHSTEP(F3, c, d, a, b, 0,  0x6ED9EBA1, 13);
	HASHSTEP(F3, b, c, d, a, 6,  0x6ED9EBA1, 6);
	HASHSTEP(F3, a, b, c, d, 13, 0x6ED9EBA1, 5);
	HASHSTEP(F3, d, a, b, c, 11, 0x6ED9EBA1, 12);
	HASHSTEP(F3, c, d, a, b, 5,  0x6ED9EBA1, 7);
	HASHSTEP(F3, b, c, d, a, 12, 0x6ED9EBA1, 5);

	HASHSTEP(F4, a, b, c, d, 1,  0x8F1BBCDC, 11);
	HASHSTEP(F4, d, a, b, c, 9,  0x8F1BBCDC, 12);
	HASHSTEP(F4, c, d, a, b, 11, 0x8F1BBCDC, 14);
	HASHSTEP(F4, b, c, d, a, 10, 0x8F1BBCDC, 15);
	HASHSTEP(F4, a, b, c, d, 0,  0x8F1BBCDC, 14);
	HASHSTEP(F4, d, a, b, c, 8,  0x8F1BBCDC, 15);
	HASHSTEP(F4, c, d, a, b, 12, 0x8F1BBCDC, 9);
	HASHSTEP(F4, b, c, d, a, 4,  0x8F1BBCDC, 8);
	HASHSTEP(F4, a, b, c, d, 13, 0x8F1BBCDC, 9);
	HASHSTEP(F4, d, a, b, c, 3,  0x8F1BBCDC, 14);
	HASHSTEP(F4, c, d, a, b, 7,  0x8F1BBCDC, 5);
	HASHSTEP(F4, b, c, d, a, 15, 0x8F1BBCDC, 6);
	HASHSTEP(F4, a, b, c, d, 14, 0x8F1BBCDC, 8);
	HASHSTEP(F4, d, a, b, c, 5,  0x8F1BBCDC, 6);
	HASHSTEP(F4, c, d, a, b, 6,  0x8F1BBCDC, 5);
	HASHSTEP(F4, b, c, d, a, 2,  0x8F1BBCDC, 12);

#undef a
#undef b
#undef c
#undef d
#undef e

/*	A' = h0;
	B' = h1;
	C' = h2;
	D' = h3;
	E' = h4;


	T = rol_s'(j)(A' + f(79-j, B', C', D') + X[r'(j)] + K'(j)) + E';
	A = E';
	E' = D';
	D' = rol_10(C');
	C' = B';
	B' = T;
*/
#define a aVal2
#define b bVal2
#define c cVal2
#define d dVal2
#define e eVal2
	HASHSTEP(F4, a, b, c, d, 5,  0x50A28BE6, 8);
	HASHSTEP(F4, d, a, b, c, 14, 0x50A28BE6, 9);
	HASHSTEP(F4, c, d, a, b, 7,  0x50A28BE6, 9);
	HASHSTEP(F4, b, c, d, a, 0,  0x50A28BE6, 11);
	HASHSTEP(F4, a, b, c, d, 9,  0x50A28BE6, 13);
	HASHSTEP(F4, d, a, b, c, 2,  0x50A28BE6, 15);
	HASHSTEP(F4, c, d, a, b, 11, 0x50A28BE6, 15);
	HASHSTEP(F4, b, c, d, a, 4,  0x50A28BE6, 5);
	HASHSTEP(F4, a, b, c, d, 13, 0x50A28BE6, 7);
	HASHSTEP(F4, d, a, b, c, 6,  0x50A28BE6, 7);
	HASHSTEP(F4, c, d, a, b, 15, 0x50A28BE6, 8);
	HASHSTEP(F4, b, c, d, a, 8,  0x50A28BE6, 11);
	HASHSTEP(F4, a, b, c, d, 1,  0x50A28BE6, 14);
	HASHSTEP(F4, d, a, b, c, 10, 0x50A28BE6, 14);
	HASHSTEP(F4, c, d, a, b, 3,  0x50A28BE6, 12);
	HASHSTEP(F4, b, c, d, a, 12, 0x50A28BE6, 6);

	HASHSTEP(F3, a, b, c, d, 6,  0x5C4DD124, 9);
	HASHSTEP(F3, d, a, b, c, 11, 0x5C4DD124, 13);
	HASHSTEP(F3, c, d, a, b, 3,  0x5C4DD124, 15);
	HASHSTEP(F3, b, c, d, a, 7,  0x5C4DD124, 7);
	HASHSTEP(F3, a, b, c, d, 0,  0x5C4DD124, 12);
	HASHSTEP(F3, d, a, b, c, 13, 0x5C4DD124, 8);
	HASHSTEP(F3, c, d, a, b, 5,  0x5C4DD124, 9);
	HASHSTEP(F3, b, c, d, a, 10, 0x5C4DD124, 11);
	HASHSTEP(F3, a, b, c, d, 14, 0x5C4DD124, 7);
	HASHSTEP(F3, d, a, b, c, 15, 0x5C4DD124, 7);
	HASHSTEP(F3, c, d, a, b, 8,  0x5C4DD124, 12);
	HASHSTEP(F3, b, c, d, a, 12, 0x5C4DD124, 7);
	HASHSTEP(F3, a, b, c, d, 4,  0x5C4DD124, 6);
	HASHSTEP(F3, d, a, b, c, 9,  0x5C4DD124, 15);
	HASHSTEP(F3, c, d, a, b, 1,  0x5C4DD124, 13);
	HASHSTEP(F3, b, c, d, a, 2,  0x5C4DD124, 11);

	HASHSTEP(F2, a, b, c, d, 15, 0x6D703EF3, 9);
	HASHSTEP(F2, d, a, b, c, 5,  0x6D703EF3, 7);
	HASHSTEP(F2, c, d, a, b, 1,  0x6D703EF3, 15);
	HASHSTEP(F2, b, c, d, a, 3,  0x6D703EF3, 11);
	HASHSTEP(F2, a, b, c, d, 7,  0x6D703EF3, 8);
	HASHSTEP(F2, d, a, b, c, 14, 0x6D703EF3, 6);
	HASHSTEP(F2, c, d, a, b, 6,  0x6D703EF3, 6);
	HASHSTEP(F2, b, c, d, a, 9,  0x6D703EF3, 14);
	HASHSTEP(F2, a, b, c, d, 11, 0x6D703EF3, 12);
	HASHSTEP(F2, d, a, b, c, 8,  0x6D703EF3, 13);
	HASHSTEP(F2, c, d, a, b, 12, 0x6D703EF3, 5);
	HASHSTEP(F2, b, c, d, a, 2,  0x6D703EF3, 14);
	HASHSTEP(F2, a, b, c, d, 10, 0x6D703EF3, 13);
	HASHSTEP(F2, d, a, b, c, 0,  0x6D703EF3, 13);
	HASHSTEP(F2, c, d, a, b, 4,  0x6D703EF3, 7);
	HASHSTEP(F2, b, c, d, a, 13, 0x6D703EF3, 5);

	HASHSTEP(F1, a, b, c, d, 8,  0x00000000, 15);
	HASHSTEP(F1, d, a, b, c, 6,  0x00000000, 5);
	HASHSTEP(F1, c, d, a, b, 4,  0x00000000, 8);
	HASHSTEP(F1, b, c, d, a, 1,  0x00000000, 11);
	HASHSTEP(F1, a, b, c, d, 3,  0x00000000, 14);
	HASHSTEP(F1, d, a, b, c, 11, 0x00000000, 14);
	HASHSTEP(F1, c, d, a, b, 15, 0x00000000, 6);
	HASHSTEP(F1, b, c, d, a, 0,  0x00000000, 14);
	HASHSTEP(F1, a, b, c, d, 5,  0x00000000, 6);
	HASHSTEP(F1, d, a, b, c, 12, 0x00000000, 9);
	HASHSTEP(F1, c, d, a, b, 2,  0x00000000, 12);
	HASHSTEP(F1, b, c, d, a, 13, 0x00000000, 9);
	HASHSTEP(F1, a, b, c, d, 9,  0x00000000, 12);
	HASHSTEP(F1, d, a, b, c, 7,  0x00000000, 5);
	HASHSTEP(F1, c, d, a, b, 10, 0x00000000, 15);
	HASHSTEP(F1, b, c, d, a, 14, 0x00000000, 8);

#undef a
#undef b
#undef c
#undef d
#undef e

/*
        T := h1 + C + D';
		h1 := h2 + D + E';
		h2 := h3 + E + A';
        h3 := h4 + A + B';
		h4 := h0 + B + C';
		h0 := T;*/

	UInt32 t = keys[0];
	keys[0] = keys[1] + cVal + dVal2;
	keys[1] = keys[2] + dVal + aVal2;
	keys[2] = keys[3] + aVal + bVal2;
	keys[3] = t + bVal + cVal2;
}
#endif
