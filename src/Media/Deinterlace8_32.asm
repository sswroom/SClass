section .text

global _Deinterlace8_VerticalFilter
global _Deinterlace8_VerticalFilterOdd
global _Deinterlace8_VerticalFilterEven

;void Deinterlace8_VerticalFilter(UInt8 *inPt, UInt8 *outPt, OSInt width, OSInt height, OSInt tap, Int32 *index, Int64 *weight, OSInt sstep, OSInt dstep);
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
_Deinterlace8_VerticalFilter:
	push ebp
	push ebx
	push esi
	push edi
	
	shr dword [esp+28],1 ;width
	pxor xmm6,xmm6

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
	pxor xmm3,xmm3
	ALIGN 16
vflop3:
	mov eax,dword [ebx]
	movq xmm0,[ecx+eax]
	mov eax,dword [ebx+4]
	movq xmm2,[ecx+eax]
	movdqa xmm5,[edi]
	punpcklbw xmm0,xmm0
	punpcklbw xmm2,xmm2
	movdqu xmm4,xmm0
	punpcklwd xmm0,xmm2
	punpckhwd xmm4,xmm2
	psrlw xmm0,1
	psrlw xmm4,1
	pmaddwd xmm0, xmm5
	pmaddwd xmm4, xmm5
	paddd xmm1,xmm0
	paddd xmm3,xmm4
	add ebx,8
	add edi,16
	sub edx,2
	jnz vflop3

	psrad xmm1,22
	psrad xmm3,22
	packssdw xmm1,xmm3
	packuswb xmm1,xmm6
	movq [esi],xmm1
	add esi,8
	add ecx,8
	dec ebp
	jnz vflop2

	mov eax,dword [esp+36] ;tap
	lea ebx,[eax*4]
	lea edi,[eax*8]
	add dword [esp+40],ebx ;index
	add dword [esp+44],edi ;weight
	mov eax,dword [esp+52] ;dstep
	add dword [esp+24],eax ;outPt

	dec dword [esp+32] ;currHeight
	jnz vflop
	
	pop edi
	pop esi
	pop ebx
	pop ebp
	ret

;void Deinterlace8_VerticalFilterOdd(UInt8 *inPt, UInt8 *inPtCurr, UInt8 *outPt, OSInt width, OSInt height, OSInt tap, Int32 *index, Int64 *weight, OSInt sstep, OSInt dstep);
;0 edi
;4 esi
;8 ebx
;12 ebp
;16 retAddr
;20 inPt
;24 inptCurr
;28 outPt
;32 width
;36 height
;40 tap
;44 index
;48 weight
;52 sstep
;56 dstep

	align 16
_Deinterlace8_VerticalFilterOdd:
	push ebp
	push ebx
	push esi
	push edi
	test dword [esp+32],3 ;width
	jnz vfosstart
	test dword [esp+28],15 ;outPt
	jnz vfosstart
	test dword [esp+56],15 ;dstep
	jnz vfosstart
	jmp vfostart
	
	align 16
vfosstart:							;if ((width & 3) != 0 || (((OSInt)outPt) & 15) != 0 || (dstep & 15) != 0)
	mov eax,dword [esp+36] ;height
	shr dword [esp+32],1 ;width
	shr eax,1
	mov dword [esp+36],eax ;currHeight
	pxor xmm6,xmm6

	align 16
vfoslop:
	mov esi,dword [esp+28] ;outPt
	mov ecx,dword [esp+24] ;inPtCurr

	mov eax,dword [esp+32] ;width
	shr eax,1
	ALIGN 16
vfoslop4:
	movdqu xmm1,[ecx]
	movdqu [esi],xmm1
	add ecx,16
	add esi,16
	dec eax
	jnz vfoslop4
	mov eax,dword [esp+52] ;sstep
	mov esi,dword [esp+56] ;dstep
	add dword [esp+24],eax ;inPtCurr
	add dword [esp+28],esi ;outPt

	mov eax,dword [esp+40] ;tap
	lea ebx,[eax*4]
	lea edi,[eax*8]
	add dword [esp+44],ebx ;index
	add dword [esp+48],edi ;weight

	mov esi,dword [esp+28] ;outPt
	mov ecx,dword [esp+20] ;inPt
	mov ebp,dword [esp+32] ;width

	align 16
