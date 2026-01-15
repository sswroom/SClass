section .text

global _LanczosResizer16_C8_horizontal_filter_pa
global _LanczosResizer16_C8_horizontal_filter
global _LanczosResizer16_C8_vertical_filter
global _LanczosResizer16_C8_expand
global _LanczosResizer16_C8_expand_pa
global _LanczosResizer16_C8_collapse
global _LanczosResizer16_C8_imgcopy
global _LanczosResizer16_C8_imgcopy_pa

;void LanczosResizer16_C8_horizontal_filter_pa(UInt8 *inPt, UInt8 *outPt, IntOS dwidth, IntOS height, IntOS tap, IntOS *index, Int64 *weight, IntOS sstep, IntOS dstep, UInt8 *rgbTable, IntOS swidth, UInt8 *tmpbuff)
;0 edi
;4 esi
;8 ebx
;12 ebp
;16 retAddr
;20 inPt
;24 outPt
;28 dwidth
;32 height
;36 tap
;40 index
;44 weight
;48 sstep
;52 dstep
;56 rgbTable
;60 swidth
;64 tmpbuff

	align 16
_LanczosResizer16_C8_horizontal_filter_pa:
	push ebp
	push ebx
	push esi
	push edi
	
	shr dword [esp+36],1 ;tap
	pxor xmm3,xmm3
	align 16
hfpalop:
	mov esi,dword [esp+20] ;inPt
	mov ebx,dword [esp+56] ;rgbTable
	mov edi,dword [esp+64] ;tmpbuff

	mov ecx,dword [esp+60] ;swidth
	align 16
hfpalop4:
	movq xmm7,[esi]
	pextrw edx,xmm7,0
	movq xmm1,[ebx+edx*8+1310720]
	pextrw edx,xmm7,1
	movq xmm0,[ebx+edx*8+786432]
	pextrw edx,xmm7,2
	paddsw xmm1,xmm0
	movq xmm0,[ebx+edx*8+262144]
	pextrw edx,xmm7,3
	paddsw xmm1,xmm0

	movd xmm0,edx
	punpcklwd xmm0, xmm0
	punpcklwd xmm0, xmm0
	pmulhuw xmm0, xmm1
	movq xmm1, [ebx+edx*8+1835008]
	paddsw xmm0,xmm1
	movq [edi],xmm0

	lea esi,[esi+8]
	lea edi,[edi+8]
	dec ecx
	jnz hfpalop4

	mov esi,dword [esp+64] ;tmpbuff
	mov edi,dword [esp+24] ;outPt

	mov ecx,dword [esp+28] ;dwidth

	mov ebx,dword [esp+40] ;index
	mov ebp,dword [esp+44] ;weight
	align 16
hfpalop2:
	mov edx,dword [esp+36] ;tap
	pxor xmm2,xmm2
	ALIGN 16
hfpalop3:
	mov eax,dword [ebx]
	movq xmm0,[esi+eax]
	mov eax,dword [ebx+4]
	movq xmm1,[esi+eax]
	punpcklwd xmm0,xmm1
	pmaddwd xmm0,[ebp]
	paddd xmm2,xmm0
	
	lea ebp,[ebp+16]
	lea ebx,[ebx+8]
	dec edx
	jnz hfpalop3

	psrad xmm2,15
	packssdw xmm2,xmm3
	movq [edi],xmm2
	lea edi,[edi+8]
	dec ecx
	jnz hfpalop2

	mov eax,dword [esp+48] ;sstep
	mov edx,dword [esp+52] ;dstep
	add dword [esp+20],eax ;inPt
	add dword [esp+24],edx ;outPt

	dec dword [esp+32] ;currHeight
	jnz hfpalop
	pop edi
	pop esi
	pop ebx
	pop ebp
	ret

;void LanczosResizer16_C8_horizontal_filter(UInt8 *inPt, UInt8 *outPt, IntOS dwidth, IntOS height, IntOS tap, IntOS *index, Int64 *weight, IntOS sstep, IntOS dstep, UInt8 *rgbTable, IntOS swidth, UInt8 *tmpbuff)
;0 edi
;4 esi
;8 ebx
;12 ebp
;16 retAddr
;20 inPt
;24 outPt
;28 dwidth
;32 height
;36 tap
;40 index
;44 weight
;48 sstep
;52 dstep
;56 rgbTable
;60 swidth
;64 tmpbuff

	align 16
_LanczosResizer16_C8_horizontal_filter:
	push ebp
	push ebx
	push esi
	push edi
	shr dword [esp+36],1 ;tap
	pxor xmm3,xmm3
	align 16
