section .text

global RMD128_CalcBlock

%define a ebx
%define b ecx
%define c esi
%define d edx

;#define F1(x, y, z) _asm {mov eax,x} _asm {xor eax,y} _asm {xor eax,z}
%macro F1 3
	mov eax,%1
	xor eax,%2
	xor eax,%3
%endmacro

;#define F2(x, y, z) _asm {push edi} _asm {mov eax,x} _asm {mov edi,eax} _asm {and eax,y} _asm {not edi} _asm {and edi,z} _asm {or eax,edi} _asm {pop edi}
%macro F2 3
	mov eax,%1
	mov edi,%1
	and eax,%2
	not edi
	and edi,%3
	or eax,edi
%endmacro

;#define F3(x, y, z) _asm {mov eax,y} _asm {not eax} _asm{or eax,x} _asm {xor eax,z}
%macro F3 3
	mov eax,%2
	not eax
	or eax,%1
	xor eax,%3
%endmacro

;#define F4(x, y, z) _asm {push edi} _asm {mov eax,z} _asm {mov edi,eax} _asm {and eax,x} _asm {not edi} _asm {and edi,y} _asm {or eax,edi} _asm {pop edi}
%macro F4 3
	mov eax,%3
	mov edi,%3
	and eax,%1
	not edi
	and edi,%2
	or eax,edi
%endmacro

;#define HASHSTEP(f, w, x, y, z, data, data2, s) f(x, y, z) _asm {add w, eax} _asm{add w, dword ptr [edi + data * 4]} _asm {add w, data2} _asm {rol w, s}
%macro HASHSTEP 8
	%1 %3, %4, %5
	add %2, %7
	add %2, dword [r9 + %6 * 4]
	add %2, eax
	rol %2, %8
%endmacro

;#define HASHSTEP0(f, w, x, y, z, data, data2, s) f(x, y, z) _asm {add w, eax} _asm{add w, dword ptr [edi + data * 4]} _asm {add w, data2} _asm {rol w, s}
%macro HASHSTEP0 8
	%1 %3, %4, %5
	add %2, dword [r9 + %6 * 4]
	add %2, eax
	rol %2, %8
%endmacro

;void RMD128_CalcBlock(UInt32 *keys, const UInt8 *block)
;0 rdi
;8 rsi
;16 rbx
;24 retAddr
;rcx keys
;rdx block
	align 16
