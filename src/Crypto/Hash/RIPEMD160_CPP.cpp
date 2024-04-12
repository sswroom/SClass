#include "Stdafx.h"
#include <stdlib.h>

/* #define F1(x, y, z) (x & y | ~x & z) */
//#define HASHSTEP(f, w, x, y, z, data, s) (w += f(x, y, z) + data, w = _rotl(w, s),  w += x)
#undef HAS_ASM32
#ifdef HAS_ASM32
#define F1(x, y, z) _asm {mov eax,x} _asm {xor eax,y} _asm {xor eax,z}
#define F2(x, y, z) _asm {mov eax,x} _asm {mov edx,eax} _asm {and eax,y} _asm {not edx} _asm {and edx,z} _asm {or eax,edx}
#define F3(x, y, z) _asm {mov eax,y} _asm {not eax} _asm{or eax,x} _asm {xor eax,z}
#define F4(x, y, z) _asm {mov eax,z} _asm {mov edx,eax} _asm {and eax,x} _asm {not edx} _asm {and edx,y} _asm {or eax,edx}
#define F5(x, y, z) _asm {mov eax,z} _asm {not eax} _asm {or eax,y} _asm {xor eax,x}

#define HASHSTEP(f, v, w, x, y, z, data, data2, s) f(w, x, y) _asm {add eax,v} _asm{add eax,dword ptr [edi + data * 4]} _asm {add eax, data2} _asm {rol eax, s} _asm {add eax,z} _asm {mov v,eax} _asm {rol x,10}
#define HASHSTEP0(f, v, w, x, y, z, data, data2, s) f(w, x, y) _asm {add eax,v} _asm{add eax,dword ptr [edi + data * 4]} _asm {rol eax, s} _asm {add eax,z} _asm {mov v,eax} _asm {rol x,10}
#define a ebx
#define b ecx
#define c esi
#define d dVal
#define e eVal