hflop:
	mov esi,dword [esp+20] ;inPt
	mov ebx,dword [esp+56] ;rgbTable
	mov edi,dword [esp+64] ;tmpbuff

	mov ecx,dword [esp+60] ;swidth
	align 16
hflop4:
	movq xmm7,[esi]
	pextrw edx,xmm7,0
	movq xmm1, [ebx+edx*8+1310720]
	pextrw edx,xmm7,1
	movq xmm0, [ebx+edx*8+786432]
	pextrw edx,xmm7,2
	paddsw xmm1,xmm0
	movq xmm0, [ebx+edx*8+262144]
	paddsw xmm1,xmm0
	pextrw edx,xmm7,3
	movq xmm0, [ebx+edx*8+1835008]
	paddsw xmm1,xmm0
	movq [edi],xmm1

	lea esi,[esi+8]
	lea edi,[edi+8]
	dec ecx
	jnz hflop4

	mov edi,dword [esp+24] ;outPt
	mov ecx,dword [esp+28] ;dwidth
	mov ebx,dword [esp+40] ;index
	mov ebp,dword [esp+44] ;weight
	mov esi,dword [esp+64] ;tmpbuff
	align 16
hflop2:
	mov edx,dword [esp+36] ;tap
	pxor xmm2,xmm2
	align 16
hflop3:
	mov eax,dword [ebx]
	movq xmm0,[esi+eax]
	mov eax,dword [ebx+4]
	movq xmm1,[esi+eax]
	punpcklwd xmm0,xmm1
	pmaddwd xmm0,[ebp]
	paddd xmm2,xmm0
	
	lea ebp,[ebp+16]
	lea ebx,[ebx+8]
	dec edx
	jnz hflop3

	psrad xmm2,15
	packssdw xmm2,xmm3
	movq [edi],xmm2
	lea edi,[edi+8]
	dec ecx
	jnz hflop2

	mov eax,dword [esp+48] ;sstep
	mov edx,dword [esp+52] ;dstep
	add dword [esp+20],eax ;inPt
	add dword [esp+24],edx ;outPt

	dec dword [esp+32] ;currHeight
	jnz hflop
	pop edi
	pop esi
	pop ebx
	pop ebp
	ret

;void LanczosResizer16_C8_vertical_filter(UInt8 *inPt, UInt8 *outPt, IntOS dwidth, IntOS height, IntOS tap, IntOS *index, Int64 *weight, IntOS sstep, IntOS dstep, UInt8 *rgbTable)
;0 edi
;4 esi
;8 ebx
;12 ebp
;16 retAddr
;20 inPt
;24 outPt
;28 dwidth
;32 height
;36 tap
;40 index
;44 weight
;48 sstep
;52 dstep
;56 rgbTable

	align 16
_LanczosResizer16_C8_vertical_filter:
	push ebp
	push ebx
	push esi
	push edi
	mov eax,dword [esp+28] ;dwidth
	lea edx,[eax*4]
	sub dword [esp+52],edx ;dstep
	test eax,1
	jz vfstart								;if (dwidth & 1)

	shr dword [esp+36],1 ;tap
	mov edi,dword [esp+24] ;outPt
	pxor xmm3,xmm3
	align 16
vflop:
	mov esi,dword [esp+20] ;inPt
	mov ebp,dword [esp+28] ;dwidth

	align 16
vflop2:
	mov ebx,dword [esp+40] ;index
	mov edx,dword [esp+44] ;weight

	mov ecx,dword [esp+36] ;tap
	pxor xmm2,xmm2
	ALIGN 16
vflop3:
	mov eax,dword [ebx]
	movq xmm0,[esi+eax]
	mov eax,dword [ebx+4]
	movq xmm1,[esi+eax]
	punpcklwd xmm0, xmm1
	pmaddwd xmm0,[edx]
	paddd xmm2,xmm0
	lea ebx,[ebx+8]
	lea edx,[edx+16]

	dec ecx
	jnz vflop3

	mov ebx,dword [esp+56] ;rgbTable

	psrad xmm2,15
	packssdw xmm2,xmm3
	pextrw edx,xmm2,2
	mov al,byte [ebx+edx+131072]
	pextrw edx,xmm2,3
	mov ah,byte [ebx+edx+196608]
	shl eax,16
	pextrw edx,xmm2,0
	mov al,byte [ebx+edx]
	pextrw edx,xmm2,1
	mov ah,byte [ebx+edx+65536]

	movnti dword [edi],eax
	lea esi,[esi+8]
	lea edi,[edi+4]
	dec ebp
	jnz vflop2

	mov eax,dword [esp+36] ;tap
	lea edx,[eax*8]
	shl eax,4
	add dword [esp+40],edx ;index
	add dword [esp+44],eax ;weight

	add edx,dword [esp+52] ;dstep

	dec dword [esp+32] ;currHeight
	jnz vflop
	jmp vfexit

	align 16
