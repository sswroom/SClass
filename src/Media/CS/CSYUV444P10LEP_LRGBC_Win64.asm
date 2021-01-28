section .text

global CSYUV444P10LEP_LRGBC_convert

;void CSYUV444P10LEP_LRGBC_convert(UInt8 *yPtr, UInt8 *uPtr, UInt8 *vPtr, UInt8 *dest, OSInt width, OSInt height, OSInt dbpl, OSInt yBpl, Int64 *yuv2rgb, Int64 *rgbGammaCorr);
; 0 rdi
; 8 rsi
; 16 rbx
; 24 rbp
; 32 retAddr
; rcx yPtr
; rdx uPtr
; r8 vPtr
; r9 dest
; 72 width
; 80 height
; 88 dbpl
; 96 yBpl
; 104 yuv2rgb
; 112 rgbGammaCorr

	align 16
CSYUV444P10LEP_LRGBC_convert:
	push rbp
	push rbx
	push rsi
	push rdi
	mov r10,qword [rsp+72] ;width
	mov r11,qword [rsp+80] ;height
	lea rax,[r10 * 8]
	lea rbx,[r10 * 2]
	sub qword [rsp+88],rax ;dbpl
	sub qword [rsp+96],rbx ;yBpl
	mov rsi,qword [rsp+104] ;yuv2rgb
	mov rdi,qword [rsp+112] ;rgbGammaCorr
	shr r10,3
	align 16
convlop:
	mov rbp,r10
	align 16
