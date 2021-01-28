section .text

global _CSRGB8_RGB8_UpdateRGBTablePal
global CSRGB8_RGB8_UpdateRGBTablePal
global _CSRGB8_RGB8_Convert
global CSRGB8_RGB8_Convert

;void CSRGB8_RGB8_UpdateRGBTable(UInt8 *srcPal, UInt8 *destPal, UInt8 *rgbTable, OSInt nColor)
;0 edi
;4 esi
;8 ebx
;12 retAddr
;16 srcPal
;20 destPal
;24 rgbTable
;28 nColor
	align 16
_CSRGB8_RGB8_UpdateRGBTablePal:
CSRGB8_RGB8_UpdateRGBTablePal:
	push ebx
	push esi
	push edi
	mov esi,dword [esp+16] ;srcPal
	mov edi,dword [esp+20] ;destPal
	mov ebx,dword [esp+24] ;rgbTable
	mov ecx,dword [esp+28] ;nColor
	align 16
urtlop:
	movzx edx,byte [esi+2]
	mov al,byte [ebx+edx+512]
	mov ah,byte [esi+3]
	shl eax,16
	movzx edx,byte [esi+1]
	mov ah,byte [ebx+edx+256]
	movzx edx,byte [esi]
	mov al,byte [ebx+edx]
	mov dword [edi],eax
	lea esi,[esi+4]
	lea edi,[edi+4]
	dec ecx
	jnz urtlop
	pop edi
	pop esi
	pop ebx
	ret

;void CSRGB8_RGB8_Convert(UInt8 *srcPtr, UInt8 *destPtr, OSInt width, OSInt height, OSInt srcRGBBpl, OSInt destRGBBpl, OSInt srcNBits, OSInt destNbits, UInt8 *srcPal, UInt8 *destPal, UInt8 *rgbTable)
;0 edi
;4 esi
;8 ebx
;12 ebp
;16 retAddr
;20 srcPtr
;24 destPtr
;28 width
;32 height
;36 srcRGBBpl
;40 destRGBBpl
;44 srcNBits
;48 destNBits
;52 srcPal
;56 destPal
;60 rgbTable
	align 16
_CSRGB8_RGB8_Convert:
CSRGB8_RGB8_Convert:
	push ebp
	push ebx
	push esi
	push edi
	mov eax,dword [esp+48] ;destNBits
	cmp eax,32
	jz crgb32start
	jmp crgbexit
	
	align 16
crgb32start:
	mov eax,dword [esp+44] ;srcNBits
	cmp eax,32
	jz crgb32_32start
	cmp eax,24
	jz crgb32_24start
	cmp eax,16
	jz crgb32_16start
	cmp eax,8
	jz crgb32_8start
	cmp eax,4
	jz crgb32_4start
	jmp crgbexit
	
	align 16
crgb32_32start:
	mov esi,dword [esp+20] ;srcPtr
	mov edi,dword [esp+24] ;destPtr
	mov eax,dword [esp+28] ;width
	mov ebp,dword [esp+32] ;height
	mov ebx,dword [esp+60] ;tab
	shl eax,2
	sub dword [esp+36],eax ;srcRGBBpl
	sub dword [esp+40],eax ;destRGBBpl
	
	align 16
crgb32_32lop:
	mov ecx,dword [esp+28] ;width
	align 16
crgb32_32lop2:
	movzx eax,byte [esi+2]
	mov dl,byte [ebx+eax+512]
	mov dh,byte [esi+3]
	movzx eax,byte [esi+1]
	shl edx,16
	mov dh,byte [ebx+eax+256]
	movzx eax,byte [esi]
	mov dl,byte [ebx+eax]
	mov dword [edi],edx
	lea esi,[esi+4]
	lea edi,[edi+4]
	dec ecx
	jnz crgb32_32lop2
	add esi,dword [esp+36] ;srcRGBBpl
	add edi,dword [esp+40] ;destRGBBpl
	dec ebp
	jnz crgb32_32lop
	jmp crgbexit
	
	align 16
crgb32_24start:
	mov esi,dword [esp+20] ;srcPtr
	mov edi,dword [esp+24] ;destPtr
	mov eax,dword [esp+28] ;width
	mov ebp,dword [esp+32] ;height
	mov ebx,dword [esp+60] ;tab
	lea edx,[eax*4]
	lea eax,[eax*2+eax]
	sub dword [esp+36],eax ;srcRGBBpl
	sub dword [esp+40],edx ;destRGBBpl
	align 16
crgb32_24lop:
	mov ecx,dword [esp+28] ;width
	align 16