vfoslop2:
	mov ebx,dword [esp+44] ;index
	mov edi,dword [esp+48] ;weight

	mov edx,dword [esp+40] ;tap
	pxor xmm1,xmm1
	pxor xmm3,xmm3
	ALIGN 16
vfoslop3:
	mov eax,dword [ebx]
	movq xmm0,[ecx+eax]
	mov eax,dword [ebx+4]
	movq xmm2,[ecx+eax]
	movdqa xmm5,[edi]
	punpcklbw xmm0,xmm0
	punpcklbw xmm2,xmm2
	movdqu xmm4,xmm0
	punpcklwd xmm0,xmm2
	punpckhwd xmm4,xmm2
	psrlw xmm0,1
	psrlw xmm4,1
	pmaddwd xmm0, xmm5
	pmaddwd xmm4, xmm5
	paddd xmm1,xmm0
	paddd xmm3,xmm4
	add ebx,8
	add edi,16
	sub edx,2
	jnz vfoslop3

	psrad xmm1,22
	psrad xmm3,22
	packssdw xmm1,xmm3
	packuswb xmm1,xmm6
	movq [esi],xmm1
	add esi,8
	add ecx,8
	dec ebp
	jnz vfoslop2

	mov eax,dword [esp+40] ;tap
	mov esi,dword [esp+56] ;dstep
	lea ebx,[eax*4]
	lea edi,[eax*8]
	add dword [esp+44],ebx ;index
	add dword [esp+48],edi ;weight
	add dword [esp+28],esi ;outPt

	dec dword [esp+36] ;currHeight
	jnz vfoslop
	jmp vfoexit
			
	align 16
vfostart:
	mov eax,dword [esp+36] ;height
	shr dword [esp+32],2 ;width
	shr eax,1
	mov dword [esp+36],eax ;currHeight
	pxor xmm6,xmm6

	align 16
vfolop:
	mov esi,dword [esp+28] ;outPt
	mov ecx,dword [esp+24] ;inPtCurr

	mov eax,dword [esp+32] ;width
	ALIGN 16
vfolop4:
	movdqu xmm1,[ecx]
	movntdq [esi],xmm1
	add ecx,16
	add esi,16
	dec eax
	jnz vfolop4
	mov eax,dword [esp+52] ;sstep
	mov esi,dword [esp+56] ;dstep
	add dword [esp+24],eax ;inPtCurr
	add dword [esp+28],esi ;outPt

	mov eax,dword [esp+40] ;tap
	lea ebx,[eax*4]
	lea edi,[eax*8]
	add dword [esp+44],ebx ;index
	add dword [esp+48],edi ;weight

	mov esi,dword [esp+28] ;outPt
	mov ecx,dword [esp+20] ;inPt

	mov ebp,dword [esp+32] ;width
	align 16
vfolop2:
	mov ebx,dword [esp+44] ;index
	mov edi,dword [esp+48] ;weight

	mov edx,dword [esp+40] ;tap
	pxor xmm4,xmm4
	pxor xmm5,xmm5
	pxor xmm6,xmm6
	pxor xmm7,xmm7
	ALIGN 16
