section .text

global LanczosResizer16_C8_horizontal_filter_pa
global LanczosResizer16_C8_horizontal_filter
global LanczosResizer16_C8_vertical_filter
global LanczosResizer16_C8_expand
global LanczosResizer16_C8_expand_pa
global LanczosResizer16_C8_collapse
global LanczosResizer16_C8_imgcopy
global LanczosResizer16_C8_imgcopy_pa

;void LanczosResizer16_C8_horizontal_filter_pa(UInt8 *inPt, UInt8 *outPt, OSInt dwidth, OSInt height, OSInt tap, OSInt *index, Int64 *weight, OSInt sstep, OSInt dstep, UInt8 *rgbTable, OSInt swidth, UInt8 *tmpbuff)
;0 rdi
;8 rsi
;16 rbx
;24 rbp
;32 retAddr
;rcx inPt r10
;rdx outPt r11
;r8 dwidth
;r9 height
;72 tap
;80 index
;88 weight
;96 sstep
;104 dstep
;112 rgbTable
;120 swidth
;128 tmpbuff

	align 16
LanczosResizer16_C8_horizontal_filter_pa:
	push rbp
	push rbx
	push rsi
	push rdi
	mov r10,rcx
	mov r11,rdx
	
	shr qword [rsp+72],1 ;tap
	pxor xmm3,xmm3
	align 16
hfpalop:
	mov rsi,r10 ;inPt
	mov rbx,qword [rsp+112] ;rgbTable
	mov rdi,qword [rsp+128] ;tmpbuff

	mov rcx,qword [rsp+120] ;swidth
	align 16
hfpalop4:
	movq xmm7,[rsi]
	pextrw rdx,xmm7,0
	movq xmm1,[rbx+rdx*8+1310720]
	pextrw rdx,xmm7,1
	movq xmm0,[rbx+rdx*8+786432]
	pextrw rdx,xmm7,2
	paddsw xmm1,xmm0
	movq xmm0,[rbx+rdx*8+262144]
	pextrw rdx,xmm7,3
	paddsw xmm1,xmm0

	movd xmm0,rdx
	punpcklwd xmm0, xmm0
	punpcklwd xmm0, xmm0
	pmulhuw xmm0, xmm1
	movq xmm1, [rbx+rdx*8+1835008]
	paddsw xmm0,xmm1
	movq [rdi],xmm0

	lea rsi,[rsi+8]
	lea rdi,[rdi+8]
	dec rcx
	jnz hfpalop4

	mov rsi,qword [rsp+128] ;tmpbuff
	mov rdi,r11 ;outPt

	mov rcx,r8 ;dwidth

	mov rbx,qword [rsp+80] ;index
	mov rbp,qword [rsp+88] ;weight
	align 16
hfpalop2:
	mov rdx,qword [rsp+72] ;tap
	pxor xmm2,xmm2
	ALIGN 16
hfpalop3:
	mov rax,qword [rbx]
	movq xmm0,[rsi+rax]
	mov rax,qword [rbx+8]
	movq xmm1,[rsi+rax]
	punpcklwd xmm0,xmm1
	pmaddwd xmm0,[rbp]
	paddd xmm2,xmm0
	
	lea rbp,[rbp+16]
	lea rbx,[rbx+16]
	dec rdx
	jnz hfpalop3

	psrad xmm2,15
	packssdw xmm2,xmm3
	movq [rdi],xmm2
	lea rdi,[rdi+8]
	dec rcx
	jnz hfpalop2

	add r10,qword [rsp+96] ;sstep inPt
	add r11,qword [rsp+104] ;dstep outPt

	dec r9 ;currHeight
	jnz hfpalop
	pop rdi
	pop rsi
	pop rbx
	pop rbp
	ret

;void LanczosResizer16_C8_horizontal_filter(UInt8 *inPt, UInt8 *outPt, OSInt dwidth, OSInt height, OSInt tap, OSInt *index, Int64 *weight, OSInt sstep, OSInt dstep, UInt8 *rgbTable, OSInt swidth, UInt8 *tmpbuff)
;0 rdi
;8 rsi
;16 rbx
;24 rbp
;32 retAddr
;rcx inPt r10
;rdx outPt r11
;r8 dwidth
;r9 height
;72 tap
;80 index
;88 weight
;96 sstep
;104 dstep
;112 rgbTable
;120 swidth
;128 tmpbuff

	align 16
LanczosResizer16_C8_horizontal_filter:
	push rbp
	push rbx
	push rsi
	push rdi
	mov r10,rcx
	mov r11,rdx
	
	test r8,1 ;dwidth
	jnz hf_1start
	test r11,15 ;outPt
	jnz hf_1start
	test qword [rsp+104],15 ;dstep
	jnz hf_1start
	
	shr qword [rsp+72],1 ;tap
	pxor xmm3,xmm3
	cmp qword [rsp+72],8
	jz hf8_2start
	jmp hf_1lop

	align 16
