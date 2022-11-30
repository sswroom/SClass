section .text

global _LanczosResizer8_C8_horizontal_filter_pa
global LanczosResizer8_C8_horizontal_filter_pa
global _LanczosResizer8_C8_horizontal_filter
global LanczosResizer8_C8_horizontal_filter
global _LanczosResizer8_C8_vertical_filter
global LanczosResizer8_C8_vertical_filter
global _LanczosResizer8_C8_expand
global LanczosResizer8_C8_expand
global _LanczosResizer8_C8_expand_pa
global LanczosResizer8_C8_expand_pa
global _LanczosResizer8_C8_collapse
global LanczosResizer8_C8_collapse
global _LanczosResizer8_C8_imgcopy
global LanczosResizer8_C8_imgcopy
global _LanczosResizer8_C8_imgcopy_pa
global LanczosResizer8_C8_imgcopy_pa

;void LanczosResizer8_C8_horizontal_filter_pa(UInt8 *inPt, UInt8 *outPt, OSInt dwidth, OSInt height, OSInt tap, OSInt *index, Int64 *weight, OSInt sstep, OSInt dstep, UInt8 *rgbTable, OSInt swidth, UInt8 *tmpbuff)
;0 edi
;4 esi
;8 ebx
;12 ebp
;16 retAddr
;20 inpt
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
_LanczosResizer8_C8_horizontal_filter_pa:
LanczosResizer8_C8_horizontal_filter_pa:
	push ebp
	push ebx
	push esi
	push edi
	test dword [esp+28],1
	jnz hfpastart
	cmp dword [esp+36],6
	jz hfpa6start
	jmp hfpa2start

	align 16
hfpastart:							;if (dwidth & 1)
	shr dword [esp+36],1 ;tap
	pxor xmm3,xmm3
	align 16
hfpalop:
	mov ecx,dword [esp+20] ;inPt
	mov edi,dword [esp+64] ;tmpbuff
	mov esi,dword [esp+56] ;rgbTable

	mov ebp,dword [esp+60] ;swidth
	shr ebp,1
	jnb hfpalop4

	mov eax,dword [ecx]
	movzx edx,al
	movq xmm1, [esi+edx*8+266240]
	movzx edx,ah
	movq xmm0, [esi+edx*8+264192]
	shr eax,16
	paddsw xmm1,xmm0
	movzx edx,al
	movq xmm0, [esi+edx*8+262144]
	movzx edx,ah
	paddsw xmm1,xmm0

	movd xmm0,edx
	punpcklbw xmm0, xmm0
	punpcklbw xmm0, xmm0
	punpcklbw xmm0, xmm0
	psrlw xmm0,1
	pmulhw xmm0, xmm1
	psllw xmm0,1
	movq xmm1, [esi+edx*8+268288]
	paddsw xmm0,xmm1
	movq [edi],xmm0

	lea edi,[edi+8]
	lea ecx,[ecx+4]

	align 16
hfpalop4:
	mov eax,dword [ecx]
	mov ebx,dword [ecx+4]
	movzx edx,al
	movq xmm1, [esi+edx*8+266240]
	movzx edx,bl
	movq xmm2, [esi+edx*8+266240]
	movzx edx,ah
	punpcklqdq xmm1,xmm2
	movq xmm0, [esi+edx*8+264192]
	movzx edx,bh
	movq xmm2, [esi+edx*8+264192]
	shr eax,16
	shr ebx,16
	punpcklqdq xmm0,xmm2
	paddsw xmm1,xmm0
	movzx edx,al
	movq xmm0, [esi+edx*8+262144]
	movzx edx,bl
	movq xmm2, [esi+edx*8+262144]
	movzx edx,ah
	mov dh,bh
	punpcklqdq xmm0,xmm2
	paddsw xmm1,xmm0

	movd xmm0,edx
	punpcklbw xmm0, xmm0
	punpcklbw xmm0, xmm0
	punpcklbw xmm0, xmm0
	psrlw xmm0,1
	pmulhw xmm0, xmm1
	psllw xmm0,1
	movzx edx,ah
	movq xmm1, [esi+edx*8+268288]
	movzx edx,bh
	movq xmm2, [esi+edx*8+268288]
	punpcklqdq xmm1,xmm2
	paddsw xmm0,xmm1
	movdqu [edi],xmm0

	lea edi,[edi+16]
	lea ecx,[ecx+8]
	dec ebp
	jnz hfpalop4

	mov ecx,dword [esp+64] ;tmpbuff
	mov esi,dword [esp+24] ;outPt

	mov ebp,dword [esp+28] ;dwidth

	mov edi,dword [esp+44] ;weight
	mov ebx,dword [esp+40] ;index
	align 16
hfpalop2:
	mov edx,dword [esp+36] ;tap
	pxor xmm2,xmm2
	ALIGN 16
hfpalop3:
	mov eax,dword [ebx]
	movq xmm0,[ecx+eax]
	mov eax,dword [ebx+4]
	movq xmm1,[ecx+eax]
	punpcklwd xmm0,xmm1
	pmaddwd xmm0,[edi]
	paddd xmm2,xmm0
	
	lea edi,[edi+16]
	lea ebx,[ebx+8]
	dec edx
	jnz hfpalop3

	psrad xmm2,14
	packssdw xmm2,xmm3
	movq [esi],xmm2
	lea esi,[esi+8]
	dec ebp
	jnz hfpalop2

	mov eax,dword [esp+48] ;sstep
	mov edx,dword [esp+52] ;dstep
	add dword [esp+20],eax ;inPt
	add dword [esp+24],edx ;outPt

	dec dword [esp+32] ;currHeight
	jnz hfpalop
	jmp hfpaexit

	align 16
