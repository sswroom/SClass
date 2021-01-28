section .text

global _CSRGB8_LRGBC_UpdateRGBTablePal
global CSRGB8_LRGBC_UpdateRGBTablePal
global _CSRGB8_LRGBC_Convert
global CSRGB8_LRGBC_Convert
global _CSRGB8_LRGBC_ConvertW8A8
global CSRGB8_LRGBC_ConvertW8A8
global _CSRGB8_LRGBC_ConvertB5G5R5
global CSRGB8_LRGBC_ConvertB5G5R5
global _CSRGB8_LRGBC_ConvertB5G6R5
global CSRGB8_LRGBC_ConvertB5G6R5
global _CSRGB8_LRGBC_ConvertR8G8B8
global CSRGB8_LRGBC_ConvertR8G8B8
global _CSRGB8_LRGBC_ConvertR8G8B8A8
global CSRGB8_LRGBC_ConvertR8G8B8A8
global _CSRGB8_LRGBC_ConvertP1_A1
global CSRGB8_LRGBC_ConvertP1_A1
global _CSRGB8_LRGBC_ConvertP2_A1
global CSRGB8_LRGBC_ConvertP2_A1
global _CSRGB8_LRGBC_ConvertP4_A1
global CSRGB8_LRGBC_ConvertP4_A1
global _CSRGB8_LRGBC_ConvertP8_A1
global CSRGB8_LRGBC_ConvertP8_A1

;void CSRGB8_LRGB_UpdateRGBTable(UInt8 *srcPal, UInt8 *destPal, UInt8 *rgbTable, OSInt nColor)
;0 edi
;4 esi
;8 ebx
;12 retAddr
;16 srcPal
;20 destPal
;24 rgbTable
;28 nColor
	align 16
_CSRGB8_LRGBC_UpdateRGBTablePal:
CSRGB8_LRGBC_UpdateRGBTablePal:
	push ebx
	push esi
	push edi
	mov esi,dword [esp+16] ;srcPal
	mov edi,dword [esp+20] ;destPal
	mov ebx,dword [esp+24] ;rgbTable
	mov ecx,dword [esp+28] ;nColor
	align 16
urtlop:
	movzx edx,byte [esi]
	movq xmm1,[ebx+edx*8+4096]
	movzx edx,byte [esi+1]
	movq xmm0,[ebx+edx*8+2048]
	paddsw xmm1,xmm0
	movzx edx,byte [esi+2]
	movq xmm0,[ebx+edx*8]
	paddsw xmm1,xmm0
	movq [edi],xmm1
	lea esi,[esi+4]
	lea edi,[edi+8]
	dec ecx
	jnz urtlop
	pop edi
	pop esi
	pop ebx
	ret

;void CSRGB8_LRGBC_Convert(UInt8 *srcPtr, UInt8 *destPtr, OSInt width, OSInt height, OSInt srcRGBBpl, OSInt destRGBBpl, OSInt srcNBits, UInt8 *srcPal, UInt8 *destPal, UInt8 *rgbTable)
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
;48 srcPal
;52 destPal
;56 rgbTable
	align 16
_CSRGB8_LRGBC_Convert:
CSRGB8_LRGBC_Convert:
	push ebp
	push ebx
	push esi
	push edi
	mov eax,dword [esp+44] ;srcNBits
	cmp eax,32
	jz crgb32start
	cmp eax,24
	jz crgb24start
	cmp eax,16
	jz crgb15start
	cmp eax,8
	jz crgb8start
	cmp eax,4
	jz crgb4start
	cmp eax,2
	jz crgb2start
	cmp eax,1
	jz crgb1start
	jmp crgbexit
	
;void CSRGB8_LRGBC_ConvertW8A8(UInt8 *srcPtr, UInt8 *destPtr, OSInt width, OSInt height, OSInt srcRGBBpl, OSInt destRGBBpl, OSInt srcNBits, UInt8 *srcPal, UInt8 *destPal, UInt8 *rgbTable)
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
;48 srcPal
;52 destPal
;56 rgbTable
	align 16
_CSRGB8_LRGBC_ConvertW8A8:
CSRGB8_LRGBC_ConvertW8A8:
	push ebp
	push ebx
	push esi
	push edi
	mov eax,dword [esp+44] ;srcNBits
	cmp eax,16
	jz ca8w8start
	jmp crgbexit

;void CSRGB8_LRGBC_ConvertB5G5R5(UInt8 *srcPtr, UInt8 *destPtr, OSInt width, OSInt height, OSInt srcRGBBpl, OSInt destRGBBpl, OSInt srcNBits, UInt8 *srcPal, UInt8 *destPal, UInt8 *rgbTable)
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
;48 srcPal
;52 destPal
;56 rgbTable
	align 16
_CSRGB8_LRGBC_ConvertB5G5R5:
CSRGB8_LRGBC_ConvertB5G5R5:
	push ebp
	push ebx
	push esi
	push edi
	mov eax,dword [esp+44] ;srcNBits
	cmp eax,16
	jz crgb15start
	jmp crgbexit