extern "C" void RMD160_CalcBlock(UInt32 *keys, const UInt8 *block)
{
	UInt32 eVal;
	UInt32 bVal;
	UInt32 firstVals[5];
	_asm
	{
		mov edi,keys
		mov ebx,dword ptr [edi]
		mov ecx,dword ptr [edi+4]
		mov esi,dword ptr [edi+8]
		mov eax,dword ptr [edi+12]
		mov edx,dword ptr [edi+16]
		mov dVal,eax
		mov eVal,edx
		mov edi,block
	}
/*	A = h0;
	B = h1;
	C = h2;
	D = h3;
	E = h4;

	T = rol_s(j)(A + f(j, B, C, D) + X[r(j)] + K(j)) + E;
	A = E;
	E = D;
	D = rol_10(C);
	C = B;
	B = T;*/

	HASHSTEP0(F1, a, b, c, d, e, 0,  0x00000000, 11);
	HASHSTEP0(F1, e, a, b, c, d, 1,  0x00000000, 14);
	HASHSTEP0(F1, d, e, a, b, c, 2,  0x00000000, 15);
	HASHSTEP0(F1, c, d, e, a, b, 3,  0x00000000, 12);
	HASHSTEP0(F1, b, c, d, e, a, 4,  0x00000000, 5);
	HASHSTEP0(F1, a, b, c, d, e, 5,  0x00000000, 8);
	HASHSTEP0(F1, e, a, b, c, d, 6,  0x00000000, 7);
	HASHSTEP0(F1, d, e, a, b, c, 7,  0x00000000, 9);
	HASHSTEP0(F1, c, d, e, a, b, 8,  0x00000000, 11);
	HASHSTEP0(F1, b, c, d, e, a, 9,  0x00000000, 13);
	HASHSTEP0(F1, a, b, c, d, e, 10, 0x00000000, 14);
	HASHSTEP0(F1, e, a, b, c, d, 11, 0x00000000, 15);
	HASHSTEP0(F1, d, e, a, b, c, 12, 0x00000000, 6);
	HASHSTEP0(F1, c, d, e, a, b, 13, 0x00000000, 7);
	HASHSTEP0(F1, b, c, d, e, a, 14, 0x00000000, 9);
	HASHSTEP0(F1, a, b, c, d, e, 15, 0x00000000, 8);
	HASHSTEP(F2, e, a, b, c, d, 7,  0x5A827999, 7);
	HASHSTEP(F2, d, e, a, b, c, 4,  0x5A827999, 6);
	HASHSTEP(F2, c, d, e, a, b, 13, 0x5A827999, 8);
	HASHSTEP(F2, b, c, d, e, a, 1,  0x5A827999, 13);

	HASHSTEP(F2, a, b, c, d, e, 10, 0x5A827999, 11);
	HASHSTEP(F2, e, a, b, c, d, 6,  0x5A827999, 9);
	HASHSTEP(F2, d, e, a, b, c, 15, 0x5A827999, 7);
	HASHSTEP(F2, c, d, e, a, b, 3,  0x5A827999, 15);
	HASHSTEP(F2, b, c, d, e, a, 12, 0x5A827999, 7);
	HASHSTEP(F2, a, b, c, d, e, 0,  0x5A827999, 12);
	HASHSTEP(F2, e, a, b, c, d, 9,  0x5A827999, 15);
	HASHSTEP(F2, d, e, a, b, c, 5,  0x5A827999, 9);
	HASHSTEP(F2, c, d, e, a, b, 2,  0x5A827999, 11);
	HASHSTEP(F2, b, c, d, e, a, 14, 0x5A827999, 7);
	HASHSTEP(F2, a, b, c, d, e, 11, 0x5A827999, 13);
	HASHSTEP(F2, e, a, b, c, d, 8,  0x5A827999, 12);
	HASHSTEP(F3, d, e, a, b, c, 3,  0x6ED9EBA1, 11);
	HASHSTEP(F3, c, d, e, a, b, 10, 0x6ED9EBA1, 13);
	HASHSTEP(F3, b, c, d, e, a, 14, 0x6ED9EBA1, 6);
	HASHSTEP(F3, a, b, c, d, e, 4,  0x6ED9EBA1, 7);
	HASHSTEP(F3, e, a, b, c, d, 9,  0x6ED9EBA1, 14);
	HASHSTEP(F3, d, e, a, b, c, 15, 0x6ED9EBA1, 9);
	HASHSTEP(F3, c, d, e, a, b, 8,  0x6ED9EBA1, 13);
	HASHSTEP(F3, b, c, d, e, a, 1,  0x6ED9EBA1, 15);

	HASHSTEP(F3, a, b, c, d, e, 2,  0x6ED9EBA1, 14);
	HASHSTEP(F3, e, a, b, c, d, 7,  0x6ED9EBA1, 8);
	HASHSTEP(F3, d, e, a, b, c, 0,  0x6ED9EBA1, 13);
	HASHSTEP(F3, c, d, e, a, b, 6,  0x6ED9EBA1, 6);
	HASHSTEP(F3, b, c, d, e, a, 13, 0x6ED9EBA1, 5);
	HASHSTEP(F3, a, b, c, d, e, 11, 0x6ED9EBA1, 12);
	HASHSTEP(F3, e, a, b, c, d, 5,  0x6ED9EBA1, 7);
	HASHSTEP(F3, d, e, a, b, c, 12, 0x6ED9EBA1, 5);
	HASHSTEP(F4, c, d, e, a, b, 1,  0x8F1BBCDC, 11);
	HASHSTEP(F4, b, c, d, e, a, 9,  0x8F1BBCDC, 12);
	HASHSTEP(F4, a, b, c, d, e, 11, 0x8F1BBCDC, 14);
	HASHSTEP(F4, e, a, b, c, d, 10, 0x8F1BBCDC, 15);
	HASHSTEP(F4, d, e, a, b, c, 0,  0x8F1BBCDC, 14);
	HASHSTEP(F4, c, d, e, a, b, 8,  0x8F1BBCDC, 15);
	HASHSTEP(F4, b, c, d, e, a, 12, 0x8F1BBCDC, 9);
	HASHSTEP(F4, a, b, c, d, e, 4,  0x8F1BBCDC, 8);
	HASHSTEP(F4, e, a, b, c, d, 13, 0x8F1BBCDC, 9);
	HASHSTEP(F4, d, e, a, b, c, 3,  0x8F1BBCDC, 14);
	HASHSTEP(F4, c, d, e, a, b, 7,  0x8F1BBCDC, 5);
	HASHSTEP(F4, b, c, d, e, a, 15, 0x8F1BBCDC, 6);

	HASHSTEP(F4, a, b, c, d, e, 14, 0x8F1BBCDC, 8);
	HASHSTEP(F4, e, a, b, c, d, 5,  0x8F1BBCDC, 6);
	HASHSTEP(F4, d, e, a, b, c, 6,  0x8F1BBCDC, 5);
	HASHSTEP(F4, c, d, e, a, b, 2,  0x8F1BBCDC, 12);
	HASHSTEP(F5, b, c, d, e, a, 4,  0xA953FD4E, 9);
	HASHSTEP(F5, a, b, c, d, e, 0,  0xA953FD4E, 15);
	HASHSTEP(F5, e, a, b, c, d, 5,  0xA953FD4E, 5);
	HASHSTEP(F5, d, e, a, b, c, 9,  0xA953FD4E, 11);
	HASHSTEP(F5, c, d, e, a, b, 7,  0xA953FD4E, 6);
	HASHSTEP(F5, b, c, d, e, a, 12, 0xA953FD4E, 8);
	HASHSTEP(F5, a, b, c, d, e, 2,  0xA953FD4E, 13);
	HASHSTEP(F5, e, a, b, c, d, 10, 0xA953FD4E, 12);
	HASHSTEP(F5, d, e, a, b, c, 14, 0xA953FD4E, 5);
	HASHSTEP(F5, c, d, e, a, b, 1,  0xA953FD4E, 12);
	HASHSTEP(F5, b, c, d, e, a, 3,  0xA953FD4E, 13);
	HASHSTEP(F5, a, b, c, d, e, 8,  0xA953FD4E, 14);
	HASHSTEP(F5, e, a, b, c, d, 11, 0xA953FD4E, 11);
	HASHSTEP(F5, d, e, a, b, c, 6,  0xA953FD4E, 8);
	HASHSTEP(F5, c, d, e, a, b, 15, 0xA953FD4E, 5);
	HASHSTEP(F5, b, c, d, e, a, 13, 0xA953FD4E, 6);

	_asm
	{
		mov eax,dVal
		mov edx,eVal
		mov dword ptr firstVals[0],ebx
		mov dword ptr firstVals[4],ecx
		mov dword ptr firstVals[8],esi
		mov dword ptr firstVals[12],eax
		mov dword ptr firstVals[16],edx
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
		mov eax,dword ptr [edi+12]
		mov edx,dword ptr [edi+16]
		mov dVal,eax
		mov eVal,edx
		mov edi,block
	}

	HASHSTEP(F5, a, b, c, d, e, 5,  0x50A28BE6, 8);
	HASHSTEP(F5, e, a, b, c, d, 14, 0x50A28BE6, 9);
	HASHSTEP(F5, d, e, a, b, c, 7,  0x50A28BE6, 9);
	HASHSTEP(F5, c, d, e, a, b, 0,  0x50A28BE6, 11);
	HASHSTEP(F5, b, c, d, e, a, 9,  0x50A28BE6, 13);
	HASHSTEP(F5, a, b, c, d, e, 2,  0x50A28BE6, 15);
	HASHSTEP(F5, e, a, b, c, d, 11, 0x50A28BE6, 15);
	HASHSTEP(F5, d, e, a, b, c, 4,  0x50A28BE6, 5);
	HASHSTEP(F5, c, d, e, a, b, 13, 0x50A28BE6, 7);
	HASHSTEP(F5, b, c, d, e, a, 6,  0x50A28BE6, 7);
	HASHSTEP(F5, a, b, c, d, e, 15, 0x50A28BE6, 8);
	HASHSTEP(F5, e, a, b, c, d, 8,  0x50A28BE6, 11);
	HASHSTEP(F5, d, e, a, b, c, 1,  0x50A28BE6, 14);
	HASHSTEP(F5, c, d, e, a, b, 10, 0x50A28BE6, 14);
	HASHSTEP(F5, b, c, d, e, a, 3,  0x50A28BE6, 12);
	HASHSTEP(F5, a, b, c, d, e, 12, 0x50A28BE6, 6);
	HASHSTEP(F4, e, a, b, c, d, 6,  0x5C4DD124, 9);
	HASHSTEP(F4, d, e, a, b, c, 11, 0x5C4DD124, 13);
	HASHSTEP(F4, c, d, e, a, b, 3,  0x5C4DD124, 15);
	HASHSTEP(F4, b, c, d, e, a, 7,  0x5C4DD124, 7);

	HASHSTEP(F4, a, b, c, d, e, 0,  0x5C4DD124, 12);
	HASHSTEP(F4, e, a, b, c, d, 13, 0x5C4DD124, 8);
	HASHSTEP(F4, d, e, a, b, c, 5,  0x5C4DD124, 9);
	HASHSTEP(F4, c, d, e, a, b, 10, 0x5C4DD124, 11);
	HASHSTEP(F4, b, c, d, e, a, 14, 0x5C4DD124, 7);
	HASHSTEP(F4, a, b, c, d, e, 15, 0x5C4DD124, 7);
	HASHSTEP(F4, e, a, b, c, d, 8,  0x5C4DD124, 12);
	HASHSTEP(F4, d, e, a, b, c, 12, 0x5C4DD124, 7);
	HASHSTEP(F4, c, d, e, a, b, 4,  0x5C4DD124, 6);
	HASHSTEP(F4, b, c, d, e, a, 9,  0x5C4DD124, 15);
	HASHSTEP(F4, a, b, c, d, e, 1,  0x5C4DD124, 13);
	HASHSTEP(F4, e, a, b, c, d, 2,  0x5C4DD124, 11);
	HASHSTEP(F3, d, e, a, b, c, 15, 0x6D703EF3, 9);
	HASHSTEP(F3, c, d, e, a, b, 5,  0x6D703EF3, 7);
	HASHSTEP(F3, b, c, d, e, a, 1,  0x6D703EF3, 15);
	HASHSTEP(F3, a, b, c, d, e, 3,  0x6D703EF3, 11);
	HASHSTEP(F3, e, a, b, c, d, 7,  0x6D703EF3, 8);
	HASHSTEP(F3, d, e, a, b, c, 14, 0x6D703EF3, 6);
	HASHSTEP(F3, c, d, e, a, b, 6,  0x6D703EF3, 6);
	HASHSTEP(F3, b, c, d, e, a, 9,  0x6D703EF3, 14);

	HASHSTEP(F3, a, b, c, d, e, 11, 0x6D703EF3, 12);
	HASHSTEP(F3, e, a, b, c, d, 8,  0x6D703EF3, 13);
	HASHSTEP(F3, d, e, a, b, c, 12, 0x6D703EF3, 5);
	HASHSTEP(F3, c, d, e, a, b, 2,  0x6D703EF3, 14);
	HASHSTEP(F3, b, c, d, e, a, 10, 0x6D703EF3, 13);
	HASHSTEP(F3, a, b, c, d, e, 0,  0x6D703EF3, 13);
	HASHSTEP(F3, e, a, b, c, d, 4,  0x6D703EF3, 7);
	HASHSTEP(F3, d, e, a, b, c, 13, 0x6D703EF3, 5);
	HASHSTEP(F2, c, d, e, a, b, 8,  0x7A6D76E9, 15);
	HASHSTEP(F2, b, c, d, e, a, 6,  0x7A6D76E9, 5);
	HASHSTEP(F2, a, b, c, d, e, 4,  0x7A6D76E9, 8);
	HASHSTEP(F2, e, a, b, c, d, 1,  0x7A6D76E9, 11);
	HASHSTEP(F2, d, e, a, b, c, 3,  0x7A6D76E9, 14);
	HASHSTEP(F2, c, d, e, a, b, 11, 0x7A6D76E9, 14);
	HASHSTEP(F2, b, c, d, e, a, 15, 0x7A6D76E9, 6);
	HASHSTEP(F2, a, b, c, d, e, 0,  0x7A6D76E9, 14);
	HASHSTEP(F2, e, a, b, c, d, 5,  0x7A6D76E9, 6);
	HASHSTEP(F2, d, e, a, b, c, 12, 0x7A6D76E9, 9);
	HASHSTEP(F2, c, d, e, a, b, 2,  0x7A6D76E9, 12);
	HASHSTEP(F2, b, c, d, e, a, 13, 0x7A6D76E9, 9);

	HASHSTEP(F2, a, b, c, d, e, 9,  0x7A6D76E9, 12);
	HASHSTEP(F2, e, a, b, c, d, 7,  0x7A6D76E9, 5);
	HASHSTEP(F2, d, e, a, b, c, 10, 0x7A6D76E9, 15);
	HASHSTEP(F2, c, d, e, a, b, 14, 0x7A6D76E9, 8);
	HASHSTEP0(F1, b, c, d, e, a, 12, 0x00000000, 8);
	HASHSTEP0(F1, a, b, c, d, e, 15, 0x00000000, 5);
	HASHSTEP0(F1, e, a, b, c, d, 10, 0x00000000, 12);
	HASHSTEP0(F1, d, e, a, b, c, 4,  0x00000000, 9);
	HASHSTEP0(F1, c, d, e, a, b, 1,  0x00000000, 12);
	HASHSTEP0(F1, b, c, d, e, a, 5,  0x00000000, 5);
	HASHSTEP0(F1, a, b, c, d, e, 8,  0x00000000, 14);
	HASHSTEP0(F1, e, a, b, c, d, 7,  0x00000000, 6);
	HASHSTEP0(F1, d, e, a, b, c, 6,  0x00000000, 8);
	HASHSTEP0(F1, c, d, e, a, b, 2,  0x00000000, 13);
	HASHSTEP0(F1, b, c, d, e, a, 13, 0x00000000, 6);
	HASHSTEP0(F1, a, b, c, d, e, 14, 0x00000000, 5);
	HASHSTEP0(F1, e, a, b, c, d, 0,  0x00000000, 15);
	HASHSTEP0(F1, d, e, a, b, c, 3,  0x00000000, 13);
	HASHSTEP0(F1, c, d, e, a, b, 9,  0x00000000, 11);
	HASHSTEP0(F1, b, c, d, e, a, 11, 0x00000000, 11);

/*
        T := h1 + C + D';
		h1 := h2 + D + E';
		h2 := h3 + E + A';
        h3 := h4 + A + B';
		h4 := h0 + B + C';
		h0 := T;*/

	_asm
	{
		mov edi,keys
		mov edx,dword ptr [edi]

		mov eax,dword ptr [edi+4]
		add eax,firstVals[8]
		add eax,dVal
		mov dword ptr [edi],eax
		
		mov eax,dword ptr [edi+8]
		add eax,firstVals[12]
		add eax,eVal
		mov dword ptr [edi+4],eax

		mov eax,dword ptr [edi+12]
		add eax,firstVals[16]
		add eax,ebx
		mov dword ptr [edi+8],eax

		mov eax,dword ptr [edi+16]
		add eax,firstVals[0]
		add eax,ecx
		mov dword ptr [edi+12],eax

		mov eax,edx
		add eax,firstVals[4]
		add eax,esi
		mov dword ptr [edi+16],eax
	}
}