hfpa6start:								;else if (tap == 6)
	shr dword [esp+28],1 ;dwidth
	shr dword [esp+36],1 ;tap
	align 16
hfpa6lop:
	mov ecx,dword [esp+20] ;inPt
	mov edi,dword [esp+64] ;tmpbuff
	mov esi,dword [esp+56] ;rgbTable

	mov ebp,dword [esp+60] ;swidth
	shr ebp,1
	jnb hfpa6lop4

	mov eax,dword [ecx]
	movzx edx,al
	movq xmm1,[esi+edx*8+266240]
	movzx edx,ah
	movq xmm0,[esi+edx*8+264192]
	shr eax,16
	paddsw xmm1,xmm0
	movzx edx,al
	movq xmm0,[esi+edx*8+262144]
	movzx edx,ah
	paddsw xmm1,xmm0

	movd xmm0,edx
	punpcklbw xmm0, xmm0
	punpcklbw xmm0, xmm0
	punpcklbw xmm0, xmm0
	psrlw xmm0,1
	pmulhw xmm0, xmm1
	psllw xmm0,1
	movq xmm1,[esi+edx*8+268288]
	paddsw xmm0,xmm1
	movq [edi],xmm0

	lea edi,[edi+8]
	lea ecx,[ecx+4]

	align 16
hfpa6lop4:
	mov eax,dword [ecx]
	mov ebx,dword [ecx+4]
	movzx edx,al
	movq xmm1,[esi+edx*8+266240]
	movzx edx,bl
	movq xmm2,[esi+edx*8+266240]
	movzx edx,ah
	punpcklqdq xmm1,xmm2
	movq xmm0,[esi+edx*8+264192]
	movzx edx,bh
	movq xmm2,[esi+edx*8+264192]
	shr eax,16
	shr ebx,16
	punpcklqdq xmm0,xmm2
	paddsw xmm1,xmm0
	movzx edx,al
	movq xmm0,[esi+edx*8+262144]
	movzx edx,bl
	movq xmm2,[esi+edx*8+262144]
	movzx edx,ah
	mov dh,bh
	punpcklqdq xmm0,xmm2
	paddsw xmm1,xmm0

	movd xmm0,edx
	punpcklbw xmm0, xmm0
	punpcklbw xmm0, xmm0
	punpcklbw xmm0, xmm0
	psrlw xmm0,1
	pmulhw xmm0, xmm1
	psllw xmm0,1
	movzx edx,ah
	movq xmm1,[esi+edx*8+268288]
	movzx edx,bh
	movq xmm2,[esi+edx*8+268288]
	punpcklqdq xmm1,xmm2
	paddsw xmm0,xmm1
	movdqu [edi],xmm0

	lea edi,[edi+16]
	lea ecx,[ecx+8]
	dec ebp
	jnz hfpa6lop4

	mov ecx,dword [esp+64] ;tmpbuff
	mov esi,dword [esp+24] ;outPt

	mov ebp,dword [esp+28] ;dwidth

	mov edi,dword [esp+44] ;weight
	mov ebx,dword [esp+40] ;index
	align 16
hfpa6lop2:
	mov eax,dword [ebx]
	pxor xmm1,xmm1
	mov edx,dword [ebx+4]
	movdqu xmm0,[ecx+eax]
	movdqa xmm4,xmm0
	movdqu xmm2,[ecx+edx]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[edi]
	punpckhwd xmm4,xmm2
	pmaddwd xmm4, [edi+16]
	paddd xmm1,xmm0
	mov eax,dword [ebx+8]
	paddd xmm1,xmm4
	movdqu xmm0,[ecx+eax]
	pshufd xmm2,xmm0,14
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[edi+32]
	paddd xmm1,xmm0

	mov eax,dword [ebx+12]
	pxor xmm3,xmm3
	mov edx,dword [ebx+16]
	movdqu xmm0,[ecx+eax]
	movdqa xmm4,xmm0
	movdqu xmm2,[ecx+edx]
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

	psrad xmm1,14
	psrad xmm3,14
	packssdw xmm1,xmm3
	movdqu [esi],xmm1
	lea esi,[esi+16]
	dec ebp
	jnz hfpa6lop2

	mov eax,dword [esp+48] ;sstep
	mov edx,dword [esp+52] ;dstep
	add dword [esp+20],eax ;inPt
	add dword [esp+24],edx ;outPt

	dec dword [esp+32] ;currHeight
	jnz hfpa6lop
	jmp hfpaexit

	align 16
hfpa2start:
	shr dword [esp+28],1 ;dwidth
	shr dword [esp+36],1 ;tap
	align 16
hfpa2lop:
	mov ecx,dword [esp+20] ;inPt
	mov edi,dword [esp+64] ;tmpbuff
	mov esi,dword [esp+56] ;rgbTable

	mov ebp,dword [esp+60] ;swidth
	shr ebp,1
	jnb hfpa2lop4

	mov eax,dword [ecx]
	movzx edx,al
	movq xmm1,[esi+edx*8+266240]
	movzx edx,ah
	movq xmm0,[esi+edx*8+264192]
	shr eax,16
	paddsw xmm1,xmm0
	movzx edx,al
	movq xmm0,[esi+edx*8+262144]
	movzx edx,ah
	paddsw xmm1,xmm0

	movd xmm0,edx
	punpcklbw xmm0, xmm0
	punpcklbw xmm0, xmm0
	punpcklbw xmm0, xmm0
	psrlw xmm0,1
	pmulhw xmm0, xmm1
	psllw xmm0,1
	movq xmm1,[esi+edx*8+268288]
	paddsw xmm0,xmm1
	movq [edi],xmm0

	lea edi,[edi+8]
	lea ecx,[ecx+4]

	align 16