vfolop3:
	mov eax,dword [ebx]
	movq xmm2,[ecx+eax]
	mov eax,dword [ebx+4]
	movq xmm3,[ecx+eax]
	movdqa xmm0,[edi]
	punpcklbw xmm2,xmm2
	punpcklbw xmm3,xmm3
	movdqu xmm1,xmm2
	punpcklwd xmm1,xmm3
	punpckhwd xmm2,xmm3
	psrlw xmm1,1
	psrlw xmm2,1
	pmaddwd xmm1,xmm0
	pmaddwd xmm2,xmm0
	paddd xmm4,xmm1
	paddd xmm5,xmm2

	mov eax,dword [ebx]
	movq xmm2,[ecx+eax+8]
	mov eax,dword [ebx+4]
	movq xmm3,[ecx+eax+8]
	punpcklbw xmm2,xmm2
	punpcklbw xmm3,xmm3
	movdqu xmm1,xmm2
	punpcklwd xmm1,xmm3
	punpckhwd xmm2,xmm3
	psrlw xmm1,1
	psrlw xmm2,1
	pmaddwd xmm1,xmm0
	pmaddwd xmm2,xmm0
	paddd xmm6,xmm1
	paddd xmm7,xmm2

	add ebx,8
	add edi,16
	sub edx,2
	jnz vfolop3

	psrad xmm4,22
	psrad xmm5,22
	psrad xmm6,22
	psrad xmm7,22
	packssdw xmm4,xmm5
	packssdw xmm6,xmm7
	packuswb xmm4,xmm6
	movntdq [esi],xmm4
	add esi,16
	add ecx,16
	dec ebp
	jnz vfolop2

	mov eax,dword [esp+40] ;tap
	lea ebx,[eax*4]
	lea edi,[eax*8]
	add dword [esp+44],ebx ;index
	add dword [esp+48],edi ;weight

	mov esi,dword [esp+56] ;dstep
	add dword [esp+28],esi ;outPt

	dec dword [esp+36] ;currHeight
	jnz vfolop
	
	align 16
vfoexit:
	pop edi
	pop esi
	pop ebx
	pop ebp
	ret

;void Deinterlace8_VerticalFilterEven(UInt8 *inPt, UInt8 *inPtCurr, UInt8 *outPt, OSInt width, OSInt height, OSInt tap, Int32 *index, Int64 *weight, OSInt sstep, OSInt dstep);
;0 edi
;4 esi
;8 ebx
;12 ebp
;16 retAddr
;20 inPt
;24 inptCurr
;28 outPt
;32 width
;36 height
;40 tap
;44 index
;48 weight
;52 sstep
;56 dstep

	align 16
_Deinterlace8_VerticalFilterEven:
	push ebp
	push ebx
	push esi
	push edi
	test dword [esp+32],3 ;width
	jnz vfesstart
	test dword [esp+28],15 ;outPt
	jnz vfesstart
	test dword [esp+56],15 ;dstep
	jnz vfesstart
	jmp vfestart

	align 16
vfesstart:								;if ((width & 3) != 0 || (((OSInt)outPt) & 15) != 0 || (dstep & 15) != 0)
	mov eax,dword [esp+36] ;height
	shr dword [esp+32],1 ;width
	shr eax,1
	mov dword [esp+36],eax ;currHeight
	pxor xmm6,xmm6

	align 16
vfeslop:
	mov esi,dword [esp+28] ;outPt
	mov ecx,dword [esp+20] ;inPt

	mov ebp,dword [esp+32] ;width

	align 16
vfeslop2:
	mov ebx,dword [esp+44] ;index
	mov edi,dword [esp+48] ;weight

	mov edx,dword [esp+40] ;tap
	pxor xmm1,xmm1
	pxor xmm3,xmm3
	ALIGN 16
vfeslop3:
	mov eax,dword [ebx]
	movq xmm0,[ecx+eax]
	mov eax,dword [ebx+4]
	movq xmm2,[ecx+eax]
	movdqa xmm5,[edi]
	punpcklbw xmm0,xmm0
	punpcklbw xmm2,xmm2
	movdqu xmm4,xmm0
	punpcklwd xmm0,xmm2
	punpckhwd xmm4,xmm2
	psrlw xmm0,1
	psrlw xmm4,1
	pmaddwd xmm0, xmm5
	pmaddwd xmm4, xmm5
	paddd xmm1,xmm0
	paddd xmm3,xmm4
	add ebx,8
	add edi,16
	sub edx,2
	jnz vfeslop3

	psrad xmm1,22
	psrad xmm3,22
	packssdw xmm1,xmm3
	packuswb xmm1,xmm6
	movq [esi],xmm1
	add esi,8
	add ecx,8
	dec ebp
	jnz vfeslop2

	mov eax,dword [esp+40] ;tap
	mov esi,dword [esp+56] ;dstep
	lea ebx,[eax*4]
	lea edi,[eax*8]
	add dword [esp+28],esi ;outPt
	add dword [esp+44],ebx ;index
	add dword [esp+48],edi ;weight

	mov ecx,dword [esp+24] ;inPtCurr

	mov eax,dword [esp+32] ;width
	shr eax,1
	ALIGN 16