hf_1start:
	shr qword [rsp+72],1 ;tap
	pxor xmm3,xmm3
	cmp qword [rsp+72],8
	jz hf8_1lop
	
	align 16
hf_1lop:
	mov rsi,r10 ;inPt
	mov rbx,qword [rsp+112] ;rgbTable
	mov rdi,qword [rsp+128] ;tmpbuff

	mov rcx,qword [rsp+120] ;swidth
	align 16
hf_1lop4:
	movq xmm7,[rsi]
	pextrw rdx,xmm7,0
	movq xmm1, [rbx+rdx*8+1310720]
	pextrw rdx,xmm7,1
	movq xmm0, [rbx+rdx*8+786432]
	pextrw rdx,xmm7,2
	paddsw xmm1,xmm0
	movq xmm0, [rbx+rdx*8+262144]
	paddsw xmm1,xmm0
	pextrw rdx,xmm7,3
	movq xmm0, [rbx+rdx*8+1835008]
	paddsw xmm1,xmm0
	movq [rdi],xmm1

	lea rsi,[rsi+8]
	lea rdi,[rdi+8]
	dec rcx
	jnz hf_1lop4

	mov rdi,r11 ;outPt
	mov rcx,r8 ;dwidth
	mov rbx,qword [rsp+80] ;index
	mov rbp,qword [rsp+88] ;weight
	mov rsi,qword [rsp+128] ;tmpbuff
	align 16
hf_1lop2:
	mov rdx,qword [rsp+72] ;tap
	pxor xmm2,xmm2
	align 16
hf_1lop3:
	mov rax,qword [rbx]
	movq xmm0,[rsi+rax]
	mov rax,qword [rbx+8]
	movq xmm1,[rsi+rax]
	punpcklwd xmm0,xmm1
	pmaddwd xmm0,[rbp]
	paddd xmm2,xmm0
	
	lea rbp,[rbp+16]
	lea rbx,[rbx+16]
	dec rdx
	jnz hf_1lop3

	psrad xmm2,15
	packssdw xmm2,xmm3
	movq [rdi],xmm2
	lea rdi,[rdi+8]
	dec rcx
	jnz hf_1lop2

	add r10,qword [rsp+96] ;sstep inPt
	add r11,qword [rsp+104] ;dstep outPt

	dec r9 ;currHeight
	jnz hf_1lop
	jmp hfexit
	
	align 16
hf8_1lop:
	mov rsi,r10 ;inPt
	mov rbx,qword [rsp+112] ;rgbTable
	mov rdi,qword [rsp+128] ;tmpbuff

	mov rcx,qword [rsp+120] ;swidth
	align 16
hf8_1lop4:
	movq xmm7,[rsi]
	pextrw rdx,xmm7,0
	movq xmm1, [rbx+rdx*8+1310720]
	pextrw rdx,xmm7,1
	movq xmm0, [rbx+rdx*8+786432]
	pextrw rdx,xmm7,2
	paddsw xmm1,xmm0
	movq xmm0, [rbx+rdx*8+262144]
	paddsw xmm1,xmm0
	pextrw rdx,xmm7,3
	movq xmm0, [rbx+rdx*8+1835008]
	paddsw xmm1,xmm0
	movq [rdi],xmm1

	lea rsi,[rsi+8]
	lea rdi,[rdi+8]
	dec rcx
	jnz hf8_1lop4

	mov rdi,r11 ;outPt
	mov rcx,r8 ;dwidth
	mov rbx,qword [rsp+80] ;index
	mov rbp,qword [rsp+88] ;weight
	mov rsi,qword [rsp+128] ;tmpbuff
	
	align 16