hfpa2lop4:
	mov eax,dword [ecx]
	mov ebx,dword [ecx+4]
	movzx edx,al
	movq xmm1, [esi+edx*8+266240]
	movzx edx,bl
	movq xmm2, [esi+edx*8+266240]
	movzx edx,ah
	punpcklqdq xmm1,xmm2
	movq xmm0, [esi+edx*8+264192]
	movzx edx,bh
	movq xmm2, [esi+edx*8+264192]
	shr eax,16
	shr ebx,16
	punpcklqdq xmm0,xmm2
	paddsw xmm1,xmm0
	movzx edx,al
	movq xmm0, [esi+edx*8+262144]
	movzx edx,bl
	movq xmm2, [esi+edx*8+262144]
	movzx edx,ah
	mov dh,bh
	punpcklqdq xmm0,xmm2
	paddsw xmm1,xmm0

	movd xmm0,edx
	punpcklbw xmm0, xmm0
	punpcklbw xmm0, xmm0
	punpcklbw xmm0, xmm0
	psrlw xmm0,1
	pmulhw xmm0, xmm1
	psllw xmm0,1
	movzx edx,ah
	movq xmm1,[esi+edx*8+268288]
	movzx edx,bh
	movq xmm2,[esi+edx*8+268288]
	punpcklqdq xmm1,xmm2
	paddsw xmm0,xmm1
	movdqu [edi],xmm0

	lea edi,[edi+16]
	lea ecx,[ecx+8]
	dec ebp
	jnz hfpa2lop4

	mov ecx,dword [esp+64] ;tmpbuff
	mov esi,dword [esp+24] ;outPt

	mov ebp,dword [esp+28] ;dwidth

	mov ebx,dword [esp+40] ;index
	mov edi,dword [esp+44] ;weight
	align 16
hfpa2lop2:
	mov edx,dword [esp+36] ;tap
	pxor xmm2,xmm2
	pxor xmm3,xmm3
	ALIGN 16
hfpa2lop3:
	mov eax,dword [ebx]
	movq xmm0,[ecx+eax]
	mov eax,dword [ebx+4]
	movq xmm1,[ecx+eax]
	punpcklwd xmm0,xmm1
	pmaddwd xmm0, [edi]
	paddd xmm2,xmm0
	
	lea edi,[edi+16]
	lea ebx,[ebx+8]
	dec edx
	jnz hfpa2lop3

	mov edx,dword [esp+36] ;tap
	ALIGN 16
hfpa2lop3b:
	mov eax,dword [ebx]
	movq xmm0,[ecx+eax]
	mov eax,dword [ebx+4]
	movq xmm1,[ecx+eax]
	punpcklwd xmm0,xmm1
	pmaddwd xmm0,[edi]
	paddd xmm3,xmm0
	
	lea edi,[edi+16]
	lea ebx,[ebx+8]
	dec edx
	jnz hfpa2lop3b

	psrad xmm2,14
	psrad xmm3,14
	packssdw xmm2,xmm3
	movdqu [esi],xmm2
	lea esi,[esi+16]
	dec ebp
	jnz hfpa2lop2

	mov eax,dword [esp+48] ;sstep
	mov edx,dword [esp+52] ;dstep
	add dword [esp+20],eax ;inPt
	add dword [esp+24],edx ;outPt

	dec dword [esp+32] ;currHeight
	jnz hfpa2lop
	
	align 16
hfpaexit:
	pop edi
	pop esi
	pop ebx
	pop ebp
	ret

;void LanczosResizer8_C8_horizontal_filter(UInt8 *inPt, UInt8 *outPt, OSInt dwidth, OSInt height, OSInt tap, OSInt *index, Int64 *weight, OSInt sstep, OSInt dstep, UInt8 *rgbTable, OSInt swidth, UInt8 *tmpbuff)
;0 edi
;4 esi
;8 ebx
;12 ebp
;16 retAddr
;20 inpt
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
_LanczosResizer8_C8_horizontal_filter:
LanczosResizer8_C8_horizontal_filter:
	push ebp
	push ebx
	push esi
	push edi
	test dword [esp+28],1 ;dwidth
	jnz hfstart
	cmp dword [esp+36],6 ;tap
	jz hf6start
	jmp hf2start
	
	align 16
hfstart:					;if (dwidth & 1)
	shr dword [esp+36],1 ;tap
	pxor xmm3,xmm3
	align 16
hflop:
	mov ecx,dword [esp+20] ;inPt
	mov edi,dword [esp+64] ;tmpbuff
	mov esi,dword [esp+56] ;rgbTable

	mov ebp,dword [esp+60] ;swidth
	shr ebp,1
	jnb hflop4

	mov eax,dword [ecx]
	movzx edx,al
	movq xmm1,[esi+edx*8+266240]
	movzx edx,ah
	movq xmm0,[esi+edx*8+264192]
	shr eax,16
	paddsw xmm1,xmm0
	movzx edx,al
	movq xmm0,[esi+edx*8+262144]
	paddsw xmm1,xmm0
	movzx edx,ah
	movq xmm0,[esi+edx*8+268288]
	paddsw xmm1,xmm0
	movq [edi],xmm1
	lea edi,[edi+8]
	lea ecx,[ecx+4]

	align 16
