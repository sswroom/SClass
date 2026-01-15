section .text

global _LanczosResizerLR_C32_CPU_horizontal_filter
global _LanczosResizerLR_C32_CPU_vertical_filter
global _LanczosResizerLR_C32_CPU_vertical_filter_na
global _LanczosResizerLR_C32_CPU_hv_filter
global _LanczosResizerLR_C32_CPU_collapse
global _LanczosResizerLR_C32_CPU_collapse_na

;void LanczosResizerLR_C32_CPU_horizontal_filter(UInt8 *inPt, UInt8 *outPt, IntOS width, IntOS height, IntOS tap, IntOS *index, Int64 *weight, IntOS sstep, IntOS dstep, UInt8 *rgbTable)
;0 tmpV
;16 edi
;20 esi
;24 ebx
;28 ebp
;32 retAddr
;36 inPt
;40 outPt
;44 width
;48 height / currHeight
;52 tap
;56 index
;60 weight
;64 sstep
;68 dstep / dAdd
;72 rgbTable

	align 16
_LanczosResizerLR_C32_CPU_horizontal_filter:
	push ebp
	push ebx
	push esi
	push edi
	mov eax,16384				; Int32 tmpV[4] = {16384, 16384, 16384, 16384};
	push eax
	push eax
	push eax
	push eax
	mov edx,dword [esp+44] ;width
	lea ecx,[edx*8]				; IntOS dAdd = dstep - width * 8;
	sub dword [esp+68],ecx
	test edx,1
	jnz hfstart
	mov ecx,dword [esp+52] ;tap
	cmp ecx,6
	jz hf6nstart
	cmp ecx,8
	jz hf8nstart
	cmp ecx,16
	jz hf16nstart
	jmp hfstart2
	
	align 16
hfstart:						; if (width & 1)
	shr dword [esp+52],1 ;tap
	pxor xmm3,xmm3
	movdqu xmm7,[esp+0] ;tmpV
	mov ecx,dword [esp+36] ;inPt
	mov esi,dword [esp+40] ;outPt
	align 16
hflop:

	mov ebp,dword [esp+44] ;width
	mov edi,dword [esp+60] ;weight
	mov ebx,dword [esp+56] ;index
	align 16
hflop2:
	mov edx,dword [esp+52] ;tap
	movdqa xmm1,xmm7
	ALIGN 16
hflop3:
	mov eax,dword [ebx]
	movq xmm0,[ecx+eax]
	mov eax,dword [ebx+4]
	movq xmm2,[ecx+eax]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0, [edi]
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

	add ecx,dword [esp+64] ;sstep
	add esi,dword [esp+68] ;dAdd

	dec dword [esp+48] ;currHeight
	jnz hflop
	jmp hfexit

	align 16
hf6nstart:						; else if (tap == 6)
	test dword [esp+40],15 ;outPt
	jnz hf6start
	test dword [esp+68],15 ;dstep
	jnz hf6start
								; if ((((IntOS)outPt) & 15) == 0 && (dstep & 15) == 0)
	shr dword [esp+44],1 ;width
	mov edx,dword [esp+48] ;height
	mov ecx,dword [esp+36] ;inPt
	mov esi,dword [esp+40] ;outPt
	movdqu xmm7,[esp+0] ;tmpV
	ALIGN 16
hf6nlop4:

	mov ebp,dword [esp+44] ;width

	mov edi,dword [esp+60] ;weight
	mov ebx,dword [esp+56] ;index
	ALIGN 16
hf6nlop5:
	mov eax,dword [ebx]
	movdqa xmm1,xmm7
	movdqu xmm0,[ecx+eax]
	mov eax,dword [ebx+4]
	movdqa xmm4,xmm0
	movdqu xmm2,[ecx+eax]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[edi]
	punpckhwd xmm4,xmm2
	pmaddwd xmm4,[edi+16]
	paddd xmm1,xmm0
	mov eax,dword [ebx+8]
	paddd xmm1,xmm4
	movdqu xmm0,[ecx+eax]
	pshufd xmm2,xmm0,14
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[edi+32]
	paddd xmm1,xmm0

	mov eax,dword [ebx+12]
	movdqa xmm3,xmm7
	movdqu xmm0,[ecx+eax]
	mov eax,dword [ebx+16]
	movdqa xmm4,xmm0
	movdqu xmm2,[ecx+eax]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[edi+48]
	punpckhwd xmm4,xmm2
	pmaddwd xmm4,[edi+64]
	paddd xmm3,xmm0
	mov eax,dword [ebx+20]
	paddd xmm3,xmm4
	movdqu xmm0,[ecx+eax]
	pshufd xmm2,xmm0,14
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[edi+80]
	paddd xmm3,xmm0

	lea edi,[edi+96]
	lea ebx,[ebx+24]

	psrad xmm1,15
	psrad xmm3,15
	packssdw xmm1,xmm3
	movntdq [esi],xmm1

	lea esi,[esi+16]
	dec ebp
	jnz hf6nlop5

	add ecx,dword [esp+64] ;sstep
	add esi,dword [esp+68] ;dAdd

	dec edx
	jnz hf6nlop4
	jmp hfexit

hf6start:
	shr dword [esp+44],1 ;width
	mov edx,dword [esp+48] ;height
	mov ecx,dword [esp+36] ;inPt
	mov esi,dword [esp+40] ;outPt
	movdqu xmm7,[esp+0] ;tmpV
	ALIGN 16
hf6lop4:

	mov ebp,dword [esp+44] ;width

	mov edi,dword [esp+60] ;weight
	mov ebx,dword [esp+56] ;index
	ALIGN 16
hf6lop5:

	mov eax,dword [ebx]
	movdqa xmm1,xmm7
	movdqu xmm0,[ecx+eax]
	mov eax,dword [ebx+4]
	movdqa xmm4,xmm0
	movdqu xmm2,[ecx+eax]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[edi]
	punpckhwd xmm4,xmm2
	pmaddwd xmm4,[edi+16]
	paddd xmm1,xmm0
	mov eax,dword [ebx+8]
	paddd xmm1,xmm4
	movq xmm0,[ecx+eax]
	movq xmm2,[ecx+eax+8]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[edi+32]
	paddd xmm1,xmm0

	mov eax,dword [ebx+12]
	movdqa xmm3,xmm7
	movdqu xmm0,[ecx+eax]
	mov eax,dword [ebx+16]
	movdqa xmm4,xmm0
	movdqu xmm2,[ecx+eax]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[edi+48]
	punpckhwd xmm4,xmm2
	pmaddwd xmm4,[edi+64]
	paddd xmm3,xmm0
	mov eax,dword [ebx+20]
	paddd xmm3,xmm4
	movq xmm0,[ecx+eax]
	movq xmm2,[ecx+eax+8]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[edi+80]
	paddd xmm3,xmm0
	lea edi,[edi+96]
	lea ebx,[ebx+24]

	psrad xmm1,15
	psrad xmm3,15
	packssdw xmm1,xmm3
	movdqu [esi],xmm1

	lea esi,[esi+16]
	dec ebp
	jnz hf6lop5

	add ecx,dword [esp+64] ;sstep
	add esi,dword [esp+68] ;dAdd

	dec edx
	jnz hf6lop4
	jmp hfexit

	align 16
hf8nstart:						; else if (tap == 8)
	test dword [esp+40],15 ;outPt
	jnz hf8start
	test dword [esp+68],15 ;dstep
	jnz hf8start
								; if ((((IntOS)outPt) & 15) == 0 && (dstep & 15) == 0)
	shr dword [esp+44],1 ;width
	mov edx,dword [esp+48] ;height
	mov ecx,dword [esp+36] ;inPt
	mov esi,dword [esp+40] ;outPt
	mov ebx,dword [esp+60] ;weight
	movdqu xmm7,[esp+0] ;tmpV
	ALIGN 16
hf8nlop4:

	mov ebp,dword [esp+44] ;width

	mov edi,ebx
	ALIGN 16
