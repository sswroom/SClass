section .text

global LanczosResizerH8_8_horizontal_filter
global LanczosResizerH8_8_horizontal_filter8
global LanczosResizerH8_8_vertical_filter
global LanczosResizerH8_8_expand
global LanczosResizerH8_8_collapse

;void LanczosResizerH8_8_horizontal_filter(UInt8 *inPt, UInt8 *outPt, IntOS width, IntOS height, IntOS tap, IntOS *index, Int64 *weight, IntOS sstep, IntOS dstep)
;0 rdi
;8 rsi
;16 rbx
;24 rbp
;32 retAddr
;rcx inPt r10
;rdx outPt r11
;r8 width
;r9 height
;72 tap
;80 index
;88 weight
;96 sstep
;104 dstep

	align 16
LanczosResizerH8_8_horizontal_filter:
	push rbp
	push rbx
	push rsi
	push rdi
	mov r10,rcx
	mov r11,rdx
	mov eax,0xff80ff80
	mov edx,0x80808080
	movd xmm5,eax
	movd xmm6,edx
	punpckldq xmm5,xmm5
	punpckldq xmm6,xmm6
	punpckldq xmm5,xmm5						;UInt8 toAdd[16] = {0x80, 0xff, 0x80, 0xff, 0x80, 0xff, 0x80, 0xff, 0x80, 0xff, 0x80, 0xff, 0x80, 0xff, 0x80, 0xff};
	punpckldq xmm6,xmm6						;UInt8 toAdd2[16] = {0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80};
	mov rax,r8 ;width
	lea rdx,[rax*4]
	sub qword [rsp+104],rdx ;dstep			dstep = dstep - width * 4;
	test rax,3
	jnz hfstart
	mov rdx,qword [rsp+72] ;tap
	test rdx,1
	jnz hfstart
	cmp rdx,6
	jz hf6start
	cmp rdx,8
	jz hf8start
	jmp hfstart2

	align 16
hfstart:									;if (width & 3 || tap & 1)
	pxor xmm3,xmm3
	mov rcx,r10 ;inPt
	mov rsi,r11 ;outPt
	align 16
hflop:
	mov rbp,r8 ;width
	mov rbx,qword [rsp+80] ;index
	mov rdi,qword [rsp+88] ;weight
	align 16
hflop2:
	mov rdx,qword [rsp+72] ;tap
	pxor xmm1,xmm1
	align 16
hflop3:
	mov rax,qword [rbx]
	movq xmm0,[rcx+rax*2]
	movq xmm2,[rdi]
	pmulhw xmm0,xmm2
	paddsw xmm1,xmm0
	lea rdi,[rdi+8]
	lea rbx,[rbx+8]
	dec rdx
	jnz hflop3

	psraw xmm1,5
	paddw xmm1,xmm5
	packsswb xmm1,xmm3
	paddb xmm1,xmm6

	movd dword [rsi],xmm1
	lea rsi,[rsi+4]
	dec rbp
	jnz hflop2

	add rcx,qword [rsp+96] ;sstep
	add rsi,qword [rsp+104] ;dstep

	dec r9 ;currHeight
	jnz hflop
	jmp hfexit

	align 16
hf6start:							;else if (tap == 6)
	mov rcx,r10 ;inPt
	mov rsi,r11 ;outPt
	mov rdx,r9 ;height
	align 16
hf6lop4:
	mov rbp,r8 ;width
	shr rbp,2

	mov rbx,qword [rsp+80] ;index
	mov rdi,qword [rsp+88] ;weight
	ALIGN 16
