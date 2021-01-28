section .text

global _CSUYVY_LRGBC_do_yuy2rgb
global CSUYVY_LRGBC_do_yuy2rgb

;void CSUYVY_LRGBC_do_yuy2rgb(UInt8 *src, UInt8 *dest, OSInt width, OSInt height, OSInt dbpl, Int64 *yuv2rgb, Int64 *rgbGammaCorr);
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
_CSUYVY_LRGBC_do_yuy2rgb:
CSUYVY_LRGBC_do_yuy2rgb:
	push ebp
	push ebx
	push esi
	push edi
	
	mov eax,dword [esp+28] ;width
	lea edx,[eax*8]
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

	movzx eax,byte [esi+0]
	movq xmm2,[ebx+eax*8+2048]
	movzx eax,byte [esi+1]
	movq xmm0,[ebx+eax*8]
	movzx eax,byte [esi+2]
	movq xmm3,[ebx+eax*8+4096]
	movzx eax,byte [esi+3]
	movq xmm1,[ebx+eax*8]
	paddsw xmm2,xmm3
	paddsw xmm0,xmm2
	psraw xmm2,1
	paddsw xmm1,xmm2
	
	movd eax,xmm0
	movzx ecx,ax
	movq xmm5,[edx+ecx*8+1048576]
	shr eax,16
	movq xmm6,[edx+eax*8+524288]
	paddsw xmm5,xmm6
	pextrw eax,xmm0,2
	movq xmm6,[edx+eax*8]
	paddsw xmm5,xmm6
	movq [edi],xmm5

	lea edi,[edi+16]
	lea esi,[esi+4]
	align 16
u2rlop2:
	movzx eax,byte [esi+0]
	movq xmm2,[ebx+eax*8+2048]
	movzx eax,byte [esi+1]
	movq xmm0,[ebx+eax*8]
	movzx eax,byte [esi+2]
	movq xmm3,[ebx+eax*8+4096]
	paddsw xmm2,xmm3
	paddsw xmm0,xmm2

	movd eax,xmm0
	movzx ecx,ax
	movq xmm5,[edx+ecx*8+1048576]
	shr eax,16
	movq xmm6,[edx+eax*8+524288]
	paddsw xmm5,xmm6
	pextrw eax,xmm0,2
	movq xmm6,[edx+eax*8]
	paddsw xmm5,xmm6
	movq [edi],xmm5

	psraw xmm2,1
	paddsw xmm1,xmm2

	movd eax,xmm1
	movzx ecx,ax
	movq xmm5,[edx+ecx*8+1048576]
	shr eax,16
	movq xmm6,[edx+eax*8+524288]
	paddsw xmm5,xmm6
	pextrw eax,xmm1,2
	movq xmm6,[edx+eax*8]
	paddsw xmm5,xmm6
	movq [edi-8],xmm5

	movzx eax,byte [esi+3]
	movq xmm1,[ebx+eax*8]
	paddsw xmm1,xmm2

	lea esi,[esi+4]
	lea edi,[edi+16]
	dec ebp
	jnz u2rlop2

	movzx eax,byte [esi+0]
	movq xmm2,[ebx+eax*8+2048]
	movzx eax,byte [esi+1]
	movq xmm0,[ebx+eax*8]
	movzx eax,byte [esi+2]
	movq xmm3,[ebx+eax*8+4096]
	paddsw xmm2,xmm3
	paddsw xmm0,xmm2

	movd eax,xmm0
	movzx ecx,ax
	movq xmm5,[edx+ecx*8+1048576]
	shr eax,16
	movq xmm6,[edx+eax*8+524288]
	paddsw xmm5,xmm6
	pextrw eax,xmm0,2
	movq xmm6,[edx+eax*8]
	paddsw xmm5,xmm6
	movq [edi],xmm5

	movzx eax,byte [esi+3]
	movq xmm0,[ebx+eax*8]
	paddsw xmm0,xmm2

	movd eax,xmm0
	movzx ecx,ax
	movq xmm5,[edx+ecx*8+1048576]
	shr eax,16
	movq xmm6,[edx+eax*8+524288]
	paddsw xmm5,xmm6
	pextrw eax,xmm0,2
	movq xmm6,[edx+eax*8]
	paddsw xmm5,xmm6
	movq [edi+8],xmm5

	psraw xmm2,1
	paddsw xmm1,xmm2

	movd eax,xmm1
	movzx ecx,ax
	movq xmm5,[edx+ecx*8+1048576]
	shr eax,16
	movq xmm6,[edx+eax*8+524288]
	paddsw xmm5,xmm6
	pextrw eax,xmm1,2
	movq xmm6,[edx+eax*8]
	paddsw xmm5,xmm6
	movq [edi-8],xmm5

	lea esi,[esi+4]
	lea edi,[edi+16]

	add edi,dword [esp+36] ;dbpl
	dec dword [esp+32] ;hleft
	jnz u2rlop

	pop edi
	pop esi
	pop ebx
	pop ebp
	ret
