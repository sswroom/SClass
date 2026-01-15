section .text

global _LanczosResizerH8_8_horizontal_filter
global _LanczosResizerH8_8_horizontal_filter8
global _LanczosResizerH8_8_vertical_filter
global _LanczosResizerH8_8_expand
global _LanczosResizerH8_8_collapse

;void LanczosResizerH8_8_horizontal_filter(UInt8 *inPt, UInt8 *outPt, IntOS width, IntOS height, IntOS tap, IntOS *index, Int64 *weight, IntOS sstep, IntOS dstep)
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
_LanczosResizerH8_8_horizontal_filter:
	push ebp
	push ebx
	push esi
	push edi
	mov eax,0xff80ff80
	mov edx,0x80808080
	movd xmm5,eax
	movd xmm6,edx
	punpckldq xmm5,xmm5
	punpckldq xmm6,xmm6
	punpckldq xmm5,xmm5						;UInt8 toAdd[16] = {0x80, 0xff, 0x80, 0xff, 0x80, 0xff, 0x80, 0xff, 0x80, 0xff, 0x80, 0xff, 0x80, 0xff, 0x80, 0xff};
	punpckldq xmm6,xmm6						;UInt8 toAdd2[16] = {0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80};
	mov eax,dword [esp+28] ;width
	lea edx,[eax*4]
	sub dword [esp+52],edx ;dstep			dstep = dstep - width * 4;
	test eax,3
	jnz hfstart
	mov edx,dword [esp+36] ;tap
	test edx,1
	jnz hfstart
	cmp edx,6
	jz hf6start
	test eax,15
	jnz hfstart2
	cmp edx,8
	jz hf8start
	jmp hfstart2

	align 16
hfstart:									;if (width & 3 || tap & 1)
	pxor xmm3,xmm3
	mov ecx,dword [esp+20] ;inPt
	mov esi,dword [esp+24] ;outPt
	align 16
hflop:
	mov ebp,dword [esp+28] ;width
	mov ebx,dword [esp+40] ;index
	mov edi,dword [esp+44] ;weight
	align 16
hflop2:
	mov edx,dword [esp+36] ;tap
	pxor xmm1,xmm1
	align 16
hflop3:
	mov eax,dword [ebx]
	movq xmm0,[ecx+eax*2]
	movq xmm2,[edi]
	pmulhw xmm0,xmm2
	paddsw xmm1,xmm0
	lea edi,[edi+8]
	lea ebx,[ebx+4]
	dec edx
	jnz hflop3

	psraw xmm1,5
	paddw xmm1,xmm5
	packsswb xmm1,xmm3
	paddb xmm1,xmm6

	movd dword [esi],xmm1
	lea esi,[esi+4]
	dec ebp
	jnz hflop2

	add ecx,dword [esp+48] ;sstep
	add esi,dword [esp+52] ;dstep

	dec dword [esp+32] ;currHeight
	jnz hflop
	jmp hfexit

	align 16
hf6start:							;else if (tap == 6)
	mov ecx,dword [esp+20] ;inPt
	mov esi,dword [esp+24] ;outPt
	mov edx,dword [esp+32] ;height
	align 16
hf6lop4:
	mov ebp,dword [esp+28] ;width
	shr ebp,2

	mov ebx,dword [esp+40] ;index
	mov edi,dword [esp+44] ;weight
	ALIGN 16
hf6lop5:

	pxor xmm1,xmm1
	pxor xmm2,xmm2
	mov eax,dword [ebx]
	movq xmm0,[ecx+eax*2]
	mov eax,dword [ebx+4]
	movq xmm3,[ecx+eax*2]
	punpcklqdq xmm0,xmm3
	pmulhw xmm0,[edi]
	paddsw xmm1,xmm0
	mov eax,dword [ebx+8]
	movq xmm0,[ecx+eax*2]
	mov eax,dword [ebx+12]
	movq xmm3,[ecx+eax*2]
	punpcklqdq xmm0,xmm3
	pmulhw xmm0,[edi+16]
	paddsw xmm1,xmm0
	mov eax,dword [ebx+16]
	movq xmm0,[ecx+eax*2]
	mov eax,dword [ebx+20]
	movq xmm3,[ecx+eax*2]
	punpcklqdq xmm0,xmm3
	pmulhw xmm0,[edi+32]
	paddsw xmm1,xmm0
	lea ebx,[ebx+24]
	lea edi,[edi+48]

	mov eax,dword [ebx]
	movq xmm0,[ecx+eax*2]
	mov eax,dword [ebx+4]
	movq xmm3,[ecx+eax*2]
	punpcklqdq xmm0,xmm3
	pmulhw xmm0,[edi]
	paddsw xmm2,xmm0
	mov eax,dword [ebx+8]
	movq xmm0,[ecx+eax*2]
	mov eax,dword [ebx+12]
	movq xmm3,[ecx+eax*2]
	punpcklqdq xmm0,xmm3
	pmulhw xmm0,[edi+16]
	paddsw xmm2,xmm0
	mov eax,dword [ebx+16]
	movq xmm0,[ecx+eax*2]
	mov eax,dword [ebx+20]
	movq xmm3,[ecx+eax*2]
	punpcklqdq xmm0,xmm3
	pmulhw xmm0, [edi+32]
	paddsw xmm2,xmm0
	lea ebx,[ebx+24]
	lea edi,[edi+48]

	movdqa xmm0,xmm1
	punpcklqdq xmm0,xmm2
	punpckhqdq xmm1,xmm2
	paddsw xmm1,xmm0

	pxor xmm2,xmm2
	pxor xmm4,xmm4
	mov eax,dword [ebx]
	movq xmm0,[ecx+eax*2]
	mov eax,dword [ebx+4]
	movq xmm3,[ecx+eax*2]
	punpcklqdq xmm0,xmm3
	pmulhw xmm0,[edi]
	paddsw xmm2,xmm0
	mov eax,dword [ebx+8]
	movq xmm0,[ecx+eax*2]
	mov eax,dword [ebx+12]
	movq xmm3,[ecx+eax*2]
	punpcklqdq xmm0,xmm3
	pmulhw xmm0,[edi+16]
	paddsw xmm2,xmm0
	mov eax,dword [ebx+16]
	movq xmm0,[ecx+eax*2]
	mov eax,dword [ebx+20]
	movq xmm3,[ecx+eax*2]
	punpcklqdq xmm0,xmm3
	pmulhw xmm0,[edi+32]
	paddsw xmm2,xmm0
	lea ebx,[ebx+24]
	lea edi,[edi+48]

	mov eax,dword [ebx]
	movq xmm0,[ecx+eax*2]
	mov eax,dword [ebx+4]
	movq xmm3,[ecx+eax*2]
	punpcklqdq xmm0,xmm3
	pmulhw xmm0,[edi]
	paddsw xmm4,xmm0
	mov eax,dword [ebx+8]
	movq xmm0,[ecx+eax*2]
	mov eax,dword [ebx+12]
	movq xmm3,[ecx+eax*2]
	punpcklqdq xmm0,xmm3
	pmulhw xmm0,[edi+16]
	paddsw xmm4,xmm0
	mov eax,dword [ebx+16]
	movq xmm0,[ecx+eax*2]
	mov eax,dword [ebx+20]
	movq xmm3,[ecx+eax*2]
	punpcklqdq xmm0,xmm3
	pmulhw xmm0,[edi+32]
	paddsw xmm4,xmm0
	lea ebx,[ebx+24]
	lea edi,[edi+48]

	movdqa xmm0,xmm2
	punpcklqdq xmm0,xmm4
	punpckhqdq xmm2,xmm4
	paddsw xmm2,xmm0

	psraw xmm1,5
	psraw xmm2,5
	paddw xmm1,xmm5
	paddw xmm2,xmm5
	packsswb xmm1,xmm2
	paddb xmm1,xmm6
	movdqu [esi],xmm1
	lea esi,[esi+16]
	dec ebp
	jnz hf6lop5

	add ecx,dword [esp+48] ;sstep
	add esi,dword [esp+52] ;dstep

	dec edx
	jnz hf6lop4
	jmp hfexit

	align 16