hflop4:
	mov eax,dword [ecx]
	mov ebx,dword [ecx+4]
	movzx edx,al
	movq xmm1,[esi+edx*8+266240]
	movzx edx,bl
	movq xmm2,[esi+edx*8+266240]
	movzx edx,ah
	punpcklqdq xmm1,xmm2
	movq xmm0,[esi+edx*8+264192]
	movzx edx,bh
	movq xmm2,[esi+edx*8+264192]
	shr eax,16
	shr ebx,16
	punpcklqdq xmm0,xmm2
	paddsw xmm1,xmm0
	movzx edx,al
	movq xmm0,[esi+edx*8+262144]
	movzx edx,bl
	movq xmm2,[esi+edx*8+262144]
	punpcklqdq xmm0,xmm2
	paddsw xmm1,xmm0
	movzx edx,ah
	movq xmm0,[esi+edx*8+268288]
	movzx edx,bh
	movq xmm2,[esi+edx*8+268288]
	punpcklqdq xmm0,xmm2
	paddsw xmm1,xmm0
	movdqu [edi],xmm1

	lea edi,[edi+16]
	lea ecx,[ecx+8]
	dec ebp
	jnz hflop4

	mov ecx,dword [esp+64] ;tmpbuff
	mov esi,dword [esp+24] ;outPt

	mov ebp,dword [esp+28] ;dwidth

	mov edi,dword [esp+44] ;weight
	mov ebx,dword [esp+40] ;index
	align 16
hflop2:
	mov edx,dword [esp+36] ;tap
	pxor xmm2,xmm2
	align 16
hflop3:
	mov eax,dword [ebx]
	movq xmm0,[ecx+eax]
	mov eax,dword [ebx+4]
	movq xmm1,[ecx+eax]
	punpcklwd xmm0,xmm1
	pmaddwd xmm0,[edi]
	paddd xmm2,xmm0
	
	lea edi,[edi+16]
	lea ebx,[ebx+8]
	dec edx
	jnz hflop3

	psrad xmm2,14
	packssdw xmm2,xmm3
	movq [esi],xmm2
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
	shr dword [esp+28],1 ;dwidth
	shr dword [esp+36],1 ;tap
	pxor xmm3,xmm3
	align 16
hf6lop:
	mov ecx,dword [esp+20] ;inPt
	mov edi,dword [esp+64] ;tmpbuff
	mov esi,dword [esp+56] ;rgbTable

	mov ebp,dword [esp+60] ;swidth
	shr ebp,1
	jnb hf6lop4

	mov eax,dword [ecx]
	movzx edx,al
	movq xmm1,[esi+edx*8+266240]
	movzx edx,ah
	movq xmm0,[esi+edx*8+264192]
	shr eax,16
	paddsw xmm1,xmm0
	movzx edx,al
	movq xmm0,[esi+edx*8+262144]
	paddsw xmm1,xmm0
	movzx edx,ah
	movq xmm0,[esi+edx*8+268288]
	paddsw xmm1,xmm0
	movq [edi],xmm1
	lea edi,[edi+8]
	lea ecx,[ecx+4]

	align 16
hf6lop4:
	mov eax,dword [ecx]
	mov ebx,dword [ecx+4]
	movzx edx,al
	movq xmm1,[esi+edx*8+266240]
	movzx edx,bl
	movq xmm2,[esi+edx*8+266240]
	movzx edx,ah
	punpcklqdq xmm1,xmm2
	movq xmm0,[esi+edx*8+264192]
	movzx edx,bh
	movq xmm2,[esi+edx*8+264192]
	shr eax,16
	shr ebx,16
	punpcklqdq xmm0,xmm2
	paddsw xmm1,xmm0
	movzx edx,al
	movq xmm0,[esi+edx*8+262144]
	movzx edx,bl
	movq xmm2,[esi+edx*8+262144]
	punpcklqdq xmm0,xmm2
	paddsw xmm1,xmm0
	movzx edx,ah
	movq xmm0,[esi+edx*8+268288]
	movzx edx,bh
	movq xmm2,[esi+edx*8+268288]
	punpcklqdq xmm0,xmm2
	paddsw xmm1,xmm0
	movdqu [edi],xmm1

	lea edi,[edi+16]
	lea ecx,[ecx+8]
	dec ebp
	jnz hf6lop4

	mov ecx,dword [esp+64] ;tmpbuff
	mov esi,dword [esp+24] ;outPt

	mov ebp,dword [esp+28] ;dwidth

	mov edi,dword [esp+44] ;weight
	mov ebx,dword [esp+40] ;index
	align 16
hf6lop2:
	mov eax,dword [ebx]
	pxor xmm1,xmm1
	mov edx,dword [ebx+4]
	movdqu xmm0,[ecx+eax]
	movdqa xmm4,xmm0
	movdqu xmm2,[ecx+edx]
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
	pmaddwd xmm0, [edi+32]
	paddd xmm1,xmm0

	mov eax,dword [ebx+12]
	pxor xmm3,xmm3
	mov edx,dword [ebx+16]
	movdqu xmm0,[ecx+eax]
	movdqa xmm4,xmm0
	movdqu xmm2,[ecx+edx]
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

	psrad xmm1,14
	psrad xmm3,14
	packssdw xmm1,xmm3
	movdqu [esi],xmm1
	lea esi,[esi+16]
	dec ebp
	jnz hf6lop2

	mov eax,dword [esp+48] ;sstep
	mov edx,dword [esp+52] ;dstep
	add dword [esp+20],eax ;inPt
	add dword [esp+24],edx ;outPt

	dec dword [esp+32] ;currHeight
	jnz hf6lop
	jmp hfexit

	align 16
