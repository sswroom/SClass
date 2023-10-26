section .text

%ifdef __CYGWIN__
extern _UseAVX
%define UseAVX _UseAVX
%else
extern UseAVX
%endif
global CSYUV444P10LEP_RGB32C_convert

;void CSYUV444P10LEP_RGB32C_convert(UInt8 *yPtr, UInt8 *dest, OSInt width, OSInt height, OSInt dbpl, OSInt yBpl, Int64 *yuv2rgb, Int64 *rgbGammaCorr);
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
CSYUV444P10LEP_RGB32C_convert:
	cmp dword [rel UseAVX],0
	jnz CSYUV444P10LEP_RGB32C_convert_AVX
	push rbp
	push rbx
	push rsi
	push rdi
	mov r10,qword [rsp+72] ;width
	mov r11,qword [rsp+80] ;height
	lea rax,[r10 * 4]
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
	
	pextrw rax,xmm3,0
	mov ebx,dword [rdi+rax*4+1572864]
	pextrw rax,xmm3,1
	or ebx,dword [rdi+rax*4+1835008]
	pextrw rax,xmm3,2
	or ebx,dword [rdi+rax*4+2097152]
	movnti dword [r9],ebx

	pextrw rax,xmm3,4
	mov ebx,dword [rdi+rax*4+1572864]
	pextrw rax,xmm3,5
	or ebx,dword [rdi+rax*4+1835008]
	pextrw rax,xmm3,6
	or ebx,dword [rdi+rax*4+2097152]
	movnti dword [r9+4],ebx
	
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
	
	pextrw rax,xmm3,0
	mov ebx,dword [rdi+rax*4+1572864]
	pextrw rax,xmm3,1
	or ebx,dword [rdi+rax*4+1835008]
	pextrw rax,xmm3,2
	or ebx,dword [rdi+rax*4+2097152]
	movnti dword [r9+8],ebx

	pextrw rax,xmm3,4
	mov ebx,dword [rdi+rax*4+1572864]
	pextrw rax,xmm3,5
	or ebx,dword [rdi+rax*4+1835008]
	pextrw rax,xmm3,6
	or ebx,dword [rdi+rax*4+2097152]
	movnti dword [r9+12],ebx

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
	
	pextrw rax,xmm3,0
	mov ebx,dword [rdi+rax*4+1572864]
	pextrw rax,xmm3,1
	or ebx,dword [rdi+rax*4+1835008]
	pextrw rax,xmm3,2
	or ebx,dword [rdi+rax*4+2097152]
	movnti dword [r9+16],ebx

	pextrw rax,xmm3,4
	mov ebx,dword [rdi+rax*4+1572864]
	pextrw rax,xmm3,5
	or ebx,dword [rdi+rax*4+1835008]
	pextrw rax,xmm3,6
	or ebx,dword [rdi+rax*4+2097152]
	movnti dword [r9+20],ebx

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
	
	pextrw rax,xmm3,0
	mov ebx,dword [rdi+rax*4+1572864]
	pextrw rax,xmm3,1
	or ebx,dword [rdi+rax*4+1835008]
	pextrw rax,xmm3,2
	or ebx,dword [rdi+rax*4+2097152]
	movnti dword [r9+24],ebx

	pextrw rax,xmm3,4
	mov ebx,dword [rdi+rax*4+1572864]
	pextrw rax,xmm3,5
	or ebx,dword [rdi+rax*4+1835008]
	pextrw rax,xmm3,6
	or ebx,dword [rdi+rax*4+2097152]
	movnti dword [r9+28],ebx

	lea rcx,[rcx+16]
	lea rdx,[rdx+16]
	lea r8,[r8+16]
	lea r9,[r9+32]
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

	align 16
CSYUV444P10LEP_RGB32C_convert_AVX:
	push rbp
	push rbx
	push rsi
	push rdi
	movdqu [rsp+40],xmm6
	movdqu [rsp+56],xmm7
	mov r10,qword [rsp+72] ;width
	mov r11,qword [rsp+80] ;height
	lea rax,[r10 * 4]
	lea rbx,[r10 * 2]
	sub qword [rsp+88],rax ;dbpl
	sub qword [rsp+96],rbx ;yBpl
	mov rsi,qword [rsp+104] ;yuv2rgb
	mov rdi,qword [rsp+112] ;rgbGammaCorr
	shr r10,3
	align 16
convavxlop:
	mov rbp,r10
	align 16