hf6lop5:

	pxor xmm1,xmm1
	pxor xmm2,xmm2
	mov rax,qword [rbx]
	movq xmm0,[rcx+rax*2]
	mov rax,qword [rbx+8]
	movq xmm3,[rcx+rax*2]
	punpcklqdq xmm0,xmm3
	pmulhw xmm0,[edi]
	paddsw xmm1,xmm0
	mov rax,qword [rbx+16]
	movq xmm0,[rcx+rax*2]
	mov rax,qword [rbx+24]
	movq xmm3,[rcx+rax*2]
	punpcklqdq xmm0,xmm3
	pmulhw xmm0,[edi+16]
	paddsw xmm1,xmm0
	mov rax,qword [rbx+32]
	movq xmm0,[rcx+rax*2]
	mov rax,qword [rbx+40]
	movq xmm3,[rcx+rax*2]
	punpcklqdq xmm0,xmm3
	pmulhw xmm0,[rdi+32]
	paddsw xmm1,xmm0
	lea rbx,[rbx+48]
	lea rdi,[rdi+48]

	mov rax,qword [rbx]
	movq xmm0,[rcx+rax*2]
	mov rax,qword [rbx+8]
	movq xmm3,[rcx+rax*2]
	punpcklqdq xmm0,xmm3
	pmulhw xmm0,[rdi]
	paddsw xmm2,xmm0
	mov rax,qword [rbx+16]
	movq xmm0,[rcx+rax*2]
	mov rax,qword [rbx+24]
	movq xmm3,[rcx+rax*2]
	punpcklqdq xmm0,xmm3
	pmulhw xmm0,[rdi+16]
	paddsw xmm2,xmm0
	mov rax,qword [rbx+32]
	movq xmm0,[rcx+rax*2]
	mov rax,qword [rbx+40]
	movq xmm3,[rcx+rax*2]
	punpcklqdq xmm0,xmm3
	pmulhw xmm0, [rdi+32]
	paddsw xmm2,xmm0
	lea rbx,[rbx+48]
	lea rdi,[rdi+48]

	movdqa xmm0,xmm1
	punpcklqdq xmm0,xmm2
	punpckhqdq xmm1,xmm2
	paddsw xmm1,xmm0

	pxor xmm2,xmm2
	pxor xmm4,xmm4
	mov rax,qword [rbx]
	movq xmm0,[rcx+rax*2]
	mov rax,qword [rbx+8]
	movq xmm3,[rcx+rax*2]
	punpcklqdq xmm0,xmm3
	pmulhw xmm0,[rdi]
	paddsw xmm2,xmm0
	mov rax,qword [rbx+16]
	movq xmm0,[rcx+rax*2]
	mov rax,qword [rbx+24]
	movq xmm3,[rcx+rax*2]
	punpcklqdq xmm0,xmm3
	pmulhw xmm0,[rdi+16]
	paddsw xmm2,xmm0
	mov rax,qword [rbx+32]
	movq xmm0,[rcx+rax*2]
	mov rax,qword [rbx+40]
	movq xmm3,[rcx+rax*2]
	punpcklqdq xmm0,xmm3
	pmulhw xmm0,[rdi+32]
	paddsw xmm2,xmm0
	lea rbx,[rbx+48]
	lea rdi,[rdi+48]

	mov rax,qword [rbx]
	movq xmm0,[rcx+rax*2]
	mov rax,qword [rbx+8]
	movq xmm3,[rcx+rax*2]
	punpcklqdq xmm0,xmm3
	pmulhw xmm0,[edi]
	paddsw xmm4,xmm0
	mov rax,qword [rbx+16]
	movq xmm0,[rcx+rax*2]
	mov rax,qword [rbx+24]
	movq xmm3,[rcx+rax*2]
	punpcklqdq xmm0,xmm3
	pmulhw xmm0,[rdi+16]
	paddsw xmm4,xmm0
	mov rax,qword [rbx+32]
	movq xmm0,[rcx+rax*2]
	mov rax,qword [rbx+40]
	movq xmm3,[rcx+rax*2]
	punpcklqdq xmm0,xmm3
	pmulhw xmm0,[rdi+32]
	paddsw xmm4,xmm0
	lea rbx,[rbx+48]
	lea rdi,[rdi+48]

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
	movdqu [rsi],xmm1
	lea rsi,[rsi+16]
	dec rbp
	jnz hf6lop5

	add rcx,qword [rsp+96] ;sstep
	add rsi,qword [rsp+104] ;dstep

	dec rdx
	jnz hf6lop4
	jmp hfexit

	align 16
hf8start:							;else if (tap == 8)
	shr r8,2 ;width
	mov rbx,r9 ;height
	mov rcx,r10 ;inPt
	mov rsi,r11 ;outPt
	mov rdx,qword [rsp+104] ;dstep

	align 16
hf8lop4:

	mov rbp,r8 ;width
	mov rdi,qword [rsp+88] ;weight
	xor rax,rax
	ALIGN 16
hf8lop5:

	pxor xmm1,xmm1
	pxor xmm2,xmm2
	mov eax,dword [rdi]
	movdqu xmm0,[rcx+rax*2]
	movdqu xmm3,[rcx+rax*2+16]
	pmulhw xmm0,[rdi+16]
	pmulhw xmm3,[rdi+32]
	paddsw xmm1,xmm0
	paddsw xmm1,xmm3
	movdqu xmm0,[rcx+rax*2+32]
	movdqu xmm3,[rcx+rax*2+48]
	pmulhw xmm0,[rdi+48]
	pmulhw xmm3,[rdi+64]
	paddsw xmm1,xmm0
	paddsw xmm1,xmm3

	mov eax,dword [rdi+4]
	movdqu xmm0,[rcx+rax*2]
	movdqu xmm3,[rcx+rax*2+16]
	pmulhw xmm0,[rdi+80]
	pmulhw xmm3,[rdi+96]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm3
	movdqu xmm0,[rcx+rax*2+32]
	movdqu xmm3,[rcx+rax*2+48]
	pmulhw xmm0,[rdi+112]
	pmulhw xmm3,[rdi+128]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm3

	movdqa xmm0,xmm1
	punpcklqdq xmm0,xmm2
	punpckhqdq xmm1,xmm2
	paddsw xmm1,xmm0

	pxor xmm2,xmm2
	pxor xmm4,xmm4
	mov eax,dword [rdi+8]
	movdqu xmm0,[rcx+rax*2]
	movdqu xmm3,[rcx+rax*2+16]
	pmulhw xmm0,[rdi+144]
	pmulhw xmm3,[rdi+160]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm3
	movdqu xmm0,[rcx+rax*2+32]
	movdqu xmm3,[rcx+rax*2+48]
	pmulhw xmm0,[rdi+176]
	pmulhw xmm3,[rdi+192]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm3

	mov eax,dword [rdi+12]
	movdqu xmm0,[rcx+rax*2]
	movdqu xmm3,[rcx+rax*2+16]
	pmulhw xmm0,[rdi+208]
	pmulhw xmm3,[rdi+224]
	paddsw xmm4,xmm0
	paddsw xmm4,xmm3
	movdqu xmm0,[rcx+rax*2+32]
	movdqu xmm3,[rcx+rax*2+48]
	pmulhw xmm0,[rdi+240]
	pmulhw xmm3,[rdi+256]
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
	movntdq [rsi],xmm1
	lea rsi,[rsi+16]
	dec rbp
	jnz hf8lop5

	add rcx,qword [rsp+96] ;sstep
	add rsi,rdx ;dAdd

	dec rbx
	jnz hf8lop4
	jmp hfexit

	align 16
