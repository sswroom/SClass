section .text

global _LanczosResizerLR_C16_horizontal_filter
global _LanczosResizerLR_C16_vertical_filter
global _LanczosResizerLR_C16_collapse

;void LanczosResizerLR_C16_horizontal_filter(UInt8 *inPt, UInt8 *outPt, IntOS width, IntOS height, Int32 tap, Int32 *index, Int64 *weight, IntOS sstep, IntOS dstep, UInt8 *rgbTable)
;0 edi
;4 esi
;8 ebx
;12 ebp
;16 retAddr
;20 inPt
;24 outPt
;28 width
;32 height
;36 tap
;40 index
;44 weight
;48 sstep
;52 dstep
;56 rgbTable

	align 16
_LanczosResizerLR_C16_horizontal_filter:
	push ebp
	push ebx
	push esi
	push edi
	test dword [esp+28],1 ;width
	jnz hfstart
	mov eax,dword [esp+36] ;tap
	cmp eax,6
	jz hf6start
	jmp hfstart2
	
	align 16
hfstart:							;if (width & 1)
	shr dword [esp+36],1 ;tap
	pxor xmm3,xmm3
	align 16
hflop:
	mov esi,dword [esp+24] ;outPt
	mov ebp,dword [esp+28] ;width
	mov edi,dword [esp+44] ;weight
	mov ebx,dword [esp+40] ;index
	align 16
hflop2:
	mov ecx,dword [esp+20] ;inPt
	mov edx,dword [esp+36] ;tap
	pxor xmm1,xmm1
	ALIGN 16
hflop3:
	mov eax,dword [ebx]
	movq xmm0,[ecx+eax]
	mov eax,dword [ebx+4]
	movq xmm2,[ecx+eax]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[edi]
	paddd xmm1,xmm0
	lea edi,[edi+16]
	lea ebx,[ebx+8]
	dec edx
	jnz hflop3

	psrad xmm1,15
	packssdw xmm1,xmm3
	movq [esi],xmm1
	lea esi,[esi+8]
	dec ebp
	jnz hflop2

	mov eax,dword [esp+48] ;sstep
	mov edx,dword [esp+52] ;dstep
	add dword [esp+20],eax ;inPt
	add dword [esp+24],edx ;outPt

	dec dword [esp+32] ;currHeight
	jnz hflop
	jmp hfexit

	align 16
hf6start:								;else if (tap == 6)
	test dword [esp+24],15 ;outPt
	jnz hf6nstart
	test dword [esp+52],15 ;dstep
	jnz hf6nstart
	jmp hf6start2
	
	align 16
hf6nstart:								;if ((((IntOS)outPt) & 15) == 0 && (dstep & 15) == 0)
	shr dword [esp+28],1 ;width
	align 16
hf6nlop4:
	mov esi,dword [esp+24] ;outPt
	mov ecx,dword [esp+20] ;inPt

	mov ebp,dword [esp+28] ;width
	mov ebx,dword [esp+40] ;index
	mov edi,dword [esp+44] ;weight
	ALIGN 16
hf6nlop5:
	pxor xmm1,xmm1
	mov eax,dword [ebx]
	mov edx,dword [ebx+4]
	movq xmm0,[ecx+eax]
	movq xmm2,[ecx+edx]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[edi]
	paddd xmm1,xmm0
	mov eax,dword [ebx+8]
	mov edx,dword [ebx+12]
	movq xmm0,[ecx+eax]
	movq xmm2,[ecx+edx]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[edi+16]
	paddd xmm1,xmm0
	mov eax,dword [ebx+16]
	mov edx,dword [ebx+20]
	movq xmm0,[ecx+eax]
	movq xmm2,[ecx+edx]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[edi+32]
	paddd xmm1,xmm0
	lea edi,[edi+48]
	lea ebx,[ebx+24]

	pxor xmm3,xmm3
	mov eax,dword [ebx]
	mov edx,dword [ebx+4]
	movq xmm0,[ecx+eax]
	movq xmm2,[ecx+edx]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[edi]
	paddd xmm3,xmm0
	mov eax,dword [ebx+8]
	mov edx,dword [ebx+12]
	movq xmm0,[ecx+eax]
	movq xmm2,[ecx+edx]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[edi+16]
	paddd xmm3,xmm0
	mov eax,dword [ebx+16]
	mov edx,dword [ebx+20]
	movq xmm0,[ecx+eax]
	movq xmm2,[ecx+edx]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[edi+32]
	paddd xmm3,xmm0
	lea ebx,[ebx+24]
	lea edi,[edi+48]

	psrad xmm1,15
	psrad xmm3,15
	packssdw xmm1,xmm3
	movntdq [esi],xmm1

	lea esi,[esi+16]
	dec ebp
	jnz hf6nlop5

	mov eax,dword [esp+48] ;sstep
	mov edx,dword [esp+52] ;dstep
	add dword [esp+20],eax ;inPt
	add dword [esp+24],edx ;outPt

	dec dword [esp+32] ;currHeight
	jnz hf6nlop4
	jmp hfexit
	
	align 16
