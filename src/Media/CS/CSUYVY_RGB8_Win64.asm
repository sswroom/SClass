section .text

global CSUYVY_RGB8_do_uyvy2rgb

;void CSUYVY_RGB8_do_uyvy2rgb(UInt8 *src, UInt8 *dest, OSInt width, OSInt height, OSInt dbpl, Int64 *yuv2rgb, UInt8 *rgbGammaCorr);
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
CSUYVY_RGB8_do_uyvy2rgb:
	push rbx
	push rsi
	push rdi
	
	mov rsi,rcx
	mov rdi,rdx
	
	lea rdx,[r8*4]
	shr r8,1
	sub r8,2 ;width				OSInt wsize = (width >> 1) - 2;
	sub qword [rsp+64],rdx ;dbpl

	mov rbx,qword [rsp+72] ;yuv2rgb
	mov rdx,qword [rsp+80] ;rgbGammaCorr

	align 16
u2rlop:
	mov r10,r8 ;wsize

	movzx rax,byte [rsi+1]
	movq xmm0,[rbx+rax*8]
	movzx rax,byte [rsi+3]
	movq xmm1,[rbx+rax*8]
	movzx rax,byte [rsi]
	movq xmm2,[rbx+rax*8+2048]
	movzx rax,byte [rsi+2]
	movq xmm3,[rbx+rax*8+4096]
	paddsw xmm0,xmm2
	paddsw xmm0,xmm3
	psraw xmm2,1
	psraw xmm3,1
	paddsw xmm1,xmm2
	paddsw xmm1,xmm3
	
	pextrw rax,xmm0,2
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
	align 16
u2rlop2:
	movzx rax,byte [rsi+1]
	movq xmm0,[rbx+rax*8]
	movzx rax,byte [rsi]
	movq xmm2,[rbx+rax*8+2048]
	movzx rax,byte [rsi+2]
	movq xmm3,[rbx+rax*8+4096]
	paddsw xmm0,xmm2
	paddsw xmm0,xmm3

	pextrw rax,xmm0,2
	movzx ecx,byte [rdx+rax]
	or ecx,0xff00
	shl ecx,16
	pextrw rax,xmm0,1
	mov ch,byte [rdx+rax+65536]
	pextrw rax,xmm0,0
	mov cl,byte [rdx+rax+131072]
	mov dword [rdi],ecx

	psraw xmm2,1
	psraw xmm3,1
	paddsw xmm1,xmm2
	paddsw xmm1,xmm3

	pextrw rax,xmm1,2
	movzx ecx,byte [rdx+rax]
	or ecx,0xff00
	shl ecx,16
	pextrw rax,xmm1,1
	mov ch,byte [rdx+rax+65536]
	pextrw rax,xmm1,0
	mov cl,byte [rdx+rax+131072]
	mov dword [rdi-4],ecx

	movzx rax,byte [rsi+3]
	movq xmm1,[rbx+rax*8]
	paddsw xmm1,xmm2
	paddsw xmm1,xmm3

	lea rsi,[rsi+4]
	lea rdi,[rdi+8]
	dec r10
	jnz u2rlop2

	movzx rax,byte [rsi+1]
	movq xmm0,[rbx+rax*8]
	movzx rax,byte [rsi]
	movq xmm2,[rbx+rax*8+2048]
	movzx rax,byte [rsi+2]
	movq xmm3,[rbx+rax*8+4096]
	paddsw xmm0,xmm2
	paddsw xmm0,xmm3

	pextrw rax,xmm0,2
	movzx ecx,byte [rdx+rax]
	or ecx,0xff00
	shl ecx,16
	pextrw rax,xmm0,1
	mov ch,byte [rdx+rax+65536]
	pextrw rax,xmm0,0
	mov cl,byte [rdx+rax+131072]
	mov dword [rdi],ecx

	movzx rax,byte [rsi+3]
	movq xmm0,[rbx+rax*8]
	paddsw xmm0,xmm2
	paddsw xmm0,xmm3

	pextrw rax,xmm0,2
	movzx ecx,byte [rdx+rax]
	or ecx,0xff00
	shl ecx,16
	pextrw rax,xmm0,1
	mov ch,byte [rdx+rax+65536]
	pextrw rax,xmm0,0
	mov cl,byte [rdx+rax+131072]
	mov dword [rdi+4],ecx

	psraw xmm2,1
	psraw xmm3,1
	paddsw xmm1,xmm2
	paddsw xmm1,xmm3

	pextrw rax,xmm1,2
	movzx ecx,byte [rdx+rax]
	or ecx,0xff00
	shl ecx,16
	pextrw rax,xmm1,1
	mov ch,byte [rdx+rax+65536]
	pextrw rax,xmm1,0
	mov cl,byte [rdx+rax+131072]
	mov dword [rdi-4],ecx
	
	lea rdi,[rdi+8]
	lea rsi,[rsi+4]
	add rdi,qword [rsp+64] ;dbpl
	dec r9 ;hleft
	jnz u2rlop

	pop rdi
	pop rsi
	pop rbx
	ret