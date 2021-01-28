section .text

extern UseAVX

global CSAYUV444_10_RGB32C_convert

;void CSAYUV444_10_RGB32C_convert(UInt8 *yPtr, UInt8 *dest, OSInt width, OSInt height, OSInt dbpl, OSInt yBpl, Int64 *yuv2rgb, Int64 *rgbGammaCorr);
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
CSAYUV444_10_RGB32C_convert:
	cmp dword [rel UseAVX],0
	jnz CSAYUV444_10_RGB32C_convert_AVX
	push rbp
	push rbx
	push rsi
	push rdi
	mov r10,qword [rsp+72] ;dbpl
	mov r11,qword [rsp+80] ;yBpl
	lea rax,[r8 * 8]
	lea rbx,[r8 * 4]
	sub r10,rbx ;dbpl
	sub r11,rax ;yBpl
	mov rsi,qword [rsp+88] ;yuv2rgb
	mov rdi,qword [rsp+96] ;rgbGammaCorr
	shr r8,1
	align 16
convlop:
	mov rbp,r8
	align 16
convlop2:
	movups xmm0,[rcx]
	pextrw rax,xmm0,1 ;y
	movq xmm1,[rsi+rax*8]
	pextrw rbx,xmm0,5 ;y
	movhpd xmm1,[rsi+rbx*8]
	pextrw rax,xmm0,0 ;u
	movq xmm2,[rsi+rax*8+524288]
	pextrw rbx,xmm0,4 ;u
	movhpd xmm2,[rsi+rbx*8+524288]
	paddsw xmm1,xmm2
	pextrw rax,xmm0,2 ;v
	movq xmm2,[rsi+rax*8+1048576]
	pextrw rbx,xmm0,6 ;v
	movhpd xmm2,[rsi+rbx*8+1048576]
	paddsw xmm1,xmm2
	
	pextrw rax,xmm1,2 ;r
	movq xmm0,[rdi+rax*8]
	pextrw rbx,xmm1,6 ;r
	movhpd xmm0,[rdi+rbx*8]
	pextrw rax,xmm1,1 ;g
	movq xmm2,[rdi+rax*8+524288]
	pextrw rbx,xmm1,5 ;g
	movhpd xmm2,[rdi+rbx*8+524288]
	paddsw xmm0,xmm2
	pextrw rax,xmm1,0 ;b
	movq xmm2,[rdi+rax*8+1048576]
	pextrw rbx,xmm1,4 ;b
	movhpd xmm2,[rdi+rbx*8+1048576]
	paddsw xmm0,xmm2
	
	pextrw rax,xmm0,0
	mov ebx,dword [rdi+rax*4+1572864]
	pextrw rax,xmm0,1
	or ebx,dword [rdi+rax*4+1835008]
	pextrw rax,xmm0,2
	or ebx,dword [rdi+rax*4+2097152]
	movnti dword [rdx],ebx

	pextrw rax,xmm0,4
	mov ebx,dword [rdi+rax*4+1572864]
	pextrw rax,xmm0,5
	or ebx,dword [rdi+rax*4+1835008]
	pextrw rax,xmm0,6
	or ebx,dword [rdi+rax*4+2097152]
	movnti dword [rdx+4],ebx
	
	lea rcx,[rcx+16]
	lea rdx,[rdx+8]
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

	align 16
CSAYUV444_10_RGB32C_convert_AVX:
	push rbp
	push rbx
	push rsi
	push rdi
	mov r10,qword [rsp+72] ;dbpl
	mov r11,qword [rsp+80] ;yBpl
	lea rax,[r8 * 8]
	lea rbx,[r8 * 4]
	sub r10,rbx ;dbpl
	sub r11,rax ;yBpl
	mov rsi,qword [rsp+88] ;yuv2rgb
	mov rdi,qword [rsp+96] ;rgbGammaCorr
	shr r8,2
	align 16
convavxlop:
	mov rbp,r8
	align 16
