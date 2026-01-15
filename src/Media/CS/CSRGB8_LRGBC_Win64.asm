section .text

global CSRGB8_LRGBC_UpdateRGBTablePal
global CSRGB8_LRGBC_Convert
global CSRGB8_LRGBC_ConvertW8A8
global CSRGB8_LRGBC_ConvertB5G5R5
global CSRGB8_LRGBC_ConvertB5G6R5
global CSRGB8_LRGBC_ConvertR8G8B8
global CSRGB8_LRGBC_ConvertR8G8B8A8
global CSRGB8_LRGBC_ConvertP1_A1
global CSRGB8_LRGBC_ConvertP2_A1
global CSRGB8_LRGBC_ConvertP4_A1
global CSRGB8_LRGBC_ConvertP8_A1

;void CSRGB8_LRGB_UpdateRGBTable(UInt8 *srcPal, UInt8 *destPal, UInt8 *rgbTable, IntOS nColor)
;0 retAddr
;rcx srcPal
;rdx destPal
;r8 rgbTable
;r9 nColor
	align 16
CSRGB8_LRGBC_UpdateRGBTablePal:
urtlop:
	movzx rax,byte [rcx]
	movq xmm1,[r8+rax*8+4096]
	movzx rax,byte [rcx+1]
	movq xmm0,[r8+rax*8+2048]
	paddsw xmm1,xmm0
	movzx rax,byte [rcx+2]
	movq xmm0,[r8+rax*8]
	paddsw xmm1,xmm0
	movq [rdx],xmm1
	lea rcx,[rcx+4]
	lea rdx,[rdx+8]
	dec r9
	jnz urtlop
	ret

;void CSRGB8_LRGBC_Convert(UInt8 *srcPtr, UInt8 *destPtr, IntOS width, IntOS height, IntOS srcRGBBpl, IntOS destRGBBpl, IntOS srcNBits, UInt8 *srcPal, UInt8 *destPal, UInt8 *rgbTable)
;8 rdi
;16 rsi
;24 rbx
;32 retAddr
;rcx srcPtr
;rdx destPtr
;r8 width
;r9 height
;72 srcRGBBpl
;80 destRGBBpl
;88 srcNBits
;96 srcPal
;104 destPal
;112 rgbTable
	align 16
CSRGB8_LRGBC_Convert:
	sub rsp,32
	mov qword [rsp+8],rdi
	mov qword [rsp+16],rsi
	mov qword [rsp+24],rbx
	mov rax,qword [rsp+88] ;srcNBits
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
;8 rdi
;16 rsi
;24 rbx
;32 retAddr
;rcx srcPtr
;rdx destPtr
;r8 width
;r9 height
;72 srcRGBBpl
;80 destRGBBpl
;88 srcNBits
;96 srcPal
;104 destPal
;112 rgbTable
	align 16
CSRGB8_LRGBC_ConvertW8A8:
	sub rsp,32
	mov qword [rsp+8],rdi
	mov qword [rsp+16],rsi
	mov qword [rsp+24],rbx
	mov rax,qword [rsp+88] ;srcNBits
	cmp rax,16
	jz ca8w8start
	jmp crgbexit

;void CSRGB8_LRGBC_ConvertB5G5R5(UInt8 *srcPtr, UInt8 *destPtr, IntOS width, IntOS height, IntOS srcRGBBpl, IntOS destRGBBpl, IntOS srcNBits, UInt8 *srcPal, UInt8 *destPal, UInt8 *rgbTable)
;8 rdi
;16 rsi
;24 rbx
;32 retAddr
;rcx srcPtr
;rdx destPtr
;r8 width
;r9 height
;72 srcRGBBpl
;80 destRGBBpl
;88 srcNBits
;96 srcPal
;104 destPal
;112 rgbTable
	align 16
CSRGB8_LRGBC_ConvertB5G5R5:
	sub rsp,32
	mov qword [rsp+8],rdi
	mov qword [rsp+16],rsi
	mov qword [rsp+24],rbx
	mov rax,qword [rsp+88] ;srcNBits
	cmp rax,16
	jz crgb15start
	jmp crgbexit

;void CSRGB8_LRGBC_ConvertB5G6R5(UInt8 *srcPtr, UInt8 *destPtr, IntOS width, IntOS height, IntOS srcRGBBpl, IntOS destRGBBpl, IntOS srcNBits, UInt8 *srcPal, UInt8 *destPal, UInt8 *rgbTable)
;8 rdi
;16 rsi
;24 rbx
;32 retAddr
;rcx srcPtr
;rdx destPtr
;r8 width
;r9 height
;72 srcRGBBpl
;80 destRGBBpl
;88 srcNBits
;96 srcPal
;104 destPal
;112 rgbTable
	align 16