hf8start:							;else if (tap == 8)
	shr dword [esp+28],2 ;width
	mov ebx,dword [esp+32] ;height
	mov ecx,dword [esp+20] ;inPt
	mov esi,dword [esp+24] ;outPt
	mov edx,dword [esp+52] ;dstep

	align 16
hf8lop4:

	mov ebp,dword [esp+28] ;width
	mov edi,dword [esp+44] ;weight
	ALIGN 16
hf8lop5:

	pxor xmm1,xmm1
	pxor xmm2,xmm2
	mov eax,dword [edi]
	movdqu xmm0,[ecx+eax*2]
	movdqu xmm3,[ecx+eax*2+16]
	pmulhw xmm0,[edi+16]
	pmulhw xmm3,[edi+32]
	paddsw xmm1,xmm0
	paddsw xmm1,xmm3
	movdqu xmm0,[ecx+eax*2+32]
	movdqu xmm3,[ecx+eax*2+48]
	pmulhw xmm0,[edi+48]
	pmulhw xmm3,[edi+64]
	paddsw xmm1,xmm0
	paddsw xmm1,xmm3

	mov eax,dword [edi+4]
	movdqu xmm0,[ecx+eax*2]
	movdqu xmm3,[ecx+eax*2+16]
	pmulhw xmm0,[edi+80]
	pmulhw xmm3,[edi+96]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm3
	movdqu xmm0,[ecx+eax*2+32]
	movdqu xmm3,[ecx+eax*2+48]
	pmulhw xmm0,[edi+112]
	pmulhw xmm3,[edi+128]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm3

	movdqa xmm0,xmm1
	punpcklqdq xmm0,xmm2
	punpckhqdq xmm1,xmm2
	paddsw xmm1,xmm0

	pxor xmm2,xmm2
	pxor xmm4,xmm4
	mov eax,dword [edi+8]
	movdqu xmm0,[ecx+eax*2]
	movdqu xmm3,[ecx+eax*2+16]
	pmulhw xmm0,[edi+144]
	pmulhw xmm3,[edi+160]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm3
	movdqu xmm0,[ecx+eax*2+32]
	movdqu xmm3,[ecx+eax*2+48]
	pmulhw xmm0,[edi+176]
	pmulhw xmm3,[edi+192]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm3

	mov eax,dword [edi+12]
	movdqu xmm0,[ecx+eax*2]
	movdqu xmm3,[ecx+eax*2+16]
	pmulhw xmm0,[edi+208]
	pmulhw xmm3,[edi+224]
	paddsw xmm4,xmm0
	paddsw xmm4,xmm3
	movdqu xmm0,[ecx+eax*2+32]
	movdqu xmm3,[ecx+eax*2+48]
	pmulhw xmm0,[edi+240]
	pmulhw xmm3,[edi+256]
	paddsw xmm4,xmm0
	paddsw xmm4,xmm3
	lea edi,[edi+272]

	movdqa xmm0,xmm2
	punpcklqdq xmm0,xmm4
	punpckhqdq xmm2,xmm4
	paddsw xmm2,xmm0

	psraw xmm1,5
	psraw xmm2,5
	paddw xmm1,xmm5
	paddw xmm2,xmm5
	packsswb xmm1,xmm2
	paddb xmm1,xmm6
	movntdq [esi],xmm1
	lea esi,[esi+16]
	dec ebp
	jnz hf8lop5

	add ecx,dword [esp+48] ;sstep
	add esi,edx ;dAdd

	dec ebx
	jnz hf8lop4
	jmp hfexit

	align 16
hfstart2:
	shr dword [esp+36],1 ;tap
	shr dword [esp+28],2 ;width
	mov ecx,dword [esp+20] ;inPt
	mov esi,dword [esp+24] ;outPt
	align 16