hf8_1lop2:
	mov rax,qword [rbx]
	mov rdx,qword [rbx+8]
	movq xmm0,[rsi+rax]
	movq xmm1,[rsi+rdx]
	punpcklwd xmm0,xmm1
	pmaddwd xmm0,[rbp]
	movdqa xmm2,xmm0

	mov rax,qword [rbx+16]
	mov rdx,qword [rbx+24]
	movq xmm0,[rsi+rax]
	movq xmm1,[rsi+rdx]
	punpcklwd xmm0,xmm1
	pmaddwd xmm0,[rbp+16]
	paddd xmm2,xmm0

	mov rax,qword [rbx+32]
	mov rdx,qword [rbx+40]
	movq xmm0,[rsi+rax]
	movq xmm1,[rsi+rdx]
	punpcklwd xmm0,xmm1
	pmaddwd xmm0,[rbp+32]
	paddd xmm2,xmm0

	mov rax,qword [rbx+48]
	mov rdx,qword [rbx+56]
	movq xmm0,[rsi+rax]
	movq xmm1,[rsi+rdx]
	punpcklwd xmm0,xmm1
	pmaddwd xmm0,[rbp+48]
	paddd xmm2,xmm0
	
	mov rax,qword [rbx+64]
	mov rdx,qword [rbx+72]
	movq xmm0,[rsi+rax]
	movq xmm1,[rsi+rdx]
	punpcklwd xmm0,xmm1
	pmaddwd xmm0,[rbp+64]
	paddd xmm2,xmm0

	mov rax,qword [rbx+80]
	mov rdx,qword [rbx+88]
	movq xmm0,[rsi+rax]
	movq xmm1,[rsi+rdx]
	punpcklwd xmm0,xmm1
	pmaddwd xmm0,[rbp+80]
	paddd xmm2,xmm0
	
	mov rax,qword [rbx+96]
	mov rdx,qword [rbx+104]
	movq xmm0,[rsi+rax]
	movq xmm1,[rsi+rdx]
	punpcklwd xmm0,xmm1
	pmaddwd xmm0,[rbp+96]
	paddd xmm2,xmm0

	mov rax,qword [rbx+112]
	mov rdx,qword [rbx+120]
	movq xmm0,[rsi+rax]
	movq xmm1,[rsi+rdx]
	punpcklwd xmm0,xmm1
	pmaddwd xmm0,[rbp+112]
	paddd xmm2,xmm0
	
	lea rbp,[rbp+128]
	lea rbx,[rbx+128]

	psrad xmm2,15
	packssdw xmm2,xmm3
	movq [rdi],xmm2
	lea rdi,[rdi+8]
	dec rcx
	jnz hf8_1lop2

	add r10,qword [rsp+96] ;sstep inPt
	add r11,qword [rsp+104] ;dstep outPt

	dec r9 ;currHeight
	jnz hf8_1lop
	jmp hfexit
	
	align 16
hf8_2start:
	shr r8,1
	
	align 16
hf8_2lop:
	mov rsi,r10 ;inPt
	mov rbx,qword [rsp+112] ;rgbTable
	mov rdi,qword [rsp+128] ;tmpbuff

	mov rcx,qword [rsp+120] ;swidth
	align 16
hf8_2lop4:
	movq xmm7,[rsi]
	pextrw rdx,xmm7,0
	movq xmm1, [rbx+rdx*8+1310720]
	pextrw rdx,xmm7,1
	movq xmm0, [rbx+rdx*8+786432]
	pextrw rdx,xmm7,2
	paddsw xmm1,xmm0
	movq xmm0, [rbx+rdx*8+262144]
	paddsw xmm1,xmm0
	pextrw rdx,xmm7,3
	movq xmm0, [rbx+rdx*8+1835008]
	paddsw xmm1,xmm0
	movq [rdi],xmm1

	lea rsi,[rsi+8]
	lea rdi,[rdi+8]
	dec rcx
	jnz hf8_2lop4

	mov rdi,r11 ;outPt
	mov rcx,r8 ;dwidth
	mov rbx,qword [rsp+80] ;index
	mov rbp,qword [rsp+88] ;weight
	mov rsi,qword [rsp+128] ;tmpbuff
	
	align 16
