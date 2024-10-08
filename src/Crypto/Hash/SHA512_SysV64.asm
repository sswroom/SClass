%ifidn __OUTPUT_FORMAT__,elf
section .note.GNU-stack noalloc noexec nowrite progbits
%endif
%ifidn __OUTPUT_FORMAT__,elf32
section .note.GNU-stack noalloc noexec nowrite progbits
%endif
%ifidn __OUTPUT_FORMAT__,elf64
section .note.GNU-stack noalloc noexec nowrite progbits
%endif

section .text

global SHA512_CalcBlock
global _SHA512_CalcBlock

%define a rbx
%define b rcx
%define c rsi
%define d rdi
%define e r8
%define f r9
%define g r10
%define h r11

;	temp1 = w[n] = ReadMUInt64(&Message_Block[n * 8]); \
;	temp1 += ROR64(v5, 14) ^ ROR64(v5, 18) ^ ROR64(v5, 41); \
;	temp1 += (v5 & v6) ^ ((~v5) & v7); \
;	temp1 += v8 + k; \
;	temp2 = ROR64(v1, 28) ^ ROR64(v1, 34) ^ ROR64(v1, 39); \
;	temp2 += (v1 & v2) ^ (v1 & v3) ^ (v2 & v3); \
;	v4 += temp1; \
;	v8 = temp1 + temp2;

%macro SHASTEP1 10
	mov rax,[rbp + %9 * 8]
	bswap rax
	mov [rsp + %9 * 8],rax
	add %8,rax
	mov rax,%10
	add %8,rax
	mov rax,%5
	mov rdx,%5
	ror rax,14
	ror rdx,18
	xor rax,rdx
	mov rdx,%5
	ror rdx,41
	xor rax,rdx
	add %8,rax
	mov rax,%5
	mov rdx,%5
	and rax,%6
	not rdx
	and rdx,%7
	xor rax,rdx
	add %8,rax

	add %4,%8
	mov rax,%1
	mov rdx,%1
	ror rax,28
	ror rdx,34
	xor rax,rdx
	mov rdx,%1
	ror rdx,39
	xor rax,rdx
	add %8,rax
	mov rax,%1
	mov rdx,%1
	and rax,%2
	and rdx,%3
	xor rax,rdx
	mov rdx,%2
	and rdx,%3
	xor rax,rdx
	add %8,rax
%endmacro

;	temp1 = w[n] = w[n - 16] + (ROR64(w[n - 15], 1) ^ ROR64(w[n - 15], 8) ^ (w[n - 15] >> 7)) + w[n - 7] + (ROR64(w[n - 2], 19) ^ ROR64(w[n - 2],  61) ^ (w[n - 2] >> 6)); \
;	temp1 += ROR64(v5, 14) ^ ROR64(v5, 18) ^ ROR64(v5, 41); \
;	temp1 += (v5 & v6) ^ ((~v5) & v7); \
;	temp1 += v8 + k; \
;	temp2 = ROR64(v1, 28) ^ ROR64(v1, 34) ^ ROR64(v1, 39); \
;	temp2 += (v1 & v2) ^ (v1 & v3) ^ (v2 & v3); \
;	v4 += temp1; \
;	v8 = temp1 + temp2;

%macro SHASTEP2 10
	mov rbp,[rsp + %9 * 8 - 15 * 8]
	mov rdx,rbp
	mov rax,rbp
	ror rdx,1
	ror rax,8
	xor rax,rdx
	mov rdx,rbp
	shr rdx,7
	xor rax,rdx
	add rax,[rsp + %9 * 8 - 16 * 8]
	mov rbp,[rsp + %9 * 8 - 2 * 8]
	mov rdx,rbp
	ror rdx,19
	rol rbp,3
	xor rdx,rbp
	mov rbp,[rsp + %9 * 8 - 2 * 8]
	shr rbp,6
	xor rdx,rbp
	add rax,rdx
	mov [rsp + %9 * 8],rax
	add %8,rax
	mov rax,%10
	add %8,rax
	mov rax,%5
	mov rdx,%5
	ror rax,14
	ror rdx,18
	xor rax,rdx
	mov rdx,%5
	ror rdx,41
	xor rax,rdx
	add %8,rax
	mov rax,%5
	mov rdx,%5
	and rax,%6
	not rdx
	and rdx,%7
	xor rax,rdx
	add %8,rax

	add %4,%8
	mov rax,%1
	mov rdx,%1
	ror rax,28
	ror rdx,34
	xor rax,rdx
	mov rdx,%1
	ror rdx,39
	xor rax,rdx
	add %8,rax
	mov rax,%1
	mov rdx,%1
	and rax,%2
	and rdx,%3
	xor rax,rdx
	mov rdx,%2
	and rdx,%3
	xor rax,rdx
	add %8,rax
%endmacro

