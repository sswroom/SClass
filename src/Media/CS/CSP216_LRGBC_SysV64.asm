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

global CSP216_LRGBC_do_yuy2rgb
global _CSP216_LRGBC_do_yuy2rgb

;void CSP216_LRGBC_do_yuy2rgb(UInt8 *srcY, UInt8 *srcUV, UInt8 *dest, IntOS width, IntOS height, IntOS dbpl, Int64 *yuv2rgb, Int64 *rgbGammaCorr);
;8 rdi
;16 rsi
;24 rbx
;32 retAddr
;rcx srcY
;rdx srcUV
;r8 dest
;r9 width
;72 height
;80 dbpl
;88 yuv2rgb
;96 rgbGammaCorr
	align 16
CSP216_LRGBC_do_yuy2rgb:
_CSP216_LRGBC_do_yuy2rgb:
	sub rsp,32
	mov qword [rsp+8],rdi
	mov qword [rsp+16],rsi
	mov qword [rsp+24],rbx
	lea rax,[r9*8] ;width
	shr r9,1
	sub r9,2 ;Int32 wsize = (width >> 1) - 2;
	sub qword [rsp+80],rax ;dbpl

	mov rsi,qword [rsp+72] ;height
	mov rbx,qword [rsp+88] ;yuv2rgb
	mov rdi,qword [rsp+96] ;rgbGammaCorr

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

	add r8,qword [rsp+80] ;dbpl
	dec rsi ;hleft
	jnz u2rlop

	mov rdi,qword [rsp+8]
	mov rsi,qword [rsp+16]
	mov rbx,qword [rsp+24]
	add rsp,32
	ret
	