hf2start:
	shr dword [esp+28],1 ;dwidth
	shr dword [esp+36],1 ;tap
	pxor xmm3,xmm3
	align 16
hf2lop:
	mov ecx,dword [esp+20] ;inPt
	mov edi,dword [esp+64] ;tmpbuff
	mov esi,dword [esp+56] ;rgbTable

	mov ebp,dword [esp+60] ;swidth
	shr ebp,1
	jnb hf2lop4

	mov eax,dword [ecx]
	movzx edx,al
	movq xmm1,[esi+edx*8+266240]
	movzx edx,ah
	movq xmm0,[esi+edx*8+264192]
	shr eax,16
	paddsw xmm1,xmm0
	movzx edx,al
	movq xmm0,[esi+edx*8+262144]
	paddsw xmm1,xmm0
	movzx edx,ah
	movq xmm0,[esi+edx*8+268288]
	paddsw xmm1,xmm0
	movq [edi],xmm1
	lea edi,[edi+8]
	lea ecx,[ecx+4]

	align 16
hf2lop4:
	mov eax,dword [ecx]
	mov ebx,dword [ecx+4]
	movzx edx,al
	movq xmm1,[esi+edx*8+266240]
	movzx edx,bl
	movq xmm2,[esi+edx*8+266240]
	movzx edx,ah
	punpcklqdq xmm1,xmm2
	movq xmm0,[esi+edx*8+264192]
	movzx edx,bh
	movq xmm2,[esi+edx*8+264192]
	shr eax,16
	shr ebx,16
	punpcklqdq xmm0,xmm2
	paddsw xmm1,xmm0
	movzx edx,al
	movq xmm0,[esi+edx*8+262144]
	movzx edx,bl
	movq xmm2,[esi+edx*8+262144]
	punpcklqdq xmm0,xmm2
	paddsw xmm1,xmm0
	movzx edx,ah
	movq xmm0,[esi+edx*8+268288]
	movzx edx,bh
	movq xmm2,[esi+edx*8+268288]
	punpcklqdq xmm0,xmm2
	paddsw xmm1,xmm0
	movdqu [edi],xmm1

	lea edi,[edi+16]
	lea ecx,[ecx+8]
	dec ebp
	jnz hf2lop4

	mov ecx,dword [esp+64] ;tmpbuff
	mov esi,dword [esp+24] ;outPt

	mov ebp,dword [esp+28] ;dwidth

	mov ebx,dword [esp+40] ;index
	mov edi,dword [esp+44] ;weight
	align 16
hf2lop2:
	mov edx,dword [esp+36] ;tap
	pxor xmm2,xmm2
	pxor xmm3,xmm3
	align 16
hf2lop3:
	mov eax,dword [ebx]
	movq xmm0,[ecx+eax]
	mov eax,dword [ebx+4]
	movq xmm1,[ecx+eax]
	punpcklwd xmm0,xmm1
	pmaddwd xmm0,[edi]
	paddd xmm2,xmm0
	
	lea edi,[edi+16]
	lea ebx,[ebx+8]
	dec edx
	jnz hf2lop3

	mov edx,dword [esp+36] ;tap
	align 16
hf2lop3b:
	mov eax,dword [ebx]
	movq xmm0,[ecx+eax]
	mov eax,dword [ebx+4]
	movq xmm1,[ecx+eax]
	punpcklwd xmm0,xmm1
	pmaddwd xmm0,[edi]
	paddd xmm3,xmm0
	
	lea edi,[edi+16]
	lea ebx,[ebx+8]
	dec edx
	jnz hf2lop3b

	psrad xmm2,14
	psrad xmm3,14
	packssdw xmm2,xmm3
	movdqu [esi],xmm2
	lea esi,[esi+16]
	dec ebp
	jnz hf2lop2

	mov eax,dword [esp+48] ;sstep
	mov edx,dword [esp+52] ;dstep
	add dword [esp+20],eax ;inPt
	add dword [esp+24],edx ;outPt

	dec dword [esp+32] ;currHeight
	jnz hf2lop
	
	align 16
hfexit:
	pop edi
	pop esi
	pop ebx
	pop ebp
	ret

;void LanczosResizer8_C8_vertical_filter(UInt8 *inPt, UInt8 *outPt, OSInt dwidth, OSInt height, OSInt tap, OSInt *index, Int64 *weight, OSInt sstep, OSInt dstep, UInt8 *rgbTable)
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
_LanczosResizer8_C8_vertical_filter:
LanczosResizer8_C8_vertical_filter:
	push ebp
	push ebx
	push esi
	push edi
	test dword [esp+28],1 ;dwidth
	jnz vf1start
	cmp dword [esp+36],6 ;tap
	jz vf6start
	jmp vfstart
	
	align 16
vf1start:					; if (dwidth & 1)
	shr dword [esp+36],1 ;tap
	mov eax,dword [esp+28] ;dwidth
	shl eax,2
	mov ecx,dword [esp+24] ;outPt
	sub dword [esp+52],eax ;dstep
	pxor xmm3,xmm3
	align 16
vf1lop:
	mov esi,dword [esp+20] ;inPt
	mov ebp,dword [esp+28] ;dwidth
	align 16
vf1lop2:
	mov ebx,dword [esp+40] ;index
	mov edi,dword [esp+44] ;weight

	mov edx,dword [esp+36] ;tap
	pxor xmm2,xmm2
	ALIGN 16
