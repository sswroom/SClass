section .text

global _CSAYUV444_10_LRGBC_convert
global CSAYUV444_10_LRGBC_convert

;void CSAYUV444_10_LRGBC_convert(UInt8 *yPtr, UInt8 *dest, OSInt width, OSInt height, OSInt dbpl, OSInt yBpl, Int64 *yuv2rgb, Int64 *rgbGammaCorr);
; 0 edi
; 4 esi
; 8 ebx
; 12 ebp
; 16 retAddr
; 20 yPtr
; 24 dest
; 28 width
; 32 height
; 36 dbpl
; 40 yBpl
; 44 yuv2rgb
; 48 rgbGammaCorr

	align 16
_CSAYUV444_10_LRGBC_convert:
CSAYUV444_10_LRGBC_convert:
	push ebp
	push ebx
	push esi
	push edi
	mov ecx,dword [esp+28] ;width
	lea eax,[ecx * 8]
	sub dword [esp+36],eax ;dbpl
	sub dword [esp+40],eax ;yBpl
	shr ecx,1
	mov dword [esp+28],ecx ;width
	mov ecx,dword [esp+20] ;yPtr
	mov edx,dword [esp+24] ;dest
	mov esi,dword [esp+44] ;yuv2rgb
	mov edi,dword [esp+48] ;rgbGammaCorr
	align 16
convlop:
	mov ebp,dword [esp+28] ;width
	align 16
convlop2:
	movdqu xmm0,[ecx]
	pextrw eax,xmm0,1 ;y
	movq xmm1,[esi+eax*8]
	pextrw ebx,xmm0,5 ;y
	movq xmm2,[esi+ebx*8]
	punpcklqdq xmm1,xmm2
	pextrw eax,xmm0,0 ;u
	movq xmm2,[esi+eax*8+524288]
	pextrw ebx,xmm0,4 ;u
	movq xmm3,[esi+ebx*8+524288]
	punpcklqdq xmm2,xmm3
	paddsw xmm1,xmm2
	pextrw eax,xmm0,2 ;v
	movq xmm2,[esi+eax*8+1048576]
	pextrw ebx,xmm0,6 ;v
	movq xmm3,[esi+ebx*8+1048576]
	punpcklqdq xmm2,xmm3
	paddsw xmm1,xmm2
	
	pextrw eax,xmm1,2 ;r
	movq xmm0,[edi+eax*8]
	pextrw ebx,xmm1,6 ;r
	movq xmm2,[edi+ebx*8]
	punpcklqdq xmm0,xmm2
	pextrw eax,xmm1,1 ;g
	movq xmm2,[edi+eax*8+524288]
	pextrw ebx,xmm1,5 ;g
	movq xmm3,[edi+ebx*8+524288]
	punpcklqdq xmm2,xmm3
	paddsw xmm0,xmm2
	pextrw eax,xmm1,0 ;b
	movq xmm2,[edi+eax*8+1048576]
	pextrw ebx,xmm1,4 ;b
	movq xmm3,[edi+ebx*8+1048576]
	punpcklqdq xmm2,xmm3
	paddsw xmm0,xmm2
	movntdq [edx],xmm0
	
	lea ecx,[ecx+16]
	lea edx,[edx+16]
	dec ebp
	jnz convlop2
	
	add ecx,dword [esp+40] ;yBpl
	add edx,dword [esp+36] ;dbpl
	dec dword [esp+32] ;height
	jnz convlop
	
	pop edi
	pop esi
	pop ebx
	pop ebp
	ret
