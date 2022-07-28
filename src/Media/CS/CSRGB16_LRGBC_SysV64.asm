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

global CSRGB16_LRGBC_Convert
global _CSRGB16_LRGBC_Convert
global CSRGB16_LRGBC_ConvertA2B10G10R10
global _CSRGB16_LRGBC_ConvertA2B10G10R10

;void CSRGB16_LRGBC_Convert(UInt8 *srcPtr, UInt8 *destPtr, OSInt width, OSInt height, OSInt srcNBits, OSInt srcRGBBpl, OSInt destRGBBpl, UInt8 *rgbTable);
;0 rbx
;8 retAddr
;rdi rcx srcPtr
;rsi rdx destPtr
;rdx r8 width
;rcx r9 height
;r8 srcNBits
;r9 srcRGBBpl
;16 destRGBBpl
;24 rgbTable
	align 16
CSRGB16_LRGBC_Convert:
_CSRGB16_LRGBC_Convert:
	push rbx
	mov rbx,qword [rsp+24] ;rgbTable
	cmp r8,64
	jz crgb64start
	cmp r8,48
	jz crgb48start
	cmp r8,32
	jz crgb32start
	cmp r8,16
	jz crgb16start
	jmp crgbexit
	
;void CSRGB16_LRGBC_ConvertA2B10G10R10(UInt8 *srcPtr, UInt8 *destPtr, OSInt width, OSInt height, OSInt srcNBits, OSInt srcRGBBpl, OSInt destRGBBpl, UInt8 *rgbTable);
;0 rbx
;8 retAddr
;rdi rcx srcPtr
;rsi rdx destPtr
;rdx r8 width
;rcx r9 height
;r8 srcNBits
;r9 srcRGBBpl
;16 destRGBBpl
;24 rgbTable
	align 16
CSRGB16_LRGBC_ConvertA2B10G10R10:
_CSRGB16_LRGBC_ConvertA2B10G10R10:
	push rbx
	mov rbx,qword [rsp+24] ;rgbTable
	cmp r8,32
	jz ca2b10g10r10start
	jmp crgbexit
	
	align 16
crgb64start:
	lea rax,[rdx*8]
	mov r11,qword [rsp+16] ;destRGBBpl
	sub r9,rax ;srcRGBBpl
	sub r11,rax ;destRGBBpl
	
	align 16
crgb64lop:
	mov r8,rdx ;width
	align 16
crgb64lop2:
	movzx rax,word [rdi]
	movq xmm1,[rbx+rax*8+1048576]
	movzx rax,word [rdi+2]
	movq xmm0,[rbx+rax*8+524288]
	paddsw xmm1,xmm0
	movzx rax,word [rdi+4]
	movq xmm0,[rbx+rax*8]
	paddsw xmm1,xmm0
	movq [rsi],xmm1
	lea rdi,[rdi+8]
	lea rsi,[rsi+8]
	dec r8
	jnz crgb64lop2
	add rdi,r9 ;srcRGBBpl
	add rsi,r11 ;destRGBBpl
	dec rcx
	jnz crgb64lop
	jmp crgbexit

	align 16
crgb32start:
	lea rax,[rdx*4]
	lea r8,[rdx*8]
	mov r11,qword [rsp+16] ;destRGBBpl
	sub r9,rax ;srcRGBBpl
	sub r11,r8 ;destRGBBpl
	
	align 16
crgb32lop:
	mov r8,rdx ;width
	align 16
crgb32lop2:
	movzx rax,word [rdi]
	movq xmm1,[rbx+rax*8+1048576]
	movq xmm0,[rbx+rax*8+524288]
	paddsw xmm1,xmm0
	movq xmm0, [rbx+rax*8]
	paddsw xmm1,xmm0
	movq [rsi],xmm1
	lea rdi,[rdi+4]
	lea rsi,[rsi+8]
	dec r8
	jnz crgb32lop2
	add rdi,r9 ;srcRGBBpl
	add rsi,r11 ;destRGBBpl
	dec rcx
	jnz crgb32lop
	jmp crgbexit
	
	align 16
