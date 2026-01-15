section .text

global _DeinterlaceLR_VerticalFilter
global _DeinterlaceLR_VerticalFilterOdd
global _DeinterlaceLR_VerticalFilterEven

;void DeinterlaceLR_VerticalFilter(UInt8 *inPt, UInt8 *outPt, IntOS width, IntOS height, IntOS tap, IntOS *index, Int64 *weight, IntOS sstep, IntOS dstep)
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

	align 16
_DeinterlaceLR_VerticalFilter:
	push ebp
	push ebx
	push esi
	push edi
	
	mov eax,dword [esp+36] ;tap
	cmp eax,6
	jz vf6start
	cmp eax,4
	jz vf4start
	jmp vfstart
	
	align 16
vf6start:	
	mov eax,dword [esp+28] ;width
	mov esi,dword [esp+24] ;outPt
	pxor xmm3,xmm3
	shl eax,3
	sub dword [esp+52],eax ;dstep
	mov ebx,dword [esp+40] ;index
	mov edi,dword [esp+44] ;weight

	align 16
vf6lop:
	mov ecx,dword [esp+20] ;inPt
	mov ebp,dword [esp+28] ;width
	
	ALIGN 16
vf6lop2:

	pxor xmm1,xmm1
	mov eax,dword [ebx]
	movq xmm0,[ecx+eax]
	mov eax,dword [ebx+4]
	movq xmm2,[ecx+eax]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[edi]
	paddd xmm1,xmm0
	mov eax,dword [ebx+8]
	movq xmm0,[ecx+eax]
	mov eax,dword [ebx+12]
	movq xmm2,[ecx+eax]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[edi+16]
	paddd xmm1,xmm0
	mov eax,dword [ebx+16]
	movq xmm0,[ecx+eax]
	mov eax,dword [ebx+20]
	movq xmm2,[ecx+eax]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[edi+32]
	paddd xmm1,xmm0

	psrad xmm1,15
	packssdw xmm1,xmm3
	movq [esi],xmm1
	lea esi,[esi+8]
	lea ecx,[ecx+8]
	dec ebp
	jnz vf6lop2

	lea ebx,[ebx+24] ;index
	lea edi,[edi+48] ;weight
	add esi,dword [esp+52] ;dstep

	dec dword [esp+32] ;currHeight
	jnz vf6lop
	jmp vfexit
	
	align 16
vf4start:
	mov eax,dword [esp+28] ;width
	mov ebx,dword [esp+40] ;index
	mov edi,dword [esp+44] ;weight
	shl eax,3
	mov esi,dword [esp+24] ;outPt
	sub dword [esp+52],eax ;dstep
	pxor xmm3,xmm3

	align 16
vf4lop:
	mov ecx,dword [esp+20] ;inPt

	movdqa xmm6,[edi]
	movdqa xmm7,[edi+16]

	mov ebp,dword [esp+28] ;width
	ALIGN 16
vf4lop2:
	pxor xmm1,xmm1
	mov eax,dword [ebx]
	movq xmm0,[ecx+eax]
	mov eax,dword [ebx+4]
	movq xmm2,[ecx+eax]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,xmm6
	paddd xmm1,xmm0
	mov eax,dword [ebx+8]
	movq xmm0,[ecx+eax]
	mov eax,dword [ebx+12]
	movq xmm2,[ecx+eax]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,xmm7
	paddd xmm1,xmm0

	psrad xmm1,15
	packssdw xmm1,xmm3
	movq [esi],xmm1
	lea esi,[esi+8]
	lea ecx,[ecx+8]
	dec ebp
	jnz vf4lop2

	lea ebx,[ebx+16]
	lea edi,[edi+32]
	add esi,[esp+52] ;dstep

	dec dword [esp+32] ;currHeight
	jnz vf4lop
	jmp vfexit

	align 16
vfstart:
	pxor xmm3,xmm3

	align 16
