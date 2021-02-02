section .text

global SHA1_CalcBlock

%define A ebx
%define B ecx
%define C esi
%define D edi
%define E ebp

;#define W_T(t) asm{mov eax,dword ptr W[((t * 4) & 63)]} asm{xor eax,dword ptr W[((t * 4 + 8) & 63)]} asm{xor eax,dword ptr W[((t * 4 + 32) & 63)]} asm{xor eax,dword ptr W[((t * 4 + 52) & 63)]} asm{rol eax,1} asm{add edx,eax} asm{mov dword ptr W[((t * 4) & 63)],eax}
%macro W_T 1
	mov eax,dword [rsp + ((%1 * 4) & 63)]
	xor eax,dword [rsp + ((%1 * 4 + 8) & 63)]
	xor eax,dword [rsp + ((%1 * 4 + 32) & 63)]
	xor eax,dword [rsp + ((%1 * 4 + 52) & 63)]
	rol eax,1
	mov dword [rsp + ((%1 * 4) & 63)],eax
	add edx,eax
%endmacro

;#define F1(valA, valB, valC, valD, valE, t)		asm{mov eax,valB}	asm{mov edx,eax}	asm{and edx,valC}	asm{not eax}       asm{and eax, valD} asm{or edx, eax}											asm{mov eax, valA}	asm{rol eax, 5}	asm{add edx,eax} asm{add edx,dword ptr W[t * 4]} asm {add edx, 0x5A827999} asm {add valE,edx} asm{ror valB,2}
%macro F1 6
	mov eax, %2
	mov edx, eax
	and edx, %3
	not eax
	and eax, %4
	or edx, eax
	
	mov r10d, %1
	rol r10d, 5
	add edx, dword [rsp + %6 * 4]
	add edx, 0x5A827999
	add edx, r10d
	add %5, edx
	ror %2, 2
%endmacro

;#define F1_B(valA, valB, valC, valD, valE, t)	asm{mov eax,valB}	asm{mov edx,eax}	asm{and edx,valC}	asm{not eax}       asm{and eax, valD} asm{or edx, eax}											asm{mov eax, valA}	asm{rol eax, 5}	asm{add edx,eax} W_T(t) asm {add edx, 0x5A827999} asm {add valE,edx} asm{ror valB,2}
%macro F1_B 6
	mov eax, %2
	mov edx, eax
	not eax
	and eax, %4
	and edx, %3
	or edx, eax
	
	mov r10d, %1
	rol r10d, 5
	W_T %6
	add edx,r10d
	add edx, 0x5A827999
	add %5, edx
	ror %2, 2
%endmacro

;#define F2(valA, valB, valC, valD, valE, t)		asm{mov edx,valB}	asm{xor edx,valC}	asm{xor edx,valD}																										asm{mov eax, valA} asm{rol eax, 5}	asm{add edx,eax} asm{add edx,dword ptr W[t * 4]} asm {add edx, 0x6ED9EBA1} asm {add valE,edx} asm{ror valB,2}
%macro F2 6
	mov edx, %2
	xor edx, %3
	xor edx, %4
	
	mov r10d, %1
	rol r10d, 5
	add edx, dword [rsp + %6 * 4]
	add edx, 0x6ED9EBA1
	add edx, r10d
	add %5, edx
	ror %2, 2
%endmacro

;#define F2_B(valA, valB, valC, valD, valE, t)	asm{mov edx,valB}	asm{xor edx,valC}	asm{xor edx,valD}																										asm{mov eax, valA} asm{rol eax, 5}	asm{add edx,eax} W_T(t) asm {add edx, 0x6ED9EBA1} asm {add valE,edx} asm{ror valB,2}
%macro F2_B 6
	mov edx, %2
	xor edx, %3
	xor edx, %4
	
	mov r10d, %1
	rol r10d, 5
	W_T %6
	add edx, r10d
	add edx, 0x6ED9EBA1
	add %5, edx
	ror %2, 2
%endmacro

;#define F3(valA, valB, valC, valD, valE, t)		asm{mov eax,valB}	asm{mov edx,eax}	asm{and edx,valC}	asm{and eax,valD}	asm{or edx, eax}	asm{mov eax,valD}	asm{and eax,valC}	asm{or edx,eax}	asm{mov eax, valA} asm{rol eax, 5}	asm{add edx,eax} asm{add edx,dword ptr W[t * 4]} asm {add edx, 0x8F1BBCDC} asm {add valE,edx} asm{ror valB,2}
%macro F3 6
	mov eax, %2
	mov edx, %2
	and eax, %4
	and edx, %3
	or edx, eax
	mov eax, %4
	and eax, %3
	or edx, eax
	
	mov r10d, %1
	rol r10d, 5
	add edx, dword [rsp + %6 * 4]
	add edx, 0x8F1BBCDC
	add edx, r10d
	add %5 ,edx
	ror %2, 2
