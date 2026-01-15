section .text

global _CSRGBF_LRGBC_Convert
global CSRGBF_LRGBC_Convert

;void CSRGBF_LRGBC_Convert(UInt8 *srcPtr, UInt8 *destPtr, IntOS width, IntOS height, IntOS srcNBits, IntOS srcRGBBpl, IntOS destRGBBpl, UInt8 *rgbTable);
;0 edi
;4 esi
;8 ebx
;12 ebp
;16 retAddr
;20 srcPtr
;24 destPtr
;28 width
;32 height
;36 srcNBits
;40 srcRGBBpl
;44 destRGBBpl
;48 rgbTable
	align 16
_CSRGBF_LRGBC_Convert:
CSRGBF_LRGBC_Convert:
	push ebp
	push ebx
	push esi
	push edi
	mov eax,dword [esp+36] ;srcNBits
	mov edx,dword [esp+28] ;width
	cmp eax,128
	jz cargbf32start
	cmp eax,96
	jz crgbf32start
	cmp eax,64
	jz cawf32start
	cmp eax,32
	jz cwf32start
	jmp crgbexit
	
	align 16
cargbf32start:
	lea ecx,[edx*8]
	lea eax,[ecx*2]
	sub dword [esp+40],eax ;srcRGBBpl
	sub dword [esp+44],ecx ;destRGBBpl

	mov eax,32767
	cvtsi2ss xmm4,eax
	unpcklps xmm4,xmm4
	unpcklps xmm4,xmm4
	pxor xmm3,xmm3
	
	mov esi,dword [esp+20] ;srcPtr
	mov edi,dword [esp+24] ;destPtr
	mov ebp,dword [esp+32] ;height
	mov ebx,dword [esp+48] ;rgbTable
	align 16
cargbf32lop:
	mov ecx,dword [esp+28] ;width
	align 16
cargbf32lop2:
	movaps xmm0,[esi]
	mulps xmm0,xmm4
	cvtps2dq xmm2,xmm0
	packssdw xmm2,xmm3

	pextrw eax,xmm2,0
	movq xmm1,[ebx+eax*8+1048576]
	pextrw eax,xmm2,1
	movq xmm0,[ebx+eax*8+524288]
	paddsw xmm1,xmm0
	pextrw eax,xmm2,2
	movq xmm0, [ebx+eax*8]
	paddsw xmm1,xmm0
	movq [edi],xmm1
	lea esi,[esi+16]
	lea edi,[edi+8]
	dec ecx
	jnz cargbf32lop2
	add esi,dword [esp+40] ;srcRGBBpl
	add edi,dword [esp+44] ;destRGBBpl
	dec ebp
	jnz cargbf32lop
	jmp crgbexit

	align 16
crgbf32start:
	lea ecx,[edx*4]
	lea eax,[ecx*2+ecx]
	lea ecx,[ecx*2]
	sub dword [esp+40],eax ;srcRGBBpl
	sub dword [esp+44],ecx ;destRGBBpl

	mov eax,32767
	cvtsi2ss xmm4,eax
	unpcklps xmm4,xmm4
	unpcklps xmm4,xmm4
	pxor xmm3,xmm3
	
	mov esi,dword [esp+20] ;srcPtr
	mov edi,dword [esp+24] ;destPtr
	mov ebp,dword [esp+32] ;height
	mov ebx,dword [esp+48] ;rgbTable
	align 16
crgbf32lop:
	mov ecx,dword [esp+28] ;width
	align 16
crgbf32lop2:
	movq xmm0,[esi]
	movss xmm5,[esi+8]
	punpcklqdq xmm0,xmm5
	mulps xmm0,xmm4
	cvtps2dq xmm2,xmm0
	packssdw xmm2,xmm3

	pextrw eax,xmm2,0
	movq xmm1,[ebx+eax*8+1048576]
	pextrw eax,xmm2,1
	movq xmm0,[ebx+eax*8+524288]
	paddsw xmm1,xmm0
	pextrw eax,xmm2,2
	movq xmm0, [ebx+eax*8]
	paddsw xmm1,xmm0
	movq [edi],xmm1
	lea esi,[esi+12]
	lea edi,[edi+8]
	dec ecx
	jnz crgbf32lop2
	add esi,dword [esp+40] ;srcRGBBpl
	add edi,dword [esp+44] ;destRGBBpl
	dec ebp
	jnz crgbf32lop
	jmp crgbexit

	align 16
cawf32start:
	lea ecx,[edx*8]
	sub dword [esp+40],ecx ;srcRGBBpl
	sub dword [esp+44],ecx ;destRGBBpl

	mov eax,32767
	cvtsi2ss xmm4,eax
	unpcklps xmm4,xmm4
	unpcklps xmm4,xmm4
	pxor xmm3,xmm3
	
	mov esi,dword [esp+20] ;srcPtr
	mov edi,dword [esp+24] ;destPtr
	mov ebp,dword [esp+32] ;height
	mov ebx,dword [esp+48] ;rgbTable
	align 16
cawf32lop:
	mov ecx,dword [esp+28] ;width
	align 16
cawf32lop2:
	movq xmm0,[esi]
	mulps xmm0,xmm4
	cvtps2dq xmm2,xmm0
	packssdw xmm2,xmm3

	pextrw eax,xmm2,0
	movq xmm1,[ebx+eax*8+1048576]
	movq xmm0,[ebx+eax*8+524288]
	paddsw xmm1,xmm0
	movq xmm0, [ebx+eax*8]
	paddsw xmm1,xmm0
	movq [edi],xmm1
	lea esi,[esi+8]
	lea edi,[edi+8]
	dec ecx
	jnz cawf32lop2
	add esi,dword [esp+40] ;srcRGBBpl
	add edi,dword [esp+44] ;destRGBBpl
	dec ebp
	jnz cawf32lop
	jmp crgbexit

	align 16
cwf32start:
	lea ecx,[edx*8]
	lea eax,[edx*4]
	sub dword [esp+40],eax ;srcRGBBpl
	sub dword [esp+44],ecx ;destRGBBpl

	mov eax,32767
	cvtsi2ss xmm4,eax
	unpcklps xmm4,xmm4
	unpcklps xmm4,xmm4
	pxor xmm3,xmm3
	
	mov esi,dword [esp+20] ;srcPtr
	mov edi,dword [esp+24] ;destPtr
	mov ebp,dword [esp+32] ;height
	mov ebx,dword [esp+48] ;rgbTable
	align 16
cwf32lop:
	mov ecx,dword [esp+28] ;width
	align 16
cwf32lop2:
	movss xmm0,[esi]
	mulps xmm0,xmm4
	cvtps2dq xmm2,xmm0
	packssdw xmm2,xmm3

	pextrw eax,xmm2,0
	movq xmm1,[ebx+eax*8+1048576]
	movq xmm0,[ebx+eax*8+524288]
	paddsw xmm1,xmm0
	movq xmm0, [ebx+eax*8]
	paddsw xmm1,xmm0
	movq [edi],xmm1
	lea esi,[esi+4]
	lea edi,[edi+8]
	dec ecx
	jnz cwf32lop2
	add esi,dword [esp+40] ;srcRGBBpl
	add edi,dword [esp+44] ;destRGBBpl
	dec ebp
	jnz cwf32lop
	jmp crgbexit

	align 16
crgbexit:
	pop edi
	pop esi
	pop ebx
	pop ebp
	ret