vflop:
	mov esi,dword [esp+24] ;outPt
	mov ecx,dword [esp+20] ;inPt

	mov ebp,dword [esp+28] ;width
	align 16
vflop2:
	mov ebx,dword [esp+40] ;index
	mov edi,dword [esp+44] ;weight

	mov edx,dword [esp+36] ;tap
	pxor xmm1,xmm1
	ALIGN 16
vflop3:
	mov eax,dword [ebx]
	movq xmm0,[ecx+eax]
	mov eax,dword [ebx+4]
	movq xmm2,[ecx+eax]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[edi]
	paddd xmm1,xmm0
	add ebx,8
	add edi,16
	sub edx,2
	jnz vflop3

	psrad xmm1,15
	packssdw xmm1,xmm3
	movq [esi],xmm1
	lea esi,[esi+8]
	lea ecx,[ecx+8]
	dec ebp
	jnz vflop2

	mov eax,dword [esp+36] ;tap
	lea edx,[eax*8]
	shl eax,2
	add dword [esp+40],eax ;index
	add dword [esp+44],edx ;weight

	mov edx,dword [esp+52] ;dstep
	add dword [esp+24],edx ;outPt

	dec dword [esp+32] ;currHeight
	jnz vflop

	align 16
vfexit:
	pop edi
	pop esi
	pop ebx
	pop ebp
	ret

;void DeinterlaceLR_VerticalFilterOdd(UInt8 *inPt, UInt8 *inPtCurr, UInt8 *outPt, IntOS width, IntOS height, IntOS tap, IntOS *index, Int64 *weight, IntOS sstep, IntOS dstep)
;0 edi
;4 esi
;8 ebx
;12 ebp
;16 retAddr
;20 inPt
;24 inPtCurr
;28 outPt
;32 width
;36 height
;40 tap
;44 index
;48 weight
;52 sstep
;56 dstep

	align 16
_DeinterlaceLR_VerticalFilterOdd:
	push ebp
	push ebx
	push esi
	push edi
	mov eax,dword [esp+40] ;tap
	cmp eax,6
	jz vfo6start
	cmp eax,4
	jz vfo4start
	jmp vfostart

	align 16
vfo6start:
	mov eax,dword [esp+36] ;height
	mov edx,dword [esp+32] ;width
	shr eax,1
	shl edx,3
	mov dword [esp+36],eax ;currHeight
	sub dword [esp+56],edx ;dstep
	pxor xmm3,xmm3

	add dword [esp+44],24 ;index 4 * 6
	add dword [esp+48],48 ;weight 8 * 6
	mov edi,dword [esp+28] ;outPt
	mov ebx,dword [esp+44] ;index

	align 16
vfo6lop:
	mov esi,dword [esp+24] ;inPtCurr

	mov ebp,dword [esp+32] ;width
	lea ecx,[ebp*2]
	rep movsd
	mov ecx,dword [esp+52] ;sstep
	add edi,dword [esp+56] ;dstep
	add dword [esp+24],ecx ;inPtCurr

	mov ecx,dword [esp+48] ;weight
	movdqa xmm4, [ecx]
	movdqa xmm5, [ecx+16]
	movdqa xmm6, [ecx+32]

	mov esi,dword [esp+20] ;inPt
	ALIGN 16
vfo6lop2:
	pxor xmm1,xmm1
	mov eax,dword [ebx]
	mov edx,dword [ebx+4]
	movq xmm0,[esi+eax]
	movq xmm2,[esi+edx]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,xmm4
	paddd xmm1,xmm0
	mov eax,dword [ebx+8]
	mov edx,dword [ebx+12]
	movq xmm0,[esi+eax]
	movq xmm2,[esi+edx]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,xmm5
	paddd xmm1,xmm0
	mov eax,dword [ebx+16]
	mov edx,dword [ebx+20]
	movq xmm0,[esi+eax]
	movq xmm2,[esi+edx]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,xmm6
	paddd xmm1,xmm0

	psrad xmm1,15
	packssdw xmm1,xmm3
	movq [edi],xmm1
	lea esi,[esi+8]
	lea edi,[edi+8]
	dec ebp
	jnz vfo6lop2

	add ebx,48 ;4 * 6 * 2
	add dword [esp+48],96 ;weight 8 * 6 * 2
	add edi,[esp+56] ;dstep

	dec dword [esp+36] ;currHeight
	jnz vfo6lop
	jmp vfoexit

	align 16