CSRGB8_LRGBC_ConvertB5G6R5:
	sub rsp,32
	mov qword [rsp+8],rdi
	mov qword [rsp+16],rsi
	mov qword [rsp+24],rbx
	mov rax,qword [rsp+88] ;srcNBits
	cmp rax,16
	jz crgb16start
	jmp crgbexit

;void CSRGB8_LRGBC_ConvertR8G8B8(UInt8 *srcPtr, UInt8 *destPtr, IntOS width, IntOS height, IntOS srcRGBBpl, IntOS destRGBBpl, IntOS srcNBits, UInt8 *srcPal, UInt8 *destPal, UInt8 *rgbTable)
;8 rdi
;16 rsi
;24 rbx
;32 retAddr
;rcx srcPtr
;rdx destPtr
;r8 width
;r9 height
;72 srcRGBBpl
;80 destRGBBpl
;88 srcNBits
;96 srcPal
;104 destPal
;112 rgbTable
	align 16
CSRGB8_LRGBC_ConvertR8G8B8:
	sub rsp,32
	mov qword [rsp+8],rdi
	mov qword [rsp+16],rsi
	mov qword [rsp+24],rbx
	mov rax,qword [rsp+88] ;srcNBits
	cmp rax,24
	jz crgb24rstart
	jmp crgbexit

;void CSRGB8_LRGBC_ConvertR8G8B8A8(UInt8 *srcPtr, UInt8 *destPtr, IntOS width, IntOS height, IntOS srcRGBBpl, IntOS destRGBBpl, IntOS srcNBits, UInt8 *srcPal, UInt8 *destPal, UInt8 *rgbTable)
;8 rdi
;16 rsi
;24 rbx
;32 retAddr
;rcx srcPtr
;rdx destPtr
;r8 width
;r9 height
;72 srcRGBBpl
;80 destRGBBpl
;88 srcNBits
;96 srcPal
;104 destPal
;112 rgbTable
	align 16
CSRGB8_LRGBC_ConvertR8G8B8A8:
	sub rsp,32
	mov qword [rsp+8],rdi
	mov qword [rsp+16],rsi
	mov qword [rsp+24],rbx
	mov rax,qword [rsp+88] ;srcNBits
	cmp rax,32
	jz crgb24rstart
	jmp crgbexit

;void CSRGB8_LRGBC_ConvertP1_A1(UInt8 *srcPtr, UInt8 *destPtr, IntOS width, IntOS height, IntOS srcRGBBpl, IntOS destRGBBpl, IntOS srcNBits, UInt8 *srcPal, UInt8 *destPal, UInt8 *rgbTable)
;8 rdi
;16 rsi
;24 rbx
;32 retAddr
;rcx srcPtr
;rdx destPtr
;r8 width
;r9 height
;72 srcRGBBpl
;80 destRGBBpl
;88 srcNBits
;96 srcPal
;104 destPal
;112 rgbTable
	align 16
CSRGB8_LRGBC_ConvertP1_A1:
	sub rsp,32
	mov qword [rsp+8],rdi
	mov qword [rsp+16],rsi
	mov qword [rsp+24],rbx
	jmp cp1a1start

;void CSRGB8_LRGBC_ConvertP2_A1(UInt8 *srcPtr, UInt8 *destPtr, IntOS width, IntOS height, IntOS srcRGBBpl, IntOS destRGBBpl, IntOS srcNBits, UInt8 *srcPal, UInt8 *destPal, UInt8 *rgbTable)
;8 rdi
;16 rsi
;24 rbx
;32 retAddr
;rcx srcPtr
;rdx destPtr
;r8 width
;r9 height
;72 srcRGBBpl
;80 destRGBBpl
;88 srcNBits
;96 srcPal
;104 destPal
;112 rgbTable
	align 16
CSRGB8_LRGBC_ConvertP2_A1:
	sub rsp,32
	mov qword [rsp+8],rdi
	mov qword [rsp+16],rsi
	mov qword [rsp+24],rbx
	jmp cp2a1start

;void CSRGB8_LRGBC_ConvertP4_A1(UInt8 *srcPtr, UInt8 *destPtr, IntOS width, IntOS height, IntOS srcRGBBpl, IntOS destRGBBpl, IntOS srcNBits, UInt8 *srcPal, UInt8 *destPal, UInt8 *rgbTable)
;8 rdi
;16 rsi
;24 rbx
;32 retAddr
;rcx srcPtr
;rdx destPtr
;r8 width
;r9 height
;72 srcRGBBpl
;80 destRGBBpl
;88 srcNBits
;96 srcPal
;104 destPal
;112 rgbTable
	align 16
CSRGB8_LRGBC_ConvertP4_A1:
	sub rsp,32
	mov qword [rsp+8],rdi
	mov qword [rsp+16],rsi
	mov qword [rsp+24],rbx
	jmp cp4a1start

