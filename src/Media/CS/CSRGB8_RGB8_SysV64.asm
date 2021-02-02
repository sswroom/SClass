section .text

global CSRGB8_RGB8_UpdateRGBTablePal
global _CSRGB8_RGB8_UpdateRGBTablePal
global CSRGB8_RGB8_Convert
global _CSRGB8_RGB8_Convert

;void CSRGB8_RGB8_UpdateRGBTable(UInt8 *srcPal, UInt8 *destPal, UInt8 *rgbTable, OSInt nColor)
;0 rbx
;8 retAddr
;rcx srcPal
;rdx destPal
;r8 rgbTable
;r9 nColor
	align 16
CSRGB8_RGB8_UpdateRGBTablePal:
_CSRGB8_RGB8_UpdateRGBTablePal:
	push rbx
	align 16
urtlop:
	movzx rbx,byte [rcx+2]
	mov al,byte [r8+rbx+512]
	mov ah,byte [rcx+3]
	shl eax,8
	movzx rbx,byte [rcx+1]
	mov al,byte [r8+rbx+256]
	shl eax,8
	movzx rbx,byte [rcx]
	mov al,byte [r8+rbx]
	mov dword [rdx],eax
	lea rcx,[rcx+4]
	lea rdx,[rdx+4]
	dec r9
	jnz urtlop
	pop rbx
	ret

;void CSRGB8_RGB8_Convert(UInt8 *srcPtr, UInt8 *destPtr, OSInt width, OSInt height, OSInt srcRGBBpl, OSInt destRGBBpl, OSInt srcNBits, OSInt destNbits, UInt8 *srcPal, UInt8 *destPal, UInt8 *rgbTable)
;0 rdi
;8 rsi
;16 rbx
;32 retAddr
;rcx srcPtr
;rdx destPtr
;r8 width
;r9 height
;72 srcRGBBpl
;80 destRGBBpl
;88 srcNBits
;96 destNBits
;104 srcPal
;112 destPal
;120 rgbTable
	align 16
CSRGB8_RGB8_Convert:
_CSRGB8_RGB8_Convert:
	sub rsp,32
	mov qword [rsp],rdi
	mov qword [rsp+8],rsi
	mov qword [rsp+16],rbx
	mov rax,qword [rsp+96] ;destNBits
	cmp rax,32
	jz crgb32start
	jmp crgbexit
	
	align 16
crgb32start:
	mov rax,qword [rsp+88] ;srcNBits
	cmp rax,32
	jz crgb32_32start
	cmp rax,24
	jz crgb32_24start
	cmp rax,16
	jz crgb32_16start
	cmp rax,8
	jz crgb32_8start
	cmp rax,4
	jz crgb32_4start
	jmp crgbexit
	
	align 16
crgb32_32start:
	mov rsi,rcx ;srcPtr
	mov rdi,rdx ;destPtr
	lea rax,[r8*4] ;width * 4
	mov rbx,qword [rsp+120] ;tab
	sub qword [rsp+72],rax ;srcRGBBpl
	sub qword [rsp+80],rax ;destRGBBpl
	
	align 16
crgb32_32lop:
	mov rcx,r8 ;width
	align 16
crgb32_32lop2:
	movzx rax,byte [rsi+2]
	mov dl,byte [rbx+rax+512]
	mov dh,byte [rsi+3]
	movzx rax,byte [rsi+1]
	shl edx,16
	mov dh,byte [rbx+rax+256]
	movzx rax,byte [rsi]
	mov dl,byte [rbx+rax]
	mov dword [rdi],edx
	lea rsi,[rsi+4]
	lea rdi,[rdi+4]
	dec rcx
	jnz crgb32_32lop2
	add rsi,qword [rsp+72] ;srcRGBBpl
	add rdi,qword [rsp+80] ;destRGBBpl
	dec r9
	jnz crgb32_32lop
	jmp crgbexit
	
	align 16
crgb32_24start:
	mov rsi,rcx ;srcPtr
	mov rdi,rdx ;destPtr
	mov rbx,qword [rsp+120] ;tab
	lea rdx,[r8*4]
	lea rax,[r8*2+r8]
	sub qword [rsp+72],rax ;srcRGBBpl
	sub qword [rsp+80],rdx ;destRGBBpl
	align 16
crgb32_24lop:
	mov rcx,r8 ;width
	align 16