hf6start2:
	shr dword [esp+28],1 ;width
	align 16
hf6lop4:
	mov esi,dword [esp+24] ;outPt
	mov ebp,dword [esp+28] ;width
	mov ebx,dword [esp+40] ;index
	mov edi,dword [esp+44] ;weight
	mov ecx,dword [esp+20] ;inPt
	ALIGN 16
hf6lop5:
	pxor xmm1,xmm1
	mov eax,dword [ebx]
	mov edx,dword [ebx+4]
	movq xmm0,[ecx+eax]
	movq xmm2,[ecx+edx]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[edi]
	paddd xmm1,xmm0
	mov eax,dword [ebx+8]
	mov edx,dword [ebx+12]
	movq xmm0,[ecx+eax]
	movq xmm2,[ecx+edx]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[edi+16]
	paddd xmm1,xmm0
	mov eax,dword [ebx+16]
	mov edx,dword [ebx+20]
	movq xmm0,[ecx+eax]
	movq xmm2,[ecx+edx]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[edi+32]
	paddd xmm1,xmm0
	lea edi,[edi+48]
	lea ebx,[ebx+24]

	pxor xmm3,xmm3
	mov eax,dword [ebx]
	mov edx,dword [ebx+4]
	movq xmm0,[ecx+eax]
	movq xmm2,[ecx+edx]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[edi]
	paddd xmm3,xmm0
	mov eax,dword [ebx+8]
	mov edx,dword [ebx+12]
	movq xmm0,[ecx+eax]
	movq xmm2,[ecx+edx]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[edi+16]
	paddd xmm3,xmm0
	mov eax,dword [ebx+16]
	mov edx,dword [ebx+20]
	movq xmm0,[ecx+eax]
	movq xmm2,[ecx+edx]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[edi+32]
	paddd xmm3,xmm0
	lea edi,[edi+48]
	lea ebx,[ebx+24]

	psrad xmm1,15
	psrad xmm3,15
	packssdw xmm1,xmm3
	movdqu [esi],xmm1

	lea esi,[esi+16]
	dec ebp
	jnz hf6lop5

	mov eax,dword [esp+48] ;sstep
	mov edx,dword [esp+52] ;dstep
	add dword [esp+20],eax ;inPt
	add dword [esp+24],edx ;outPt

	dec dword [esp+32] ;currHeight
	jnz hf6lop4
	jmp hfexit

	align 16
hfstart2:
	shr dword [esp+36],1 ;tap
	shr dword [esp+28],1 ;width
	align 16
hflop4:
	mov ecx,dword [esp+20] ;inPt
	mov esi,dword [esp+24] ;outPt
	mov ebp,dword [esp+28] ;width
	mov ebx,dword [esp+40] ;index
	mov edi,dword [esp+44] ;weight
	align 16
hflop5:
	mov edx,dword [esp+36] ;tap
	pxor xmm1,xmm1
	ALIGN 16
hflop6:
	mov eax,dword [ebx]
	movq xmm0,[ecx+eax]
	mov eax,dword [ebx+4]
	movq xmm2,[ecx+eax]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[edi]
	paddd xmm1,xmm0
	lea edi,[edi+16]
	lea ebx,[ebx+8]
	dec edx
	jnz hflop6

	mov edx,dword [esp+36] ;tap
	pxor xmm3,xmm3
	ALIGN 16
hflop7:
	mov eax,dword [ebx]
	movq xmm0,[ecx+eax]
	mov eax,dword [ebx+4]
	movq xmm2,[ecx+eax]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[edi]
	paddd xmm3,xmm0
	lea edi,[edi+16]
	lea ebx,[ebx+8]
	dec edx
	jnz hflop7

	psrad xmm1,15
	psrad xmm3,15
	packssdw xmm1,xmm3
	movdqu [esi],xmm1

	lea esi,[esi+16]
	dec ebp
	jnz hflop5

	mov eax,dword [esp+48] ;sstep
	mov edx,dword [esp+52] ;dstep
	add dword [esp+20],eax ;inPt
	add dword [esp+24],edx ;outPt

	dec dword [esp+32] ;currHeight
	jnz hflop4

	align 16
hfexit:
	pop edi
	pop esi
	pop ebx
	pop ebp
	ret	