convavxlop2:
	movntdqa xmm0,[rcx]
	movntdqa xmm4,[rcx+16]
	pextrw rax,xmm0,1 ;y
	movsd xmm1,[rsi+rax*8]
	pextrw rax,xmm0,5 ;y
	movhpd xmm1,[rsi+rax*8]
	pextrw rax,xmm4,1 ;y
	movsd xmm5,[rsi+rax*8]
	pextrw rax,xmm4,5 ;y
	movhpd xmm5,[rsi+rax*8]
	pextrw rax,xmm0,0 ;u
	movsd xmm2,[rsi+rax*8+524288]
	pextrw rax,xmm0,4 ;u
	movhpd xmm2,[rsi+rax*8+524288]
	pextrw rax,xmm4,0 ;u
	paddsw xmm1,xmm2
	movsd xmm2,[rsi+rax*8+524288]
	pextrw rax,xmm4,4 ;u
	movhpd xmm2,[rsi+rax*8+524288]
	pextrw rax,xmm0,2 ;v
	paddsw xmm5,xmm2
	movsd xmm2,[rsi+rax*8+1048576]
	pextrw rax,xmm0,6 ;v
	movhpd xmm2,[rsi+rax*8+1048576]
	pextrw rax,xmm4,2 ;v
	paddsw xmm1,xmm2
	movsd xmm2,[rsi+rax*8+1048576]
	pextrw rax,xmm4,6 ;v
	movhpd xmm2,[rsi+rax*8+1048576]
	
	pextrw rax,xmm1,2 ;r
	paddsw xmm5,xmm2
	movsd xmm0,[rdi+rax*8]
	pextrw rax,xmm1,6 ;r
	movhpd xmm0,[rdi+rax*8]
	pextrw rax,xmm5,2 ;r
	movsd xmm4,[rdi+rax*8]
	pextrw rax,xmm5,6 ;r
	movhpd xmm4,[rdi+rax*8]
	pextrw rax,xmm1,1 ;g
	movsd xmm2,[rdi+rax*8+524288]
	pextrw rax,xmm1,5 ;g
	movhpd xmm2,[rdi+rax*8+524288]
	pextrw rax,xmm5,1 ;g
	paddsw xmm0,xmm2
	movsd xmm2,[rdi+rax*8+524288]
	pextrw rax,xmm5,5 ;g
	movhpd xmm2,[rdi+rax*8+524288]
	pextrw rax,xmm1,0 ;b
	paddsw xmm4,xmm2
	movsd xmm2,[rdi+rax*8+1048576]
	pextrw rax,xmm1,4 ;b
	movhpd xmm2,[rdi+rax*8+1048576]
	pextrw rax,xmm5,0 ;b
	paddsw xmm0,xmm2
	movsd xmm2,[rdi+rax*8+1048576]
	pextrw rax,xmm5,4 ;b
	movhpd xmm2,[rdi+rax*8+1048576]
	paddsw xmm4,xmm2
	
	pextrw rax,xmm0,0
	movd xmm1,dword [rdi+rax*4+1572864]
	pextrw rax,xmm0,4
	pinsrd xmm1,dword [rdi+rax*4+1572864],1
	pextrw rax,xmm4,0
	pinsrd xmm1,dword [rdi+rax*4+1572864],2
	pextrw rax,xmm4,4
	pinsrd xmm1,dword [rdi+rax*4+1572864],3
	pextrw rax,xmm0,1
	movd xmm2,dword [rdi+rax*4+1835008]
	pextrw rax,xmm0,5
	pinsrd xmm2,dword [rdi+rax*4+1835008],1
	pextrw rax,xmm4,1
	pinsrd xmm2,dword [rdi+rax*4+1835008],2
	pextrw rax,xmm4,5
	pinsrd xmm2,dword [rdi+rax*4+1835008],3
	por xmm1,xmm2
	pextrw rax,xmm0,2
	movd xmm2,dword [rdi+rax*4+2097152]
	pextrw rax,xmm0,6
	pinsrd xmm2,dword [rdi+rax*4+2097152],1
	pextrw rax,xmm4,2
	pinsrd xmm2,dword [rdi+rax*4+2097152],2
	pextrw rax,xmm4,6
	pinsrd xmm2,dword [rdi+rax*4+2097152],3
	por xmm1,xmm2
	movntps [rdx],xmm1
	
	lea rcx,[rcx+32]
	lea rdx,[rdx+16]
	dec rbp
	jnz convavxlop2
	
	add rcx,r11 ;yBpl
	add rdx,r10 ;dbpl
	dec r9
	jnz convavxlop
	
	pop rdi
	pop rsi
	pop rbx
	pop rbp
	ret
