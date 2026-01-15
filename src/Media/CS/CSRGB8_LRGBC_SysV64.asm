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

global CSRGB8_LRGBC_UpdateRGBTablePal
global _CSRGB8_LRGBC_UpdateRGBTablePal
global CSRGB8_LRGBC_Convert
global _CSRGB8_LRGBC_Convert
global CSRGB8_LRGBC_ConvertW8A8
global _CSRGB8_LRGBC_ConvertW8A8
global CSRGB8_LRGBC_ConvertB5G5R5
global _CSRGB8_LRGBC_ConvertB5G5R5
global CSRGB8_LRGBC_ConvertB5G6R5
global _CSRGB8_LRGBC_ConvertB5G6R5
global CSRGB8_LRGBC_ConvertR8G8B8
global _CSRGB8_LRGBC_ConvertR8G8B8
global CSRGB8_LRGBC_ConvertR8G8B8A8
global _CSRGB8_LRGBC_ConvertR8G8B8A8
global CSRGB8_LRGBC_ConvertP1_A1
global _CSRGB8_LRGBC_ConvertP1_A1
global CSRGB8_LRGBC_ConvertP2_A1
global _CSRGB8_LRGBC_ConvertP2_A1
global CSRGB8_LRGBC_ConvertP4_A1
global _CSRGB8_LRGBC_ConvertP4_A1
global CSRGB8_LRGBC_ConvertP8_A1
global _CSRGB8_LRGBC_ConvertP8_A1

;void CSRGB8_LRGB_UpdateRGBTable(UInt8 *srcPal, UInt8 *destPal, UInt8 *rgbTable, IntOS nColor)
;0 retAddr
;rdi srcPal
;rsi destPal
;rdx rgbTable
;rcx nColor
	align 16
CSRGB8_LRGBC_UpdateRGBTablePal:
_CSRGB8_LRGBC_UpdateRGBTablePal:
urtlop:
	movzx rax,byte [rdi]
	movq xmm1,[rdx+rax*8+4096]
	movzx rax,byte [rdi+1]
	movq xmm0,[rdx+rax*8+2048]
	paddsw xmm1,xmm0
	movzx rax,byte [rdi+2]
	movq xmm0,[rdx+rax*8]
	paddsw xmm1,xmm0
	movq [rsi],xmm1
	lea rdi,[rdi+4]
	lea rsi,[rsi+8]
	dec rcx
	jnz urtlop
	ret

;void CSRGB8_LRGBC_Convert(UInt8 *srcPtr, UInt8 *destPtr, IntOS width, IntOS height, IntOS srcRGBBpl, IntOS destRGBBpl, IntOS srcNBits, UInt8 *srcPal, UInt8 *destPal, UInt8 *rgbTable)
;0 retAddr
;rdi srcPtr
;rsi destPtr
;rdx width
;rcx height
;r8 srcRGBBpl
;r9 destRGBBpl
;8 srcNBits
;16 srcPal
;24 destPal
;32 rgbTable
	align 16
CSRGB8_LRGBC_Convert:
_CSRGB8_LRGBC_Convert:
	mov r10,rbx
	mov rax,qword [rsp+8] ;srcNBits
	cmp rax,32
	jz crgb32start
	cmp rax,24
	jz crgb24start
	cmp rax,16
	jz crgb15start
	cmp rax,8
	jz crgb8start
	cmp rax,4
	jz crgb4start
	cmp rax,2
	jz crgb2start
	cmp rax,1
	jz crgb1start
	jmp crgbexit
	
;void CSRGB8_LRGBC_ConvertW8A8(UInt8 *srcPtr, UInt8 *destPtr, IntOS width, IntOS height, IntOS srcRGBBpl, IntOS destRGBBpl, IntOS srcNBits, UInt8 *srcPal, UInt8 *destPal, UInt8 *rgbTable)
;0 retAddr
;rdi srcPtr
;rsi destPtr
;rdx width
;rcx height
;r8 srcRGBBpl
;r9 destRGBBpl
;8 srcNBits
;16 srcPal
;24 destPal
;32 rgbTable
	align 16
CSRGB8_LRGBC_ConvertW8A8:
_CSRGB8_LRGBC_ConvertW8A8:
	mov r10,rbx
	mov rax,qword [rsp+8] ;srcNBits
	cmp rax,16
	jz ca8w8start
	jmp crgbexit

;void CSRGB8_LRGBC_ConvertB5G5R5(UInt8 *srcPtr, UInt8 *destPtr, IntOS width, IntOS height, IntOS srcRGBBpl, IntOS destRGBBpl, IntOS srcNBits, UInt8 *srcPal, UInt8 *destPal, UInt8 *rgbTable)
;0 retAddr
;rdi srcPtr
;rsi destPtr
;rdx width
;rcx height
;r8 srcRGBBpl
;r9 destRGBBpl
;8 srcNBits
;16 srcPal
;24 destPal
;32 rgbTable
	align 16
CSRGB8_LRGBC_ConvertB5G5R5:
_CSRGB8_LRGBC_ConvertB5G5R5:
	mov r10,rbx
	mov rax,qword [rsp+8] ;srcNBits
	cmp rax,16
	jz crgb15start
	jmp crgbexit