%endmacro

;#define F3_B(valA, valB, valC, valD, valE, t)	asm{mov eax,valB}	asm{mov edx,eax}	asm{and edx,valC}	asm{and eax,valD}	asm{or edx, eax}	asm{mov eax,valD}	asm{and eax,valC}	asm{or edx,eax}	asm{mov eax, valA} asm{rol eax, 5}	asm{add edx,eax} W_T(t) asm {add edx, 0x8F1BBCDC} asm {add valE,edx} asm{ror valB,2}
%macro F3_B 6
	mov eax, %2
	mov edx, %2
	and eax, %4
	and edx, %3
	or edx, eax
	mov eax, %4
	and eax, %3
	or edx,eax
	
	mov r10d, %1
	rol r10d, 5
	W_T %6
	add edx,r10d
	add edx, 0x8F1BBCDC
	add %5, edx
	ror %2, 2
%endmacro

;#define F4(valA, valB, valC, valD, valE, t)		asm{mov edx,valB}	asm{xor edx,valC}	asm{xor edx,valD}																										asm{mov eax, valA} asm{rol eax, 5}	asm{add edx,eax} asm{add edx,dword ptr W[t * 4]} asm {add edx, 0xCA62C1D6} asm {add valE,edx} asm{ror valB,2}
%macro F4 6
	mov edx, %2
	xor edx, %3
	xor edx, %4
	
	mov r10d, %1
	rol r10d, 5
	add edx, dword [rsp + %6 * 4]
	add edx, 0xCA62C1D6
	add edx, r10d
	add %5, edx
	ror %2, 2
%endmacro

;#define F4_B(valA, valB, valC, valD, valE, t)	asm{mov edx,valB}	asm{xor edx,valC}	asm{xor edx,valD}																										asm{mov eax, valA} asm{rol eax, 5}	asm{add edx,eax} W_T(t) asm {add edx, 0xCA62C1D6} asm {add valE,edx} asm{ror valB,2}
%macro F4_B 6
	mov edx, %2
	xor edx, %4
	xor edx, %3
	
	mov r10d, %1
	rol r10d, 5
	W_T %6
	add edx, r10d
	add edx, 0xCA62C1D6
	add %5, edx
	ror %2, 2
%endmacro

;void SHA1_CalcBlock(UInt32 *Intermediate_Hash, const UInt8 *Message_Block)
;0 W
;64 edi
;72 esi
;80 ebx
;88 ebp
;96 retAddr
;rcx Intermediate_Hash hashVal
;rdx Message_Block
	align 16