;void LanczosResizerLR_C16_vertical_filter(UInt8 *inPt, UInt8 *outPt, IntOS width, IntOS height, Int32 tap, Int32 *index, Int64 *weight, IntOS sstep, IntOS dstep, UInt8 *rgbTable)
;0 currWidth
;4 edi
;8 esi
;12 ebx
;16 ebp
;20 retAddr
;24 inPt
;28 outPt
;32 width
;36 height
;40 tap
;44 index
;48 weight
;52 sstep
;56 dstep
;60 rgbTable

	align 16
_LanczosResizerLR_C16_vertical_filter:
	push ebp
	push ebx
	push esi
	push edi
	push eax
	test dword [esp+32],1 ;width
	jnz vfstart
	mov eax,dword [esp+40]
	cmp eax,6
	jz vf6start
	jmp vfstart2

	align 16
vfstart:					;if (width & 1)
	shr dword [esp+40],1 ;tap
	pxor xmm3,xmm3
	align 16
vflop:
	mov ecx,dword [esp+28] ;outPt
	mov esi,dword [esp+24] ;inPt

	mov eax,dword [esp+32] ;width
	mov ebp,dword [esp+60] ;rgbTable
	mov dword [esp+0],eax ;currWidth
	align 16
vflop2:
	mov ebx,dword [esp+44] ;index
	mov edi,dword [esp+48] ;weight

	mov edx,dword [esp+40] ;tap
	pxor xmm1,xmm1
	ALIGN 16
vflop3:
	mov eax,dword [ebx]
	movq xmm0,[esi+eax]
	mov eax,dword [ebx+4]
	movq xmm2,[esi+eax]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[edi]
	paddd xmm1,xmm0
	add ebx,8
	add edi,16
	dec edx
	jnz vflop3

	psrad xmm1,15
	packssdw xmm1,xmm3
	pextrw edx,xmm1,0
	mov ax,word [ebp+edx*2]
	pextrw edx,xmm1,1
	or ax,word [ebp+edx*2+131072]
	pextrw edx,xmm1,2
	or ax,word [ebp+edx*2+262144]

	mov word [ecx],ax
	add ecx,2
	add esi,8
	dec dword [esp+0] ;currWidth
	jnz vflop2

	mov eax,dword [esp+40] ;tap
	lea edx,[eax*8]
	shl eax,4
	add dword [esp+44],edx ;index
	add dword [esp+48],eax ;weight

	mov edx,dword [esp+56] ;dstep
	add dword [esp+28],edx ;outPt

	dec dword [esp+36] ;currHeight
	jnz vflop
	jmp vfexit

	align 16
vf6start:									;else if (tap == 6)
	shr dword [esp+32],1 ;width
	mov ebx,dword [esp+44] ;index
	mov edi,dword [esp+48] ;weight
	mov ebp,dword [esp+60] ;rgbTable
	align 16
vf6lop4:
	mov ecx,dword [esp+28] ;outPt
	mov esi,dword [esp+24] ;inPt
	movdqa xmm5,[edi]
	movdqa xmm6,[edi+16]
	movdqa xmm7,[edi+32]

	mov eax,dword [esp+32] ;width
	mov dword [esp+0],eax ;currWidth
	ALIGN 16
vf6lop5:
	pxor xmm2,xmm2
	pxor xmm3,xmm3
	mov eax,dword [ebx]
	mov edx,dword [ebx+4]
	movdqu xmm0,[esi+eax]
	movdqu xmm1,[esi+edx]
	movdqu xmm4,xmm0
	punpcklwd xmm0, xmm1
	punpckhwd xmm4, xmm1
	pmaddwd xmm0,xmm5
	pmaddwd xmm4,xmm5
	paddd xmm2,xmm0
	paddd xmm3,xmm4
	mov eax,dword [ebx+8]
	mov edx,dword [ebx+12]
	movdqu xmm0,[esi+eax]
	movdqu xmm1,[esi+edx]
	movdqu xmm4,xmm0
	punpcklwd xmm0, xmm1
	punpckhwd xmm4, xmm1
	pmaddwd xmm0,xmm6
	pmaddwd xmm4,xmm6
	paddd xmm2,xmm0
	paddd xmm3,xmm4
	mov eax,dword [ebx+16]
	mov edx,dword [ebx+20]
	movdqu xmm0,[esi+eax]
	movdqu xmm1,[esi+edx]
	movdqu xmm4,xmm0
	punpcklwd xmm0, xmm1
	punpckhwd xmm4, xmm1
	pmaddwd xmm0,xmm7
	pmaddwd xmm4,xmm7
	paddd xmm2,xmm0
	paddd xmm3,xmm4

	pxor xmm4,xmm4

	psrad xmm3,15
	packssdw xmm3,xmm4
	pextrw edx,xmm3,0
	mov ax,word [ebp+edx*2]
	pextrw edx,xmm3,1
	or ax,word [ebp+edx*2+131072]
	pextrw edx,xmm3,2
	or ax,word [ebp+edx*2+262144]

	psrad xmm2,15
	shl eax,16
	packssdw xmm2,xmm4
	pextrw edx,xmm3,0
	mov ax,word [ebp+edx*2]
	pextrw edx,xmm3,1
	or ax,word [ebp+edx*2+131072]
	pextrw edx,xmm3,2
	or ax,word  [ebp+edx*2+262144]
	movnti dword [ecx],eax
	
	add ecx,4
	add esi,16
	dec dword [esp+0] ;currWidth
	jnz vf6lop5

	add ebx,24
	add edi,48

	mov edx,dword [esp+56] ;dstep
	add dword [esp+28],edx ;outPt

	dec dword [esp+36] ;currHeight
	jnz vf6lop4
	jmp vfexit

	align 16
