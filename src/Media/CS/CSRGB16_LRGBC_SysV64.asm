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

global CSRGB16_LRGBC_ConvertB16G16R16A16
global _CSRGB16_LRGBC_ConvertB16G16R16A16
global CSRGB16_LRGBC_ConvertR16G16B16A16
global _CSRGB16_LRGBC_ConvertR16G16B16A16
global CSRGB16_LRGBC_ConvertB16G16R16
global _CSRGB16_LRGBC_ConvertB16G16R16
global CSRGB16_LRGBC_ConvertR16G16B16
global _CSRGB16_LRGBC_ConvertR16G16B16
global CSRGB16_LRGBC_ConvertW16A16
global _CSRGB16_LRGBC_ConvertW16A16
global CSRGB16_LRGBC_ConvertW16
global _CSRGB16_LRGBC_ConvertW16
global CSRGB16_LRGBC_ConvertA2B10G10R10
global _CSRGB16_LRGBC_ConvertA2B10G10R10

;void CSRGB16_LRGBC_ConvertB16G16R16A16(UInt8 *srcPtr, UInt8 *destPtr, OSInt width, OSInt height, OSInt srcRGBBpl, OSInt destRGBBpl, UInt8 *rgbTable);
;0 rbx
;8 retAddr
;rdi srcPtr
;rsi destPtr
;rdx width
;rcx height
;r8 srcRGBBpl
;r9 destRGBBpl
;16 rgbTable
	align 16
CSRGB16_LRGBC_ConvertB16G16R16A16:
_CSRGB16_LRGBC_ConvertB16G16R16A16:
	push rbx
	mov rbx,qword [rsp+16] ;rgbTable
	lea rax,[rdx*8] ;width
	sub r8,rax ;srcRGBBpl
	sub r9,rax ;destRGBBpl
	
	align 16
cbgra16lop:
	mov r10,rdx ;width
	align 16
cbgra16lop2:
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
	dec r10
	jnz cbgra16lop2
	add rdi,r8 ;srcRGBBpl
	add rsi,r9 ;destRGBBpl
	dec rcx
	jnz cbgra16lop
	pop rbx
	ret

;void CSRGB16_LRGBC_ConvertR16G16B16A16(UInt8 *srcPtr, UInt8 *destPtr, OSInt width, OSInt height, OSInt srcRGBBpl, OSInt destRGBBpl, UInt8 *rgbTable);
;0 rbx
;8 retAddr
;rdi srcPtr
;rsi destPtr
;rdx width
;rcx height
;r8 srcRGBBpl
;r9 destRGBBpl
;16 rgbTable
	align 16
CSRGB16_LRGBC_ConvertR16G16B16A16:
_CSRGB16_LRGBC_ConvertR16G16B16A16:
	push rbx
	mov rbx,qword [rsp+16] ;rgbTable
	lea rax,[rdx*8] ;width
	sub r8,rax ;srcRGBBpl
	sub r9,rax ;destRGBBpl
	
	align 16
crgba16lop:
	mov r10,rdx ;width
	align 16
crgba16lop2:
	movzx rax,word [rdi+4]
	movq xmm1,[rbx+rax*8+1048576]
	movzx rax,word [rdi+2]
	movq xmm0,[rbx+rax*8+524288]
	paddsw xmm1,xmm0
	movzx rax,word [rdi]
	movq xmm0,[rbx+rax*8]
	paddsw xmm1,xmm0
	movq [rsi],xmm1
	lea rdi,[rdi+8]
	lea rsi,[rsi+8]
	dec r10
	jnz crgba16lop2
	add rdi,r8 ;srcRGBBpl
	add rsi,r9 ;destRGBBpl
	dec rcx
	jnz crgba16lop
	pop rbx
	ret

;void CSRGB16_LRGBC_ConvertB16G16R16(UInt8 *srcPtr, UInt8 *destPtr, OSInt width, OSInt height, OSInt srcRGBBpl, OSInt destRGBBpl, UInt8 *rgbTable);
;0 rbx
;8 retAddr
;rdi srcPtr
;rsi destPtr
;rdx width
;rcx height
;r8 srcRGBBpl
;r9 destRGBBpl
;16 rgbTable
	align 16
CSRGB16_LRGBC_ConvertB16G16R16:
_CSRGB16_LRGBC_ConvertB16G16R16:
	push rbx
	mov rbx,qword [rsp+16] ;rgbTable
	lea rax,[rdx+rdx*2]
	lea r10,[rdx*8]
	lea rax,[rax*2]
	sub r8,rax ;srcRGBBpl
	sub r9,r10 ;destRGBBpl
	align 16
cbgr16lop:
	mov r10,rdx ;width
	align 16
cbgr16lop2:
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
	dec r10
	jnz cbgr16lop2
	add rdi,r8 ;srcRGBBpl
	add rsi,r9 ;destRGBBpl
	dec rcx
	jnz cbgr16lop
	pop rbx
	ret

;void CSRGB16_LRGBC_ConvertR16G16B16(UInt8 *srcPtr, UInt8 *destPtr, OSInt width, OSInt height, OSInt srcRGBBpl, OSInt destRGBBpl, UInt8 *rgbTable);
;0 rbx
;8 retAddr
;rdi srcPtr
;rsi destPtr
;rdx width
;rcx height
;r8 srcRGBBpl
;r9 destRGBBpl
;16 rgbTable
	align 16