hf2lop4:

	mov ebp,dword [esp+28] ;width
	
	mov ebx,dword [esp+40] ;index
	mov edi,dword [esp+44] ;weight
	align 16
hf2lop5:
	mov edx,dword [esp+36] ;tap
	pxor xmm1,xmm1
	pxor xmm2,xmm2
	ALIGN 16
hf2lop6:
	mov eax,dword [ebx]
	movq xmm0,[ecx+eax*2]
	mov eax,dword [ebx+4]
	movq xmm3,[ecx+eax*2]
	lea ebx,[ebx+8]
	punpcklqdq xmm0,xmm3
	pmulhw xmm0,[edi]
	lea edi,[edi+16]
	paddsw xmm1,xmm0
	dec edx
	jnz hf2lop6

	mov edx,dword [esp+36] ;tap
	ALIGN 16
hf2lop7:
	mov eax,dword [ebx]
	movq xmm0,[ecx+eax*2]
	mov eax,dword [ebx+4]
	movq xmm3,[ecx+eax*2]
	lea ebx,[ebx+8]
	punpcklqdq xmm0,xmm3
	pmulhw xmm0,[edi]
	lea edi,[edi+16]
	paddsw xmm2,xmm0
	dec edx
	jnz hf2lop7

	movdqa xmm0,xmm1
	punpcklqdq xmm0,xmm2
	punpckhqdq xmm1,xmm2
	paddsw xmm1,xmm0

	mov edx,dword [esp+36] ;tap
	pxor xmm2,xmm2
	pxor xmm4,xmm4
	ALIGN 16
hf2lop6b:
	mov eax,dword [ebx]
	movq xmm0,[ecx+eax*2]
	mov eax,dword [ebx+4]
	movq xmm3,[ecx+eax*2]
	lea ebx,[ebx+8]
	punpcklqdq xmm0,xmm3
	pmulhw xmm0,[edi]
	lea edi,[edi+16]
	paddsw xmm2,xmm0
	dec edx
	jnz hf2lop6b

	mov edx,dword [esp+36] ;tap
	ALIGN 16
hf2lop7b:
	mov eax,dword [ebx]
	movq xmm0,[ecx+eax*2]
	mov eax,dword [ebx+4]
	movq xmm3,[ecx+eax*2]
	lea ebx,[ebx+8]
	punpcklqdq xmm0,xmm3
	pmulhw xmm0,[edi]
	lea edi,[edi+16]
	paddsw xmm4,xmm0
	dec edx
	jnz hf2lop7b

	movdqa xmm0,xmm2
	punpcklqdq xmm0,xmm4
	punpckhqdq xmm2,xmm4
	paddsw xmm2,xmm0

	psraw xmm1,5
	psraw xmm2,5
	paddw xmm1,xmm5
	paddw xmm2,xmm5
	packsswb xmm1,xmm2
	paddb xmm1,xmm6
	movdqu [esi],xmm1
	lea esi,[esi+16]
	dec ebp
	jnz hf2lop5

	add ecx,dword [esp+48] ;sstep
	add esi,dword [esp+52] ;dstep

	dec dword [esp+32] ;currHeight
	jnz hf2lop4
	
	align 16
hfexit:
	pop edi
	pop esi
	pop ebx
	pop ebp
	ret

;void LanczosResizerH8_8_horizontal_filter8(UInt8 *inPt, UInt8 *outPt, IntOS width, IntOS height, IntOS tap, IntOS *index, Int64 *weight, IntOS sstep, IntOS dstep)
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
_LanczosResizerH8_8_horizontal_filter8:
	push ebp
	push ebx
	push esi
	push edi
	mov eax,0xff80ff80
	mov edx,0x80808080
	movd xmm5,eax
	movd xmm6,edx
	punpckldq xmm5,xmm5
	punpckldq xmm6,xmm6
	punpckldq xmm5,xmm5						;UInt8 toAdd[16] = {0x80, 0xff, 0x80, 0xff, 0x80, 0xff, 0x80, 0xff, 0x80, 0xff, 0x80, 0xff, 0x80, 0xff, 0x80, 0xff};
	punpckldq xmm6,xmm6						;UInt8 toAdd2[16] = {0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80};
	mov eax,dword [esp+28] ;width
	lea edx,[eax*4]
	sub dword [esp+52],edx ;dstep			dstep = dstep - width * 4;
	test eax,3
	jnz hf8_start
	mov edx,dword [esp+36] ;tap
	test edx,1
	jnz hf8_start
	cmp edx,6
	jz hf8_6start
	cmp edx,8
	jz hf8_8start
	jmp hf8_start2

	align 16
hf8_start:									;if (width & 3 || tap & 1)
	pxor xmm3,xmm3
	mov ecx,dword [esp+20] ;inPt
	mov esi,dword [esp+24] ;outPt
	align 16
hf8_lop:

	mov ebp,dword [esp+28] ;width
	mov ebx,dword [esp+40] ;index
	mov edi,dword [esp+44] ;weight
	align 16
hf8_lop2:
	mov edx,dword [esp+36] ;tap
	pxor xmm1,xmm1
	align 16
hf8_lop3:
	mov eax,dword [ebx]
	movd xmm0,[ecx+eax]
	punpcklbw xmm0,xmm3
	psllw xmm0,6
	movq xmm4,[edi]
	pmulhw xmm0,xmm4
	paddsw xmm1,xmm0
	lea edi,[edi+8]
	lea ebx,[ebx+4]
	dec edx
	jnz hf8_lop3

	psraw xmm1,5
	paddw xmm1,xmm5
	packsswb xmm1,xmm3
	paddb xmm1,xmm6

	movd dword [esi],xmm1
	lea esi,[esi+4]
	dec ebp
	jnz hf8_lop2

	add ecx,dword [esp+48] ;sstep
	add esi,dword [esp+52] ;dstep

	dec dword [esp+32] ;currHeight
	jnz hf8_lop
	jmp hf8exit

	align 16