crgb32_24lop2:
	movzx rax,byte [rsi+2]
	mov dl,byte [rbx+rax+512]
	mov dh,255
	movzx rax,byte [rsi+1]
	shl edx,16
	mov dh,byte [rbx+rax+256]
	movzx rax,byte [rsi]
	mov dl,byte [rbx+rax]
	mov dword [rdi],edx
	lea rsi,[rsi+3]
	lea rdi,[rdi+4]
	dec rcx
	jnz crgb32_24lop2
	add rsi,qword [rsp+72] ;srcRGBBpl
	add rdi,qword [rsp+80] ;destRGBBpl
	dec r9
	jnz crgb32_24lop
	jmp crgbexit

	align 16
crgb32_16start:
	mov rsi,rcx ;srcPtr
	mov rdi,rdx ;destPtr
	mov rbx,qword [rsp+120] ;tab
	lea rdx,[r8*4]
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
crgb32_16lop2:
	mov rcx,r8 ;width
	align 16
crgb32_16lop:
	movzx eax,word [rsi]
	shl eax,6
	shr ax,3
	shr al,3
	movd xmm0, eax
	punpcklbw xmm0,xmm1
	pmullw xmm0, xmm4
	psrlw xmm0,4
	pminsw xmm0,xmm5
	pmaxsw xmm0,xmm1

	pextrw rdx,xmm0,2
	mov al,byte [rbx+rdx+512]
	mov ah,0xff
	shl eax,16
	pextrw rdx,xmm0,1
	mov ah,byte [rbx+rdx+256]
	pextrw rdx,xmm0,1
	mov al,byte [rbx+rdx]
	mov dword [rdi],eax

	lea rsi,[rsi+2]
	lea rdi,[rdi+4]
	dec rcx
	jnz crgb32_16lop

	add rsi,qword [rsp+72] ;srcRGBBpl
	add rdi,qword [rsp+80] ;destRGBBpl
	dec r9
	jnz crgb32_16lop2
	jmp crgbexit

	align 16
crgb32_8start:
	mov rsi,rcx ;srcPtr
	mov rdi,rdx ;destPtr
	mov rbx,qword [rsp+112] ;destPal
	lea rdx,[r8*4]
	sub qword [rsp+72],r8 ;srcRGBBpl
	sub qword [rsp+80],rdx ;destRGBBpl
	align 16
crgb32_8lop2:
	mov rcx,r8 ;width
	align 16
crgb32_8lop:
	movzx rax,byte [rsi]
	mov edx,dword [rbx+rax*4]
	mov dword [rdi],edx
	inc rsi
	lea rdi,[rdi+4]
	dec rcx
	jnz crgb32_8lop

	add rsi,qword [rsp+72] ;srcRGBBpl
	add rdi,qword [rsp+80] ;destRGBBpl
	dec r9
	jnz crgb32_8lop2
	jmp crgbexit

	align 16
crgb32_4start:	
	mov rsi,rcx ;srcPtr
	mov rdi,rdx ;destPtr
	mov rbx,qword [rsp+112] ;destPal
	mov rax,r8
	lea rdx,[r8*4]
	shr rax,1
	sub qword [rsp+72],rax ;srcRGBBpl
	sub qword [rsp+80],rdx ;destRGBBpl
	align 16
crgb32_4lop2:
	mov rcx,r8 ;width
	shr rcx,1
crgb32_4lop:
	movzx rax,byte [rsi]
	shr rax,4
	mov edx,dword [rbx+rax*4]
	mov dword [rdi],edx

	movzx rax,byte [rsi]
	and rax,0xf
	mov edx,dword [rbx+rax*4]
	mov dword [rdi+4],edx

	inc rsi
	lea rdi,[rdi+8]
	dec rcx
	jnz crgb32_4lop
	test r8,1
	jz crgb32_4lop3

	movzx rax,byte [rsi]
	shr rax,4
	mov edx,dword [rbx+rax*4]
	mov dword [rdi],edx
	inc rsi
	lea rdi,[rdi+4]

	align 16
crgb32_4lop3:
	add rsi,qword [rsp+72] ;srcRGBBpl
	add rdi,qword [rsp+80] ;destRGBBpl
	dec r9 ;height
	jnz crgb32_4lop2
	jmp crgbexit

	align 16
crgbexit:
	mov rdi,qword [rsp]
	mov rsi,qword [rsp+8]
	mov rbx,qword [rsp+16]
	add rsp,32
	ret