vfo4start:
	test dword [esp+32],7
	jz vfo4start2			;if (width & 7)
	
	mov eax,dword [esp+36] ;height
	shr eax,1
	mov dword [esp+36],eax ;currHeight
	shr dword [esp+32],1 ;width

	mov ebx,dword [esp+44] ;index
	add dword [esp+48],32 ;weight 8 * 4
	add ebx,16 ;4 * 4

	align 16
vfo4_7lop:
	mov edi,dword [esp+28] ;outPt
	mov esi,dword [esp+24] ;inPtCurr

	mov eax,dword [esp+32] ;width
	mov ecx,eax
	shr eax,2
	jz vfo4_7lop4
	ALIGN 16
vfo4_7lop3:
	movdqu xmm0,[esi]
	movdqu xmm1,[esi+16]
	movdqu xmm2,[esi+32]
	movdqu xmm3,[esi+48]
	movntdq [edi],xmm0
	movntdq [edi+16],xmm1
	movntdq [edi+32],xmm2
	movntdq [edi+48],xmm3
	lea esi,[esi+64]
	lea edi,[edi+64]
	dec eax
	jnz vfo4_7lop3
	align 16
vfo4_7lop4:
	and ecx,3
	jz vfo4_7lop6
	ALIGN 16
vfo4_7lop5:
	movdqu xmm0,[esi]
	movntdq [edi],xmm0
	lea esi,[esi+16]
	lea edi,[edi+16]
	dec ecx
	jnz vfo4_7lop5
	align 16
vfo4_7lop6:
	mov edi,dword [esp+28] ;outPt
	mov ecx,dword [esp+52] ;sstep
	add edi,dword [esp+56] ;dstep
	add dword [esp+24],ecx ;inPtCurr

	mov ecx,dword [esp+48] ;weight

	movdqa xmm4,[ecx]
	movdqa xmm5,[ecx+16]

	mov esi,dword [esp+20] ;inPt
	mov ecx,dword [esp+32] ;width
	ALIGN 16
vfo4_7lop2:
	pxor xmm1,xmm1
	pxor xmm6,xmm6

	mov eax,dword [ebx]
	mov edx,dword [ebx+4]
	movdqa xmm0,[esi+eax]
	movdqa xmm2,[esi+edx]
	movdqa xmm3,xmm0
	punpcklwd xmm0,xmm2
	punpckhwd xmm3,xmm2
	pmaddwd xmm0, xmm4
	pmaddwd xmm3, xmm4
	paddd xmm1,xmm0
	paddd xmm6,xmm3
	mov eax,dword [ebx+8]
	mov edx,dword [ebx+12]
	movdqa xmm0,[esi+eax]
	movdqa xmm2,[esi+edx]
	movdqa xmm3,xmm0
	punpcklwd xmm0,xmm2
	punpckhwd xmm3,xmm2
	pmaddwd xmm0, xmm5
	pmaddwd xmm3, xmm5
	paddd xmm1,xmm0
	paddd xmm6,xmm3

	psrad xmm1,15
	psrad xmm6,15
	packssdw xmm1,xmm6
	movntdq [edi],xmm1
	add esi,16
	add edi,16
	dec ecx
	jnz vfo4_7lop2

	add ebx,32 ;4 * 4 * 2
	add dword [esp+48],64 ;weight 8 * 4 * 2

	mov edx,dword [esp+56] ;dstep
	shl edx,1
	add dword [esp+28],edx ;outPt

	dec dword [esp+36] ;currHeight
	jnz vfo4_7lop
	jmp vfoexit

	align 16
