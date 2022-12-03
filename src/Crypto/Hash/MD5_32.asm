section .text

global _MD5_CalcBlock
global MD5_CalcBlock

%define a ebx
%define b ecx
%define c edx
%define d esi

%define S0 0
%define S1 1
%define S2 2
%define S3 3

;#define F1(x, y, z) _asm {mov eax,y} _asm {xor eax,z} _asm {and eax,x} _asm {xor eax,z}
%macro F1 3
	mov eax,%2
	xor eax,%3
	and eax,%1
	xor eax,%3
%endmacro

;#define F2(x, y, z) F1(z, x, y)
%macro F2 3
	F1 %3, %1, %2
%endmacro

;#define F3(x, y, z) _asm {mov eax,x} _asm {xor eax,y} _asm {xor eax,z}
%macro F3 3
	mov eax,%1
	xor eax,%2
	xor eax,%3
%endmacro

;#define F4(x, y, z) _asm {mov eax,z} _asm {not eax} _asm {or eax,x} _asm {xor eax,y}
%macro F4 3
	mov eax,%3
	not eax
	or eax,%1
	xor eax,%2
%endmacro

;#define MD5STEP(f, w, x, y, z, data, data2, s) f(x, y, z) _asm {add w,eax} _asm{add w,dword ptr [edi + data * 4]} _asm {add w, data2} _asm {rol w, s} _asm {add w,x}
%macro MD5STEP 9
	%1 %3, %4, %5
	add %2, eax
	add %2, dword [edi + (%6 * 4 + %7) * 4]
	add %2, %8
	rol %2, %9
	add %2, %3
%endmacro

;void MD5_CalcBlock(UInt32 *hVals, const UInt8 *block, UOSInt blockCnt);
;0 edi
;4 esi
;8 ebx
;12 ebp
;16 retAddr
;20 hVals
;24 block
;28 blockCnt
 
	align 16
_MD5_CalcBlock:
MD5_CalcBlock:
	push ebp
	push ebx
	push esi
	push edi
	mov ebp,dword [esp+20]
	mov edi,dword [esp+24] ;blk
	mov ebx,dword [ebp+0] ;h0
	mov ecx,dword [ebp+4] ;h1
	mov edx,dword [ebp+8] ;h2
	mov esi,dword [ebp+12] ;h3
	align 16