;void CSRGB8_LRGBC_ConvertP8_A1(UInt8 *srcPtr, UInt8 *destPtr, IntOS width, IntOS height, IntOS srcRGBBpl, IntOS destRGBBpl, IntOS srcNBits, UInt8 *srcPal, UInt8 *destPal, UInt8 *rgbTable)
;8 rdi
;16 rsi
;24 rbx
;32 retAddr
;rcx srcPtr
;rdx destPtr
;r8 width
;r9 height
;72 srcRGBBpl
;80 destRGBBpl
;88 srcNBits
;96 srcPal
;104 destPal
;112 rgbTable
	align 16
CSRGB8_LRGBC_ConvertP8_A1:
	sub rsp,32
	mov qword [rsp+8],rdi
	mov qword [rsp+16],rsi
	mov qword [rsp+24],rbx
	jmp cp8a1start

	align 16
crgb32start:
	mov rsi,rcx ;srcPtr
	mov rdi,rdx ;destPtr
	mov rbx,qword [rsp+112] ;tab
	lea rax,[r8*4]
	lea rdx,[rax*2]
	sub qword [rsp+72],rax ;srcRGBBpl
	sub qword [rsp+80],rdx ;destRGBBpl
	
	align 16
crgb32lop:
	mov rcx,r8 ;width
	align 16
crgb32lop2:
	movzx rax,byte [rsi]
	movq xmm1,[rbx+rax*8+4096]
	movzx rax,byte [rsi+1]
	movq xmm0,[rbx+rax*8+2048]
	paddsw xmm1,xmm0
	movzx rax,byte [rsi+2]
	movq xmm0,[rbx+rax*8]
	paddsw xmm1,xmm0
	movq [rdi],xmm1
	lea rsi,[rsi+4]
	lea rdi,[rdi+8]
	dec rcx
	jnz crgb32lop2
	add rsi,qword [rsp+72] ;srcRGBBpl
	add rdi,qword [rsp+80] ;destRGBBpl
	dec r9
	jnz crgb32lop
	jmp crgbexit
	
	align 16
crgb32rstart:
	mov rsi,rcx ;srcPtr
	mov rdi,rdx ;destPtr
	mov rbx,qword [rsp+112] ;tab
	lea rax,[r8*4]
	lea rdx,[rax*2]
	sub qword [rsp+72],rax ;srcRGBBpl
	sub qword [rsp+80],rdx ;destRGBBpl
	
	align 16
crgb32rlop:
	mov rcx,r8 ;width
	align 16
crgb32rlop2:
	movzx rax,byte [rsi+2]
	movq xmm1,[rbx+rax*8+4096]
	movzx rax,byte [rsi+1]
	movq xmm0,[rbx+rax*8+2048]
	paddsw xmm1,xmm0
	movzx rax,byte [rsi+0]
	movq xmm0,[rbx+rax*8]
	paddsw xmm1,xmm0
	movq [rdi],xmm1
	lea rsi,[rsi+4]
	lea rdi,[rdi+8]
	dec rcx
	jnz crgb32rlop2
	add rsi,qword [rsp+72] ;srcRGBBpl
	add rdi,qword [rsp+80] ;destRGBBpl
	dec r9
	jnz crgb32rlop
	jmp crgbexit
	
	align 16
crgb24start:
	mov rsi,rcx ;srcPtr
	mov rdi,rdx ;destPtr
	mov rbx,qword [rsp+112] ;tab
	lea rdx,[r8*8]
	lea rax,[r8*2+r8]
	sub qword [rsp+72],rax ;srcRGBBpl
	sub qword [rsp+80],rdx ;destRGBBpl
	align 16
crgb24lop:
	mov rcx,r8 ;width
	align 16
crgb24lop2:
	movzx rax,byte [rsi]
	movq xmm1,[rbx+rax*8+4096]
	movzx rax,byte [rsi+1]
	movq xmm0,[rbx+rax*8+2048]
	paddsw xmm1,xmm0
	movzx rax,byte [rsi+2]
	movq xmm0,[rbx+rax*8]
	paddsw xmm1,xmm0
	movq [rdi],xmm1
	lea rsi,[rsi+3]
	lea rdi,[rdi+8]
	dec rcx
	jnz crgb24lop2
	add rsi,qword [rsp+72] ;srcRGBBpl
	add rdi,qword [rsp+80] ;destRGBBpl
	dec r9
	jnz crgb24lop
	jmp crgbexit

	align 16
crgb24rstart:
	mov rsi,rcx ;srcPtr
	mov rdi,rdx ;destPtr
	mov rbx,qword [rsp+112] ;tab
	lea rdx,[r8*8]
	lea rax,[r8*2+r8]
	sub qword [rsp+72],rax ;srcRGBBpl
	sub qword [rsp+80],rdx ;destRGBBpl
	align 16
