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

global CSRGBF_LRGBC_Convert
global _CSRGBF_LRGBC_Convert

;void CSRGBF_LRGBC_Convert(UInt8 *srcPtr, UInt8 *destPtr, IntOS width, IntOS height, IntOS srcNBits, IntOS srcRGBBpl, IntOS destRGBBpl, UInt8 *rgbTable);
;0 rbx
;8 retAddr
;rdi srcPtr
;rsi destPtr
;rdx width
;rcx height
;r8 srcNBits
;r9 srcRGBBpl
;16 destRGBBpl
;24 rgbTable
	align 16
CSRGBF_LRGBC_Convert:
_CSRGBF_LRGBC_Convert:
	push rbx
	mov rbx,qword [rsp+24] ;rgbTable
	cmp r8,128
	jz cargbf32start
	cmp r8,96
	jz crgbf32start
	cmp r8,64
	jz cawf32start
	cmp r8,32
	jz cwf32start
	jmp crgbexit
	
	align 16
cargbf32start:
	mov r8,qword [rsp+16] ;destRGBBpl
	lea r10,[rdx*8]
	lea rax,[r10*2]
	sub r9,rax ;srcRGBBpl
	sub r8,r10 ;destRGBBpl
	mov rax,32767
	cvtsi2ss xmm4,eax
	unpcklps xmm4,xmm4
	unpcklps xmm4,xmm4
	pxor xmm3,xmm3
	
	align 16
cargbf32lop:
	mov r10,rdx ;width
	align 16
cargbf32lop2:
	movaps xmm0,[rdi]
	mulps xmm0,xmm4
	cvtps2dq xmm2,xmm0
	packssdw xmm2,xmm3
	
	pextrw rax,xmm2,0
	movq xmm1,[rbx+rax*8+1048576]
	pextrw rax,xmm2,1
	movq xmm0,[rbx+rax*8+524288]
	paddsw xmm1,xmm0
	pextrw rax,xmm2,2
	movq xmm0, [rbx+rax*8]
	paddsw xmm1,xmm0
	movq [rsi],xmm1
	lea rdi,[rdi+16]
	lea rsi,[rsi+8]
	dec r10
	jnz cargbf32lop2
	add rdi,r9 ;srcRGBBpl
	add rsi,r8 ;destRGBBpl
	dec rcx
	jnz cargbf32lop
	jmp crgbexit

	align 16
crgbf32start:
	mov r8,qword [rsp+16] ;destRGBBpl
	lea r10,[rdx*4]
	lea rax,[r10*2+r10]
	lea r10,[r10*2]
	sub r9,rax ;srcRGBBpl
	sub r8,r10 ;destRGBBpl
	mov rax,32767
	cvtsi2ss xmm4,eax
	unpcklps xmm4,xmm4
	unpcklps xmm4,xmm4
	pxor xmm3,xmm3
	
	align 16
crgbf32lop:
	mov r10,rdx ;width
	align 16
crgbf32lop2:
	movq xmm0,[rdi]
	movss xmm5,[rdi+8]
	punpcklqdq xmm0,xmm5
	mulps xmm0,xmm4
	cvtps2dq xmm2,xmm0
	packssdw xmm2,xmm3
	
	pextrw rax,xmm2,0
	movq xmm1,[rbx+rax*8+1048576]
	pextrw rax,xmm2,1
	movq xmm0,[rbx+rax*8+524288]
	paddsw xmm1,xmm0
	pextrw rax,xmm2,2
	movq xmm0, [rbx+rax*8]
	paddsw xmm1,xmm0
	movq [rsi],xmm1
	lea rdi,[rdi+12]
	lea rsi,[rsi+8]
	dec r10
	jnz crgbf32lop2
	add rdi,r9 ;srcRGBBpl
	add rsi,r8 ;destRGBBpl
	dec rcx
	jnz crgbf32lop
	jmp crgbexit

	align 16
cawf32start:
	mov r8,qword [rsp+16] ;destRGBBpl
	lea rax,[rdx*8]
	sub r9,rax ;srcRGBBpl
	sub r8,rax ;destRGBBpl
	mov rax,32767
	cvtsi2ss xmm4,eax
	unpcklps xmm4,xmm4
	unpcklps xmm4,xmm4
	pxor xmm3,xmm3
	
	align 16
cawf32lop:
	mov r10,rdx ;width
	align 16
cawf32lop2:
	movq xmm0,[rdi]
	mulps xmm0,xmm4
	cvtps2dq xmm2,xmm0
	packssdw xmm2,xmm3
	
	pextrw rax,xmm2,0
	movq xmm1,[rbx+rax*8+1048576]
	movq xmm0,[rbx+rax*8+524288]
	paddsw xmm1,xmm0
	movq xmm0, [rbx+rax*8]
	paddsw xmm1,xmm0
	movq [rsi],xmm1
	lea rdi,[rdi+8]
	lea rsi,[rsi+8]
	dec r10
	jnz cawf32lop2
	add rdi,r9 ;srcRGBBpl
	add rsi,r8 ;destRGBBpl
	dec rcx
	jnz cawf32lop
	jmp crgbexit

	align 16
cwf32start:
	mov r8,qword [rsp+16] ;destRGBBpl
	lea r10,[rdx*8]
	lea rax,[rdx*4]
	sub r9,rax ;srcRGBBpl
	sub r8,r10 ;destRGBBpl
	mov rax,32767
	cvtsi2ss xmm4,eax
	unpcklps xmm4,xmm4
	unpcklps xmm4,xmm4
	pxor xmm3,xmm3
	
	align 16
cwf32lop:
	mov r10,rdx ;width
	align 16
cwf32lop2:
	movss xmm0,[rdi]
	mulps xmm0,xmm4
	cvtps2dq xmm2,xmm0
	packssdw xmm2,xmm3
	
	pextrw rax,xmm2,0
	movq xmm1,[rbx+rax*8+1048576]
	movq xmm0,[rbx+rax*8+524288]
	paddsw xmm1,xmm0
	movq xmm0, [rbx+rax*8]
	paddsw xmm1,xmm0
	movq [rsi],xmm1
	lea rdi,[rdi+4]
	lea rsi,[rsi+8]
	dec r10
	jnz cwf32lop2
	add rdi,r9 ;srcRGBBpl
	add rsi,r8 ;destRGBBpl
	dec rcx
	jnz cwf32lop
	jmp crgbexit

	align 16
crgbexit:
	pop rbx
	ret

