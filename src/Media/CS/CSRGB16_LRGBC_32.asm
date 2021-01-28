section .text

global _CSRGB16_LRGBC_Convert
global CSRGB16_LRGBC_Convert
global _CSRGB16_LRGBC_ConvertA2B10G10R10
global CSRGB16_LRGBC_ConvertA2B10G10R10

;void CSRGB16_LRGBC_Convert(UInt8 *srcPtr, UInt8 *destPtr, OSInt width, OSInt height, OSInt srcNBits, OSInt srcRGBBpl, OSInt destRGBBpl, UInt8 *rgbTable);
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
_CSRGB16_LRGBC_Convert:
CSRGB16_LRGBC_Convert:
	push ebp
	push ebx
	push esi
	push edi
	mov eax,dword [esp+36] ;srcNBits
	mov edx,dword [esp+28] ;width
	cmp eax,64
	jz crgb64start
	cmp eax,48
	jz crgb48start
	cmp eax,32
	jz crgb32start
	cmp eax,16
	jz crgb16start
	jmp crgbexit
	
;void CSRGB16_LRGBC_ConvertA2B10G10R10(UInt8 *srcPtr, UInt8 *destPtr, OSInt width, OSInt height, OSInt srcNBits, OSInt srcRGBBpl, OSInt destRGBBpl, UInt8 *rgbTable);
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
_CSRGB16_LRGBC_ConvertA2B10G10R10:
CSRGB16_LRGBC_ConvertA2B10G10R10:
	push ebp
	push ebx
	push esi
	push edi
	mov eax,dword [esp+36] ;srcNBits
	mov edx,dword [esp+28] ;width
	cmp eax,32
	jz ca2b10g10r10start
	jmp crgbexit
	
	align 16
crgb64start:
	lea ecx,[edx*8]
	sub dword [esp+40],ecx ;srcRGBBpl
	sub dword [esp+44],ecx ;destRGBBpl
	
	mov esi,dword [esp+20] ;srcPtr
	mov edi,dword [esp+24] ;destPtr
	mov ebp,dword [esp+32] ;height
	mov ebx,dword [esp+48] ;rgbTable
	align 16
crgb64lop:
	mov ecx,dword [esp+28] ;width
	align 16
crgb64lop2:
	movzx eax,word [esi]
	movq xmm1,[ebx+eax*8+1048576]
	movzx eax,word [esi+2]
	movq xmm0,[ebx+eax*8+524288]
	paddsw xmm1,xmm0
	movzx eax,word [esi+4]
	movq xmm0, [ebx+eax*8]
	paddsw xmm1,xmm0
	movq [edi],xmm1
	lea edi,[edi+8]
	lea esi,[esi+8]
	dec ecx
	jnz crgb64lop2
	add esi,dword [esp+40] ;srcRGBBpl
	add edi,dword [esp+44] ;destRGBBpl
	dec ebp
	jnz crgb64lop
	jmp crgbexit
	
	align 16
crgb32start:
	lea eax,[edx*4]
	lea ecx,[edx*8]
	sub dword [esp+40],eax ;srcRGBBpl
	sub dword [esp+44],ecx ;destRGBBpl
	
	mov esi,dword [esp+20] ;srcPtr
	mov edi,dword [esp+24] ;destPtr
	mov ebp,dword [esp+32] ;height
	mov ebx,dword [esp+48] ;rgbTable
	align 16
crgb32lop:
	mov ecx,dword [esp+28] ;width
	align 16
crgb32lop2:
	movzx eax,word [esi]
	movq xmm1,[ebx+eax*8+1048576]
	movq xmm0,[ebx+eax*8+524288]
	paddsw xmm1,xmm0
	movq xmm2, [ebx+eax*8]
	paddsw xmm1,xmm2
	movq [edi],xmm1
	lea esi,[esi+4]
	lea edi,[edi+8]
	dec ecx
	jnz crgb32lop2
	add esi,dword [esp+40] ;srcRGBBpl
	add edi,dword [esp+44] ;destRGBBpl
	dec ebp
	jnz crgb32lop
	jmp crgbexit

	align 16
