section .text

global _CSAYUV_RGB8_do_ayuvrgb
global CSAYUV_RGB8_do_ayuvrgb

;void CSAYUV_RGB8_do_ayuvrgb(UInt8 *src, UInt8 *dest, IntOS width, IntOS height, IntOS dbpl, Int64 *yuv2rgb, UInt8 *rgbGammaCorr);
;0 edi
;4 esi
;8 ebx
;12 ebp
;16 retAddr
;20 src
;24 dest
;28 width
;32 height
;36 dbpl
;40 yuv2rgb
;44 rgbGammaCorr

	align 16
_CSAYUV_RGB8_do_ayuvrgb:
CSAYUV_RGB8_do_ayuvrgb:
	push ebp
	push ebx
	push esi
	push edi
	
	mov eax,dword [esp+28] ;width
	shl eax,2
	sub dword [esp+36],eax ;dbpl
	
	mov esi,dword [esp+20] ;src
	mov edi,dword [esp+24] ;dest
	mov ebx,dword [esp+40] ;yuv2rgb
	mov edx,dword [esp+44] ;rgbGammaCorr

	align 16
u2rlop:
	mov ebp,dword [esp+28] ;width

	align 16
u2rlop2:
	mov eax,dword [esi]
	movzx ecx,al
	movq xmm0,[ebx+ecx*8+4096]
	movzx ecx,ah
	shr eax,16
	movq xmm1,[ebx+ecx*8+2048]
	paddsw xmm0,xmm1
	movzx ecx,al
	movq xmm1,[ebx+ecx*8]
	paddsw xmm0,xmm1

	pextrw ecx,xmm0,2
	mov al,byte [edx+ecx]
	shl eax,16
	pextrw ecx,xmm0,0
	mov al,byte [edx+ecx+131072]
	pextrw ecx,xmm0,1
	mov ah,byte [edx+ecx+65536]
	mov dword [edi],eax
	lea esi,[esi+4]
	lea edi,[edi+4]
	dec ebp
	jnz u2rlop2

	add edi,dword [esp+36] ;dbpl
	dec dword [esp+32] ;hleft
	jnz u2rlop

	pop edi
	pop esi
	pop ebx
	pop ebp
	ret
