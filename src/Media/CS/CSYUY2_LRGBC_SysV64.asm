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

global CSYUY2_LRGBC_do_yuy2rgb
global _CSYUY2_LRGBC_do_yuy2rgb

;void CSYUY2_LRGBC_do_yuy2rgb(UInt8 *src, UInt8 *dest, OSInt width, OSInt height, OSInt dbpl, Int64 *yuv2rgb, Int64 *rgbGammaCorr);
;0 rbx
;8 retAddr
;rdi src
;rsi dest
;rdx width
;rcx height
;r8 dbpl
;r9 yuv2rgb
;16 rgbGammaCorr

	align 16
CSYUY2_LRGBC_do_yuy2rgb:
_CSYUY2_LRGBC_do_yuy2rgb:
	push rbx
	
	lea rax,[rdx*8] ;width
	shr rdx,1
	sub rdx,2 ;width				OSInt wsize = (width >> 1) - 2;
	sub r8,rax ;dbpl

	mov rbx,r9 ;yuv2rgb
	mov r9,qword [rsp+16] ;rgbGammaCorr

	align 16
u2rlop:
	mov r10,rdx ;wsize

	movzx rax,byte [rdi+0]
	movq xmm0,[rbx+rax*8]
	movzx rax,byte [rdi+1]
	movq xmm2,[rbx+rax*8+2048]
	movzx rax,byte [rdi+2]
	movq xmm1,[rbx+rax*8]
	movzx rax,byte [rdi+3]
	movq xmm3,[rbx+rax*8+4096]
	paddsw xmm2,xmm3
	paddsw xmm0,xmm2
	psraw xmm2,1
	paddsw xmm1,xmm2
	
	pextrw rax,xmm0,0
	movq xmm5,[r9+rax*8+1048576]
	pextrw rax,xmm0,1
	movq xmm6,[r9+rax*8+524288]
	paddsw xmm5,xmm6
	pextrw rax,xmm0,2
	movq xmm6,[r9+rax*8]
	paddsw xmm5,xmm6
	movq [rsi],xmm5

	lea rsi,[rsi+16]
	lea rdi,[rdi+4]
	align 16
u2rlop2:
	movzx rax,byte [rdi+0]
	movq xmm0,[rbx+rax*8]
	movzx rax,byte [rdi+1]
	movq xmm2,[rbx+rax*8+2048]
	movzx rax,byte [rdi+3]
	movq xmm3,[rbx+rax*8+4096]
	paddsw xmm2,xmm3
	paddsw xmm0,xmm2

	pextrw rax,xmm0,0
	movq xmm5,[r9+rax*8+1048576]
	pextrw rax,xmm0,1
	movq xmm6,[r9+rax*8+524288]
	paddsw xmm5,xmm6
	pextrw rax,xmm0,2
	movq xmm6,[r9+rax*8]
	paddsw xmm5,xmm6
	movq [rsi],xmm5

	psraw xmm2,1
	paddsw xmm1,xmm2

	pextrw rax,xmm1,0
	movq xmm5,[r9+rax*8+1048576]
	pextrw rax,xmm1,1
	movq xmm6,[r9+rax*8+524288]
	paddsw xmm5,xmm6
	pextrw rax,xmm1,2
	movq xmm6,[r9+rax*8]
	paddsw xmm5,xmm6
	movq [rsi-8],xmm5

	movzx rax,byte [rdi+2]
	movq xmm1,[rbx+rax*8]
	paddsw xmm1,xmm2

	lea rdi,[rdi+4]
	lea rsi,[rsi+16]
	dec r10
	jnz u2rlop2

	movzx rax,byte [rdi+0]
	movq xmm0,[rbx+rax*8]
	movzx rax,byte [rdi+1]
	movq xmm2,[rbx+rax*8+2048]
	movzx rax,byte [rdi+3]
	movq xmm3,[rbx+rax*8+4096]
	paddsw xmm2,xmm3
	paddsw xmm0,xmm2

	pextrw rax,xmm0,0
	movq xmm5,[r9+rax*8+1048576]
	pextrw rax,xmm0,1
	movq xmm6,[r9+rax*8+524288]
	paddsw xmm5,xmm6
	pextrw rax,xmm0,2
	movq xmm6,[r9+rax*8]
	paddsw xmm5,xmm6
	movq [rsi],xmm5

	movzx rax,byte [rdi+2]
	movq xmm0,[rbx+rax*8]
	paddsw xmm0,xmm2

	pextrw rax,xmm0,0
	movq xmm5,[r9+rax*8+1048576]
	pextrw rax,xmm0,1
	movq xmm6,[r9+rax*8+524288]
	paddsw xmm5,xmm6
	pextrw rax,xmm0,2
	movq xmm6,[r9+rax*8]
	paddsw xmm5,xmm6
	movq [rsi+8],xmm5

	psraw xmm2,1
	paddsw xmm1,xmm2

	pextrw rax,xmm1,0
	movq xmm5,[r9+rax*8+1048576]
	pextrw rax,xmm1,1
	movq xmm6,[r9+rax*8+524288]
	paddsw xmm5,xmm6
	pextrw rax,xmm1,2
	movq xmm6,[r9+rax*8]
	paddsw xmm5,xmm6
	movq [rsi-8],xmm5

	lea rdi,[rdi+4]
	lea rsi,[rsi+16]

	add rsi,r8 ;dbpl
	dec rcx ;hleft
	jnz u2rlop

	pop rbx
	ret