hf8_6start:									;else if (tap == 6)
	pxor xmm7, xmm7
	shr dword [esp+28],2
	mov ecx,dword [esp+20] ;inPt
	mov esi,dword [esp+24] ;outPt
	mov edx,dword [esp+32] ;height
	align 16
hf8_6lop4:
	mov ebp,dword [esp+28] ;width
	mov ebx,dword [esp+40] ;index
	mov edi,dword [esp+44] ;weight
	ALIGN 16
hf8_6lop5:
	pxor xmm1,xmm1
	pxor xmm2,xmm2
	mov eax,dword [ebx]
	movd xmm0,[ecx+eax]
	mov eax,dword [ebx+4]
	movd xmm3,[ecx+eax]
	punpckldq xmm0,xmm3
	punpcklbw xmm0,xmm7
	psllw xmm0,6
	pmulhw xmm0,[edi]
	paddsw xmm1,xmm0
	mov eax,dword [ebx+8]
	movd xmm0,[ecx+eax]
	mov eax,dword [ebx+12]
	movd xmm3,[ecx+eax]
	punpckldq xmm0,xmm3
	punpcklbw xmm0,xmm7
	psllw xmm0,6
	pmulhw xmm0,[edi+16]
	paddsw xmm1,xmm0
	mov eax,dword [ebx+16]
	movd xmm0,[ecx+eax]
	mov eax,dword [ebx+20]
	movd xmm3,[ecx+eax]
	punpckldq xmm0,xmm3
	punpcklbw xmm0,xmm7
	psllw xmm0,6
	pmulhw xmm0,[edi+32]
	paddsw xmm1,xmm0
	lea ebx,[ebx+24]
	lea edi,[edi+48]

	mov eax,dword [ebx]
	movd xmm0,[ecx+eax]
	mov eax,dword [ebx+4]
	movd xmm3,[ecx+eax]
	punpckldq xmm0,xmm3
	punpcklbw xmm0,xmm7
	psllw xmm0,6
	pmulhw xmm0,[edi]
	paddsw xmm2,xmm0
	mov eax,dword [ebx+8]
	movd xmm0,[ecx+eax]
	mov eax,dword [ebx+12]
	movd xmm3,[ecx+eax]
	punpckldq xmm0,xmm3
	punpcklbw xmm0,xmm7
	psllw xmm0,6
	pmulhw xmm0,[edi+16]
	paddsw xmm2,xmm0
	mov eax,dword [ebx+16]
	movd xmm0,[ecx+eax]
	mov eax,dword [ebx+20]
	movd xmm3,[ecx+eax]
	punpckldq xmm0,xmm3
	punpcklbw xmm0,xmm7
	psllw xmm0,6
	pmulhw xmm0,[edi+32]
	paddsw xmm2,xmm0
	lea ebx,[ebx+24]
	lea edi,[edi+48]

	movdqa xmm0,xmm1
	punpcklqdq xmm0,xmm2
	punpckhqdq xmm1,xmm2
	paddsw xmm1,xmm0

	pxor xmm2,xmm2
	pxor xmm4,xmm4
	mov eax,dword [ebx]
	movd xmm0,[ecx+eax]
	mov eax,dword [ebx+4]
	movd xmm3,[ecx+eax]
	punpckldq xmm0,xmm3
	punpcklbw xmm0,xmm7
	psllw xmm0,6
	pmulhw xmm0,[edi]
	paddsw xmm2,xmm0
	mov eax,dword [ebx+8]
	movd xmm0,[ecx+eax]
	mov eax,dword [ebx+12]
	movd xmm3,[ecx+eax]
	punpckldq xmm0,xmm3
	punpcklbw xmm0,xmm7
	psllw xmm0,6
	pmulhw xmm0,[edi+16]
	paddsw xmm2,xmm0
	mov eax,dword [ebx+16]
	movd xmm0,[ecx+eax]
	mov eax,dword [ebx+20]
	movd xmm3,[ecx+eax]
	punpckldq xmm0,xmm3
	punpcklbw xmm0,xmm7
	psllw xmm0,6
	pmulhw xmm0,[edi+32]
	paddsw xmm2,xmm0
	lea ebx,[ebx+24]
	lea edi,[edi+48]

	mov eax,dword [ebx]
	movd xmm0,[ecx+eax]
	mov eax,dword [ebx+4]
	movd xmm3,[ecx+eax]
	punpckldq xmm0,xmm3
	punpcklbw xmm0,xmm7
	psllw xmm0,6
	pmulhw xmm0,[edi]
	paddsw xmm4,xmm0
	mov eax,dword [ebx+8]
	movd xmm0,[ecx+eax]
	mov eax,dword [ebx+12]
	movd xmm3,[ecx+eax]
	punpckldq xmm0,xmm3
	punpcklbw xmm0,xmm7
	psllw xmm0,6
	pmulhw xmm0, [edi+16]
	paddsw xmm4,xmm0
	mov eax,dword [ebx+16]
	movd xmm0,[ecx+eax]
	mov eax,dword [ebx+20]
	movd xmm3,[ecx+eax]
	punpckldq xmm0,xmm3
	punpcklbw xmm0,xmm7
	psllw xmm0,6
	pmulhw xmm0,[edi+32]
	paddsw xmm4,xmm0
	lea ebx,[ebx+24]
	lea edi,[edi+48]

	movdqa xmm0,xmm2
	punpcklqdq xmm0,xmm4
	punpckhqdq xmm2,xmm4
	paddsw xmm2,xmm0

	psraw xmm1,5
	psraw xmm2,5
	paddw xmm1,xmm5
	paddw xmm2,xmm5
	packsswb xmm1,xmm2
	paddb xmm1,xmm6
	movdqu [esi],xmm1
	lea esi,[esi+16]
	dec ebp
	jnz hf8_6lop5

	add ecx,dword [esp+48] ;sstep
	add esi,dword [esp+52] ;dstep

	dec edx
	jnz hf8_6lop4
	jmp hf8exit

	align 16