hfstart2:
	shr qword [rsp+72],1 ;tap
	shr r8,2 ;width
	mov rcx,r10 ;inPt
	mov rsi,r11 ;outPt
	align 16
hf2lop4:

	mov rbp,r8 ;width
	
	mov rbx,qword [rsp+80] ;index
	mov rdi,qword [rsp+88] ;weight
	align 16
hf2lop5:
	mov rdx,qword [rsp+72] ;tap
	pxor xmm1,xmm1
	pxor xmm2,xmm2
	ALIGN 16
hf2lop6:
	mov rax,qword [rbx]
	movq xmm0,[rcx+rax*2]
	mov rax,qword [rbx+8]
	movhpd xmm0,[rcx+rax*2]
	lea rbx,[rbx+16]
	pmulhw xmm0,[rdi]
	lea rdi,[rdi+16]
	paddsw xmm1,xmm0
	dec rdx
	jnz hf2lop6

	mov rdx,qword [rsp+72] ;tap
	ALIGN 16
hf2lop7:
	mov rax,qword [rbx]
	movq xmm0,[rcx+rax*2]
	mov rax,qword [rbx+8]
	movhpd xmm0,[rcx+rax*2]
	lea rbx,[rbx+16]
	pmulhw xmm0,[rdi]
	lea rdi,[rdi+16]
	paddsw xmm2,xmm0
	dec rdx
	jnz hf2lop7

	movdqa xmm0,xmm1
	punpcklqdq xmm0,xmm2
	punpckhqdq xmm1,xmm2
	paddsw xmm1,xmm0

	mov rdx,qword [rsp+72] ;tap
	pxor xmm2,xmm2
	pxor xmm4,xmm4
	ALIGN 16
hf2lop6b:
	mov rax,qword [rbx]
	movq xmm0,[rcx+rax*2]
	mov rax,qword [rbx+8]
	movhpd xmm0,[rcx+rax*2]
	lea rbx,[rbx+16]
	pmulhw xmm0,[rdi]
	lea rdi,[rdi+16]
	paddsw xmm2,xmm0
	dec rdx
	jnz hf2lop6b

	mov rdx,qword [rsp+72] ;tap
	ALIGN 16
hf2lop7b:
	mov rax,qword [rbx]
	movq xmm0,[rcx+rax*2]
	mov rax,qword [rbx+8]
	movhpd xmm0,[rcx+rax*2]
	lea rbx,[rbx+16]
	pmulhw xmm0,[rdi]
	lea rdi,[rdi+16]
	paddsw xmm4,xmm0
	dec rdx
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
	movdqu [rsi],xmm1
	lea rsi,[rsi+16]
	dec rbp
	jnz hf2lop5

	add rcx,qword [rsp+96] ;sstep
	add rsi,qword [rsp+104] ;dstep

	dec r9 ;currHeight
	jnz hf2lop4
	
	align 16
hfexit:
	pop rdi
	pop rsi
	pop rbx
	pop rbp
	ret

;void LanczosResizerH8_8_horizontal_filter8(UInt8 *inPt, UInt8 *outPt, IntOS width, IntOS height, IntOS tap, IntOS *index, Int64 *weight, IntOS sstep, IntOS dstep)
;0 rdi
;8 rsi
;16 rbx
;24 rbp
;32 retAddr
;rcx inPt r10
;rdx outPt r11
;r8 width
;r9 height
;72 tap
;80 index
;88 weight
;96 sstep
;104 dstep

	align 16
LanczosResizerH8_8_horizontal_filter8:
	push rbp
	push rbx
	push rsi
	push rdi
	mov r10,rcx
	mov r11,rdx
	mov eax,0xff80ff80
	mov edx,0x80808080
	movd xmm5,eax
	movd xmm6,edx
	punpckldq xmm5,xmm5
	punpckldq xmm6,xmm6
	punpckldq xmm5,xmm5						;UInt8 toAdd[16] = {0x80, 0xff, 0x80, 0xff, 0x80, 0xff, 0x80, 0xff, 0x80, 0xff, 0x80, 0xff, 0x80, 0xff, 0x80, 0xff};
	punpckldq xmm6,xmm6						;UInt8 toAdd2[16] = {0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80};
	mov rax,r8 ;width
	lea rdx,[rax*4]
	sub qword [rsp+104],rdx ;dstep			dstep = dstep - width * 4;
	test rax,3
	jnz hf8_start
	mov rdx,qword [rsp+72] ;tap
	test rdx,1
	jnz hf8_start
	cmp rdx,6
	jz hf8_6start
	cmp rdx,8
	jz hf8_8start
	jmp hf8_start2

	align 16
hf8_start:									;if (width & 3 || tap & 1)
	pxor xmm3,xmm3
	mov rcx,r10 ;inPt
	mov rsi,r11 ;outPt
	align 16
hf8_lop:

	mov rbp,r8 ;width
	mov rbx,qword [rsp+80] ;index
	mov rdi,qword [rsp+88] ;weight
	align 16
hf8_lop2:
	mov rdx,qword [rsp+72] ;tap
	pxor xmm1,xmm1
	align 16
hf8_lop3:
	mov rax,qword [rbx]
	movd xmm0,[rcx+rax]
	punpcklbw xmm0,xmm3
	psllw xmm0,6
	movq xmm4,[rdi]
	pmulhw xmm0,xmm4
	paddsw xmm1,xmm0
	lea rdi,[rdi+8]
	lea rbx,[rbx+8]
	dec rdx
	jnz hf8_lop3

	psraw xmm1,5
	paddw xmm1,xmm5
	packsswb xmm1,xmm3
	paddb xmm1,xmm6

	movd dword [rsi],xmm1
	lea rsi,[rsi+4]
	dec rbp
	jnz hf8_lop2

	add rcx,qword [rsp+96] ;sstep
	add rsi,qword [rsp+104] ;dstep

	dec r9 ;currHeight
	jnz hf8_lop
	jmp hf8exit

	align 16