;void CSRGB8_LRGBC_ConvertB5G6R5(UInt8 *srcPtr, UInt8 *destPtr, OSInt width, OSInt height, OSInt srcRGBBpl, OSInt destRGBBpl, OSInt srcNBits, UInt8 *srcPal, UInt8 *destPal, UInt8 *rgbTable)
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
;48 srcPal
;52 destPal
;56 rgbTable
	align 16
_CSRGB8_LRGBC_ConvertB5G6R5:
CSRGB8_LRGBC_ConvertB5G6R5:
	push ebp
	push ebx
	push esi
	push edi
	mov eax,dword [esp+44] ;srcNBits
	cmp eax,16
	jz crgb16start
	jmp crgbexit

;void CSRGB8_LRGBC_ConvertR8G8B8(UInt8 *srcPtr, UInt8 *destPtr, OSInt width, OSInt height, OSInt srcRGBBpl, OSInt destRGBBpl, OSInt srcNBits, UInt8 *srcPal, UInt8 *destPal, UInt8 *rgbTable)
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
;48 srcPal
;52 destPal
;56 rgbTable
	align 16
_CSRGB8_LRGBC_ConvertR8G8B8:
CSRGB8_LRGBC_ConvertR8G8B8:
	push ebp
	push ebx
	push esi
	push edi
	mov eax,dword [esp+44] ;srcNBits
	cmp eax,24
	jz crgb24rstart
	jmp crgbexit

;void CSRGB8_LRGBC_ConvertR8G8B8A8(UInt8 *srcPtr, UInt8 *destPtr, OSInt width, OSInt height, OSInt srcRGBBpl, OSInt destRGBBpl, OSInt srcNBits, UInt8 *srcPal, UInt8 *destPal, UInt8 *rgbTable)
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
;48 srcPal
;52 destPal
;56 rgbTable
	align 16
_CSRGB8_LRGBC_ConvertR8G8B8A8:
CSRGB8_LRGBC_ConvertR8G8B8A8:
	push ebp
	push ebx
	push esi
	push edi
	mov eax,dword [esp+44] ;srcNBits
	cmp eax,32
	jz crgb32rstart
	jmp crgbexit

;void CSRGB8_LRGBC_ConvertP1_A1(UInt8 *srcPtr, UInt8 *destPtr, OSInt width, OSInt height, OSInt srcRGBBpl, OSInt destRGBBpl, OSInt srcNBits, UInt8 *srcPal, UInt8 *destPal, UInt8 *rgbTable)
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
;48 srcPal
;52 destPal
;56 rgbTable
	align 16
_CSRGB8_LRGBC_ConvertP1_A1:
CSRGB8_LRGBC_ConvertP1_A1:
	push ebp
	push ebx
	push esi
	push edi
	jmp cp1_a1start

;void CSRGB8_LRGBC_ConvertP2_A1(UInt8 *srcPtr, UInt8 *destPtr, OSInt width, OSInt height, OSInt srcRGBBpl, OSInt destRGBBpl, OSInt srcNBits, UInt8 *srcPal, UInt8 *destPal, UInt8 *rgbTable)
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
;48 srcPal
;52 destPal
;56 rgbTable
	align 16
_CSRGB8_LRGBC_ConvertP2_A1:
CSRGB8_LRGBC_ConvertP2_A1:
	push ebp
	push ebx
	push esi
	push edi
	jmp cp2_a1start

;void CSRGB8_LRGBC_ConvertP4_A1(UInt8 *srcPtr, UInt8 *destPtr, OSInt width, OSInt height, OSInt srcRGBBpl, OSInt destRGBBpl, OSInt srcNBits, UInt8 *srcPal, UInt8 *destPal, UInt8 *rgbTable)
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
;48 srcPal
;52 destPal
;56 rgbTable
	align 16
_CSRGB8_LRGBC_ConvertP4_A1:
CSRGB8_LRGBC_ConvertP4_A1:
	push ebp
	push ebx
	push esi
	push edi
	jmp cp4_a1start

;void CSRGB8_LRGBC_ConvertP8_A1(UInt8 *srcPtr, UInt8 *destPtr, OSInt width, OSInt height, OSInt srcRGBBpl, OSInt destRGBBpl, OSInt srcNBits, UInt8 *srcPal, UInt8 *destPal, UInt8 *rgbTable)
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
;48 srcPal
;52 destPal
;56 rgbTable
	align 16
_CSRGB8_LRGBC_ConvertP8_A1:
CSRGB8_LRGBC_ConvertP8_A1:
	push ebp
	push ebx
	push esi
	push edi
	jmp cp8_a1start

	align 16
crgb32start:
	mov esi,dword [esp+20] ;srcPtr
	mov edi,dword [esp+24] ;destPtr
	mov eax,dword [esp+28] ;width
	mov ebp,dword [esp+32] ;height
	mov ebx,dword [esp+56] ;tab
	shl eax,2
	lea edx,[eax*2]
	sub dword [esp+36],eax ;srcRGBBpl
	sub dword [esp+40],edx ;destRGBBpl
	
	align 16