ca2b10g10r10start:
	lea eax,[edx*4]
	lea ecx,[edx*8]
	sub dword [esp+40],eax ;srcRGBBpl
	sub dword [esp+44],ecx ;destRGBBpl
	
	mov esi,dword [esp+20] ;srcPtr
	mov edi,dword [esp+24] ;destPtr
	mov ebp,dword [esp+32] ;height
	mov ebx,dword [esp+48] ;rgbTable
	align 16
ca2b10g10r10lop:
	mov ecx,dword [esp+28] ;width
	align 16
ca2b10g10r10lop2:
	mov edx,dword [esi]
	mov eax,edx
	shr edx,14
	shl eax,6
	shr dx,6
	shr ax,6
	movd xmm1,edx
	movd xmm2,eax
	punpckldq xmm2,xmm1
	psllw xmm2,6
	movdqa xmm1,xmm2
	psrlw xmm2,10
	por xmm2,xmm1
	pextrw eax,xmm2,0
	movq xmm1,[ebx+eax*8]
	pextrw eax,xmm2,1
	movq xmm0,[ebx+eax*8+524288]
	paddsw xmm1,xmm0
	pextrw eax,xmm2,2
	movq xmm0,[ebx+eax*8+1048576]
	paddsw xmm1,xmm0
	movq [edi],xmm1
	lea esi,[esi+4]
	lea edi,[edi+8]
	dec ecx
	jnz ca2b10g10r10lop2
	add esi,dword [esp+40] ;srcRGBBpl
	add edi,dword [esp+44] ;destRGBBpl
	dec ebp
	jnz ca2b10g10r10lop
	jmp crgbexit

	align 16
crgb16start:
	lea eax,[edx*2]
	lea ecx,[edx*8]
	sub dword [esp+40],eax ;srcRGBBpl
	sub dword [esp+44],ecx ;destRGBBpl
	
	mov esi,dword [esp+20] ;srcPtr
	mov edi,dword [esp+24] ;destPtr
	mov ebp,dword [esp+32] ;height
	mov ebx,dword [esp+48] ;rgbTable
	align 16
crgb16lop:
	mov ecx,dword [esp+28] ;width
	align 16
crgb16lop2:
	movzx eax,word [esi]
	movq xmm1,[ebx+eax*8+1048576]
	movq xmm0,[ebx+eax*8+524288]
	paddsw xmm1,xmm0
	movq xmm2, [ebx+eax*8]
	paddsw xmm1,xmm2
	movq [edi],xmm1
	lea esi,[esi+2]
	lea edi,[edi+8]
	dec ecx
	jnz crgb16lop2
	add esi,dword [esp+40] ;srcRGBBpl
	add edi,dword [esp+44] ;destRGBBpl
	dec ebp
	jnz crgb16lop
	jmp crgbexit

	align 16
crgb48start:
	lea ecx,[edx*8]
	mov eax,6
	mul edx
	sub dword [esp+40],eax ;srcRGBBpl
	sub dword [esp+44],ecx ;destRGBBpl
	mov ebx,dword [esp+48] ;rgbTable
	mov esi,dword [esp+20] ;srcPtr
	mov edi,dword [esp+24] ;destPtr
	mov ebp,dword [esp+32] ;height
	align 16
crgb48lop:
	mov ecx,dword [esp+28] ;width
	align 16
crgb48lop2:
	movzx eax,word [esi]
	movq xmm1,[ebx+eax*8+1048576]
	movzx eax,word [esi+2]
	movq xmm0,[ebx+eax*8+524288]
	paddsw xmm1,xmm0
	movzx eax,word [esi+4]
	movq xmm0,[ebx+eax*8]
	paddsw xmm1,xmm0
	movq [edi],xmm1
	lea edi,[edi+8]
	lea esi,[esi+6]
	dec ecx
	jnz crgb48lop2
	add esi,dword [esp+40] ;srcRGBBpl
	add edi,dword [esp+44] ;destRGBBpl
	dec ebp
	jnz crgb48lop
	
	align 16
crgbexit:
	pop edi
	pop esi
	pop ebx
	pop ebp
	ret
