section .text

global CSUYVY_LRGBC_do_yuy2rgb
global _CSUYVY_LRGBC_do_yuy2rgb

;void CSUYVY_LRGBC_do_yuy2rgb(UInt8 *src, UInt8 *dest, OSInt width, OSInt height, OSInt dbpl, Int64 *yuv2rgb, Int64 *rgbGammaCorr);
;0 edi
;8 esi
;16 ebx
;24 retAddr
;rcx src
;rdx dest
;r8 width
;r9 height
;64 dbpl
;72 yuv2rgb
;80 rgbGammaCorr

	align 16
CSUYVY_LRGBC_do_yuy2rgb:
_CSUYVY_LRGBC_do_yuy2rgb:
	push rbx
	push rsi
	push rdi
	
	mov rsi,rcx
	mov rdi,rdx
	
	lea rdx,[r8*8]
	shr r8,1
	sub r8,2 ;				OSInt wsize = (width >> 1) - 2;
	sub qword [rsp+64],rdx ;dbpl

	mov rbx,qword [rsp+72] ;yuv2rgb
	mov rdx,qword [rsp+80] ;rgbGammaCorr

	align 16
u2rlop:
	mov r10,r8 ;wsize

	movzx rax,byte [rsi+0]
	movq xmm2,[rbx+rax*8+2048]
	movzx rax,byte [rsi+1]
	movq xmm0,[rbx+rax*8]
	movzx rax,byte [rsi+2]
	movq xmm3,[rbx+rax*8+4096]
	movzx rax,byte [rsi+3]
	movq xmm1,[rbx+rax*8]
	paddsw xmm2,xmm3
	paddsw xmm0,xmm2
	psraw xmm2,1
	paddsw xmm1,xmm2
	
	pextrw rax,xmm0,0
	movq xmm5,[rdx+rax*8+1048576]
	pextrw rax,xmm0,1
	movq xmm6,[rdx+rax*8+524288]
	paddsw xmm5,xmm6
	pextrw rax,xmm0,2
	movq xmm6,[rdx+rax*8]
	paddsw xmm5,xmm6
	movq [rdi],xmm5

	lea rdi,[rdi+16]
	lea rsi,[rsi+4]
	align 16
u2rlop2:
	movzx rax,byte [rsi+0]
	movq xmm2,[rbx+rax*8+2048]
	movzx rax,byte [rsi+1]
	movq xmm0,[rbx+rax*8]
	movzx rax,byte [rsi+2]
	movq xmm3,[rbx+rax*8+4096]
	paddsw xmm2,xmm3
	paddsw xmm0,xmm2

	pextrw rax,xmm0,0
	movq xmm5,[rdx+rax*8+1048576]
	pextrw rax,xmm0,1
	movq xmm6,[rdx+rax*8+524288]
	paddsw xmm5,xmm6
	pextrw rax,xmm0,2
	movq xmm6,[rdx+rax*8]
	paddsw xmm5,xmm6
	movq [rdi],xmm5

	psraw xmm2,1
	paddsw xmm1,xmm2

	pextrw rax,xmm1,0
	movq xmm5,[rdx+rcx*8+1048576]
	pextrw rax,xmm1,1
	movq xmm6,[rdx+rax*8+524288]
	paddsw xmm5,xmm6
	pextrw rax,xmm1,2
	movq xmm6,[rdx+rax*8]
	paddsw xmm5,xmm6
	movq [rdi-8],xmm5

	movzx rax,byte [rsi+3]
	movq xmm1,[rbx+rax*8]
	paddsw xmm1,xmm2

	lea rsi,[rsi+4]
	lea rdi,[rdi+16]
	dec r10
	jnz u2rlop2

	movzx rax,byte [rsi+0]
	movq xmm2,[rbx+rax*8+2048]
	movzx rax,byte [rsi+1]
	movq xmm0,[rbx+rax*8]
	movzx rax,byte [rsi+2]
	movq xmm3,[rbx+rax*8+4096]
	paddsw xmm2,xmm3
	paddsw xmm0,xmm2

	pextrw rax,xmm0,0
	movq xmm5,[rdx+rax*8+1048576]
	pextrw rax,xmm0,1
	movq xmm6,[rdx+rax*8+524288]
	paddsw xmm5,xmm6
	pextrw rax,xmm0,2
	movq xmm6,[rdx+rax*8]
	paddsw xmm5,xmm6
	movq [rdi],xmm5

	movzx rax,byte [rsi+3]
	movq xmm0,[rbx+rax*8]
	paddsw xmm0,xmm2

	pextrw rax,xmm0,0
	movq xmm5,[rdx+rax*8+1048576]
	pextrw rax,xmm0,1
	movq xmm6,[rdx+rax*8+524288]
	paddsw xmm5,xmm6
	pextrw eax,xmm0,2
	movq xmm6,[rdx+rax*8]
	paddsw xmm5,xmm6
	movq [rdi+8],xmm5

	psraw xmm2,1
	paddsw xmm1,xmm2

	pextrw rax,xmm1,0
	movq xmm5,[rdx+rax*8+1048576]
	pextrw rax,xmm1,1
	movq xmm6,[rdx+rax*8+524288]
	paddsw xmm5,xmm6
	pextrw rax,xmm1,2
	movq xmm6,[rdx+rax*8]
	paddsw xmm5,xmm6
	movq [rdi-8],xmm5

	lea rsi,[rsi+4]
	lea rdi,[rdi+16]

	add rdi,qword [rsp+64] ;dbpl
	dec r9 ;hleft
	jnz u2rlop

	pop rdi
	pop rsi
	pop rbx
	ret