SHA1_CalcBlock:
	push rbp
	push rbx
	push rsi
	push rdi
	mov r8,rcx
	mov r9,rdx
	sub rsp,64
	mov rdi,rsp ;W
	mov rsi,r9 ;Message_Block
	mov eax,dword [rsi]
	bswap eax
	mov dword [rdi],eax
	mov eax,dword [rsi+4]
	bswap eax
	mov dword [rdi+4],eax
	mov eax,dword [rsi+8]
	bswap eax
	mov dword [rdi+8],eax
	mov eax,dword [rsi+12]
	bswap eax
	mov dword [rdi+12],eax
	mov eax,dword [rsi+16]
	bswap eax
	mov dword [rdi+16],eax
	mov eax,dword [rsi+20]
	bswap eax
	mov dword [rdi+20],eax
	mov eax,dword [rsi+24]
	bswap eax
	mov dword [rdi+24],eax
	mov eax,dword [rsi+28]
	bswap eax
	mov dword [rdi+28],eax
	mov eax,dword [rsi+32]
	bswap eax
	mov dword [rdi+32],eax
	mov eax,dword [rsi+36]
	bswap eax
	mov dword [rdi+36],eax
	mov eax,dword [rsi+40]
	bswap eax
	mov dword [rdi+40],eax
	mov eax,dword [rsi+44]
	bswap eax
	mov dword [rdi+44],eax
	mov eax,dword [rsi+48]
	bswap eax
	mov dword [rdi+48],eax
	mov eax,dword [rsi+52]
	bswap eax
	mov dword [rdi+52],eax
	mov eax,dword [rsi+56]
	bswap eax
	mov dword [rdi+56],eax
	mov eax,dword [rsi+60]
	bswap eax
	mov dword [rdi+60],eax

	mov ebx,dword [r8] ;hashVal
	mov ecx,dword [r8 + 4]
	mov esi,dword [r8 + 8]
	mov edi,dword [r8 + 12]
	mov ebp,dword [r8 + 16]

	F1 A, B, C, D, E, 0
	F1 E, A, B, C, D, 1
	F1 D, E, A, B, C, 2
	F1 C, D, E, A, B, 3
	F1 B, C, D, E, A, 4
	F1 A, B, C, D, E, 5
	F1 E, A, B, C, D, 6
	F1 D, E, A, B, C, 7
	F1 C, D, E, A, B, 8
	F1 B, C, D, E, A, 9
	F1 A, B, C, D, E, 10
	F1 E, A, B, C, D, 11
	F1 D, E, A, B, C, 12
	F1 C, D, E, A, B, 13
	F1 B, C, D, E, A, 14
	F1 A, B, C, D, E, 15
	F1_B E, A, B, C, D, 16
	F1_B D, E, A, B, C, 17
	F1_B C, D, E, A, B, 18
	F1_B B, C, D, E, A, 19
	F2_B A, B, C, D, E, 20
	F2_B E, A, B, C, D, 21
	F2_B D, E, A, B, C, 22
	F2_B C, D, E, A, B, 23
	F2_B B, C, D, E, A, 24
	F2_B A, B, C, D, E, 25
	F2_B E, A, B, C, D, 26
	F2_B D, E, A, B, C, 27
	F2_B C, D, E, A, B, 28
	F2_B B, C, D, E, A, 29
	F2_B A, B, C, D, E, 30
	F2_B E, A, B, C, D, 31
	F2_B D, E, A, B, C, 32
	F2_B C, D, E, A, B, 33
	F2_B B, C, D, E, A, 34
	F2_B A, B, C, D, E, 35
	F2_B E, A, B, C, D, 36
	F2_B D, E, A, B, C, 37
	F2_B C, D, E, A, B, 38
	F2_B B, C, D, E, A, 39
	F3_B A, B, C, D, E, 40
	F3_B E, A, B, C, D, 41
	F3_B D, E, A, B, C, 42
	F3_B C, D, E, A, B, 43
	F3_B B, C, D, E, A, 44
	F3_B A, B, C, D, E, 45
	F3_B E, A, B, C, D, 46
	F3_B D, E, A, B, C, 47
	F3_B C, D, E, A, B, 48
	F3_B B, C, D, E, A, 49
	F3_B A, B, C, D, E, 50
	F3_B E, A, B, C, D, 51
	F3_B D, E, A, B, C, 52
	F3_B C, D, E, A, B, 53
	F3_B B, C, D, E, A, 54
	F3_B A, B, C, D, E, 55
	F3_B E, A, B, C, D, 56
	F3_B D, E, A, B, C, 57
	F3_B C, D, E, A, B, 58
	F3_B B, C, D, E, A, 59
	F4_B A, B, C, D, E, 60
	F4_B E, A, B, C, D, 61
	F4_B D, E, A, B, C, 62
	F4_B C, D, E, A, B, 63
	F4_B B, C, D, E, A, 64
	F4_B A, B, C, D, E, 65
	F4_B E, A, B, C, D, 66
	F4_B D, E, A, B, C, 67
	F4_B C, D, E, A, B, 68
	F4_B B, C, D, E, A, 69
	F4_B A, B, C, D, E, 70
	F4_B E, A, B, C, D, 71
	F4_B D, E, A, B, C, 72
	F4_B C, D, E, A, B, 73
	F4_B B, C, D, E, A, 74
	F4_B A, B, C, D, E, 75
	F4_B E, A, B, C, D, 76
	F4_B D, E, A, B, C, 77
	F4_B C, D, E, A, B, 78
	F4_B B, C, D, E, A, 79

	add dword [r8], ebx ;hashVal
	add dword [r8 + 4], ecx
	add dword [r8 + 8], esi
	add dword [r8 + 12], edi
	add dword [r8 + 16], ebp

	add rsp,64
	pop rdi
	pop rsi
	pop rbx
	pop rbp
	ret