vfeslop4:
	movdqu xmm1,[ecx]
	movdqu [esi],xmm1
	add ecx,16
	add esi,16
	dec eax
	jnz vfeslop4
	mov eax,dword [esp+52] ;sstep
	mov esi,dword [esp+56] ;dstep
	add [esp+24],eax ;inPtCurr
	add [esp+28],esi ;outPt

	mov eax,dword [esp+40] ;tap
	lea ebx,[eax*4]
	lea edi,[eax*8]
	add dword [esp+44],ebx ;index
	add dword [esp+48],edi ;weight

	dec dword [esp+36] ;currHeight
	jnz vfeslop
	jmp vfeexit

	align 16
vfestart:
	mov eax,dword [esp+36] ;height
	shr dword [esp+32],2 ;width
	shr eax,1
	mov dword [esp+36],eax ;currHeight
	pxor xmm6,xmm6

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
	pxor xmm4,xmm4
	pxor xmm5,xmm5
	pxor xmm6,xmm6
	pxor xmm7,xmm7
	ALIGN 16
vfelop3:
	mov eax,dword [ebx]
	movq xmm2,[ecx+eax]
	mov eax,dword [ebx+4]
	movq xmm3,[ecx+eax]
	movdqa xmm0,[edi]
	punpcklbw xmm2,xmm2
	punpcklbw xmm3,xmm3
	movdqu xmm1,xmm2
	punpcklwd xmm1,xmm3
	punpckhwd xmm2,xmm3
	psrlw xmm1,1
	psrlw xmm2,1
	pmaddwd xmm1, xmm0
	pmaddwd xmm2, xmm0
	paddd xmm4,xmm1
	paddd xmm5,xmm2

	mov eax,dword [ebx]
	movq xmm2,[ecx+eax+8]
	mov eax,dword [ebx+4]
	movq xmm3,[ecx+eax+8]
	punpcklbw xmm2,xmm2
	punpcklbw xmm3,xmm3
	movdqu xmm1,xmm2
	punpcklwd xmm1,xmm3
	punpckhwd xmm2,xmm3
	psrlw xmm1,1
	psrlw xmm2,1
	pmaddwd xmm1, xmm0
	pmaddwd xmm2, xmm0
	paddd xmm6,xmm1
	paddd xmm7,xmm2

	add ebx,8
	add edi,16
	sub edx,2
	jnz vfelop3

	psrad xmm4,22
	psrad xmm5,22
	psrad xmm6,22
	psrad xmm7,22
	packssdw xmm4,xmm5
	packssdw xmm6,xmm7
	packuswb xmm4,xmm6
	movntdq [esi],xmm4
	add esi,16
	add ecx,16
	dec ebp
	jnz vfelop2

	mov eax,dword [esp+40] ;tap
	mov esi,dword [esp+56] ;dstep
	lea ebx,[eax*4]
	lea edi,[eax*8]
	add dword [esp+28],esi ;outPt
	add dword [esp+44],ebx ;index
	add dword [esp+48],edi ;weight

	mov ecx,dword [esp+24] ;inPtCurr
	mov esi,dword [esp+28] ;outPt
	mov eax,dword [esp+32] ;width
	ALIGN 16
vfelop4:
	movdqu xmm1,[ecx]
	movntdq [esi],xmm1
	add ecx,16
	add esi,16
	dec eax
	jnz vfelop4
	mov eax,dword [esp+52] ;sstep
	mov esi,dword [esp+56] ;dstep
	add dword [esp+24],eax ;inPtCurr
	add dword [esp+28],esi ;outPt

	mov eax,dword [esp+40] ;tap
	lea ebx,[eax*4]
	lea edi,[eax*8]
	add dword [esp+44],ebx ;index
	add dword [esp+48],edi ;weight

	dec dword [esp+36] ;currHeight
	jnz vfelop
	
	align 16
vfeexit:
	pop edi
	pop esi
	pop ebx
	pop ebp
	ret