hf8nlop5:
	mov eax,dword [edi]
	prefetcht0 [ecx+eax+128]
	movdqa xmm1,xmm7
	movdqu xmm0,[ecx+eax]
	movdqa xmm4,xmm0
	movdqu xmm2,[ecx+eax+16]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[edi+16]
	punpckhwd xmm4,xmm2
	pmaddwd xmm4,[edi+32]
	paddd xmm1,xmm0
	paddd xmm1,xmm4
	movdqu xmm0,[ecx+eax+32]
	movdqa xmm4,xmm0
	movdqu xmm2,[ecx+eax+48]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[edi+48]
	punpckhwd xmm4,xmm2
	pmaddwd xmm4,[edi+64]
	paddd xmm1,xmm0
	paddd xmm1,xmm4

	mov eax,dword [edi+8]
	movdqa xmm3,xmm7
	movdqu xmm0,[ecx+eax]
	movdqa xmm4,xmm0
	movdqu xmm2,[ecx+eax+16]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[edi+80]
	punpckhwd xmm4,xmm2
	pmaddwd xmm4,[edi+96]
	paddd xmm3,xmm0
	paddd xmm3,xmm4
	movdqu xmm0,[ecx+eax+32]
	movdqa xmm4,xmm0
	movdqu xmm2,[ecx+eax+48]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[edi+112]
	punpckhwd xmm4,xmm2
	pmaddwd xmm4,[edi+128]
	paddd xmm3,xmm0
	paddd xmm3,xmm4

	lea edi,[edi+144]

	psrad xmm1,15
	psrad xmm3,15
	packssdw xmm1,xmm3
	movntdq [esi],xmm1

	lea esi,[esi+16]
	dec ebp
	jnz hf8nlop5

	add ecx,dword [esp+64] ;sstep
	add esi,dword [esp+68] ;dAdd

	dec edx
	jnz hf8nlop4
	jmp hfexit

	align 16
hf8start:
	shr dword [esp+44],1 ;width
	mov edx,dword [esp+48] ;height
	mov ecx,dword [esp+36] ;inPt
	mov esi,dword [esp+40] ;outPt
	mov ebx,dword [esp+60] ;weight
	movdqu xmm7,[esp+0] ;tmpV
	ALIGN 16
hf8lop4:

	mov ebp,dword [esp+44] ;width

	mov edi,ebx
	ALIGN 16
hf8lop5:
	mov eax,dword [edi]
	prefetcht0 [ecx+eax+128]
	movdqa xmm1,xmm7
	movdqu xmm0,[ecx+eax]
	movdqa xmm4,xmm0
	movdqu xmm2,[ecx+eax+16]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[edi+16]
	punpckhwd xmm4,xmm2
	pmaddwd xmm4,[edi+32]
	paddd xmm1,xmm0
	paddd xmm1,xmm4
	movdqu xmm0,[ecx+eax+32]
	movdqa xmm4,xmm0
	movdqu xmm2,[ecx+eax+48]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[edi+48]
	punpckhwd xmm4,xmm2
	pmaddwd xmm4,[edi+64]
	paddd xmm1,xmm0
	paddd xmm1,xmm4

	mov eax,dword [edi+8]
	movdqa xmm3,xmm7
	movdqu xmm0,[ecx+eax]
	movdqa xmm4,xmm0
	movdqu xmm2,[ecx+eax+16]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[edi+80]
	punpckhwd xmm4,xmm2
	pmaddwd xmm4,[edi+96]
	paddd xmm3,xmm0
	paddd xmm3,xmm4
	movdqu xmm0,[ecx+eax+32]
	movdqa xmm4,xmm0
	movdqu xmm2,[ecx+eax+48]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[edi+112]
	punpckhwd xmm4,xmm2
	pmaddwd xmm4,[edi+128]
	paddd xmm3,xmm0
	paddd xmm3,xmm4
	lea edi,[edi+144]

	psrad xmm1,15
	psrad xmm3,15
	packssdw xmm1,xmm3
	movdqu [esi],xmm1

	lea esi,[esi+16]
	dec ebp
	jnz hf8lop5

	add ecx,dword [esp+64] ;sstep
	add esi,dword [esp+68] ;dAdd

	dec edx
	jnz hf8lop4
	jmp hfexit

	align 16
hf16nstart:						; else if (tap == 16)
	test dword [esp+40],15 ;outPt
	jnz hf16start
	test dword [esp+68],15 ;dstep
	jnz hf16start
								; if ((((IntOS)outPt) & 15) == 0 && (dstep & 15) == 0)
	shr dword [esp+44],1 ;width
	mov edx,dword [esp+48] ;height
	mov ecx,dword [esp+36] ;inPt
	mov esi,dword [esp+40] ;outPt
	mov ebx,dword [esp+60] ;weight
	movdqu xmm7,[esp+0] ;tmpV
	ALIGN 16
hf16nlop4:

	mov ebp,dword [esp+44] ;width

	mov edi,ebx
	ALIGN 16
hf16nlop5:
	mov eax,dword [edi]
	prefetcht0 [ecx+eax+128]
	movdqa xmm1,xmm7
	movdqu xmm0,[ecx+eax]
	movdqa xmm4,xmm0
	movdqu xmm2,[ecx+eax+16]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[edi+16]
	punpckhwd xmm4,xmm2
	pmaddwd xmm4,[edi+32]
	paddd xmm1,xmm0
	paddd xmm1,xmm4
	movdqu xmm0,[ecx+eax+32]
	movdqa xmm4,xmm0
	movdqu xmm2,[ecx+eax+48]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[edi+48]
	punpckhwd xmm4,xmm2
	pmaddwd xmm4,[edi+64]
	paddd xmm1,xmm0
	paddd xmm1,xmm4
	movdqu xmm0,[ecx+eax+64]
	movdqa xmm4,xmm0
	movdqu xmm2,[ecx+eax+80]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[edi+80]
	punpckhwd xmm4,xmm2
	pmaddwd xmm4,[edi+96]
	paddd xmm1,xmm0
	paddd xmm1,xmm4
	movdqu xmm0,[ecx+eax+96]
	movdqa xmm4,xmm0
	movdqu xmm2,[ecx+eax+112]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[edi+112]
	punpckhwd xmm4,xmm2
	pmaddwd xmm4,[edi+128]
	paddd xmm1,xmm0
	paddd xmm1,xmm4

	mov eax,dword [edi+8]
	movdqa xmm3,xmm7
	movdqu xmm0,[ecx+eax]
	movdqa xmm4,xmm0
	movdqu xmm2,[ecx+eax+16]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[edi+144]
	punpckhwd xmm4,xmm2
	pmaddwd xmm4,[edi+160]
	paddd xmm3,xmm0
	paddd xmm3,xmm4
	movdqu xmm0,[ecx+eax+32]
	movdqa xmm4,xmm0
	movdqu xmm2,[ecx+eax+48]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[edi+176]
	punpckhwd xmm4,xmm2
	pmaddwd xmm4,[edi+192]
	paddd xmm3,xmm0
	paddd xmm3,xmm4
	movdqu xmm0,[ecx+eax+64]
	movdqa xmm4,xmm0
	movdqu xmm2,[ecx+eax+80]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[edi+208]
	punpckhwd xmm4,xmm2
	pmaddwd xmm4,[edi+224]
	paddd xmm3,xmm0
	paddd xmm3,xmm4
	movdqu xmm0,[ecx+eax+96]
	movdqa xmm4,xmm0
	movdqu xmm2,[ecx+eax+112]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[edi+240]
	punpckhwd xmm4,xmm2
	pmaddwd xmm4,[edi+256]
	paddd xmm3,xmm0
	paddd xmm3,xmm4

	lea edi,[edi+272]

	psrad xmm1,15
	psrad xmm3,15
	packssdw xmm1,xmm3
	movntdq [esi],xmm1

	lea esi,[esi+16]
	dec ebp
	jnz hf16nlop5

	add ecx,dword [esp+64] ;sstep
	add esi,dword [esp+68] ;dAdd

	dec edx
	jnz hf16nlop4
	jmp hfexit

	align 16	
hf16start:
	shr dword [esp+44],1 ;width
	mov edx,dword [esp+48] ;height
	mov ecx,dword [esp+36] ;inPt
	mov esi,dword [esp+40] ;outPt
	mov ebx,dword [esp+60] ;weight
	movdqu xmm7,[esp+0] ;tmpV
	ALIGN 16
hf16lop4:

	mov ebp,dword [esp+44] ;width

	mov edi,ebx
	ALIGN 16