crgb32lop:
	mov ecx,dword [esp+28] ;width
	align 16
crgb32lop2:
	movzx eax,byte [esi]
	movq xmm1,[ebx+eax*8+4096]
	movzx eax,byte [esi+1]
	movq xmm0,[ebx+eax*8+2048]
	paddsw xmm1,xmm0
	movzx eax,byte [esi+2]
	movq xmm0,[ebx+eax*8]
	paddsw xmm1,xmm0
	movq [edi],xmm1
	lea esi,[esi+4]
	lea edi,[edi+8]
	dec ecx
	jnz crgb32lop2
	add esi,dword [esp+36] ;srcRGBBpl
	add edi,dword [esp+40] ;destRGBBpl
	dec ebp
	jnz crgb32lop
	jmp crgbexit
	
	align 16
crgb32rstart:
	mov esi,dword [esp+20] ;srcPtr
	mov edi,dword [esp+24] ;destPtr
	mov eax,dword [esp+28] ;width
	mov ebp,dword [esp+32] ;height
	mov ebx,dword [esp+56] ;tab
	shl eax,2
	lea edx,[eax*2]
	sub dword [esp+36],eax ;srcRGBBpl
	sub dword [esp+40],edx ;destRGBBpl
	
	align 16
crgb32rlop:
	mov ecx,dword [esp+28] ;width
	align 16
crgb32rlop2:
	movzx eax,byte [esi+2]
	movq xmm1,[ebx+eax*8+4096]
	movzx eax,byte [esi+1]
	movq xmm0,[ebx+eax*8+2048]
	paddsw xmm1,xmm0
	movzx eax,byte [esi+0]
	movq xmm0,[ebx+eax*8]
	paddsw xmm1,xmm0
	movq [edi],xmm1
	lea esi,[esi+4]
	lea edi,[edi+8]
	dec ecx
	jnz crgb32rlop2
	add esi,dword [esp+36] ;srcRGBBpl
	add edi,dword [esp+40] ;destRGBBpl
	dec ebp
	jnz crgb32rlop
	jmp crgbexit
	
	align 16
crgb24start:
	mov esi,dword [esp+20] ;srcPtr
	mov edi,dword [esp+24] ;destPtr
	mov eax,dword [esp+28] ;width
	mov ebp,dword [esp+32] ;height
	mov ebx,dword [esp+56] ;tab
	lea edx,[eax*8]
	lea eax,[eax*2+eax]
	sub dword [esp+36],eax ;srcRGBBpl
	sub dword [esp+40],edx ;destRGBBpl
	align 16
crgb24lop:
	mov ecx,dword [esp+28] ;width
	align 16
crgb24lop2:
	movzx eax,byte [esi]
	movq xmm1,[ebx+eax*8+4096]
	movzx eax,byte [esi+1]
	movq xmm0,[ebx+eax*8+2048]
	paddsw xmm1,xmm0
	movzx eax,byte [esi+2]
	movq xmm0,[ebx+eax*8]
	paddsw xmm1,xmm0
	movq [edi],xmm1
	lea esi,[esi+3]
	lea edi,[edi+8]
	dec ecx
	jnz crgb24lop2
	add esi,dword [esp+36] ;srcRGBBpl
	add edi,dword [esp+40] ;destRGBBpl
	dec ebp
	jnz crgb24lop
	jmp crgbexit

	align 16
crgb24rstart:
	mov esi,dword [esp+20] ;srcPtr
	mov edi,dword [esp+24] ;destPtr
	mov eax,dword [esp+28] ;width
	mov ebp,dword [esp+32] ;height
	mov ebx,dword [esp+56] ;tab
	lea edx,[eax*8]
	lea eax,[eax*2+eax]
	sub dword [esp+36],eax ;srcRGBBpl
	sub dword [esp+40],edx ;destRGBBpl
	align 16
crgb24rlop:
	mov ecx,dword [esp+28] ;width
	align 16
crgb24rlop2:
	movzx eax,byte [esi+2]
	movq xmm1,[ebx+eax*8+4096]
	movzx eax,byte [esi+1]
	movq xmm0,[ebx+eax*8+2048]
	paddsw xmm1,xmm0
	movzx eax,byte [esi+0]
	movq xmm0,[ebx+eax*8]
	paddsw xmm1,xmm0
	movq [edi],xmm1
	lea esi,[esi+3]
	lea edi,[edi+8]
	dec ecx
	jnz crgb24rlop2
	add esi,dword [esp+36] ;srcRGBBpl
	add edi,dword [esp+40] ;destRGBBpl
	dec ebp
	jnz crgb24rlop
	jmp crgbexit

	align 16
ca8w8start:
	mov esi,dword [esp+20] ;srcPtr
	mov edi,dword [esp+24] ;destPtr
	mov eax,dword [esp+28] ;width
	mov ebp,dword [esp+32] ;height
	mov ebx,dword [esp+56] ;tab
	lea edx,[eax*8]
	lea eax,[eax*2]
	sub dword [esp+36],eax ;srcRGBBpl
	sub dword [esp+40],edx ;destRGBBpl
	align 16