convlop2:
	movups xmm0,[rcx]
	movups xmm4,[rdx]
	movups xmm5,[r8]

	pextrw rax,xmm0,0 ;y
	movq xmm1,[rsi+rax*8]
	pextrw rax,xmm0,1 ;y
	movhpd xmm1,[rsi+rax*8]
	pextrw rax,xmm4,0 ;u
	movq xmm2,[rsi+rax*8+524288]
	pextrw rax,xmm4,1 ;u
	movhpd xmm2,[rsi+rax*8+524288]
	paddsw xmm1,xmm2
	pextrw rax,xmm5,0 ;v
	movq xmm2,[rsi+rax*8+1048576]
	pextrw rax,xmm5,1 ;v
	movhpd xmm2,[rsi+rax*8+1048576]
	paddsw xmm1,xmm2
	
	pextrw rax,xmm1,2 ;r
	movq xmm3,[rdi+rax*8]
	pextrw rax,xmm1,6 ;r
	movhpd xmm3,[rdi+rax*8]
	pextrw rax,xmm1,1 ;g
	movq xmm2,[rdi+rax*8+524288]
	pextrw rax,xmm1,5 ;g
	movhpd xmm2,[rdi+rax*8+524288]
	paddsw xmm3,xmm2
	pextrw rax,xmm1,0 ;b
	movq xmm2,[rdi+rax*8+1048576]
	pextrw rax,xmm1,4 ;b
	movhpd xmm2,[rdi+rax*8+1048576]
	paddsw xmm3,xmm2
	movntdq [r9],xmm3
	
	pextrw rax,xmm0,2 ;y
	movq xmm1,[rsi+rax*8]
	pextrw rax,xmm0,3 ;y
	movhpd xmm1,[rsi+rax*8]
	pextrw rax,xmm4,2 ;u
	movq xmm2,[rsi+rax*8+524288]
	pextrw rax,xmm4,3 ;u
	movhpd xmm2,[rsi+rax*8+524288]
	paddsw xmm1,xmm2
	pextrw rax,xmm5,2 ;v
	movq xmm2,[rsi+rax*8+1048576]
	pextrw rax,xmm5,3 ;v
	movhpd xmm2,[rsi+rax*8+1048576]
	paddsw xmm1,xmm2
	
	pextrw rax,xmm1,2 ;r
	movq xmm3,[rdi+rax*8]
	pextrw rax,xmm1,6 ;r
	movhpd xmm3,[rdi+rax*8]
	pextrw rax,xmm1,1 ;g
	movq xmm2,[rdi+rax*8+524288]
	pextrw rax,xmm1,5 ;g
	movhpd xmm2,[rdi+rax*8+524288]
	paddsw xmm3,xmm2
	pextrw rax,xmm1,0 ;b
	movq xmm2,[rdi+rax*8+1048576]
	pextrw rax,xmm1,4 ;b
	movhpd xmm2,[rdi+rax*8+1048576]
	paddsw xmm3,xmm2
	movntdq [r9+16],xmm3
	
	pextrw rax,xmm0,4 ;y
	movq xmm1,[rsi+rax*8]
	pextrw rax,xmm0,5 ;y
	movhpd xmm1,[rsi+rax*8]
	pextrw rax,xmm4,4 ;u
	movq xmm2,[rsi+rax*8+524288]
	pextrw rax,xmm4,5 ;u
	movhpd xmm2,[rsi+rax*8+524288]
	paddsw xmm1,xmm2
	pextrw rax,xmm5,4 ;v
	movq xmm2,[rsi+rax*8+1048576]
	pextrw rax,xmm5,5 ;v
	movhpd xmm2,[rsi+rax*8+1048576]
	paddsw xmm1,xmm2
	
	pextrw rax,xmm1,2 ;r
	movq xmm3,[rdi+rax*8]
	pextrw rax,xmm1,6 ;r
	movhpd xmm3,[rdi+rax*8]
	pextrw rax,xmm1,1 ;g
	movq xmm2,[rdi+rax*8+524288]
	pextrw rax,xmm1,5 ;g
	movhpd xmm2,[rdi+rax*8+524288]
	paddsw xmm3,xmm2
	pextrw rax,xmm1,0 ;b
	movq xmm2,[rdi+rax*8+1048576]
	pextrw rax,xmm1,4 ;b
	movhpd xmm2,[rdi+rax*8+1048576]
	paddsw xmm3,xmm2
	movntdq [r9+32],xmm3
	
	pextrw rax,xmm0,6 ;y
	movq xmm1,[rsi+rax*8]
	pextrw rax,xmm0,7 ;y
	movhpd xmm1,[rsi+rax*8]
	pextrw rax,xmm4,6 ;u
	movq xmm2,[rsi+rax*8+524288]
	pextrw rax,xmm4,7 ;u
	movhpd xmm2,[rsi+rax*8+524288]
	paddsw xmm1,xmm2
	pextrw rax,xmm5,6 ;v
	movq xmm2,[rsi+rax*8+1048576]
	pextrw rax,xmm5,7 ;v
	movhpd xmm2,[rsi+rax*8+1048576]
	paddsw xmm1,xmm2
	
	pextrw rax,xmm1,2 ;r
	movq xmm3,[rdi+rax*8]
	pextrw rax,xmm1,6 ;r
	movhpd xmm3,[rdi+rax*8]
	pextrw rax,xmm1,1 ;g
	movq xmm2,[rdi+rax*8+524288]
	pextrw rax,xmm1,5 ;g
	movhpd xmm2,[rdi+rax*8+524288]
	paddsw xmm3,xmm2
	pextrw rax,xmm1,0 ;b
	movq xmm2,[rdi+rax*8+1048576]
	pextrw rax,xmm1,4 ;b
	movhpd xmm2,[rdi+rax*8+1048576]
	paddsw xmm3,xmm2
	movntdq [r9+48],xmm3
	
	lea rcx,[rcx+16]
	lea rdx,[rdx+16]
	lea r8,[r8+16]
	lea r9,[r9+64]
	dec rbp
	jnz convlop2
	
	mov rax,qword [rsp+96] ;yBpl
	add r9,qword [rsp+88] ;dbpl
	add rcx,rax
	add rdx,rax
	add r8,rax
	dec r11
	jnz convlop
	
	pop rdi
	pop rsi
	pop rbx
	pop rbp
	ret