hf16lop5:
	mov eax,dword [edi]
	prefetcht0 [ecx+eax+128]
	movdqa xmm1,xmm7
	movdqu xmm0,[ecx+eax]
	movdqa xmm4,xmm0
	movdqu xmm2,[ecx+eax+16]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[edi+16]
	punpckhwd xmm4,xmm2
	pmaddwd xmm4,[edi+32]
	paddd xmm1,xmm0
	paddd xmm1,xmm4
	movdqu xmm0,[ecx+eax+32]
	movdqa xmm4,xmm0
	movdqu xmm2,[ecx+eax+48]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[edi+48]
	punpckhwd xmm4,xmm2
	pmaddwd xmm4,[edi+64]
	paddd xmm1,xmm0
	paddd xmm1,xmm4
	movdqu xmm0,[ecx+eax+64]
	movdqa xmm4,xmm0
	movdqu xmm2,[ecx+eax+80]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[edi+80]
	punpckhwd xmm4,xmm2
	pmaddwd xmm4,[edi+96]
	paddd xmm1,xmm0
	paddd xmm1,xmm4
	movdqu xmm0,[ecx+eax+96]
	movdqa xmm4,xmm0
	movdqu xmm2,[ecx+eax+112]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[edi+112]
	punpckhwd xmm4,xmm2
	pmaddwd xmm4,[edi+128]
	paddd xmm1,xmm0
	paddd xmm1,xmm4

	mov eax,dword [edi+8]
	movdqa xmm3,xmm7
	movdqu xmm0,[ecx+eax]
	movdqa xmm4,xmm0
	movdqu xmm2,[ecx+eax+16]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[edi+144]
	punpckhwd xmm4,xmm2
	pmaddwd xmm4,[edi+160]
	paddd xmm3,xmm0
	paddd xmm3,xmm4
	movdqu xmm0,[ecx+eax+32]
	movdqa xmm4,xmm0
	movdqu xmm2,[ecx+eax+48]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[edi+176]
	punpckhwd xmm4,xmm2
	pmaddwd xmm4,[edi+192]
	paddd xmm3,xmm0
	paddd xmm3,xmm4
	movdqu xmm0,[ecx+eax+64]
	movdqa xmm4,xmm0
	movdqu xmm2,[ecx+eax+80]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[edi+208]
	punpckhwd xmm4,xmm2
	pmaddwd xmm4,[edi+224]
	paddd xmm3,xmm0
	paddd xmm3,xmm4
	movdqu xmm0,[ecx+eax+96]
	movdqa xmm4,xmm0
	movdqu xmm2,[ecx+eax+112]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[edi+240]
	punpckhwd xmm4,xmm2
	pmaddwd xmm4,[edi+256]
	paddd xmm3,xmm0
	paddd xmm3,xmm4

	lea edi,[edi+272]

	psrad xmm1,15
	psrad xmm3,15
	packssdw xmm1,xmm3
	movdqu [esi],xmm1

	lea esi,[esi+16]
	dec ebp
	jnz hf16lop5

	add ecx,dword [esp+64] ;sstep
	add esi,dword [esp+68] ;dAdd

	dec edx
	jnz hf16lop4
	jmp hfexit

	align 16
hfstart2:
	shr dword [esp+52],1 ;tap
	shr dword [esp+44],1 ;width
	mov ecx,dword [esp+36] ;inPt
	mov esi,dword [esp+40] ;outPt
	movdqu xmm7,[esp+0] ;tmpV
	align 16
hflop4:
	mov ebp,dword [esp+44] ;width

	mov edi,dword [esp+60] ;weight
	mov ebx,dword [esp+56] ;index
	align 16
hflop5:
	mov edx,dword [esp+52] ;tap
	movdqa xmm1,xmm7
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

	mov edx,dword [esp+52] ;tap
	movdqa xmm3,xmm7
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

	add ecx,dword [esp+64] ;sstep
	add esi,dword [esp+68] ;dAdd

	dec dword [esp+48] ;currHeight
	jnz hflop4
	
	align 16
hfexit:
	pop eax
	pop eax
	pop eax
	pop eax
	pop edi
	pop esi
	pop ebx
	pop ebp
	ret

;void LanczosResizerLR_C32_CPU_vertical_filter(UInt8 *inPt, UInt8 *outPt, IntOS width, IntOS height, IntOS tap, IntOS *index, Int64 *weight, IntOS sstep, IntOS dstep, UInt8 *rgbTable)
;0 tmpV
;16 edi
;20 esi
;24 ebx
;28 ebp
;32 retAddr
;36 inPt
;40 outPt
;44 width
;48 height / currHeight
;52 tap
;56 index
;60 weight
;64 sstep
;68 dstep / dAdd
;72 rgbTable

	align 16
_LanczosResizerLR_C32_CPU_vertical_filter:
	mov eax,16384
	push ebp
	push ebx
	push esi
	push edi
	push eax						;Int32 tmpV[4] = {16384, 16384, 16384, 16384};
	push eax
	push eax
	push eax
	mov edx,dword [esp+44] ;width
	lea ecx,[edx*4]
	sub dword [esp+68],ecx ;dAdd
	mov eax,dword [esp+52] ;tap
	test edx,1						;if (width & 1)
	jz vfstart2
	cmp eax,6
	jz vf6_1start
	jmp vf_1start
	
	align 16
vf6_1start:							; if (tap == 6)
	shr dword [esp+44],1 ;width
	mov edi,dword [esp+72] ;rgbTable
	mov ebx,dword [esp+60] ;weight
	mov ecx,dword [esp+40] ;outPt
	ALIGN 16
vf6_1lop4:
	movdqa xmm5,[ebx+32]
	movdqa xmm6,[ebx+48]
	movdqa xmm7,[ebx+64]

	mov esi,dword [esp+36] ;inPt
	mov ebp,dword [esp+44] ;width
	ALIGN 16
vf6_1lop5:
	mov eax,dword [ebx]
	movdqu xmm2,[esp+0] ;tmpV
	mov edx,dword [ebx+4]
	movdqa xmm3,xmm2
	movdqu xmm0,[esi+eax]
	movdqa xmm4,xmm0
	movdqu xmm1,[esi+edx]
	punpcklwd xmm0, xmm1
	punpckhwd xmm4, xmm1
	mov eax,dword [ebx+8]
	pmaddwd xmm0,xmm5
	pmaddwd xmm4,xmm5
	mov edx,dword [ebx+12]
	paddd xmm2,xmm0
	paddd xmm3,xmm4
	movdqu xmm0,[esi+eax]
	movdqa xmm4,xmm0
	movdqu xmm1,[esi+edx]
	punpcklwd xmm0, xmm1
	punpckhwd xmm4, xmm1
	mov eax,dword [ebx+16]
	pmaddwd xmm0,xmm6
	pmaddwd xmm4,xmm6
	mov edx,dword [ebx+20]
	paddd xmm2,xmm0
	paddd xmm3,xmm4
	movdqu xmm0,[esi+eax]
	movdqa xmm4,xmm0
	movdqu xmm1,[esi+edx]
	punpcklwd xmm0, xmm1
	punpckhwd xmm4, xmm1
	pmaddwd xmm0,xmm7
	pmaddwd xmm4,xmm7
	paddd xmm2,xmm0
	paddd xmm3,xmm4

	psrad xmm3,15
	psrad xmm2,15
	packssdw xmm2,xmm3
	pextrw edx,xmm2,0
	mov eax,dword [edi+edx*4]
	pextrw edx,xmm2,1
	or eax,dword [edi+edx*4+262144]
	pextrw edx,xmm2,2
	or eax,dword [edi+edx*4+524288]
	pextrw edx,xmm2,3
	or eax,dword [edi+edx*4+786432]
	movnti dword [ecx],eax

	pextrw edx,xmm2,4
	mov eax,dword [edi+edx*4]
	pextrw edx,xmm2,5
	or eax,dword [edi+edx*4+262144]
	pextrw edx,xmm2,6
	or eax,dword [edi+edx*4+524288]
	pextrw edx,xmm2,7
	or eax,dword [edi+edx*4+786432]
	movnti dword [ecx+4],eax
	lea esi,[esi+16]
	lea ecx,[ecx+8]
	dec ebp
	jnz vf6_1lop5

	mov eax,dword [ebx]
	movdqu xmm2,[esp+0] ;tmpV
	mov edx,dword [ebx+4]
	movq xmm0,[esi+eax]
	movq xmm1,[esi+edx]
	punpcklwd xmm0, xmm1
	mov eax,dword [ebx+8]
	pmaddwd xmm0,xmm5
	mov edx,dword [ebx+12]
	paddd xmm2,xmm0
	movdqu xmm0,[esi+eax]
	movdqu xmm1,[esi+edx]
	punpcklwd xmm0, xmm1
	mov eax,dword [ebx+16]
	pmaddwd xmm0,xmm6
	mov edx,dword [ebx+20]
	paddd xmm2,xmm0
	movdqu xmm0,[esi+eax]
	movdqu xmm1,[esi+edx]
	punpcklwd xmm0, xmm1
	pmaddwd xmm0,xmm7
	paddd xmm2,xmm0

	psrad xmm2,15
	packssdw xmm2,xmm2
	pextrw edx,xmm2,0
	mov eax,dword [edi+edx*4]
	pextrw edx,xmm2,1
	or eax,dword [edi+edx*4+262144]
	pextrw edx,xmm2,2
	or eax,dword [edi+edx*4+524288]
	pextrw edx,xmm2,3
	or eax,dword [edi+edx*4+786432]
	movnti dword [ecx],eax
	lea ecx,[ecx+4]

	add ebx,80
	add ecx,dword [esp+68] ;dAdd

	dec dword [esp+48] ;currHeight
	jnz vf6_1lop4
	jmp vfexit

	align 16