;void CSRGB8_LRGBC_ConvertB5G6R5(UInt8 *srcPtr, UInt8 *destPtr, IntOS width, IntOS height, IntOS srcRGBBpl, IntOS destRGBBpl, IntOS srcNBits, UInt8 *srcPal, UInt8 *destPal, UInt8 *rgbTable)
;0 retAddr
;rdi srcPtr
;rsi destPtr
;rdx width
;rcx height
;r8 srcRGBBpl
;r9 destRGBBpl
;8 srcNBits
;16 srcPal
;24 destPal
;32 rgbTable
	align 16
CSRGB8_LRGBC_ConvertB5G6R5:
_CSRGB8_LRGBC_ConvertB5G6R5:
	mov r10,rbx
	mov rax,qword [rsp+8] ;srcNBits
	cmp rax,16
	jz crgb16start
	jmp crgbexit

;void CSRGB8_LRGBC_ConvertR8G8B8(UInt8 *srcPtr, UInt8 *destPtr, IntOS width, IntOS height, IntOS srcRGBBpl, IntOS destRGBBpl, IntOS srcNBits, UInt8 *srcPal, UInt8 *destPal, UInt8 *rgbTable)
;0 retAddr
;rdi srcPtr
;rsi destPtr
;rdx width
;rcx height
;r8 srcRGBBpl
;r9 destRGBBpl
;8 srcNBits
;16 srcPal
;24 destPal
;32 rgbTable
	align 16
CSRGB8_LRGBC_ConvertR8G8B8:
_CSRGB8_LRGBC_ConvertR8G8B8:
	mov r10,rbx
	mov rax,qword [rsp+8] ;srcNBits
	cmp rax,24
	jz crgb24rstart
	jmp crgbexit

;void CSRGB8_LRGBC_ConvertR8G8B8A8(UInt8 *srcPtr, UInt8 *destPtr, IntOS width, IntOS height, IntOS srcRGBBpl, IntOS destRGBBpl, IntOS srcNBits, UInt8 *srcPal, UInt8 *destPal, UInt8 *rgbTable)
;0 retAddr
;rdi srcPtr
;rsi destPtr
;rdx width
;rcx height
;r8 srcRGBBpl
;r9 destRGBBpl
;8 srcNBits
;16 srcPal
;24 destPal
;32 rgbTable
	align 16
CSRGB8_LRGBC_ConvertR8G8B8A8:
_CSRGB8_LRGBC_ConvertR8G8B8A8:
	mov r10,rbx
	mov rax,qword [rsp+8] ;srcNBits
	cmp rax,32
	jz crgb32rstart
	jmp crgbexit

;void CSRGB8_LRGBC_ConvertP1_A1(UInt8 *srcPtr, UInt8 *destPtr, IntOS width, IntOS height, IntOS srcRGBBpl, IntOS destRGBBpl, IntOS srcNBits, UInt8 *srcPal, UInt8 *destPal, UInt8 *rgbTable)
;0 retAddr
;rdi srcPtr
;rsi destPtr
;rdx width
;rcx height
;r8 srcRGBBpl
;r9 destRGBBpl
;8 srcNBits
;16 srcPal
;24 destPal
;32 rgbTable
	align 16
CSRGB8_LRGBC_ConvertP1_A1:
_CSRGB8_LRGBC_ConvertP1_A1:
	mov r10,rbx
	jmp cp1_a1start

;void CSRGB8_LRGBC_ConvertP2_A1(UInt8 *srcPtr, UInt8 *destPtr, IntOS width, IntOS height, IntOS srcRGBBpl, IntOS destRGBBpl, IntOS srcNBits, UInt8 *srcPal, UInt8 *destPal, UInt8 *rgbTable)
;0 retAddr
;rdi srcPtr
;rsi destPtr
;rdx width
;rcx height
;r8 srcRGBBpl
;r9 destRGBBpl
;8 srcNBits
;16 srcPal
;24 destPal
;32 rgbTable
	align 16
CSRGB8_LRGBC_ConvertP2_A1:
_CSRGB8_LRGBC_ConvertP2_A1:
	mov r10,rbx
	jmp cp2_a1start

;void CSRGB8_LRGBC_ConvertP4_A1(UInt8 *srcPtr, UInt8 *destPtr, IntOS width, IntOS height, IntOS srcRGBBpl, IntOS destRGBBpl, IntOS srcNBits, UInt8 *srcPal, UInt8 *destPal, UInt8 *rgbTable)
;0 retAddr
;rdi srcPtr
;rsi destPtr
;rdx width
;rcx height
;r8 srcRGBBpl
;r9 destRGBBpl
;8 srcNBits
;16 srcPal
;24 destPal
;32 rgbTable
	align 16
CSRGB8_LRGBC_ConvertP4_A1:
_CSRGB8_LRGBC_ConvertP4_A1:
	mov r10,rbx
	jmp cp4_a1start