vf1lop3:
	mov eax,dword [ebx]
	movq xmm0, [esi+eax]
	mov eax,dword [ebx+4]
	movq xmm1,[esi+eax]
	punpcklwd xmm0, xmm1
	pmaddwd xmm0,[edi]
	paddd xmm2,xmm0
	lea edi,[edi+16]
	lea ebx,[ebx+8]

	dec edx
	jnz vf1lop3

	mov edi,dword [esp+56] ;rgbTable

	psrad xmm2,15
	packssdw xmm2,xmm3
	pextrw ebx,xmm2,2
	pextrw edx,xmm2,3
	mov al,byte [edi+ebx+131072]
	mov ah,byte [edi+edx+196608]
	shl eax,16
	pextrw ebx,xmm2,0
	pextrw edx,xmm2,1
	mov al,byte [edi+ebx]
	mov ah,byte [edi+edx+65536]

	movnti dword [ecx],eax
	lea esi,[esi+8]
	lea ecx,[ecx+4]
	dec ebp
	jnz vf1lop2

	mov eax,dword [esp+36] ;tap
	shl eax,3
	lea edx,[eax*2]
	add dword [esp+40],eax ;index
	add dword [esp+44],edx ;weight

	add ecx,dword [esp+52] ;dstep

	dec dword [esp+32] ;currHeight
	jnz vf1lop
	jmp vfexit

	align 16
vf6start:				;else if (tap == 6)
	mov eax,dword [esp+28] ;dwidth
	shr dword [esp+36],1 ;tap
	lea edx,[eax*4]
	shr eax,1
	sub dword [esp+52],edx ;dstep
	mov dword [esp+28],eax ;dwidth

	mov ecx,dword [esp+24] ;outPt
	mov edi,dword [esp+40] ;index
	mov ebp,dword [esp+56] ;rgbTable
	pxor xmm3,xmm3
	align 16
vf6lop4:
	mov ebx,dword [esp+44] ;weight
	mov esi,dword [esp+20] ;inPt

	movdqa xmm5,[ebx]
	movdqa xmm6,[ebx+16]
	movdqa xmm7,[ebx+32]

	mov ebx,dword [esp+28] ;dwidth

	align 16
vf6lop5:
	pxor xmm3,xmm3
	pxor xmm4,xmm4

	mov eax,dword [edi]
	mov edx,dword [edi+4]
	movdqu xmm0,[esi+eax]
	movdqu xmm1,[esi+edx]
	movdqu xmm2,xmm0
	punpcklwd xmm0, xmm1
	punpckhwd xmm2, xmm1
	pmaddwd xmm0,xmm5
	pmaddwd xmm2,xmm5
	paddd xmm3,xmm0
	paddd xmm4,xmm2
	mov eax,dword [edi+8]
	mov edx,dword [edi+12]
	movdqu xmm0,[esi+eax]
	movdqu xmm1,[esi+edx]
	movdqu xmm2,xmm0
	punpcklwd xmm0, xmm1
	punpckhwd xmm2, xmm1
	pmaddwd xmm0,xmm6
	pmaddwd xmm2,xmm6
	paddd xmm3,xmm0
	paddd xmm4,xmm2
	mov eax,dword [edi+16]
	mov edx,dword [edi+20]
	movdqu xmm0,[esi+eax]
	movdqu xmm1,[esi+edx]
	movdqu xmm2,xmm0
	punpcklwd xmm0, xmm1
	punpckhwd xmm2, xmm1
	pmaddwd xmm0,xmm7
	pmaddwd xmm2,xmm7
	paddd xmm3,xmm0
	paddd xmm4,xmm2

	psrad xmm3,15
	psrad xmm4,15
	packssdw xmm3,xmm4
	pextrw edx,xmm3,2
	mov al,byte [ebp+edx+131072]
	pextrw edx,xmm3,3
	mov ah,byte [ebp+edx+196608]
	shl eax,16
	pextrw edx,xmm3,0
	mov al,byte [ebp+edx]
	pextrw edx,xmm3,1
	mov ah,byte [ebp+edx+65536]
	movd xmm0,eax

	pextrw edx,xmm3,6
	mov al,byte [ebp+edx+131072]
	pextrw edx,xmm3,7
	mov ah,byte [ebp+edx+196608]
	shl eax,16
	pextrw edx,xmm3,4
	mov al,byte [ebp+edx]
	pextrw edx,xmm3,5
	mov ah,byte [ebp+edx+65536]
	movd xmm1,eax
	punpckldq xmm0,xmm1

	movq [ecx],xmm0
	lea ecx,[ecx+8]
	lea esi,[esi+16]
	dec ebx
	jnz vf6lop5

	add edi,24 ;index
	add dword [esp+44],48 ;weight

	add ecx,dword [esp+52] ;dstep

	dec dword [esp+32] ;currHeight
	jnz vf6lop4
	jmp vfexit

	align 16
vfstart:
	shr dword [esp+36],1 ;tap
	shr dword [esp+28],1 ;dwidth
	pxor xmm3,xmm3
	align 16
vflop4:

	mov esi,dword [esp+20] ;inPt
	mov ecx,dword [esp+24] ;outPt
	mov ebp,dword [esp+28] ;dwidth
	
	align 16
vflop5:

	mov edx,dword [esp+36] ;tap
	mov ebx,dword [esp+40] ;index
	mov edi,dword [esp+44] ;weight
	pxor xmm2,xmm2
	pxor xmm6,xmm6
	ALIGN 16