vf_1start:
	shr dword [esp+52],1 ;tap
	movdqu xmm7,[esp+0] ;tmpV
	pxor xmm3,xmm3
	mov ecx,dword [esp+40] ;outPt
	align 16
vflop:
	mov esi,dword [esp+36] ;inPt
	mov ebx,dword [esp+56] ;index

	mov ebp,dword [esp+44] ;width

	align 16
vflop2:
	mov edi,dword [esp+60] ;weight
	mov edx,dword [esp+52] ;tap

	movdqa xmm1,xmm7
	ALIGN 16
vflop3:
	mov eax,dword [ebx]
	movq xmm0,[esi+eax]
	mov eax,dword [ebx+4]
	movq xmm2,[esi+eax]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[edi]
	paddd xmm1,xmm0
	lea ebx,[ebx+8]
	lea edi,[edi+16]
	dec edx
	jnz vflop3

	mov edi,dword [esp+72] ;rgbTable
	psrad xmm1,15
	packssdw xmm1,xmm3
	pextrw edx,xmm1,0
	mov eax,dword [edi+edx*4]
	pextrw edx,xmm1,1
	or eax,dword [edi+edx*4+262144]
	pextrw edx,xmm1,2
	or eax,dword [edi+edx*4+524288]
	pextrw edx,xmm1,3
	or eax,dword [edi+edx*4+786432]

	mov dword [ecx],eax
	lea ecx,[ecx+4]
	lea esi,[esi+8]
	dec ebp
	jnz vflop2

	mov edx,dword [esp+52] ;tap
	lea eax,[edx*8]
	shl edx,4
	add dword [esp+56],eax ;index
	add dword [esp+60],edx ;weight
	add ecx,dword [esp+68] ;dAdd

	dec dword [esp+48] ;currHeight
	jnz vflop
	jmp vfexit

vfstart2:
	cmp eax,6
	jz vf6start
	cmp eax,8
	jz vf8start
	jmp vfstart

	align 16
vf6start:							; else if (tap == 6)
	shr dword [esp+44],1 ;width
	mov edi,dword [esp+72] ;rgbTable
	mov ebx,dword [esp+60] ;weight
	mov ecx,dword [esp+40] ;outPt
	ALIGN 16
vf6lop4:

	movdqa xmm5,[ebx+32]
	movdqa xmm6,[ebx+48]
	movdqa xmm7,[ebx+64]

	mov esi,dword [esp+36] ;inPt
	mov ebp,dword [esp+44] ;width
	ALIGN 16
vf6lop5:
	mov eax,dword [ebx]
	movdqu xmm2,[esp+0] ;tmpV
	mov edx,dword [ebx+4]
	movdqa xmm3,xmm2
	movdqu xmm0,[esi+eax]
	movdqa xmm4,xmm0
	movdqu xmm1,[esi+edx]
	punpcklwd xmm0, xmm1
	punpckhwd xmm4, xmm1
	mov eax,dword [ebx+8]
	pmaddwd xmm0,xmm5
	pmaddwd xmm4,xmm5
	mov edx,dword [ebx+12]
	paddd xmm2,xmm0
	paddd xmm3,xmm4
	movdqu xmm0,[esi+eax]
	movdqa xmm4,xmm0
	movdqu xmm1,[esi+edx]
	punpcklwd xmm0,xmm1
	punpckhwd xmm4,xmm1
	mov eax,dword [ebx+16]
	pmaddwd xmm0,xmm6
	pmaddwd xmm4,xmm6
	mov edx,dword [ebx+20]
	paddd xmm2,xmm0
	paddd xmm3,xmm4
	movdqu xmm0,[esi+eax]
	movdqa xmm4,xmm0
	movdqu xmm1,[esi+edx]
	punpcklwd xmm0,xmm1
	punpckhwd xmm4,xmm1
	pmaddwd xmm0,xmm7
	pmaddwd xmm4,xmm7
	paddd xmm2,xmm0
	paddd xmm3,xmm4

	psrad xmm3,15
	psrad xmm2,15
	packssdw xmm2,xmm3
	pextrw edx,xmm2,0
	mov eax,dword [edi+edx*4]
	pextrw edx,xmm2,1
	or eax,dword [edi+edx*4+262144]
	pextrw edx,xmm2,2
	or eax,dword [edi+edx*4+524288]
	pextrw edx,xmm2,3
	or eax,dword [edi+edx*4+786432]
	movnti dword [ecx],eax

	pextrw edx,xmm2,4
	mov eax,dword [edi+edx*4]
	pextrw edx,xmm2,5
	or eax,dword [edi+edx*4+262144]
	pextrw edx,xmm2,6
	or eax,dword [edi+edx*4+524288]
	pextrw edx,xmm2,7
	or eax,dword [edi+edx*4+786432]
	movnti dword [ecx+4],eax
	lea esi,[esi+16]
	lea ecx,[ecx+8]
	dec ebp
	jnz vf6lop5

	add ebx,80
	add ecx,dword [esp+68] ;dAdd

	dec dword [esp+48] ;currHeight
	jnz vf6lop4
	jmp vfexit
	
	align 16
vf8start:							;else if (tap == 8)
	shr dword [esp+44],1 ;width
	mov ebx,dword [esp+56] ;index
	mov ecx,dword [esp+40] ;outPt
	ALIGN 16
vf8lop4:

	mov edi,dword [esp+60] ;weight
	movdqa xmm5,[edi]
	punpcklqdq xmm5,[edi+16]
	movdqa xmm6,[edi+32]
	movdqa xmm7,[edi+48]
	add edi,64
	mov dword [esp+60],edi ;weight

	mov esi,dword [esp+36] ;inPt
	mov edi,dword [esp+72] ;rgbTable
	mov ebp,dword [esp+44] ;width
	ALIGN 16