vfo4start2:
	mov eax,dword [esp+36] ;height
	mov edx,dword [esp+32] ;width
	shr eax,1
	mov dword [esp+36],eax ;currHeight
	lea ecx,[edx*8]
	shr edx,1
	sub dword [esp+56],ecx ;dstep
	sub dword [esp+52],ecx ;sstep
	mov dword [esp+32],edx ;width

	mov edi,dword [esp+28] ;outPt
	mov ebp,dword [esp+48] ;weight
	mov ebx,dword [esp+44] ;index
	mov edx,dword [esp+24] ;inPtCurr
	lea ebp,[ebp+32] ;8 * 4
	lea ebx,[ebx+16] ;4 * 4

	ALIGN 16
vfo4lopa:
	mov eax,dword [esp+32] ;width
	shr eax,2
	ALIGN 16
vfo4lopa3:
	movdqu xmm0,[edx]
	movdqu xmm1,[edx+16]
	movdqu xmm2,[edx+32]
	movdqu xmm3,[edx+48]
	movntdq [edi],xmm0
	movntdq [edi+16],xmm1
	movntdq [edi+32],xmm2
	movntdq [edi+48],xmm3
	lea edx,[edx+64]
	lea edi,[edi+64]
	dec eax
	jnz vfo4lopa3

	add edx,dword [esp+52] ;sstep
	add edi,dword [esp+56] ;dstep

	movdqa xmm4,[ebp]
	movdqa xmm5,[ebp+16]

	mov esi,dword [esp+20] ;inPt
	mov ecx,dword [esp+32] ;width
	ALIGN 16
vfo4lopa2:
	pxor xmm1,xmm1
	pxor xmm6,xmm6

	mov eax,dword [ebx]
	movdqa xmm0,[esi+eax]
	mov eax,dword [ebx+4]
	movdqa xmm2,[esi+eax]
	movdqa xmm3,xmm0
	punpcklwd xmm0,xmm2
	punpckhwd xmm3,xmm2
	pmaddwd xmm0, xmm4
	pmaddwd xmm3, xmm4
	paddd xmm1,xmm0
	paddd xmm6,xmm3
	mov eax,dword [ebx+8]
	movdqa xmm0,[esi+eax]
	mov eax,dword [ebx+12]
	movdqa xmm2,[esi+eax]
	movdqa xmm3,xmm0
	punpcklwd xmm0,xmm2
	punpckhwd xmm3,xmm2
	pmaddwd xmm0, xmm5
	pmaddwd xmm3, xmm5
	paddd xmm1,xmm0
	paddd xmm6,xmm3

	psrad xmm1,15
	psrad xmm6,15
	packssdw xmm1,xmm6
	movntdq [edi],xmm1
	lea esi,[esi+16]
	lea edi,[edi+16]
	dec ecx
	jnz vfo4lopa2

	lea ebx,[ebx+32] ;4 * 4 * 2
	lea ebp,[ebp+64] ;8 * 4 * 2

	add edi,dword [esp+56] ;dstep

	dec dword [esp+36] ;currHeight
	jnz vfo4lopa
	jmp vfoexit

	align 16
vfostart:
	pxor xmm3,xmm3

	align 16
vfolop:
	mov esi,dword [esp+28] ;outPt
	mov ecx,dword [esp+20] ;inPt

	mov ebp,dword [esp+32] ;width
	align 16
vfolop2:
	mov ebx,dword [esp+44] ;index
	mov edi,dword [esp+48] ;weight

	mov edx,dword [esp+40] ;tap
	pxor xmm1,xmm1
	ALIGN 16