vflop6:
	mov eax,dword [ebx]
	movdqu xmm0, [esi+eax]
	mov eax,dword [ebx+4]
	movdqu xmm1, [esi+eax]
	movdqu xmm4,xmm0
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

	mov edi,dword [esp+56] ;rgbTable

	psrad xmm2,15
	psrad xmm6,15
	packssdw xmm2,xmm6
	pextrw ebx,xmm2,2
	pextrw edx,xmm2,3
	mov al,byte [edi+ebx+131072]
	mov ah,byte [edi+edx+196608]
	shl eax,16
	pextrw ebx,xmm2,0
	pextrw edx,xmm2,1
	mov al,byte [edi+ebx]
	mov ah,byte [edi+edx+65536]
	movd xmm0,eax

	pextrw ebx,xmm2,6
	pextrw edx,xmm2,7
	mov al,byte [edi+ebx+131072]
	mov ah,byte [edi+edx+196608]
	shl eax,16
	pextrw ebx,xmm2,4
	pextrw edx,xmm2,5
	mov al,byte [edi+ebx]
	mov ah,byte [edi+edx+65536]
	movd xmm1,eax
	punpckldq xmm0,xmm1

	movq [ecx],xmm0
	lea ecx,[ecx+8]
	lea esi,[esi+16]
	dec ebp
	jnz vflop5

	mov eax,dword [esp+36] ;tap
	shl eax,3
	lea edx,[eax*2]
	add dword [esp+40],eax ;index
	add dword [esp+44],edx ;weight

	mov edx,dword [esp+52] ;dstep
	add dword [esp+24],edx ;outPt

	dec dword [esp+32] ;currHeight
	jnz vflop4

	align 16
vfexit:
	pop edi
	pop esi
	pop ebx
	pop ebp
	ret

;void LanczosResizer8_C8_expand(UInt8 *inPt, UInt8 *outPt, OSInt width, OSInt height, OSInt sstep, OSInt dstep, UInt8 *rgbTable)
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
_LanczosResizer8_C8_expand:
LanczosResizer8_C8_expand:
	push ebp
	push ebx
	push esi
	push edi
	
	mov eax,dword [esp+28] ;width
	lea edx,[eax*4]
	shl eax,3
	sub dword [esp+36],edx ;sstep
	sub dword [esp+40],eax ;dstep
	mov ecx,dword [esp+20] ;inPt
	mov esi,dword [esp+24] ;outPt
	mov ebp,dword [esp+32] ;height
	mov ebx,dword [esp+44] ;rgbTable
	align 16
explop:
	mov eax,dword [esp+28] ;width

	ALIGN 16
explop2:
	movzx edx,byte [ecx+3]
	movq xmm1,[ebx+edx*8+268288]
	movzx edx,byte [ecx+2]
	movq xmm0,[ebx+edx*8+262144]
	paddsw xmm1,xmm0
	movzx edx,byte [ecx+1]
	movq xmm0,[ebx+edx*8+264192]
	paddsw xmm1,xmm0
	movzx edx,byte [ecx]
	movq xmm0,[ebx+edx*8+266240]
	paddsw xmm1,xmm0
	movq [esi],xmm1

	lea esi,[esi+8]
	lea ecx,[ecx+4]
	dec eax
	jnz explop2

	add ecx,dword [esp+36] ;sstep
	add esi,dword [esp+40] ;dstep

	dec ebp
	jnz explop
	pop edi
	pop esi
	pop ebx
	pop ebp
	ret

;void LanczosResizer8_C8_expand_pa(UInt8 *inPt, UInt8 *outPt, OSInt width, OSInt height, OSInt sstep, OSInt dstep, UInt8 *rgbTable)
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
_LanczosResizer8_C8_expand_pa:
LanczosResizer8_C8_expand_pa:
	push ebp
	push ebx
	push esi
	push edi
	mov eax,dword [esp+28] ;width
	lea edx,[eax*4]
	shl eax,3
	sub dword [esp+36],edx ;sstep
	sub dword [esp+40],eax ;dstep
	mov ecx,dword [esp+20] ;inPt
	mov esi,dword [esp+24] ;outPt
	mov ebp,dword [esp+32] ;height
	mov ebx,dword [esp+44] ;rgbTable
	align 16
exppalop:
	mov edi,dword [esp+28] ;width
	ALIGN 16
exppalop2:
	mov eax,dword [ecx]
	movzx edx,al
	movq xmm1,[ebx+edx*8+266240]
	movzx edx,ah
	movq xmm0,[ebx+edx*8+264192]
	shr eax,16
	paddsw xmm1,xmm0
	movzx edx,al
	movq xmm0,[ebx+edx*8+262144]
	paddsw xmm1,xmm0
	movzx edx,ah

	movd xmm0,edx
	punpcklbw xmm0, xmm0
	punpcklbw xmm0, xmm0
	punpcklbw xmm0, xmm0
	psrlw xmm0,1
	pmulhw xmm0, xmm1
	psllw xmm0,1
	movq xmm1,[ebx+edx*8+268288]
	paddsw xmm0,xmm1
	movq [esi],xmm0

	lea ecx,[ecx+4]
	lea esi,[esi+8]
	dec edi
	jnz exppalop2

	add ecx,dword [esp+36] ;sstep
	add esi,dword [esp+40] ;dstep

	dec ebp
	jnz exppalop
	pop edi
	pop esi
	pop ebx
	pop ebp
	ret

