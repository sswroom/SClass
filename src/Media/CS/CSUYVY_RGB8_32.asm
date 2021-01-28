section .text

global _CSUYVY_RGB8_do_uyvy2rgb
global CSUYVY_RGB8_do_uyvy2rgb

;void CSUYVY_RGB8_do_uyvy2rgb(UInt8 *src, UInt8 *dest, OSInt width, OSInt height, OSInt dbpl, Int64 *yuv2rgb, UInt8 *rgbGammaCorr);
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
_CSUYVY_RGB8_do_uyvy2rgb:
CSUYVY_RGB8_do_uyvy2rgb:
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

	movzx eax,byte [esi+1]
	movq xmm0,[ebx+eax*8]
	movzx eax,byte [esi+3]
	movq xmm1,[ebx+eax*8]
	movzx eax,byte [esi]
	movq xmm2,[ebx+eax*8+2048]
	movzx eax,byte [esi+2]
	movq xmm3,[ebx+eax*8+4096]
	paddsw xmm0,xmm2
	paddsw xmm0,xmm3
	psraw xmm2,1
	psraw xmm3,1
	paddsw xmm1,xmm2
	paddsw xmm1,xmm3
	
	pextrw eax,xmm0,2
	movzx ecx,byte [edx+eax]
	or ecx,0xff00
	shl ecx,16
	pextrw eax,xmm0,1
	mov ch,byte [edx+eax+65536]
	pextrw eax,xmm0,0
	mov cl,byte [edx+eax+131072]
	mov dword [edi],ecx
	lea edi,[edi+8]
	lea esi,[esi+4]
	align 16
u2rlop2:
	movzx eax,byte [esi+1]
	movq xmm0,[ebx+eax*8]
	movzx eax,byte [esi]
	movq xmm2,[ebx+eax*8+2048]
	movzx eax,byte [esi+2]
	movq xmm3,[ebx+eax*8+4096]
	paddsw xmm0,xmm2
	paddsw xmm0,xmm3

	pextrw eax,xmm0,2
	movzx ecx,byte [edx+eax]
	or ecx,0xff00
	shl ecx,16
	pextrw eax,xmm0,1
	mov ch,byte [edx+eax+65536]
	pextrw eax,xmm0,0
	mov cl,byte [edx+eax+131072]
	mov dword [edi],ecx

	psraw xmm2,1
	psraw xmm3,1
	paddsw xmm1,xmm2
	paddsw xmm1,xmm3

	pextrw eax,xmm1,2
	movzx ecx,byte [edx+eax]
	or ecx,0xff00
	shl ecx,16
	pextrw eax,xmm1,1
	mov ch,byte [edx+eax+65536]
	pextrw eax,xmm1,0
	mov cl,byte [edx+eax+131072]
	mov dword [edi-4],ecx

	movzx eax,byte [esi+3]
	movq xmm1,[ebx+eax*8]
	paddsw xmm1,xmm2
	paddsw xmm1,xmm3

	lea esi,[esi+4]
	lea edi,[edi+8]
	dec ebp
	jnz u2rlop2

	movzx eax,byte [esi+1]
	movq xmm0,[ebx+eax*8]
	movzx eax,byte [esi]
	movq xmm2,[ebx+eax*8+2048]
	movzx eax,byte [esi+2]
	movq xmm3,[ebx+eax*8+4096]
	paddsw xmm0,xmm2
	paddsw xmm0,xmm3

	pextrw eax,xmm0,2
	movzx ecx,byte [edx+eax]
	or ecx,0xff00
	shl ecx,16
	pextrw eax,xmm0,1
	mov ch,byte [edx+eax+65536]
	pextrw eax,xmm0,0
	mov cl,byte [edx+eax+131072]
	mov dword [edi],ecx

	movzx eax,byte [esi+3]
	movq xmm0,[ebx+eax*8]
	paddsw xmm0,xmm2
	paddsw xmm0,xmm3

	pextrw eax,xmm0,2
	movzx ecx,byte [edx+eax]
	or ecx,0xff00
	shl ecx,16
	pextrw eax,xmm0,1
	mov ch,byte [edx+eax+65536]
	pextrw eax,xmm0,0
	mov cl,byte [edx+eax+131072]
	mov dword [edi+4],ecx

	psraw xmm2,1
	psraw xmm3,1
	paddsw xmm1,xmm2
	paddsw xmm1,xmm3

	pextrw eax,xmm1,2
	movzx ecx,byte [edx+eax]
	or ecx,0xff00
	shl ecx,16
	pextrw eax,xmm1,1
	mov ch,byte [edx+eax+65536]
	pextrw eax,xmm1,0
	mov cl,byte [edx+eax+131072]
	mov dword [edi-4],ecx
	
	lea edi,[edi+8]
	lea esi,[esi+4]
	add edi,dword [esp+36] ;dbpl
	dec dword [esp+32] ;hleft
	jnz u2rlop

	pop edi
	pop esi
	pop ebx
	pop ebp
	ret
