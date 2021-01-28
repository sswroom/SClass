section .text

global CSP216_LRGBC_do_yuy2rgb

;void CSP216_LRGBC_do_yuy2rgb(UInt8 *srcY, UInt8 *srcUV, UInt8 *dest, OSInt width, OSInt height, OSInt dbpl, Int64 *yuv2rgb, Int64 *rgbGammaCorr);
;-24 rdi
;-16 rsi
;-8 rbx
;0 retAddr
;rcx srcY
;rdx srcUV
;r8 dest
;r9 width
;40 height
;48 dbpl
;56 yuv2rgb
;64 rgbGammaCorr
	align 16
CSP216_LRGBC_do_yuy2rgb:
	mov qword [rsp-24],rdi
	mov qword [rsp-16],rsi
	mov qword [rsp-8],rbx
	lea rax,[r9*8] ;width
	shr r9,1
	sub r9,2 ;Int32 wsize = (width >> 1) - 2;
	sub qword [rsp+48],rax ;dbpl

	mov rsi,qword [rsp+40] ;height
	mov rbx,qword [rsp+56] ;yuv2rgb
	mov rdi,qword [rsp+64] ;rgbGammaCorr

	align 16
u2rlop:
	mov rbp,r9 ;wsize

	movzx rax,word [rcx+0]
	movq xmm0,[rbx+rax*8]
	movzx rax,word [rdx+0]
	movq xmm2,[rbx+rax*8+524288]
	movzx rax,word [rcx+2]
	movq xmm1,[rbx+rax*8]
	movzx rax,word [rdx+2]
	movq xmm3,[rbx+rax*8+1048576]
	paddsw xmm2,xmm3
	paddsw xmm0,xmm2
	psraw xmm2,1
	paddsw xmm1,xmm2
	
	pextrw rax,xmm0,0
	movq xmm5,[rdi+rax*8+1048576]
	pextrw rax,xmm0,1
	movq xmm6,[rdi+rax*8+524288]
	paddsw xmm5,xmm6
	pextrw rax,xmm0,2
	movq xmm6,[rdi+rax*8]
	paddsw xmm5,xmm6
	movq [r8],xmm5

	lea r8,[r8+16]
	lea rdx,[rdx+4]
	lea rcx,[rcx+4]
	align 16
u2rlop2:
	movzx rax,word [rcx+0]
	movq xmm0,[rbx+rax*8]
	movzx rax,word [rdx+0]
	movq xmm2,[rbx+rax*8+524288]
	movzx rax,word [rdx+2]
	movq xmm3,[rbx+rax*8+1048576]
	paddsw xmm2,xmm3
	paddsw xmm0,xmm2

	pextrw rax,xmm0,0
	movq xmm5,[rdi+rax*8+1048576]
	pextrw rax,xmm0,1
	movq xmm6,[rdi+rax*8+524288]
	paddsw xmm5,xmm6
	pextrw rax,xmm0,2
	movq xmm6,[rdi+rax*8]
	paddsw xmm5,xmm6
	movq [r8],xmm5

	psraw xmm2,1
	paddsw xmm1,xmm2

	pextrw rax,xmm1,0
	movq xmm5,[rdi+rax*8+1048576]
	pextrw rax,xmm1,1
	movq xmm6,[rdi+rax*8+524288]
	paddsw xmm5,xmm6
	pextrw rax,xmm1,2
	movq xmm6,[rdi+rax*8]
	paddsw xmm5,xmm6
	movq [r8-8],xmm5

	movzx rax,word [rcx+2]
	movq xmm1,[rbx+rax*8]
	paddsw xmm1,xmm2

	lea rdx,[rdx+4]
	lea rcx,[rcx+4]
	lea r8,[r8+16]
	dec rbp
	jnz u2rlop2

	movzx rax,word [rcx+0]
	movq xmm0,[rbx+rax*8]
	movzx rax,word [rdx+0]
	movq xmm2,[rbx+rax*8+524288]
	movzx rax,word [rdx+2]
	movq xmm3,[rbx+rax*8+1048576]
	paddsw xmm2,xmm3
	paddsw xmm0,xmm2

	pextrw rax,xmm0,0
	movq xmm5,[rdi+rax*8+1048576]
	pextrw rax,xmm0,1
	movq xmm6,[rdi+rax*8+524288]
	paddsw xmm5,xmm6
	pextrw rax,xmm0,2
	movq xmm6,[rdi+rax*8]
	paddsw xmm5,xmm6
	movq [r8],xmm5

	movzx rax,word [rcx+2]
	movq xmm0,[rbx+rax*8]
	paddsw xmm0,xmm2

	pextrw rax,xmm0,0
	movq xmm5,[rdi+rax*8+1048576]
	pextrw rax,xmm0,1
	movq xmm6,[rdi+rax*8+524288]
	paddsw xmm5,xmm6
	pextrw rax,xmm0,2
	movq xmm6,[rdi+rax*8]
	paddsw xmm5,xmm6
	movq [r8+8],xmm5

	psraw xmm2,1
	paddsw xmm1,xmm2

	pextrw rax,xmm1,0
	movq xmm5,[rdi+rax*8+1048576]
	pextrw rax,xmm1,1
	movq xmm6,[rdi+rax*8+524288]
	paddsw xmm5,xmm6
	pextrw rax,xmm1,2
	movq xmm6,[rdi+rax*8]
	paddsw xmm5,xmm6
	movq [r8],xmm5

	lea rdx,[rdx+4]
	lea rcx,[rcx+4]
	lea r8,[r8+8]

	add r8,qword [rsp+48] ;dbpl
	dec rsi ;hleft
	jnz u2rlop

	mov rdi,qword [rsp-24]
	mov rsi,qword [rsp-16]
	mov rbx,qword [rsp-8]
	ret
	