vf8lop5:
	mov eax,dword [ebx]
	mov edx,dword [ebx+4]
	movdqu xmm2,[esp+0] ;tmpV
	movdqa xmm3,xmm2
	movdqu xmm0,[esi+eax]
	movdqu xmm1,[esi+edx]
	movdqa xmm4,xmm0
	punpcklwd xmm0, xmm1
	punpckhwd xmm4, xmm1
	movdqa xmm1,xmm5
	punpckldq xmm1,xmm5
	pmaddwd xmm0,xmm1
	pmaddwd xmm4,xmm1
	mov eax,dword [ebx+8]
	mov edx,dword [ebx+12]
	paddd xmm2,xmm0
	paddd xmm3,xmm4
	movdqu xmm0,[esi+eax]
	movdqu xmm1,[esi+edx]
	movdqa xmm4,xmm0
	punpcklwd xmm0, xmm1
	punpckhwd xmm4, xmm1
	movdqa xmm1,xmm5
	punpckhdq xmm1,xmm5
	pmaddwd xmm0,xmm1
	pmaddwd xmm4,xmm1
	mov eax,dword [ebx+16]
	mov edx,dword [ebx+20]
	paddd xmm2,xmm0
	paddd xmm3,xmm4
	movdqu xmm0,[esi+eax]
	movdqu xmm1,[esi+edx]
	movdqa xmm4,xmm0
	punpcklwd xmm0, xmm1
	punpckhwd xmm4, xmm1
	pmaddwd xmm0,xmm6
	pmaddwd xmm4,xmm6
	mov eax,dword [ebx+24]
	mov edx,dword [ebx+28]
	paddd xmm2,xmm0
	paddd xmm3,xmm4
	movdqu xmm0,[esi+eax]
	movdqu xmm1,[esi+edx]
	movdqa xmm4,xmm0
	punpcklwd xmm0, xmm1
	punpckhwd xmm4, xmm1
	pmaddwd xmm0,xmm7
	pmaddwd xmm4,xmm7
	paddd xmm2,xmm0
	paddd xmm3,xmm4

	psrad xmm3,15
	psrad xmm2,15
	packssdw xmm2,xmm3
	pextrw edx,xmm2,0
	mov eax,dword [edi+edx*4]
	pextrw edx,xmm2,1
	or eax,dword [edi+edx*4+262144]
	pextrw edx,xmm2,2
	or eax,dword [edi+edx*4+524288]
	pextrw edx,xmm2,3
	or eax,dword [edi+edx*4+786432]
	movnti dword [ecx],eax

	pextrw edx,xmm2,4
	mov eax,dword [edi+edx*4]
	pextrw edx,xmm2,5
	or eax,dword [edi+edx*4+262144]
	pextrw edx,xmm2,6
	or eax,dword [edi+edx*4+524288]
	pextrw edx,xmm2,7
	or eax,dword [edi+edx*4+786432]
	movnti dword [ecx+4],eax
	lea esi,[esi+16]
	lea ecx,[ecx+8]
	dec ebp
	jnz vf8lop5

	add ebx,32
	add ecx,dword [esp+68] ;dAdd

	dec dword [esp+48] ;currHeight
	jnz vf8lop4
	jmp vfexit
	
	align 16
vfstart:
	shr dword [esp+52],1 ;tap
	shr dword [esp+44],1 ;width
	mov ecx,dword [esp+40] ;outPt
	movdqu xmm7,[esp+0] ;tmpV
	pxor xmm3,xmm3
vflop4:
	mov esi,dword [esp+36] ;inPt
	mov ebp,dword [esp+44] ;width

vflop5:
	mov edi,dword [esp+60] ;weight
	mov ebx,dword [esp+56] ;index

	mov edx,dword [esp+52] ;tap
	movdqa xmm2,xmm7
	movdqa xmm6,xmm7
	ALIGN 16
vflop6:
	mov eax,dword [ebx]
	movdqu xmm0,[esi+eax]
	mov eax,dword [ebx+4]
	movdqu xmm4,xmm0
	movdqu xmm1,[esi+eax]
	punpcklwd xmm0, xmm1
	punpckhwd xmm4, xmm1
	movdqa xmm5,[edi]
	pmaddwd xmm0,xmm5
	pmaddwd xmm4,xmm5
	paddd xmm2,xmm0
	paddd xmm6,xmm4
	lea edi,[edi+16]
	lea ebx,[ebx+8]

	dec edx
	jnz vflop6

	mov edi,dword [esp+72] ;rgbTable

	psrad xmm6,15
	packssdw xmm6,xmm3
	pextrw edx,xmm6,0
	mov eax,dword [edi+edx*4]
	pextrw edx,xmm6,1
	or eax,dword [edi+edx*4+262144]
	pextrw edx,xmm6,2
	or eax,dword [edi+edx*4+524288]
	pextrw edx,xmm6,3
	or eax,dword [edi+edx*4+786432]
	movd xmm0,eax

	psrad xmm2,15
	packssdw xmm2,xmm3
	pextrw edx,xmm2,0
	mov eax,dword [edi+edx*4]
	pextrw edx,xmm2,1
	or eax,dword [edi+edx*4+262144]
	pextrw edx,xmm2,2
	or eax,dword [edi+edx*4+524288]
	pextrw edx,xmm2,3
	or eax,dword [edi+edx*4+786432]

	mov dword [ecx],eax
	movd dword [ecx+4],xmm0
	lea ecx,[ecx+8]
	lea esi,[esi+16]
	dec ebp
	jnz vflop5

	mov eax,dword [esp+52] ;tap
	lea ebx,[eax*8]
	shl eax,4
	add dword [esp+60],eax ;weight
	add dword [esp+56],ebx ;index
	add ecx,dword [esp+68] ;dAdd

	dec dword [esp+48] ;currHeight
	jnz vflop4
	
	align 16
vfexit:
	add esp,16
	pop edi
	pop esi
	pop ebx
	pop ebp
	ret

;void LanczosResizerLR_C32_CPU_vertical_filter_na(UInt8 *inPt, UInt8 *outPt, IntOS width, IntOS height, IntOS tap, IntOS *index, Int64 *weight, IntOS sstep, IntOS dstep, UInt8 *rgbTable)
;0 tmpV
;16 edi
;20 esi
;24 ebx
;28 ebp
;32 retAddr
;36 inPt
;40 outPt
;44 width
;48 height / currHeight
;52 tap
;56 index
;60 weight
;64 sstep
;68 dstep / dAdd
;72 rgbTable

	align 16
_LanczosResizerLR_C32_CPU_vertical_filter_na:
	mov eax,16384
	push ebp
	push ebx
	push esi
	push edi
	push eax
	push eax
	push eax
	push eax						;Int32 tmpV[4] = {16384, 16384, 16384, 16384};
	mov edx,dword [esp+44] ;width
	mov eax,dword [esp+52] ;tap
	lea ecx,[edx*4]
	sub dword [esp+68],ecx			;IntOS dAdd = dstep - width * 4;
	test edx,1						;if (width & 1)
	jz vfnastart2
	cmp eax,6
	jz vf6na_1start
	jmp vfna_1start
	
vf6na_1start:						;if (tap == 6)
	shr dword [esp+44],1 ;width
	mov edi,dword [esp+72] ;rgbTable
	mov ebx,dword [esp+60] ;weight
	mov ecx,dword [esp+40] ;outPt
	ALIGN 16
vf6na_1lop4:

	movdqa xmm5,[ebx+32]
	movdqa xmm6,[ebx+48]
	movdqa xmm7,[ebx+64]

	mov esi,dword [esp+36] ;inPt
	mov ebp,dword [esp+44] ;width
	ALIGN 16
vf6na_1lop5:
	mov eax,dword [ebx]
	movdqu xmm2,[esp+0] ;tmpV
	mov edx,dword [ebx+4]
	movdqa xmm3,xmm2
	movdqu xmm0,[esi+eax]
	movdqa xmm4,xmm0
	movdqu xmm1,[esi+edx]
	punpcklwd xmm0, xmm1
	punpckhwd xmm4, xmm1
	mov eax,dword [ebx+8]
	pmaddwd xmm0,xmm5
	pmaddwd xmm4,xmm5
	mov edx,dword [ebx+12]
	paddd xmm2,xmm0
	paddd xmm3,xmm4
	movdqu xmm0,[esi+eax]
	movdqa xmm4,xmm0
	movdqu xmm1,[esi+edx]
	punpcklwd xmm0, xmm1
	punpckhwd xmm4, xmm1
	mov eax,dword [ebx+16]
	pmaddwd xmm0,xmm6
	pmaddwd xmm4,xmm6
	mov edx,dword [ebx+20]
	paddd xmm2,xmm0
	paddd xmm3,xmm4
	movdqu xmm0,[esi+eax]
	movdqa xmm4,xmm0
	movdqu xmm1,[esi+edx]
	punpcklwd xmm0, xmm1
	punpckhwd xmm4, xmm1
	pmaddwd xmm0,xmm7
	pmaddwd xmm4,xmm7
	paddd xmm2,xmm0
	paddd xmm3,xmm4

	psrad xmm3,15
	psrad xmm2,15
	packssdw xmm2,xmm3
	pextrw edx,xmm2,0
	mov eax,dword [edi+edx*4]
	pextrw edx,xmm2,1
	or eax,dword [edi+edx*4+262144]
	pextrw edx,xmm2,2
	or eax,dword [edi+edx*4+524288]
	movnti dword [ecx],eax

	pextrw edx,xmm2,4
	mov eax,dword [edi+edx*4]
	pextrw edx,xmm2,5
	or eax,dword [edi+edx*4+262144]
	pextrw edx,xmm2,6
	or eax,dword [edi+edx*4+524288]
	movnti dword [ecx+4],eax
	lea esi,[esi+16]
	lea ecx,[ecx+8]
	dec ebp
	jnz vf6na_1lop5

	mov eax,dword [ebx]
	movdqu xmm2,[esp+0] ;tmpV
	mov edx,dword [ebx+4]
	movq xmm0,[esi+eax]
	movq xmm1,[esi+edx]
	punpcklwd xmm0, xmm1
	mov eax,dword [ebx+8]
	pmaddwd xmm0,xmm5
	mov edx,dword [ebx+12]
	paddd xmm2,xmm0
	movdqu xmm0,[esi+eax]
	movdqu xmm1,[esi+edx]
	punpcklwd xmm0, xmm1
	mov eax,dword [ebx+16]
	pmaddwd xmm0,xmm6
	mov edx,dword [ebx+20]
	paddd xmm2,xmm0
	movdqu xmm0,[esi+eax]
	movdqu xmm1,[esi+edx]
	punpcklwd xmm0, xmm1
	pmaddwd xmm0,xmm7
	paddd xmm2,xmm0

	psrad xmm2,15
	packssdw xmm2,xmm2
	pextrw edx,xmm2,0
	mov eax,dword [edi+edx*4]
	pextrw edx,xmm2,1
	or eax,dword [edi+edx*4+262144]
	pextrw edx,xmm2,2
	or eax,dword [edi+edx*4+524288]
	movnti dword [ecx],eax
	lea ecx,[ecx+4]

	add ebx,80
	add ecx,dword [esp+68] ;dAdd

	dec dword [esp+48] ;currHeight
	jnz vf6na_1lop4
	jmp vfnaexit

	align 16
