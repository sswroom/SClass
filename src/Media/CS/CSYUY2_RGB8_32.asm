section .text

global _CSYUY2_RGB8_do_yuy2rgb
global CSYUY2_RGB8_do_yuy2rgb

;void CSYUY2_RGB8_do_yuy2rgb(UInt8 *src, UInt8 *dest, OSInt width, OSInt height, OSInt dbpl, Int64 *yuv2rgb, UInt8 *rgbGammaCorr);
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
_CSYUY2_RGB8_do_yuy2rgb:
CSYUY2_RGB8_do_yuy2rgb:
	push ebp
	push ebx
	push esi
	push edi
	
	mov eax,dword [esp+28] ;width
	lea edx,[eax*4]
	shr eax,1
	sub eax,2
	mov dword [esp+28],eax ;width				OSInt wsize = (width >> 1) - 2;
	sub dword [esp+36],edx ;dbpl

	mov esi,dword [esp+20] ;src
	mov edi,dword [esp+24] ;dest
	mov ebx,dword [esp+40] ;yuv2rgb
	mov edx,dword [esp+44] ;rgbGammaCorr

	align 16
u2rlop:
	mov ebp,dword [esp+28] ;wsize

	movzx eax,byte [esi]
	movq xmm0,[ebx+eax*8]
	movzx eax,byte [esi+1]
	movq xmm2,[ebx+eax*8+2048]
	movzx eax,byte [esi+2]
	movq xmm1,[ebx+eax*8]
	movzx eax,byte [esi+3]
	movq xmm4,[ebx+eax*8+4096]
	paddsw xmm2,xmm4
	paddsw xmm0,xmm2
	psraw xmm2,1
	paddsw xmm1,xmm2

	pextrw eax,xmm0, 2
	movzx ecx,byte [edx+eax]
	or ecx,0xff00
	shl ecx,16
	pextrw eax,xmm0,1
	mov ch,byte [edx+eax+65536]
	pextrw eax,xmm0,2
	mov cl,byte [edx+eax+131072]
	mov dword [edi],ecx
	lea edi,[edi+8]
	lea esi,[esi+4]

	ALIGN 16
u2rlop2:
	movzx eax,byte [esi]
	movq xmm0,[ebx+eax*8]
	movzx eax,byte [esi+1]
	movq xmm2,[ebx+eax*8+2048]
	movzx eax,byte [esi+2]
	movq xmm4,[ebx+eax*8]
	movzx eax,byte [esi+3]
	movq xmm5,[ebx+eax*8+4096]
	paddsw xmm2,xmm5
	paddsw xmm0,xmm2
	psraw xmm2,1

	pextrw eax,xmm0,2
	movzx ecx,byte [edx+eax]
	paddsw xmm1,xmm2
	or ecx,0xff00
	shl ecx,16
	pextrw eax,xmm0,1
	mov ch,byte [edx+eax+65536]
	pextrw eax,xmm0,0
	mov cl,byte [edx+eax+131072]
	mov dword [edi],ecx

	pextrw eax,xmm1,2
	movzx ecx,byte [edx+eax]
	or ecx,0xff00
	shl ecx,16
	pextrw eax,xmm1,1
	mov ch,byte [edx+eax+65536]
	pextrw eax,xmm1,0
	mov cl,byte [edx+eax+131072]
	mov dword [edi-4],ecx

	movq xmm1,xmm4
	paddsw xmm1,xmm2

	lea esi,[esi+4]
	lea edi,[edi+8]
	dec ebp
	jnz u2rlop2

	movzx eax,byte [esi]
	movq xmm0,[ebx+eax*8]
	movzx eax,byte [esi+1]
	movq xmm2,[ebx+eax*8+2048]
	movzx eax,byte [esi+2]
	movq xmm4,[ebx+eax*8]
	movzx eax,byte [esi+3]
	movq xmm3,[ebx+eax*8+4096]
	paddsw xmm2,xmm3
	paddsw xmm0,xmm2
	paddsw xmm4,xmm2

	pextrw eax,xmm0,2
	movzx ecx,byte [edx+eax]
	or ecx,0xff00
	shl ecx,16
	pextrw eax,xmm0,1
	mov ch,byte [edx+eax+65536]
	pextrw eax,xmm0,0
	mov cl,byte [edx+eax+131072]
	mov dword [edi],ecx

	pextrw eax,xmm4,2
	movzx ecx,byte [edx+eax]
	or ecx,0xff00
	shl ecx,16
	pextrw eax,xmm4,1
	mov ch,byte [edx+eax+65536]
	pextrw eax,xmm4,0
	mov cl,byte [edx+eax+131072]
	mov dword [edi+4],ecx

	psraw xmm2,1
	paddsw xmm1,xmm2

	pextrw eax,xmm1,2
	movzx ecx,byte [edx+eax]
	or ecx,0xff00
	shl ecx,16
	pextrw eax,xmm1,1
	mov ch,byte [edx+eax+65536]
	pextrw eax,xmm1,0
	mov cl,byte [edx+eax+131072]
	mov dword [edi-4],ecx

	lea esi,[esi+4]
	lea edi,[edi+8]

	add edi,dword [esp+36] ;dbpl
	dec dword [esp+32] ;hleft
	jnz u2rlop

	pop edi
	pop esi
	pop ebx
	pop ebp
	ret
