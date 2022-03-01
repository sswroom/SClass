section .text

global CSYUV444P10LEP_RGB32C_convert
global _CSYUV444P10LEP_RGB32C_convert

%define rex_w db 0x48
;void CSYUV444P10LEP_RGB32C_convert(UInt8 *yPtr, UInt8 *uPtr, UInt8 *vPtr, UInt8 *dest, OSInt width, OSInt height, OSInt dbpl, OSInt yBpl, Int64 *yuv2rgb, Int64 *rgbGammaCorr);
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
CSYUV444P10LEP_RGB32C_convert:
_CSYUV444P10LEP_RGB32C_convert:
	push rbp
	push rbx
	lea rax,[r8 * 4]
	lea rbx,[r8 * 2]
	sub qword [rsp+24],rax ;dbpl
	sub qword [rsp+32],rbx ;yBpl
	mov rbx,qword [rsp+40] ;yuv2rgb
	mov rbp,qword [rsp+48] ;rgbGammaCorr
	shr r8,3
	mov r11,r8
	align 16
convlop:
	mov r10,r11
	align 16
convlop2:
	movdqu xmm0,[rdi] ;y
	movdqu xmm4,[rsi] ;u
	movdqu xmm5,[rdx] ;v

	pextrw rax,xmm0,0 ;y
	movq xmm1,[rbx+rax*8]
	pextrw rax,xmm0,1 ;y
	movhps xmm1,[rbx+rax*8]
	pextrw rax,xmm4,0 ;u
	movq xmm2,[rbx+rax*8+524288]
	pextrw rax,xmm4,1 ;u
	movhps xmm2,[rbx+rax*8+524288]
	paddsw xmm1,xmm2
	pextrw rax,xmm5,0 ;v
	movq xmm2,[rbx+rax*8+1048576]
	pextrw rax,xmm5,1 ;v
	movhps xmm2,[rbx+rax*8+1048576]
	paddsw xmm1,xmm2
	
	pextrw rax,xmm1,2 ;r
	movq xmm3,[rbp+rax*8]
	pextrw rax,xmm1,6 ;r
	movhps xmm3,[rbp+rax*8]
	pextrw rax,xmm1,1 ;g
	movq xmm2,[rbp+rax*8+524288]
	pextrw rax,xmm1,5 ;g
	movhps xmm2,[rbp+rax*8+524288]
	paddsw xmm3,xmm2
	pextrw rax,xmm1,0 ;b
	movq xmm2,[rbp+rax*8+1048576]
	pextrw rax,xmm1,4 ;b
	movhps xmm2,[rbp+rax*8+1048576]
	paddsw xmm3,xmm2
	
	pextrw rax,xmm3,0
	mov r8d,dword [rbp+rax*4+1572864]
	pextrw rax,xmm3,1
	or r8d,dword [rbp+rax*4+1835008]
	pextrw rax,xmm3,2
	or r8d,dword [rbp+rax*4+2097152]
	pinsrd xmm6,r8d,0

	pextrw rax,xmm3,4
	mov r8d,dword [rbp+rax*4+1572864]
	pextrw rax,xmm3,5
	or r8d,dword [rbp+rax*4+1835008]
	pextrw rax,xmm3,6
	or r8d,dword [rbp+rax*4+2097152]
	pinsrd xmm6,r8d,1
	
	pextrw rax,xmm0,2 ;y
	movq xmm1,[rbx+rax*8]
	pextrw rax,xmm0,3 ;y
	movhps xmm1,[rbx+rax*8]
	pextrw rax,xmm4,2 ;u
	movq xmm2,[rbx+rax*8+524288]
	pextrw rax,xmm4,3 ;u
	movhps xmm2,[rbx+rax*8+524288]
	paddsw xmm1,xmm2
	pextrw rax,xmm5,2 ;v
	movq xmm2,[rbx+rax*8+1048576]
	pextrw rax,xmm5,3 ;v
	movhps xmm2,[rbx+rax*8+1048576]
	paddsw xmm1,xmm2
	
	pextrw rax,xmm1,2 ;r
	movq xmm3,[rbp+rax*8]
	pextrw rax,xmm1,6 ;r
	movhps xmm3,[rbp+rax*8]
	pextrw rax,xmm1,1 ;g
	movq xmm2,[rbp+rax*8+524288]
	pextrw rax,xmm1,5 ;g
	movhps xmm2,[rbp+rax*8+524288]
	paddsw xmm3,xmm2
	pextrw rax,xmm1,0 ;b
	movq xmm2,[rbp+rax*8+1048576]
	pextrw rax,xmm1,4 ;b
	movhps xmm2,[rbp+rax*8+1048576]
	paddsw xmm3,xmm2
	
	pextrw rax,xmm3,0
	mov r8d,dword [rbp+rax*4+1572864]
	pextrw rax,xmm3,1
	or r8d,dword [rbp+rax*4+1835008]
	pextrw rax,xmm3,2
	or r8d,dword [rbp+rax*4+2097152]
	pinsrd xmm6,r8d,2

	pextrw rax,xmm3,4
	mov r8d,dword [rbp+rax*4+1572864]
	pextrw rax,xmm3,5
	or r8d,dword [rbp+rax*4+1835008]
	pextrw rax,xmm3,6
	or r8d,dword [rbp+rax*4+2097152]
	pinsrd xmm6,r8d,3
	movntps [rcx],xmm6
	
	pextrw rax,xmm0,4 ;y
	movq xmm1,[rbx+rax*8]
	pextrw rax,xmm0,5 ;y
	movhps xmm1,[rbx+rax*8]
	pextrw rax,xmm4,4 ;u
	movq xmm2,[rbx+rax*8+524288]
	pextrw rax,xmm4,5 ;u
	movhps xmm2,[rbx+rax*8+524288]
	paddsw xmm1,xmm2
	pextrw rax,xmm5,4 ;v
	movq xmm2,[rbx+rax*8+1048576]
	pextrw rax,xmm5,5 ;v
	movhps xmm2,[rbx+rax*8+1048576]
	paddsw xmm1,xmm2
	
	pextrw rax,xmm1,2 ;r
	movq xmm3,[rbp+rax*8]
	pextrw rax,xmm1,6 ;r
	movhps xmm3,[rbp+rax*8]
	pextrw rax,xmm1,1 ;g
	movq xmm2,[rbp+rax*8+524288]
	pextrw rax,xmm1,5 ;g
	movhps xmm2,[rbp+rax*8+524288]
	paddsw xmm3,xmm2
	pextrw rax,xmm1,0 ;b
	movq xmm2,[rbp+rax*8+1048576]
	pextrw rax,xmm1,4 ;b
	movhps xmm2,[rbp+rax*8+1048576]
	paddsw xmm3,xmm2
	
	pextrw rax,xmm3,0
	mov r8d,dword [rbp+rax*4+1572864]
	pextrw rax,xmm3,1
	or r8d,dword [rbp+rax*4+1835008]
	pextrw rax,xmm3,2
	or r8d,dword [rbp+rax*4+2097152]
	pinsrd xmm6,r8d,0

	pextrw rax,xmm3,4
	mov r8d,dword [rbp+rax*4+1572864]
	pextrw rax,xmm3,5
	or r8d,dword [rbp+rax*4+1835008]
	pextrw rax,xmm3,6
	or r8d,dword [rbp+rax*4+2097152]
	pinsrd xmm6,r8d,1
	
	pextrw rax,xmm0,6 ;y
	movq xmm1,[rbx+rax*8]
	pextrw rax,xmm0,7 ;y
	movhps xmm1,[rbx+rax*8]
	pextrw rax,xmm4,6 ;u
	movq xmm2,[rbx+rax*8+524288]
	pextrw rax,xmm4,7 ;u
	movhps xmm2,[rbx+rax*8+524288]
	paddsw xmm1,xmm2
	pextrw rax,xmm5,6 ;v
	movq xmm2,[rbx+rax*8+1048576]
	pextrw rax,xmm5,7 ;v
	movhps xmm2,[rbx+rax*8+1048576]
	paddsw xmm1,xmm2
	
	pextrw rax,xmm1,2 ;r
	movq xmm3,[rbp+rax*8]
	pextrw rax,xmm1,6 ;r
	movhps xmm3,[rbp+rax*8]
	pextrw rax,xmm1,1 ;g
	movq xmm2,[rbp+rax*8+524288]
	pextrw rax,xmm1,5 ;g
	movhps xmm2,[rbp+rax*8+524288]
	paddsw xmm3,xmm2
	pextrw rax,xmm1,0 ;b
	movq xmm2,[rbp+rax*8+1048576]
	pextrw rax,xmm1,4 ;b
	movhps xmm2,[rbp+rax*8+1048576]
	paddsw xmm3,xmm2
	
	pextrw rax,xmm3,0
	mov r8d,dword [rbp+rax*4+1572864]
	pextrw rax,xmm3,1
	or r8d,dword [rbp+rax*4+1835008]
	pextrw rax,xmm3,2
	or r8d,dword [rbp+rax*4+2097152]
	pinsrd xmm6,r8d,2

	pextrw rax,xmm3,4
	mov r8d,dword [rbp+rax*4+1572864]
	pextrw rax,xmm3,5
	or r8d,dword [rbp+rax*4+1835008]
	pextrw rax,xmm3,6
	or r8d,dword [rbp+rax*4+2097152]
	pinsrd xmm6,r8d,3
	movntps [rcx+16],xmm6
	
	add rdi,16
	add rsi,16
	add rdx,16
	add rcx,32
	dec r10
	jnz convlop2
	
	mov rax,qword [rsp+32] ;yBpl
	add rcx,qword [rsp+24] ;dbpl
	add rdi,rax
	add rsi,rax
	add rdx,rax
	dec r9
	jnz convlop
	
	pop rbx
	pop rbp
	ret

