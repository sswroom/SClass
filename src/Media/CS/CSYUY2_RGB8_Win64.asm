section .text

global CSYUY2_RGB8_do_yuy2rgb

;void CSYUY2_RGB8_do_yuy2rgb(UInt8 *src, UInt8 *dest, OSInt width, OSInt height, OSInt dbpl, Int64 *yuv2rgb, UInt8 *rgbGammaCorr);
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
CSYUY2_RGB8_do_yuy2rgb:
	push rbx
	push rsi
	push rdi
	
	mov rsi,rcx ;src
	mov rdi,rdx ;dest
	lea rdx,[r8*4] ;width
	shr r8,1
	sub r8,2 ;width				OSInt wsize = (width >> 1) - 2;
	sub qword [rsp+64],rdx ;dbpl

	mov rbx,qword [rsp+72] ;yuv2rgb
	mov rdx,qword [rsp+80] ;rgbGammaCorr

	align 16
u2rlop:
	mov r10,r8 ;wsize

	movzx rax,byte [rsi]
	movq xmm0,[rbx+rax*8]
	movzx rax,byte [rsi+1]
	movq xmm2,[rbx+rax*8+2048]
	movzx rax,byte [rsi+2]
	movq xmm1,[rbx+rax*8]
	movzx rax,byte [rsi+3]
	movq xmm4,[rbx+rax*8+4096]
	paddsw xmm2,xmm4
	paddsw xmm0,xmm2
	psraw xmm2,1
	paddsw xmm1,xmm2

	movq xmm3,xmm0
	punpckhdq xmm3,xmm3

	pextrw rax,xmm0, 2
	movzx ecx,byte [rdx+rax]
	or ecx,0xff00
	shl ecx,16
	pextrw rax,xmm0,1
	mov ch,byte [rdx+rax+65536]
	pextrw rax,xmm0,0
	mov cl,byte [rdx+rax+131072]
	mov dword [rdi],ecx
	lea rdi,[rdi+8]
	lea rsi,[rsi+4]

	ALIGN 16
u2rlop2:
	movzx rax,byte [rsi]
	movq xmm0,[rbx+rax*8]
	movzx rax,byte [rsi+1]
	movq xmm2,[rbx+rax*8+2048]
	movzx rax,byte [rsi+2]
	movq xmm4,[rbx+rax*8]
	movzx rax,byte [rsi+3]
	movq xmm5,[rbx+rax*8+4096]
	paddsw xmm2,xmm5
	paddsw xmm0,xmm2
	psraw xmm2,1

	pextrw rax,xmm0,2
	movzx ecx,byte [rdx+rax]
	paddsw xmm1,xmm2
	or ecx,0xff00
	shl ecx,16
	pextrw rax,xmm0,1
	mov ch,byte [rdx+rax+65536]
	pextrw rax,xmm0,0
	mov cl,byte [rdx+rax+131072]
	mov dword [rdi],ecx

	pextrw rax,xmm1,2
	movzx ecx,byte [rdx+rax]
	or ecx,0xff00
	shl ecx,16
	pextrw rax,xmm1,1
	mov ch,byte [rdx+rax+65536]
	pextrw rax,xmm1,0
	mov cl,byte [rdx+rax+131072]
	mov dword [rdi-4],ecx

	movq xmm1,xmm4
	paddsw xmm1,xmm2

	lea rsi,[rsi+4]
	lea rdi,[rdi+8]
	dec r10
	jnz u2rlop2

	movzx rax,byte [rsi]
	movq xmm0,[rbx+rax*8]
	movzx rax,byte [rsi+1]
	movq xmm2,[rbx+rax*8+2048]
	movzx rax,byte [rsi+2]
	movq xmm4,[rbx+rax*8]
	movzx rax,byte [rsi+3]
	movq xmm3,[rbx+rax*8+4096]
	paddsw xmm2,xmm3
	paddsw xmm0,xmm2
	paddsw xmm4,xmm2

	pextrw rax,xmm0,2
	movzx ecx,byte [rdx+rax]
	or ecx,0xff00
	shl ecx,16
	pextrw rax,xmm0,1
	mov ch,byte [rdx+rax+65536]
	pextrw rax,xmm0,0
	mov cl,byte [rdx+rax+131072]
	mov dword [rdi],ecx

	pextrw rax,xmm4,2
	movzx ecx,byte [rdx+rax]
	or ecx,0xff00
	shl ecx,16
	pextrw rax,xmm4,1
	mov ch,byte [rdx+rax+65536]
	pextrw rax,xmm4,0
	mov cl,byte [rdx+rax+131072]
	mov dword [rdi+4],ecx

	psraw xmm2,1
	paddsw xmm1,xmm2

	pextrw rax,xmm1,2
	movzx ecx,byte [rdx+rax]
	or ecx,0xff00
	shl ecx,16
	pextrw rax,xmm1,1
	mov ch,byte [rdx+rax+65536]
	pextrw rax,xmm1,0
	mov cl,byte [rdx+rax+131072]
	mov dword [rdi-4],ecx

	lea rsi,[rsi+4]
	lea rdi,[rdi+8]

	add rdi,qword [rsp+64] ;dbpl
	dec r9 ;hleft
	jnz u2rlop

	pop rdi
	pop rsi
	pop rbx
	ret
