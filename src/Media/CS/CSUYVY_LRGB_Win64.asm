section .text

global CSUYVY_LRGB_do_uyvy2rgb

;void CSUYVY_LRGB_do_uyvy2rgb(UInt8 *src, UInt8 *dest, IntOS width, IntOS height, IntOS dbpl, Int64 *yuv2rgb, UInt8 *rgbGammaCorr);
;0 rbp
;8 rdi
;16 rsi
;24 rbx
;32 retAddr
;rcx src
;rdx dest
;r8 width
;r9 height
;72 dbpl
;80 yuv2rgb
;88 rgbGammaCorr

	align 16
CSUYVY_LRGB_do_uyvy2rgb:
	push rbp
	push rbx
	push rsi
	push rdi
	
	mov rsi,rcx
	mov rdi,rdx
	lea rdx,[r8*8]
	shr r8,1
	sub r8,2 ;width				IntOS wsize = (width >> 1) - 2;
	sub qword [rsp+72],rdx ;dbpl

	mov rbx,qword [rsp+80] ;yuv2rgb
	mov rdx,qword [rsp+88] ;rgbGammaCorr

	align 16
u2rlop:
	mov rbp,r8 ;wsize

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
	
	mov ecx,0x3fff0000
	pextrw rax,xmm0,2
	mov cx,word [rdx+rax*2]
	mov dword [rdi+4],ecx
	pextrw rax,xmm0,1
	mov cx,word [rdx+rax*2]
	shl ecx,16
	pextrw rax,xmm0,0
	mov cx,word [rdx+rax*2]
	mov dword [rdi],ecx
	lea rdi,[rdi+16]
	lea rsi,[rsi+4]
u2rlop2:
	movzx rax,byte [rsi+1]
	movq xmm0,[rbx+rax*8]
	movzx rax,byte [rsi]
	movq xmm2,[rbx+rax*8+2048]
	movzx rax,byte [rsi+2]
	movq xmm3,[rbx+rax*8+4096]
	paddsw xmm0,xmm2
	paddsw xmm0,xmm3

	mov ecx,0x3fff0000
	pextrw rax,xmm0,2
	mov cx,word [rdx+rax*2]
	mov dword [rdi+4],ecx
	pextrw rax,xmm0,1
	mov cx,word [rdx+rax*2]
	shl ecx,16
	pextrw rax,xmm0,0
	mov cx,word [rdx+rax*2]
	mov dword [rdi],ecx

	psraw xmm2,1
	psraw xmm3,1
	paddsw xmm1,xmm2
	paddsw xmm1,xmm3

	mov ecx,0x3fff0000
	pextrw rax,xmm1,2
	mov cx,word [rdx+rax*2]
	mov dword [rdi-4],ecx
	pextrw rax,xmm1,1
	mov cx,word [rdx+rax*2]
	shl ecx,16
	pextrw rax,xmm1,0
	mov cx,word [rdx+rax*2]
	mov dword [rdi-8],ecx

	movzx rax,byte [rsi+3]
	movq xmm1,[rbx+rax*8]
	paddsw xmm1,xmm2
	paddsw xmm1,xmm3

	lea rsi,[rsi+4]
	lea rdi,[rdi+16]
	dec rbp
	jnz u2rlop2

	movzx rax,byte [rsi+1]
	movq xmm0,[rbx+rax*8]
	movzx rax,byte [rsi]
	movq xmm2,[rbx+rax*8+2048]
	movzx rax,byte [rsi+2]
	movq xmm3,[rbx+rax*8+4096]
	paddsw xmm0,xmm2
	paddsw xmm0,xmm3

	mov ecx,0x3fff0000
	pextrw rax,xmm0,2
	mov cx,word [rdx+rax*2]
	mov dword [rdi+4],ecx
	pextrw rax,xmm0,1
	mov cx,word [rdx+rax*2]
	shl ecx,16
	pextrw rax,xmm0,0
	mov cx,word [rdx+rax*2]
	mov dword [rdi],ecx

	movzx rax,byte [rsi+3]
	movq xmm0,[rbx+rax*8]
	paddsw xmm0,xmm2
	paddsw xmm0,xmm3

	mov ecx,0x3fff0000
	pextrw rax,xmm0,2
	mov cx,word [rdx+rax*2]
	mov dword [rdi+12],ecx
	pextrw rax,xmm0,1
	mov cx,word [rdx+rax*2]
	shl ecx,16
	pextrw rax,xmm0,0
	mov cx,word [rdx+rax*2]
	mov dword [rdi+8],ecx

	psraw xmm2,1
	psraw xmm3,1
	paddsw xmm1,xmm2
	paddsw xmm1,xmm3

	mov ecx,0x3fff0000
	pextrw rax,xmm1,2
	mov cx,word [rdx+rax*2]
	mov dword [rdi-4],ecx
	pextrw rax,xmm1,1
	mov cx,word [rdx+rax*2]
	shl ecx,16
	pextrw rax,xmm1,0
	mov cx,word [rdx+rax*2]
	mov dword [rdi-8],ecx

	lea rsi,[rsi+4]
	add rdi,qword [rsp+72] ;dbpl
	dec r9 ;hleft
	jnz u2rlop

	pop rdi
	pop rsi
	pop rbx
	pop rbp
	ret