convavxlop2:
	movntdqa xmm0,[rcx]
	movntdqa xmm4,[rdx]
	movntdqa xmm5,[r8]
	
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

	pextrw rax,xmm0,2 ;y
	movq xmm6,[rsi+rax*8]
	pextrw rax,xmm0,3 ;y
	movhpd xmm6,[rsi+rax*8]
	pextrw rax,xmm4,2 ;u
	movq xmm2,[rsi+rax*8+524288]
	pextrw rax,xmm4,3 ;u
	movhpd xmm2,[rsi+rax*8+524288]
	paddsw xmm6,xmm2
	pextrw rax,xmm5,2 ;v
	movq xmm2,[rsi+rax*8+1048576]
	pextrw rax,xmm5,3 ;v
	movhpd xmm2,[rsi+rax*8+1048576]
	paddsw xmm6,xmm2
	
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

	pextrw rax,xmm6,2 ;r
	movq xmm7,[rdi+rax*8]
	pextrw rax,xmm6,6 ;r
	movhpd xmm7,[rdi+rax*8]
	pextrw rax,xmm6,1 ;g
	movq xmm2,[rdi+rax*8+524288]
	pextrw rax,xmm6,5 ;g
	movhpd xmm2,[rdi+rax*8+524288]
	paddsw xmm7,xmm2
	pextrw rax,xmm6,0 ;b
	movq xmm2,[rdi+rax*8+1048576]
	pextrw rax,xmm6,4 ;b
	movhpd xmm2,[rdi+rax*8+1048576]
	paddsw xmm7,xmm2

	pextrw rax,xmm3,0
	movd xmm1,dword [rdi+rax*4+1572864]
	pextrw rax,xmm3,4
	pinsrd xmm1,dword [rdi+rax*4+1572864],1
	pextrw rax,xmm7,0
	pinsrd xmm1,dword [rdi+rax*4+1572864],2
	pextrw rax,xmm7,4
	pinsrd xmm1,dword [rdi+rax*4+1572864],3
	pextrw rax,xmm3,1
	movd xmm2,dword [rdi+rax*4+1835008]
	pextrw rax,xmm3,5
	pinsrd xmm2,dword [rdi+rax*4+1835008],1
	pextrw rax,xmm7,1
	pinsrd xmm2,dword [rdi+rax*4+1835008],2
	pextrw rax,xmm7,5
	pinsrd xmm2,dword [rdi+rax*4+1835008],3
	por xmm1,xmm2
	pextrw rax,xmm3,2
	movd xmm2,dword [rdi+rax*4+2097152]
	pextrw rax,xmm3,6
	pinsrd xmm2,dword [rdi+rax*4+2097152],1
	pextrw rax,xmm7,2
	pinsrd xmm2,dword [rdi+rax*4+2097152],2
	pextrw rax,xmm7,6
	pinsrd xmm2,dword [rdi+rax*4+2097152],3
	por xmm1,xmm2
	movntdq [r9],xmm1

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
	
	pextrw rax,xmm0,6 ;y
	movq xmm6,[rsi+rax*8]
	pextrw rax,xmm0,7 ;y
	movhpd xmm6,[rsi+rax*8]
	pextrw rax,xmm4,6 ;u
	movq xmm2,[rsi+rax*8+524288]
	pextrw rax,xmm4,7 ;u
	movhpd xmm2,[rsi+rax*8+524288]
	paddsw xmm6,xmm2
	pextrw rax,xmm5,6 ;v
	movq xmm2,[rsi+rax*8+1048576]
	pextrw rax,xmm5,7 ;v
	movhpd xmm2,[rsi+rax*8+1048576]
	paddsw xmm6,xmm2
	
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

	pextrw rax,xmm6,2 ;r
	movq xmm7,[rdi+rax*8]
	pextrw rax,xmm6,6 ;r
	movhpd xmm7,[rdi+rax*8]
	pextrw rax,xmm6,1 ;g
	movq xmm2,[rdi+rax*8+524288]
	pextrw rax,xmm6,5 ;g
	movhpd xmm2,[rdi+rax*8+524288]
	paddsw xmm7,xmm2
	pextrw rax,xmm6,0 ;b
	movq xmm2,[rdi+rax*8+1048576]
	pextrw rax,xmm6,4 ;b
	movhpd xmm2,[rdi+rax*8+1048576]
	paddsw xmm7,xmm2

	pextrw rax,xmm3,0
	movd xmm1,dword [rdi+rax*4+1572864]
	pextrw rax,xmm3,4
	pinsrd xmm1,dword [rdi+rax*4+1572864],1
	pextrw rax,xmm7,0
	pinsrd xmm1,dword [rdi+rax*4+1572864],2
	pextrw rax,xmm7,4
	pinsrd xmm1,dword [rdi+rax*4+1572864],3
	pextrw rax,xmm3,1
	movd xmm2,dword [rdi+rax*4+1835008]
	pextrw rax,xmm3,5
	pinsrd xmm2,dword [rdi+rax*4+1835008],1
	pextrw rax,xmm7,1
	pinsrd xmm2,dword [rdi+rax*4+1835008],2
	pextrw rax,xmm7,5
	pinsrd xmm2,dword [rdi+rax*4+1835008],3
	por xmm1,xmm2
	pextrw rax,xmm3,2
	movd xmm2,dword [rdi+rax*4+2097152]
	pextrw rax,xmm3,6
	pinsrd xmm2,dword [rdi+rax*4+2097152],1
	pextrw rax,xmm7,2
	pinsrd xmm2,dword [rdi+rax*4+2097152],2
	pextrw rax,xmm7,6
	pinsrd xmm2,dword [rdi+rax*4+2097152],3
	por xmm1,xmm2
	movntdq [r9+16],xmm1

	lea rcx,[rcx+16]
	lea rdx,[rdx+16]
	lea r8,[r8+16]
	lea r9,[r9+32]
	dec rbp
	jnz convavxlop2
	
	mov rax,qword [rsp+96] ;yBpl
	add r9,qword [rsp+88] ;dbpl
	add rcx,rax
	add rdx,rax
	add r8,rax
	dec r11
	jnz convavxlop
	
	movdqu xmm6,[rsp+40]
	movdqu xmm7,[rsp+56]
	pop rdi
	pop rsi
	pop rbx
	pop rbp
	ret