;void SHA512_CalcBlock(UInt32 *Intermediate_Hash, const UInt8 *Message_Block)
;0 W
;640 rbx
;648 rbp
;656 retAddr
;rdi Intermediate_Hash
;rsi Message_Block
	align 16
SHA512_CalcBlock:
_SHA512_CalcBlock:
	push rbp
	push rbx
	push r12
	sub rsp,640
	mov rbp,rsi ;Message_Block
	mov r12,rdi ;Intermediate_Hash
	mov a,[r12 + 0]
	mov b,[r12 + 8]
	mov c,[r12 + 16]
	mov d,[r12 + 24]
	mov e,[r12 + 32]
	mov f,[r12 + 40]
	mov g,[r12 + 48]
	mov h,[r12 + 56]

	SHASTEP1 a, b, c, d, e, f, g, h, 0, 0x428a2f98d728ae22
	SHASTEP1 h, a, b, c, d, e, f, g, 1, 0x7137449123ef65cd
	SHASTEP1 g, h, a, b, c, d, e, f, 2, 0xb5c0fbcfec4d3b2f
	SHASTEP1 f, g, h, a, b, c, d, e, 3, 0xe9b5dba58189dbbc
	SHASTEP1 e, f, g, h, a, b, c, d, 4, 0x3956c25bf348b538
	SHASTEP1 d, e, f, g, h, a, b, c, 5, 0x59f111f1b605d019
	SHASTEP1 c, d, e, f, g, h, a, b, 6, 0x923f82a4af194f9b
	SHASTEP1 b, c, d, e, f, g, h, a, 7, 0xab1c5ed5da6d8118

	SHASTEP1 a, b, c, d, e, f, g, h, 8, 0xd807aa98a3030242
	SHASTEP1 h, a, b, c, d, e, f, g, 9, 0x12835b0145706fbe
	SHASTEP1 g, h, a, b, c, d, e, f, 10, 0x243185be4ee4b28c
	SHASTEP1 f, g, h, a, b, c, d, e, 11, 0x550c7dc3d5ffb4e2
	SHASTEP1 e, f, g, h, a, b, c, d, 12, 0x72be5d74f27b896f
	SHASTEP1 d, e, f, g, h, a, b, c, 13, 0x80deb1fe3b1696b1
	SHASTEP1 c, d, e, f, g, h, a, b, 14, 0x9bdc06a725c71235
	SHASTEP1 b, c, d, e, f, g, h, a, 15, 0xc19bf174cf692694

	SHASTEP2 a, b, c, d, e, f, g, h, 16, 0xe49b69c19ef14ad2
	SHASTEP2 h, a, b, c, d, e, f, g, 17, 0xefbe4786384f25e3
	SHASTEP2 g, h, a, b, c, d, e, f, 18, 0x0fc19dc68b8cd5b5
	SHASTEP2 f, g, h, a, b, c, d, e, 19, 0x240ca1cc77ac9c65
	SHASTEP2 e, f, g, h, a, b, c, d, 20, 0x2de92c6f592b0275
	SHASTEP2 d, e, f, g, h, a, b, c, 21, 0x4a7484aa6ea6e483
	SHASTEP2 c, d, e, f, g, h, a, b, 22, 0x5cb0a9dcbd41fbd4
	SHASTEP2 b, c, d, e, f, g, h, a, 23, 0x76f988da831153b5

	SHASTEP2 a, b, c, d, e, f, g, h, 24, 0x983e5152ee66dfab
	SHASTEP2 h, a, b, c, d, e, f, g, 25, 0xa831c66d2db43210
	SHASTEP2 g, h, a, b, c, d, e, f, 26, 0xb00327c898fb213f
	SHASTEP2 f, g, h, a, b, c, d, e, 27, 0xbf597fc7beef0ee4
	SHASTEP2 e, f, g, h, a, b, c, d, 28, 0xc6e00bf33da88fc2
	SHASTEP2 d, e, f, g, h, a, b, c, 29, 0xd5a79147930aa725
	SHASTEP2 c, d, e, f, g, h, a, b, 30, 0x06ca6351e003826f
	SHASTEP2 b, c, d, e, f, g, h, a, 31, 0x142929670a0e6e70

	SHASTEP2 a, b, c, d, e, f, g, h, 32, 0x27b70a8546d22ffc
	SHASTEP2 h, a, b, c, d, e, f, g, 33, 0x2e1b21385c26c926
	SHASTEP2 g, h, a, b, c, d, e, f, 34, 0x4d2c6dfc5ac42aed
	SHASTEP2 f, g, h, a, b, c, d, e, 35, 0x53380d139d95b3df
	SHASTEP2 e, f, g, h, a, b, c, d, 36, 0x650a73548baf63de
	SHASTEP2 d, e, f, g, h, a, b, c, 37, 0x766a0abb3c77b2a8
	SHASTEP2 c, d, e, f, g, h, a, b, 38, 0x81c2c92e47edaee6
	SHASTEP2 b, c, d, e, f, g, h, a, 39, 0x92722c851482353b

	SHASTEP2 a, b, c, d, e, f, g, h, 40, 0xa2bfe8a14cf10364
	SHASTEP2 h, a, b, c, d, e, f, g, 41, 0xa81a664bbc423001
	SHASTEP2 g, h, a, b, c, d, e, f, 42, 0xc24b8b70d0f89791
	SHASTEP2 f, g, h, a, b, c, d, e, 43, 0xc76c51a30654be30
	SHASTEP2 e, f, g, h, a, b, c, d, 44, 0xd192e819d6ef5218
	SHASTEP2 d, e, f, g, h, a, b, c, 45, 0xd69906245565a910
	SHASTEP2 c, d, e, f, g, h, a, b, 46, 0xf40e35855771202a
	SHASTEP2 b, c, d, e, f, g, h, a, 47, 0x106aa07032bbd1b8

	SHASTEP2 a, b, c, d, e, f, g, h, 48, 0x19a4c116b8d2d0c8
	SHASTEP2 h, a, b, c, d, e, f, g, 49, 0x1e376c085141ab53
	SHASTEP2 g, h, a, b, c, d, e, f, 50, 0x2748774cdf8eeb99
	SHASTEP2 f, g, h, a, b, c, d, e, 51, 0x34b0bcb5e19b48a8
	SHASTEP2 e, f, g, h, a, b, c, d, 52, 0x391c0cb3c5c95a63
	SHASTEP2 d, e, f, g, h, a, b, c, 53, 0x4ed8aa4ae3418acb
	SHASTEP2 c, d, e, f, g, h, a, b, 54, 0x5b9cca4f7763e373
	SHASTEP2 b, c, d, e, f, g, h, a, 55, 0x682e6ff3d6b2b8a3

	SHASTEP2 a, b, c, d, e, f, g, h, 56, 0x748f82ee5defb2fc
	SHASTEP2 h, a, b, c, d, e, f, g, 57, 0x78a5636f43172f60
	SHASTEP2 g, h, a, b, c, d, e, f, 58, 0x84c87814a1f0ab72
	SHASTEP2 f, g, h, a, b, c, d, e, 59, 0x8cc702081a6439ec
	SHASTEP2 e, f, g, h, a, b, c, d, 60, 0x90befffa23631e28
	SHASTEP2 d, e, f, g, h, a, b, c, 61, 0xa4506cebde82bde9
	SHASTEP2 c, d, e, f, g, h, a, b, 62, 0xbef9a3f7b2c67915
	SHASTEP2 b, c, d, e, f, g, h, a, 63, 0xc67178f2e372532b

	SHASTEP2 a, b, c, d, e, f, g, h, 64, 0xca273eceea26619c
	SHASTEP2 h, a, b, c, d, e, f, g, 65, 0xd186b8c721c0c207
	SHASTEP2 g, h, a, b, c, d, e, f, 66, 0xeada7dd6cde0eb1e
	SHASTEP2 f, g, h, a, b, c, d, e, 67, 0xf57d4f7fee6ed178
	SHASTEP2 e, f, g, h, a, b, c, d, 68, 0x06f067aa72176fba
	SHASTEP2 d, e, f, g, h, a, b, c, 69, 0x0a637dc5a2c898a6
	SHASTEP2 c, d, e, f, g, h, a, b, 70, 0x113f9804bef90dae
	SHASTEP2 b, c, d, e, f, g, h, a, 71, 0x1b710b35131c471b

	SHASTEP2 a, b, c, d, e, f, g, h, 72, 0x28db77f523047d84
	SHASTEP2 h, a, b, c, d, e, f, g, 73, 0x32caab7b40c72493
	SHASTEP2 g, h, a, b, c, d, e, f, 74, 0x3c9ebe0a15c9bebc
	SHASTEP2 f, g, h, a, b, c, d, e, 75, 0x431d67c49c100d4c
	SHASTEP2 e, f, g, h, a, b, c, d, 76, 0x4cc5d4becb3e42b6
	SHASTEP2 d, e, f, g, h, a, b, c, 77, 0x597f299cfc657e2a
	SHASTEP2 c, d, e, f, g, h, a, b, 78, 0x5fcb6fab3ad6faec
	SHASTEP2 b, c, d, e, f, g, h, a, 79, 0x6c44198c4a475817

	add [r12 + 0], a
	add [r12 + 8], b
	add [r12 + 16], c
	add [r12 + 24], d
	add [r12 + 32], e
	add [r12 + 40], f
	add [r12 + 48], g
	add [r12 + 56], h

	add rsp,640
	pop r12
	pop rbx
	pop rbp
	ret