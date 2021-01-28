section .text

global CSUYVY_RGB8_do_uyvy2rgb
global _CSUYVY_RGB8_do_uyvy2rgb

;void CSUYVY_RGB8_do_uyvy2rgb(UInt8 *src, UInt8 *dest, OSInt width, OSInt height, OSInt dbpl, Int64 *yuv2rgb, UInt8 *rgbGammaCorr);
;0 ebx
;8 retAddr
;rdi rcx src
;rsi rdx dest
;rdx r8 width
;rcx r9 height
;r8 64 dbpl
;r9 72 yuv2rgb
;16 rgbGammaCorr

	align 16
CSUYVY_RGB8_do_uyvy2rgb:
_CSUYVY_RGB8_do_uyvy2rgb:
	push rbx
	
	lea rax,[rdx*4]
	shr rdx,1
	sub rdx,2 ;width				OSInt wsize = (width >> 1) - 2;
	sub r8,rax ;dbpl

	mov r11,rcx ;hleft
	mov rbx,qword [rsp+16] ;rgbGammaCorr

	align 16
u2rlop:
	mov r10,rdx;wsize

	movzx rax,byte [rdi+1]
	movq xmm0,[r9+rax*8]
	movzx rax,byte [rdi+3]
	movq xmm1,[r9+rax*8]
	movzx rax,byte [rdi]
	movq xmm2,[r9+rax*8+2048]
	movzx rax,byte [rdi+2]
	movq xmm3,[r9+rax*8+4096]
	paddsw xmm0,xmm2
	paddsw xmm0,xmm3
	psraw xmm2,1
	psraw xmm3,1
	paddsw xmm1,xmm2
	paddsw xmm1,xmm3
	
	pextrw rax,xmm0,2
	movzx ecx,byte [rbx+rax]
	or ecx,0xff00
	shl ecx,16
	pextrw rax,xmm0,1
	mov ch,byte [rbx+rax+65536]
	pextrw rax,xmm0,0
	mov cl,byte [rbx+rax+131072]
	mov dword [rsi],ecx
	lea rsi,[rsi+8]
	lea rdi,[rdi+4]
	align 16
u2rlop2:
	movzx rax,byte [rdi+1]
	movq xmm0,[r9+rax*8]
	movzx rax,byte [rdi]
	movq xmm2,[r9+rax*8+2048]
	movzx rax,byte [rdi+2]
	movq xmm3,[r9+rax*8+4096]
	paddsw xmm0,xmm2
	paddsw xmm0,xmm3

	pextrw rax,xmm0,2
	movzx ecx,byte [rbx+rax]
	or ecx,0xff00
	shl ecx,16
	pextrw rax,xmm0,1
	mov ch,byte [rbx+rax+65536]
	pextrw rax,xmm0,0
	mov cl,byte [rbx+rax+131072]
	mov dword [rsi],ecx

	psraw xmm2,1
	psraw xmm3,1
	paddsw xmm1,xmm2
	paddsw xmm1,xmm3

	pextrw rax,xmm1,2
	movzx ecx,byte [rbx+rax]
	or ecx,0xff00
	shl ecx,16
	pextrw rax,xmm1,1
	mov ch,byte [rbx+rax+65536]
	pextrw rax,xmm1,0
	mov cl,byte [rbx+rax+131072]
	mov dword [rsi-4],ecx

	movzx rax,byte [rdi+3]
	movq xmm1,[r9+rax*8]
	paddsw xmm1,xmm2
	paddsw xmm1,xmm3

	lea rdi,[rdi+4]
	lea rsi,[rsi+8]
	dec r10
	jnz u2rlop2

	movzx rax,byte [rdi+1]
	movq xmm0,[r9+rax*8]
	movzx rax,byte [rdi]
	movq xmm2,[r9+rax*8+2048]
	movzx rax,byte [rdi+2]
	movq xmm3,[r9+rax*8+4096]
	paddsw xmm0,xmm2
	paddsw xmm0,xmm3

	pextrw rax,xmm0,2
	movzx ecx,byte [rbx+rax]
	or ecx,0xff00
	shl ecx,16
	pextrw rax,xmm0,1
	mov ch,byte [rbx+rax+65536]
	pextrw rax,xmm0,0
	mov cl,byte [rbx+rax+131072]
	mov dword [rsi],ecx

	movzx rax,byte [rdi+3]
	movq xmm0,[r9+rax*8]
	paddsw xmm0,xmm2
	paddsw xmm0,xmm3

	pextrw rax,xmm0,2
	movzx ecx,byte [rbx+rax]
	or ecx,0xff00
	shl ecx,16
	pextrw rax,xmm0,1
	mov ch,byte [rbx+rax+65536]
	pextrw rax,xmm0,0
	mov cl,byte [rbx+rax+131072]
	mov dword [rsi+4],ecx

	psraw xmm2,1
	psraw xmm3,1
	paddsw xmm1,xmm2
	paddsw xmm1,xmm3

	pextrw rax,xmm1,2
	movzx ecx,byte [rbx+rax]
	or ecx,0xff00
	shl ecx,16
	pextrw rax,xmm1,1
	mov ch,byte [rbx+rax+65536]
	pextrw rax,xmm1,0
	mov cl,byte [rbx+rax+131072]
	mov dword [rsi-4],ecx
	
	lea rsi,[rsi+8]
	lea rdi,[rdi+4]
	add rsi,r8 ;dbpl
	dec r11 ;hleft
	jnz u2rlop

	pop rbx
	ret
