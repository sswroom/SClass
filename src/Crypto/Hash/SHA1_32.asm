section .text

global _SHA1_CalcBlock
global SHA1_CalcBlock

%define A ebx
%define B ecx
%define C esi
%define D edi
%define E ebp

;#define W_T(t) _asm{mov eax,dword ptr W[((t * 4) & 63)]} _asm{xor eax,dword ptr W[((t * 4 + 8) & 63)]} _asm{xor eax,dword ptr W[((t * 4 + 32) & 63)]} _asm{xor eax,dword ptr W[((t * 4 + 52) & 63)]} _asm{rol eax,1} _asm{add edx,eax} _asm{mov dword ptr W[((t * 4) & 63)],eax}
%macro W_T 1
	mov eax,dword [esp - 64 + ((%1 * 4) & 63)]
	xor eax,dword [esp - 64 + ((%1 * 4 + 8) & 63)]
	xor eax,dword [esp - 64 + ((%1 * 4 + 32) & 63)]
	xor eax,dword [esp - 64 + ((%1 * 4 + 52) & 63)]
	rol eax,1
	add edx,eax
	mov dword [esp - 64 + ((%1 * 4) & 63)],eax
%endmacro

;#define F1(valA, valB, valC, valD, valE, t)		_asm{mov eax,valB}	_asm{mov edx,eax}	_asm{and edx,valC}	_asm{not eax}       _asm{and eax, valD} _asm{or edx, eax}											_asm{mov eax, valA}	_asm{rol eax, 5}	_asm{add edx,eax} _asm{add edx,dword ptr W[t * 4]} _asm {add edx, 0x5A827999} _asm {add valE,edx} _asm{ror valB,2}
%macro F1 6
	mov eax, %2
	mov edx, eax
	and edx, %3
	not eax
	and eax, %4
	or edx, eax
	
	mov eax, %1
	rol eax, 5
	add edx, eax
	add edx, dword [esp - 64 + %6 * 4]
	add edx, 0x5A827999
	add %5, edx
	ror %2, 2
%endmacro

;#define F1_B(valA, valB, valC, valD, valE, t)	_asm{mov eax,valB}	_asm{mov edx,eax}	_asm{and edx,valC}	_asm{not eax}       _asm{and eax, valD} _asm{or edx, eax}											_asm{mov eax, valA}	_asm{rol eax, 5}	_asm{add edx,eax} W_T(t) _asm {add edx, 0x5A827999} _asm {add valE,edx} _asm{ror valB,2}
%macro F1_B 6
	mov eax, %2
	mov edx, eax
	and edx, %3
	not eax
	and eax, %4
	or edx, eax
	
	mov eax, %1
	rol eax, 5
	add edx,eax
	W_T %6
	add edx, 0x5A827999
	add %5, edx
	ror %2, 2
%endmacro

;#define F2(valA, valB, valC, valD, valE, t)		_asm{mov edx,valB}	_asm{xor edx,valC}	_asm{xor edx,valD}																										_asm{mov eax, valA} _asm{rol eax, 5}	_asm{add edx,eax} _asm{add edx,dword ptr W[t * 4]} _asm {add edx, 0x6ED9EBA1} _asm {add valE,edx} _asm{ror valB,2}
%macro F2 6
	mov edx, %2
	xor edx, %3
	xor edx, %4
	
	mov eax, %1
	rol eax, 5
	add edx, eax
	add edx, dword [esp - 64 + %6 * 4]
	add edx, 0x6ED9EBA1
	add %5, edx
	ror %2, 2
%endmacro

;#define F2_B(valA, valB, valC, valD, valE, t)	_asm{mov edx,valB}	_asm{xor edx,valC}	_asm{xor edx,valD}																										_asm{mov eax, valA} _asm{rol eax, 5}	_asm{add edx,eax} W_T(t) _asm {add edx, 0x6ED9EBA1} _asm {add valE,edx} _asm{ror valB,2}
%macro F2_B 6
	mov edx, %2
	xor edx, %3
	xor edx, %4
	
	mov eax, %1
	rol eax, 5
	add edx, eax
	W_T %6
	add edx, 0x6ED9EBA1
	add %5, edx
	ror %2, 2
%endmacro

;#define F3(valA, valB, valC, valD, valE, t)		_asm{mov eax,valB}	_asm{mov edx,eax}	_asm{and edx,valC}	_asm{and eax,valD}	_asm{or edx, eax}	_asm{mov eax,valD}	_asm{and eax,valC}	_asm{or edx,eax}	_asm{mov eax, valA} _asm{rol eax, 5}	_asm{add edx,eax} _asm{add edx,dword ptr W[t * 4]} _asm {add edx, 0x8F1BBCDC} _asm {add valE,edx} _asm{ror valB,2}
%macro F3 6
	mov eax, %2
	mov edx, eax
	and edx, %3
	and eax, %4
	or edx, eax
	mov eax, %4
	and eax, %3
	or edx, eax
	
	mov eax, %1
	rol eax, 5
	add edx, eax
	add edx, dword [esp - 64 + %6 * 4]
	add edx, 0x8F1BBCDC
	add %5 ,edx
	ror %2, 2