#elif 1
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
#define F5(x, y, z) (x ^ (y | ~z))
#define HASHSTEP(f, v, w, x, y, z, data, data2, s) {v = z + _rotl(v + f(w, x, y) + *(UInt32*)&block[data * 4] + data2, s);  x = _rotl(x, 10);}

extern "C" void RMD160_CalcBlock(UInt32 *keys, const UInt8 *block)
{
	UInt32 aVal;
	UInt32 bVal;
	UInt32 cVal;
	UInt32 eVal;
	UInt32 dVal;
	UInt32 aVal2;
	UInt32 bVal2;
	UInt32 cVal2;
	UInt32 eVal2;
	UInt32 dVal2;
	aVal = aVal2 = keys[0];
	bVal = bVal2 = keys[1];
	cVal = cVal2 = keys[2];
	dVal = dVal2 = keys[3];
	eVal = eVal2 = keys[4];

/*	A = h0;
	B = h1;
	C = h2;
	D = h3;
	E = h4;

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
#define e eVal

	HASHSTEP(F1, a, b, c, d, e, 0,  0x00000000, 11);
	HASHSTEP(F1, e, a, b, c, d, 1,  0x00000000, 14);
	HASHSTEP(F1, d, e, a, b, c, 2,  0x00000000, 15);
	HASHSTEP(F1, c, d, e, a, b, 3,  0x00000000, 12);
	HASHSTEP(F1, b, c, d, e, a, 4,  0x00000000, 5);
	HASHSTEP(F1, a, b, c, d, e, 5,  0x00000000, 8);
	HASHSTEP(F1, e, a, b, c, d, 6,  0x00000000, 7);
	HASHSTEP(F1, d, e, a, b, c, 7,  0x00000000, 9);
	HASHSTEP(F1, c, d, e, a, b, 8,  0x00000000, 11);
	HASHSTEP(F1, b, c, d, e, a, 9,  0x00000000, 13);
	HASHSTEP(F1, a, b, c, d, e, 10, 0x00000000, 14);
	HASHSTEP(F1, e, a, b, c, d, 11, 0x00000000, 15);
	HASHSTEP(F1, d, e, a, b, c, 12, 0x00000000, 6);
	HASHSTEP(F1, c, d, e, a, b, 13, 0x00000000, 7);
	HASHSTEP(F1, b, c, d, e, a, 14, 0x00000000, 9);
	HASHSTEP(F1, a, b, c, d, e, 15, 0x00000000, 8);
	HASHSTEP(F2, e, a, b, c, d, 7,  0x5A827999, 7);
	HASHSTEP(F2, d, e, a, b, c, 4,  0x5A827999, 6);
	HASHSTEP(F2, c, d, e, a, b, 13, 0x5A827999, 8);
	HASHSTEP(F2, b, c, d, e, a, 1,  0x5A827999, 13);

	HASHSTEP(F2, a, b, c, d, e, 10, 0x5A827999, 11);
	HASHSTEP(F2, e, a, b, c, d, 6,  0x5A827999, 9);
	HASHSTEP(F2, d, e, a, b, c, 15, 0x5A827999, 7);
	HASHSTEP(F2, c, d, e, a, b, 3,  0x5A827999, 15);
	HASHSTEP(F2, b, c, d, e, a, 12, 0x5A827999, 7);
	HASHSTEP(F2, a, b, c, d, e, 0,  0x5A827999, 12);
	HASHSTEP(F2, e, a, b, c, d, 9,  0x5A827999, 15);
	HASHSTEP(F2, d, e, a, b, c, 5,  0x5A827999, 9);
	HASHSTEP(F2, c, d, e, a, b, 2,  0x5A827999, 11);
	HASHSTEP(F2, b, c, d, e, a, 14, 0x5A827999, 7);
	HASHSTEP(F2, a, b, c, d, e, 11, 0x5A827999, 13);
	HASHSTEP(F2, e, a, b, c, d, 8,  0x5A827999, 12);
	HASHSTEP(F3, d, e, a, b, c, 3,  0x6ED9EBA1, 11);
	HASHSTEP(F3, c, d, e, a, b, 10, 0x6ED9EBA1, 13);
	HASHSTEP(F3, b, c, d, e, a, 14, 0x6ED9EBA1, 6);
	HASHSTEP(F3, a, b, c, d, e, 4,  0x6ED9EBA1, 7);
	HASHSTEP(F3, e, a, b, c, d, 9,  0x6ED9EBA1, 14);
	HASHSTEP(F3, d, e, a, b, c, 15, 0x6ED9EBA1, 9);
	HASHSTEP(F3, c, d, e, a, b, 8,  0x6ED9EBA1, 13);
	HASHSTEP(F3, b, c, d, e, a, 1,  0x6ED9EBA1, 15);

	HASHSTEP(F3, a, b, c, d, e, 2,  0x6ED9EBA1, 14);
	HASHSTEP(F3, e, a, b, c, d, 7,  0x6ED9EBA1, 8);
	HASHSTEP(F3, d, e, a, b, c, 0,  0x6ED9EBA1, 13);
	HASHSTEP(F3, c, d, e, a, b, 6,  0x6ED9EBA1, 6);
	HASHSTEP(F3, b, c, d, e, a, 13, 0x6ED9EBA1, 5);
	HASHSTEP(F3, a, b, c, d, e, 11, 0x6ED9EBA1, 12);
	HASHSTEP(F3, e, a, b, c, d, 5,  0x6ED9EBA1, 7);
	HASHSTEP(F3, d, e, a, b, c, 12, 0x6ED9EBA1, 5);
	HASHSTEP(F4, c, d, e, a, b, 1,  0x8F1BBCDC, 11);
	HASHSTEP(F4, b, c, d, e, a, 9,  0x8F1BBCDC, 12);
	HASHSTEP(F4, a, b, c, d, e, 11, 0x8F1BBCDC, 14);
	HASHSTEP(F4, e, a, b, c, d, 10, 0x8F1BBCDC, 15);
	HASHSTEP(F4, d, e, a, b, c, 0,  0x8F1BBCDC, 14);
	HASHSTEP(F4, c, d, e, a, b, 8,  0x8F1BBCDC, 15);
	HASHSTEP(F4, b, c, d, e, a, 12, 0x8F1BBCDC, 9);
	HASHSTEP(F4, a, b, c, d, e, 4,  0x8F1BBCDC, 8);
	HASHSTEP(F4, e, a, b, c, d, 13, 0x8F1BBCDC, 9);
	HASHSTEP(F4, d, e, a, b, c, 3,  0x8F1BBCDC, 14);
	HASHSTEP(F4, c, d, e, a, b, 7,  0x8F1BBCDC, 5);
	HASHSTEP(F4, b, c, d, e, a, 15, 0x8F1BBCDC, 6);

	HASHSTEP(F4, a, b, c, d, e, 14, 0x8F1BBCDC, 8);
	HASHSTEP(F4, e, a, b, c, d, 5,  0x8F1BBCDC, 6);
	HASHSTEP(F4, d, e, a, b, c, 6,  0x8F1BBCDC, 5);
	HASHSTEP(F4, c, d, e, a, b, 2,  0x8F1BBCDC, 12);
	HASHSTEP(F5, b, c, d, e, a, 4,  0xA953FD4E, 9);
	HASHSTEP(F5, a, b, c, d, e, 0,  0xA953FD4E, 15);
	HASHSTEP(F5, e, a, b, c, d, 5,  0xA953FD4E, 5);
	HASHSTEP(F5, d, e, a, b, c, 9,  0xA953FD4E, 11);
	HASHSTEP(F5, c, d, e, a, b, 7,  0xA953FD4E, 6);
	HASHSTEP(F5, b, c, d, e, a, 12, 0xA953FD4E, 8);
	HASHSTEP(F5, a, b, c, d, e, 2,  0xA953FD4E, 13);
	HASHSTEP(F5, e, a, b, c, d, 10, 0xA953FD4E, 12);
	HASHSTEP(F5, d, e, a, b, c, 14, 0xA953FD4E, 5);
	HASHSTEP(F5, c, d, e, a, b, 1,  0xA953FD4E, 12);
	HASHSTEP(F5, b, c, d, e, a, 3,  0xA953FD4E, 13);
	HASHSTEP(F5, a, b, c, d, e, 8,  0xA953FD4E, 14);
	HASHSTEP(F5, e, a, b, c, d, 11, 0xA953FD4E, 11);
	HASHSTEP(F5, d, e, a, b, c, 6,  0xA953FD4E, 8);
	HASHSTEP(F5, c, d, e, a, b, 15, 0xA953FD4E, 5);
	HASHSTEP(F5, b, c, d, e, a, 13, 0xA953FD4E, 6);

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

	HASHSTEP(F5, a, b, c, d, e, 5,  0x50A28BE6, 8);
	HASHSTEP(F5, e, a, b, c, d, 14, 0x50A28BE6, 9);
	HASHSTEP(F5, d, e, a, b, c, 7,  0x50A28BE6, 9);
	HASHSTEP(F5, c, d, e, a, b, 0,  0x50A28BE6, 11);
	HASHSTEP(F5, b, c, d, e, a, 9,  0x50A28BE6, 13);
	HASHSTEP(F5, a, b, c, d, e, 2,  0x50A28BE6, 15);
	HASHSTEP(F5, e, a, b, c, d, 11, 0x50A28BE6, 15);
	HASHSTEP(F5, d, e, a, b, c, 4,  0x50A28BE6, 5);
	HASHSTEP(F5, c, d, e, a, b, 13, 0x50A28BE6, 7);
	HASHSTEP(F5, b, c, d, e, a, 6,  0x50A28BE6, 7);
	HASHSTEP(F5, a, b, c, d, e, 15, 0x50A28BE6, 8);
	HASHSTEP(F5, e, a, b, c, d, 8,  0x50A28BE6, 11);
	HASHSTEP(F5, d, e, a, b, c, 1,  0x50A28BE6, 14);
	HASHSTEP(F5, c, d, e, a, b, 10, 0x50A28BE6, 14);
	HASHSTEP(F5, b, c, d, e, a, 3,  0x50A28BE6, 12);
	HASHSTEP(F5, a, b, c, d, e, 12, 0x50A28BE6, 6);
	HASHSTEP(F4, e, a, b, c, d, 6,  0x5C4DD124, 9);
	HASHSTEP(F4, d, e, a, b, c, 11, 0x5C4DD124, 13);
	HASHSTEP(F4, c, d, e, a, b, 3,  0x5C4DD124, 15);
	HASHSTEP(F4, b, c, d, e, a, 7,  0x5C4DD124, 7);

	HASHSTEP(F4, a, b, c, d, e, 0,  0x5C4DD124, 12);
	HASHSTEP(F4, e, a, b, c, d, 13, 0x5C4DD124, 8);
	HASHSTEP(F4, d, e, a, b, c, 5,  0x5C4DD124, 9);
	HASHSTEP(F4, c, d, e, a, b, 10, 0x5C4DD124, 11);
	HASHSTEP(F4, b, c, d, e, a, 14, 0x5C4DD124, 7);
	HASHSTEP(F4, a, b, c, d, e, 15, 0x5C4DD124, 7);
	HASHSTEP(F4, e, a, b, c, d, 8,  0x5C4DD124, 12);
	HASHSTEP(F4, d, e, a, b, c, 12, 0x5C4DD124, 7);
	HASHSTEP(F4, c, d, e, a, b, 4,  0x5C4DD124, 6);
	HASHSTEP(F4, b, c, d, e, a, 9,  0x5C4DD124, 15);
	HASHSTEP(F4, a, b, c, d, e, 1,  0x5C4DD124, 13);
	HASHSTEP(F4, e, a, b, c, d, 2,  0x5C4DD124, 11);
	HASHSTEP(F3, d, e, a, b, c, 15, 0x6D703EF3, 9);
	HASHSTEP(F3, c, d, e, a, b, 5,  0x6D703EF3, 7);
	HASHSTEP(F3, b, c, d, e, a, 1,  0x6D703EF3, 15);
	HASHSTEP(F3, a, b, c, d, e, 3,  0x6D703EF3, 11);
	HASHSTEP(F3, e, a, b, c, d, 7,  0x6D703EF3, 8);
	HASHSTEP(F3, d, e, a, b, c, 14, 0x6D703EF3, 6);
	HASHSTEP(F3, c, d, e, a, b, 6,  0x6D703EF3, 6);
	HASHSTEP(F3, b, c, d, e, a, 9,  0x6D703EF3, 14);

	HASHSTEP(F3, a, b, c, d, e, 11, 0x6D703EF3, 12);
	HASHSTEP(F3, e, a, b, c, d, 8,  0x6D703EF3, 13);
	HASHSTEP(F3, d, e, a, b, c, 12, 0x6D703EF3, 5);
	HASHSTEP(F3, c, d, e, a, b, 2,  0x6D703EF3, 14);
	HASHSTEP(F3, b, c, d, e, a, 10, 0x6D703EF3, 13);
	HASHSTEP(F3, a, b, c, d, e, 0,  0x6D703EF3, 13);
	HASHSTEP(F3, e, a, b, c, d, 4,  0x6D703EF3, 7);
	HASHSTEP(F3, d, e, a, b, c, 13, 0x6D703EF3, 5);
	HASHSTEP(F2, c, d, e, a, b, 8,  0x7A6D76E9, 15);
	HASHSTEP(F2, b, c, d, e, a, 6,  0x7A6D76E9, 5);
	HASHSTEP(F2, a, b, c, d, e, 4,  0x7A6D76E9, 8);
	HASHSTEP(F2, e, a, b, c, d, 1,  0x7A6D76E9, 11);
	HASHSTEP(F2, d, e, a, b, c, 3,  0x7A6D76E9, 14);
	HASHSTEP(F2, c, d, e, a, b, 11, 0x7A6D76E9, 14);
	HASHSTEP(F2, b, c, d, e, a, 15, 0x7A6D76E9, 6);
	HASHSTEP(F2, a, b, c, d, e, 0,  0x7A6D76E9, 14);
	HASHSTEP(F2, e, a, b, c, d, 5,  0x7A6D76E9, 6);
	HASHSTEP(F2, d, e, a, b, c, 12, 0x7A6D76E9, 9);
	HASHSTEP(F2, c, d, e, a, b, 2,  0x7A6D76E9, 12);
	HASHSTEP(F2, b, c, d, e, a, 13, 0x7A6D76E9, 9);

	HASHSTEP(F2, a, b, c, d, e, 9,  0x7A6D76E9, 12);
	HASHSTEP(F2, e, a, b, c, d, 7,  0x7A6D76E9, 5);
	HASHSTEP(F2, d, e, a, b, c, 10, 0x7A6D76E9, 15);
	HASHSTEP(F2, c, d, e, a, b, 14, 0x7A6D76E9, 8);
	HASHSTEP(F1, b, c, d, e, a, 12, 0x00000000, 8);
	HASHSTEP(F1, a, b, c, d, e, 15, 0x00000000, 5);
	HASHSTEP(F1, e, a, b, c, d, 10, 0x00000000, 12);
	HASHSTEP(F1, d, e, a, b, c, 4,  0x00000000, 9);
	HASHSTEP(F1, c, d, e, a, b, 1,  0x00000000, 12);
	HASHSTEP(F1, b, c, d, e, a, 5,  0x00000000, 5);
	HASHSTEP(F1, a, b, c, d, e, 8,  0x00000000, 14);
	HASHSTEP(F1, e, a, b, c, d, 7,  0x00000000, 6);
	HASHSTEP(F1, d, e, a, b, c, 6,  0x00000000, 8);
	HASHSTEP(F1, c, d, e, a, b, 2,  0x00000000, 13);
	HASHSTEP(F1, b, c, d, e, a, 13, 0x00000000, 6);
	HASHSTEP(F1, a, b, c, d, e, 14, 0x00000000, 5);
	HASHSTEP(F1, e, a, b, c, d, 0,  0x00000000, 15);
	HASHSTEP(F1, d, e, a, b, c, 3,  0x00000000, 13);
	HASHSTEP(F1, c, d, e, a, b, 9,  0x00000000, 11);
	HASHSTEP(F1, b, c, d, e, a, 11, 0x00000000, 11);

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
	keys[1] = keys[2] + dVal + eVal2;
	keys[2] = keys[3] + eVal + aVal2;
	keys[3] = keys[4] + aVal + bVal2;
	keys[4] = t + bVal + cVal2;
}
#else
extern "C" void RMD160_CalcBlock(UInt32 *keys, const UInt8 *block)
{
    UInt32 a,b,c,d,e,aa,bb,cc,dd,ee,t;
 
#define K0  0x00000000
#define K1  0x5A827999
#define K2  0x6ED9EBA1
#define K3  0x8F1BBCDC
#define K4  0xA953FD4E
#define KK0 0x50A28BE6
#define KK1 0x5C4DD124
#define KK2 0x6D703EF3
#define KK3 0x7A6D76E9
#define KK4 0x00000000
#define F0(x,y,z)   ( (x) ^ (y) ^ (z) )
#define F1(x,y,z)   ( ((x) & (y)) | (~(x) & (z)) )
#define F2(x,y,z)   ( ((x) | ~(y)) ^ (z) )
#define F3(x,y,z)   ( ((x) & (z)) | ((y) & ~(z)) )
#define F4(x,y,z)   ( (x) ^ ((y) | ~(z)) )
#define R(a,b,c,d,e,f,k,r,s) do { t = a + f(b,c,d) + k + *(UInt32*)&block[r * 4]; \
				  a = _rotl(t,s) + e;	       \
				  c = _rotl(c,10);	       \
				} while(0)

    /* left lane */
    a = keys[0];
    b = keys[1];
    c = keys[2];
    d = keys[3];
    e = keys[4];
    R( a, b, c, d, e, F0, K0,  0, 11 );
    R( e, a, b, c, d, F0, K0,  1, 14 );
    R( d, e, a, b, c, F0, K0,  2, 15 );
    R( c, d, e, a, b, F0, K0,  3, 12 );
    R( b, c, d, e, a, F0, K0,  4,  5 );
    R( a, b, c, d, e, F0, K0,  5,  8 );
    R( e, a, b, c, d, F0, K0,  6,  7 );
    R( d, e, a, b, c, F0, K0,  7,  9 );
    R( c, d, e, a, b, F0, K0,  8, 11 );
    R( b, c, d, e, a, F0, K0,  9, 13 );
    R( a, b, c, d, e, F0, K0, 10, 14 );
    R( e, a, b, c, d, F0, K0, 11, 15 );
    R( d, e, a, b, c, F0, K0, 12,  6 );
    R( c, d, e, a, b, F0, K0, 13,  7 );
    R( b, c, d, e, a, F0, K0, 14,  9 );
    R( a, b, c, d, e, F0, K0, 15,  8 );
    R( e, a, b, c, d, F1, K1,  7,  7 );
    R( d, e, a, b, c, F1, K1,  4,  6 );
    R( c, d, e, a, b, F1, K1, 13,  8 );
    R( b, c, d, e, a, F1, K1,  1, 13 );
    R( a, b, c, d, e, F1, K1, 10, 11 );
    R( e, a, b, c, d, F1, K1,  6,  9 );
    R( d, e, a, b, c, F1, K1, 15,  7 );
    R( c, d, e, a, b, F1, K1,  3, 15 );
    R( b, c, d, e, a, F1, K1, 12,  7 );
    R( a, b, c, d, e, F1, K1,  0, 12 );
    R( e, a, b, c, d, F1, K1,  9, 15 );
    R( d, e, a, b, c, F1, K1,  5,  9 );
    R( c, d, e, a, b, F1, K1,  2, 11 );
    R( b, c, d, e, a, F1, K1, 14,  7 );
    R( a, b, c, d, e, F1, K1, 11, 13 );
    R( e, a, b, c, d, F1, K1,  8, 12 );
    R( d, e, a, b, c, F2, K2,  3, 11 );
    R( c, d, e, a, b, F2, K2, 10, 13 );
    R( b, c, d, e, a, F2, K2, 14,  6 );
    R( a, b, c, d, e, F2, K2,  4,  7 );
    R( e, a, b, c, d, F2, K2,  9, 14 );
    R( d, e, a, b, c, F2, K2, 15,  9 );
    R( c, d, e, a, b, F2, K2,  8, 13 );
    R( b, c, d, e, a, F2, K2,  1, 15 );
    R( a, b, c, d, e, F2, K2,  2, 14 );
    R( e, a, b, c, d, F2, K2,  7,  8 );
    R( d, e, a, b, c, F2, K2,  0, 13 );
    R( c, d, e, a, b, F2, K2,  6,  6 );
    R( b, c, d, e, a, F2, K2, 13,  5 );
    R( a, b, c, d, e, F2, K2, 11, 12 );
    R( e, a, b, c, d, F2, K2,  5,  7 );
    R( d, e, a, b, c, F2, K2, 12,  5 );
    R( c, d, e, a, b, F3, K3,  1, 11 );
    R( b, c, d, e, a, F3, K3,  9, 12 );
    R( a, b, c, d, e, F3, K3, 11, 14 );
    R( e, a, b, c, d, F3, K3, 10, 15 );
    R( d, e, a, b, c, F3, K3,  0, 14 );
    R( c, d, e, a, b, F3, K3,  8, 15 );
    R( b, c, d, e, a, F3, K3, 12,  9 );
    R( a, b, c, d, e, F3, K3,  4,  8 );
    R( e, a, b, c, d, F3, K3, 13,  9 );
    R( d, e, a, b, c, F3, K3,  3, 14 );
    R( c, d, e, a, b, F3, K3,  7,  5 );
    R( b, c, d, e, a, F3, K3, 15,  6 );
    R( a, b, c, d, e, F3, K3, 14,  8 );
    R( e, a, b, c, d, F3, K3,  5,  6 );
    R( d, e, a, b, c, F3, K3,  6,  5 );
    R( c, d, e, a, b, F3, K3,  2, 12 );
    R( b, c, d, e, a, F4, K4,  4,  9 );
    R( a, b, c, d, e, F4, K4,  0, 15 );
    R( e, a, b, c, d, F4, K4,  5,  5 );
    R( d, e, a, b, c, F4, K4,  9, 11 );
    R( c, d, e, a, b, F4, K4,  7,  6 );
    R( b, c, d, e, a, F4, K4, 12,  8 );
    R( a, b, c, d, e, F4, K4,  2, 13 );
    R( e, a, b, c, d, F4, K4, 10, 12 );
    R( d, e, a, b, c, F4, K4, 14,  5 );
    R( c, d, e, a, b, F4, K4,  1, 12 );
    R( b, c, d, e, a, F4, K4,  3, 13 );
    R( a, b, c, d, e, F4, K4,  8, 14 );
    R( e, a, b, c, d, F4, K4, 11, 11 );
    R( d, e, a, b, c, F4, K4,  6,  8 );
    R( c, d, e, a, b, F4, K4, 15,  5 );
    R( b, c, d, e, a, F4, K4, 13,  6 );

    aa = a; bb = b; cc = c; dd = d; ee = e;

    /* right lane */
    a = keys[0];
    b = keys[1];
    c = keys[2];
    d = keys[3];
    e = keys[4];
    R( a, b, c, d, e, F4, KK0,	5,  8);
    R( e, a, b, c, d, F4, KK0, 14,  9);
    R( d, e, a, b, c, F4, KK0,	7,  9);
    R( c, d, e, a, b, F4, KK0,	0, 11);
    R( b, c, d, e, a, F4, KK0,	9, 13);
    R( a, b, c, d, e, F4, KK0,	2, 15);
    R( e, a, b, c, d, F4, KK0, 11, 15);
    R( d, e, a, b, c, F4, KK0,	4,  5);
    R( c, d, e, a, b, F4, KK0, 13,  7);
    R( b, c, d, e, a, F4, KK0,	6,  7);
    R( a, b, c, d, e, F4, KK0, 15,  8);
    R( e, a, b, c, d, F4, KK0,	8, 11);
    R( d, e, a, b, c, F4, KK0,	1, 14);
    R( c, d, e, a, b, F4, KK0, 10, 14);
    R( b, c, d, e, a, F4, KK0,	3, 12);
    R( a, b, c, d, e, F4, KK0, 12,  6);
    R( e, a, b, c, d, F3, KK1,	6,  9);
    R( d, e, a, b, c, F3, KK1, 11, 13);
    R( c, d, e, a, b, F3, KK1,	3, 15);
    R( b, c, d, e, a, F3, KK1,	7,  7);
    R( a, b, c, d, e, F3, KK1,	0, 12);
    R( e, a, b, c, d, F3, KK1, 13,  8);
    R( d, e, a, b, c, F3, KK1,	5,  9);
    R( c, d, e, a, b, F3, KK1, 10, 11);
    R( b, c, d, e, a, F3, KK1, 14,  7);
    R( a, b, c, d, e, F3, KK1, 15,  7);
    R( e, a, b, c, d, F3, KK1,	8, 12);
    R( d, e, a, b, c, F3, KK1, 12,  7);
    R( c, d, e, a, b, F3, KK1,	4,  6);
    R( b, c, d, e, a, F3, KK1,	9, 15);
    R( a, b, c, d, e, F3, KK1,	1, 13);
    R( e, a, b, c, d, F3, KK1,	2, 11);
    R( d, e, a, b, c, F2, KK2, 15,  9);
    R( c, d, e, a, b, F2, KK2,	5,  7);
    R( b, c, d, e, a, F2, KK2,	1, 15);
    R( a, b, c, d, e, F2, KK2,	3, 11);
    R( e, a, b, c, d, F2, KK2,	7,  8);
    R( d, e, a, b, c, F2, KK2, 14,  6);
    R( c, d, e, a, b, F2, KK2,	6,  6);
    R( b, c, d, e, a, F2, KK2,	9, 14);
    R( a, b, c, d, e, F2, KK2, 11, 12);
    R( e, a, b, c, d, F2, KK2,	8, 13);
    R( d, e, a, b, c, F2, KK2, 12,  5);
    R( c, d, e, a, b, F2, KK2,	2, 14);
    R( b, c, d, e, a, F2, KK2, 10, 13);
    R( a, b, c, d, e, F2, KK2,	0, 13);
    R( e, a, b, c, d, F2, KK2,	4,  7);
    R( d, e, a, b, c, F2, KK2, 13,  5);
    R( c, d, e, a, b, F1, KK3,	8, 15);
    R( b, c, d, e, a, F1, KK3,	6,  5);
    R( a, b, c, d, e, F1, KK3,	4,  8);
    R( e, a, b, c, d, F1, KK3,	1, 11);
    R( d, e, a, b, c, F1, KK3,	3, 14);
    R( c, d, e, a, b, F1, KK3, 11, 14);
    R( b, c, d, e, a, F1, KK3, 15,  6);
    R( a, b, c, d, e, F1, KK3,	0, 14);
    R( e, a, b, c, d, F1, KK3,	5,  6);
    R( d, e, a, b, c, F1, KK3, 12,  9);
    R( c, d, e, a, b, F1, KK3,	2, 12);
    R( b, c, d, e, a, F1, KK3, 13,  9);
    R( a, b, c, d, e, F1, KK3,	9, 12);
    R( e, a, b, c, d, F1, KK3,	7,  5);
    R( d, e, a, b, c, F1, KK3, 10, 15);
    R( c, d, e, a, b, F1, KK3, 14,  8);
    R( b, c, d, e, a, F0, KK4, 12,  8);
    R( a, b, c, d, e, F0, KK4, 15,  5);
    R( e, a, b, c, d, F0, KK4, 10, 12);
    R( d, e, a, b, c, F0, KK4,	4,  9);
    R( c, d, e, a, b, F0, KK4,	1, 12);
    R( b, c, d, e, a, F0, KK4,	5,  5);
    R( a, b, c, d, e, F0, KK4,	8, 14);
    R( e, a, b, c, d, F0, KK4,	7,  6);
    R( d, e, a, b, c, F0, KK4,	6,  8);
    R( c, d, e, a, b, F0, KK4,	2, 13);
    R( b, c, d, e, a, F0, KK4, 13,  6);
    R( a, b, c, d, e, F0, KK4, 14,  5);
    R( e, a, b, c, d, F0, KK4,	0, 15);
    R( d, e, a, b, c, F0, KK4,	3, 13);
    R( c, d, e, a, b, F0, KK4,	9, 11);
    R( b, c, d, e, a, F0, KK4, 11, 11);


    t	   = keys[1] + d + cc;
    keys[1] = keys[2] + e + dd;
    keys[2] = keys[3] + a + ee;
    keys[3] = keys[4] + b + aa;
    keys[4] = keys[0] + c + bb;
    keys[0] = t;
}
#endif
