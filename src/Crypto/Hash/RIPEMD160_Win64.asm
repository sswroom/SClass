section .text

global RMD160_CalcBlock

%define a ebx
%define b ecx
%define c esi
%define d edi
%define e ebp

;#define F1(x, y, z) _asm {mov eax,x} _asm {xor eax,y} _asm {xor eax,z}
%macro F1 3
	mov eax,%1
	xor eax,%2
	xor eax,%3
%endmacro

;#define F2(x, y, z) _asm {mov eax,x} _asm {mov edx,eax} _asm {and eax,y} _asm {not edx} _asm {and edx,z} _asm {or eax,edx}
%macro F2 3
	mov edx,%1
	mov eax,%1
	not edx
	and edx,%3
	and eax,%2
	or eax,edx
%endmacro

;#define F3(x, y, z) _asm {mov eax,y} _asm {not eax} _asm{or eax,x} _asm {xor eax,z}
%macro F3 3
	mov eax,%2
	not eax
	or eax,%1
	xor eax,%3
%endmacro

;#define F4(x, y, z) _asm {mov eax,z} _asm {mov edx,eax} _asm {and eax,x} _asm {not edx} _asm {and edx,y} _asm {or eax,edx}
%macro F4 3
	mov edx,%3
	mov eax,%3
	not edx
	and edx,%2
	and eax,%1
	or eax,edx
%endmacro

;#define F5(x, y, z) _asm {mov eax,z} _asm {not eax} _asm {or eax,y} _asm {xor eax,x}
%macro F5 3
	mov eax,%3
	not eax
	or eax,%2
	xor eax,%1
%endmacro

;#define HASHSTEP(f, v, w, x, y, z, data, data2, s) f(w, x, y) _asm {add eax,v} _asm{add eax,dword ptr [edi + data * 4]} _asm {add eax, data2} _asm {rol eax, s} _asm {add eax,z} _asm {mov v,eax} _asm {rol x,10}
%macro HASHSTEP 9
	%1 %3, %4, %5
	add %2,%8
	add %2,dword [r9 + %7 * 4]
	add %2,eax
	rol %2, %9
	rol %4, 10
	add %2, %6
%endmacro

;#define HASHSTEP0(f, v, w, x, y, z, data, data2, s) f(w, x, y) _asm {add eax,v} _asm{add eax,dword ptr [edi + data * 4]} _asm {rol eax, s} _asm {add eax,z} _asm {mov v,eax} _asm {rol x,10}
%macro HASHSTEP0 9
	%1 %3, %4, %5
	add %2,dword [r9 + %7 * 4]
	add %2,eax
	rol %2, %9
	rol %4, 10
	add %2, %6
%endmacro

;void RMD160_CalcBlock(UInt32 *keys, const UInt8 *block)
;0 rbp
;8 rdi
;16 rsi
;24 rbx
;32 retAddr
;rcx keys
;rdx block
	align 16
