section .text

global MD5_CalcBlock

%define a ebx
%define b ecx
%define c edx
%define d esi

;#define F1(x, y, z) asm {mov eax,y} asm {xor eax,z} asm {and eax,x} asm {xor eax,z}
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

;#define F3(x, y, z) asm {mov eax,x} asm {xor eax,y} asm {xor eax,z}
%macro F3 3
	mov eax,%3
	xor eax,%2
	xor eax,%1
%endmacro

;#define F4(x, y, z) asm {mov eax,z} asm {not eax} asm {or eax,x} asm {xor eax,y}
%macro F4 3
	mov eax,%3
	not eax
	or eax,%1
	xor eax,%2
%endmacro

;#define MD5STEP(f, w, x, y, z, data, data2, s) f(x, y, z) asm {add w,eax} asm{add w,dword ptr [edi + data * 4]} asm {add w, data2} asm {rol w, s} asm {add w,x}
%macro MD5STEP 8
	%1 %3, %4, %5
	add %2, dword [rdi + %6 * 4]
	add %2, %7
	add %2, eax
	rol %2, %8
	add %2, %3
%endmacro

;void MD5_CalcBlock(UInt32 *hVals, const UInt8 *block, UIntOS blockCnt);
;0 rdi
;8 rsi
;16 rbx
;24 retAddr
;rcx hVals
;rdx block
;r8 blockCnt
	align 16
MD5_CalcBlock:
	push rbx
	push rsi
	push rdi
	mov r9,rcx
	mov rdi,rdx ;blk
	mov ebx,dword [r9] ;h0
	mov ecx,dword [r9+4] ;h1
	mov edx,dword [r9+8] ;h2
	mov esi,dword [r9+12] ;h3
	
	align 16
cblop:	

    MD5STEP F1, a, b, c, d, 0, 0xd76aa478, 7
    MD5STEP F1, d, a, b, c, 1, 0xe8c7b756, 12
    MD5STEP F1, c, d, a, b, 2, 0x242070db, 17
    MD5STEP F1, b, c, d, a, 3, 0xc1bdceee, 22
    MD5STEP F1, a, b, c, d, 4, 0xf57c0faf, 7
    MD5STEP F1, d, a, b, c, 5, 0x4787c62a, 12
    MD5STEP F1, c, d, a, b, 6, 0xa8304613, 17
    MD5STEP F1, b, c, d, a, 7, 0xfd469501, 22
    MD5STEP F1, a, b, c, d, 8, 0x698098d8, 7
    MD5STEP F1, d, a, b, c, 9, 0x8b44f7af, 12
    MD5STEP F1, c, d, a, b, 10, 0xffff5bb1, 17
    MD5STEP F1, b, c, d, a, 11, 0x895cd7be, 22
    MD5STEP F1, a, b, c, d, 12, 0x6b901122, 7
    MD5STEP F1, d, a, b, c, 13, 0xfd987193, 12
    MD5STEP F1, c, d, a, b, 14, 0xa679438e, 17
    MD5STEP F1, b, c, d, a, 15, 0x49b40821, 22

    MD5STEP F2, a, b, c, d, 1, 0xf61e2562, 5
    MD5STEP F2, d, a, b, c, 6, 0xc040b340, 9
    MD5STEP F2, c, d, a, b, 11, 0x265e5a51, 14
    MD5STEP F2, b, c, d, a, 0, 0xe9b6c7aa, 20
    MD5STEP F2, a, b, c, d, 5, 0xd62f105d, 5
    MD5STEP F2, d, a, b, c, 10, 0x02441453, 9
    MD5STEP F2, c, d, a, b, 15, 0xd8a1e681, 14
    MD5STEP F2, b, c, d, a, 4, 0xe7d3fbc8, 20
    MD5STEP F2, a, b, c, d, 9, 0x21e1cde6, 5
    MD5STEP F2, d, a, b, c, 14, 0xc33707d6, 9
    MD5STEP F2, c, d, a, b, 3, 0xf4d50d87, 14
    MD5STEP F2, b, c, d, a, 8, 0x455a14ed, 20
    MD5STEP F2, a, b, c, d, 13, 0xa9e3e905, 5
    MD5STEP F2, d, a, b, c, 2, 0xfcefa3f8, 9
    MD5STEP F2, c, d, a, b, 7, 0x676f02d9, 14
    MD5STEP F2, b, c, d, a, 12, 0x8d2a4c8a, 20

    MD5STEP F3, a, b, c, d, 5, 0xfffa3942, 4
    MD5STEP F3, d, a, b, c, 8, 0x8771f681, 11
    MD5STEP F3, c, d, a, b, 11, 0x6d9d6122, 16
    MD5STEP F3, b, c, d, a, 14, 0xfde5380c, 23
    MD5STEP F3, a, b, c, d, 1, 0xa4beea44, 4
    MD5STEP F3, d, a, b, c, 4, 0x4bdecfa9, 11
    MD5STEP F3, c, d, a, b, 7, 0xf6bb4b60, 16
    MD5STEP F3, b, c, d, a, 10, 0xbebfbc70, 23
    MD5STEP F3, a, b, c, d, 13, 0x289b7ec6, 4
    MD5STEP F3, d, a, b, c, 0, 0xeaa127fa, 11
    MD5STEP F3, c, d, a, b, 3, 0xd4ef3085, 16
    MD5STEP F3, b, c, d, a, 6, 0x04881d05, 23
    MD5STEP F3, a, b, c, d, 9, 0xd9d4d039, 4
    MD5STEP F3, d, a, b, c, 12, 0xe6db99e5, 11
    MD5STEP F3, c, d, a, b, 15, 0x1fa27cf8, 16
    MD5STEP F3, b, c, d, a, 2, 0xc4ac5665, 23

	MD5STEP F4, a, b, c, d, 0, 0xf4292244, 6
    MD5STEP F4, d, a, b, c, 7, 0x432aff97, 10
    MD5STEP F4, c, d, a, b, 14, 0xab9423a7, 15
    MD5STEP F4, b, c, d, a, 5, 0xfc93a039, 21
    MD5STEP F4, a, b, c, d, 12, 0x655b59c3, 6
    MD5STEP F4, d, a, b, c, 3, 0x8f0ccc92, 10
    MD5STEP F4, c, d, a, b, 10, 0xffeff47d, 15
    MD5STEP F4, b, c, d, a, 1, 0x85845dd1, 21
    MD5STEP F4, a, b, c, d, 8, 0x6fa87e4f, 6
    MD5STEP F4, d, a, b, c, 15, 0xfe2ce6e0, 10
    MD5STEP F4, c, d, a, b, 6, 0xa3014314, 15
    MD5STEP F4, b, c, d, a, 13, 0x4e0811a1, 21
    MD5STEP F4, a, b, c, d, 4, 0xf7537e82, 6
    MD5STEP F4, d, a, b, c, 11, 0xbd3af235, 10
    MD5STEP F4, c, d, a, b, 2, 0x2ad7d2bb, 15
    MD5STEP F4, b, c, d, a, 9, 0xeb86d391, 21

	add ebx,dword [r9] ;h0
	add ecx,dword [r9+4] ;h1
	add edx,dword [r9+8] ;h2
	add esi,dword [r9+12] ;h3

	mov dword [r9],ebx ;h0
	mov dword [r9+4],ecx ;h1
	mov dword [r9+8],edx ;h2
	mov dword [r9+12],esi ;h3
	
	lea rdi,[rdi+64]
	dec r8
	jnz cblop
	
	pop rdi
	pop rsi
	pop rbx
	ret