;void CSRGB8_LRGBC_ConvertP8_A1(UInt8 *srcPtr, UInt8 *destPtr, IntOS width, IntOS height, IntOS srcRGBBpl, IntOS destRGBBpl, IntOS srcNBits, UInt8 *srcPal, UInt8 *destPal, UInt8 *rgbTable)
;0 retAddr
;rdi srcPtr
;rsi destPtr
;rdx width
;rcx height
;r8 srcRGBBpl
;r9 destRGBBpl
;8 srcNBits
;16 srcPal
;24 destPal
;32 rgbTable
	align 16
CSRGB8_LRGBC_ConvertP8_A1:
_CSRGB8_LRGBC_ConvertP8_A1:
	mov r10,rbx
	jmp cp8_a1start

	align 16
crgb32start:
	lea rax,[rdx*4]
	lea rbx,[rdx*8]
	sub r8,rax ;srcRGBBpl
	sub r9,rbx ;destRGBBpl
	mov rbx,qword [rsp+32] ;tab
	
	align 16
crgb32lop:
	mov r11,rdx ;width
	align 16
crgb32lop2:
	movzx rax,byte [rdi]
	movq xmm1,[rbx+rax*8+4096]
	movzx rax,byte [rdi+1]
	movq xmm0,[rbx+rax*8+2048]
	paddsw xmm1,xmm0
	movzx rax,byte [rdi+2]
	movq xmm0,[rbx+rax*8]
	paddsw xmm1,xmm0
	movq [rsi],xmm1
	lea rdi,[rdi+4]
	lea rsi,[rsi+8]
	dec r11
	jnz crgb32lop2
	add rdi,r8 ;srcRGBBpl
	add rsi,r9 ;destRGBBpl
	dec rcx
	jnz crgb32lop
	jmp crgbexit
	
	align 16
crgb32rstart:
	lea rax,[rdx*4]
	lea rbx,[rdx*8]
	sub r8,rax ;srcRGBBpl
	sub r9,rbx ;destRGBBpl
	mov rbx,qword [rsp+32] ;tab
	
	align 16
crgb32rlop:
	mov r11,rdx ;width
	align 16
crgb32rlop2:
	movzx rax,byte [rdi+2]
	movq xmm1,[rbx+rax*8+4096]
	movzx rax,byte [rdi+1]
	movq xmm0,[rbx+rax*8+2048]
	paddsw xmm1,xmm0
	movzx rax,byte [rdi+0]
	movq xmm0,[rbx+rax*8]
	paddsw xmm1,xmm0
	movq [rsi],xmm1
	lea rdi,[rdi+4]
	lea rsi,[rsi+8]
	dec r11
	jnz crgb32rlop2
	add rdi,r8 ;srcRGBBpl
	add rsi,r9 ;destRGBBpl
	dec rcx
	jnz crgb32rlop
	jmp crgbexit
	
	align 16
crgb24start:
	lea rax,[rdx*2+rdx]
	lea rbx,[rdx*8]
	sub r8,rax ;srcRGBBpl
	sub r9,rbx ;destRGBBpl
	mov rbx,qword [rsp+32] ;tab
	align 16
crgb24lop:
	mov r11,rdx ;width
	align 16
crgb24lop2:
	movzx rax,byte [rdi]
	movq xmm1,[rbx+rax*8+4096]
	movzx rax,byte [rdi+1]
	movq xmm0,[rbx+rax*8+2048]
	paddsw xmm1,xmm0
	movzx rax,byte [rdi+2]
	movq xmm0,[rbx+rax*8]
	paddsw xmm1,xmm0
	movq [rsi],xmm1
	lea rdi,[rdi+3]
	lea rsi,[rsi+8]
	dec r11
	jnz crgb24lop2
	add rdi,r8 ;srcRGBBpl
	add rsi,r9 ;destRGBBpl
	dec rcx
	jnz crgb24lop
	jmp crgbexit

	align 16
crgb24rstart:
	lea rax,[rdx*2+rdx]
	lea rbx,[rdx*8]
	sub r8,rax ;srcRGBBpl
	sub r9,rbx ;destRGBBpl
	mov rbx,qword [rsp+32] ;tab
	shr rdx,1
	jb crgb24rolop
	align 16
crgb24rlop:
	mov r11,rdx ;width
	align 16
crgb24rlop2:
	movzx rax,byte [rdi+2]
	movq xmm1,[rbx+rax*8+4096]
	movzx rax,byte [rdi+5]
	movhps xmm1,[rbx+rax*8+4096]
	movzx rax,byte [rdi+1]
	movq xmm0,[rbx+rax*8+2048]
	movzx rax,byte [rdi+4]
	movhps xmm0,[rbx+rax*8+2048]
	paddsw xmm1,xmm0
	movzx rax,byte [rdi+0]
	movq xmm0,[rbx+rax*8]
	movzx rax,byte [rdi+3]
	movhps xmm0,[rbx+rax*8]
	paddsw xmm1,xmm0
	movdqu [rsi],xmm1
	add rdi,6
	add rsi,16
	dec r11
	jnz crgb24rlop2
	add rdi,r8 ;srcRGBBpl
	add rsi,r9 ;destRGBBpl
	dec rcx
	jnz crgb24rlop
	jmp crgbexit

	align 16
crgb24rolop:
	mov r11,rdx ;width
	align 16