hf8_2lop2:
	mov rax,qword [rbx]
	mov rdx,qword [rbx+8]
	movq xmm0,[rsi+rax]
	movq xmm1,[rsi+rdx]
	punpcklwd xmm0,xmm1
	pmaddwd xmm0,[rbp]
	movdqa xmm2,xmm0

	mov rax,qword [rbx+16]
	mov rdx,qword [rbx+24]
	movq xmm0,[rsi+rax]
	movq xmm1,[rsi+rdx]
	punpcklwd xmm0,xmm1
	pmaddwd xmm0,[rbp+16]
	paddd xmm2,xmm0

	mov rax,qword [rbx+32]
	mov rdx,qword [rbx+40]
	movq xmm0,[rsi+rax]
	movq xmm1,[rsi+rdx]
	punpcklwd xmm0,xmm1
	pmaddwd xmm0,[rbp+32]
	paddd xmm2,xmm0

	mov rax,qword [rbx+48]
	mov rdx,qword [rbx+56]
	movq xmm0,[rsi+rax]
	movq xmm1,[rsi+rdx]
	punpcklwd xmm0,xmm1
	pmaddwd xmm0,[rbp+48]
	paddd xmm2,xmm0
	
	mov rax,qword [rbx+64]
	mov rdx,qword [rbx+72]
	movq xmm0,[rsi+rax]
	movq xmm1,[rsi+rdx]
	punpcklwd xmm0,xmm1
	pmaddwd xmm0,[rbp+64]
	paddd xmm2,xmm0

	mov rax,qword [rbx+80]
	mov rdx,qword [rbx+88]
	movq xmm0,[rsi+rax]
	movq xmm1,[rsi+rdx]
	punpcklwd xmm0,xmm1
	pmaddwd xmm0,[rbp+80]
	paddd xmm2,xmm0
	
	mov rax,qword [rbx+96]
	mov rdx,qword [rbx+104]
	movq xmm0,[rsi+rax]
	movq xmm1,[rsi+rdx]
	punpcklwd xmm0,xmm1
	pmaddwd xmm0,[rbp+96]
	paddd xmm2,xmm0

	mov rax,qword [rbx+112]
	mov rdx,qword [rbx+120]
	movq xmm0,[rsi+rax]
	movq xmm1,[rsi+rdx]
	punpcklwd xmm0,xmm1
	pmaddwd xmm0,[rbp+112]
	paddd xmm2,xmm0
	
	lea rbp,[rbp+128]
	lea rbx,[rbx+128]

	mov rax,qword [rbx]
	mov rdx,qword [rbx+8]
	movq xmm0,[rsi+rax]
	movq xmm1,[rsi+rdx]
	punpcklwd xmm0,xmm1
	pmaddwd xmm0,[rbp]
	movdqa xmm4,xmm0

	mov rax,qword [rbx+16]
	mov rdx,qword [rbx+24]
	movq xmm0,[rsi+rax]
	movq xmm1,[rsi+rdx]
	punpcklwd xmm0,xmm1
	pmaddwd xmm0,[rbp+16]
	paddd xmm4,xmm0

	mov rax,qword [rbx+32]
	mov rdx,qword [rbx+40]
	movq xmm0,[rsi+rax]
	movq xmm1,[rsi+rdx]
	punpcklwd xmm0,xmm1
	pmaddwd xmm0,[rbp+32]
	paddd xmm4,xmm0

	mov rax,qword [rbx+48]
	mov rdx,qword [rbx+56]
	movq xmm0,[rsi+rax]
	movq xmm1,[rsi+rdx]
	punpcklwd xmm0,xmm1
	pmaddwd xmm0,[rbp+48]
	paddd xmm4,xmm0
	
	mov rax,qword [rbx+64]
	mov rdx,qword [rbx+72]
	movq xmm0,[rsi+rax]
	movq xmm1,[rsi+rdx]
	punpcklwd xmm0,xmm1
	pmaddwd xmm0,[rbp+64]
	paddd xmm4,xmm0

	mov rax,qword [rbx+80]
	mov rdx,qword [rbx+88]
	movq xmm0,[rsi+rax]
	movq xmm1,[rsi+rdx]
	punpcklwd xmm0,xmm1
	pmaddwd xmm0,[rbp+80]
	paddd xmm4,xmm0
	
	mov rax,qword [rbx+96]
	mov rdx,qword [rbx+104]
	movq xmm0,[rsi+rax]
	movq xmm1,[rsi+rdx]
	punpcklwd xmm0,xmm1
	pmaddwd xmm0,[rbp+96]
	paddd xmm4,xmm0

	mov rax,qword [rbx+112]
	mov rdx,qword [rbx+120]
	movq xmm0,[rsi+rax]
	movq xmm1,[rsi+rdx]
	punpcklwd xmm0,xmm1
	pmaddwd xmm0,[rbp+112]
	paddd xmm4,xmm0
	
	lea rbp,[rbp+128]
	lea rbx,[rbx+128]

	psrad xmm2,15
	psrad xmm4,15
	packssdw xmm2,xmm4
	movdqa [rdi],xmm2
	lea rdi,[rdi+16]
	dec rcx
	jnz hf8_2lop2

	add r10,qword [rsp+96] ;sstep inPt
	add r11,qword [rsp+104] ;dstep outPt

	dec r9 ;currHeight
	jnz hf8_2lop
	

	align 16
hfexit:
	pop rdi
	pop rsi
	pop rbx
	pop rbp
	ret

;void LanczosResizer16_C8_vertical_filter(UInt8 *inPt, UInt8 *outPt, OSInt dwidth, OSInt height, OSInt tap, OSInt *index, Int64 *weight, OSInt sstep, OSInt dstep, UInt8 *rgbTable)
;0 rdi
;8 rsi
;16 rbx
;24 rbp
;32 retAddr
;rcx inPt r10
;rdx outPt r11
;r8 dwidth
;r9 height
;72 tap
;80 index
;88 weight
;96 sstep
;104 dstep
;112 rgbTable

	align 16
LanczosResizer16_C8_vertical_filter:
	push rbp
	push rbx
	push rsi
	push rdi
	mov r10,rcx
	mov r11,rdx
	mov rax,r8 ;dwidth
	lea rdx,[rax*4]
	sub qword [rsp+104],rdx ;dstep
	test rax,1
	jz vfstart								;if (dwidth & 1)

	shr qword [rsp+72],1 ;tap
	mov rdi,r11 ;outPt
	pxor xmm3,xmm3
	align 16