vfstart2:
	shr dword [esp+40],1 ;tap
	shr dword [esp+32],1 ;width
	mov ebp,dword [esp+60] ;rgbTable

	pxor xmm3,xmm3
	align 16
vflop4:
	mov ecx,dword [esp+28] ;outPt
	mov esi,dword [esp+24] ;inPt
	mov eax,dword [esp+32] ;width
	mov dword [esp+0],eax ;currWidth

	align 16
vflop5:
	mov ebx,dword [esp+44] ;index
	mov edi,dword [esp+48] ;weight
	mov edx,dword [esp+40] ;tap
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
	movdqa xmm5,[edi]
	pmaddwd xmm0,xmm5
	pmaddwd xmm4,xmm5
	paddd xmm2,xmm0
	paddd xmm6,xmm4
	add edi,16
	add ebx,8

	dec edx
	jnz vflop6

	psrad xmm6,15
	packssdw xmm6,xmm3
	pextrw edx,xmm6,0
	mov ax,word [edi+edx*2]
	pextrw edx,xmm6,1
	or ax,word [edi+edx*2+131072]
	pextrw edx,xmm6,2
	or ax,word [edi+edx*2+262144]
	shl eax,16

	psrad xmm2,15
	packssdw xmm2,xmm3
	pextrw edx,xmm2,0
	mov ax,word [edi+edx*2]
	pextrw edx,xmm2,1
	or ax,word [edi+edx*2+131072]
	pextrw edx,xmm2,2
	or ax,word [edi+edx*2+262144]

	movnti dword [ecx],eax
	add ecx,4
	add esi,16
	dec dword [esp+0] ;currWidth
	jnz vflop5

	mov eax,dword [esp+40] ;tap
	shl eax,3
	lea edx,[eax*2]
	add dword [esp+44],eax ;index
	add dword [esp+48],edx ;weight

	mov edx,dword [esp+56] ;dstep
	add dword [esp+28],edx ;outPt

	dec dword [esp+36] ;currHeight
	jnz vflop4
		
	align 16
vfexit:
	pop eax
	pop edi
	pop esi
	pop ebx
	pop ebp
	ret

;void LanczosResizerLR_C16_collapse(UInt8 *inPt, UInt8 *outPt, Int32 width, Int32 height, Int32 sstep, Int32 dstep, UInt8 *rgbTable)
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
_LanczosResizerLR_C16_collapse:
	push ebp
	push ebx
	push esi
	push edi
	
	mov eax,dword [esp+28] ;width
	lea edx,[eax*8]
	shl eax,1
	sub dword [esp+36],edx ;sstep
	sub dword [esp+40],eax ;dstep
	mov ebx,dword [esp+44] ;rgbTable
	mov ecx,dword [esp+20] ;inPt
	mov esi,dword [esp+24] ;outPt
	mov ebp,dword [esp+32] ;height
collop:

	mov edi,dword [esp+28] ;width

	ALIGN 16
collop2:
	movzx edx,word [ecx+4]
	mov ax,word [ebx+edx*2+262144]
	movzx edx,word [ecx+2]
	or ax,word [ebx+edx*2+131072]
	movzx edx,word [ecx]
	or ax,word [ebx+edx*2]

	mov word [esi],ax
	lea esi,[esi+2]
	lea ecx,[ecx+8]
	dec edi
	jnz collop2

	add ecx,dword [esp+36] ;sstep
	add esi,dword [esp+40] ;dstep

	dec ebp
	jnz collop
	pop edi
	pop esi
	pop ebx
	pop ebp
	ret