ca8w8lop:
	mov ecx,dword [esp+28] ;width
	align 16
ca8w8lop2:
	movzx eax,byte [esi]
	movq xmm1,[ebx+eax*8+4096]
	movq xmm0,[ebx+eax*8+2048]
	paddsw xmm1,xmm0
	movq xmm0,[ebx+eax*8]
	paddsw xmm1,xmm0
	movq [edi],xmm1
	lea esi,[esi+2]
	lea edi,[edi+8]
	dec ecx
	jnz ca8w8lop2
	add esi,dword [esp+36] ;srcRGBBpl
	add edi,dword [esp+40] ;destRGBBpl
	dec ebp
	jnz ca8w8lop
	jmp crgbexit

	align 16
crgb15start:
	mov esi,dword [esp+20] ;srcPtr
	mov edi,dword [esp+24] ;destPtr
	mov eax,dword [esp+28] ;width
	mov ebp,dword [esp+32] ;height
	mov ebx,dword [esp+56] ;tab
	lea edx,[eax*8]
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
crgb15lop2:
	mov ecx,dword [esp+28] ;width
	align 16
crgb15lop:
	movzx eax,word [esi]
	and eax,0x7fff
	shl eax,6
	shr ax,3
	shr al,3
	movd xmm0, eax
	punpcklbw xmm0,xmm1
	pmullw xmm0, xmm4
	psrlw xmm0,4
	pminsw xmm0,xmm5
	pmaxsw xmm0,xmm1

	pextrw eax,xmm0,0
	movq xmm2,[ebx+eax*8+4096]
	pextrw eax,xmm0,1
	movq xmm3,[ebx+eax*8+2048]
	paddsw xmm2,xmm3
	pextrw eax,xmm0,2
	movq xmm3,[ebx+eax*8]
	paddsw xmm2,xmm3
	movq [edi],xmm2

	lea esi,[esi+2]
	lea edi,[edi+8]
	dec ecx
	jnz crgb15lop

	add esi,dword [esp+36] ;srcRGBBpl
	add edi,dword [esp+40] ;destRGBBpl
	dec ebp
	jnz crgb15lop2
	jmp crgbexit

	align 16
crgb16start:
	mov esi,dword [esp+20] ;srcPtr
	mov edi,dword [esp+24] ;destPtr
	mov eax,dword [esp+28] ;width
	mov ebp,dword [esp+32] ;height
	mov ebx,dword [esp+56] ;tab
	lea edx,[eax*8]
	lea eax,[eax*2]
	sub dword [esp+36],eax ;srcRGBBpl
	sub dword [esp+40],edx ;destRGBBpl
	pxor xmm1,xmm1
	mov ecx,0x00410084
	mov edx,0x00ff00ff
	movd xmm4,ecx
	movd xmm5,edx
	punpckldq xmm4,xmm4
	punpckldq xmm5,xmm5
	align 16
crgb16lop2:
	mov ecx,dword [esp+28] ;width
	align 16
crgb16lop:
	movzx eax,word [esi]
	shl eax,5
	shr ax,2
	shr al,3
	movd xmm0, eax
	punpcklbw xmm0,xmm1
	pmullw xmm0, xmm4
	psrlw xmm0,4
	pminsw xmm0,xmm5
	pmaxsw xmm0,xmm1

	pextrw eax,xmm0,0
	movq xmm2,[ebx+eax*8+4096]
	pextrw eax,xmm0,1
	movq xmm3,[ebx+eax*8+2048]
	paddsw xmm2,xmm3
	pextrw eax,xmm0,2
	movq xmm3,[ebx+eax*8]
	paddsw xmm2,xmm3
	movq [edi],xmm2

	lea esi,[esi+2]
	lea edi,[edi+8]
	dec ecx
	jnz crgb16lop

	add esi,dword [esp+36] ;srcRGBBpl
	add edi,dword [esp+40] ;destRGBBpl
	dec ebp
	jnz crgb16lop2
	jmp crgbexit

	align 16
crgb8start:
	mov esi,dword [esp+20] ;srcPtr
	mov edi,dword [esp+24] ;destPtr
	mov eax,dword [esp+28] ;width
	mov ebp,dword [esp+32] ;height
	mov ebx,dword [esp+52] ;destPal
	lea edx,[eax*8]
	sub dword [esp+36],eax ;srcRGBBpl
	sub dword [esp+40],edx ;destRGBBpl
	align 16
crgb8lop2:
	mov ecx,dword [esp+28] ;width
	align 16
crgb8lop:
	movzx eax,byte [esi]
	movq xmm0,[ebx+eax*8]
	movq [edi],xmm0
	inc esi
	lea edi,[edi+8]
	dec ecx
	jnz crgb8lop

	add esi,dword [esp+36] ;srcRGBBpl
	add edi,dword [esp+40] ;destRGBBpl
	dec ebp
	jnz crgb8lop2
	jmp crgbexit

	align 16