crgb24rlop:
	mov rcx,r8 ;width
	align 16
crgb24rlop2:
	movzx rax,byte [rsi+2]
	movq xmm1,[rbx+rax*8+4096]
	movzx rax,byte [rsi+1]
	movq xmm0,[rbx+rax*8+2048]
	paddsw xmm1,xmm0
	movzx rax,byte [rsi+0]
	movq xmm0,[rbx+rax*8]
	paddsw xmm1,xmm0
	movq [rdi],xmm1
	lea rsi,[rsi+3]
	lea rdi,[rdi+8]
	dec rcx
	jnz crgb24rlop2
	add rsi,qword [rsp+72] ;srcRGBBpl
	add rdi,qword [rsp+80] ;destRGBBpl
	dec r9
	jnz crgb24rlop
	jmp crgbexit

ca8w8start:
	mov rsi,rcx ;srcPtr
	mov rdi,rdx ;destPtr
	mov rbx,qword [rsp+112] ;tab
	lea rax,[r8*2]
	lea rdx,[rax*4]
	sub qword [rsp+72],rax ;srcRGBBpl
	sub qword [rsp+80],rdx ;destRGBBpl
	
	align 16
ca8w8lop:
	mov rcx,r8 ;width
	align 16
ca8w8lop2:
	movzx rax,byte [rsi]
	movq xmm1,[rbx+rax*8+4096]
	movq xmm0,[rbx+rax*8+2048]
	paddsw xmm1,xmm0
	movq xmm0,[rbx+rax*8]
	paddsw xmm1,xmm0
	movq [rdi],xmm1
	lea rsi,[rsi+2]
	lea rdi,[rdi+8]
	dec rcx
	jnz ca8w8lop2
	add rsi,qword [rsp+72] ;srcRGBBpl
	add rdi,qword [rsp+80] ;destRGBBpl
	dec r9
	jnz ca8w8lop
	jmp crgbexit

	align 16
crgb15start:
	mov rsi,rcx ;srcPtr
	mov rdi,rdx ;destPtr
	mov rbx,qword [rsp+112] ;tab
	lea rdx,[r8*8]
	lea rax,[r8*2]
	sub qword [rsp+72],rax ;srcRGBBpl
	sub qword [rsp+80],rdx ;destRGBBpl
	pxor xmm1,xmm1
	mov ecx,0x00840084
	mov edx,0x00ff00ff
	movd xmm4,ecx
	movd xmm5,edx
	punpckldq xmm4,xmm4
	punpckldq xmm5,xmm5
	align 16
crgb15lop2:
	mov rcx,r8 ;width
	align 16
crgb15lop:
	movzx eax,word [rsi]
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
	movq [rdi],xmm2

	lea rsi,[rsi+2]
	lea rdi,[rdi+8]
	dec rcx
	jnz crgb15lop

	add rsi,qword [rsp+72] ;srcRGBBpl
	add rdi,qword [rsp+80] ;destRGBBpl
	dec r9
	jnz crgb15lop2
	jmp crgbexit

	align 16
crgb16start:
	mov rsi,rcx ;srcPtr
	mov rdi,rdx ;destPtr
	mov rbx,qword [rsp+112] ;tab
	lea rdx,[r8*8]
	lea rax,[r8*2]
	sub qword [rsp+72],rax ;srcRGBBpl
	sub qword [rsp+80],rdx ;destRGBBpl
	pxor xmm1,xmm1
	mov ecx,0x00410084
	mov edx,0x00ff00ff
	movd xmm4,ecx
	movd xmm5,edx
	punpckldq xmm4,xmm4
	punpckldq xmm5,xmm5
	align 16
crgb16lop2:
	mov rcx,r8 ;width
	align 16
crgb16lop:
	movzx eax,word [rsi]
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
	movq [rdi],xmm2

	lea rsi,[rsi+2]
	lea rdi,[rdi+8]
	dec rcx
	jnz crgb16lop

	add rsi,qword [rsp+72] ;srcRGBBpl
	add rdi,qword [rsp+80] ;destRGBBpl
	dec r9
	jnz crgb16lop2
	jmp crgbexit

	align 16
crgb8start:
	mov rsi,rcx ;srcPtr
	mov rdi,rdx ;destPtr
	mov rbx,qword [rsp+104] ;destPal
	lea rdx,[r8*8]
	sub qword [rsp+72],r8 ;srcRGBBpl
	sub qword [rsp+80],rdx ;destRGBBpl
	align 16
crgb8lop2:
	mov rcx,r8 ;width
	align 16
crgb8lop:
	movzx rax,byte [rsi]
	movq xmm0,[rbx+rax*8]
	movq [rdi],xmm0
	inc rsi
	lea rdi,[rdi+8]
	dec rcx
	jnz crgb8lop

	add rsi,qword [rsp+72] ;srcRGBBpl
	add rdi,qword [rsp+80] ;destRGBBpl
	dec r9
	jnz crgb8lop2
	jmp crgbexit

	align 16