vfna_1start:
	shr dword [esp+52],1 ;tap
	pxor xmm3,xmm3
	mov ecx,dword [esp+40] ;outPt
	align 16
vfnalop:
	mov esi,dword [esp+36] ;inPt
	mov ebp,dword [esp+44] ;width

	align 16
vfnalop2:
	mov ebx,dword [esp+56] ;index
	mov edi,dword [esp+60] ;weight

	mov edx,dword [esp+52] ;tap
	pxor xmm1,xmm1
	ALIGN 16
vfnalop3:
	mov eax,dword [ebx]
	movq xmm0,[esi+eax]
	mov eax,dword [ebx+4]
	movq xmm2,[esi+eax]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[edi]
	paddd xmm1,xmm0
	lea ebx,[ebx+8]
	lea edi,[edi+16]
	dec edx
	jnz vfnalop3

	psrad xmm1,15
	packssdw xmm1,xmm3
	mov edi,dword [esp+72] ;rgbTable

	pextrw edx,xmm1,0
	mov eax,dword [edi+edx*4]
	pextrw edx,xmm1,1
	or eax,dword [edi+edx*4+262144]
	pextrw edx,xmm1,2
	or eax,dword [edi+edx*4+524288]

	mov dword [ecx],eax
	lea ecx,[ecx+4]
	lea esi,[esi+8]
	dec ebp
	jnz vfnalop2

	mov eax,dword [esp+52] ;tap
	lea ebx,[eax*8]
	shl eax,4
	add dword [esp+56],ebx ;index
	add dword [esp+60],eax ;weight

	add ecx,dword [esp+68] ;dAdd

	dec dword [esp+48] ;currHeight
	jnz vfnalop
	jmp vfnaexit

	align 16
vfnastart2:
	cmp eax,6
	jz vf6nastart
	cmp eax,7
	jz vf8nastart
	jmp vfnastart
	
	align 16
vf6nastart:						; else if (tap == 6)
	shr dword [esp+44],1 ;width
	mov ecx,dword [esp+40] ;outPt
	mov ebx,dword [esp+60] ;weight
	mov edi,dword [esp+72] ;rgbTable
	ALIGN 16
vf6nalop4:
	movdqa xmm5,[ebx+32]
	movdqa xmm6,[ebx+48]
	movdqa xmm7,[ebx+64]

	mov esi,dword [esp+36] ;inPt
	mov ebp,dword [esp+44] ;width
	ALIGN 16
vf6nalop5:
	mov eax,dword [ebx]
	pxor xmm2,xmm2
	mov edx,dword [ebx+4]
	pxor xmm3,xmm3
	movdqu xmm0,[esi+eax]
	movdqa xmm4,xmm0
	movdqu xmm1,[esi+edx]
	punpcklwd xmm0, xmm1
	punpckhwd xmm4, xmm1
	pmaddwd xmm0,xmm5
	mov eax,dword [ebx+8]
	pmaddwd xmm4,xmm5
	mov edx,dword [ebx+12]
	paddd xmm2,xmm0
	movdqu xmm0,[esi+eax]
	paddd xmm3,xmm4
	movdqu xmm1,[esi+edx]
	movdqa xmm4,xmm0
	punpcklwd xmm0, xmm1
	punpckhwd xmm4, xmm1
	pmaddwd xmm0,xmm6
	mov eax,dword [ebx+16]
	pmaddwd xmm4,xmm6
	mov edx,dword [ebx+20]
	paddd xmm2,xmm0
	movdqu xmm0,[esi+eax]
	paddd xmm3,xmm4
	movdqu xmm1,[esi+edx]
	movdqa xmm4,xmm0
	punpcklwd xmm0, xmm1
	punpckhwd xmm4, xmm1
	pmaddwd xmm0,xmm7
	pmaddwd xmm4,xmm7
	paddd xmm2,xmm0
	paddd xmm3,xmm4

	psrad xmm3,15
	psrad xmm2,15
	packssdw xmm2,xmm3
	pextrw edx,xmm2,0
	mov eax,dword [edi+edx*4]
	pextrw edx,xmm2,1
	or eax,dword [edi+edx*4+262144]
	pextrw edx,xmm2,2
	or eax,dword [edi+edx*4+524288]
	movnti dword [ecx],eax

	pextrw edx,xmm2,4
	mov eax,dword [edi+edx*4]
	pextrw edx,xmm2,5
	or eax,dword [edi+edx*4+262144]
	pextrw edx,xmm2,6
	or eax,dword [edi+edx*4+524288]
	movnti dword [ecx+4],eax

	lea ecx,[ecx+8]
	lea esi,[esi+16]
	dec ebp
	jnz vf6nalop5

	lea ebx,[ebx+80]
	add ecx,dword [esp+68] ;dAdd

	dec dword [esp+48] ;currHeight
	jnz vf6nalop4
	jmp vfnaexit
	
	align 16
vf8nastart:						;else if (tap == 8)
	shr dword [esp+44],1 ;width
	mov ecx,dword [esp+40] ;outPt
	mov ebx,dword [esp+56] ;index
	mov edi,dword [esp+72] ;rgbTable
	ALIGN 16
vf8nalop4:

	mov ebp,dword [esp+60] ;weight
	movdqa xmm5,[ebp]
	punpcklqdq xmm5,[ebp+16]
	movdqa xmm6,[ebp+32]
	movdqa xmm7,[ebp+48]
	add ebp,64
	mov dword [esp+60],ebp ;weight

	mov esi,dword [esp+36] ;inPt
	mov ebp,dword [esp+44] ;width
	ALIGN 16
