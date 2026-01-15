section .text

global _CSP216_LRGBC_do_yuy2rgb
global CSP216_LRGBC_do_yuy2rgb

;void CSP216_LRGBC_do_yuy2rgb(UInt8 *srcY, UInt8 *srcUV, UInt8 *dest, IntOS width, IntOS height, IntOS dbpl, Int64 *yuv2rgb, Int64 *rgbGammaCorr);
;0 edi
;4 esi
;8 ebx
;12 ebp
;16 retAddr
;20 srcY
;24 srcUV
;28 dest
;32 width
;36 height
;40 dbpl
;44 yuv2rgb
;48 rgbGammaCorr
	align 16
_CSP216_LRGBC_do_yuy2rgb:
CSP216_LRGBC_do_yuy2rgb:
	push ebp
	push ebx
	push esi
	push edi
	mov eax,dword [esp+32]
	lea edx,[eax*8]
	shr eax,1
	sub eax,2
	mov dword [esp+32],eax ;Int32 wsize = (width >> 1) - 2;
	sub dword [esp+40],edx ;dbpl

	mov esi,dword [esp+20] ;srcY
	mov ecx,dword [esp+24] ;srcUV
	mov edi,dword [esp+28] ;dest
	mov ebx,dword [esp+44] ;yuv2rgb
	mov edx,dword [esp+48] ;rgbGammaCorr

	align 16
u2rlop:
	mov ebp,dword [esp+32] ;wsize

	movzx eax,word [esi+0]
	movq xmm0,[ebx+eax*8]
	movzx eax,word [ecx+0]
	movq xmm2,[ebx+eax*8+524288]
	movzx eax,word [esi+2]
	movq xmm1,[ebx+eax*8]
	movzx eax,word [ecx+2]
	movq xmm3,[ebx+eax*8+1048576]
	paddsw xmm2,xmm3
	paddsw xmm0,xmm2
	psraw xmm2,1
	paddsw xmm1,xmm2
	
	pextrw eax,xmm0,0
	movq xmm5,[edx+eax*8+1048576]
	pextrw eax,xmm0,1
	movq xmm6,[edx+eax*8+524288]
	paddsw xmm5,xmm6
	pextrw eax,xmm0,2
	movq xmm6,[edx+eax*8]
	paddsw xmm5,xmm6
	movq [edi],xmm5

	lea edi,[edi+16]
	lea ecx,[ecx+4]
	lea esi,[esi+4]
	align 16
u2rlop2:
	movzx eax,word [esi+0]
	movq xmm0,[ebx+eax*8]
	movzx eax,word [ecx+0]
	movq xmm2,[ebx+eax*8+524288]
	movzx eax,word [ecx+2]
	movq xmm3,[ebx+eax*8+1048576]
	paddsw xmm2,xmm3
	paddsw xmm0,xmm2

	pextrw eax,xmm0,0
	movq xmm5,[edx+eax*8+1048576]
	pextrw eax,xmm0,1
	movq xmm6,[edx+eax*8+524288]
	paddsw xmm5,xmm6
	pextrw eax,xmm0,2
	movq xmm6,[edx+eax*8]
	paddsw xmm5,xmm6
	movq [edi],xmm5

	psraw xmm2,1
	paddsw xmm1,xmm2

	pextrw eax,xmm1,0
	movq xmm5,[edx+eax*8+1048576]
	pextrw eax,xmm1,1
	movq xmm6,[edx+eax*8+524288]
	paddsw xmm5,xmm6
	pextrw eax,xmm1,2
	movq xmm6,[edx+eax*8]
	paddsw xmm5,xmm6
	movq [edi-8],xmm5

	movzx eax,word [esi+2]
	movq xmm1,[ebx+eax*8]
	paddsw xmm1,xmm2

	lea ecx,[ecx+4]
	lea esi,[esi+4]
	lea edi,[edi+16]
	dec ebp
	jnz u2rlop2

	movzx eax,word [esi+0]
	movq xmm0,[ebx+eax*8]
	movzx eax,word [ecx+0]
	movq xmm2,[ebx+eax*8+524288]
	movzx eax,word [ecx+2]
	movq xmm3,[ebx+eax*8+1048576]
	paddsw xmm2,xmm3
	paddsw xmm0,xmm2

	pextrw eax,xmm0,0
	movq xmm5,[edx+eax*8+1048576]
	pextrw eax,xmm0,1
	movq xmm6,[edx+eax*8+524288]
	paddsw xmm5,xmm6
	pextrw eax,xmm0,2
	movq xmm6,[edx+eax*8]
	paddsw xmm5,xmm6
	movq [edi],xmm5

	movzx eax,word [esi+2]
	movq xmm0,[ebx+eax*8]
	paddsw xmm0,xmm2

	pextrw eax,xmm0,0
	movq xmm5,[edx+eax*8+1048576]
	pextrw eax,xmm0,1
	movq xmm6,[edx+eax*8+524288]
	paddsw xmm5,xmm6
	pextrw eax,xmm0,2
	movq xmm6,[edx+eax*8]
	paddsw xmm5,xmm6
	movq [edi+8],xmm5

	psraw xmm2,1
	paddsw xmm1,xmm2

	pextrw eax,xmm1,0
	movq xmm5,[edx+eax*8+1048576]
	pextrw eax,xmm1,1
	movq xmm6,[edx+eax*8+524288]
	paddsw xmm5,xmm6
	pextrw eax,xmm1,2
	movq xmm6,[edx+eax*8]
	paddsw xmm5,xmm6
	movq [edi],xmm5

	lea ecx,[ecx+4]
	lea esi,[esi+4]
	lea edi,[edi+8]

	add edi,dword [esp+40] ;dbpl
	dec dword [esp+36] ;hleft
	jnz u2rlop

	pop edi
	pop esi
	pop ebx
	pop ebp
	ret
	