vflop:
	mov rsi,r10 ;inPt
	mov rbp,r8 ;dwidth

	align 16
vflop2:
	mov rbx,qword [rsp+80] ;index
	mov rdx,qword [rsp+88] ;weight

	mov rcx,qword [rsp+72] ;tap
	pxor xmm2,xmm2
	ALIGN 16
vflop3:
	mov rax,qword [rbx]
	movq xmm0,[rsi+rax]
	mov rax,qword [rbx+8]
	movq xmm1,[rsi+rax]
	punpcklwd xmm0, xmm1
	pmaddwd xmm0,[rdx]
	paddd xmm2,xmm0
	lea rbx,[rbx+16]
	lea rdx,[rdx+16]

	dec rcx
	jnz vflop3

	mov rbx,qword [rsp+112] ;rgbTable

	psrad xmm2,15
	packssdw xmm2,xmm3
	pextrw rdx,xmm2,2
	mov al,byte [rbx+rdx+131072]
	pextrw rdx,xmm2,3
	mov ah,byte [rbx+rdx+196608]
	shl eax,16
	pextrw rdx,xmm2,0
	mov al,byte [rbx+rdx]
	pextrw rdx,xmm2,1
	mov ah,byte [rbx+rdx+65536]

	movnti dword [rdi],eax
	lea rsi,[rsi+8]
	lea rdi,[rdi+4]
	dec rbp
	jnz vflop2

	mov rax,qword [rsp+72] ;tap
	shl rax,4
	add qword [rsp+80],rax ;index
	add qword [rsp+88],rax ;weight

	add rdx,qword [rsp+104] ;dstep

	dec r9 ;currHeight
	jnz vflop
	jmp vfexit

	align 16
vfstart:
	cmp qword [rsp+72],6 ;tap
	jz vf6start
	shr qword [rsp+72],1 ;tap
	shr r8,1 ;dwidth
	mov rdi,r11 ;outPt
	pxor xmm3,xmm3
	align 16
vflop4:
	mov rsi,r10 ;inPt
	mov rbp,r8 ;dwidth
	align 16
vflop5:
	mov rbx,qword [rsp+80] ;index
	mov rdx,qword [rsp+88] ;weight

	mov rcx,qword [rsp+72] ;tap
	pxor xmm2,xmm2
	pxor xmm6,xmm6
	ALIGN 16
vflop6:
	mov rax,qword [rbx]
	movdqu xmm0,[rsi+rax]
	mov rax,qword [rbx+8]
	movdqu xmm1,[rsi+rax]
	movdqu xmm4,xmm0
	punpcklwd xmm0, xmm1
	punpckhwd xmm4, xmm1
	movdqa xmm5,[rdx]
	pmaddwd xmm0,xmm5
	pmaddwd xmm4,xmm5
	paddd xmm2,xmm0
	paddd xmm6,xmm4
	lea rdx,[rdx+16]
	lea rbx,[rbx+16]

	dec rcx
	jnz vflop6

	mov rbx,qword [rsp+112] ;rgbTable

	psrad xmm6,15
	packssdw xmm6,xmm3
	pextrw rdx,xmm6,2
	mov al,byte [rbx+rdx+131072]
	pextrw rdx,xmm6,3
	mov ah,byte [rbx+rdx+196608]
	shl eax,16
	pextrw rdx,xmm6,0
	mov al,byte [rbx+rdx]
	pextrw rdx,xmm6,1
	mov ah,byte [rbx+rdx+65536]
	movd xmm0,eax

	psrad xmm2,15
	packssdw xmm2,xmm3
	pextrw rdx,xmm2,2
	mov al,byte [rbx+rdx+131072]
	pextrw rdx,xmm2,3
	mov ah,byte [rbx+rdx+196608]
	shl eax,16
	pextrw rdx,xmm2,0
	mov al,byte [rbx+rdx]
	pextrw rdx,xmm2,1
	mov ah,byte [rbx+rdx+65536]
	movd xmm1,eax
	punpckldq xmm1,xmm0

	movq [rdi],xmm1
	lea rsi,[rsi+16]
	lea rdi,[rdi+8]
	dec rbp
	jnz vflop5

	mov rax,qword [rsp+72] ;tap
	shl rax,4
	add qword [rsp+80],rax ;index
	add qword [rsp+88],rax ;weight

	add rdi,qword [rsp+104] ;dstep

	dec r9 ;currHeight
	jnz vflop4
	jmp vfexit

;0 xmm6
;16 xmm7
;32 r15
;40 r14
;48 r13
;56 r12
;64 rdi
;72 rsi
;80 rbx
;88 rbp
;96 retAddr
;rcx inPt r10
;rdx outPt r11
;r8 dwidth
;128 r9 height
;136 tap
;144 index
;152 weight
;160 sstep
;168 dstep
;176 rgbTable
	align 16