vf8nalop5:
	mov eax,dword [ebx]
	pxor xmm2,xmm2
	mov edx,dword [ebx+4]
	pxor xmm3,xmm3
	movdqu xmm0,[esi+eax]
	movdqa xmm4,xmm0
	movdqu xmm1,[esi+edx]
	punpcklwd xmm0, xmm1
	punpckhwd xmm4, xmm1
	movdqa xmm1,xmm5
	punpckldq xmm1,xmm5
	mov eax,dword [ebx+8]
	pmaddwd xmm0,xmm1
	pmaddwd xmm4,xmm1
	mov edx,dword [ebx+12]
	paddd xmm2,xmm0
	movdqu xmm0,[esi+eax]
	paddd xmm3,xmm4
	movdqu xmm1,[esi+edx]
	movdqa xmm4,xmm0
	punpcklwd xmm0, xmm1
	punpckhwd xmm4, xmm1
	movdqa xmm1,xmm5
	punpckhdq xmm1,xmm5
	mov eax,dword [ebx+16]
	pmaddwd xmm0,xmm1
	pmaddwd xmm4,xmm1
	mov edx,dword [ebx+20]
	paddd xmm2,xmm0
	movdqu xmm0,[esi+eax]
	paddd xmm3,xmm4
	movdqu xmm1,[esi+edx]
	movdqa xmm4,xmm0
	punpcklwd xmm0, xmm1
	punpckhwd xmm4, xmm1
	mov eax,dword [ebx+24]
	pmaddwd xmm0,xmm6
	pmaddwd xmm4,xmm6
	mov edx,dword [ebx+28]
	paddd xmm2,xmm0
	movdqu xmm0,[esi+eax]
	paddd xmm3,xmm4
	movdqu xmm1,[esi+edx]
	movdqa xmm4,xmm0
	punpcklwd xmm0, xmm1
	punpckhwd xmm4, xmm1
	pmaddwd xmm0,xmm7
	pmaddwd xmm4,xmm7
	paddd xmm2,xmm0
	paddd xmm3,xmm4

	psrad xmm3,15
	psrad xmm2,15
	packssdw xmm2,xmm3
	pextrw edx,xmm2,0
	mov eax,dword [edi+edx*4]
	pextrw edx,xmm2,1
	or eax,dword [edi+edx*4+262144]
	pextrw edx,xmm2,2
	or eax,dword [edi+edx*4+524288]
	movnti dword [ecx],eax

	pextrw edx,xmm2,4
	mov eax,dword [edi+edx*4]
	pextrw edx,xmm2,5
	or eax,dword [edi+edx*4+262144]
	pextrw edx,xmm2,6
	or eax,dword [edi+edx*4+524288]
	movnti dword [ecx+4],eax

	lea ecx,[ecx+8]
	lea esi,[esi+16]
	dec ebp
	jnz vf8nalop5

	lea ebx,[ebx+32]
	add ecx,dword [esp+68] ;dAdd

	dec dword [esp+48] ;currHeight
	jnz vf8nalop4
	jmp vfnaexit
	
	align 16
vfnastart:
	shr dword [esp+52],1 ;tap
	shr dword [esp+44],1 ;width
	mov ecx,dword [esp+40] ;outPt
	pxor xmm3,xmm3
	align 16
vfnalop4:
	mov esi,dword [esp+36] ;inPt
	mov ebp,dword [esp+44] ;width

	align 16
vfnalop5:
	mov edi,dword [esp+60] ;weight
	mov ebx,dword [esp+56] ;index

	mov edx,dword [esp+52] ;tap
	pxor xmm2,xmm2
	pxor xmm6,xmm6
	ALIGN 16
vfnalop6:
	mov eax,dword [ebx]
	movdqu xmm0,[esi+eax]
	mov eax,dword [ebx+4]
	movdqa xmm4,xmm0
	movdqu xmm1,[esi+eax]
	punpcklwd xmm0, xmm1
	punpckhwd xmm4, xmm1
	movdqa xmm5,[edi]
	pmaddwd xmm0,xmm5
	pmaddwd xmm4,xmm5
	paddd xmm2,xmm0
	paddd xmm6,xmm4
	lea edi,[edi+16]
	lea ebx,[ebx+8]

	dec edx
	jnz vfnalop6

	mov edi,dword [esp+72] ;rgbTable

	psrad xmm6,15
	packssdw xmm6,xmm3
	pextrw edx,xmm6,0
	mov eax,dword [edi+edx*4]
	pextrw edx,xmm6,1
	or eax,dword [edi+edx*4+262144]
	pextrw edx,xmm6,2
	or eax,dword [edi+edx*4+524288]
	movd xmm0,eax

	psrad xmm2,15
	packssdw xmm2,xmm3
	pextrw edx,xmm2,0
	mov eax,dword [edi+edx*4]
	pextrw edx,xmm2,1
	or eax,dword [edi+edx*4+262144]
	pextrw edx,xmm2,2
	or eax,dword [edi+edx*4+524288]

	mov dword [ecx],eax
	movd dword [ecx+4],xmm0
	lea ecx,[ecx+8]
	lea esi,[esi+16]
	dec ebp
	jnz vfnalop5

	mov eax,dword [esp+52] ;tap
	lea edx,[eax*8]
	shl eax,4
	add dword [esp+56],edx ;index
	add dword [esp+60],eax ;weight

	add ecx,dword [esp+68] ;dAdd

	dec dword [esp+48] ;currHeight
	jnz vfnalop4

	align 16
vfnaexit:
	add esp,16
	pop edi
	pop esi
	pop ebx
	pop ebp
	ret

;void LanczosResizerLR_C32_CPU_hv_filter(UInt8 *inPt, UInt8 *outPt, IntOS dwidth, IntOS dheight, IntOS swidth, Int32 htap, Int32 *hindex, Int64 *hweight, Int32 vtap, Int32 *vindex, Int64 *vweight, IntOS sstep, IntOS dstep, UInt8 *rgbTable, UInt8 *buffPt)
;0 currWidth
;4 edi
;8 esi
;12 ebx
;16 ebp
;20 retAddr
;24 inPt
;28 outPt
;32 dwidth
;36 dheight / currHeight
;40 swidth
;44 htap
;48 hindex
;52 hweight
;56 vtap
;60 vindex
;64 vweight
;68 sstep
;72 dstep / dAdd
;76 rgbTable
;80 buffPt

	align 16
_LanczosResizerLR_C32_CPU_hv_filter:
	push ebp
	push ebx
	push esi
	push edi
	push eax
	cmp dword [esp+44],6 ;htap
	jnz hvfexit
	cmp dword [esp+56],6 ;vtap
	jnz hvfexit

	shr dword [esp+40],1 ;swidth
	shr dword [esp+32],1 ;dwidth
hvf6lop1:
	mov edi,dword [esp+64] ;vweight
	movdqa xmm5,[edi]
	movdqa xmm6,[edi+16]
	movdqa xmm7,[edi+32]

	mov esi,dword [esp+24] ;inPt
	mov ecx,dword [esp+80] ;buffPt

	mov edi,dword [esp+40] ;swidth
	ALIGN 16
hvf6lop2:
	mov ebx,dword [esp+60] ;vindex
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

	psrad xmm3,15
	psrad xmm2,15
	packssdw xmm2,xmm3
	movdqa [ecx],xmm2

	add ecx,16
	add esi,16
	dec edi
	jnz hvf6lop2

	add dword [esp+60],24 ;vindex
	add dword [esp+64],48 ;vweight

	mov eax,dword [esp+32] ;dwidth
	mov esi,dword [esp+28] ;outPt
	mov dword [esp+0],eax ;currWidth

	mov edi,dword [esp+52] ;hweight
	mov ebx,dword [esp+48] ;hindex
	ALIGN 16
hvf6lop3:
	mov ecx,dword [esp+80] ;buffPt
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
	add edi,48
	add ebx,24

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
	add edi,48
	add ebx,24

	psrad xmm1,15
	psrad xmm3,15
	packssdw xmm1,xmm3

	mov ecx,dword [esp+76] ;rgbTable
	pextrw edx,xmm1,0
	mov eax,dword [ecx+edx*4]
	pextrw edx,xmm1,1
	or eax,dword [ecx+edx*4+262144]
	pextrw edx,xmm1,2
	or eax,dword [ecx+edx*4+524288]
	mov dword [esi],eax

	pextrw edx,xmm1,4
	mov eax,dword [ecx+edx*4]
	pextrw edx,xmm1,5
	or eax,dword [ecx+edx*4+262144]
	pextrw edx,xmm1,6
	or eax,dword [ecx+edx*4+524288]
	mov dword [esi+4],eax

	add esi,8
	dec dword [esp+0] ;currWidth
	jnz hvf6lop3

	mov edx,dword [esp+72] ;dstep
	add dword [esp+28],edx ;outPt

	dec dword [esp+36] ;currHeight
	jnz hvf6lop1
	
	align 16
hvfexit:
	pop eax
	pop edi
	pop esi
	pop ebx
	pop ebp
	ret

;void LanczosResizerLR_C32_CPU_collapse(UInt8 *inPt, UInt8 *outPt, IntOS width, IntOS height, IntOS sstep, IntOS dstep, UInt8 *rgbTable)
;0 edi
;4 esi
;8 ebx
;12 ebp
;16 retAddr
;20 inPt
;24 outPt
;28 width
;32 height
;36 sstep / sAdd
;40 dstep / dAdd
;44 rgbTable

	align 16