crgb4start:	
	mov rsi,rcx ;srcPtr
	mov rdi,rdx ;destPtr
	mov rbx,qword [rsp+104] ;destPal
	mov rax,r8
	lea rdx,[r8*8]
	shr rax,1
	sub qword [rsp+72],rax ;srcRGBBpl
	sub qword [rsp+80],rdx ;destRGBBpl
	align 16
crgb4lop2:
	mov rcx,r8 ;width
	shr rcx,1
crgb4lop:
	movzx rax,byte [rsi]
	shr rax,4
	movq xmm0,[rbx+rax*8]
	movq [rdi],xmm0

	movzx rax,byte [rsi]
	and rax,0xf
	movq xmm0,[rbx+rax*8]
	movq [rdi+8],xmm0

	inc rsi
	lea rdi,[rdi+16]
	dec rcx
	jnz crgb4lop
	test r8,1
	jz crgb4lop3

	movzx rax,byte [rsi]
	shr rax,4
	movq xmm0,[rbx+rax*8]
	movq [rdi],xmm0
	lea rdi,[rdi+8]

	align 16
crgb4lop3:
	add rsi,qword [rsp+72] ;srcRGBBpl
	add rdi,qword [rsp+80] ;destRGBBpl
	dec r9 ;height
	jnz crgb4lop2
	jmp crgbexit

	align 16
crgb2start:	
	mov rsi,rcx ;srcPtr
	mov rdi,rdx ;destPtr
	mov rbx,qword [rsp+104] ;destPal
	mov rax,r8
	lea rdx,[r8*8]
	shr rax,2
	sub qword [rsp+72],rax ;srcRGBBpl
	sub qword [rsp+80],rdx ;destRGBBpl
	align 16
crgb2lop2:
	mov rcx,r8 ;width
	shr rcx,2
crgb2lop:
	mov dl,byte [rsi]
	movzx rax,dl
	shr rax,6
	movq xmm0,[rbx+rax*8]
	movq [rdi],xmm0

	movzx rax,dl
	shr rax,4
	and rax,3
	movq xmm0,[rbx+rax*8]
	movq [rdi+8],xmm0

	movzx rax,dl
	shr rax,2
	and rax,3
	movq xmm0,[rbx+rax*8]
	movq [rdi+16],xmm0

	movzx rax,dl
	and rax,3
	movq xmm0,[rbx+rax*8]
	movq [rdi+24],xmm0

	inc rsi
	lea rdi,[rdi+32]
	dec rcx
	jnz crgb2lop
	test r8,3
	jz crgb2lop3

	mov rcx,r8
	and rcx,3
	movzx rdx,byte [rsi]
crgb2lop4:
	rol dl,2
	movzx rax,dl
	and rax,3
	movq xmm0,[rbx+rax*8]
	movq [rdi],xmm0
	lea rdi,[rdi+8]
	dec rcx
	jnz crgb2lop4

	align 16
crgb2lop3:
	add rsi,qword [rsp+72] ;srcRGBBpl
	add rdi,qword [rsp+80] ;destRGBBpl
	dec r9 ;height
	jnz crgb2lop2
	jmp crgbexit

	align 16
crgb1start:	
	mov rsi,rcx ;srcPtr
	mov rdi,rdx ;destPtr
	mov rbx,qword [rsp+104] ;destPal
	mov rax,r8
	lea rdx,[r8*8]
	shr rax,3
	sub qword [rsp+72],rax ;srcRGBBpl
	sub qword [rsp+80],rdx ;destRGBBpl
	align 16
crgb1lop2:
	mov rcx,r8 ;width
	shr rcx,3
	jz crgb1lop5
crgb1lop:
	mov dl,byte [rsi]
	movzx rax,dl
	shr rax,7
	movq xmm0,[rbx+rax*8]
	movq [rdi],xmm0

	movzx rax,dl
	shr rax,6
	and rax,1
	movq xmm0,[rbx+rax*8]
	movq [rdi+8],xmm0

	movzx rax,dl
	shr rax,5
	and rax,1
	movq xmm0,[rbx+rax*8]
	movq [rdi+16],xmm0

	movzx rax,dl
	shr rax,4
	and rax,1
	movq xmm0,[rbx+rax*8]
	movq [rdi+24],xmm0

	movzx rax,dl
	shr rax,3
	and rax,1
	movq xmm0,[rbx+rax*8]
	movq [rdi+32],xmm0

	movzx rax,dl
	shr rax,2
	and rax,1
	movq xmm0,[rbx+rax*8]
	movq [rdi+40],xmm0

	movzx rax,dl
	shr rax,1
	and rax,1
	movq xmm0,[rbx+rax*8]
	movq [rdi+48],xmm0

	movzx rax,dl
	and rax,1
	movq xmm0,[rbx+rax*8]
	movq [rdi+56],xmm0

	inc rsi
	lea rdi,[rdi+64]
	dec rcx
	jnz crgb1lop
	
