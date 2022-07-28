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

global CSYUY2_RGB8_do_yuy2rgb
global _CSYUY2_RGB8_do_yuy2rgb

;void CSYUY2_RGB8_do_yuy2rgb(UInt8 *src, UInt8 *dest, OSInt width, OSInt height, OSInt dbpl, Int64 *yuv2rgb, UInt8 *rgbGammaCorr);
;0 ebx
;8 retAddr
;rdi src
;rsi dest
;rdx width
;rcx height
;r8 dbpl
;r9 yuv2rgb
;16 rgbGammaCorr

	align 16
CSYUY2_RGB8_do_yuy2rgb:
_CSYUY2_RGB8_do_yuy2rgb:
	push rbx
	
	lea rbx,[rdx*4] ;width
	shr rdx,1
	sub rdx,2 ;width				OSInt wsize = (width >> 1) - 2;
	sub r8,rbx ;dbpl

	mov rbx,qword [rsp+16] ;rgbGammaCorr
	mov r11,rcx ;height

	align 16
u2rlop:
	mov r10,rdx ;wsize

	movzx rax,byte [rdi]
	movq xmm0,[r9+rax*8]
	movzx rax,byte [rdi+1]
	movq xmm2,[r9+rax*8+2048]
	movzx rax,byte [rdi+2]
	movq xmm1,[r9+rax*8]
	movzx rax,byte [rdi+3]
	movq xmm4,[r9+rax*8+4096]
	paddsw xmm2,xmm4
	paddsw xmm0,xmm2
	psraw xmm2,1
	paddsw xmm1,xmm2

	movq xmm3,xmm0
	punpckhdq xmm3,xmm3

	pextrw rax,xmm0, 2
	movzx ecx,byte [rbx+rax]
	or ecx,0xff00
	shl ecx,16
	pextrw rax,xmm0,1
	mov ch,byte [rbx+rax+65536]
	pextrw rax,xmm0,2
	mov cl,byte [rbx+rax+131072]
	mov dword [rsi],ecx
	lea rsi,[rsi+8]
	lea rdi,[rdi+4]

	ALIGN 16
u2rlop2:
	movzx rax,byte [rdi]
	movq xmm0,[r9+rax*8]
	movzx rax,byte [rdi+1]
	movq xmm2,[r9+rax*8+2048]
	movzx rax,byte [rdi+2]
	movq xmm4,[r9+rax*8]
	movzx rax,byte [rdi+3]
	movq xmm5,[r9+rax*8+4096]
	paddsw xmm2,xmm5
	paddsw xmm0,xmm2
	psraw xmm2,1

	pextrw rax,xmm0,2
	movzx ecx,byte [rbx+rax]
	paddsw xmm1,xmm2
	or ecx,0xff00
	shl ecx,16
	pextrw rax,xmm0,1
	mov ch,byte [rbx+rax+65536]
	pextrw rax,xmm0,0
	mov cl,byte [rbx+rax+131072]
	mov dword [rsi],ecx

	pextrw rax,xmm1,2
	movzx ecx,byte [rbx+rax]
	or ecx,0xff00
	shl ecx,16
	pextrw rax,xmm1,1
	mov ch,byte [rbx+rax+65536]
	pextrw rax,xmm1,0
	mov cl,byte [rbx+rax+131072]
	mov dword [rsi-4],ecx

	movq xmm1,xmm4
	paddsw xmm1,xmm2

	lea rdi,[rdi+4]
	lea rsi,[rsi+8]
	dec r10
	jnz u2rlop2

	movzx rax,byte [rdi]
	movq xmm0,[r9+rax*8]
	movzx rax,byte [rdi+1]
	movq xmm2,[r9+rax*8+2048]
	movzx rax,byte [rdi+2]
	movq xmm4,[r9+rax*8]
	movzx rax,byte [rdi+3]
	movq xmm3,[r9+rax*8+4096]
	paddsw xmm2,xmm3
	paddsw xmm0,xmm2
	paddsw xmm4,xmm2

	pextrw rax,xmm0,2
	movzx ecx,byte [rbx+rax]
	or ecx,0xff00
	shl ecx,16
	pextrw rax,xmm0,1
	mov ch,byte [rbx+rax+65536]
	pextrw rax,xmm0,0
	mov cl,byte [rbx+rax+131072]
	mov dword [rsi],ecx

	pextrw rax,xmm4,2
	movzx ecx,byte [rbx+rax]
	or ecx,0xff00
	shl ecx,16
	pextrw rax,xmm4,1
	mov ch,byte [rbx+rax+65536]
	pextrw rax,xmm4,0
	mov cl,byte [rbx+rax+131072]
	mov dword [rsi+4],ecx

	psraw xmm2,1
	paddsw xmm1,xmm2

	pextrw rax,xmm1,2
	movzx ecx,byte [rbx+rax]
	or ecx,0xff00
	shl ecx,16
	pextrw rax,xmm1,1
	mov ch,byte [rbx+rax+65536]
	pextrw rax,xmm1,0
	mov cl,byte [rbx+rax+131072]
	mov dword [rsi-4],ecx

	lea rdi,[rdi+4]
	lea rsi,[rsi+8]

	add rsi,r8 ;dbpl
	dec r11 ;hleft
	jnz u2rlop

	pop rbx
	ret