vfstart:
	shr dword [esp+36],1 ;tap
	shr dword [esp+28],1 ;dwidth
	mov edi,dword [esp+24] ;outPt
	pxor xmm3,xmm3
	align 16
vflop4:
	mov esi,dword [esp+20] ;inPt
	mov ebp,dword [esp+28] ;dwidth
	align 16
vflop5:
	mov ebx,dword [esp+40] ;index
	mov edx,dword [esp+44] ;weight

	mov ecx,dword [esp+36] ;tap
	pxor xmm2,xmm2
	pxor xmm6,xmm6
	ALIGN 16
vflop6:
	mov eax,dword [ebx]
	movdqu xmm0,[esi+eax]
	mov eax,dword [ebx+4]
	movdqu xmm1,[esi+eax]
	movdqu xmm4,xmm0
	punpcklwd xmm0, xmm1
	punpckhwd xmm4, xmm1
	movdqa xmm5,[edx]
	pmaddwd xmm0,xmm5
	pmaddwd xmm4,xmm5
	paddd xmm2,xmm0
	paddd xmm6,xmm4
	lea edx,[edx+16]
	lea ebx,[ebx+8]

	dec ecx
	jnz vflop6

	mov ebx,dword [esp+56] ;rgbTable

	psrad xmm6,15
	packssdw xmm6,xmm3
	pextrw edx,xmm6,2
	mov al,byte [ebx+edx+131072]
	pextrw edx,xmm6,3
	mov ah,byte [ebx+edx+196608]
	shl eax,16
	pextrw edx,xmm6,0
	mov al,byte  [ebx+edx]
	pextrw edx,xmm6,1
	mov ah,byte [ebx+edx+65536]
	movd xmm0,eax

	psrad xmm2,15
	packssdw xmm2,xmm3
	pextrw edx,xmm2,2
	mov al,byte [ebx+edx+131072]
	pextrw edx,xmm2,3
	mov ah,byte [ebx+edx+196608]
	shl eax,16
	pextrw edx,xmm2,0
	mov al,byte [ebx+edx]
	pextrw edx,xmm2,1
	mov ah,byte [ebx+edx+65536]
	movd xmm1,eax
	punpckldq xmm1,xmm0

	movq [edi],xmm1
	lea esi,[esi+16]
	lea edi,[edi+8]
	dec ebp
	jnz vflop5

	mov eax,dword [esp+36] ;tap
	lea edx,[eax*8]
	shl eax,4
	add dword [esp+40],edx ;index
	add dword [esp+44],eax ;weight

	add edi,dword [esp+52] ;dstep

	dec dword [esp+32] ;currHeight
	jnz vflop4

	align 16
vfexit:
	pop edi
	pop esi
	pop ebx
	pop ebp
	ret

;void LanczosResizer16_C8_expand(UInt8 *inPt, UInt8 *outPt, IntOS width, IntOS height, IntOS sstep, IntOS dstep, UInt8 *rgbTable)
;0 edi
;4 esi
;8 ebx
;12 ebp
;16 retAddr
;20 inPt
;24 outPt
;28 width
;32 height
;36 sstep
;40 dstep
;44 rgbTable

	align 16
_LanczosResizer16_C8_expand:
	push ebp
	push ebx
	push esi
	push edi
	mov eax,dword [esp+28]
	shl eax,3
	sub dword [esp+36],eax ;sstep
	sub dword [esp+40],eax ;dstep

	mov ebp,dword [esp+32] ;height
	mov esi,dword [esp+20] ;inPt
	mov edi,dword [esp+24] ;outPt
	mov ebx,dword [esp+44] ;rgbTable
	align 16
explop:

	mov ecx,dword [esp+28] ;width
	ALIGN 16
explop2:
	movq xmm7,[ecx]
	pextrw edx,xmm7,0
	movq xmm1,[ebx+edx*8+1310720]
	pextrw edx,xmm7,1
	movq xmm0,[ebx+edx*8+786432]
	pextrw edx,xmm7,2
	paddsw xmm1,xmm0
	movq xmm0,[ebx+edx*8+262144]
	paddsw xmm1,xmm0
	pextrw edx,xmm7,3
	movq xmm0,[ebx+edx*8+1835008]
	paddsw xmm1,xmm0
	movq [edi],xmm1

	lea esi,[esi+8]
	lea edi,[edi+8]
	dec ecx
	jnz explop2

	add esi,dword [esp+36] ;sstep
	add edi,dword [esp+40] ;dstep

	dec ebp
	jnz explop
	pop edi
	pop esi
	pop ebx
	pop ebp
	ret