crgb4start:	
	mov esi,dword [esp+20] ;srcPtr
	mov edi,dword [esp+24] ;destPtr
	mov eax,dword [esp+28] ;width
	mov ebx,dword [esp+52] ;destPal
	mov ebp,eax
	lea edx,[eax*8]
	shr eax,1
	sub dword [esp+36],eax ;srcRGBBpl
	sub dword [esp+40],edx ;destRGBBpl
	
	align 16
crgb4lop2:
	mov ecx,ebp ;width
	shr ecx,1
	jz crgb4lop5
	
	align 16
crgb4lop:
	movzx eax,byte [esi]
	shr eax,4
	movq xmm0,[ebx+eax*8]
	movq [edi],xmm0

	movzx eax,byte [esi]
	and eax,0xf
	movq xmm0,[ebx+eax*8]
	movq [edi+8],xmm0

	inc esi
	lea edi,[edi+16]
	dec ecx
	jnz crgb4lop
	
	align 16
crgb4lop5:
	test ebp,1
	jz crgb4lop3

	movzx eax,byte [esi]
	shr eax,4
	movq xmm0,[ebx+eax*8]
	movq [edi],xmm0
	lea edi,[edi+8]

	align 16
crgb4lop3:
	add esi,dword [esp+36] ;srcRGBBpl
	add edi,dword [esp+40] ;destRGBBpl
	dec dword [esp+32] ;height
	jnz crgb4lop2
	jmp crgbexit

	align 16
crgb2start:	
	mov esi,dword [esp+20] ;srcPtr
	mov edi,dword [esp+24] ;destPtr
	mov eax,dword [esp+28] ;width
	mov ebx,dword [esp+52] ;destPal
	mov ebp,eax
	lea edx,[eax*8]
	shr eax,2
	sub dword [esp+36],eax ;srcRGBBpl
	sub dword [esp+40],edx ;destRGBBpl

	align 16
crgb2lop2:
	mov ecx,ebp ;width
	shr ecx,2
	jz crgb2lop5

	align 16
crgb2lop:
	movzx eax,byte [esi]
	rol al,2
	movzx edx,al
	and edx,3
	movq xmm0,[ebx+edx*8]
	movq [edi],xmm0

	rol al,2
	movzx edx,al
	and edx,3
	movq xmm0,[ebx+edx*8]
	movq [edi+8],xmm0

	rol al,2
	movzx edx,al
	and edx,3
	movq xmm0,[ebx+edx*8]
	movq [edi+16],xmm0

	rol al,2
	movzx edx,al
	and edx,3
	movq xmm0,[ebx+edx*8]
	movq [edi+24],xmm0

	inc esi
	lea edi,[edi+32]
	dec ecx
	jnz crgb2lop

crgb2lop5:
	mov ecx,ebp
	and ecx,3
	jz crgb2lop3

	movzx eax,byte [esi]
	align 16
crgb2lop4:
	rol al,2
	movzx edx,al
	and edx,3
	movq xmm0,[ebx+edx*8]
	movq [edi],xmm0
	lea edi,[edi+8]
	dec ecx
	jnz crgb2lop4

	align 16
crgb2lop3:
	add esi,dword [esp+36] ;srcRGBBpl
	add edi,dword [esp+40] ;destRGBBpl
	dec dword [esp+32] ;height
	jnz crgb2lop2
	jmp crgbexit

	align 16
crgb1start:	
	mov esi,dword [esp+20] ;srcPtr
	mov edi,dword [esp+24] ;destPtr
	mov eax,dword [esp+28] ;width
	mov ebx,dword [esp+52] ;destPal
	mov ebp,eax
	lea edx,[eax*8]
	shr eax,3
	sub dword [esp+36],eax ;srcRGBBpl
	sub dword [esp+40],edx ;destRGBBpl

	align 16
crgb1lop2:
	mov ecx,ebp ;width
	shr ecx,3
	jz crgb1lop5

	align 16
crgb1lop:
	movzx eax,byte [esi]
	rol al,1
	movzx edx,al
	and edx,1
	movq xmm0,[ebx+edx*8]
	movq [edi],xmm0

	rol al,1
	movzx edx,al
	and edx,1
	movq xmm0,[ebx+edx*8]
	movq [edi+8],xmm0

	rol al,1
	movzx edx,al
	and edx,1
	movq xmm0,[ebx+edx*8]
	movq [edi+16],xmm0

	rol al,1
	movzx edx,al
	and edx,1
	movq xmm0,[ebx+edx*8]
	movq [edi+24],xmm0

	rol al,1
	movzx edx,al
	and edx,1
	movq xmm0,[ebx+edx*8]
	movq [edi+32],xmm0

	rol al,1
	movzx edx,al
	and edx,1
	movq xmm0,[ebx+edx*8]
	movq [edi+40],xmm0

	rol al,1
	movzx edx,al
	and edx,1
	movq xmm0,[ebx+edx*8]
	movq [edi+48],xmm0

	rol al,1
	movzx edx,al
	and edx,1
	movq xmm0,[ebx+edx*8]
	movq [edi+56],xmm0

	inc esi
	lea edi,[edi+64]
	dec ecx
	jnz crgb1lop