hf8_8start:										;else if (tap == 8)
	shr dword [esp+28],2 ;width
	pxor xmm7, xmm7
	mov ecx,dword [esp+20] ;inPt
	mov esi,dword [esp+24] ;outPt
	mov edx,dword [esp+32] ;height
	mov ebx,dword [esp+52] ;dstep
	align 16
hf8_8lop4:
	mov ebp,dword [esp+28] ;width
	mov edi,dword [esp+44] ;weight
	ALIGN 16
hf8_8lop5:

	pxor xmm1,xmm1
	pxor xmm2,xmm2
	mov eax,dword [edi]
	movdqu xmm0,[ecx+eax]
	movdqa xmm3,xmm0
	punpcklbw xmm0,xmm0
	punpckhbw xmm3,xmm3
	psrlw xmm0,2
	psrlw xmm3,2
	pmulhw xmm0,[edi+16]
	pmulhw xmm3,[edi+32]
	paddsw xmm1,xmm0
	paddsw xmm1,xmm3
	movdqu xmm0,[ecx+eax+16]
	movdqa xmm3,xmm0
	punpcklbw xmm0,xmm0
	punpckhbw xmm3,xmm3
	psrlw xmm0,2
	psrlw xmm3,2
	pmulhw xmm0,[edi+48]
	pmulhw xmm3,[edi+64]
	paddsw xmm1,xmm0
	paddsw xmm1,xmm3

	mov eax,dword [edi+4]
	movdqu xmm0,[ecx+eax]
	movdqa xmm3,xmm0
	punpcklbw xmm0,xmm0
	punpckhbw xmm3,xmm3
	psrlw xmm0,2
	psrlw xmm3,2
	pmulhw xmm0,[edi+16]
	pmulhw xmm3,[edi+32]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm3
	movdqu xmm0,[ecx+eax+16]
	movdqa xmm3,xmm0
	punpcklbw xmm0,xmm0
	punpckhbw xmm3,xmm3
	psrlw xmm0,2
	psrlw xmm3,2
	pmulhw xmm0,[edi+48]
	pmulhw xmm3,[edi+64]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm3

	movdqa xmm0,xmm1
	punpcklqdq xmm0,xmm2
	punpckhqdq xmm1,xmm2
	paddsw xmm1,xmm0

	pxor xmm2,xmm2
	pxor xmm4,xmm4
	mov eax,dword [edi+8]
	movdqu xmm0,[ecx+eax]
	movdqa xmm3,xmm0
	punpcklbw xmm0,xmm0
	punpckhbw xmm3,xmm3
	psrlw xmm0,2
	psrlw xmm3,2
	pmulhw xmm0,[edi+16]
	pmulhw xmm3,[edi+32]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm3
	movdqu xmm0,[ecx+eax+16]
	movdqa xmm3,xmm0
	punpcklbw xmm0,xmm0
	punpckhbw xmm3,xmm3
	psrlw xmm0,2
	psrlw xmm3,2
	pmulhw xmm0,[edi+48]
	pmulhw xmm3,[edi+64]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm3

	mov eax,dword [edi+12]
	movdqu xmm0,[ecx+eax]
	movdqa xmm3,xmm0
	punpcklbw xmm0,xmm0
	punpckhbw xmm3,xmm3
	psrlw xmm0,2
	psrlw xmm3,2
	pmulhw xmm0,[edi+16]
	pmulhw xmm3,[edi+32]
	paddsw xmm4,xmm0
	paddsw xmm4,xmm3
	movdqu xmm0,[ecx+eax+16]
	movdqa xmm3,xmm0
	punpcklbw xmm0,xmm0
	punpckhbw xmm3,xmm3
	psrlw xmm0,2
	psrlw xmm3,2
	pmulhw xmm0,[edi+48]
	pmulhw xmm3,[edi+64]
	paddsw xmm4,xmm0
	paddsw xmm4,xmm3

	movdqa xmm0,xmm2
	punpcklqdq xmm0,xmm4
	punpckhqdq xmm2,xmm4
	paddsw xmm2,xmm0

	psraw xmm1,5
	psraw xmm2,5
	paddw xmm1,xmm5
	paddw xmm2,xmm5
	packsswb xmm1,xmm2
	paddb xmm1,xmm6
	movdqu [esi],xmm1
	lea edi,[edi+272]
	lea esi,[esi+16]
	dec ebp
	jnz hf8_8lop5

	add ecx,dword [esp+48] ;sstep
	add esi,ebx ;dstep

	dec edx
	jnz hf8_8lop4
	jmp hf8exit

	align 16
hf8_start2:
	shr dword [esp+36],1 ;tap
	shr dword [esp+28],2 ;width
	pxor xmm7, xmm7
	mov ecx,dword [esp+20] ;inPt
	mov esi,dword [esp+24] ;outPt
	align 16
hf8_lop4:
	mov ebp,dword [esp+28] ;width
	mov ebx,dword [esp+40] ;index
	mov edi,dword [esp+44] ;weight
	align 16
hf8_lop5:
	mov edx,dword [esp+36] ;tap
	pxor xmm1,xmm1
	pxor xmm2,xmm2
	ALIGN 16
hf8_lop6:
	mov eax,dword [ebx]
	movd xmm0,[ecx+eax]
	mov eax,dword [ebx+4]
	movd xmm3,[ecx+eax]
	add ebx,8
	punpckldq xmm0,xmm3
	punpcklbw xmm0,xmm7
	psllw xmm0,6
	pmulhw xmm0,[edi]
	add edi,16
	paddsw xmm1,xmm0
	dec edx
	jnz hf8_lop6

	mov edx,dword [esp+36] ;tap
	ALIGN 16
hf8_lop7:
	mov eax,dword [ebx]
	movd xmm0,[ecx+eax]
	mov eax,dword [ebx+4]
	movd xmm3,[ecx+eax]
	add ebx,8
	punpckldq xmm0,xmm3
	punpcklbw xmm0,xmm7
	psllw xmm0,6
	pmulhw xmm0,[edi]
	add edi,16
	paddsw xmm2,xmm0
	dec edx
	jnz hf8_lop7

	movdqa xmm0,xmm1
	punpcklqdq xmm0,xmm2
	punpckhqdq xmm1,xmm2
	paddsw xmm1,xmm0

	mov edx,dword [esp+36] ;tap
	pxor xmm2,xmm2
	pxor xmm4,xmm4
	ALIGN 16