ca2b10g10r10start:
	lea rax,[rdx*4]
	lea r8,[rdx*8]
	mov r11,qword [rsp+16] ;destRGBBpl
	sub r9,rax ;srcRGBBpl
	sub r11,r8 ;destRGBBpl
	mov r10,rcx
	
	align 16
ca2b10g10r10lop:
	mov r8,rdx ;width
	align 16
ca2b10g10r10lop2:
	mov ecx,dword [rdi]
	mov rax,rcx
	shr rcx,14
	shl rax,6
	shr cx,6
	shr ax,6
	movd xmm1,ecx
	movd xmm2,eax
	punpckldq xmm2,xmm1
	psllw xmm2,6
	movdqa xmm1,xmm2
	psrlw xmm2,10
	por xmm2,xmm1
	pextrw rax,xmm2,0
	movq xmm1, [rbx+rax*8]
	pextrw rax,xmm2,1
	movq xmm0,[rbx+rax*8+524288]
	paddsw xmm1,xmm0
	pextrw rax,xmm2,2
	movq xmm0,[rbx+rax*8+1048576]
	paddsw xmm1,xmm0
	movq [rsi],xmm1
	lea rdi,[rdi+4]
	lea rsi,[rsi+8]
	dec r8
	jnz ca2b10g10r10lop2
	add rdi,r9 ;srcRGBBpl
	add rsi,r11 ;destRGBBpl
	dec r10
	jnz ca2b10g10r10lop
	jmp crgbexit
	
	align 16
crgb16start:
	lea rax,[rdx*2]
	lea r8,[rdx*8]
	mov r11,qword [rsp+16] ;destRGBBpl
	sub r9,rax ;srcRGBBpl
	sub r11,r8 ;destRGBBpl
	
	align 16
crgb16lop:
	mov r8,rdx ;width
	align 16
crgb16lop2:
	movzx rax,word [rdi]
	movq xmm1,[rbx+rax*8+1048576]
	movq xmm0,[rbx+rax*8+524288]
	paddsw xmm1,xmm0
	movq xmm2, [rbx+rax*8]
	paddsw xmm1,xmm2
	movq [rsi],xmm1
	lea rdi,[rdi+2]
	lea rsi,[rsi+8]
	dec r8
	jnz crgb16lop2
	add rdi,r9 ;srcRGBBpl
	add rsi,r11 ;destRGBBpl
	dec rcx
	jnz crgb16lop
	jmp crgbexit

;0 rbx
;8 retAddr
;rdi rcx srcPtr
;rsi rdx destPtr
;rdx r8 width
;rcx r9 height
;r8 srcNBits
;r9 srcRGBBpl
;16 destRGBBpl
;24 rgbTable
	align 16
crgb48start:
	lea rax,[rdx+rdx*2]
	lea r8,[rdx*8]
	lea rax,[rax*2]
	mov r11,qword [rsp+16] ;destRGBBpl
	sub r9,rax ;srcRGBBpl
	sub r11,r8 ;destRGBBpl
	align 16
crgb48lop:
	mov r8,rdx ;width
	align 16
crgb48lop2:
	movzx rax,word [rdi]
	movq xmm1,[rbx+rax*8+1048576]
	movzx rax,word [rdi+2]
	movq xmm0,[rbx+rax*8+524288]
	paddsw xmm1,xmm0
	movzx rax,word [rdi+4]
	movq xmm0,[rbx+rax*8]
	paddsw xmm1,xmm0
	movq [rsi],xmm1
	lea rsi,[rsi+8]
	lea rdi,[rdi+6]
	dec r8
	jnz crgb48lop2
	add rdi,r9 ;srcRGBBpl
	add rsi,r11 ;destRGBBpl
	dec rcx
	jnz crgb48lop
	
	align 16
crgbexit:
	pop rbx
	ret