vf6start:
	push r12
	push r13
	push r14
	push r15
	sub rsp,32
	movdqu [rsp],xmm6
	movdqu [rsp+16],xmm7
	mov [rsp+128],r9
	shr r8,1 ;dwidth
	mov rdi,r11 ;outPt
	align 16
vf6lop4:
	mov rsi,r10 ;inPt
	mov rbp,r8 ;dwidth
	mov rdx,qword [rsp+152] ;weight
	movdqa xmm3,[rdx]
	movdqa xmm5,[rdx+16]
	movdqa xmm7,[rdx+32]
	mov rdx,qword [rsp+144] ;index
	mov rcx,qword [rdx]
	mov r9,qword [rdx+8]
	mov r12,qword [rdx+16]
	mov r13,qword [rdx+24]
	mov r14,qword [rdx+32]
	mov r15,qword [rdx+40]
	align 16
vf6lop5:
	movdqu xmm0,[rsi+rcx]
	movdqu xmm1,[rsi+r9]
	movdqu xmm4,xmm0
	punpcklwd xmm0, xmm1
	punpckhwd xmm4, xmm1
	pmaddwd xmm0,xmm3
	pmaddwd xmm4,xmm3
	movdqa xmm2,xmm0
	movdqa xmm6,xmm4

	movdqu xmm0,[rsi+r12]
	movdqu xmm1,[rsi+r13]
	movdqu xmm4,xmm0
	punpcklwd xmm0, xmm1
	punpckhwd xmm4, xmm1
	pmaddwd xmm0,xmm5
	pmaddwd xmm4,xmm5
	paddd xmm2,xmm0
	paddd xmm6,xmm4

	movdqu xmm0,[rsi+r14]
	movdqu xmm1,[rsi+r15]
	movdqu xmm4,xmm0
	punpcklwd xmm0, xmm1
	punpckhwd xmm4, xmm1
	pmaddwd xmm0,xmm7
	pmaddwd xmm4,xmm7
	paddd xmm2,xmm0
	paddd xmm6,xmm4

	mov rbx,qword [rsp+112] ;rgbTable
	pxor xmm4,xmm4

	psrad xmm6,15
	packssdw xmm6,xmm4
	pextrw rdx,xmm6,2
	mov al,byte [rbx+rdx+131072]
	pextrw rdx,xmm6,3
	mov ah,byte [rbx+rdx+196608]
	shl eax,16
	pextrw rdx,xmm6,0
	mov al,byte [rbx+rdx]
	pextrw rdx,xmm6,1
	mov ah,byte [rbx+rdx+65536]
	movd xmm0,eax

	psrad xmm2,15
	packssdw xmm2,xmm4
	pextrw rdx,xmm2,2
	mov al,byte [rbx+rdx+131072]
	pextrw rdx,xmm2,3
	mov ah,byte [rbx+rdx+196608]
	shl eax,16
	pextrw rdx,xmm2,0
	mov al,byte [rbx+rdx]
	pextrw rdx,xmm2,1
	mov ah,byte [rbx+rdx+65536]
	movd xmm1,eax
	punpckldq xmm1,xmm0

	movq [rdi],xmm1
	lea rsi,[rsi+16]
	lea rdi,[rdi+8]
	dec rbp
	jnz vf6lop5

	add qword [rsp+80],48 ;index
	add qword [rsp+88],48 ;weight
	add rdi,qword [rsp+104] ;dstep

	dec qword [rsp+128] ;currHeight
	jnz vf6lop4
	movdqu xmm6,[rsp]
	movdqu xmm7,[rsp+16]
	add rsp,32
	pop r15
	pop r14
	pop r13
	pop r12
;	jmp vfexit
	
	align 16
vfexit:
	pop rdi
	pop rsi
	pop rbx
	pop rbp
	ret

;void LanczosResizer16_C8_expand(UInt8 *inPt, UInt8 *outPt, OSInt width, OSInt height, OSInt sstep, OSInt dstep, UInt8 *rgbTable)
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
;88 rgbTable

	align 16
LanczosResizer16_C8_expand:
	push rbp
	push rbx
	push rsi
	push rdi
	mov rsi,rcx ;inPt
	mov rdi,rdx ;outPt
	mov rax,qword [rsp+56]
	shl rax,3
	sub qword [rsp+72],rax ;sstep
	sub qword [rsp+80],rax ;dstep

	mov rbp,r9 ;height
	mov rbx,qword [rsp+88] ;rgbTable
	align 16
explop:

	mov rcx,r8 ;width
	ALIGN 16
