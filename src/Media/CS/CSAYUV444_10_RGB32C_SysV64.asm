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

global CSAYUV444_10_RGB32C_convert
global _CSAYUV444_10_RGB32C_convert

;void CSAYUV444_10_RGB32C_convert(UInt8 *yPtr, UInt8 *dest, OSInt width, OSInt height, OSInt dbpl, OSInt yBpl, Int64 *yuv2rgb, Int64 *rgbGammaCorr);
; 0 rbx
; 8 rbp
; 16 retAddr
; rdi yPtr
; rsi dest
; rdx width
; rcx height
; r8 dbpl
; r9 yBpl
; 24 yuv2rgb
; 32 rgbGammaCorr

	align 16
CSAYUV444_10_RGB32C_convert:
_CSAYUV444_10_RGB32C_convert:
	push rbp
	push rbx
	lea rax,[rdx * 8]
	lea rbx,[rdx * 4]
	sub r8,rbx ;dbpl
	sub r9,rax ;yBpl
	mov rbx,qword [rsp+24] ;yuv2rgb
	mov rbp,qword [rsp+32] ;rgbGammaCorr
	shr rdx,1
	mov r11,rdx
	align 16
convlop:
	mov r10,r11
	align 16
convlop2:
	movdqu xmm0,[rdi]
	pextrw rax,xmm0,1 ;y
	movq xmm1,[rbx+rax*8]
	pextrw rax,xmm0,5 ;y
	movq xmm2,[rbx+rax*8]
	punpcklqdq xmm1,xmm2
	pextrw rax,xmm0,0 ;u
	movq xmm2,[rbx+rax*8+524288]
	pextrw rax,xmm0,4 ;u
	movq xmm3,[rbx+rax*8+524288]
	punpcklqdq xmm2,xmm3
	paddsw xmm1,xmm2
	pextrw rax,xmm0,2 ;v
	movq xmm2,[rbx+rax*8+1048576]
	pextrw rax,xmm0,6 ;v
	movq xmm3,[rbx+rax*8+1048576]
	punpcklqdq xmm2,xmm3
	paddsw xmm1,xmm2
	
	pextrw rax,xmm1,2 ;r
	movq xmm0,[rbp+rax*8]
	pextrw rax,xmm1,6 ;r
	movq xmm2,[rbp+rax*8]
	punpcklqdq xmm0,xmm2
	pextrw rax,xmm1,1 ;g
	movq xmm2,[rbp+rax*8+524288]
	pextrw rax,xmm1,5 ;g
	movq xmm3,[rbp+rax*8+524288]
	punpcklqdq xmm2,xmm3
	paddsw xmm0,xmm2
	pextrw rax,xmm1,0 ;b
	movq xmm2,[rbp+rax*8+1048576]
	pextrw rax,xmm1,4 ;b
	movq xmm3,[rbp+rax*8+1048576]
	punpcklqdq xmm2,xmm3
	paddsw xmm0,xmm2
	
	pextrw rax,xmm0,0
	mov edx,dword [rbp+rax*4+1572864]
	pextrw rax,xmm0,1
	or edx,dword [rbp+rax*4+1835008]
	pextrw rax,xmm0,2
	or edx,dword [rbp+rax*4+2097152]
	movnti dword [rsi],edx

	pextrw rax,xmm0,4
	mov edx,dword [rbp+rax*4+1572864]
	pextrw rax,xmm0,5
	or edx,dword [rbp+rax*4+1835008]
	pextrw rax,xmm0,6
	or edx,dword [rbp+rax*4+2097152]
	movnti dword [rsi+4],edx
	
	lea rdi,[rdi+16]
	lea rsi,[rsi+8]
	dec r10
	jnz convlop2
	
	add rdi,r9 ;yBpl
	add rsi,r8 ;dbpl
	dec rcx
	jnz convlop
	
	pop rbx
	pop rbp
	ret