crgb1lop5:	
	test r8,7
	jz crgb1lop3

	mov rcx,r8
	and rcx,7
	movzx rdx,byte [rsi]
crgb1lop4:
	rol dl,1
	movzx rax,dl
	and rax,1
	movq xmm0,[rbx+rax*8]
	movq [rdi],xmm0
	lea rdi,[rdi+8]
	dec rcx
	jnz crgb1lop4

	align 16
crgb1lop3:
	add rsi,qword [rsp+72] ;srcRGBBpl
	add rdi,qword [rsp+80] ;destRGBBpl
	dec r9 ;height
	jnz crgb1lop2
	jmp crgbexit

	align 16
cp8a1start:
	mov rsi,rcx ;srcPtr
	mov r10,rdx ;destPtr
	mov rbx,qword [rsp+104] ;destPal
	mov word [rsp],0
	mov word [rsp+2],16383

	align 16
cp8a1lop2:
	mov rcx,r8 ;width
	mov rdi,r10
	align 16
cp8a1lop:
	movzx rax,byte [rsi]
	movq xmm0,[rbx+rax*8]
	movq [rdi],xmm0
	inc rsi
	lea rdi,[rdi+8]
	dec rcx
	jnz cp8a1lop
	
	mov rcx,r8
	mov rdi,r10
	shr rcx,3
	align 16
cp8a1lop3:
	movzx rdx,byte [rsi]

	movzx rax,dl
	shr rax,7
	mov ax,word [rsp+rax*2]
	mov word [rdi+6],ax
	
	movzx rax,dl
	shr rax,6
	and rax,1
	mov ax,word [rsp+rax*2]
	mov word [rdi+14],ax
	
	movzx rax,dl
	shr rax,5
	and rax,1
	mov ax,word [rsp+rax*2]
	mov word [rdi+22],ax
	
	movzx rax,dl
	shr rax,4
	and rax,1
	mov ax,word [rsp+rax*2]
	mov word [rdi+30],ax
	
	movzx rax,dl
	shr rax,3
	and rax,1
	mov ax,word [rsp+rax*2]
	mov word [rdi+38],ax
	
	movzx rax,dl
	shr rax,2
	and rax,1
	mov ax,word [rsp+rax*2]
	mov word [rdi+46],ax
	
	movzx rax,dl
	shr rax,1
	and rax,1
	mov ax,word [rsp+rax*2]
	mov word [rdi+54],ax
	
	movzx rax,dl
	and rax,1
	mov ax,word [rsp+rax*2]
	mov word [rdi+62],ax
	
	inc rsi
	lea rdi,[rdi+64]
	dec rcx
	jnz cp8a1lop3

	mov rcx,r8 ;width
	and rcx,7
	jz cp8a1lop3b
	mov dl,byte [rsi]
	align 16
cp8a1lop3a:
	movzx rax,dl
	shr rax,7
	mov ax,word [rsp+rax*2]
	mov word [rdi+6],ax
	rol dl,1
	lea rdi,[rdi+8]
	dec rcx
	jnz cp8a1lop3a
	inc esi
	
	align 16
cp8a1lop3b:
	add r10,qword [rsp+80] ;destRGBBpl
	dec r9
	jnz cp8a1lop2
	jmp crgbexit

	align 16
cp4a1start:	
	mov rsi,rcx ;srcPtr
	mov r10,rdx ;destPtr
	mov rbx,qword [rsp+104] ;destPal
	mov word [rsp],0
	mov word [rsp+2],16383

	align 16
cp4a1lop2:
	mov rcx,r8 ;width
	mov rdi,r10
	shr rcx,1
	align 16
cp4a1lop:
	movzx rax,byte [rsi]
	shr rax,4
	movq xmm0,[rbx+rax*8]
	movq [rdi],xmm0

	movzx rax,byte [rsi]
	and rax,0xf
	movq xmm0,[rbx+rax*8]
	movq [rdi+8],xmm0

	inc rsi
	lea rdi,[rdi+16]
	dec rcx
	jnz cp4a1lop
	
	test r8,1
	jz cp4a1lop1b
	movzx rax,byte [rsi]
	shr rax,4
	movq xmm0,[rbx+rax*8]
	movq [rdi],xmm0
	inc rsi
	lea rdi,[rdi+8]

	align 16
cp4a1lop1b:
	mov rcx,r8
	mov rdi,r10
	shr rcx,3
	align 16