;void LanczosResizer16_C8_expand_pa(UInt8 *inPt, UInt8 *outPt, IntOS width, IntOS height, IntOS sstep, IntOS dstep, UInt8 *rgbTable)
;0 edi
;4 esi
;8 ebx
;12 ebp
;16 retAddr
;20 inPt
;24 outPt
;28 width
;32 height
;36 sstep
;40 dstep
;44 rgbTable

	align 16
_LanczosResizer16_C8_expand_pa:
	push ebp
	push ebx
	push esi
	push edi
	mov eax,dword [esp+28] ;width
	shl eax,3
	sub dword [esp+36],eax ;sstep
	sub dword [esp+40],eax ;dstep

	mov esi,dword [esp+20] ;inPt
	mov edi,dword [esp+24] ;outPt
	mov ebp,dword [esp+32] ;height
	mov ebx,dword [esp+44] ;rgbTable
	align 16
exppalop:

	mov ecx,dword [esp+28] ;width
	ALIGN 16
exppalop2:
	movq xmm7,[esi]
	pextrw edx,xmm7,0
	movq xmm1,[ebx+edx*8+1310720]
	pextrw edx,xmm7,1
	movq xmm0,[ebx+edx*8+786432]
	pextrw edx,xmm7,2
	paddsw xmm1,xmm0
	movq xmm0,[ebx+edx*8+262144]
	paddsw xmm1,xmm0
	pextrw edx,xmm7,3

	movd xmm0,edx
	punpcklwd xmm0, xmm0
	punpcklwd xmm0, xmm0
	pmulhuw xmm0, xmm1
	movq xmm1,[ebx+edx*8+1835008]
	paddsw xmm0,xmm1
	movq [edi],xmm0

	lea esi,[esi+8]
	lea edi,[edi+8]
	dec ecx
	jnz exppalop2

	add esi,dword [esp+36] ;sstep
	add edi,dword [esp+40] ;dstep

	dec ebp
	jnz exppalop
	pop edi
	pop esi
	pop ebx
	pop ebp
	ret

;void LanczosResizer16_C8_collapse(UInt8 *inPt, UInt8 *outPt, IntOS width, IntOS height, IntOS sstep, IntOS dstep, UInt8 *rgbTable)
;0 edi
;4 esi
;8 ebx
;12 ebp
;16 retAddr
;20 inPt
;24 outPt
;28 width
;32 height
;36 sstep
;40 dstep
;44 rgbTable

	align 16
_LanczosResizer16_C8_collapse:
	push ebp
	push ebx
	push esi
	push edi
	
	mov eax,dword [esp+28] ;width
	lea edx,[eax*8]
	shl eax,2
	sub dword [esp+36],edx ;sstep
	sub dword [esp+40],eax ;dstep
	mov esi,dword [esp+20] ;inPt
	mov edi,dword [esp+24] ;outPt
	mov ebp,dword [esp+32] ;height
	mov ebx,dword [esp+44] ;rgbTable
	align 16
collop:
	mov ecx,dword [esp+28] ;width
	ALIGN 16
collop2:
	movzx edx,word [esi+6]
	mov ah,byte [ebx+edx+196608]
	movzx edx,word [esi+4]
	mov al,byte [ebx+edx+131072]
	shl eax,16
	movzx edx,word [esi+2]
	mov ah,byte [ebx+edx+65536]
	movzx edx,word [esi]
	mov al,byte [ebx+edx]

	movnti dword [edi],eax
	lea esi,[esi+8]
	add edi,[edi+4]
	dec ecx
	jnz collop2

	add esi,dword [esp+36]
	add edi,dword [esp+40]

	dec ebp
	jnz collop
	pop edi
	pop esi
	pop ebx
	pop ebp
	ret

;void LanczosResizer16_C8_imgcopy(UInt8 *inPt, UInt8 *outPt, IntOS width, IntOS height, IntOS sstep, IntOS dstep, UInt8 *rgbTable)
;0 edi
;4 esi
;8 ebx
;12 ebp
;16 retAddr
;20 inPt
;24 outPt
;28 width
;32 height
;36 sstep
;40 dstep
;44 rgbTable

	align 16
_LanczosResizer16_C8_imgcopy:
	push ebp
	push ebx
	push esi
	push edi

	mov eax,dword [esp+28] ;width
	lea edx,[eax*8]
	shl eax,2
	sub dword [esp+36],edx ;sstep
	sub dword [esp+40],eax ;dstep
	mov esi,dword [esp+20] ;inPt
	mov edi,dword [esp+24] ;outPt
	mov ebp,dword [esp+32] ;height
	mov ebx,dword [esp+44] ;rgbTable
	align 16
