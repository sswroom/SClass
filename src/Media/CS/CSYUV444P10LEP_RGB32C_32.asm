section .text

global _CSYUV444P10LEP_RGB32C_convert
global CSYUV444P10LEP_RGB32C_convert

;void CSAYUV444_10_RGB32C_convert(UInt8 *yPtr, UInt8 *uPtr, UInt8 *vPtr, UInt8 *dest, OSInt width, OSInt height, OSInt dbpl, OSInt yBpl, Int64 *yuv2rgb, Int64 *rgbGammaCorr);
; 0 edi
; 4 esi
; 8 ebx
; 12 ebp
; 16 retAddr
; 20 yPtr
; 24 uPtr
; 28 vPtr
; 32 dest
; 36 width
; 40 height
; 44 dbpl
; 48 yBpl
; 52 yuv2rgb
; 56 rgbGammaCorr

	align 16
_CSYUV444P10LEP_RGB32C_convert:
CSYUV444P10LEP_RGB32C_convert:
	push ebp
	push ebx
	push esi
	push edi
	mov ecx,dword [esp+36] ;width
	lea eax,[ecx * 2]
	lea ebx,[ecx * 4]
	sub dword [esp+44],ebx ;dbpl
	sub dword [esp+48],eax ;yBpl
	shr ecx,3
	mov dword [esp+36],ecx ;width
	mov edx,dword [esp+32] ;dest
	mov esi,dword [esp+52] ;yuv2rgb
	mov edi,dword [esp+56] ;rgbGammaCorr
	align 16
convlop:
	mov ebp,dword [esp+36] ;width
	align 16