crgb24rolop2:
	movzx rax,byte [rdi+2]
	movq xmm1,[rbx+rax*8+4096]
	movzx rax,byte [rdi+5]
	movhps xmm1,[rbx+rax*8+4096]
	movzx rax,byte [rdi+1]
	movq xmm0,[rbx+rax*8+2048]
	movzx rax,byte [rdi+4]
	movhps xmm0,[rbx+rax*8+2048]
	paddsw xmm1,xmm0
	movzx rax,byte [rdi+0]
	movq xmm0,[rbx+rax*8]
	movzx rax,byte [rdi+3]
	movhps xmm0,[rbx+rax*8]
	paddsw xmm1,xmm0
	movdqu [rsi],xmm1
	add rdi,6
	add rsi,16
	dec r11
	jnz crgb24rolop2
	movzx rax,byte [rdi+2]
	movq xmm1,[rbx+rax*8+4096]
	movzx rax,byte [rdi+1]
	movq xmm0,[rbx+rax*8+2048]
	paddsw xmm1,xmm0
	movzx rax,byte [rdi+0]
	movq xmm0,[rbx+rax*8]
	paddsw xmm1,xmm0
	movq [rsi],xmm1
	add rdi,3
	add rsi,8

	add rdi,r8 ;srcRGBBpl
	add rsi,r9 ;destRGBBpl
	dec rcx
	jnz crgb24rolop
	jmp crgbexit

ca8w8start:
	lea rax,[rdx*2]
	lea rbx,[rdx*8]
	sub r8,rax ;srcRGBBpl
	sub r9,rbx ;destRGBBpl
	mov rbx,qword [rsp+32] ;tab
	
	align 16
ca8w8lop:
	mov r11,rdx ;width
	align 16
ca8w8lop2:
	movzx rax,byte [rdi]
	movq xmm1,[rbx+rax*8+4096]
	movq xmm0,[rbx+rax*8+2048]
	paddsw xmm1,xmm0
	movq xmm0,[rbx+rax*8]
	paddsw xmm1,xmm0
	movq [rsi],xmm1
	lea rdi,[rdi+2]
	lea rsi,[rsi+8]
	dec rcx
	jnz ca8w8lop2
	add rdi,r8 ;srcRGBBpl
	add rsi,r9 ;destRGBBpl
	dec rcx
	jnz ca8w8lop
	jmp crgbexit

	align 16
crgb15start:
	lea rax,[rdx*2]
	lea rbx,[rdx*8]
	sub r8,rax ;srcRGBBpl
	sub r9,rbx ;destRGBBpl
	pxor xmm1,xmm1
	mov eax,0x00840084
	mov ebx,0x00ff00ff
	movd xmm4,eax
	movd xmm5,ebx
	punpckldq xmm4,xmm4
	punpckldq xmm5,xmm5
	mov rbx,qword [rsp+32] ;tab
	align 16
crgb15lop2:
	mov r11,rdx ;width
	align 16
crgb15lop:
	movzx eax,word [rdi]
	and eax,0x7fff
	shl eax,6
	shr ax,3
	shr al,3
	movd xmm0, eax
	punpcklbw xmm0,xmm1
	pmullw xmm0, xmm4
	psrlw xmm0,4
	pminsw xmm0,xmm5
	pmaxsw xmm0,xmm1

	pextrw rax,xmm0,0
	movq xmm2,[rbx+rax*8+4096]
	pextrw rax,xmm0,1
	movq xmm3,[rbx+rax*8+2048]
	paddsw xmm2,xmm3
	pextrw rax,xmm0,2
	movq xmm3,[rbx+rax*8]
	paddsw xmm2,xmm3
	movq [rsi],xmm2

	lea rdi,[rdi+2]
	lea rsi,[rsi+8]
	dec r11
	jnz crgb15lop

	add rdi,r8 ;srcRGBBpl
	add rsi,r9 ;destRGBBpl
	dec rcx
	jnz crgb15lop2
	jmp crgbexit

	align 16
crgb16start:
	lea rax,[rdx*2]
	lea rbx,[rdx*8]
	sub r8,rax ;srcRGBBpl
	sub r9,rbx ;destRGBBpl
	pxor xmm1,xmm1
	mov eax,0x00410084
	mov ebx,0x00ff00ff
	movd xmm4,eax
	movd xmm5,ebx
	punpckldq xmm4,xmm4
	punpckldq xmm5,xmm5
	mov rbx,qword [rsp+32] ;tab
	align 16
crgb16lop2:
	mov r11,rdx ;width
	align 16
crgb16lop:
	movzx eax,word [rdi]
	shl eax,5
	shr ax,2
	shr al,3
	movd xmm0, eax
	punpcklbw xmm0,xmm1
	pmullw xmm0, xmm4
	psrlw xmm0,4
	pminsw xmm0,xmm5
	pmaxsw xmm0,xmm1

	pextrw rax,xmm0,0
	movq xmm2,[rbx+rax*8+4096]
	pextrw rax,xmm0,1
	movq xmm3,[rbx+rax*8+2048]
	paddsw xmm2,xmm3
	pextrw rax,xmm0,2
	movq xmm3,[rbx+rax*8]
	paddsw xmm2,xmm3
	movq [rsi],xmm2

	lea rdi,[rdi+2]
	lea rsi,[rsi+8]
	dec r11
	jnz crgb16lop

	add rdi,r8 ;srcRGBBpl
	add rsi,r9 ;destRGBBpl
	dec rcx
	jnz crgb16lop2
	jmp crgbexit

