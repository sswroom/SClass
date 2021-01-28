section .text

global CSY416_LRGBC_convert

;void CSY416_LRGBC_convert(UInt8 *yPtr, UInt8 *dest, OSInt width, OSInt height, OSInt dbpl, OSInt yBpl, Int64 *yuv2rgb, Int64 *rgbGammaCorr);
; 0 rdi
; 8 rsi
; 16 rbx
; 24 rbp
; 32 retAddr
; rcx yPtr
; rdx dest
; r8 width
; r9 height
; 72 dbpl
; 80 yBpl
; 88 yuv2rgb
; 96 rgbGammaCorr

	align 16
CSY416_LRGBC_convert:
	push rbp
	push rbx
	push rsi
	push rdi
	mov r10,qword [rsp+72] ;dbpl
	mov r11,qword [rsp+80] ;yBpl
	lea rax,[r8 * 8]
	sub r10,rax ;dbpl
	sub r11,rax ;yBpl
	mov rsi,qword [rsp+88] ;yuv2rgb
	mov rdi,qword [rsp+96] ;rgbGammaCorr
	shr r8,1
	align 16
convlop:
	mov rbp,r8
	align 16
convlop2:
	movdqu xmm0,[rcx]
	pextrw rax,xmm0,1 ;y
	movq xmm1,[rsi+rax*8]
	pextrw rbx,xmm0,5 ;y
	movq xmm2,[rsi+rbx*8]
	punpcklqdq xmm1,xmm2
	pextrw rax,xmm0,0 ;u
	movq xmm2,[rsi+rax*8+524288]
	pextrw rbx,xmm0,4 ;u
	movq xmm3,[rsi+rbx*8+524288]
	punpcklqdq xmm2,xmm3
	paddsw xmm1,xmm2
	pextrw rax,xmm0,2 ;v
	movq xmm2,[rsi+rax*8+1048576]
	pextrw rbx,xmm0,6 ;v
	movq xmm3,[rsi+rbx*8+1048576]
	punpcklqdq xmm2,xmm3
	paddsw xmm1,xmm2
	
	pextrw rax,xmm1,2 ;r
	movq xmm0,[rdi+rax*8]
	pextrw rbx,xmm1,6 ;r
	movq xmm2,[rdi+rbx*8]
	punpcklqdq xmm0,xmm2
	pextrw rax,xmm1,1 ;g
	movq xmm2,[rdi+rax*8+524288]
	pextrw rbx,xmm1,5 ;g
	movq xmm3,[rdi+rbx*8+524288]
	punpcklqdq xmm2,xmm3
	paddsw xmm0,xmm2
	pextrw rax,xmm1,0 ;b
	movq xmm2,[rdi+rax*8+1048576]
	pextrw rbx,xmm1,4 ;b
	movq xmm3,[rdi+rbx*8+1048576]
	punpcklqdq xmm2,xmm3
	paddsw xmm0,xmm2
	movntdq [rdx],xmm0
	
	lea rcx,[rcx+16]
	lea rdx,[rdx+16]
	dec rbp
	jnz convlop2
	
	add rcx,r11 ;yBpl
	add rdx,r10 ;dbpl
	dec r9
	jnz convlop
	
	pop rdi
	pop rsi
	pop rbx
	pop rbp
	ret