convlop2:
	mov ecx,dword [esp+20] ;yPtr
	movdqu xmm0,[ecx]
	lea ecx,[ecx+16]
	mov dword [esp+20],ecx ;yPtr
	mov ecx,dword [esp+24] ;uPtr
	movdqu xmm4,[ecx]
	lea ecx,[ecx+16]
	mov dword [esp+24],ecx ;uPtr
	mov ecx,dword [esp+28] ;vPtr
	movdqu xmm5,[ecx]
	lea ecx,[ecx+16]
	mov dword [esp+28],ecx ;vPtr

	pextrw eax,xmm0,0 ;y
	movq xmm1,[esi+eax*8]
	pextrw ebx,xmm0,1 ;y
	movhpd xmm1,[esi+ebx*8]
	pextrw eax,xmm4,0 ;u
	movq xmm2,[esi+eax*8+524288]
	pextrw ebx,xmm4,1 ;u
	movhpd xmm2,[esi+ebx*8+524288]
	paddsw xmm1,xmm2
	pextrw eax,xmm5,0 ;v
	movq xmm2,[esi+eax*8+1048576]
	pextrw ebx,xmm5,1 ;v
	movhpd xmm2,[esi+ebx*8+1048576]
	paddsw xmm1,xmm2
	
	pextrw eax,xmm1,2 ;r
	movq xmm3,[edi+eax*8]
	pextrw ebx,xmm1,6 ;r
	movhpd xmm3,[edi+ebx*8]
	pextrw eax,xmm1,1 ;g
	movq xmm2,[edi+eax*8+524288]
	pextrw ebx,xmm1,5 ;g
	movhpd xmm2,[edi+ebx*8+524288]
	paddsw xmm3,xmm2
	pextrw eax,xmm1,0 ;b
	movq xmm2,[edi+eax*8+1048576]
	pextrw ebx,xmm1,4 ;b
	movhpd xmm2,[edi+ebx*8+1048576]
	paddsw xmm3,xmm2
	
	pextrw eax,xmm3,0
	mov ebx,dword [edi+eax*4+1572864]
	pextrw eax,xmm3,1
	or ebx,dword [edi+eax*4+1835008]
	pextrw eax,xmm3,2
	or ebx,dword [edi+eax*4+2097152]
	movnti dword [edx],ebx

	pextrw eax,xmm3,4
	mov ebx,dword [edi+eax*4+1572864]
	pextrw eax,xmm3,5
	or ebx,dword [edi+eax*4+1835008]
	pextrw eax,xmm3,6
	or ebx,dword [edi+eax*4+2097152]
	movnti dword [edx+4],ebx
	
	pextrw eax,xmm0,2 ;y
	movq xmm1,[esi+eax*8]
	pextrw ebx,xmm0,3 ;y
	movhpd xmm1,[esi+ebx*8]
	pextrw eax,xmm4,2 ;u
	movq xmm2,[esi+eax*8+524288]
	pextrw ebx,xmm4,3 ;u
	movhpd xmm2,[esi+ebx*8+524288]
	paddsw xmm1,xmm2
	pextrw eax,xmm5,2 ;v
	movq xmm2,[esi+eax*8+1048576]
	pextrw ebx,xmm5,3 ;v
	movhpd xmm2,[esi+ebx*8+1048576]
	paddsw xmm1,xmm2
	
	pextrw eax,xmm1,2 ;r
	movq xmm3,[edi+eax*8]
	pextrw ebx,xmm1,6 ;r
	movhpd xmm3,[edi+ebx*8]
	pextrw eax,xmm1,1 ;g
	movq xmm2,[edi+eax*8+524288]
	pextrw ebx,xmm1,5 ;g
	movhpd xmm2,[edi+ebx*8+524288]
	paddsw xmm3,xmm2
	pextrw eax,xmm1,0 ;b
	movq xmm2,[edi+eax*8+1048576]
	pextrw ebx,xmm1,4 ;b
	movhpd xmm2,[edi+ebx*8+1048576]
	paddsw xmm3,xmm2
	
	pextrw eax,xmm3,0
	mov ebx,dword [edi+eax*4+1572864]
	pextrw eax,xmm3,1
	or ebx,dword [edi+eax*4+1835008]
	pextrw eax,xmm3,2
	or ebx,dword [edi+eax*4+2097152]
	movnti dword [edx+8],ebx

	pextrw eax,xmm3,4
	mov ebx,dword [edi+eax*4+1572864]
	pextrw eax,xmm3,5
	or ebx,dword [edi+eax*4+1835008]
	pextrw eax,xmm3,6
	or ebx,dword [edi+eax*4+2097152]
	movnti dword [edx+12],ebx

	pextrw eax,xmm0,4 ;y
	movq xmm1,[esi+eax*8]
	pextrw ebx,xmm0,5 ;y
	movhpd xmm1,[esi+ebx*8]
	pextrw eax,xmm4,4 ;u
	movq xmm2,[esi+eax*8+524288]
	pextrw ebx,xmm4,5 ;u
	movhpd xmm2,[esi+ebx*8+524288]
	paddsw xmm1,xmm2
	pextrw eax,xmm5,4 ;v
	movq xmm2,[esi+eax*8+1048576]
	pextrw ebx,xmm5,5 ;v
	movhpd xmm2,[esi+ebx*8+1048576]
	paddsw xmm1,xmm2
	
	pextrw eax,xmm1,2 ;r
	movq xmm3,[edi+eax*8]
	pextrw ebx,xmm1,6 ;r
	movhpd xmm3,[edi+ebx*8]
	pextrw eax,xmm1,1 ;g
	movq xmm2,[edi+eax*8+524288]
	pextrw ebx,xmm1,5 ;g
	movhpd xmm2,[edi+ebx*8+524288]
	paddsw xmm3,xmm2
	pextrw eax,xmm1,0 ;b
	movq xmm2,[edi+eax*8+1048576]
	pextrw ebx,xmm1,4 ;b
	movhpd xmm2,[edi+ebx*8+1048576]
	paddsw xmm3,xmm2
	
	pextrw eax,xmm3,0
	mov ebx,dword [edi+eax*4+1572864]
	pextrw eax,xmm3,1
	or ebx,dword [edi+eax*4+1835008]
	pextrw eax,xmm3,2
	or ebx,dword [edi+eax*4+2097152]
	movnti dword [edx+16],ebx

	pextrw eax,xmm3,4
	mov ebx,dword [edi+eax*4+1572864]
	pextrw eax,xmm3,5
	or ebx,dword [edi+eax*4+1835008]
	pextrw eax,xmm3,6
	or ebx,dword [edi+eax*4+2097152]
	movnti dword [edx+20],ebx

	pextrw eax,xmm0,6 ;y
	movq xmm1,[esi+eax*8]
	pextrw ebx,xmm0,7 ;y
	movhpd xmm1,[esi+ebx*8]
	pextrw eax,xmm4,6 ;u
	movq xmm2,[esi+eax*8+524288]
	pextrw ebx,xmm4,7 ;u
	movhpd xmm2,[esi+ebx*8+524288]
	paddsw xmm1,xmm2
	pextrw eax,xmm5,6 ;v
	movq xmm2,[esi+eax*8+1048576]
	pextrw ebx,xmm5,7 ;v
	movhpd xmm2,[esi+ebx*8+1048576]
	paddsw xmm1,xmm2
	
	pextrw eax,xmm1,2 ;r
	movq xmm3,[edi+eax*8]
	pextrw ebx,xmm1,6 ;r
	movhpd xmm3,[edi+ebx*8]
	pextrw eax,xmm1,1 ;g
	movq xmm2,[edi+eax*8+524288]
	pextrw ebx,xmm1,5 ;g
	movhpd xmm2,[edi+ebx*8+524288]
	paddsw xmm3,xmm2
	pextrw eax,xmm1,0 ;b
	movq xmm2,[edi+eax*8+1048576]
	pextrw ebx,xmm1,4 ;b
	movhpd xmm2,[edi+ebx*8+1048576]
	paddsw xmm3,xmm2
	
	pextrw eax,xmm3,0
	mov ebx,dword [edi+eax*4+1572864]
	pextrw eax,xmm3,1
	or ebx,dword [edi+eax*4+1835008]
	pextrw eax,xmm3,2
	or ebx,dword [edi+eax*4+2097152]
	movnti dword [edx+24],ebx

	pextrw eax,xmm3,4
	mov ebx,dword [edi+eax*4+1572864]
	pextrw eax,xmm3,5
	or ebx,dword [edi+eax*4+1835008]
	pextrw eax,xmm3,6
	or ebx,dword [edi+eax*4+2097152]
	movnti dword [edx+28],ebx

	lea edx,[edx+32]
	dec ebp
	jnz convlop2
	
	mov eax,dword [esp+48] ;yBpl
	add edx,dword [esp+44] ;dbpl
	add dword [esp+20],eax ;yPtr
	add dword [esp+24],eax ;uPtr
	add dword [esp+28],eax ;vPtr
	dec dword [esp+40] ;height
	jnz convlop
	
	pop edi
	pop esi
	pop ebx
	pop ebp
	ret