hf8_lop6b:
	mov eax,dword [ebx]
	movd xmm0,[ecx+eax]
	mov eax,dword [ebx+4]
	movd xmm3,[ecx+eax]
	add ebx,8
	punpckldq xmm0,xmm3
	punpcklbw xmm0,xmm7
	psllw xmm0,6
	pmulhw xmm0,[edi]
	add edi,16
	paddsw xmm2,xmm0
	dec edx
	jnz hf8_lop6b

	mov edx,dword [esp+36] ;tap
	ALIGN 16
hf8_lop7b:
	mov eax,dword [ebx]
	movd xmm0,[ecx+eax]
	mov eax,dword [ebx+4]
	movd xmm3,[ecx+eax]
	add ebx,8
	punpckldq xmm0,xmm3
	punpcklbw xmm0,xmm7
	psllw xmm0,6
	pmulhw xmm0,[edi]
	add edi,16
	paddsw xmm4,xmm0
	dec edx
	jnz hf8_lop7b

	movdqa xmm0,xmm2
	punpcklqdq xmm0,xmm4
	punpckhqdq xmm2,xmm4
	paddsw xmm2,xmm0

	psraw xmm1,5
	psraw xmm2,5
	paddw xmm1,xmm5
	paddw xmm2,xmm5
	packsswb xmm1,xmm2
	paddb xmm1,xmm6
	movdqu [esi],xmm1
	add esi,16
	dec ebp
	jnz hf8_lop5

	add ecx,dword [esp+48] ;sstep
	add esi,dword [esp+52] ;dstep

	dec dword [esp+32] ;currHeight
	jnz hf8_lop4
	align 16
hf8exit:
	pop edi
	pop esi
	pop ebx
	pop ebp
	ret

;void LanczosResizerH8_8_vertical_filter(UInt8 *inPt, UInt8 *outPt, IntOS width, IntOS height, IntOS tap, IntOS *index, Int64 *weight, IntOS sstep, IntOS dstep)
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

	align 16
_LanczosResizerH8_8_vertical_filter:
	push ebp
	push ebx
	push esi
	push edi
	push eax
	mov eax,dword [esp+32] ;width
	lea edx,[eax*8]
	mov esi,dword [esp+28] ;outPt
	sub dword [esp+56],edx ;dstep
	test eax,3
	jnz vfstart
	test esi,15
	jnz vfstart
	test dword [esp+56],15 ;dstep
	jnz vfstart
	mov eax,dword [esp+40] ;tap
	cmp eax,6
	jz vf6start
	jmp vfstart2
	
	align 16
vfstart:							;if ((width & 3) != 0 || (((IntOS)outPt) & 15) != 0 || (dstep & 15) != 0)
vflop:
	mov eax,dword [esp+32] ;width
	mov ecx,dword [esp+24] ;inPt

	mov dword [esp+0],eax ;currWidth
	test esi,15
	jz vflop2_1

	mov edx,1
	dec eax
	mov dword [esp+0],eax ;currWidth

	align 16
vflop2_0:
	mov ebx,dword [esp+44] ;index
	mov edi,dword [esp+48] ;weight

	mov ebp,dword [esp+40] ;tap
	pxor xmm1,xmm1
vflop3_0:
	mov eax,dword [ebx]
	movd xmm0,dword [ecx+eax]
	lea ebx,[ebx+4]
	punpcklbw xmm0,xmm0
	psrlw xmm0,1
	movq xmm2,[edi]
	pmulhw xmm0,xmm2
	paddsw xmm1,xmm0
	lea edi,[edi+8]
	dec ebp
	jnz vflop3_0

	movq [esi],xmm1
	lea ecx,[ecx+4]
	lea esi,[esi+8]
	dec edx
	jnz vflop2_0

	align 16
vflop2_1:
	mov edx,dword [esp+0] ;currWidth
	shr edx,2
	jz vflop4
	align 16
vflop2:
	mov ebx,dword [esp+44] ;index
	mov edi,dword [esp+48] ;weight

	mov ebp,dword [esp+40] ;tap
	pxor xmm3,xmm3
	pxor xmm4,xmm4
	ALIGN 16
vflop3:
	mov eax,dword [ebx]
	movdqu xmm0,[ecx+eax]
	lea ebx,[ebx+4]
	movdqa xmm1,xmm0
	movq xmm2,[edi]
	punpcklbw xmm0,xmm0
	punpckhbw xmm1,xmm1
	punpcklqdq xmm2,xmm2
	psrlw xmm0,1
	psrlw xmm1,1
	pmulhw xmm0, xmm2
	pmulhw xmm1, xmm2
	paddsw xmm3,xmm0
	paddsw xmm4,xmm1
	lea edi,[edi+8]
	dec ebp
	jnz vflop3

	movntdq [esi],xmm3 ;movdqu?
	movntdq [esi+16],xmm4 ;movdqu?
	lea esi,[esi+32]
	lea ecx,[ecx+16]
	dec edx
	jnz vflop2
	
	align 16
vflop4:
	mov edx,3
	and edx,dword [esp+0] ;currWidth
	jz vflop5
	align 16
vflop2_2:
	mov ebx,dword [esp+44] ;index
	mov edi,dword [esp+48] ;weight

	mov ebp,dword [esp+40] ;tap
	pxor xmm1,xmm1
	align 16
vflop3_2:
	mov eax,dword [ebx]
	movd xmm0,dword [ecx+eax]
	lea ebx,[ebx+4]
	punpcklbw xmm0,xmm0
	psrlw xmm0,1
	movq xmm2,[edi]
	pmulhw xmm0,xmm2
	paddsw xmm1,xmm0
	lea edi,[edi+8]
	dec ebp
	jnz vflop3_2

	movq [esi],xmm1
	lea esi,[esi+8]
	lea ecx,[ecx+4]
	dec edx
	jnz vflop2_2