;void LanczosResizer8_C8_collapse(UInt8 *inPt, UInt8 *outPt, OSInt width, OSInt height, OSInt sstep, OSInt dstep, UInt8 *rgbTable)
;	Int64 toAdd = 0xff80ff80ff80ff80;
;	Int64 toAdd2 = 0x80808080;
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
_LanczosResizer8_C8_collapse:
LanczosResizer8_C8_collapse:
	push ebp
	push ebx
	push esi
	push edi
	mov eax,dword [esp+28] ;width
	lea edx,[eax*8]
	shl eax,2
	sub dword [esp+36],edx ;sstep
	sub dword [esp+40],eax ;dstep
	mov ebp,dword [esp+32] ;height
	mov ecx,dword [esp+20] ;inPt
	mov esi,dword [esp+24] ;outPt
	mov ebx,dword [esp+44] ;rgbTable
	align 16
collop:
	mov edi,dword [esp+28] ;width
	
	ALIGN 16
collop2:
	movzx edx,word [ecx+6]
	mov ah,byte [ebx+edx+196608]
	movzx edx,word [ecx+4]
	mov al,byte [ebx+edx+131072]
	shl eax,16
	movzx edx,word [ecx+2]
	mov ah,byte [ebx+edx+65536]
	movzx edx,word [ecx]
	mov al,byte [ebx+edx]

	movnti dword [esi],eax
	lea esi,[esi+4]
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

;void LanczosResizer8_C8_imgcopy(UInt8 *inPt, UInt8 *outPt, OSInt width, OSInt height, OSInt sstep, OSInt dstep, UInt8 *rgbTable)
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
_LanczosResizer8_C8_imgcopy:
LanczosResizer8_C8_imgcopy:
	push ebp
	push ebx
	push esi
	push edi
	mov eax,dword [esp+28] ;width
	mov ebx,dword [esp+44] ;rgbTable
	shl eax,2
	sub dword [esp+36],eax ;sstep
	sub dword [esp+40],eax ;dstep
	mov ebp,dword [esp+32] ;height
	mov esi,dword [esp+20] ;inPt
	mov edi,dword [esp+24] ;outPt
	align 16
iclop:
	mov edx,dword [esp+28] ;width
	ALIGN 16
iclop2:
	movzx eax,byte [esi+2]
	movq xmm1,[ebx+eax*8+262144]
	movzx eax,byte [esi+1]
	movq xmm0,[ebx+eax*8+264192]
	paddsw xmm1,xmm0
	movzx eax,byte [esi]
	movq xmm0,[ebx+eax*8+266240]
	paddsw xmm1,xmm0
	pextrw ecx,xmm1,2
	mov al,byte [ebx+ecx+131072]
	mov ah,byte [esi+3]
	shl eax,16
	pextrw ecx,xmm1,1
	mov ah,byte [ebx+ecx+65536]
	pextrw ecx,xmm1,0
	mov al,byte [ebx+ecx]
	movnti [edi],eax
	lea esi,[esi+4]
	lea edi,[edi+4]
	dec edx
	jnz iclop2

	add esi,dword [esp+36] ;sstep
	add edi,dword [esp+40] ;dstep
	dec ebp
	jnz iclop
	pop edi
	pop esi
	pop ebx
	pop ebp
	ret

;void LanczosResizer8_C8_imgcopy_pa(UInt8 *inPt, UInt8 *outPt, OSInt width, OSInt height, OSInt sstep, OSInt dstep, UInt8 *rgbTable)
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
_LanczosResizer8_C8_imgcopy_pa:
LanczosResizer8_C8_imgcopy_pa:
	push ebp
	push ebx
	push esi
	push edi
	mov eax,dword [esp+28] ;width
	shr eax,1
	lea edx,[eax*8]
	mov dword [esp+28],eax ;width = width >> 1;
	sub dword [esp+36],edx ;sstep
	sub dword [esp+40],edx ;dstep

	mov ecx,dword [esp+32] ;height
	mov esi,dword [esp+20] ;inPt
	mov edi,dword [esp+24] ;outPt
	mov ebx,dword [esp+44] ;rgbTable
	align 16
icpalop:
	mov ebp,dword [esp+28] ;width
	ALIGN 16
icpalop2:
	mov eax,dword [esi]
	movzx edx,al
	movq xmm1,[ebx+edx*8+266240]
	movzx edx,ah
	movq xmm0,[ebx+edx*8+264192]
	shr eax,16
	paddsw xmm1,xmm0
	movzx edx,al
	movq xmm0,[ebx+edx*8+262144]
	paddsw xmm1,xmm0
	movzx edx,ah
	movd xmm3,edx

	mov eax,dword [esi+4]
	movzx edx,al
	movq xmm2,[ebx+edx*8+266240]
	movzx edx,ah
	movq xmm0,[ebx+edx*8+264192]
	shr eax,16
	paddsw xmm2,xmm0
	movzx edx,al
	movq xmm0,[ebx+edx*8+262144]
	paddsw xmm2,xmm0
	punpcklqdq xmm1,xmm2
	movzx edx,ah
	movd xmm0,edx

	punpcklbw xmm3, xmm0
	punpcklbw xmm3, xmm3
	punpcklwd xmm3, xmm3
	punpckldq xmm3, xmm3
	psrlw xmm3,1
	pmulhw xmm1, xmm3
	psllw xmm1,1

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

	lea esi,[esi+8]
	lea edi,[edi+8]
	dec ebp
	jnz icpalop2

	add esi,dword [esp+36] ;sstep
	add edi,dword [esp+40] ;dstep
	dec ecx
	jnz icpalop
	
	pop edi
	pop esi
	pop ebx
	pop ebp
	ret