hf8_6start:									;else if (tap == 6)
	pxor xmm7, xmm7
	shr r8,2 ;width
	mov rcx,r10 ;inPt
	mov rsi,r11 ;outPt
	mov rdx,r9 ;height
	align 16
hf8_6lop4:
	mov rbp,r8 ;width
	mov rbx,qword [rsp+80] ;index
	mov rdi,qword [rsp+88] ;weight
	ALIGN 16
hf8_6lop5:
	pxor xmm1,xmm1
	pxor xmm2,xmm2
	mov rax,qword [rbx]
	movd xmm0,[rcx+rax]
	mov rax,qword [rbx+8]
	movd xmm3,[rcx+rax]
	punpckldq xmm0,xmm3
	punpcklbw xmm0,xmm7
	psllw xmm0,6
	pmulhw xmm0,[rdi]
	paddsw xmm1,xmm0
	mov rax,qword [rbx+16]
	movd xmm0,[rcx+rax]
	mov rax,qword [rbx+24]
	movd xmm3,[rcx+rax]
	punpckldq xmm0,xmm3
	punpcklbw xmm0,xmm7
	psllw xmm0,6
	pmulhw xmm0,[rdi+16]
	paddsw xmm1,xmm0
	mov rax,qword [rbx+32]
	movd xmm0,[rcx+rax]
	mov rax,qword [rbx+40]
	movd xmm3,[rcx+rax]
	punpckldq xmm0,xmm3
	punpcklbw xmm0,xmm7
	psllw xmm0,6
	pmulhw xmm0,[rdi+32]
	paddsw xmm1,xmm0
	lea rbx,[rbx+48]
	lea rdi,[rdi+48]

	mov rax,qword [rbx]
	movd xmm0,[rcx+rax]
	mov rax,qword [rbx+8]
	movd xmm3,[rcx+rax]
	punpckldq xmm0,xmm3
	punpcklbw xmm0,xmm7
	psllw xmm0,6
	pmulhw xmm0,[rdi]
	paddsw xmm2,xmm0
	mov rax,qword [rbx+16]
	movd xmm0,[rcx+rax]
	mov rax,qword [rbx+24]
	movd xmm3,[rcx+rax]
	punpckldq xmm0,xmm3
	punpcklbw xmm0,xmm7
	psllw xmm0,6
	pmulhw xmm0,[rdi+16]
	paddsw xmm2,xmm0
	mov rax,qword [rbx+32]
	movd xmm0,[rcx+rax]
	mov rax,qword [rbx+40]
	movd xmm3,[rcx+rax]
	punpckldq xmm0,xmm3
	punpcklbw xmm0,xmm7
	psllw xmm0,6
	pmulhw xmm0,[rdi+32]
	paddsw xmm2,xmm0
	lea rbx,[rbx+48]
	lea rdi,[rdi+48]

	movdqa xmm0,xmm1
	punpcklqdq xmm0,xmm2
	punpckhqdq xmm1,xmm2
	paddsw xmm1,xmm0

	pxor xmm2,xmm2
	pxor xmm4,xmm4
	mov rax,qword [rbx]
	movd xmm0,[rcx+rax]
	mov rax,qword [rbx+8]
	movd xmm3,[rcx+rax]
	punpckldq xmm0,xmm3
	punpcklbw xmm0,xmm7
	psllw xmm0,6
	pmulhw xmm0,[rdi]
	paddsw xmm2,xmm0
	mov rax,qword [rbx+16]
	movd xmm0,[rcx+rax]
	mov rax,qword [rbx+24]
	movd xmm3,[rcx+rax]
	punpckldq xmm0,xmm3
	punpcklbw xmm0,xmm7
	psllw xmm0,6
	pmulhw xmm0,[rdi+16]
	paddsw xmm2,xmm0
	mov rax,qword [rbx+32]
	movd xmm0,[rcx+rax]
	mov rax,qword [rbx+40]
	movd xmm3,[rcx+rax]
	punpckldq xmm0,xmm3
	punpcklbw xmm0,xmm7
	psllw xmm0,6
	pmulhw xmm0,[rdi+32]
	paddsw xmm2,xmm0
	lea rbx,[rbx+48]
	lea rdi,[rdi+48]

	mov rax,qword [rbx]
	movd xmm0,[rcx+rax]
	mov rax,qword [rbx+8]
	movd xmm3,[rcx+rax]
	punpckldq xmm0,xmm3
	punpcklbw xmm0,xmm7
	psllw xmm0,6
	pmulhw xmm0,[rdi]
	paddsw xmm4,xmm0
	mov rax,qword [rbx+16]
	movd xmm0,[rcx+rax]
	mov rax,qword [rbx+24]
	movd xmm3,[rcx+rax]
	punpckldq xmm0,xmm3
	punpcklbw xmm0,xmm7
	psllw xmm0,6
	pmulhw xmm0, [rdi+16]
	paddsw xmm4,xmm0
	mov rax,qword [rbx+32]
	movd xmm0,[rcx+rax]
	mov rax,qword [rbx+40]
	movd xmm3,[rcx+rax]
	punpckldq xmm0,xmm3
	punpcklbw xmm0,xmm7
	psllw xmm0,6
	pmulhw xmm0,[rdi+32]
	paddsw xmm4,xmm0
	lea rbx,[rbx+48]
	lea rdi,[rdi+48]

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
	movdqu [rsi],xmm1
	lea rsi,[rsi+16]
	dec rbp
	jnz hf8_6lop5

	add rcx,qword [rsp+96] ;sstep
	add rsi,qword [rsp+104] ;dstep

	dec rdx
	jnz hf8_6lop4
	jmp hf8exit

	align 16