vflop5:
	mov eax,dword [esp+40] ;tap
	lea edx,[eax*8]
	shl eax,2
	add dword [esp+44],eax ;index
	add dword [esp+48],edx ;weight

	add esi,dword [esp+56] ;dstep

	dec dword [esp+36] ;currHeight
	jnz vflop
	jmp vfexit

	align 16
vf6start:								;else if (tap == 6)
	shr dword [esp+32],2 ;width
	mov ebp,dword [esp+36] ;height
	mov ebx,dword [esp+44] ;index
	mov edi,dword [esp+48] ;weight

	align 16
vf6lop1b:
	mov ecx,dword [esp+24] ;inPt
	mov edx,dword [esp+32] ;width

	movdqa xmm5,[edi]
	movdqa xmm6,[edi+16]
	movdqa xmm7,[edi+32]
	ALIGN 16
vf6lop2b:
	pxor xmm3,xmm3
	pxor xmm4,xmm4

	mov eax,dword [ebx]
	movdqa xmm2,xmm5
	movdqu xmm0,[ecx+eax]
	movdqa xmm1,xmm0
	punpcklbw xmm0,xmm0
	punpckhbw xmm1,xmm1
	punpcklqdq xmm2,xmm2
	psrlw xmm0,1
	psrlw xmm1,1
	pmulhw xmm0,xmm2
	pmulhw xmm1,xmm2
	paddsw xmm3,xmm0
	paddsw xmm4,xmm1
	mov eax,dword [ebx+4]
	movdqa xmm2,xmm5
	movdqu xmm0,[ecx+eax]
	movdqa xmm1,xmm0
	punpcklbw xmm0,xmm0
	punpckhbw xmm1,xmm1
	punpckhqdq xmm2,xmm2
	psrlw xmm0,1
	psrlw xmm1,1
	pmulhw xmm0,xmm2
	pmulhw xmm1,xmm2
	paddsw xmm3,xmm0
	paddsw xmm4,xmm1

	mov eax,dword [ebx+8]
	movdqa xmm2,xmm6
	movdqu xmm0,[ecx+eax]
	movdqa xmm1,xmm0
	punpcklbw xmm0,xmm0
	punpckhbw xmm1,xmm1
	punpcklqdq xmm2,xmm2
	psrlw xmm0,1
	psrlw xmm1,1
	pmulhw xmm0,xmm2
	pmulhw xmm1,xmm2
	paddsw xmm3,xmm0
	paddsw xmm4,xmm1
	mov eax,dword [ebx+12]
	movdqa xmm2,xmm6
	movdqu xmm0,[ecx+eax]
	movdqa xmm1,xmm0
	punpcklbw xmm0,xmm0
	punpckhbw xmm1,xmm1
	punpckhqdq xmm2,xmm2
	psrlw xmm0,1
	psrlw xmm1,1
	pmulhw xmm0,xmm2
	pmulhw xmm1,xmm2
	paddsw xmm3,xmm0
	paddsw xmm4,xmm1

	mov eax,dword [ebx+16]
	movdqa xmm2,xmm7
	movdqu xmm0,[ecx+eax]
	movdqa xmm1,xmm0
	punpcklbw xmm0,xmm0
	punpckhbw xmm1,xmm1
	punpcklqdq xmm2,xmm2
	psrlw xmm0,1
	psrlw xmm1,1
	pmulhw xmm0,xmm2
	pmulhw xmm1,xmm2
	paddsw xmm3,xmm0
	paddsw xmm4,xmm1
	mov eax,dword [ebx+20]
	movdqa xmm2,xmm7
	movdqu xmm0,[ecx+eax]
	movdqa xmm1,xmm0
	punpcklbw xmm0,xmm0
	punpckhbw xmm1,xmm1
	punpckhqdq xmm2,xmm2
	psrlw xmm0,1
	psrlw xmm1,1
	pmulhw xmm0,xmm2
	pmulhw xmm1,xmm2
	paddsw xmm3,xmm0
	paddsw xmm4,xmm1

	movntdq [esi],xmm3
	movntdq [esi+16],xmm4
	lea esi,[esi+32]
	lea ecx,[ecx+16]
	dec edx
	jnz vf6lop2b

	lea ebx,[ebx+24]
	lea edi,[edi+48]
	add esi,dword [esp+56] ;dAdd

	dec ebp
	jnz vf6lop1b
	jmp vfexit

	align 16
vfstart2:
	shr dword [esp+32],2 ;width
	align 16
vflop1b:
	mov ecx,dword [esp+24] ;inPt
	mov ebp,dword [esp+32] ;width
	align 16
vflop2b:
	mov ebx,dword [esp+44] ;index
	mov edi,dword [esp+48] ;weight

	mov edx,dword [esp+40] ;tap
	pxor xmm3,xmm3
	pxor xmm4,xmm4
	ALIGN 16
vflop3b:
	mov eax,dword [ebx]
	movdqu xmm0,[ecx+eax]
	movdqa xmm1,xmm0
	movq xmm2,[edi]
	punpcklbw xmm0,xmm0
	punpckhbw xmm1,xmm1
	punpcklqdq xmm2,xmm2
	psrlw xmm0,1
	psrlw xmm1,1
	pmulhw xmm0, xmm2
	pmulhw xmm1, xmm2
	paddsw xmm3,xmm0
	paddsw xmm4,xmm1
	lea ebx,[ebx+4]
	lea edi,[edi+8]
	dec edx
	jnz vflop3b

	movntdq [esi],xmm3
	movntdq [esi+16],xmm4
	lea esi,[esi+32]
	lea ecx,[ecx+16]
	dec ebp
	jnz vflop2b

	mov eax,dword [esp+40] ;tap
	lea edx,[eax*8]
	shl eax,2
	add dword [esp+44],eax ;index
	add dword [esp+48],edx ;weight

	add esi,dword [esp+56] ;dstep

	dec dword [esp+36] ;currHeight
	jnz vflop1b
	
	align 16