_LanczosResizerLR_C32_CPU_collapse:
	push ebp
	push ebx
	push esi
	push edi
	mov eax,dword [esp+28] ;width
	lea ecx,[eax*8]
	lea edx,[eax*4]
	sub dword [esp+36],ecx ;sAdd		IntOS sAdd = sstep - width * 8;
	sub dword [esp+40],edx ;dAdd		IntOS dAdd = dstep - width * 4;
	test eax,3
	jnz colstart
	test dword [esp+24],15 ;outPt
	jnz colstart
	test dword [esp+40],15 ;dstep
	jz col16start
	align 16
colstart:						;if ((width & 3) || (((IntOS)outPt) & 15) || (dstep & 15))
	mov ebx,dword [esp+44] ;rgbTable
	mov edi,dword [esp+32] ;height
	mov ecx,dword [esp+20] ;inPt
	mov esi,dword [esp+24] ;outPt
	align 16
collop:
	mov ebp,dword [esp+28] ;width
	align 16
collop2:
	movzx edx,word [ecx]
	mov eax,dword [ebx+edx*4]
	movzx edx,word [ecx+2]
	or eax,dword [ebx+edx*4+262144]
	movzx edx,word [ecx+4]
	or eax,dword [ebx+edx*4+524288]
	movzx edx,word [ecx+6]
	or eax,dword [ebx+edx*4+786432]

	movnti dword [esi],eax
	lea esi,[esi+4]
	lea ecx,[ecx+8]
	dec ebp
	jnz collop2

	add ecx,dword [esp+36] ;sAdd
	add esi,dword [esp+40] ;dAdd

	dec edi
	jnz collop
	jmp colexit

	align 16
col16start:
	shr dword [esp+28],2 ;width
	mov ebx,dword [esp+44] ;rgbTable
	mov ebp,dword [esp+32] ;height
	mov ecx,dword [esp+20] ;inPt
	mov esi,dword [esp+24] ;outPt
	align 16
collop16:
	mov edi,dword [esp+28] ;width

	align 16
collop16_2:
	movdqu xmm4,[ecx]
	movdqu xmm5,[ecx+16]
	pextrw edx,xmm4,0
	mov eax,dword [ebx+edx*4]
	pextrw edx,xmm4,1
	or eax,dword [ebx+edx*4+262144]
	pextrw edx,xmm4,2
	or eax,dword [ebx+edx*4+524288]
	pextrw edx,xmm4,3
	or eax,dword [ebx+edx*4+786432]
	movd xmm0,eax

	pextrw edx,xmm4,4
	mov eax,dword [ebx+edx*4]
	pextrw edx,xmm4,5
	or eax,dword [ebx+edx*4+262144]
	pextrw edx,xmm4,6
	or eax,dword [ebx+edx*4+524288]
	pextrw edx,xmm4,7
	or eax,dword [ebx+edx*4+786432]
	movd xmm1,eax
	punpckldq xmm0,xmm1

	pextrw edx,xmm5,0
	mov eax,dword [ebx+edx*4]
	pextrw edx,xmm5,1
	or eax,dword [ebx+edx*4+262144]
	pextrw edx,xmm5,2
	or eax,dword [ebx+edx*4+524288]
	pextrw edx,xmm5,3
	or eax,dword [ebx+edx*4+786432]
	movd xmm1,eax

	pextrw edx,xmm5,4
	mov eax,dword [ebx+edx*4]
	pextrw edx,xmm5,5
	or eax,dword [ebx+edx*4+262144]
	pextrw edx,xmm5,6
	or eax,dword [ebx+edx*4+524288]
	pextrw edx,xmm5,7
	or eax,dword [ebx+edx*4+786432]
	movd xmm2,eax
	punpckldq xmm1,xmm2
	punpcklqdq xmm0,xmm1

	movntdq [esi],xmm0
	lea ecx,[ecx+32]
	lea esi,[esi+16]
	dec edi
	jnz collop16_2

	add ecx,dword [esp+36] ;sAdd
	add esi,dword [esp+40] ;dAdd

	dec ebp
	jnz collop16

	align 16
colexit:
	pop edi
	pop esi
	pop ebx
	pop ebp
	ret

;void LanczosResizerLR_C32_CPU_collapse_na(UInt8 *inPt, UInt8 *outPt, IntOS width, IntOS height, IntOS sstep, IntOS dstep, UInt8 *rgbTable)
;0 edi
;4 esi
;8 ebx
;12 ebp
;16 retAddr
;20 inPt
;24 outPt
;28 width
;32 height
;36 sstep / sAdd
;40 dstep / dAdd
;44 rgbTable

	align 16
_LanczosResizerLR_C32_CPU_collapse_na:
	push ebp
	push ebx
	push esi
	push edi
	mov eax,dword [esp+28] ;width
	lea ecx,[eax*8]
	lea edx,[eax*4]
	sub dword [esp+36],ecx ;sAdd		IntOS sAdd = sstep - width * 8;
	sub dword [esp+40],edx ;dAdd		IntOS dAdd = dstep - width * 4;
	test eax,3
	jnz colnastart
	test dword [esp+24],15 ;outPt
	jnz colnastart
	test dword [esp+40],15 ;dstep
	jz colna16start
	
	align 16
colnastart:					; if ((width & 3) || (((IntOS)outPt) & 15) || (dstep & 15))
	mov ecx,dword [esp+20] ;inPt
	mov esi,dword [esp+24] ;outPt
	mov ebp,dword [esp+32] ;height
	mov ebx,dword [esp+44] ;rgbTable

	align 16
colnalop:
	mov edi,dword [esp+28] ;width
	
	align 16
colnalop2:
	movzx edx,word [ecx]
	mov eax,dword [ebx+edx*4]
	movzx edx,word [ecx+2]
	or eax,dword [ebx+edx*4+262144]
	movzx edx,word [ecx+4]
	or eax,dword [ebx+edx*4+524288]

	mov dword [esi],eax
	lea esi,[esi+4]
	lea ecx,[ecx+8]
	dec edi
	jnz colnalop2

	add ecx,dword [esp+36] ;sAdd
	add esi,dword [esp+40] ;dAdd

	dec ebp
	jnz colnalop
	jmp colnaexit
	
	align 16
colna16start:
	shr dword [esp+28],2 ;width
	mov ebx,dword [esp+44] ;rgbTable
	mov ebp,dword [esp+32] ;height
	mov esi,dword [esp+24] ;outPt
	mov ecx,dword [esp+20] ;inPt
	align 16
colnalop16:
	mov edi,dword [esp+28] ;width

	align 16
colnalop16_2:
	prefetcht0 [ecx+128]
	movdqu xmm4,[ecx]
	movdqu xmm5,[ecx+16]
	pextrw edx,xmm4,0
	mov eax,dword [ebx+edx*4]
	pextrw edx,xmm4,1
	or eax,dword [ebx+edx*4+262144]
	pextrw edx,xmm4,2
	or eax,dword [ebx+edx*4+524288]
	movd xmm0,eax

	pextrw edx,xmm4,4
	mov eax,dword [ebx+edx*4]
	pextrw edx,xmm4,5
	or eax,dword [ebx+edx*4+262144]
	pextrw edx,xmm4,6
	or eax,dword [ebx+edx*4+524288]
	movd xmm1,eax
	punpckldq xmm0,xmm1

	pextrw edx,xmm5,0
	mov eax,dword [ebx+edx*4]
	pextrw edx,xmm5,1
	or eax,dword [ebx+edx*4+262144]
	pextrw edx,xmm5,2
	or eax,dword [ebx+edx*4+524288]
	movd xmm1,eax

	pextrw edx,xmm5,4
	mov eax,dword [ebx+edx*4]
	pextrw edx,xmm5,5
	or eax,dword [ebx+edx*4+262144]
	pextrw edx,xmm5,6
	or eax,dword [ebx+edx*4+524288]
	movd xmm2,eax
	punpckldq xmm1,xmm2
	punpcklqdq xmm0,xmm1

	movntdq [esi],xmm0
	lea esi,[esi+16]
	lea ecx,[ecx+32]
	dec edi
	jnz colnalop16_2

	add ecx,dword [esp+36] ;sAdd
	add esi,dword [esp+40] ;dAdd

	dec ebp
	jnz colnalop16

	align 16
colnaexit:
	pop edi
	pop esi
	pop ebx
	pop ebp
	ret