RMD160_CalcBlock:
	push rbx
	push rsi
	push rdi
	push rbp
	sub rsp,20
	mov r8,rcx ;keys
	mov r9,rdx ;block
	
	mov a, dword [r8]
	mov b, dword [r8+4]
	mov c, dword [r8+8]
	mov d, dword [r8+12]
	mov e, dword [r8+16]
	
	HASHSTEP0 F1, a, b, c, d, e, 0,  0x00000000, 11
	HASHSTEP0 F1, e, a, b, c, d, 1,  0x00000000, 14
	HASHSTEP0 F1, d, e, a, b, c, 2,  0x00000000, 15
	HASHSTEP0 F1, c, d, e, a, b, 3,  0x00000000, 12
	HASHSTEP0 F1, b, c, d, e, a, 4,  0x00000000, 5
	HASHSTEP0 F1, a, b, c, d, e, 5,  0x00000000, 8
	HASHSTEP0 F1, e, a, b, c, d, 6,  0x00000000, 7
	HASHSTEP0 F1, d, e, a, b, c, 7,  0x00000000, 9
	HASHSTEP0 F1, c, d, e, a, b, 8,  0x00000000, 11
	HASHSTEP0 F1, b, c, d, e, a, 9,  0x00000000, 13
	HASHSTEP0 F1, a, b, c, d, e, 10, 0x00000000, 14
	HASHSTEP0 F1, e, a, b, c, d, 11, 0x00000000, 15
	HASHSTEP0 F1, d, e, a, b, c, 12, 0x00000000, 6
	HASHSTEP0 F1, c, d, e, a, b, 13, 0x00000000, 7
	HASHSTEP0 F1, b, c, d, e, a, 14, 0x00000000, 9
	HASHSTEP0 F1, a, b, c, d, e, 15, 0x00000000, 8
	HASHSTEP  F2, e, a, b, c, d, 7,  0x5A827999, 7
	HASHSTEP  F2, d, e, a, b, c, 4,  0x5A827999, 6
	HASHSTEP  F2, c, d, e, a, b, 13, 0x5A827999, 8
	HASHSTEP  F2, b, c, d, e, a, 1,  0x5A827999, 13

	HASHSTEP  F2, a, b, c, d, e, 10, 0x5A827999, 11
	HASHSTEP  F2, e, a, b, c, d, 6,  0x5A827999, 9
	HASHSTEP  F2, d, e, a, b, c, 15, 0x5A827999, 7
	HASHSTEP  F2, c, d, e, a, b, 3,  0x5A827999, 15
	HASHSTEP  F2, b, c, d, e, a, 12, 0x5A827999, 7
	HASHSTEP  F2, a, b, c, d, e, 0,  0x5A827999, 12
	HASHSTEP  F2, e, a, b, c, d, 9,  0x5A827999, 15
	HASHSTEP  F2, d, e, a, b, c, 5,  0x5A827999, 9
	HASHSTEP  F2, c, d, e, a, b, 2,  0x5A827999, 11
	HASHSTEP  F2, b, c, d, e, a, 14, 0x5A827999, 7
	HASHSTEP  F2, a, b, c, d, e, 11, 0x5A827999, 13
	HASHSTEP  F2, e, a, b, c, d, 8,  0x5A827999, 12
	HASHSTEP  F3, d, e, a, b, c, 3,  0x6ED9EBA1, 11
	HASHSTEP  F3, c, d, e, a, b, 10, 0x6ED9EBA1, 13
	HASHSTEP  F3, b, c, d, e, a, 14, 0x6ED9EBA1, 6
	HASHSTEP  F3, a, b, c, d, e, 4,  0x6ED9EBA1, 7
	HASHSTEP  F3, e, a, b, c, d, 9,  0x6ED9EBA1, 14
	HASHSTEP  F3, d, e, a, b, c, 15, 0x6ED9EBA1, 9
	HASHSTEP  F3, c, d, e, a, b, 8,  0x6ED9EBA1, 13
	HASHSTEP  F3, b, c, d, e, a, 1,  0x6ED9EBA1, 15

	HASHSTEP  F3, a, b, c, d, e, 2,  0x6ED9EBA1, 14
	HASHSTEP  F3, e, a, b, c, d, 7,  0x6ED9EBA1, 8
	HASHSTEP  F3, d, e, a, b, c, 0,  0x6ED9EBA1, 13
	HASHSTEP  F3, c, d, e, a, b, 6,  0x6ED9EBA1, 6
	HASHSTEP  F3, b, c, d, e, a, 13, 0x6ED9EBA1, 5
	HASHSTEP  F3, a, b, c, d, e, 11, 0x6ED9EBA1, 12
	HASHSTEP  F3, e, a, b, c, d, 5,  0x6ED9EBA1, 7
	HASHSTEP  F3, d, e, a, b, c, 12, 0x6ED9EBA1, 5
	HASHSTEP  F4, c, d, e, a, b, 1,  0x8F1BBCDC, 11
	HASHSTEP  F4, b, c, d, e, a, 9,  0x8F1BBCDC, 12
	HASHSTEP  F4, a, b, c, d, e, 11, 0x8F1BBCDC, 14
	HASHSTEP  F4, e, a, b, c, d, 10, 0x8F1BBCDC, 15
	HASHSTEP  F4, d, e, a, b, c, 0,  0x8F1BBCDC, 14
	HASHSTEP  F4, c, d, e, a, b, 8,  0x8F1BBCDC, 15
	HASHSTEP  F4, b, c, d, e, a, 12, 0x8F1BBCDC, 9
	HASHSTEP  F4, a, b, c, d, e, 4,  0x8F1BBCDC, 8
	HASHSTEP  F4, e, a, b, c, d, 13, 0x8F1BBCDC, 9
	HASHSTEP  F4, d, e, a, b, c, 3,  0x8F1BBCDC, 14
	HASHSTEP  F4, c, d, e, a, b, 7,  0x8F1BBCDC, 5
	HASHSTEP  F4, b, c, d, e, a, 15, 0x8F1BBCDC, 6

	HASHSTEP  F4, a, b, c, d, e, 14, 0x8F1BBCDC, 8
	HASHSTEP  F4, e, a, b, c, d, 5,  0x8F1BBCDC, 6
	HASHSTEP  F4, d, e, a, b, c, 6,  0x8F1BBCDC, 5
	HASHSTEP  F4, c, d, e, a, b, 2,  0x8F1BBCDC, 12
	HASHSTEP  F5, b, c, d, e, a, 4,  0xA953FD4E, 9
	HASHSTEP  F5, a, b, c, d, e, 0,  0xA953FD4E, 15
	HASHSTEP  F5, e, a, b, c, d, 5,  0xA953FD4E, 5
	HASHSTEP  F5, d, e, a, b, c, 9,  0xA953FD4E, 11
	HASHSTEP  F5, c, d, e, a, b, 7,  0xA953FD4E, 6
	HASHSTEP  F5, b, c, d, e, a, 12, 0xA953FD4E, 8
	HASHSTEP  F5, a, b, c, d, e, 2,  0xA953FD4E, 13
	HASHSTEP  F5, e, a, b, c, d, 10, 0xA953FD4E, 12
	HASHSTEP  F5, d, e, a, b, c, 14, 0xA953FD4E, 5
	HASHSTEP  F5, c, d, e, a, b, 1,  0xA953FD4E, 12
	HASHSTEP  F5, b, c, d, e, a, 3,  0xA953FD4E, 13
	HASHSTEP  F5, a, b, c, d, e, 8,  0xA953FD4E, 14
	HASHSTEP  F5, e, a, b, c, d, 11, 0xA953FD4E, 11
	HASHSTEP  F5, d, e, a, b, c, 6,  0xA953FD4E, 8
	HASHSTEP  F5, c, d, e, a, b, 15, 0xA953FD4E, 5
	HASHSTEP  F5, b, c, d, e, a, 13, 0xA953FD4E, 6

	mov dword [rsp+0],a
	mov dword [rsp+4],b
	mov dword [rsp+8],c
	mov dword [rsp+12],d
	mov dword [rsp+16],e

	mov a,dword [r8]
	mov b,dword [r8+4]
	mov c,dword [r8+8]
	mov d,dword [r8+12]
	mov e,dword [r8+16]

	HASHSTEP  F5, a, b, c, d, e, 5,  0x50A28BE6, 8
	HASHSTEP  F5, e, a, b, c, d, 14, 0x50A28BE6, 9
	HASHSTEP  F5, d, e, a, b, c, 7,  0x50A28BE6, 9
	HASHSTEP  F5, c, d, e, a, b, 0,  0x50A28BE6, 11
	HASHSTEP  F5, b, c, d, e, a, 9,  0x50A28BE6, 13
	HASHSTEP  F5, a, b, c, d, e, 2,  0x50A28BE6, 15
	HASHSTEP  F5, e, a, b, c, d, 11, 0x50A28BE6, 15
	HASHSTEP  F5, d, e, a, b, c, 4,  0x50A28BE6, 5
	HASHSTEP  F5, c, d, e, a, b, 13, 0x50A28BE6, 7
	HASHSTEP  F5, b, c, d, e, a, 6,  0x50A28BE6, 7
	HASHSTEP  F5, a, b, c, d, e, 15, 0x50A28BE6, 8
	HASHSTEP  F5, e, a, b, c, d, 8,  0x50A28BE6, 11
	HASHSTEP  F5, d, e, a, b, c, 1,  0x50A28BE6, 14
	HASHSTEP  F5, c, d, e, a, b, 10, 0x50A28BE6, 14
	HASHSTEP  F5, b, c, d, e, a, 3,  0x50A28BE6, 12
	HASHSTEP  F5, a, b, c, d, e, 12, 0x50A28BE6, 6
	HASHSTEP  F4, e, a, b, c, d, 6,  0x5C4DD124, 9
	HASHSTEP  F4, d, e, a, b, c, 11, 0x5C4DD124, 13
	HASHSTEP  F4, c, d, e, a, b, 3,  0x5C4DD124, 15
	HASHSTEP  F4, b, c, d, e, a, 7,  0x5C4DD124, 7

	HASHSTEP  F4, a, b, c, d, e, 0,  0x5C4DD124, 12
	HASHSTEP  F4, e, a, b, c, d, 13, 0x5C4DD124, 8
	HASHSTEP  F4, d, e, a, b, c, 5,  0x5C4DD124, 9
	HASHSTEP  F4, c, d, e, a, b, 10, 0x5C4DD124, 11
	HASHSTEP  F4, b, c, d, e, a, 14, 0x5C4DD124, 7
	HASHSTEP  F4, a, b, c, d, e, 15, 0x5C4DD124, 7
	HASHSTEP  F4, e, a, b, c, d, 8,  0x5C4DD124, 12
	HASHSTEP  F4, d, e, a, b, c, 12, 0x5C4DD124, 7
	HASHSTEP  F4, c, d, e, a, b, 4,  0x5C4DD124, 6
	HASHSTEP  F4, b, c, d, e, a, 9,  0x5C4DD124, 15
	HASHSTEP  F4, a, b, c, d, e, 1,  0x5C4DD124, 13
	HASHSTEP  F4, e, a, b, c, d, 2,  0x5C4DD124, 11
	HASHSTEP  F3, d, e, a, b, c, 15, 0x6D703EF3, 9
	HASHSTEP  F3, c, d, e, a, b, 5,  0x6D703EF3, 7
	HASHSTEP  F3, b, c, d, e, a, 1,  0x6D703EF3, 15
	HASHSTEP  F3, a, b, c, d, e, 3,  0x6D703EF3, 11
	HASHSTEP  F3, e, a, b, c, d, 7,  0x6D703EF3, 8
	HASHSTEP  F3, d, e, a, b, c, 14, 0x6D703EF3, 6
	HASHSTEP  F3, c, d, e, a, b, 6,  0x6D703EF3, 6
	HASHSTEP  F3, b, c, d, e, a, 9,  0x6D703EF3, 14

	HASHSTEP  F3, a, b, c, d, e, 11, 0x6D703EF3, 12
	HASHSTEP  F3, e, a, b, c, d, 8,  0x6D703EF3, 13
	HASHSTEP  F3, d, e, a, b, c, 12, 0x6D703EF3, 5
	HASHSTEP  F3, c, d, e, a, b, 2,  0x6D703EF3, 14
	HASHSTEP  F3, b, c, d, e, a, 10, 0x6D703EF3, 13
	HASHSTEP  F3, a, b, c, d, e, 0,  0x6D703EF3, 13
	HASHSTEP  F3, e, a, b, c, d, 4,  0x6D703EF3, 7
	HASHSTEP  F3, d, e, a, b, c, 13, 0x6D703EF3, 5
	HASHSTEP  F2, c, d, e, a, b, 8,  0x7A6D76E9, 15
	HASHSTEP  F2, b, c, d, e, a, 6,  0x7A6D76E9, 5
	HASHSTEP  F2, a, b, c, d, e, 4,  0x7A6D76E9, 8
	HASHSTEP  F2, e, a, b, c, d, 1,  0x7A6D76E9, 11
	HASHSTEP  F2, d, e, a, b, c, 3,  0x7A6D76E9, 14
	HASHSTEP  F2, c, d, e, a, b, 11, 0x7A6D76E9, 14
	HASHSTEP  F2, b, c, d, e, a, 15, 0x7A6D76E9, 6
	HASHSTEP  F2, a, b, c, d, e, 0,  0x7A6D76E9, 14
	HASHSTEP  F2, e, a, b, c, d, 5,  0x7A6D76E9, 6
	HASHSTEP  F2, d, e, a, b, c, 12, 0x7A6D76E9, 9
	HASHSTEP  F2, c, d, e, a, b, 2,  0x7A6D76E9, 12
	HASHSTEP  F2, b, c, d, e, a, 13, 0x7A6D76E9, 9

	HASHSTEP  F2, a, b, c, d, e, 9,  0x7A6D76E9, 12
	HASHSTEP  F2, e, a, b, c, d, 7,  0x7A6D76E9, 5
	HASHSTEP  F2, d, e, a, b, c, 10, 0x7A6D76E9, 15
	HASHSTEP  F2, c, d, e, a, b, 14, 0x7A6D76E9, 8
	HASHSTEP0 F1, b, c, d, e, a, 12, 0x00000000, 8
	HASHSTEP0 F1, a, b, c, d, e, 15, 0x00000000, 5
	HASHSTEP0 F1, e, a, b, c, d, 10, 0x00000000, 12
	HASHSTEP0 F1, d, e, a, b, c, 4,  0x00000000, 9
	HASHSTEP0 F1, c, d, e, a, b, 1,  0x00000000, 12
	HASHSTEP0 F1, b, c, d, e, a, 5,  0x00000000, 5
	HASHSTEP0 F1, a, b, c, d, e, 8,  0x00000000, 14
	HASHSTEP0 F1, e, a, b, c, d, 7,  0x00000000, 6
	HASHSTEP0 F1, d, e, a, b, c, 6,  0x00000000, 8
	HASHSTEP0 F1, c, d, e, a, b, 2,  0x00000000, 13
	HASHSTEP0 F1, b, c, d, e, a, 13, 0x00000000, 6
	HASHSTEP0 F1, a, b, c, d, e, 14, 0x00000000, 5
	HASHSTEP0 F1, e, a, b, c, d, 0,  0x00000000, 15
	HASHSTEP0 F1, d, e, a, b, c, 3,  0x00000000, 13
	HASHSTEP0 F1, c, d, e, a, b, 9,  0x00000000, 11
	HASHSTEP0 F1, b, c, d, e, a, 11, 0x00000000, 11

	mov edx,dword [r8]

	mov eax,dword [r8+4]
	add eax,dword [rsp+8]
	add eax,d
	mov dword [r8],eax
	
	mov eax,dword [r8+8]
	add eax,dword [rsp+12]
	add eax,e
	mov dword [r8+4],eax

	mov eax,dword [r8+12]
	add eax,dword [rsp+16]
	add eax,a
	mov dword [r8+8],eax

	mov eax,dword [r8+16]
	add eax,dword [rsp+0]
	add eax,b
	mov dword [r8+12],eax

	mov eax,edx
	add eax,dword [rsp+4]
	add eax,c
	mov dword [r8+16],eax
	
	add rsp,20
	pop rbp
	pop rdi
	pop rsi
	pop rbx
	ret