;0 retAddr
;rdi srcPtr
;rsi destPtr
;rdx width
;rcx height
;r8 srcRGBBpl
;r9 destRGBBpl
;8 srcNBits
;16 srcPal
;24 destPal
;32 rgbTable
	align 16
crgb8start:
	lea rax,[rdx*8]
	sub r8,rdx ;srcRGBBpl
	sub r9,rax ;destRGBBpl
	mov rbx,qword [rsp+24] ;destPal
	align 16
crgb8lop2:
	mov r11,rdx ;width
	align 16
crgb8lop:
	movzx eax,byte [rdi]
	movq xmm0,[rbx+rax*8]
	movq [rsi],xmm0
	inc rdi
	lea rsi,[rsi+8]
	dec r11
	jnz crgb8lop

	add rdi,r8 ;srcRGBBpl
	add rsi,r9 ;destRGBBpl
	dec rcx
	jnz crgb8lop2
	jmp crgbexit

	align 16
crgb4start:	
	mov rax,rdx
	lea rbx,[rdx*8]
	shr rax,1
	sub r8,rax ;srcRGBBpl
	sub r9,rbx ;destRGBBpl
	mov rbx,qword [rsp+24] ;destPal
	push rbp
	mov rbp,rcx
	align 16
crgb4lop2:
	mov r11,rdx ;width
	shr r11,1
crgb4lop:
	movzx rax,byte [rdi]
	shr rax,4
	movq xmm0,[rbx+rax*8]
	movq [rsi],xmm0

	movzx rax,byte [rdi]
	and rax,0xf
	movq xmm0,[rbx+rax*8]
	movq [rsi+8],xmm0

	inc rdi
	lea rsi,[rsi+16]
	dec r11
	jnz crgb4lop
	test rdx,1
	jz crgb4lop3

	movzx rax,byte [rdi]
	shr rax,4
	movq xmm0,[rbx+rax*8]
	movq [rsi],xmm0
	lea rsi,[rsi+8]

	align 16
crgb4lop3:
	add rdi,r8 ;srcRGBBpl
	add rsi,r9 ;destRGBBpl
	dec rbp ;height
	jnz crgb4lop2
	pop rbp
	jmp crgbexit

	align 16
crgb2start:	
	mov rax,rdx
	lea rbx,[rdx*8]
	shr rax,2
	sub r8,rax ;srcRGBBpl
	sub r9,rbx ;destRGBBpl
	mov rbx,qword [rsp+24] ;destPal
	push rbp
	mov rbp,rcx
	align 16
crgb2lop2:
	mov r11,rdx ;width
	shr r11,2
crgb2lop:
	mov cl,byte [rdi]
	movzx rax,cl
	shr rax,6
	movq xmm0,[rbx+rax*8]
	movq [rsi],xmm0

	movzx rax,cl
	shr rax,4
	and rax,3
	movq xmm0,[rbx+rax*8]
	movq [rsi+8],xmm0

	movzx rax,cl
	shr rax,2
	and rax,3
	movq xmm0,[rbx+rax*8]
	movq [rsi+16],xmm0

	movzx rax,cl
	and rax,3
	movq xmm0,[rbx+rax*8]
	movq [rsi+24],xmm0

	inc rdi
	lea rsi,[rsi+32]
	dec r11
	jnz crgb2lop
	test rdx,3
	jz crgb2lop3

	mov r11,rdx
	and r11,3
	movzx rcx,byte [rdi]
crgb2lop4:
	rol cl,2
	movzx rax,cl
	and rax,3
	movq xmm0,[rbx+rax*8]
	movq [rsi],xmm0
	lea rsi,[rsi+8]
	dec r11
	jnz crgb2lop4

	align 16
crgb2lop3:
	add rdi,r8 ;srcRGBBpl
	add rsi,r9 ;destRGBBpl
	dec rbp ;height
	jnz crgb2lop2
	pop rbp
	jmp crgbexit

;0 retAddr
;rdi srcPtr
;rsi destPtr
;rdx width
;rcx height
;r8 srcRGBBpl
;r9 destRGBBpl
;8 srcNBits
;16 srcPal
;24 destPal
;32 rgbTable
	align 16
crgb1start:	
	mov rax,rdx
	lea rbx,[rdx*8]
	shr rax,3
	sub r8,rax ;srcRGBBpl
	sub r9,rbx ;destRGBBpl
	mov rbx,qword [rsp+24] ;destPal
	push rbp
	mov rbp,rcx
	align 16
crgb1lop2:
	mov r11,rdx ;width
	shr r11,3
	jz crgb1lop5