vfolop3:
	mov eax,dword [ebx]
	movq xmm0,[ecx+eax]
	mov eax,dword [ebx+4]
	movq xmm2,[ecx+eax]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[edi]
	paddd xmm1,xmm0
	add ebx,8
	add edi,16
	sub edx,2
	jnz vfolop3

	psrad xmm1,15
	packssdw xmm1,xmm3
	movq [esi],xmm1
	add esi,8
	add ecx,8
	dec ebp
	jnz vfolop2

	mov eax,dword [esp+40] ;tap
	lea edx,[eax*8]
	shl eax,2
	add dword [esp+44],eax ;index
	add dword [esp+48],edx ;weight

	mov edx,dword [esp+56] ;dstep
	add dword [esp+28],edx ;outPt

	dec dword [esp+36] ;currHeight
	jnz vfolop

	align 16
vfoexit:
	pop edi
	pop esi
	pop ebx
	pop ebp
	ret


;void DeinterlaceLR_VerticalFilterEven(UInt8 *inPt, UInt8 *inPtCurr, UInt8 *outPt, IntOS width, IntOS height, IntOS tap, IntOS *index, Int64 *weight, IntOS sstep, IntOS dstep)
;0 edi
;4 esi
;8 ebx
;12 ebp
;16 retAddr
;20 inPt
;24 inPtCurr
;28 outPt
;32 width
;36 height
;40 tap
;44 index
;48 weight
;52 sstep
;56 dstep

	align 16
_DeinterlaceLR_VerticalFilterEven:
	push ebp
	push ebx
	push esi
	push edi
	mov eax,dword [esp+40] ;tap
	cmp eax,6
	jz vfe6start
	cmp eax,4
	jz vfe4start
	jmp vfestart

	align 16
vfe6start:	
	mov eax,dword [esp+36] ;height
	shr eax,1
	mov dword [esp+36],eax ;currHeight
	pxor xmm3,xmm3
	mov ebx,dword [esp+44] ;index
	mov edx,dword [esp+48] ;weight

	align 16
vfe6lop:
	mov edi,dword [esp+28] ;outPt
	mov esi,dword [esp+20] ;inPt

	movdqa xmm4,[edx]
	movdqa xmm5,[edx+16]
	movdqa xmm6,[edx+32]

	mov ebp,dword [esp+32] ;width
	ALIGN 16
vfe6lop2:
	pxor xmm1,xmm1
	mov eax,dword [ebx]
	movq xmm0,[esi+eax]
	mov eax,dword [ebx+4]
	movq xmm2,[esi+eax]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,xmm4
	paddd xmm1,xmm0
	mov eax,dword [ebx+8]
	movq xmm0,[esi+eax]
	mov eax,dword [ebx+12]
	movq xmm2,[esi+eax]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,xmm5
	paddd xmm1,xmm0
	mov eax,dword [ebx+16]
	movq xmm0,[esi+eax]
	mov eax,dword [ebx+20]
	movq xmm2,[esi+eax]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,xmm6
	paddd xmm1,xmm0

	psrad xmm1,15
	packssdw xmm1,xmm3
	movq [edi],xmm1
	add esi,8
	add edi,8
	dec ebp
	jnz vfe6lop2

	add ebx,48 ;4 * 6 * 2
	add edx,96 ;8 * 6 * 2

	mov ecx,dword [esp+56] ;dstep
	add dword [esp+28],ecx ;outPt

	mov esi,dword [esp+24] ;inPtCurr
	mov edi,dword [esp+28] ;outPt
	mov ecx,dword [esp+32] ;width
	mov eax,dword [esp+52] ;sstep
	shl ecx,1
	rep movsd
	mov ecx,dword [esp+56] ;dstep
	add dword [esp+24],eax ;inPtCurr
	add dword [esp+28],ecx ;outPt

	dec dword [esp+36] ;currHeight
	jnz vfe6lop
	jmp vfeexit

	align 16