RMD128_CalcBlock:
	push rbx
	mov r10,rsi
	mov r11,rdi
	
	sub rsp,16
	mov r8,rcx ;keys
	mov r9,rdx ;block

	mov ebx,dword [r8]
	mov ecx,dword [r8+4]
	mov esi,dword [r8+8]
	mov edx,dword [r8+12]
	
	HASHSTEP0 F1, a, b, c, d, 0,  0x00000000, 11
	HASHSTEP0 F1, d, a, b, c, 1,  0x00000000, 14
	HASHSTEP0 F1, c, d, a, b, 2,  0x00000000, 15
	HASHSTEP0 F1, b, c, d, a, 3,  0x00000000, 12
	HASHSTEP0 F1, a, b, c, d, 4,  0x00000000, 5
	HASHSTEP0 F1, d, a, b, c, 5,  0x00000000, 8
	HASHSTEP0 F1, c, d, a, b, 6,  0x00000000, 7
	HASHSTEP0 F1, b, c, d, a, 7,  0x00000000, 9
	HASHSTEP0 F1, a, b, c, d, 8,  0x00000000, 11
	HASHSTEP0 F1, d, a, b, c, 9,  0x00000000, 13
	HASHSTEP0 F1, c, d, a, b, 10, 0x00000000, 14
	HASHSTEP0 F1, b, c, d, a, 11, 0x00000000, 15
	HASHSTEP0 F1, a, b, c, d, 12, 0x00000000, 6
	HASHSTEP0 F1, d, a, b, c, 13, 0x00000000, 7
	HASHSTEP0 F1, c, d, a, b, 14, 0x00000000, 9
	HASHSTEP0 F1, b, c, d, a, 15, 0x00000000, 8

	HASHSTEP F2, a, b, c, d, 7,  0x5A827999, 7
	HASHSTEP F2, d, a, b, c, 4,  0x5A827999, 6
	HASHSTEP F2, c, d, a, b, 13, 0x5A827999, 8
	HASHSTEP F2, b, c, d, a, 1,  0x5A827999, 13
	HASHSTEP F2, a, b, c, d, 10, 0x5A827999, 11
	HASHSTEP F2, d, a, b, c, 6,  0x5A827999, 9
	HASHSTEP F2, c, d, a, b, 15, 0x5A827999, 7
	HASHSTEP F2, b, c, d, a, 3,  0x5A827999, 15
	HASHSTEP F2, a, b, c, d, 12, 0x5A827999, 7
	HASHSTEP F2, d, a, b, c, 0,  0x5A827999, 12
	HASHSTEP F2, c, d, a, b, 9,  0x5A827999, 15
	HASHSTEP F2, b, c, d, a, 5,  0x5A827999, 9
	HASHSTEP F2, a, b, c, d, 2,  0x5A827999, 11
	HASHSTEP F2, d, a, b, c, 14, 0x5A827999, 7
	HASHSTEP F2, c, d, a, b, 11, 0x5A827999, 13
	HASHSTEP F2, b, c, d, a, 8,  0x5A827999, 12

	HASHSTEP F3, a, b, c, d, 3,  0x6ED9EBA1, 11
	HASHSTEP F3, d, a, b, c, 10, 0x6ED9EBA1, 13
	HASHSTEP F3, c, d, a, b, 14, 0x6ED9EBA1, 6
	HASHSTEP F3, b, c, d, a, 4,  0x6ED9EBA1, 7
	HASHSTEP F3, a, b, c, d, 9,  0x6ED9EBA1, 14
	HASHSTEP F3, d, a, b, c, 15, 0x6ED9EBA1, 9
	HASHSTEP F3, c, d, a, b, 8,  0x6ED9EBA1, 13
	HASHSTEP F3, b, c, d, a, 1,  0x6ED9EBA1, 15
	HASHSTEP F3, a, b, c, d, 2,  0x6ED9EBA1, 14
	HASHSTEP F3, d, a, b, c, 7,  0x6ED9EBA1, 8
	HASHSTEP F3, c, d, a, b, 0,  0x6ED9EBA1, 13
	HASHSTEP F3, b, c, d, a, 6,  0x6ED9EBA1, 6
	HASHSTEP F3, a, b, c, d, 13, 0x6ED9EBA1, 5
	HASHSTEP F3, d, a, b, c, 11, 0x6ED9EBA1, 12
	HASHSTEP F3, c, d, a, b, 5,  0x6ED9EBA1, 7
	HASHSTEP F3, b, c, d, a, 12, 0x6ED9EBA1, 5

	HASHSTEP F4, a, b, c, d, 1,  0x8F1BBCDC, 11
	HASHSTEP F4, d, a, b, c, 9,  0x8F1BBCDC, 12
	HASHSTEP F4, c, d, a, b, 11, 0x8F1BBCDC, 14
	HASHSTEP F4, b, c, d, a, 10, 0x8F1BBCDC, 15
	HASHSTEP F4, a, b, c, d, 0,  0x8F1BBCDC, 14
	HASHSTEP F4, d, a, b, c, 8,  0x8F1BBCDC, 15
	HASHSTEP F4, c, d, a, b, 12, 0x8F1BBCDC, 9
	HASHSTEP F4, b, c, d, a, 4,  0x8F1BBCDC, 8
	HASHSTEP F4, a, b, c, d, 13, 0x8F1BBCDC, 9
	HASHSTEP F4, d, a, b, c, 3,  0x8F1BBCDC, 14
	HASHSTEP F4, c, d, a, b, 7,  0x8F1BBCDC, 5
	HASHSTEP F4, b, c, d, a, 15, 0x8F1BBCDC, 6
	HASHSTEP F4, a, b, c, d, 14, 0x8F1BBCDC, 8
	HASHSTEP F4, d, a, b, c, 5,  0x8F1BBCDC, 6
	HASHSTEP F4, c, d, a, b, 6,  0x8F1BBCDC, 5
	HASHSTEP F4, b, c, d, a, 2,  0x8F1BBCDC, 12

	mov dword [rsp],ebx
	mov dword [rsp+4],ecx
	mov dword [rsp+8],esi
	mov dword [rsp+12],edx

	mov ebx,dword [r8]
	mov ecx,dword [r8+4]
	mov esi,dword [r8+8]
	mov edx,dword [r8+12]

	HASHSTEP F4, a, b, c, d, 5,  0x50A28BE6, 8
	HASHSTEP F4, d, a, b, c, 14, 0x50A28BE6, 9
	HASHSTEP F4, c, d, a, b, 7,  0x50A28BE6, 9
	HASHSTEP F4, b, c, d, a, 0,  0x50A28BE6, 11
	HASHSTEP F4, a, b, c, d, 9,  0x50A28BE6, 13
	HASHSTEP F4, d, a, b, c, 2,  0x50A28BE6, 15
	HASHSTEP F4, c, d, a, b, 11, 0x50A28BE6, 15
	HASHSTEP F4, b, c, d, a, 4,  0x50A28BE6, 5
	HASHSTEP F4, a, b, c, d, 13, 0x50A28BE6, 7
	HASHSTEP F4, d, a, b, c, 6,  0x50A28BE6, 7
	HASHSTEP F4, c, d, a, b, 15, 0x50A28BE6, 8
	HASHSTEP F4, b, c, d, a, 8,  0x50A28BE6, 11
	HASHSTEP F4, a, b, c, d, 1,  0x50A28BE6, 14
	HASHSTEP F4, d, a, b, c, 10, 0x50A28BE6, 14
	HASHSTEP F4, c, d, a, b, 3,  0x50A28BE6, 12
	HASHSTEP F4, b, c, d, a, 12, 0x50A28BE6, 6

	HASHSTEP F3, a, b, c, d, 6,  0x5C4DD124, 9
	HASHSTEP F3, d, a, b, c, 11, 0x5C4DD124, 13
	HASHSTEP F3, c, d, a, b, 3,  0x5C4DD124, 15
	HASHSTEP F3, b, c, d, a, 7,  0x5C4DD124, 7
	HASHSTEP F3, a, b, c, d, 0,  0x5C4DD124, 12
	HASHSTEP F3, d, a, b, c, 13, 0x5C4DD124, 8
	HASHSTEP F3, c, d, a, b, 5,  0x5C4DD124, 9
	HASHSTEP F3, b, c, d, a, 10, 0x5C4DD124, 11
	HASHSTEP F3, a, b, c, d, 14, 0x5C4DD124, 7
	HASHSTEP F3, d, a, b, c, 15, 0x5C4DD124, 7
	HASHSTEP F3, c, d, a, b, 8,  0x5C4DD124, 12
	HASHSTEP F3, b, c, d, a, 12, 0x5C4DD124, 7
	HASHSTEP F3, a, b, c, d, 4,  0x5C4DD124, 6
	HASHSTEP F3, d, a, b, c, 9,  0x5C4DD124, 15
	HASHSTEP F3, c, d, a, b, 1,  0x5C4DD124, 13
	HASHSTEP F3, b, c, d, a, 2,  0x5C4DD124, 11

	HASHSTEP F2, a, b, c, d, 15, 0x6D703EF3, 9
	HASHSTEP F2, d, a, b, c, 5,  0x6D703EF3, 7
	HASHSTEP F2, c, d, a, b, 1,  0x6D703EF3, 15
	HASHSTEP F2, b, c, d, a, 3,  0x6D703EF3, 11
	HASHSTEP F2, a, b, c, d, 7,  0x6D703EF3, 8
	HASHSTEP F2, d, a, b, c, 14, 0x6D703EF3, 6
	HASHSTEP F2, c, d, a, b, 6,  0x6D703EF3, 6
	HASHSTEP F2, b, c, d, a, 9,  0x6D703EF3, 14
	HASHSTEP F2, a, b, c, d, 11, 0x6D703EF3, 12
	HASHSTEP F2, d, a, b, c, 8,  0x6D703EF3, 13
	HASHSTEP F2, c, d, a, b, 12, 0x6D703EF3, 5
	HASHSTEP F2, b, c, d, a, 2,  0x6D703EF3, 14
	HASHSTEP F2, a, b, c, d, 10, 0x6D703EF3, 13
	HASHSTEP F2, d, a, b, c, 0,  0x6D703EF3, 13
	HASHSTEP F2, c, d, a, b, 4,  0x6D703EF3, 7
	HASHSTEP F2, b, c, d, a, 13, 0x6D703EF3, 5

	HASHSTEP0 F1, a, b, c, d, 8,  0x00000000, 15
	HASHSTEP0 F1, d, a, b, c, 6,  0x00000000, 5
	HASHSTEP0 F1, c, d, a, b, 4,  0x00000000, 8
	HASHSTEP0 F1, b, c, d, a, 1,  0x00000000, 11
	HASHSTEP0 F1, a, b, c, d, 3,  0x00000000, 14
	HASHSTEP0 F1, d, a, b, c, 11, 0x00000000, 14
	HASHSTEP0 F1, c, d, a, b, 15, 0x00000000, 6
	HASHSTEP0 F1, b, c, d, a, 0,  0x00000000, 14
	HASHSTEP0 F1, a, b, c, d, 5,  0x00000000, 6
	HASHSTEP0 F1, d, a, b, c, 12, 0x00000000, 9
	HASHSTEP0 F1, c, d, a, b, 2,  0x00000000, 12
	HASHSTEP0 F1, b, c, d, a, 13, 0x00000000, 9
	HASHSTEP0 F1, a, b, c, d, 9,  0x00000000, 12
	HASHSTEP0 F1, d, a, b, c, 7,  0x00000000, 5
	HASHSTEP0 F1, c, d, a, b, 10, 0x00000000, 15
	HASHSTEP0 F1, b, c, d, a, 14, 0x00000000, 8

	add esi, dword [r8]
	add esi, dword [rsp+4]
	add edx, dword [r8+4]
	add edx, dword [rsp+8]
	add ebx, dword [r8+8]
	add ebx, dword [rsp+12]
	add ecx, dword [r8+12]
	add ecx, dword [rsp+0]
	mov dword [r8], edx
	mov dword [r8+4], ebx
	mov dword [r8+8], ecx
	mov dword [r8+12], esi
	
	add rsp,16
	mov rsi,r10
	mov rdi,r11
	pop rbx
	ret