%endmacro

;#define F3_B(valA, valB, valC, valD, valE, t)	_asm{mov eax,valB}	_asm{mov edx,eax}	_asm{and edx,valC}	_asm{and eax,valD}	_asm{or edx, eax}	_asm{mov eax,valD}	_asm{and eax,valC}	_asm{or edx,eax}	_asm{mov eax, valA} _asm{rol eax, 5}	_asm{add edx,eax} W_T(t) _asm {add edx, 0x8F1BBCDC} _asm {add valE,edx} _asm{ror valB,2}
%macro F3_B 6
	mov eax, %2
	mov edx, eax
	and edx, %3
	and eax, %4
	or edx, eax
	mov eax, %4
	and eax, %3
	or edx,eax
	
	mov eax, %1
	rol eax, 5
	add edx,eax
	W_T %6
	add edx, 0x8F1BBCDC
	add %5, edx
	ror %2, 2
%endmacro

;#define F4(valA, valB, valC, valD, valE, t)		_asm{mov edx,valB}	_asm{xor edx,valC}	_asm{xor edx,valD}																										_asm{mov eax, valA} _asm{rol eax, 5}	_asm{add edx,eax} _asm{add edx,dword ptr W[t * 4]} _asm {add edx, 0xCA62C1D6} _asm {add valE,edx} _asm{ror valB,2}
%macro F4 6
	mov edx, %2
	xor edx, %3
	xor edx, %4
	
	mov eax, %1
	rol eax, 5
	add edx, eax
	add edx, dword [esp - 64 + %6 * 4]
	add edx, 0xCA62C1D6
	add %5, edx
	ror %2, 2
%endmacro

;#define F4_B(valA, valB, valC, valD, valE, t)	_asm{mov edx,valB}	_asm{xor edx,valC}	_asm{xor edx,valD}																										_asm{mov eax, valA} _asm{rol eax, 5}	_asm{add edx,eax} W_T(t) _asm {add edx, 0xCA62C1D6} _asm {add valE,edx} _asm{ror valB,2}
%macro F4_B 6
	mov edx, %2
	xor edx, %3
	xor edx, %4
	
	mov eax, %1
	rol eax, 5
	add edx, eax
	W_T %6
	add edx, 0xCA62C1D6
	add %5, edx
	ror %2, 2
%endmacro

;void SHA1_CalcBlock(UInt32 *Intermediate_Hash, const UInt8 *Message_Block)
;-64 W
;0 edi
;4 esi
;8 ebx
;12 ebp
;16 retAddr
;20 Intermediate_Hash hashVal
;24 Message_Block
	align 16
_SHA1_CalcBlock:
SHA1_CalcBlock:
	push ebp
	push ebx
	push esi
	push edi

	lea edi,[esp-64] ;W
	mov esi,dword [esp+24] ;Message_Block
	mov eax,dword [esi]
	bswap eax
	mov dword [edi],eax
	mov eax,dword [esi+4]
	bswap eax
	mov dword [edi+4],eax
	mov eax,dword [esi+8]
	bswap eax
	mov dword [edi+8],eax
	mov eax,dword [esi+12]
	bswap eax
	mov dword [edi+12],eax
	mov eax,dword [esi+16]
	bswap eax
	mov dword [edi+16],eax
	mov eax,dword [esi+20]
	bswap eax
	mov dword [edi+20],eax
	mov eax,dword [esi+24]
	bswap eax
	mov dword [edi+24],eax
	mov eax,dword [esi+28]
	bswap eax
	mov dword [edi+28],eax
	mov eax,dword [esi+32]
	bswap eax
	mov dword [edi+32],eax
	mov eax,dword [esi+36]
	bswap eax
	mov dword [edi+36],eax
	mov eax,dword [esi+40]
	bswap eax
	mov dword [edi+40],eax
	mov eax,dword [esi+44]
	bswap eax
	mov dword [edi+44],eax
	mov eax,dword [esi+48]
	bswap eax
	mov dword [edi+48],eax
	mov eax,dword [esi+52]
	bswap eax
	mov dword [edi+52],eax
	mov eax,dword [esi+56]
	bswap eax
	mov dword [edi+56],eax
	mov eax,dword [esi+60]
	bswap eax
	mov dword [edi+60],eax

	mov edx,dword [esp+20] ;hashVal
	mov ebx,dword [edx]
	mov ecx,dword [edx + 4]
	mov esi,dword [edx + 8]
	mov edi,dword [edx + 12]
	mov ebp,dword [edx + 16]

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

	mov edx,dword [esp+20] ;hashVal
	add dword [edx], ebx
	add dword [edx + 4], ecx
	add dword [edx + 8], esi
	add dword [edx + 12], edi
	add dword [edx + 16], ebp

	pop edi
	pop esi
	pop ebx
	pop ebp
	ret