explop2:
	movq xmm7,[rcx]
	pextrw rdx,xmm7,0
	movq xmm1,[rbx+rdx*8+1310720]
	pextrw rdx,xmm7,1
	movq xmm0,[rbx+rdx*8+786432]
	pextrw rdx,xmm7,2
	paddsw xmm1,xmm0
	movq xmm0,[rbx+rdx*8+262144]
	paddsw xmm1,xmm0
	pextrw rdx,xmm7,3
	movq xmm0,[rbx+rdx*8+1835008]
	paddsw xmm1,xmm0
	movq [rdi],xmm1

	lea rsi,[rsi+8]
	lea rdi,[rdi+8]
	dec rcx
	jnz explop2

	add rsi,qword [rsp+72] ;sstep
	add rdi,qword [rsp+80] ;dstep

	dec rbp
	jnz explop
	pop rdi
	pop rsi
	pop rbx
	pop rbp
	ret

;void LanczosResizer16_C8_expand_pa(UInt8 *inPt, UInt8 *outPt, OSInt width, OSInt height, OSInt sstep, OSInt dstep, UInt8 *rgbTable)
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
;88 rgbTable

	align 16
LanczosResizer16_C8_expand_pa:
	push rbp
	push rbx
	push rsi
	push rdi
	mov rsi,rcx ;inPt
	mov rdi,rdx ;outPt
	mov rax,r8 ;width
	shl rax,3
	sub qword [rsp+72],rax ;sstep
	sub qword [rsp+80],rax ;dstep

	mov rbp,r9 ;height
	mov rbx,qword [rsp+88] ;rgbTable
	align 16
exppalop:

	mov rcx,r8 ;width
	ALIGN 16
exppalop2:
	movq xmm7,[rsi]
	pextrw rdx,xmm7,0
	movq xmm1,[rbx+rdx*8+1310720]
	pextrw rdx,xmm7,1
	movq xmm0,[rbx+rdx*8+786432]
	pextrw rdx,xmm7,2
	paddsw xmm1,xmm0
	movq xmm0,[rbx+rdx*8+262144]
	paddsw xmm1,xmm0
	pextrw rdx,xmm7,3

	movd xmm0,rdx
	punpcklwd xmm0, xmm0
	punpcklwd xmm0, xmm0
	pmulhuw xmm0, xmm1
	movq xmm1,[rbx+rdx*8+1835008]
	paddsw xmm0,xmm1
	movq [rdi],xmm0

	lea rsi,[rsi+8]
	lea rdi,[rdi+8]
	dec rcx
	jnz exppalop2

	add rsi,qword [rsp+72] ;sstep
	add rdi,qword [rsp+80] ;dstep

	dec rbp
	jnz exppalop
	pop rdi
	pop rsi
	pop rbx
	pop rbp
	ret

;void LanczosResizer16_C8_collapse(UInt8 *inPt, UInt8 *outPt, OSInt width, OSInt height, OSInt sstep, OSInt dstep, UInt8 *rgbTable)
;0 edi
;8 esi
;16 ebx
;24 ebp
;32 retAddr
;rcx inPt
;rdx outPt
;r8 width
;r9 height
;72 sstep
;80 dstep
;88 rgbTable

	align 16
LanczosResizer16_C8_collapse:
	push rbp
	push rbx
	push rsi
	push rdi
	
	mov rsi,rcx ;inPt
	mov rdi,rdx ;outPt
	mov rax,r8 ;width
	lea rdx,[rax*8]
	shl rax,2
	sub qword [rsp+72],rdx ;sstep
	sub qword [rsp+80],rax ;dstep
	mov rbp,r9 ;height
	mov rbx,qword [rsp+88] ;rgbTable
	align 16
collop:
	mov rcx,r8 ;width
	ALIGN 16
collop2:
	movzx rdx,word [rsi+6]
	mov ah,byte [rbx+rdx+196608]
	movzx rdx,word [rsi+4]
	mov al,byte [rbx+rdx+131072]
	shl eax,16
	movzx rdx,word [rsi+2]
	mov ah,byte [rbx+rdx+65536]
	movzx rdx,word [rsi]
	mov al,byte [rbx+rdx]

	movnti dword [rdi],eax
	lea rsi,[rsi+8]
	add rdi,[rdi+4]
	dec rcx
	jnz collop2

	add rsi,qword [rsp+72] ;sstep
	add rdi,qword [rsp+80] ;dstep

	dec rbp
	jnz collop
	pop rdi
	pop rsi
	pop rbx
	pop rbp
	ret

;void LanczosResizer16_C8_imgcopy(UInt8 *inPt, UInt8 *outPt, OSInt width, OSInt height, OSInt sstep, OSInt dstep, UInt8 *rgbTable)
;0 edi
;8 esi
;16 ebx
;24 ebp
;32 retAddr
;rcx inPt
;rdx outPt
;r8 width
;r9 height
;72 sstep
;80 dstep
;88 rgbTable

	align 16