crgb32_24lop2:
	movzx eax,byte [esi+2]
	mov dl,byte [ebx+eax+512]
	mov dh,255
	movzx eax,byte [esi+1]
	shl edx,16
	mov dh,byte [ebx+eax+256]
	movzx eax,byte [esi]
	mov dl,byte [ebx+eax]
	mov dword [edi],edx
	lea esi,[esi+3]
	lea edi,[edi+4]
	dec ecx
	jnz crgb32_24lop2
	add esi,dword [esp+36] ;srcRGBBpl
	add edi,dword [esp+40] ;destRGBBpl
	dec ebp
	jnz crgb32_24lop
	jmp crgbexit

	align 16
crgb32_16start:
	mov esi,dword [esp+20] ;srcPtr
	mov edi,dword [esp+24] ;destPtr
	mov eax,dword [esp+28] ;width
	mov ebp,dword [esp+32] ;height
	mov ebx,dword [esp+60] ;tab
	lea edx,[eax*4]
	lea eax,[eax*2]
	sub dword [esp+36],eax ;srcRGBBpl
	sub dword [esp+40],edx ;destRGBBpl
	pxor xmm1,xmm1
	mov ecx,0x00840084
	mov edx,0x00ff00ff
	movd xmm4,ecx
	movd xmm5,edx
	punpckldq xmm4,xmm4
	punpckldq xmm5,xmm5
	align 16
crgb32_16lop2:
	mov ecx,dword [esp+28] ;width
	align 16
crgb32_16lop:
	movzx eax,word [esi]
	shl eax,6
	shr ax,3
	shr al,3
	movd xmm0, eax
	punpcklbw xmm0,xmm1
	pmullw xmm0, xmm4
	psrlw xmm0,4
	pminsw xmm0,xmm5
	pmaxsw xmm0,xmm1

	pextrw edx,xmm0,2
	mov al,byte [ebx+edx+512]
	mov ah,0xff
	shl eax,16
	pextrw edx,xmm0,1
	mov ah,byte [ebx+edx+256]
	pextrw edx,xmm0,1
	mov al,byte [ebx+edx]
	mov dword [edi],eax

	lea esi,[esi+2]
	lea edi,[edi+4]
	dec ecx
	jnz crgb32_16lop

	add esi,dword [esp+36] ;srcRGBBpl
	add edi,dword [esp+40] ;destRGBBpl
	dec ebp
	jnz crgb32_16lop2
	jmp crgbexit

	align 16
crgb32_8start:
	mov esi,dword [esp+20] ;srcPtr
	mov edi,dword [esp+24] ;destPtr
	mov edx,dword [esp+28] ;width
	mov ebp,dword [esp+32] ;height
	mov ebx,dword [esp+56] ;destPal
	lea edx,[eax*4]
	sub dword [esp+36],eax ;srcRGBBpl
	sub dword [esp+40],edx ;destRGBBpl
	align 16
crgb32_8lop2:
	mov ecx,dword [esp+28] ;width
	align 16
crgb32_8lop:
	movzx eax,byte [esi]
	mov edx,dword [ebx+eax*4]
	mov dword [edi],edx
	inc esi
	lea edi,[edi+4]
	dec ecx
	jnz crgb32_8lop

	add esi,dword [esp+36] ;srcRGBBpl
	add edi,dword [esp+40] ;destRGBBpl
	dec ebp
	jnz crgb32_8lop2
	jmp crgbexit

	align 16
crgb32_4start:	
	mov esi,dword [esp+20] ;srcPtr
	mov edi,dword [esp+24] ;destPtr
	mov edx,dword [esp+28] ;width
	mov ebx,dword [esp+56] ;destPal
	mov ebp,eax
	lea edx,[eax*4]
	shr eax,1
	sub dword [esp+36],eax ;srcRGBBpl
	sub dword [esp+40],edx ;destRGBBpl
	align 16
crgb32_4lop2:
	mov ecx,ebp ;width
	shr ecx,1
crgb32_4lop:
	movzx eax,byte [esi]
	shr eax,4
	mov edx,dword [ebx+eax*4]
	mov dword [edi],edx

	movzx eax,byte [esi]
	and eax,0xf
	mov edx,dword [ebx+eax*4]
	mov dword [edi+4],edx

	inc esi
	lea edi,[edi+8]
	dec ecx
	jnz crgb32_4lop
	test ebp,1
	jz crgb32_4lop3

	movzx eax,byte [esi]
	shr eax,4
	mov edx,dword [ebx+eax*4]
	mov dword [edi],edx
	inc esi
	lea edi,[edi+4]

	align 16
crgb32_4lop3:
	add esi,dword [esp+36] ;srcRGBBpl
	add edi,dword [esp+40] ;destRGBBpl
	dec dword [esp+32] ;height
	jnz crgb32_4lop2
	jmp crgbexit

	align 16
crgbexit:
	pop edi
	pop esi
	pop ebx
	pop ebp
	ret