hf8_8start:										;else if (tap == 8)
	shr r8,2 ;width
	pxor xmm7, xmm7
	mov rcx,r10 ;inPt
	mov rsi,r11 ;outPt
	mov rdx,r9 ;height
	mov rbx,qword [rsp+104] ;dstep
	align 16
hf8_8lop4:
	mov rbp,r8 ;width
	mov rdi,qword [rsp+88] ;weight
	xor rax,rax
	ALIGN 16
hf8_8lop5:

	pxor xmm1,xmm1
	pxor xmm2,xmm2
	mov eax,dword [rdi]
	movdqu xmm0,[rcx+rax]
	movdqa xmm3,xmm0
	punpcklbw xmm0,xmm0
	punpckhbw xmm3,xmm3
	psrlw xmm0,2
	psrlw xmm3,2
	pmulhw xmm0,[rdi+16]
	pmulhw xmm3,[rdi+32]
	paddsw xmm1,xmm0
	paddsw xmm1,xmm3
	movdqu xmm0,[rcx+rax+16]
	movdqa xmm3,xmm0
	punpcklbw xmm0,xmm0
	punpckhbw xmm3,xmm3
	psrlw xmm0,2
	psrlw xmm3,2
	pmulhw xmm0,[rdi+48]
	pmulhw xmm3,[rdi+64]
	paddsw xmm1,xmm0
	paddsw xmm1,xmm3

	mov eax,dword [rdi+4]
	movdqu xmm0,[rcx+rax]
	movdqa xmm3,xmm0
	punpcklbw xmm0,xmm0
	punpckhbw xmm3,xmm3
	psrlw xmm0,2
	psrlw xmm3,2
	pmulhw xmm0,[rdi+80]
	pmulhw xmm3,[rdi+96]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm3
	movdqu xmm0,[rcx+rax+16]
	movdqa xmm3,xmm0
	punpcklbw xmm0,xmm0
	punpckhbw xmm3,xmm3
	psrlw xmm0,2
	psrlw xmm3,2
	pmulhw xmm0,[rdi+112]
	pmulhw xmm3,[rdi+128]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm3

	movdqa xmm0,xmm1
	punpcklqdq xmm0,xmm2
	punpckhqdq xmm1,xmm2
	paddsw xmm1,xmm0

	pxor xmm2,xmm2
	pxor xmm4,xmm4
	mov eax,dword [rdi+8]
	movdqu xmm0,[rcx+rax]
	movdqa xmm3,xmm0
	punpcklbw xmm0,xmm0
	punpckhbw xmm3,xmm3
	psrlw xmm0,2
	psrlw xmm3,2
	pmulhw xmm0,[rdi+144]
	pmulhw xmm3,[rdi+160]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm3
	movdqu xmm0,[rcx+rax+16]
	movdqa xmm3,xmm0
	punpcklbw xmm0,xmm0
	punpckhbw xmm3,xmm3
	psrlw xmm0,2
	psrlw xmm3,2
	pmulhw xmm0,[rdi+176]
	pmulhw xmm3,[rdi+192]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm3

	mov eax,dword [rdi+12]
	movdqu xmm0,[rcx+rax]
	movdqa xmm3,xmm0
	punpcklbw xmm0,xmm0
	punpckhbw xmm3,xmm3
	psrlw xmm0,2
	psrlw xmm3,2
	pmulhw xmm0,[rdi+208]
	pmulhw xmm3,[rdi+224]
	paddsw xmm4,xmm0
	paddsw xmm4,xmm3
	movdqu xmm0,[rcx+rax+16]
	movdqa xmm3,xmm0
	punpcklbw xmm0,xmm0
	punpckhbw xmm3,xmm3
	psrlw xmm0,2
	psrlw xmm3,2
	pmulhw xmm0,[rdi+240]
	pmulhw xmm3,[rdi+256]
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
	movdqu [rsi],xmm1
	lea rdi,[rdi+272]
	lea rsi,[rsi+16]
	dec rbp
	jnz hf8_8lop5

	add rcx,qword [rsp+96] ;sstep
	add rsi,rbx ;dstep

	dec rdx
	jnz hf8_8lop4
	jmp hf8exit

	align 16
hf8_start2:
	shr qword [rsp+72],1 ;tap
	shr r8,2 ;width
	pxor xmm7, xmm7
	mov rcx,r10 ;inPt
	mov rsi,r11 ;outPt
	align 16
hf8_lop4:
	mov rbp,r8 ;width
	mov rbx,qword [rsp+80] ;index
	mov rdi,qword [rsp+88] ;weight
	align 16
hf8_lop5:
	mov rdx,qword [rsp+72] ;tap
	pxor xmm1,xmm1
	pxor xmm2,xmm2
	ALIGN 16
hf8_lop6:
	mov rax,qword [rbx]
	movd xmm0,[rcx+rax]
	mov rax,qword [rbx+8]
	movd xmm3,[rcx+rax]
	add rbx,16
	punpckldq xmm0,xmm3
	punpcklbw xmm0,xmm7
	psllw xmm0,6
	pmulhw xmm0,[rdi]
	add rdi,16
	paddsw xmm1,xmm0
	dec rdx
	jnz hf8_lop6

	mov rdx,qword [rsp+72] ;tap
	ALIGN 16