crgb1lop:
	mov cl,byte [rdi]
	movzx rax,cl
	shr rax,7
	movq xmm0,[rbx+rax*8]
	movq [rsi],xmm0

	movzx rax,cl
	shr rax,6
	and rax,1
	movq xmm0,[rbx+rax*8]
	movq [rsi+8],xmm0

	movzx rax,cl
	shr rax,5
	and rax,1
	movq xmm0,[rbx+rax*8]
	movq [rsi+16],xmm0

	movzx rax,cl
	shr rax,4
	and rax,1
	movq xmm0,[rbx+rax*8]
	movq [rsi+24],xmm0

	movzx rax,cl
	shr rax,3
	and rax,1
	movq xmm0,[rbx+rax*8]
	movq [rsi+32],xmm0

	movzx rax,cl
	shr rax,2
	and rax,1
	movq xmm0,[rbx+rax*8]
	movq [rsi+40],xmm0

	movzx rax,cl
	shr rax,1
	and rax,1
	movq xmm0,[rbx+rax*8]
	movq [rsi+48],xmm0

	movzx rax,cl
	and rax,1
	movq xmm0,[rbx+rax*8]
	movq [rsi+56],xmm0

	inc rdi
	lea rsi,[rsi+64]
	dec r11
	jnz crgb1lop
	
crgb1lop5:	
	test rdx,7
	jz crgb1lop3

	mov r11,rdx
	and r11,7
	movzx rcx,byte [rdi]
crgb1lop4:
	rol cl,1
	movzx rax,cl
	and rax,1
	movq xmm0,[rbx+rax*8]
	movq [rsi],xmm0
	lea rsi,[rsi+8]
	dec r11
	jnz crgb1lop4

	align 16
crgb1lop3:
	add rdi,r8 ;srcRGBBpl
	add rsi,r9 ;destRGBBpl
	dec rbp ;height
	jnz crgb1lop2
	pop rbp
	jmp crgbexit

	align 16
cp8_a1start:
	mov r8,rsi
	mov eax,0x3fff0000
	mov rbx,qword [rsp+24] ;destPal
	push rax
	align 16
cp8_a1lop2:
	mov r11,rdx ;width
	mov rsi,r8
	shr r11,3
	align 16
cp8_a1lop:
	movzx eax,byte [rdi]
	movq xmm0,[ebx+eax*8]
	movq [rsi],xmm0
	inc rdi
	lea rsi,[rsi+8]
	dec r11
	jnz cp8_a1lop

	mov r11,rdx ;width
	mov rsi,r8
	shr r11,3

	align 16
cp8_a1lop3:
	movzx rax,byte [rdi]
	shr rax,7
	mov ax,[rsp+rax*2]
	mov [rsi+6],ax

	movzx rax,byte [rdi]
	shr rax,6
	and rax,1
	mov ax,[rsp+rax*2]
	mov [rsi+14],ax

	movzx rax,byte [rdi]
	shr rax,5
	and rax,1
	mov ax,[rsp+rax*2]
	mov [rsi+22],ax

	movzx rax,byte [rdi]
	shr rax,4
	and rax,1
	mov ax,[rsp+rax*2]
	mov [rsi+30],ax

	movzx rax,byte [rdi]
	shr rax,3
	and rax,1
	mov ax,[rsp+rax*2]
	mov [rsi+38],ax

	movzx rax,byte [rdi]
	shr rax,2
	and rax,1
	mov ax,[rsp+rax*2]
	mov [rsi+46],ax

	movzx rax,byte [rdi]
	shr rax,1
	and rax,1
	mov ax,[rsp+rax*2]
	mov [rsi+54],ax

	movzx rax,byte [rdi]
	and rax,1
	mov ax,[rsp+rax*2]
	mov [rsi+62],ax

	inc rdi
	lea rsi,[rsi+64]
	dec r11
	jnz cp8_a1lop3
	
	mov r11,rdx ;width
	and r11,7
	jz cp8_a1lop3b
	movq xmm3,rcx
	movzx rcx,byte [rdi]
	lea rdi,[rdi+1]
	align 16
cp8_a1lop3a:
	rol cl,1
	movzx rax,cl
	and rax,1
	mov ax,[rsp+rax*2]
	mov [rsi+6],ax

	lea rsi,[rsi+8]
	dec r11
	jnz cp8_a1lop3a
	movq rcx,xmm3
	align 16
cp8_a1lop3b:
	
	add r8,r9 ;destRGBBpl
	dec rcx
	jnz cp8_a1lop2
	pop rax
	jmp crgbexit

	align 16
cp4_a1start:	
	mov r8,rsi
	mov eax,0x3fff0000
	mov rbx,qword [rsp+24] ;destPal
	push rax
	align 16
cp4_a1lop2:
	mov r11,rdx ;width
	mov rsi,r8
	shr r11,1
	align 16
cp4_a1lop:
	movzx rax,byte [rdi]
	shr rax,4
	movq xmm0,[rbx+rax*8]
	movq [rsi],xmm0

	movzx rax,byte [rdi]
	and rax,0xf
	movq xmm0,[rbx+rax*8]
	movq [rsi+8],xmm0

	inc rdi
	lea rsi,[rsi+16]
	dec r11
	jnz cp4_a1lop

	test rdx,1 ;width
	jz cp4_a1lop1b
	movzx rax,byte [rdi]
	shr rax,4
	movq xmm0,[rbx+rax*8]
	movq [rsi],xmm0
	inc rdi
	lea rsi,[rsi+8]
	align 16