crgb1lop5:
	mov ecx,ebp
	and ecx,7
	jz crgb1lop3

	movzx eax,byte [esi]
	align 16
crgb1lop4:
	rol al,1
	movzx edx,al
	and edx,1
	movq xmm0,[ebx+edx*8]
	movq [edi],xmm0
	lea edi,[edi+8]
	dec ecx
	jnz crgb1lop4

	align 16
crgb1lop3:
	add esi,dword [esp+36] ;srcRGBBpl
	add edi,dword [esp+40] ;destRGBBpl
	dec dword [esp+32] ;height
	jnz crgb1lop2
	jmp crgbexit

CSRGB8_LRGBC_32_AVal DW 0,16383

	align 16
cp8_a1start:
	mov esi,dword [esp+20] ;srcPtr
	mov ebp,dword [esp+24] ;destPtr
	mov ebx,dword [esp+52] ;destPal

	align 16
cp8_a1lop2:
	mov ecx,dword [esp+28] ;width
	mov edi,ebp ;destPtr
	
	align 16
cp8_a1lop:
	movzx eax,byte [esi]
	movq xmm0,[ebx+eax*8]
	movq [edi],xmm0
	inc esi
	lea edi,[edi+8]
	dec ecx
	jnz cp8_a1lop

	mov ecx,dword [esp+28] ;width
	mov edi,ebp
	shr ecx,3

	align 16
cp8_a1lop3:
	movzx eax,byte [esi]
	rol al,1
	movzx edx,al
	and edx,1
	mov dx,word CSRGB8_LRGBC_32_AVal[edx*2]
	mov [edi+6],dx

	rol al,1
	movzx edx,al
	and edx,1
	mov dx,word CSRGB8_LRGBC_32_AVal[edx*2]
	mov [edi+14],dx

	rol al,1
	movzx edx,al
	and edx,1
	mov dx,word CSRGB8_LRGBC_32_AVal[edx*2]
	mov [edi+22],dx

	rol al,1
	movzx edx,al
	and edx,1
	mov dx,word CSRGB8_LRGBC_32_AVal[edx*2]
	mov [edi+30],dx

	rol al,1
	movzx edx,al
	and edx,1
	mov dx,word CSRGB8_LRGBC_32_AVal[edx*2]
	mov [edi+38],dx

	rol al,1
	movzx edx,al
	and edx,1
	mov dx,word CSRGB8_LRGBC_32_AVal[edx*2]
	mov [edi+46],dx

	rol al,1
	movzx edx,al
	and edx,1
	mov dx,word CSRGB8_LRGBC_32_AVal[edx*2]
	mov [edi+54],dx

	rol al,1
	movzx edx,al
	and edx,1
	mov dx,word CSRGB8_LRGBC_32_AVal[edx*2]
	mov [edi+62],dx

	inc esi
	lea edi,[edi+64]
	dec ecx
	jnz cp8_a1lop3
	
	mov ecx,dword [esp+28] ;width
	and ecx,7
	jz cp8_a1lop3b
	movzx eax,byte [esi]
	lea esi,[esi+1]
	align 16
cp8_a1lop3a:
	rol al,1
	movzx edx,al
	and edx,1
	mov dx,word CSRGB8_LRGBC_32_AVal[edx*2]
	mov [edi+6],dx
	lea edi,[edi+8]
	dec ecx
	jnz cp8_a1lop3a

	align 16
cp8_a1lop3b:

	add ebp,dword [esp+40] ;destRGBBpl
	dec dword [esp+32] ;height
	jnz cp8_a1lop2
	jmp crgbexit

	align 16
cp4_a1start:	
	mov esi,dword [esp+20] ;srcPtr
	mov ebx,dword [esp+52] ;destPal
	mov ebp,dword [esp+24] ;destPtr
	
	align 16
cp4_a1lop2:
	mov ecx,dword [esp+28] ;width
	mov edi,ebp ;destPtr
	shr ecx,1
	
	align 16
cp4_a1lop:
	movzx eax,byte [esi]
	shr eax,4
	movq xmm0,[ebx+eax*8]
	movq [edi],xmm0

	movzx eax,byte [esi]
	and eax,0xf
	movq xmm0,[ebx+eax*8]
	movq [edi+8],xmm0

	inc esi
	lea edi,[edi+16]
	dec ecx
	jnz cp4_a1lop

	test dword [esp+28],1 ;width
	jz cp4_a1lop1b
	movzx eax,byte [esi]
	lea esi,[esi+1]
	shr al,4
	movq xmm0,[ebx+eax*8]
	movq [edi],xmm0
	lea edi,[edi+8]

	align 16
cp4_a1lop1b:
	
	mov ecx,dword [esp+28] ;width
	mov edi,ebp
	shr ecx,3

	align 16