iclop:
	mov ecx,dword [esp+28] ;width
	ALIGN 16
iclop2:
	movq xmm7,[esi]
	pextrw edx,xmm7,0
	movq xmm1,[ebx+edx*8+1310720]
	pextrw edx,xmm7,1
	movq xmm0,[ebx+edx*8+786432]
	pextrw edx,xmm7,2
	paddsw xmm1,xmm0
	movq xmm0,[ebx+edx*8+262144]
	paddsw xmm1,xmm0
	pextrw edx,xmm1,2
	pextrw eax,xmm7,3
	mov al,byte [ebx+edx+131072]
	shl eax,16
	pextrw edx,xmm1,1
	mov ah,byte [ebx+edx+65536]
	pextrw edx,xmm1,0
	mov al,byte [ebx+edx]
	movnti dword [edi],eax
	lea esi,[esi+8]
	lea edi,[edi+4]
	dec ecx
	jnz iclop2

	add edi,dword [esp+36] ;dstep
	add esi,dword [esp+40] ;sstep
	dec ebp
	jnz iclop
	pop edi
	pop esi
	pop ebx
	pop ebp
	ret

;void LanczosResizer16_C8_imgcopy_pa(UInt8 *inPt, UInt8 *outPt, IntOS width, IntOS height, IntOS sstep, IntOS dstep, UInt8 *rgbTable)
;0 edi
;4 esi
;8 ebx
;12 ebp
;16 retAddr
;20 inPt
;24 outPt
;28 width
;32 height
;36 sstep
;40 dstep
;44 rgbTable

	align 16
_LanczosResizer16_C8_imgcopy_pa:
	push ebp
	push ebx
	push esi
	push edi
	mov eax,dword [esp+28] ;width
	lea edx,[eax*8]
	lea ecx,[eax*4]
	shr eax,1
	sub dword [esp+36],edx ;sstep
	sub dword [esp+40],ecx ;dstep
	mov dword [esp+28],eax ;width			width = width >> 1;

	mov esi,dword [esp+20] ;inPt
	mov edi,dword [esp+24] ;outPt
	mov ebx,dword [esp+44] ;rgbTable
	mov ebp,dword [esp+32] ;height
	align 16
icpalop:
	mov ecx,dword [esp+28] ;width
	ALIGN 16
icpalop2:
	movdqu xmm7,[esi]
	pextrw edx,xmm7,0
	movq xmm1,[ebx+edx*8+1310720]
	pextrw edx,xmm7,1
	movq xmm0,[ebx+edx*8+786432]
	paddsw xmm1,xmm0
	pextrw edx,xmm7,2
	movq xmm0,[ebx+edx*8+262144]
	paddsw xmm1,xmm0

	pextrw edx,xmm7,4
	movq xmm2,[ebx+edx*8+1310720]
	pextrw edx,xmm7,5
	movq xmm0,[ebx+edx*8+786432]
	paddsw xmm2,xmm0
	pextrw edx,xmm7,6
	movq xmm0,[ebx+edx*8+262144]
	paddsw xmm2,xmm0
	punpcklqdq xmm1,xmm2
	pextrw eax,xmm7,3
	pextrw edx,xmm7,7
	shl edx,16
	or edx,eax
	movd xmm3,edx
	punpcklwd xmm3, xmm3
	punpckldq xmm3, xmm3
	pmulhuw xmm1, xmm3

	pextrw eax,xmm1,2
	movzx edx,byte [ebx+eax+131072]
	mov dh,byte [esi+3]
	pextrw eax,xmm1,1
	shl edx,16
	mov dh,byte [ebx+eax+65536]
	pextrw eax,xmm1,0
	mov dl,byte [ebx+eax]
	movnti dword [edi],edx

	pextrw eax,xmm1,6
	movzx edx,byte [ebx+eax+131072]
	mov dh,byte [esi+7]
	pextrw eax,xmm1,5
	shl edx,16
	mov dh,byte [ebx+eax+65536]
	pextrw eax,xmm1,4
	mov dl,byte [ebx+eax]
	movnti dword [edi+4],edx

	lea esi,[esi+16]
	lea edi,[edi+8]
	dec ecx
	jnz icpalop2

	add esi,dword [esp+36] ;sstep
	add edi,dword [esp+40] ;dstep
	dec ebp
	jnz icpalop
	
	pop edi
	pop esi
	pop ebx
	pop ebp
	ret
 