hf8_lop7:
	mov rax,qword [rbx]
	movd xmm0,[rcx+rax]
	mov rax,qword [rbx+8]
	movd xmm3,[rcx+rax]
	add rbx,16
	punpckldq xmm0,xmm3
	punpcklbw xmm0,xmm7
	psllw xmm0,6
	pmulhw xmm0,[rdi]
	add rdi,16
	paddsw xmm2,xmm0
	dec rdx
	jnz hf8_lop7

	movdqa xmm0,xmm1
	punpcklqdq xmm0,xmm2
	punpckhqdq xmm1,xmm2
	paddsw xmm1,xmm0

	mov rdx,qword [rsp+72] ;tap
	pxor xmm2,xmm2
	pxor xmm4,xmm4
	ALIGN 16
hf8_lop6b:
	mov rax,qword [rbx]
	movd xmm0,[rcx+rax]
	mov rax,qword [rbx+8]
	movd xmm3,[rcx+rax]
	add rbx,16
	punpckldq xmm0,xmm3
	punpcklbw xmm0,xmm7
	psllw xmm0,6
	pmulhw xmm0,[rdi]
	add rdi,16
	paddsw xmm2,xmm0
	dec rdx
	jnz hf8_lop6b

	mov rdx,qword [rsp+72] ;tap
	ALIGN 16
hf8_lop7b:
	mov rax,qword [rbx]
	movd xmm0,[rcx+rax]
	mov rax,qword [rbx+8]
	movd xmm3,[rcx+rax]
	add rbx,16
	punpckldq xmm0,xmm3
	punpcklbw xmm0,xmm7
	psllw xmm0,6
	pmulhw xmm0,[rdi]
	add rdi,16
	paddsw xmm4,xmm0
	dec rdx
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
	movdqu [rsi],xmm1
	add rsi,16
	dec rbp
	jnz hf8_lop5

	add rcx,qword [rsp+96] ;sstep
	add rsi,qword [rsp+104] ;dstep

	dec r9 ;currHeight
	jnz hf8_lop4
	align 16
hf8exit:
	pop rdi
	pop rsi
	pop rbx
	pop rbp
	ret

;void LanczosResizerH8_8_vertical_filter(UInt8 *inPt, UInt8 *outPt, IntOS width, IntOS height, IntOS tap, IntOS *index, Int64 *weight, IntOS sstep, IntOS dstep)
;0 currWidth
;8 rdi
;16 rsi
;24 rbx
;32 rbp
;40 retAddr
;rcx inPt r10
;rdx outPt r11
;r8 width
;r9 height
;80 tap
;88 index
;96 weight
;104 sstep
;112 dstep

	align 16
LanczosResizerH8_8_vertical_filter:
	push rbp
	push rbx
	push rsi
	push rdi
	push rax
	mov r10,rcx
	mov r11,rdx
	mov rax,r8 ;width
	lea rdx,[rax*8]
	mov rsi,r11 ;outPt
	sub qword [rsp+112],rdx ;dstep
	test rax,3
	jnz vfstart
	test rsi,15
	jnz vfstart
	test qword [rsp+112],15 ;dstep
	jnz vfstart
	mov rax,qword [rsp+80] ;tap
	cmp rax,6
	jz vf6start
	jmp vfstart2
	
	align 16
vfstart:							;if ((width & 3) != 0 || (((IntOS)outPt) & 15) != 0 || (dstep & 15) != 0)
vflop:
	mov rax,r8 ;width
	mov rcx,r10 ;inPt

	mov qword [rsp+0],rax ;currWidth
	test rsi,15
	jz vflop2_1

	mov rdx,1
	dec rax
	mov qword [rsp+0],rax ;currWidth

	align 16
vflop2_0:
	mov rbx,qword [rsp+88] ;index
	mov rdi,qword [rsp+96] ;weight

	mov rbp,qword [rsp+80] ;tap
	pxor xmm1,xmm1
vflop3_0:
	mov rax,qword [rbx]
	movd xmm0,dword [rcx+rax]
	lea rbx,[rbx+8]
	punpcklbw xmm0,xmm0
	psrlw xmm0,1
	movq xmm2,[rdi]
	pmulhw xmm0,xmm2
	paddsw xmm1,xmm0
	lea rdi,[rdi+8]
	dec rbp
	jnz vflop3_0

	movq [rsi],xmm1
	lea rcx,[rcx+4]
	lea rsi,[rsi+8]
	dec rdx
	jnz vflop2_0

	align 16
vflop2_1:
	mov rdx,qword [rsp+0] ;currWidth
	shr rdx,2
	jz vflop4
	align 16
vflop2:
	mov rbx,qword [rsp+88] ;index
	mov rdi,qword [rsp+96] ;weight

	mov rbp,qword [rsp+80] ;tap
	pxor xmm3,xmm3
	pxor xmm4,xmm4
	ALIGN 16
vflop3:
	mov rax,qword [rbx]
	movdqu xmm0,[rcx+rax]
	lea rbx,[rbx+8]
	movdqa xmm1,xmm0
	movq xmm2,[rdi]
	punpcklbw xmm0,xmm0
	punpckhbw xmm1,xmm1
	punpcklqdq xmm2,xmm2
	psrlw xmm0,1
	psrlw xmm1,1
	pmulhw xmm0, xmm2
	pmulhw xmm1, xmm2
	paddsw xmm3,xmm0
	paddsw xmm4,xmm1
	lea rdi,[rdi+8]
	dec rbp
	jnz vflop3

	movntdq [rsi],xmm3 ;movdqu?
	movntdq [rsi+16],xmm4 ;movdqu?
	lea rsi,[rsi+32]
	lea rcx,[rcx+16]
	dec rdx
	jnz vflop2
	
	align 16