cp4_a1lop3:
	movzx eax,byte [esi]
	rol al,1
	movzx edx,al
	and edx,1
	mov dx,word CSRGB8_LRGBC_32_AVal[edx*2]
	mov [edi+6],dx

	rol al,1
	movzx edx,al
	and edx,1
	mov dx,word CSRGB8_LRGBC_32_AVal[edx*2]
	mov [edi+14],dx

	rol al,1
	movzx edx,al
	and edx,1
	mov dx,word CSRGB8_LRGBC_32_AVal[edx*2]
	mov [edi+22],dx

	rol al,1
	movzx edx,al
	and edx,1
	mov dx,word CSRGB8_LRGBC_32_AVal[edx*2]
	mov [edi+30],dx

	rol al,1
	movzx edx,al
	and edx,1
	mov dx,word CSRGB8_LRGBC_32_AVal[edx*2]
	mov [edi+38],dx

	rol al,1
	movzx edx,al
	and edx,1
	mov dx,word CSRGB8_LRGBC_32_AVal[edx*2]
	mov [edi+46],dx

	rol al,1
	movzx edx,al
	and edx,1
	mov dx,word CSRGB8_LRGBC_32_AVal[edx*2]
	mov [edi+54],dx

	rol al,1
	movzx edx,al
	and edx,1
	mov dx,word CSRGB8_LRGBC_32_AVal[edx*2]
	mov [edi+62],dx

	inc esi
	lea edi,[edi+64]
	dec ecx
	jnz cp4_a1lop3

	mov ecx,dword [esp+28] ;width
	and ecx,7
	jz cp4_a1lop3b
	movzx eax,byte [esi]
	lea esi,[esi+1]
	align 16
cp4_a1lop3a:
	rol al,1
	movzx edx,al
	and edx,1
	mov dx,word CSRGB8_LRGBC_32_AVal[edx*2]
	mov [edi+6],dx
	lea edi,[edi+8]
	dec ecx
	jnz cp4_a1lop3a

	align 16
cp4_a1lop3b:

	add ebp,dword [esp+40] ;destRGBBpl
	dec dword [esp+32] ;height
	jnz cp4_a1lop2
	jmp crgbexit

	align 16
cp2_a1start:	
	mov esi,dword [esp+20] ;srcPtr
	mov ebp,dword [esp+24] ;destPtr
	mov ebx,dword [esp+52] ;destPal

	align 16
cp2_a1lop2:
	mov ecx,dword [esp+28] ;width
	mov edi,ebp
	shr ecx,2

	align 16
cp2_a1lop:
	movzx eax,byte [esi]
	rol al,2
	movzx edx,al
	and edx,3
	movq xmm0,[ebx+edx*8]
	movq [edi],xmm0

	rol al,2
	movzx edx,al
	and edx,3
	movq xmm0,[ebx+edx*8]
	movq [edi+8],xmm0

	rol al,2
	movzx edx,al
	and edx,3
	movq xmm0,[ebx+edx*8]
	movq [edi+16],xmm0

	rol al,2
	movzx edx,al
	and edx,3
	movq xmm0,[ebx+edx*8]
	movq [edi+24],xmm0

	inc esi
	lea edi,[edi+32]
	dec ecx
	jnz cp2_a1lop

	mov ecx,dword [esp+28] ;width
	and ecx,3
	jz cp2_a1lop1b
	movzx eax,byte [esi]
	lea esi,[esi+1]
	align 16
cp2_a1lop1a:
	rol al,2
	movzx edx,al
	and edx,3
	movq xmm0,[ebx+edx*8]
	movq [edi],xmm0
	lea edi,[edi+8]
	dec ecx
	jnz cp2_a1lop1a

	align 16
cp2_a1lop1b:

	mov ecx,dword [esp+28] ;width
	mov edi,ebp
	shr ecx,3

	align 16
cp2_a1lop3:
	movzx eax,byte [esi]
	rol al,1
	movzx edx,al
	and edx,1
	mov dx,word CSRGB8_LRGBC_32_AVal[edx*2]
	mov [edi+6],dx

	rol al,1
	movzx edx,al
	and edx,1
	mov dx,word CSRGB8_LRGBC_32_AVal[edx*2]
	mov [edi+14],dx

	rol al,1
	movzx edx,al
	and edx,1
	mov dx,word CSRGB8_LRGBC_32_AVal[edx*2]
	mov [edi+22],dx

	rol al,1
	movzx edx,al
	and edx,1
	mov dx,word CSRGB8_LRGBC_32_AVal[edx*2]
	mov [edi+30],dx

	rol al,1
	movzx edx,al
	and edx,1
	mov dx,word CSRGB8_LRGBC_32_AVal[edx*2]
	mov [edi+38],dx

	rol al,1
	movzx edx,al
	and edx,1
	mov dx,word CSRGB8_LRGBC_32_AVal[edx*2]
	mov [edi+46],dx

	rol al,1
	movzx edx,al
	and edx,1
	mov dx,word CSRGB8_LRGBC_32_AVal[edx*2]
	mov [edi+54],dx

	rol al,1
	movzx edx,al
	and edx,1
	mov dx,word CSRGB8_LRGBC_32_AVal[edx*2]
	mov [edi+62],dx

	inc esi
	lea edi,[edi+64]
	dec ecx
	jnz cp2_a1lop3

	mov ecx,dword [esp+28] ;width
	and ecx,7
	jz cp2_a1lop3b
	movzx eax,byte [esi]
	lea esi,[esi+1]
	align 16
