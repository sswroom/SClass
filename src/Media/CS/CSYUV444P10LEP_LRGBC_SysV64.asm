section .text

global CSYUV444P10LEP_LRGBC_convert
global _CSYUV444P10LEP_LRGBC_convert

;void CSYUV444P10LEP_LRGBC_convert(UInt8 *yPtr, UInt8 *uPtr, UInt8 *vPtr, UInt8 *dest, OSInt width, OSInt height, OSInt dbpl, OSInt yBpl, Int64 *yuv2rgb, Int64 *rgbGammaCorr);
; 0 rbx
; 8 rbp
; 16 retAddr
; rdi yPtr
; rsi uPtr
; rdx vPtr
; rcx dest
; r8 width
; r9 height
; 24 dbpl
; 32 yBpl
; 40 yuv2rgb
; 48 rgbGammaCorr

	align 16
CSYUV444P10LEP_LRGBC_convert:
_CSYUV444P10LEP_LRGBC_convert:
	push rbp
	push rbx
	lea rax,[r8 * 8]
	sub qword [rsp+32],rax ;dbpl
	sub qword [rsp+40],rax ;yBpl
	mov rbx,qword [rsp+24] ;yuv2rgb
	mov rbp,qword [rsp+32] ;rgbGammaCorr
	shr rdx,3
	align 16
convlop:
	mov r10,r8
	align 16
convlop2:
	movdqu xmm0,[rdi] ;y
	movdqu xmm4,[rsi] ;u
	movdqu xmm5,[rdx] ;v

	pextrw rax,xmm0,0 ;y
	movq xmm1,[rbx+rax*8]
	pextrw rax,xmm0,1 ;y
	movhpd xmm1,[rbx+rax*8]
	pextrw rax,xmm4,0 ;u
	movq xmm2,[rbx+rax*8+524288]
	pextrw rax,xmm4,1 ;u
	movhpd xmm2,[rbx+rax*8+524288]
	paddsw xmm1,xmm2
	pextrw rax,xmm5,0 ;v
	movq xmm2,[rbx+rax*8+1048576]
	pextrw rax,xmm5,1 ;v
	movhpd xmm2,[rbx+rax*8+1048576]
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
	movntdq [rcx],xmm0
	
	pextrw rax,xmm0,2 ;y
	movq xmm1,[rbx+rax*8]
	pextrw rax,xmm0,3 ;y
	movhpd xmm1,[rbx+rax*8]
	pextrw rax,xmm4,2 ;u
	movq xmm2,[rbx+rax*8+524288]
	pextrw rax,xmm4,3 ;u
	movhpd xmm2,[rbx+rax*8+524288]
	paddsw xmm1,xmm2
	pextrw rax,xmm5,2 ;v
	movq xmm2,[rbx+rax*8+1048576]
	pextrw rax,xmm5,3 ;v
	movhpd xmm2,[rbx+rax*8+1048576]
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
	movntdq [rcx+16],xmm0
	
	pextrw rax,xmm0,4 ;y
	movq xmm1,[rbx+rax*8]
	pextrw rax,xmm0,5 ;y
	movhpd xmm1,[rbx+rax*8]
	pextrw rax,xmm4,4 ;u
	movq xmm2,[rbx+rax*8+524288]
	pextrw rax,xmm4,5 ;u
	movhpd xmm2,[rbx+rax*8+524288]
	paddsw xmm1,xmm2
	pextrw rax,xmm5,4 ;v
	movq xmm2,[rbx+rax*8+1048576]
	pextrw rax,xmm5,5 ;v
	movhpd xmm2,[rbx+rax*8+1048576]
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
	movntdq [rcx+32],xmm0
	
	pextrw rax,xmm0,6 ;y
	movq xmm1,[rbx+rax*8]
	pextrw rax,xmm0,7 ;y
	movhpd xmm1,[rbx+rax*8]
	pextrw rax,xmm4,6 ;u
	movq xmm2,[rbx+rax*8+524288]
	pextrw rax,xmm4,7 ;u
	movhpd xmm2,[rbx+rax*8+524288]
	paddsw xmm1,xmm2
	pextrw rax,xmm5,6 ;v
	movq xmm2,[rbx+rax*8+1048576]
	pextrw rax,xmm5,7 ;v
	movhpd xmm2,[rbx+rax*8+1048576]
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
	movntdq [rcx+48],xmm0
	
	lea rdi,[rdi+16]
	lea rsi,[rsi+16]
	lea rdx,[rdx+16]
	lea rcx,[rcx+64]
	dec r10
	jnz convlop2
	
	mov rax,qword [rsp+32] ;yBpl
	add rcx,r8 ;dbpl
	add rdi,rax
	add rsi,rax
	add rdx,rax
	dec r9
	jnz convlop
	
	pop rbx
	pop rbp
	ret