cp4a1lop3:
	movzx rdx,byte [rsi]

	movzx rax,dl
	shr rax,7
	mov ax,word [rsp+rax*2]
	mov word [rdi+6],ax
	
	movzx rax,dl
	shr rax,6
	and rax,1
	mov ax,word [rsp+rax*2]
	mov word [rdi+14],ax
	
	movzx rax,dl
	shr rax,5
	and rax,1
	mov ax,word [rsp+rax*2]
	mov word [rdi+22],ax
	
	movzx rax,dl
	shr rax,4
	and rax,1
	mov ax,word [rsp+rax*2]
	mov word [rdi+30],ax
	
	movzx rax,dl
	shr rax,3
	and rax,1
	mov ax,word [rsp+rax*2]
	mov word [rdi+38],ax
	
	movzx rax,dl
	shr rax,2
	and rax,1
	mov ax,word [rsp+rax*2]
	mov word [rdi+46],ax
	
	movzx rax,dl
	shr rax,1
	and rax,1
	mov ax,word [rsp+rax*2]
	mov word [rdi+54],ax
	
	movzx rax,dl
	and rax,1
	mov ax,word [rsp+rax*2]
	mov word [rdi+62],ax
	
	inc rsi
	lea rdi,[rdi+64]
	dec rcx
	jnz cp4a1lop3

	mov rcx,r8 ;width
	and rcx,7
	jz cp4a1lop3b
	mov dl,byte [rsi]
	align 16
cp4a1lop3a:
	movzx rax,dl
	shr rax,7
	mov ax,word [rsp+rax*2]
	mov word [rdi+6],ax
	rol dl,1
	lea rdi,[rdi+8]
	dec rcx
	jnz cp4a1lop3a
	inc esi
	
	align 16
cp4a1lop3b:
	add r10,qword [rsp+80] ;destRGBBpl
	dec r9 ;height
	jnz cp4a1lop2
	jmp crgbexit

	align 16
cp2a1start:	
	mov rsi,rcx ;srcPtr
	mov r10,rdx ;destPtr
	mov rbx,qword [rsp+104] ;destPal
	mov word [rsp],0
	mov word [rsp+2],16383

	align 16
cp2a1lop2:
	mov rcx,r8 ;width
	mov rdi,r10
	shr rcx,2
	align 16
cp2a1lop:
	mov dl,byte [rsi]
	movzx rax,dl
	shr rax,6
	movq xmm0,[rbx+rax*8]
	movq [rdi],xmm0

	movzx rax,dl
	shr rax,4
	and rax,3
	movq xmm0,[rbx+rax*8]
	movq [rdi+8],xmm0

	movzx rax,dl
	shr rax,2
	and rax,3
	movq xmm0,[rbx+rax*8]
	movq [rdi+16],xmm0

	movzx rax,dl
	and rax,3
	movq xmm0,[rbx+rax*8]
	movq [rdi+24],xmm0

	inc rsi
	lea rdi,[rdi+32]
	dec rcx
	jnz cp2a1lop

	mov rcx,r8
	and rcx,3
	jz cp2alop1b
	mov dl,byte [rsi]
	align 16
cp2a1lop1a:
	movzx rax,dl
	shr rax,6
	movq xmm0,[rbx+rax*8]
	movq [rdi],xmm0
	rol dl,2
	lea rdi,[rdi+8]
	dec rcx
	jnz cp2a1lop1a
	inc rsi
	
	align 16
cp2alop1b:
	mov rcx,r8
	mov rdi,r10
	shr rcx,3
	align 16
cp2a1lop3:
	movzx rdx,byte [rsi]

	movzx rax,dl
	shr rax,7
	mov ax,word [rsp+rax*2]
	mov word [rdi+6],ax
	
	movzx rax,dl
	shr rax,6
	and rax,1
	mov ax,word [rsp+rax*2]
	mov word [rdi+14],ax
	
	movzx rax,dl
	shr rax,5
	and rax,1
	mov ax,word [rsp+rax*2]
	mov word [rdi+22],ax
	
	movzx rax,dl
	shr rax,4
	and rax,1
	mov ax,word [rsp+rax*2]
	mov word [rdi+30],ax
	
	movzx rax,dl
	shr rax,3
	and rax,1
	mov ax,word [rsp+rax*2]
	mov word [rdi+38],ax
	
	movzx rax,dl
	shr rax,2
	and rax,1
	mov ax,word [rsp+rax*2]
	mov word [rdi+46],ax
	
	movzx rax,dl
	shr rax,1
	and rax,1
	mov ax,word [rsp+rax*2]
	mov word [rdi+54],ax
	
	movzx rax,dl
	and rax,1
	mov ax,word [rsp+rax*2]
	mov word [rdi+62],ax
	
	inc rsi
	lea rdi,[rdi+64]
	dec rcx
	jnz cp2a1lop3

	mov rcx,r8 ;width
	and rcx,7
	jz cp2a1lop3b
	mov dl,byte [rsi]
	align 16