cp2_a1lop3a:
	rol al,1
	movzx edx,al
	and edx,1
	mov dx,word CSRGB8_LRGBC_32_AVal[edx*2]
	mov [edi+6],dx
	lea edi,[edi+8]
	dec ecx
	jnz cp2_a1lop3a

	align 16
cp2_a1lop3b:

	add ebp,dword [esp+40] ;destRGBBpl
	dec dword [esp+32] ;height
	jnz cp2_a1lop2
	jmp crgbexit

	align 16
cp1_a1start:	
	mov esi,dword [esp+20] ;srcPtr
	mov ebp,dword [esp+24] ;destPtr
	mov ebx,dword [esp+52] ;destPal

	align 16
cp1_a1lop2:
	mov ecx,dword [esp+28] ;width
	mov edi,ebp
	shr ecx,3

	align 16
cp1_a1lop:
	movzx eax,byte [esi]
	rol al,1
	movzx edx,al
	and edx,1
	movq xmm0,[ebx+edx*8]
	movq [edi],xmm0

	rol al,1
	movzx edx,al
	and edx,1
	movq xmm0,[ebx+edx*8]
	movq [edi+8],xmm0

	rol al,1
	movzx edx,al
	and edx,1
	movq xmm0,[ebx+edx*8]
	movq [edi+16],xmm0

	rol al,1
	movzx edx,al
	and edx,1
	movq xmm0,[ebx+edx*8]
	movq [edi+24],xmm0

	rol al,1
	movzx edx,al
	and edx,1
	movq xmm0,[ebx+edx*8]
	movq [edi+32],xmm0

	rol al,1
	movzx edx,al
	and edx,1
	movq xmm0,[ebx+edx*8]
	movq [edi+40],xmm0

	rol al,1
	movzx edx,al
	and edx,1
	movq xmm0,[ebx+edx*8]
	movq [edi+48],xmm0

	rol al,1
	movzx edx,al
	and edx,1
	movq xmm0,[ebx+edx*8]
	movq [edi+56],xmm0

	inc esi
	lea edi,[edi+64]
	dec ecx
	jnz cp1_a1lop

	mov ecx,dword [esp+28] ;width
	and ecx,7
	jz cp1_a1lop1b
	movzx eax,byte [esi]
	lea esi,[esi+1]
	align 16
cp1_a1lop1a:
	rol al,1
	movzx edx,al
	and edx,1
	movq xmm0,[ebx+edx*8]
	movq [edi],xmm0
	lea edi,[edi+8]
	dec ecx
	jnz cp1_a1lop1a

	align 16
cp1_a1lop1b:

	mov ecx,dword [esp+28] ;width
	mov edi,ebp
	shr ecx,3

	align 16
cp1_a1lop3:
	movzx eax,byte [esi]
	rol al,1
	movzx edx,al
	and edx,1
	mov dx,word CSRGB8_LRGBC_32_AVal[edx*2]
	mov [edi+6],dx

	rol al,1
	movzx edx,al
	and edx,1
	mov dx,word CSRGB8_LRGBC_32_AVal[edx*2]
	mov [edi+14],dx

	rol al,1
	movzx edx,al
	and edx,1
	mov dx,word CSRGB8_LRGBC_32_AVal[edx*2]
	mov [edi+22],dx

	rol al,1
	movzx edx,al
	and edx,1
	mov dx,word CSRGB8_LRGBC_32_AVal[edx*2]
	mov [edi+30],dx

	rol al,1
	movzx edx,al
	and edx,1
	mov dx,word CSRGB8_LRGBC_32_AVal[edx*2]
	mov [edi+38],dx

	rol al,1
	movzx edx,al
	and edx,1
	mov dx,word CSRGB8_LRGBC_32_AVal[edx*2]
	mov [edi+46],dx

	rol al,1
	movzx edx,al
	and edx,1
	mov dx,word CSRGB8_LRGBC_32_AVal[edx*2]
	mov [edi+54],dx

	rol al,1
	movzx edx,al
	and edx,1
	mov dx,word CSRGB8_LRGBC_32_AVal[edx*2]
	mov [edi+62],dx

	inc esi
	lea edi,[edi+64]
	dec ecx
	jnz cp1_a1lop3

	mov ecx,dword [esp+28] ;width
	and ecx,7
	jz cp1_a1lop3b
	movzx eax,byte [esi]
	lea esi,[esi+1]
	align 16
cp1_a1lop3a:
	rol al,1
	movzx edx,al
	and edx,1
	mov dx,word CSRGB8_LRGBC_32_AVal[edx*2]
	mov [edi+6],dx
	lea edi,[edi+8]
	dec ecx
	jnz cp1_a1lop3a

	align 16
cp1_a1lop3b:

	add ebp,dword [esp+40] ;destRGBBpl
	dec dword [esp+32] ;height
	jnz cp1_a1lop2
	jmp crgbexit

	align 16
crgbexit:
	pop edi
	pop esi
	pop ebx
	pop ebp
	ret
