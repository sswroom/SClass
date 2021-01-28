section .text

global CSAYUV_RGB8_do_ayuvrgb
global _CSAYUV_RGB8_do_ayuvrgb

;void CSAYUV_RGB8_do_ayuvrgb(UInt8 *src, UInt8 *dest, OSInt width, OSInt height, OSInt dbpl, Int64 *yuv2rgb, UInt8 *rgbGammaCorr);
;0 rdi
;8 rsi
;16 rbx
;24 rbp
;32 retAddr
;rcx src
;rdx dest
;r8 width
;r9 height
;72 dbpl
;80 yuv2rgb
;88 rgbGammaCorr

	align 16
CSAYUV_RGB8_do_ayuvrgb:
_CSAYUV_RGB8_do_ayuvrgb:
	push rbp
	push rbx
	push rsi
	push rdi
	
	lea rax,[r8*4] ;width
	sub qword [rsp+72],rax ;dbpl
	mov rsi,rcx ;src
	mov rdi,rdx ;dest
	mov rbx,qword [rsp+80] ;yuv2rgb
	mov rdx,qword [rsp+88] ;rgbGammaCorr

	align 16
u2rlop:
	mov rbp,r8 ;width

	align 16
u2rlop2:
	mov eax,dword [rsi]
	movzx rcx,al
	movq xmm0,[rbx+rcx*8+4096]
	shr eax,8
	movzx rcx,al
	shr eax,8
	movq xmm1,[rbx+rcx*8+2048]
	paddsw xmm0,xmm1
	movzx rcx,al
	movq xmm1,[rbx+rcx*8]
	paddsw xmm0,xmm1

	pextrw rcx,xmm0,2
	mov al,byte [rdx+rcx]
	shl eax,16
	pextrw rcx,xmm0,0
	mov al,byte [rdx+rcx+131072]
	pextrw rcx,xmm0,1
	mov ah,byte [rdx+rcx+65536]
	mov dword [rdi],eax
	lea esi,[rsi+4]
	lea rdi,[rdi+4]
	dec rbp
	jnz u2rlop2

	add rdi,qword [rsp+72] ;dbpl
	dec r9 ;hleft
	jnz u2rlop

	pop rdi
	pop rsi
	pop rbx
	pop rbp
	ret