cp4_a1lop1b:

	mov r11,rdx ;width
	mov rsi,r8
	shr r11,3

	align 16
cp4_a1lop3:
	movzx rax,byte [rdi]
	shr rax,7
	mov ax,[rsp+rax*2]
	mov [rsi+6],ax

	movzx rax,byte [rdi]
	shr rax,6
	and rax,1
	mov ax,[rsp+rax*2]
	mov [rsi+14],ax

	movzx rax,byte [rdi]
	shr rax,5
	and rax,1
	mov ax,[rsp+rax*2]
	mov [rsi+22],ax

	movzx rax,byte [rdi]
	shr rax,4
	and rax,1
	mov ax,[rsp+rax*2]
	mov [rsi+30],ax

	movzx rax,byte [rdi]
	shr rax,3
	and rax,1
	mov ax,[rsp+rax*2]
	mov [rsi+38],ax

	movzx rax,byte [rdi]
	shr rax,2
	and rax,1
	mov ax,[rsp+rax*2]
	mov [rsi+46],ax

	movzx rax,byte [rdi]
	shr rax,1
	and rax,1
	mov ax,[rsp+rax*2]
	mov [rsi+54],ax

	movzx rax,byte [rdi]
	and rax,1
	mov ax,[rsp+rax*2]
	mov [rsi+62],ax

	inc rdi
	lea rsi,[rsi+64]
	dec r11
	jnz cp4_a1lop3

	mov r11,rdx ;width
	and r11,7
	jz cp4_a1lop3b
	movq xmm3,rcx
	movzx rcx,byte [rdi]
	lea rdi,[rdi+1]
	align 16
cp4_a1lop3a:
	rol cl,1
	movzx rax,cl
	and rax,1
	mov ax,[rsp+rax*2]
	mov [rsi+6],ax

	lea rsi,[rsi+8]
	dec r11
	jnz cp4_a1lop3a
	movq rcx,xmm3
	align 16
cp4_a1lop3b:
	
	add r8,r9 ;destRGBBpl
	dec rcx ;height
	jnz cp4_a1lop2
	pop rax
	jmp crgbexit

	align 16
cp2_a1start:	
	mov r8,rsi
	mov eax,0x3fff0000
	mov rbx,qword [rsp+24] ;destPal
	push rax
	align 16
cp2_a1lop2:
	mov r11,rdx ;width
	mov rsi,r8
	shr r11,2
	align 16
cp2_a1lop:
	movzx rax,byte [rdi]
	shr rax,6
	movq xmm0,[rbx+rax*8]
	movq [rsi],xmm0

	movzx rax,byte [rdi]
	shr rax,4
	and rax,3
	movq xmm0,[rbx+rax*8]
	movq [rsi+8],xmm0

	movzx rax,byte [rdi]
	shr rax,2
	and rax,3
	movq xmm0,[rbx+rax*8]
	movq [rsi+16],xmm0

	movzx rax,byte [rdi]
	and rax,3
	movq xmm0,[rbx+rax*8]
	movq [rsi+24],xmm0

	inc rdi
	lea rsi,[rsi+32]
	dec r11
	jnz cp2_a1lop

	mov r11,rdx ;width
	and r11,3
	jz cp2_a1lop1b
	movq xmm3,rcx
	movzx rcx,byte [rdi]
	lea rdi,[rdi+1]
	align 16
cp2_a1lop1a:
	rol cl,2
	movzx rax,cl
	and rax,3
	movq xmm0,[rbx+rax*8]
	movq [rsi],xmm0

	lea rsi,[rsi+32]
	dec r11
	jnz cp2_a1lop1a
	movq rcx,xmm3
	align 16
cp2_a1lop1b:

	mov r11,rdx ;width
	mov rsi,r8
	shr r11,3

	align 16
cp2_a1lop3:
	movzx rax,byte [rdi]
	shr rax,7
	mov ax,[rsp+rax*2]
	mov [rsi+6],ax

	movzx rax,byte [rdi]
	shr rax,6
	and rax,1
	mov ax,[rsp+rax*2]
	mov [rsi+14],ax

	movzx rax,byte [rdi]
	shr rax,5
	and rax,1
	mov ax,[rsp+rax*2]
	mov [rsi+22],ax

	movzx rax,byte [rdi]
	shr rax,4
	and rax,1
	mov ax,[rsp+rax*2]
	mov [rsi+30],ax

	movzx rax,byte [rdi]
	shr rax,3
	and rax,1
	mov ax,[rsp+rax*2]
	mov [rsi+38],ax

	movzx rax,byte [rdi]
	shr rax,2
	and rax,1
	mov ax,[rsp+rax*2]
	mov [rsi+46],ax

	movzx rax,byte [rdi]
	shr rax,1
	and rax,1
	mov ax,[rsp+rax*2]
	mov [rsi+54],ax

	movzx rax,byte [rdi]
	and rax,1
	mov ax,[rsp+rax*2]
	mov [rsi+62],ax

	inc rdi
	lea rsi,[rsi+64]
	dec r11
	jnz cp2_a1lop3
	
	mov r11,rdx ;width
	and r11,7
	jz cp2_a1lop3b
	movq xmm3,rcx
	movzx rcx,byte [rdi]
	lea rdi,[rdi+1]
	align 16