cblop:
	
	MD5STEP F1, a, b, c, d, S0, 0, 0xd76aa478, 7
	MD5STEP F1, d, a, b, c, S0, 1, 0xe8c7b756, 12
	MD5STEP F1, c, d, a, b, S0, 2, 0x242070db, 17
	MD5STEP F1, b, c, d, a, S0, 3, 0xc1bdceee, 22
	MD5STEP F1, a, b, c, d, S1, 0, 0xf57c0faf, 7
	MD5STEP F1, d, a, b, c, S1, 1, 0x4787c62a, 12
	MD5STEP F1, c, d, a, b, S1, 2, 0xa8304613, 17
	MD5STEP F1, b, c, d, a, S1, 3, 0xfd469501, 22
	MD5STEP F1, a, b, c, d, S2, 0, 0x698098d8, 7
	MD5STEP F1, d, a, b, c, S2, 1, 0x8b44f7af, 12
	MD5STEP F1, c, d, a, b, S2, 2, 0xffff5bb1, 17
	MD5STEP F1, b, c, d, a, S2, 3, 0x895cd7be, 22
	MD5STEP F1, a, b, c, d, S3, 0, 0x6b901122, 7
	MD5STEP F1, d, a, b, c, S3, 1, 0xfd987193, 12
	MD5STEP F1, c, d, a, b, S3, 2, 0xa679438e, 17
	MD5STEP F1, b, c, d, a, S3, 3, 0x49b40821, 22

	MD5STEP F2, a, b, c, d, S0, 1, 0xf61e2562, 5
	MD5STEP F2, d, a, b, c, S1, 2, 0xc040b340, 9
	MD5STEP F2, c, d, a, b, S2, 3, 0x265e5a51, 14
	MD5STEP F2, b, c, d, a, S0, 0, 0xe9b6c7aa, 20
	MD5STEP F2, a, b, c, d, S1, 1, 0xd62f105d, 5
	MD5STEP F2, d, a, b, c, S2, 2, 0x02441453, 9
	MD5STEP F2, c, d, a, b, S3, 3, 0xd8a1e681, 14
	MD5STEP F2, b, c, d, a, S1, 0, 0xe7d3fbc8, 20
	MD5STEP F2, a, b, c, d, S2, 1, 0x21e1cde6, 5
	MD5STEP F2, d, a, b, c, S3, 2, 0xc33707d6, 9
	MD5STEP F2, c, d, a, b, S0, 3, 0xf4d50d87, 14
	MD5STEP F2, b, c, d, a, S2, 0, 0x455a14ed, 20
	MD5STEP F2, a, b, c, d, S3, 1, 0xa9e3e905, 5
	MD5STEP F2, d, a, b, c, S0, 2, 0xfcefa3f8, 9
	MD5STEP F2, c, d, a, b, S1, 3, 0x676f02d9, 14
	MD5STEP F2, b, c, d, a, S3, 0, 0x8d2a4c8a, 20

	MD5STEP F3, a, b, c, d, S1, 1, 0xfffa3942, 4
	MD5STEP F3, d, a, b, c, S2, 0, 0x8771f681, 11
	MD5STEP F3, c, d, a, b, S2, 3, 0x6d9d6122, 16
	MD5STEP F3, b, c, d, a, S3, 2, 0xfde5380c, 23
	MD5STEP F3, a, b, c, d, S0, 1, 0xa4beea44, 4
	MD5STEP F3, d, a, b, c, S1, 0, 0x4bdecfa9, 11
	MD5STEP F3, c, d, a, b, S1, 3, 0xf6bb4b60, 16
	MD5STEP F3, b, c, d, a, S2, 2, 0xbebfbc70, 23
	MD5STEP F3, a, b, c, d, S3, 1, 0x289b7ec6, 4
	MD5STEP F3, d, a, b, c, S0, 0, 0xeaa127fa, 11
	MD5STEP F3, c, d, a, b, S0, 3, 0xd4ef3085, 16
	MD5STEP F3, b, c, d, a, S1, 2, 0x04881d05, 23
	MD5STEP F3, a, b, c, d, S2, 1, 0xd9d4d039, 4
	MD5STEP F3, d, a, b, c, S3, 0, 0xe6db99e5, 11
	MD5STEP F3, c, d, a, b, S3, 3, 0x1fa27cf8, 16
	MD5STEP F3, b, c, d, a, S0, 2, 0xc4ac5665, 23

	MD5STEP F4, a, b, c, d, S0, 0, 0xf4292244, 6
	MD5STEP F4, d, a, b, c, S1, 3, 0x432aff97, 10
	MD5STEP F4, c, d, a, b, S3, 2, 0xab9423a7, 15
	MD5STEP F4, b, c, d, a, S1, 1, 0xfc93a039, 21
	MD5STEP F4, a, b, c, d, S3, 0, 0x655b59c3, 6
	MD5STEP F4, d, a, b, c, S0, 3, 0x8f0ccc92, 10
	MD5STEP F4, c, d, a, b, S2, 2, 0xffeff47d, 15
	MD5STEP F4, b, c, d, a, S0, 1, 0x85845dd1, 21
	MD5STEP F4, a, b, c, d, S2, 0, 0x6fa87e4f, 6
	MD5STEP F4, d, a, b, c, S3, 3, 0xfe2ce6e0, 10
	MD5STEP F4, c, d, a, b, S1, 2, 0xa3014314, 15
	MD5STEP F4, b, c, d, a, S3, 1, 0x4e0811a1, 21
	MD5STEP F4, a, b, c, d, S1, 0, 0xf7537e82, 6
	MD5STEP F4, d, a, b, c, S2, 3, 0xbd3af235, 10
	MD5STEP F4, c, d, a, b, S0, 2, 0x2ad7d2bb, 15
	MD5STEP F4, b, c, d, a, S2, 1, 0xeb86d391, 21

	add ebx,dword [ebp+0] ;h0
	add ecx,dword [ebp+4] ;h1
	add edx,dword [ebp+8] ;h2
	add esi,dword [ebp+12] ;h3

	mov dword [ebp+0],ebx ;h0
	mov dword [ebp+4],ecx ;h1
	mov dword [ebp+8],edx ;h2
	mov dword [ebp+12],esi ;h3

	lea edi,[edi+64]
	dec dword [esp+28]
	jnz cblop

	pop edi
	pop esi
	pop ebx
	pop ebp
	ret