CSRGB16_LRGBC_ConvertR16G16B16:
_CSRGB16_LRGBC_ConvertR16G16B16:
	push rbx
	mov rbx,qword [rsp+16] ;rgbTable
	lea rax,[rdx+rdx*2]
	lea r10,[rdx*8]
	lea rax,[rax*2]
	sub r8,rax ;srcRGBBpl
	sub r9,r10 ;destRGBBpl
	align 16
crgb16lop:
	mov r10,rdx ;width
	align 16
crgb16lop2:
	movzx rax,word [rdi+4]
	movq xmm1,[rbx+rax*8+1048576]
	movzx rax,word [rdi+2]
	movq xmm0,[rbx+rax*8+524288]
	paddsw xmm1,xmm0
	movzx rax,word [rdi]
	movq xmm0,[rbx+rax*8]
	paddsw xmm1,xmm0
	movq [rsi],xmm1
	lea rsi,[rsi+8]
	lea rdi,[rdi+6]
	dec r10
	jnz crgb16lop2
	add rdi,r8 ;srcRGBBpl
	add rsi,r9 ;destRGBBpl
	dec rcx
	jnz crgb16lop
	pop rbx
	ret

;void CSRGB16_LRGBC_ConvertW16A16(UInt8 *srcPtr, UInt8 *destPtr, OSInt width, OSInt height, OSInt srcRGBBpl, OSInt destRGBBpl, UInt8 *rgbTable);
;0 rbx
;8 retAddr
;rdi srcPtr
;rsi destPtr
;rdx width
;rcx height
;r8 srcRGBBpl
;r9 destRGBBpl
;16 rgbTable
	align 16
CSRGB16_LRGBC_ConvertW16A16:
_CSRGB16_LRGBC_ConvertW16A16:
	push rbx
	mov rbx,qword [rsp+16] ;rgbTable
	lea rax,[rdx*4]
	lea r10,[rdx*8]
	sub r8,rax ;srcRGBBpl
	sub r9,r10 ;destRGBBpl
	
	align 16
cwa16lop:
	mov r10,rdx ;width
	align 16
cwa16lop2:
	movzx rax,word [rdi]
	movq xmm1,[rbx+rax*8+1048576]
	movq xmm0,[rbx+rax*8+524288]
	paddsw xmm1,xmm0
	movq xmm0, [rbx+rax*8]
	paddsw xmm1,xmm0
	movq [rsi],xmm1
	lea rdi,[rdi+4]
	lea rsi,[rsi+8]
	dec r10
	jnz cwa16lop2
	add rdi,r8 ;srcRGBBpl
	add rsi,r9 ;destRGBBpl
	dec rcx
	jnz cwa16lop
	pop rbx
	ret
	
;void CSRGB16_LRGBC_ConvertW16(UInt8 *srcPtr, UInt8 *destPtr, OSInt width, OSInt height, OSInt srcRGBBpl, OSInt destRGBBpl, UInt8 *rgbTable);
;0 rbx
;8 retAddr
;rdi srcPtr
;rsi destPtr
;rdx width
;rcx height
;r8 srcRGBBpl
;r9 destRGBBpl
;16 rgbTable
	align 16
CSRGB16_LRGBC_ConvertW16:
_CSRGB16_LRGBC_ConvertW16:
	push rbx
	mov rbx,qword [rsp+16] ;rgbTable
	lea rax,[rdx*2]
	lea r10,[rdx*8]
	sub r8,rax ;srcRGBBpl
	sub r9,r10 ;destRGBBpl
	
	align 16
cw16lop:
	mov r8,rdx ;width
	align 16
cw16lop2:
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
	jnz cw16lop2
	add rdi,r8 ;srcRGBBpl
	add rsi,r9 ;destRGBBpl
	dec rcx
	jnz cw16lop
	pop rbx
	ret

;void CSRGB16_LRGBC_ConvertA2B10G10R10(UInt8 *srcPtr, UInt8 *destPtr, OSInt width, OSInt height, OSInt srcNBits, OSInt srcRGBBpl, OSInt destRGBBpl, UInt8 *rgbTable);
;0 rbx
;8 retAddr
;rdi srcPtr
;rsi destPtr
;rdx width
;rcx height
;r8 srcRGBBpl
;r9 destRGBBpl
;16 rgbTable
	align 16
CSRGB16_LRGBC_ConvertA2B10G10R10:
_CSRGB16_LRGBC_ConvertA2B10G10R10:
	push rbx
	mov rbx,qword [rsp+16] ;rgbTable
	lea rax,[rdx*4]
	lea r10,[rdx*8]
	sub r8,rax ;srcRGBBpl
	sub r9,r10 ;destRGBBpl
	mov r10,rcx
	
	align 16
ca2b10g10r10lop:
	mov r11,rdx ;width
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
	dec r11
	jnz ca2b10g10r10lop2
	add rdi,r8 ;srcRGBBpl
	add rsi,r9 ;destRGBBpl
	dec r10
	jnz ca2b10g10r10lop
	pop rbx
	ret
