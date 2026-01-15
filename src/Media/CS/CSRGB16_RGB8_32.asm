section .text

global _CSRGB16_RGB8_Convert
global CSRGB16_RGB8_Convert

;void CSRGB16_RGB8_Convert(UInt8 *srcPtr, UInt8 *destPtr, IntOS width, IntOS height, IntOS srcRGBBpl, IntOS destRGBBpl, IntOS srcNBits, IntOS destNbits, UInt8 *rgbTable);
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
;52 rgbTable
	align 16
_CSRGB16_RGB8_Convert:
CSRGB16_RGB8_Convert:
	push ebp
	push ebx
	push esi
	push edi
	mov eax,dword [esp+40] ;destNBits
	cmp eax,32
	jz crgb32start
	jmp crgbexit
	
	align 16
crgb32start:
	mov eax,dword [esp+36] ;srcNBits
	cmp eax,64
	jz crgb32_64start
	cmp eax,48
	jz crgb32_48start
	jmp crgbexit

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
;52 rgbTable
	align 16
crgb32_64start:
	mov esi,dword [esp+20] ;srcPtr
	mov edi,dword [esp+24] ;destPtr
	mov eax,dword [esp+28] ;width
	mov ebp,dword [esp+32] ;height
	mov ebx,dword [esp+52] ;tab
	lea edx,[eax*4]
	shl eax,3
	sub dword [esp+36],eax ;srcRGBBpl
	sub dword [esp+40],edx ;destRGBBpl
	align 16
crgb32_64lop:
	mov ecx,dword [esp+28] ;width
	align 16
crgb32_64lop2:
	movzx edx,word [esi+4]
	mov al,byte [ebx+edx+131072]
	mov ah,byte [esi+7]
	shl eax,16
	movzx edx,word [esi+2]
	mov ah,byte [ebx+edx+65536]
	movzx edx,word [esi]
	mov al,byte [ebx+edx]
	mov dword [edi],eax
	lea esi,[esi+8]
	lea edi,[edi+4]
	dec ecx
	jnz crgb32_64lop2
	add esi,dword [esp+36] ;srcRGBBpl
	add edi,dword [esp+40] ;destRGBBpl
	dec ebp
	jnz crgb32_64lop
	jmp crgbexit

	align 16
crgb32_48start:
	mov esi,dword [esp+20] ;srcPtr
	mov edi,dword [esp+24] ;destPtr
	mov eax,dword [esp+28] ;width
	mov ebp,dword [esp+32] ;height
	mov ebx,dword [esp+52] ;tab
	lea edx,[eax*4]
	lea eax,[edx+eax*2]
	sub dword [esp+36],eax ;srcRGBBpl
	sub dword [esp+40],edx ;destRGBBpl
	align 16
crgb32_48lop:
	mov ecx,dword [esp+28] ;width
	align 16
crgb32_48lop2:
	movzx edx,word [esi+4]
	mov al,byte [ebx+edx+131072]
	mov ah,0xff
	shl eax,16
	movzx edx,word [esi+2]
	mov ah,byte [ebx+edx+65536]
	movzx edx,word [esi]
	mov al,byte [ebx+edx]
	mov dword [edi],eax
	lea esi,[esi+6]
	lea edi,[edi+4]
	dec ecx
	jnz crgb32_48lop2
	add esi,dword [esp+36] ;srcRGBBpl
	add edi,dword [esp+40] ;destRGBBpl
	dec ebp
	jnz crgb32_48lop
	
	align 16
crgbexit:
	pop edi
	pop esi
	pop ebx
	pop ebp
	ret