vfexit:
	pop eax
	pop edi
	pop esi
	pop ebx
	pop ebp
	ret

;void LanczosResizerH8_8_expand(UInt8 *inPt, UInt8 *outPt, IntOS width, IntOS height, IntOS sstep, IntOS dstep)
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

	align 16
_LanczosResizerH8_8_expand:
	push ebp
	push ebx
	push esi
	push edi
	mov ebp,dword [esp+28] ;width
	lea edx,[ebp*4]
	lea ecx,[ebp*8]
	sub dword [esp+36],edx ;sAdd				IntOS sAdd = sstep - width * 4;
	sub dword [esp+40],ecx ;dADd				IntOS dAdd = dstep - width * 8;
	
	mov edx,dword [esp+32] ;height
	mov esi,dword [esp+24] ;outPt
	mov ecx,dword [esp+20] ;inPt
	mov eax,dword [esp+36] ;sAdd
	mov ebx,dword [esp+40] ;dAdd
	
	shr ebp,1
	jb explop
	test esi,15
	jnz exp2lop
	test ebx,15
	jnz exp2lop
	jmp expalop
	
	align 16
explop: 									;if (width & 1)
	mov edi,ebp
	align 16
explop2:
	movq xmm0,[ecx]
	punpcklbw xmm0,xmm0
	psrlw xmm0,2
	movdqu [esi],xmm0
	lea ecx,[ecx+8]
	lea esi,[esi+16]
	dec edi
	jnz explop2

	movd xmm0,dword [ecx]
	punpcklbw xmm0,xmm0
	psrlw xmm0,2
	movq [esi],xmm0
	lea ecx,[ecx+eax+4]
	lea esi,[esi+ebx+8]

	dec edx
	jnz explop
	jmp expexit

	align 16
exp2lop:
	mov edi,ebp
	align 16
exp2lop2:
	movq xmm0,[ecx]
	punpcklbw xmm0,xmm0
	psrlw xmm0,2
	movdqu [esi],xmm0
	lea ecx,[ecx+8]
	lea esi,[esi+16]
	dec edi
	jnz exp2lop2

	lea ecx,[ecx+eax]
	lea esi,[esi+ebx]

	dec edx
	jnz exp2lop
	jmp expexit

	align 16
expalop:
	mov edi,ebp
	align 16
expalop2:
	movq xmm0,[ecx]
	punpcklbw xmm0,xmm0
	psrlw xmm0,2
	movntdq [esi],xmm0
	lea ecx,[ecx+8]
	lea esi,[esi+16]
	dec edi
	jnz expalop2

	lea ecx,[ecx+eax]
	lea esi,[esi+ebx]

	dec edx
	jnz expalop
	
	align 16
expexit:
	pop edi
	pop esi
	pop ebx
	pop ebp
	ret

;void LanczosResizerH8_8_collapse(UInt8 *inPt, UInt8 *outPt, IntOS width, IntOS height, IntOS sstep, IntOS dstep)
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

	align 16
_LanczosResizerH8_8_collapse:
	push ebp
	push ebx
	push esi
	push edi
	mov eax,0xff80ff80
	mov edx,0x80808080
	mov ecx,dword [esp+28] ;width
	movd xmm2,eax
	movd xmm3,edx
	punpckldq xmm2,xmm2
	punpckldq xmm3,xmm3
	punpcklqdq xmm2,xmm2
	punpcklqdq xmm3,xmm3
	lea eax,[ecx*8]
	lea edx,[ecx*4]
	mov ebx,dword [esp+36] ;sAdd
	mov edi,dword [esp+40] ;dAdd
	sub ebx,eax						;IntOS sAdd = width * 8 - sstep;
	sub edi,edx						;IntOS dAdd = width * 4 - dstep;
	mov ebp,dword [esp+32] ;height
	mov edx,ecx ;width
	pxor xmm1,xmm1
	mov esi,dword [esp+24] ;outPt
	mov ecx,dword [esp+20] ;inPt

	test edx,3
	jz collop
	shr edx,2
	test esi,15
	jnz col4lop
	test edi,15
	jnz col4lop
	jmp colalop
	
	align 16
collop:
	mov eax,edx

	align 16
collop2:
	movq xmm0,[ecx]
	psraw xmm0,6
	paddw xmm0,xmm2
	packsswb xmm0,xmm1
	paddb xmm0,xmm3
	movd dword [esi],xmm0
	lea esi,[esi+4]
	lea ecx,[ecx+8]
	dec eax
	jnz collop2

	add ecx,ebx
	add esi,edi

	dec ebp
	jnz collop
	jmp colexit

	align 16
col4lop:
	mov eax,edx

	align 16
col4lop2:
	movdqu xmm0,[ecx]
	movdqu xmm4,[ecx+16]
	psraw xmm0,6
	psraw xmm4,6
	paddw xmm0,xmm2
	paddw xmm4,xmm2
	packsswb xmm0,xmm4
	paddb xmm0,xmm3
	movdqu [esi],xmm0
	lea ecx,[ecx+32]
	lea esi,[esi+16]
	dec eax
	jnz col4lop2

	add ecx,ebx
	add esi,edi

	dec ebp
	jnz col4lop
	jmp colexit

	align 16
colalop:
	mov eax,edx

	align 16
colalop2:
	movdqu xmm0,[ecx]
	movdqu xmm4,[ecx+16]
	psraw xmm0,6
	psraw xmm4,6
	paddw xmm0,xmm2
	paddw xmm4,xmm2
	packsswb xmm0,xmm4
	paddb xmm0,xmm3
	movntdq [esi],xmm0
	lea ecx,[ecx+32]
	lea esi,[esi+16]
	dec eax
	jnz colalop2

	add ecx,ebx
	add esi,edi

	dec ebp
	jnz colalop
	
	align 16
colexit:
	pop edi
	pop esi
	pop ebx
	pop ebp
	ret