vfe4start:
	test dword [esp+32],7 ;width
	jz vfe4start2								;if (width & 7)

	mov eax,dword [esp+36] ;height
	shr eax,1
	mov dword [esp+36],eax ;currHeight

	mov ebx,dword [esp+44] ;index
	mov edx,dword [esp+48] ;weight
	ALIGN 16
vfe4_7lop:
	mov edi,dword [esp+28] ;outPt
	mov esi,dword [esp+20] ;inPt

	movdqa xmm4,[ecx]
	movdqa xmm5,[ecx+16]

	mov ecx,dword [esp+32] ;width
	shr ecx,1
	ALIGN 16
vfe4_7lop2:
	pxor xmm1,xmm1
	pxor xmm6,xmm6

	mov eax,dword [ebx]
	movdqa xmm0,[esi+eax]
	mov eax,dword [ebx+4]
	movdqa xmm2,[esi+eax]
	movdqa xmm3,xmm0
	punpcklwd xmm0,xmm2
	punpckhwd xmm3,xmm2
	pmaddwd xmm0,xmm4
	pmaddwd xmm3,xmm4
	paddd xmm1,xmm0
	paddd xmm6,xmm3
	mov eax,dword [ebx+8]
	movdqa xmm0,[esi+eax]
	mov eax,dword [ebx+12]
	movdqa xmm2,[esi+eax]
	movdqa xmm3,xmm0
	punpcklwd xmm0,xmm2
	punpckhwd xmm3,xmm2
	pmaddwd xmm0,xmm5
	pmaddwd xmm3,xmm5
	paddd xmm1,xmm0
	paddd xmm6,xmm3

	psrad xmm1,15
	psrad xmm6,15
	packssdw xmm1,xmm6
	movdqa [edi],xmm1
	add esi,16
	add edi,16
	dec ecx
	jnz vfe4_7lop2

	add ebx,32 ;4 * 4 * 2
	add edx,64 ;8 * 4 * 2

	mov eax,dword [esp+56] ;dstep
	add dword [esp+28],eax ;outPt

	mov esi,dword [esp+24] ;inPtCurr
	mov edi,dword [esp+28] ;outPt
	mov ecx,dword [esp+32] ;width
	mov eax,ecx
	shr ecx,2
	jz vfe4_7lop4
	ALIGN 16
vfe4_7lop3:
	movdqu xmm0,[esi]
	movdqu xmm1,[esi+16]
	movdqu xmm2,[esi+32]
	movdqu xmm3,[esi+48]
	movntdq [edi],xmm0
	movntdq [edi+16],xmm1
	movntdq [edi+32],xmm2
	movntdq [edi+48],xmm3
	add esi,64
	add edi,64
	dec ecx
	jnz vfe4_7lop3
	align 16
vfe4_7lop4:
	and eax,3
	jz vfe4_7lop6
	ALIGN 16
vfe4_7lop5:
	movdqu xmm0,[esi]
	movntdq [edi],xmm0
	add esi,16
	add edi,16
	dec eax
	jnz vfe4_7lop5
	align 16
vfe4_7lop6:
	mov eax,dword [esp+52] ;sstep
	mov ecx,dword [esp+56] ;dstep
	add dword [esp+24],eax ;inPtCurr
	add dword [esp+28],ecx ;outPt

	dec dword [esp+36] ;currHeight
	jnz vfe4_7lop
	jmp vfeexit

	align 16
vfe4start2:
	mov eax,dword [esp+36] ;height
	shr eax,1
	mov dword [esp+36],eax ;currHeight
	shr dword [esp+32],1 ;width

	mov edx,dword [esp+28] ;outPt
	mov ebp,dword [esp+48] ;weight
	mov ebx,dword [esp+44] ;index
	ALIGN 16