cp2a1lop3a:
	movzx rax,dl
	shr rax,7
	mov ax,word [rsp+rax*2]
	mov word [rdi+6],ax
	rol dl,1
	lea rdi,[rdi+8]
	dec rcx
	jnz cp2a1lop3a
	inc esi
	
	align 16
cp2a1lop3b:
	add r10,qword [rsp+80] ;destRGBBpl
	dec r9 ;height
	jnz cp2a1lop2
	jmp crgbexit

	align 16
cp1a1start:	
	mov rsi,rcx ;srcPtr
	mov r10,rdx ;destPtr
	mov rbx,qword [rsp+104] ;destPal
	mov word [rsp],0
	mov word [rsp+2],16383

	align 16
cp1a1lop2:
	mov rcx,r8 ;width
	mov rdi,r10
	shr rcx,3

	align 16
cp1a1lop:
	mov dl,byte [rsi]
	movzx rax,dl
	shr rax,7
	movq xmm0,[rbx+rax*8]
	movq [rdi],xmm0

	movzx rax,dl
	shr rax,6
	and rax,1
	movq xmm0,[rbx+rax*8]
	movq [rdi+8],xmm0

	movzx rax,dl
	shr rax,5
	and rax,1
	movq xmm0,[rbx+rax*8]
	movq [rdi+16],xmm0

	movzx rax,dl
	shr rax,4
	and rax,1
	movq xmm0,[rbx+rax*8]
	movq [rdi+24],xmm0

	movzx rax,dl
	shr rax,3
	and rax,1
	movq xmm0,[rbx+rax*8]
	movq [rdi+32],xmm0

	movzx rax,dl
	shr rax,2
	and rax,1
	movq xmm0,[rbx+rax*8]
	movq [rdi+40],xmm0

	movzx rax,dl
	shr rax,1
	and rax,1
	movq xmm0,[rbx+rax*8]
	movq [rdi+48],xmm0

	movzx rax,dl
	and rax,1
	movq xmm0,[rbx+rax*8]
	movq [rdi+56],xmm0

	inc rsi
	lea rdi,[rdi+64]
	dec rcx
	jnz cp1a1lop

	mov rcx,r8 ;width
	and rcx,7
	jz cp1a1lop1b
	mov dl,byte [rsi]
	align 16
cp1a1lop1a:
	movzx rax,dl
	shr rax,7
	movq xmm0,[rbx+rax*8]
	movq [rdi],xmm0
	rol dl,1
	lea rdi,[rdi+8]
	dec rcx
	jnz cp1a1lop1a
	inc esi
	
	align 16
cp1a1lop1b:
	mov rcx,r8
	mov rdi,r10
	shr rcx,3
	align 16
cp1a1lop3:
	movzx rdx,byte [rsi]

	movzx rax,dl
	shr rax,7
	mov ax,word [rsp+rax*2]
	mov word [rdi+6],ax
	
	movzx rax,dl
	shr rax,6
	and rax,1
	mov ax,word [rsp+rax*2]
	mov word [rdi+14],ax
	
	movzx rax,dl
	shr rax,5
	and rax,1
	mov ax,word [rsp+rax*2]
	mov word [rdi+22],ax
	
	movzx rax,dl
	shr rax,4
	and rax,1
	mov ax,word [rsp+rax*2]
	mov word [rdi+30],ax
	
	movzx rax,dl
	shr rax,3
	and rax,1
	mov ax,word [rsp+rax*2]
	mov word [rdi+38],ax
	
	movzx rax,dl
	shr rax,2
	and rax,1
	mov ax,word [rsp+rax*2]
	mov word [rdi+46],ax
	
	movzx rax,dl
	shr rax,1
	and rax,1
	mov ax,word [rsp+rax*2]
	mov word [rdi+54],ax
	
	movzx rax,dl
	and rax,1
	mov ax,word [rsp+rax*2]
	mov word [rdi+62],ax
	
	inc rsi
	lea rdi,[rdi+64]
	dec rcx
	jnz cp1a1lop3

	mov rcx,r8 ;width
	and rcx,7
	jz cp1a1lop3b
	mov dl,byte [rsi]
	align 16
cp1a1lop3a:
	movzx rax,dl
	shr rax,7
	mov ax,word [rsp+rax*2]
	mov word [rdi+6],ax
	rol dl,1
	lea rdi,[rdi+8]
	dec rcx
	jnz cp1a1lop3a
	inc esi
	
	align 16
cp1a1lop3b:

	add r10,qword [rsp+80] ;destRGBBpl
	dec r9 ;height
	jnz cp1a1lop2
	jmp crgbexit

	align 16
crgbexit:
	mov rdi,qword [rsp+8]
	mov rsi,qword [rsp+16]
	mov rbx,qword [rsp+24]
	add rsp,32
	ret