LanczosResizer16_C8_imgcopy:
	push rbp
	push rbx
	push rsi
	push rdi

	mov rsi,rcx ;inPt
	mov rdi,rdx ;outPt
	mov rax,r8 ;width
	lea rdx,[rax*8]
	shl rax,2
	sub qword [rsp+72],rdx ;sstep
	sub qword [rsp+80],rax ;dstep
	mov rbp,r9 ;height
	mov rbx,qword [rsp+88] ;rgbTable
	align 16
iclop:
	mov rcx,r8 ;width
	ALIGN 16
iclop2:
	movq xmm7,[rsi]
	pextrw rdx,xmm7,0
	movq xmm1,[rbx+rdx*8+1310720]
	pextrw rdx,xmm7,1
	movq xmm0,[rbx+rdx*8+786432]
	pextrw rdx,xmm7,2
	paddsw xmm1,xmm0
	movq xmm0,[rbx+rdx*8+262144]
	paddsw xmm1,xmm0
	pextrw rdx,xmm1,2
	pextrw rax,xmm7,3
	mov al,byte [rbx+rdx+131072]
	shl eax,16
	pextrw rdx,xmm1,1
	mov ah,byte [rbx+rdx+65536]
	pextrw rdx,xmm1,0
	mov al,byte [rbx+rdx]
	movnti dword [rdi],eax
	lea rsi,[rsi+8]
	lea rdi,[rdi+4]
	dec rcx
	jnz iclop2

	add rsi,qword [rsp+72] ;sstep
	add rdi,qword [rsp+80] ;dstep
	dec rbp
	jnz iclop
	pop rdi
	pop rsi
	pop rbx
	pop rbp
	ret

;void LanczosResizer16_C8_imgcopy_pa(UInt8 *inPt, UInt8 *outPt, OSInt width, OSInt height, OSInt sstep, OSInt dstep, UInt8 *rgbTable)
;0 edi
;8 esi
;16 ebx
;24 ebp
;32 retAddr
;rcx inPt
;rdx outPt
;r8 width
;r9 height
;72 sstep
;80 dstep
;88 rgbTable

	align 16
LanczosResizer16_C8_imgcopy_pa:
	push rbp
	push rbx
	push rsi
	push rdi
	mov rsi,rcx ;inPt
	mov rdi,rdx ;outPt
	mov rax,r8 ;width
	lea rdx,[rax*8]
	lea rcx,[rax*4]
	shr rax,1
	sub qword [rsp+72],rdx ;sstep
	sub qword [rsp+80],rcx ;dstep
	mov r8,rax ;width			width = width >> 1;

	mov rbx,qword [rsp+88] ;rgbTable
	mov rbp,r9 ;height
	align 16
icpalop:
	mov rcx,r8 ;width
	ALIGN 16
icpalop2:
	movdqu xmm7,[rsi]
	pextrw rdx,xmm7,0
	movq xmm1,[rbx+rdx*8+1310720]
	pextrw rdx,xmm7,1
	movq xmm0,[rbx+rdx*8+786432]
	paddsw xmm1,xmm0
	pextrw rdx,xmm7,2
	movq xmm0,[rbx+rdx*8+262144]
	paddsw xmm1,xmm0

	pextrw rdx,xmm7,4
	movq xmm2,[rbx+rdx*8+1310720]
	pextrw rdx,xmm7,5
	movq xmm0,[rbx+rdx*8+786432]
	paddsw xmm2,xmm0
	pextrw rdx,xmm7,6
	movq xmm0,[rbx+rdx*8+262144]
	paddsw xmm2,xmm0
	punpcklqdq xmm1,xmm2
	pextrw rax,xmm7,3
	pextrw rdx,xmm7,7
	shl edx,16
	or edx,eax
	movd xmm3,edx
	punpcklwd xmm3, xmm3
	punpckldq xmm3, xmm3
	pmulhuw xmm1, xmm3

	pextrw rax,xmm1,2
	movzx edx,byte [rbx+rax+131072]
	mov dh,byte [rsi+7]
	pextrw rax,xmm1,1
	shl edx,16
	mov dh,byte [rbx+rax+65536]
	pextrw rax,xmm1,0
	mov dl,byte [rbx+rax]
	movnti dword [rdi],edx

	pextrw rax,xmm1,6
	movzx edx,byte [rbx+rax+131072]
	mov dh,byte [rsi+15]
	pextrw rax,xmm1,5
	shl edx,16
	mov dh,byte [rbx+rax+65536]
	pextrw rax,xmm1,4
	mov dl,byte [rbx+rax]
	movnti dword [rdi+4],edx

	lea rsi,[rsi+16]
	lea rdi,[rdi+8]
	dec rcx
	jnz icpalop2

	add rsi,qword [rsp+72] ;sstep
	add rdi,qword [rsp+80] ;dstep
	dec rbp
	jnz icpalop
	
	pop rdi
	pop rsi
	pop rbx
	pop rbp
	ret
 