vfe4lopa:
	mov edi,edx
	mov esi,dword [esp+20] ;inPt

	movdqa xmm4,[ebp]
	movdqa xmm5,[ebp+16]

	mov ecx,dword [esp+32] ;width
	ALIGN 16
vfe4lopa2:
	pxor xmm1,xmm1
	pxor xmm6,xmm6

	mov eax,dword [ebx]
	movdqa xmm0,[esi+eax]
	mov eax,dword [ebx+4]
	movdqa xmm2,[esi+eax]
	movdqa xmm3,xmm0
	punpcklwd xmm0,xmm2
	punpckhwd xmm3,xmm2
	pmaddwd xmm0,xmm4
	pmaddwd xmm3,xmm4
	paddd xmm1,xmm0
	paddd xmm6,xmm3
	mov eax,dword [ebx+8]
	movdqa xmm0,[esi+eax]
	mov eax,dword [ebx+12]
	movdqa xmm2,[esi+eax]
	movdqa xmm3,xmm0
	punpcklwd xmm0,xmm2
	punpckhwd xmm3,xmm2
	pmaddwd xmm0,xmm5
	pmaddwd xmm3,xmm5
	paddd xmm1,xmm0
	paddd xmm6,xmm3

	psrad xmm1,15
	psrad xmm6,15
	packssdw xmm1,xmm6
	movntdq [edi],xmm1
	lea esi,[esi+16]
	lea edi,[edi+16]
	dec ecx
	jnz vfe4lopa2

	lea ebx,[ebx+32] ;4 * 4 * 2
	lea ebp,[ebp+64] ;8 * 4 * 2

	add edx,dword [esp+56] ;dstep

	mov esi,dword [esp+24] ;inPtCurr
	mov edi,edx
	mov ecx,dword [esp+32] ;width
	shr ecx,2
	ALIGN 16
vfe4lopa3:
	movdqu xmm0,[esi]
	movdqu xmm1,[esi+16]
	movdqu xmm2,[esi+32]
	movdqu xmm3,[esi+48]
	movntdq [edi],xmm0
	movntdq [edi+16],xmm1
	movntdq [edi+32],xmm2
	movntdq [edi+48],xmm3
	lea esi,[esi+64]
	lea edi,[edi+64]
	dec ecx
	jnz vfe4lopa3

	mov eax,dword [esp+52] ;sstep
	add edx,dword [esp+56] ;dstep
	add dword [esp+24],eax ;inPtCurr

	dec dword [esp+36] ;currHeight
	jnz vfe4lopa
	jmp vfeexit

	align 16
vfestart:
	pxor xmm3,xmm3

	align 16
vfelop:
	mov esi,dword [esp+28] ;outPt
	mov ecx,dword [esp+20] ;inPt

	mov ebp,dword [esp+32] ;width
	align 16
vfelop2:
	mov ebx,dword [esp+44] ;index
	mov edi,dword [esp+48] ;weight

	mov edx,dword [esp+40] ;tap
	pxor xmm1,xmm1
	ALIGN 16
vfelop3:
	mov eax,dword [ebx]
	movq xmm0,[ecx+eax]
	mov eax,dword [ebx+4]
	movq xmm2,[ecx+eax]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[edi]
	paddd xmm1,xmm0
	add ebx,8
	add edi,16
	sub edx,2
	jnz vflop3

	psrad xmm1,15
	packssdw xmm1,xmm3
	movq [esi],xmm1
	add esi,8
	add ecx,8
	dec ebp
	jnz vfelop2

	mov eax,dword [esp+40] ;tap
	lea edx,[eax*8]
	shl eax,2
	add dword [esp+44],eax ;index
	add dword [esp+48],edx ;weight

	mov edx,dword [esp+56] ;dstep
	add dword [esp+28],edx ;outPt

	dec dword [esp+36] ;currHeight
	jnz vfelop
	
	align 16
vfeexit:
	pop edi
	pop esi
	pop ebx
	pop ebp
	ret