cp2_a1lop3a:
	rol cl,1
	movzx rax,cl
	and rax,1
	mov ax,[rsp+rax*2]
	mov [rsi+6],ax

	lea rsi,[rsi+8]
	dec r11
	jnz cp2_a1lop3a
	movq rcx,xmm3
	align 16
cp2_a1lop3b:
	
	add r8,r9 ;destRGBBpl
	dec rcx ;height
	jnz cp2_a1lop2
	pop rax
	jmp crgbexit

	align 16
cp1_a1start:	
	mov r8,rsi
	mov eax,0x3fff0000
	mov rbx,qword [rsp+24] ;destPal
	push rax
	align 16
cp1_a1lop2:
	mov r11,rdx ;width
	mov rsi,r8
	shr r11,3

	align 16
cp1_a1lop:
	movzx rax,byte [rdi]
	shr rax,7
	movq xmm0,[rbx+rax*8]
	movq [rsi],xmm0

	movzx rax,byte [rdi]
	shr rax,6
	and rax,1
	movq xmm0,[rbx+rax*8]
	movq [rsi+8],xmm0

	movzx rax,byte [rdi]
	shr rax,5
	and rax,1
	movq xmm0,[rbx+rax*8]
	movq [rsi+16],xmm0

	movzx rax,byte [rdi]
	shr rax,4
	and rax,1
	movq xmm0,[rbx+rax*8]
	movq [rsi+24],xmm0

	movzx rax,byte [rdi]
	shr rax,3
	and rax,1
	movq xmm0,[rbx+rax*8]
	movq [rsi+32],xmm0

	movzx rax,byte [rdi]
	shr rax,2
	and rax,1
	movq xmm0,[rbx+rax*8]
	movq [rsi+40],xmm0

	movzx rax,byte [rdi]
	shr rax,1
	and rax,1
	movq xmm0,[rbx+rax*8]
	movq [rsi+48],xmm0

	movzx rax,byte [rdi]
	and rax,1
	movq xmm0,[rbx+rax*8]
	movq [rsi+56],xmm0

	inc rdi
	lea rsi,[rsi+64]
	dec r11
	jnz cp1_a1lop
	
	mov r11,rdx ;width
	and r11,7
	jz cp1_a1lop1b
	movq xmm3,rcx
	movzx rcx,byte [rdi]
	lea rdi,[rdi+1]
	align 16
cp1_a1lop1a:
	rol cl,1
	movzx rax,cl
	and rax,1
	movq xmm0,[rbx+rax*8]
	movq [rsi],xmm0
	lea rsi,[rsi+8]
	dec r11
	jnz cp1_a1lop1a
	movq rcx,xmm3
	align 16
cp1_a1lop1b:
	
	mov r11,rdx ;width
	mov rsi,r8
	shr r11,3

	align 16
cp1_a1lop3:
	movzx rax,byte [rdi]
	shr rax,7
	mov ax,[rsp+rax*2]
	mov [rsi+6],ax

	movzx rax,byte [rdi]
	shr rax,6
	and rax,1
	mov ax,[rsp+rax*2]
	mov [rsi+14],ax

	movzx rax,byte [rdi]
	shr rax,5
	and rax,1
	mov ax,[rsp+rax*2]
	mov [rsi+22],ax

	movzx rax,byte [rdi]
	shr rax,4
	and rax,1
	mov ax,[rsp+rax*2]
	mov [rsi+30],ax

	movzx rax,byte [rdi]
	shr rax,3
	and rax,1
	mov ax,[rsp+rax*2]
	mov [rsi+38],ax

	movzx rax,byte [rdi]
	shr rax,2
	and rax,1
	mov ax,[rsp+rax*2]
	mov [rsi+46],ax

	movzx rax,byte [rdi]
	shr rax,1
	and rax,1
	mov ax,[rsp+rax*2]
	mov [rsi+54],ax

	movzx rax,byte [rdi]
	and rax,1
	mov ax,[rsp+rax*2]
	mov [rsi+62],ax

	inc rdi
	lea rsi,[rsi+64]
	dec r11
	jnz cp1_a1lop3
	
	mov r11,rdx ;width
	and r11,7
	jz cp1_a1lop3b
	movq xmm3,rcx
	movzx rcx,byte [rdi]
	lea rdi,[rdi+1]
	align 16
cp1_a1lop3a:
	rol cl,1
	movzx rax,cl
	and rax,1
	mov ax,[rsp+rax*2]
	mov [rsi+6],ax

	lea rsi,[rsi+8]
	dec r11
	jnz cp1_a1lop3a
	movq rcx,xmm3
	align 16
cp1_a1lop3b:
	
	add r8,r9 ;destRGBBpl
	dec rcx ;height
	jnz cp1_a1lop2
	pop rax
	jmp crgbexit

	align 16
crgbexit:
	mov rbx,r10
	ret