vflop4:
	mov rdx,3
	and rdx,qword [rsp+0] ;currWidth
	jz vflop5
	align 16
vflop2_2:
	mov rbx,qword [rsp+88] ;index
	mov rdi,qword [rsp+96] ;weight

	mov rbp,qword [rsp+80] ;tap
	pxor xmm1,xmm1
	align 16
vflop3_2:
	mov rax,qword [rbx]
	movd xmm0,dword [rcx+rax]
	lea rbx,[rbx+8]
	punpcklbw xmm0,xmm0
	psrlw xmm0,1
	movq xmm2,[rdi]
	pmulhw xmm0,xmm2
	paddsw xmm1,xmm0
	lea rdi,[rdi+8]
	dec rbp
	jnz vflop3_2

	movq [rsi],xmm1
	lea rsi,[rsi+8]
	lea rcx,[rcx+4]
	dec rdx
	jnz vflop2_2
vflop5:
	mov rax,qword [rsp+80] ;tap
	shl rax,3
	add qword [rsp+88],rax ;index
	add qword [rsp+96],rax ;weight

	add rsi,qword [rsp+112] ;dstep

	dec r9 ;currHeight
	jnz vflop
	jmp vfexit

	align 16
vf6start:								;else if (tap == 6)
	shr r8,2 ;width
	mov rbp,r9 ;height
	mov rbx,qword [rsp+88] ;index
	mov rdi,qword [rsp+96] ;weight

	align 16
vf6lop1b:
	mov rcx,r10 ;inPt
	mov rdx,r8 ;width

	movdqa xmm5,[rdi]
	movdqa xmm6,[rdi+16]
	movdqa xmm7,[rdi+32]
	ALIGN 16
vf6lop2b:
	pxor xmm3,xmm3
	pxor xmm4,xmm4

	mov rax,qword [rbx]
	movdqa xmm2,xmm5
	movdqu xmm0,[rcx+rax]
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
	mov rax,qword [rbx+8]
	movdqa xmm2,xmm5
	movdqu xmm0,[rcx+rax]
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

	mov rax,qword [rbx+16]
	movdqa xmm2,xmm6
	movdqu xmm0,[rcx+rax]
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
	mov rax,qword [rbx+24]
	movdqa xmm2,xmm6
	movdqu xmm0,[rcx+rax]
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

	mov rax,qword [rbx+32]
	movdqa xmm2,xmm7
	movdqu xmm0,[rcx+rax]
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
	mov rax,qword [rbx+40]
	movdqa xmm2,xmm7
	movdqu xmm0,[rcx+rax]
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

	movntdq [rsi],xmm3
	movntdq [rsi+16],xmm4
	lea rsi,[rsi+32]
	lea rcx,[rcx+16]
	dec rdx
	jnz vf6lop2b

	lea rbx,[rbx+48]
	lea rdi,[rdi+48]
	add rsi,qword [rsp+112] ;dAdd

	dec rbp
	jnz vf6lop1b
	jmp vfexit

	align 16
vfstart2:
	shr r8,2 ;width
	align 16
vflop1b:
	mov rcx,r10 ;inPt
	mov rbp,r8 ;width
	align 16
vflop2b:
	mov rbx,qword [rsp+88] ;index
	mov rdi,qword [rsp+96] ;weight

	mov rdx,qword [rsp+80] ;tap
	pxor xmm3,xmm3
	pxor xmm4,xmm4
	ALIGN 16
vflop3b:
	mov rax,qword [rbx]
	movdqu xmm0,[rcx+rax]
	movdqa xmm1,xmm0
	movq xmm2,[rdi]
	punpcklbw xmm0,xmm0
	punpckhbw xmm1,xmm1
	punpcklqdq xmm2,xmm2
	psrlw xmm0,1
	psrlw xmm1,1
	pmulhw xmm0, xmm2
	pmulhw xmm1, xmm2
	paddsw xmm3,xmm0
	paddsw xmm4,xmm1
	lea rbx,[rbx+8]
	lea rdi,[rdi+8]
	dec rdx
	jnz vflop3b

	movntdq [rsi],xmm3
	movntdq [rsi+16],xmm4
	lea rsi,[rsi+32]
	lea rcx,[rcx+16]
	dec rbp
	jnz vflop2b

	mov rax,qword [rsp+80] ;tap
	shl rax,3
	add qword [rsp+88],rax ;index
	add qword [rsp+96],rax ;weight

	add rsi,qword [rsp+112] ;dstep

	dec r9 ;currHeight
	jnz vflop1b
	
	align 16
vfexit:
	pop rax
	pop rdi
	pop rsi
	pop rbx
	pop rbp
	ret

;void LanczosResizerH8_8_expand(UInt8 *inPt, UInt8 *outPt, IntOS width, IntOS height, IntOS sstep, IntOS dstep)
;0 rdi
;8 rsi
;16 rbx
;24 rbp
;32 retAddr
;rcx inPt
;rdx outPt
;r8 width
;r9 height
;72 sstep
;80 dstep

	align 16
LanczosResizerH8_8_expand:
	push rbp
	push rbx
	push rsi
	push rdi
	;mov rcx,rcx ;inPt
	mov rsi,rdx ;outPt
	mov rbp,r8 ;width
	lea rdx,[rbp*4]
	lea rdx,[rbp*8]
	sub qword [rsp+72],rdx ;sAdd				IntOS sAdd = sstep - width * 4;
	sub qword [rsp+80],rdx ;dAdd				IntOS dAdd = dstep - width * 8;
	
	mov rdx,r9 ;height
	mov rax,qword [rsp+72] ;sAdd
	mov rbx,qword [rsp+80] ;dAdd
	
	shr rbp,1
	jb explop
	test rsi,15
	jnz exp2lop
	test rbx,15
	jnz exp2lop
	jmp expalop
	
	align 16
explop: 									;if (width & 1)
	mov rdi,rbp
	align 16
explop2:
	movq xmm0,[rcx]
	punpcklbw xmm0,xmm0
	psrlw xmm0,2
	movdqu [rsi],xmm0
	lea rcx,[rcx+8]
	lea rsi,[rsi+16]
	dec rdi
	jnz explop2

	movd xmm0,dword [rcx]
	punpcklbw xmm0,xmm0
	psrlw xmm0,2
	movq [rsi],xmm0
	lea rcx,[rcx+rax+4]
	lea rsi,[rsi+rbx+8]

	dec rdx
	jnz explop
	jmp expexit

	align 16
exp2lop:
	mov rdi,rbp
	align 16
exp2lop2:
	movq xmm0,[rcx]
	punpcklbw xmm0,xmm0
	psrlw xmm0,2
	movdqu [rsi],xmm0
	lea rcx,[rcx+8]
	lea rsi,[rsi+16]
	dec rdi
	jnz exp2lop2

	lea rcx,[rcx+rax]
	lea rsi,[rsi+rbx]

	dec rdx
	jnz exp2lop
	jmp expexit

	align 16
expalop:
	mov rdi,rbp
	align 16
expalop2:
	movq xmm0,[rcx]
	punpcklbw xmm0,xmm0
	psrlw xmm0,2
	movntdq [rsi],xmm0
	lea rcx,[rcx+8]
	lea rsi,[rsi+16]
	dec rdi
	jnz expalop2

	lea rcx,[rcx+rax]
	lea rsi,[rsi+rbx]

	dec rdx
	jnz expalop
	
	align 16
expexit:
	pop rdi
	pop rsi
	pop rbx
	pop rbp
	ret

;void LanczosResizerH8_8_collapse(UInt8 *inPt, UInt8 *outPt, IntOS width, IntOS height, IntOS sstep, IntOS dstep)
;0 edi
;8 esi
;16 ebx
;24 ebp
;32 retAddr
;rcx inPt
;rdx outPt
;r8 width
;r9 height
;36 sstep
;40 dstep

	align 16
LanczosResizerH8_8_collapse:
	push rbp
	push rbx
	push rsi
	push rdi
	mov rsi,rdx ;outPt
	;mov rcx,rcx ;inPt
	mov eax,0xff80ff80
	mov edx,0x80808080
	movd xmm2,eax
	movd xmm3,edx
	punpckldq xmm2,xmm2
	punpckldq xmm3,xmm3
	punpcklqdq xmm2,xmm2
	punpcklqdq xmm3,xmm3
	lea rax,[r8*8] ;width
	lea rdx,[r8*4] ;width
	mov rbx,qword [rsp+72] ;sAdd
	mov rdi,qword [rsp+80] ;dAdd
	sub rbx,rax						;IntOS sAdd = width * 8 - sstep;
	sub rdi,rdx						;IntOS dAdd = width * 4 - dstep;
	mov rbp,r9 ;height
	mov rdx,r8 ;width
	pxor xmm1,xmm1

	test rdx,3
	jnz collop
	shr rdx,2
	test rsi,15
	jnz col4lop
	test rdi,15
	jnz col4lop
	jmp colalop
	
	align 16
collop:
	mov rax,rdx

	align 16
collop2:
	movq xmm0,[rcx]
	psraw xmm0,6
	paddw xmm0,xmm2
	packsswb xmm0,xmm1
	paddb xmm0,xmm3
	movd dword [rsi],xmm0
	lea rsi,[rsi+4]
	lea rcx,[rcx+8]
	dec rax
	jnz collop2

	add rcx,rbx
	add rsi,rdi

	dec rbp
	jnz collop
	jmp colexit

	align 16
col4lop:
	mov rax,rdx

	align 16
col4lop2:
	movdqu xmm0,[rcx]
	movdqu xmm4,[rcx+16]
	psraw xmm0,6
	psraw xmm4,6
	paddw xmm0,xmm2
	paddw xmm4,xmm2
	packsswb xmm0,xmm4
	paddb xmm0,xmm3
	movdqu [rsi],xmm0
	lea rcx,[rcx+32]
	lea rsi,[rsi+16]
	dec rax
	jnz col4lop2

	add rcx,rbx
	add rsi,rdi

	dec rbp
	jnz col4lop
	jmp colexit

	align 16
colalop:
	mov rax,rdx

	align 16
colalop2:
	movdqu xmm0,[rcx]
	movdqu xmm4,[rcx+16]
	psraw xmm0,6
	psraw xmm4,6
	paddw xmm0,xmm2
	paddw xmm4,xmm2
	packsswb xmm0,xmm4
	paddb xmm0,xmm3
	movntdq [rsi],xmm0
	lea rcx,[rcx+32]
	lea rsi,[rsi+16]
	dec rax
	jnz colalop2

	add rcx,rbx
	add rsi,rdi

	dec rbp
	jnz colalop
	
	align 16
colexit:
	pop rdi
	pop rsi
	pop rbx
	pop rbp
	ret

