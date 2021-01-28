section .text

global LanczosResizer8_C8_horizontal_filter_pa
global LanczosResizer8_C8_horizontal_filter
global LanczosResizer8_C8_vertical_filter
global LanczosResizer8_C8_expand
global LanczosResizer8_C8_expand_pa
global LanczosResizer8_C8_collapse
global LanczosResizer8_C8_imgcopy
global LanczosResizer8_C8_imgcopy_pa

;void LanczosResizer8_C8_horizontal_filter_pa(UInt8 *inPt, UInt8 *outPt, OSInt dwidth, OSInt height, OSInt tap, OSInt *index, Int64 *weight, OSInt sstep, OSInt dstep, UInt8 *rgbTable, OSInt swidth, UInt8 *tmpbuff)
;0 rdi
;8 rsi
;16 rbx
;24 rbp
;32 retAddr
;rcx inpt r10
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
LanczosResizer8_C8_horizontal_filter_pa:
	push rbp
	push rbx
	push rsi
	push rdi
	mov r10,rcx
	mov r11,rdx
	test r8,1 ;dwidth
	jnz hfpastart
	cmp qword [rsp+72],6 ;tap
	jz hfpa6start
	jmp hfpa2start

	align 16
hfpastart:							;if (dwidth & 1)
	shr qword [rsp+72],1 ;tap
	pxor xmm3,xmm3
	align 16
hfpalop:
	mov rcx,r10 ;inPt
	mov rdi,qword [rsp+128] ;tmpbuff
	mov rsi,qword [rsp+112] ;rgbTable

	mov rbp,qword [rsp+120] ;swidth
	shr rbp,1
	jnb hfpalop4

	mov eax,dword [rcx]
	movzx rdx,al
	movq xmm1, [rsi+rdx*8+266240]
	movzx edx,ah
	movq xmm0, [rsi+rdx*8+264192]
	shr eax,16
	paddsw xmm1,xmm0
	movzx edx,al
	movq xmm0, [rsi+rdx*8+262144]
	movzx edx,ah
	paddsw xmm1,xmm0

	movd xmm0,edx
	punpcklbw xmm0, xmm0
	punpcklbw xmm0, xmm0
	punpcklbw xmm0, xmm0
	psrlw xmm0,1
	pmulhw xmm0, xmm1
	psllw xmm0,1
	movq xmm1, [rsi+rdx*8+268288]
	paddsw xmm0,xmm1
	movq [rdi],xmm0

	lea rdi,[rdi+8]
	lea rcx,[rcx+4]

	align 16
hfpalop4:
	mov eax,dword [rcx]
	mov ebx,dword [rcx+4]
	movzx rdx,al
	movq xmm1, [rsi+rdx*8+266240]
	movzx edx,bl
	movq xmm2, [rsi+rdx*8+266240]
	movzx edx,ah
	punpcklqdq xmm1,xmm2
	movq xmm0, [rsi+rdx*8+264192]
	movzx edx,bh
	movq xmm2, [rsi+rdx*8+264192]
	shr eax,16
	shr ebx,16
	punpcklqdq xmm0,xmm2
	paddsw xmm1,xmm0
	movzx edx,al
	movq xmm0, [rsi+rdx*8+262144]
	movzx edx,bl
	movq xmm2, [rsi+rdx*8+262144]
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
	movq xmm1, [rsi+rdx*8+268288]
	movzx edx,bh
	movq xmm2, [rsi+rdx*8+268288]
	punpcklqdq xmm1,xmm2
	paddsw xmm0,xmm1
	movdqu [rdi],xmm0

	lea rdi,[rdi+16]
	lea rcx,[rcx+8]
	dec rbp
	jnz hfpalop4

	mov rsi,r11 ;outPt
	mov rbp,r8 ;dwidth
	mov rbx,qword [rsp+80] ;index
	mov rdi,qword [rsp+88] ;weight
	mov rcx,qword [rsp+128] ;tmpbuff
	align 16
hfpalop2:
	mov rdx,qword [rsp+72] ;tap
	pxor xmm2,xmm2

	ALIGN 16
hfpalop3:
	mov rax,qword [rbx]
	movq xmm0,[rcx+rax]
	mov rax,qword [rbx+8]
	movq xmm1,[rcx+rax]
	punpcklwd xmm0,xmm1
	pmaddwd xmm0,[rdi]
	paddd xmm2,xmm0
	
	lea rdi,[rdi+16]
	lea rbx,[rbx+16]
	dec rdx
	jnz hfpalop3

	psrad xmm2,15
	packssdw xmm2,xmm3
	movq [rsi],xmm2
	lea rsi,[rsi+8]
	dec rbp
	jnz hfpalop2

	add r10,qword [rsp+96] ;sstep
	add r11,qword [rsp+104] ;dstep
	dec r9 ;currHeight
	jnz hfpalop
	jmp hfpaexit

	align 16
hfpa6start:								;else if (tap == 6)
	shr r8,1 ;dwidth
	shr qword [rsp+72],1 ;tap
	align 16
hfpa6lop:
	mov rcx,r10 ;inPt
	mov rdi,qword [rsp+128] ;tmpbuff
	mov rsi,qword [rsp+112] ;rgbTable

	mov rbp,qword [rsp+120] ;swidth
	shr rbp,1
	jnb hfpa6lop4

	mov eax,dword [rcx]
	movzx rdx,al
	movq xmm1,[rsi+rdx*8+266240]
	movzx edx,ah
	movq xmm0,[rsi+rdx*8+264192]
	shr eax,16
	paddsw xmm1,xmm0
	movzx edx,al
	movq xmm0,[rsi+rdx*8+262144]
	movzx edx,ah
	paddsw xmm1,xmm0

	movd xmm0,edx
	punpcklbw xmm0, xmm0
	punpcklbw xmm0, xmm0
	punpcklbw xmm0, xmm0
	psrlw xmm0,1
	pmulhw xmm0, xmm1
	psllw xmm0,1
	movq xmm1,[rsi+rdx*8+268288]
	paddsw xmm0,xmm1
	movq [rdi],xmm0

	lea rdi,[rdi+8]
	lea rcx,[rcx+4]

	align 16
hfpa6lop4:
	mov eax,dword [rcx]
	mov ebx,dword [rcx+4]
	movzx rdx,al
	movq xmm1,[rsi+rdx*8+266240]
	movzx rdx,bl
	movq xmm2,[rsi+rdx*8+266240]
	movzx edx,ah
	punpcklqdq xmm1,xmm2
	movq xmm0,[rsi+rdx*8+264192]
	movzx edx,bh
	movq xmm2,[rsi+rdx*8+264192]
	shr eax,16
	shr ebx,16
	punpcklqdq xmm0,xmm2
	paddsw xmm1,xmm0
	movzx edx,al
	movq xmm0,[rsi+rdx*8+262144]
	movzx edx,bl
	movq xmm2,[rsi+rdx*8+262144]
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
	movq xmm1,[rsi+rdx*8+268288]
	movzx edx,bh
	movq xmm2,[rsi+rdx*8+268288]
	punpcklqdq xmm1,xmm2
	paddsw xmm0,xmm1
	movdqu [rdi],xmm0

	lea rdi,[rdi+16]
	lea rcx,[rcx+8]
	dec rbp
	jnz hfpa6lop4

	mov rsi,r11 ;outPt
	mov rbp,r8 ;dwidth
	mov rbx,qword [rsp+80] ;index
	mov rdi,qword [rsp+88] ;weight
	mov rcx,qword [rsp+128] ;tmpbuff
	align 16
hfpa6lop2:
	mov rax,qword [rbx]
	pxor xmm1,xmm1
	mov rdx,qword [rbx+8]
	movdqu xmm0,[rcx+rax]
	movdqa xmm4,xmm0
	movdqu xmm2,[rcx+rdx]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[rdi]
	punpckhwd xmm4,xmm2
	pmaddwd xmm4, [rdi+16]
	paddd xmm1,xmm0
	mov rax,qword [rbx+16]
	paddd xmm1,xmm4
	movdqu xmm0,[rcx+rax]
	pshufd xmm2,xmm0,14
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[rdi+32]
	paddd xmm1,xmm0

	mov rax,qword [rbx+24]
	pxor xmm3,xmm3
	mov rdx,qword [rbx+32]
	movdqu xmm0,[rcx+rax]
	movdqa xmm4,xmm0
	movdqu xmm2,[rcx+rdx]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[rdi+48]
	punpckhwd xmm4,xmm2
	pmaddwd xmm4,[rdi+64]
	paddd xmm3,xmm0
	mov eax,dword [rbx+40]
	paddd xmm3,xmm4
	movdqu xmm0,[rcx+rax]
	pshufd xmm2,xmm0,14
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[rdi+80]
	paddd xmm3,xmm0

	lea rdi,[rdi+96]
	lea rbx,[rbx+48]

	psrad xmm1,15
	psrad xmm3,15
	packssdw xmm1,xmm3
	movdqu [rsi],xmm1
	lea rsi,[rsi+16]
	dec rbp
	jnz hfpa6lop2

	add r10,qword [rsp+96] ;sstep
	add r11,qword [rsp+104] ;dstep

	dec r9 ;currHeight
	jnz hfpa6lop
	jmp hfpaexit

	align 16
hfpa2start:
	shr r8,1 ;dwidth
	shr qword [rsp+72],1 ;tap
	align 16
hfpa2lop:
	mov rcx,r10 ;inPt
	mov rsi,qword [rsp+112] ;rgbTable
	mov rbp,qword [rsp+120] ;swidth
	mov rdi,qword [rsp+128] ;tmpbuff

	shr rbp,1
	jnb hfpa2lop4

	mov eax,dword [rcx]
	movzx rdx,al
	movq xmm1,[rsi+rdx*8+266240]
	movzx edx,ah
	movq xmm0,[rsi+rdx*8+264192]
	shr eax,16
	paddsw xmm1,xmm0
	movzx edx,al
	movq xmm0,[rsi+rdx*8+262144]
	movzx edx,ah
	paddsw xmm1,xmm0

	movd xmm0,edx
	punpcklbw xmm0, xmm0
	punpcklbw xmm0, xmm0
	punpcklbw xmm0, xmm0
	psrlw xmm0,1
	pmulhw xmm0, xmm1
	psllw xmm0,1
	movq xmm1,[rsi+rdx*8+268288]
	paddsw xmm0,xmm1
	movq [rdi],xmm0

	lea rdi,[rdi+8]
	lea rcx,[rcx+4]

	align 16
hfpa2lop4:
	movzx rdx,byte [rcx]
	movq xmm1, [rsi+rdx*8+266240]
	movzx rdx,byte [rcx+4]
	movq xmm2, [rsi+rdx*8+266240]
	movzx rdx,byte [rcx+1]
	punpcklqdq xmm1,xmm2
	movq xmm0, [rsi+rdx*8+264192]
	movzx rdx,byte [rcx+5]
	movq xmm2, [rsi+rdx*8+264192]
	punpcklqdq xmm0,xmm2
	paddsw xmm1,xmm0
	movzx rdx,byte [rcx+2]
	movq xmm0, [rsi+rdx*8+262144]
	movzx rdx,byte [rcx+6]
	movq xmm2, [rsi+rdx*8+262144]
	movzx edx,byte [rcx+3]
	mov dh,byte [rcx+7]
	punpcklqdq xmm0,xmm2
	paddsw xmm1,xmm0

	movd xmm0,edx
	punpcklbw xmm0, xmm0
	punpcklbw xmm0, xmm0
	punpcklbw xmm0, xmm0
	psrlw xmm0,1
	pmulhw xmm0, xmm1
	psllw xmm0,1
	movzx rax,dl
	movq xmm1,[rsi+rax*8+268288]
	movzx eax,dh
	movq xmm2,[rsi+rax*8+268288]
	punpcklqdq xmm1,xmm2
	paddsw xmm0,xmm1
	movdqu [rdi],xmm0

	lea rdi,[rdi+16]
	lea rcx,[rcx+8]
	dec rbp
	jnz hfpa2lop4

	mov rsi,r11 ;outPt
	mov rbp,r8 ;dwidth
	mov rbx,qword [rsp+80] ;index
	mov rdi,qword [rsp+88] ;weight
	mov rcx,qword [rsp+128] ;tmpbuff
	align 16
hfpa2lop2:
	mov rdx,qword [rsp+72] ;tap
	pxor xmm2,xmm2
	pxor xmm3,xmm3
	ALIGN 16
hfpa2lop3:
	mov rax,qword [rbx]
	movq xmm0,[rcx+rax]
	mov rax,qword [rbx+8]
	movq xmm1,[rcx+rax]
	punpcklwd xmm0,xmm1
	pmaddwd xmm0, [rdi]
	paddd xmm2,xmm0
	
	lea rdi,[rdi+16]
	lea rbx,[rbx+16]
	dec rdx
	jnz hfpa2lop3

	mov rdx,qword [rsp+72] ;tap
	ALIGN 16
hfpa2lop3b:
	mov rax,qword [rbx]
	movq xmm0,[rcx+rax]
	mov rax,qword [rbx+8]
	movq xmm1,[rcx+rax]
	punpcklwd xmm0,xmm1
	pmaddwd xmm0,[rdi]
	paddd xmm3,xmm0
	
	lea rdi,[rdi+16]
	lea rbx,[rbx+16]
	dec rdx
	jnz hfpa2lop3b

	psrad xmm2,15
	psrad xmm3,15
	packssdw xmm2,xmm3
	movdqu [rsi],xmm2
	lea rsi,[rsi+16]
	dec rbp
	jnz hfpa2lop2

	add r10,qword [rsp+96] ;sstep
	add r11,qword [rsp+104] ;dstep
	dec r9 ;currHeight
	jnz hfpa2lop
	
	align 16
hfpaexit:
	pop rdi
	pop rsi
	pop rbx
	pop rbp
	ret

;void LanczosResizer8_C8_horizontal_filter(UInt8 *inPt, UInt8 *outPt, OSInt dwidth, OSInt height, OSInt tap, OSInt *index, Int64 *weight, OSInt sstep, OSInt dstep, UInt8 *rgbTable, OSInt swidth, UInt8 *tmpbuff)
;0 rdi
;8 rsi
;16 rbx
;24 rbp
;32 retAddr
;rcx inpt r10
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
LanczosResizer8_C8_horizontal_filter:
	push rbp
	push rbx
	push rsi
	push rdi
	mov r10,rcx
	mov r11,rdx
	test r8,1 ;dwidth
	jnz hfstart
	cmp qword [rsp+72],6 ;tap
	jz hf6start
	jmp hf2start
	
	align 16
hfstart:					;if (dwidth & 1)
	shr qword [rsp+72],1 ;tap
	pxor xmm3,xmm3
	align 16
hflop:
	mov rcx,r10 ;inPt
	mov rsi,qword [rsp+112] ;rgbTable
	mov rbp,qword [rsp+120] ;swidth
	mov rdi,qword [rsp+128] ;tmpbuff
	shr rbp,1
	jnb hflop4

	mov eax,dword [rcx]
	movzx rdx,al
	movq xmm1,[rsi+rdx*8+266240]
	movzx edx,ah
	movq xmm0,[rsi+rdx*8+264192]
	shr eax,16
	paddsw xmm1,xmm0
	movzx edx,al
	movq xmm0,[rsi+rdx*8+262144]
	paddsw xmm1,xmm0
	movzx edx,ah
	movq xmm0,[rsi+rdx*8+268288]
	paddsw xmm1,xmm0
	movq [rdi],xmm1
	lea rdi,[rdi+8]
	lea rcx,[rcx+4]

	align 16
hflop4:
	mov eax,dword [rcx]
	mov ebx,dword [rcx+4]
	movzx rdx,al
	movq xmm1,[rsi+rdx*8+266240]
	movzx edx,bl
	movq xmm2,[rsi+rdx*8+266240]
	movzx edx,ah
	punpcklqdq xmm1,xmm2
	movq xmm0,[rsi+rdx*8+264192]
	movzx edx,bh
	movq xmm2,[rsi+rdx*8+264192]
	shr eax,16
	shr ebx,16
	punpcklqdq xmm0,xmm2
	paddsw xmm1,xmm0
	movzx edx,al
	movq xmm0,[rsi+rdx*8+262144]
	movzx edx,bl
	movq xmm2,[rsi+rdx*8+262144]
	punpcklqdq xmm0,xmm2
	paddsw xmm1,xmm0
	movzx edx,ah
	movq xmm0,[rsi+rdx*8+268288]
	movzx edx,bh
	movq xmm2,[rsi+rdx*8+268288]
	punpcklqdq xmm0,xmm2
	paddsw xmm1,xmm0
	movdqu [rdi],xmm1

	lea rdi,[rdi+16]
	lea rcx,[rcx+8]
	dec rbp
	jnz hflop4

	mov rbp,r8 ;dwidth
	mov rsi,r11 ;outPt
	mov rbx,qword [rsp+80] ;index
	mov rdi,qword [rsp+88] ;weight
	mov rcx,qword [rsp+128] ;tmpbuff
	align 16
hflop2:
	mov rdx,qword [rsp+72] ;tap
	pxor xmm2,xmm2
	align 16
hflop3:
	mov rax,qword [rbx]
	movq xmm0,[rcx+rax]
	mov rax,qword [rbx+8]
	movq xmm1,[rcx+rax]
	punpcklwd xmm0,xmm1
	pmaddwd xmm0,[rdi]
	paddd xmm2,xmm0
	
	lea rdi,[rdi+16]
	lea rbx,[rbx+16]
	dec rdx
	jnz hflop3

	psrad xmm2,15
	packssdw xmm2,xmm3
	movq [rsi],xmm2
	lea rsi,[rsi+8]
	dec rbp
	jnz hflop2

	add r10,qword [rsp+96] ;sstep
	add r11,qword [rsp+104] ;dstep
	dec r9 ;currHeight
	jnz hflop
	jmp hfexit

	align 16
hf6start:								;else if (tap == 6)
	shr r8,1 ;dwidth
	shr qword [rsp+40],1 ;tap
	pxor xmm3,xmm3
	align 16
hf6lop:
	mov rcx,r10 ;inPt
	mov rsi,qword [rsp+112] ;rgbTable
	mov rbp,qword [rsp+120] ;swidth
	mov rdi,qword [rsp+128] ;tmpbuff

	shr rbp,1
	jnb hf6lop4

	mov eax,dword [rcx]
	movzx rdx,al
	movq xmm1,[rsi+rdx*8+266240]
	movzx edx,ah
	movq xmm0,[rsi+rdx*8+264192]
	shr eax,16
	paddsw xmm1,xmm0
	movzx edx,al
	movq xmm0,[rsi+rdx*8+262144]
	paddsw xmm1,xmm0
	movzx edx,ah
	movq xmm0,[rsi+rdx*8+268288]
	paddsw xmm1,xmm0
	movq [rdi],xmm1
	lea rdi,[rdi+8]
	lea rcx,[rcx+4]

	align 16
hf6lop4:
	mov eax,dword [rcx]
	mov ebx,dword [rcx+4]
	movzx rdx,al
	movq xmm1,[rsi+rdx*8+266240]
	movzx edx,bl
	movq xmm2,[rsi+rdx*8+266240]
	movzx edx,ah
	punpcklqdq xmm1,xmm2
	movq xmm0,[rsi+rdx*8+264192]
	movzx edx,bh
	movq xmm2,[rsi+rdx*8+264192]
	shr eax,16
	shr ebx,16
	punpcklqdq xmm0,xmm2
	paddsw xmm1,xmm0
	movzx edx,al
	movq xmm0,[rsi+rdx*8+262144]
	movzx edx,bl
	movq xmm2,[rsi+rdx*8+262144]
	punpcklqdq xmm0,xmm2
	paddsw xmm1,xmm0
	movzx edx,ah
	movq xmm0,[rsi+rdx*8+268288]
	movzx edx,bh
	movq xmm2,[rsi+rdx*8+268288]
	punpcklqdq xmm0,xmm2
	paddsw xmm1,xmm0
	movdqu [rdi],xmm1

	lea rdi,[rdi+16]
	lea rcx,[rcx+8]
	dec rbp
	jnz hf6lop4

	mov rbp,r8 ;dwidth
	mov rsi,r11 ;outPt
	mov rbx,qword [rsp+80] ;index
	mov rdi,qword [rsp+88] ;weight
	mov rcx,qword [rsp+128] ;tmpbuff

	align 16
hf6lop2:
	mov rax,qword [rbx]
	pxor xmm1,xmm1
	mov rdx,qword [rbx+8]
	movdqu xmm0,[rcx+rax]
	movdqa xmm4,xmm0
	movdqu xmm2,[rcx+rdx]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[rdi]
	punpckhwd xmm4,xmm2
	pmaddwd xmm4,[rdi+16]
	paddd xmm1,xmm0
	mov rax,qword [rbx+16]
	paddd xmm1,xmm4
	movdqu xmm0,[rcx+rax]
	pshufd xmm2,xmm0,14
	punpcklwd xmm0,xmm2
	pmaddwd xmm0, [rdi+32]
	paddd xmm1,xmm0

	mov rax,qword [rbx+24]
	pxor xmm3,xmm3
	mov rdx,qword [rbx+32]
	movdqu xmm0,[rcx+rax]
	movdqa xmm4,xmm0
	movdqu xmm2,[rcx+rdx]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[rdi+48]
	punpckhwd xmm4,xmm2
	pmaddwd xmm4,[rdi+64]
	paddd xmm3,xmm0
	mov rax,qword [rbx+40]
	paddd xmm3,xmm4
	movdqu xmm0,[rcx+rax]
	pshufd xmm2,xmm0,14
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[rdi+80]
	paddd xmm3,xmm0

	lea rdi,[rdi+96]
	lea rbx,[rbx+48]

	psrad xmm1,15
	psrad xmm3,15
	packssdw xmm1,xmm3
	movdqu [rsi],xmm1
	lea rsi,[rsi+16]
	dec rbp
	jnz hf6lop2

	add r10,qword [rsp+96] ;sstep
	add r11,qword [rsp+104] ;dstep

	dec r9 ;currHeight
	jnz hf6lop
	jmp hfexit

	align 16
hf2start:
	shr r8,1 ;dwidth
	shr qword [rsp+72],1 ;tap
	pxor xmm3,xmm3
	align 16
hf2lop:
	mov rcx,r10 ;inPt
	mov rsi,qword [rsp+112] ;rgbTable
	mov rbp,qword [rsp+120] ;swidth
	mov rdi,qword [rsp+128] ;tmpbuff

	shr rbp,1
	jnb hf2lop4

	mov eax,dword [rcx]
	movzx rdx,al
	movq xmm1,[rsi+rdx*8+266240]
	movzx edx,ah
	movq xmm0,[rsi+rdx*8+264192]
	shr eax,16
	paddsw xmm1,xmm0
	movzx edx,al
	movq xmm0,[rsi+rdx*8+262144]
	paddsw xmm1,xmm0
	movzx edx,ah
	movq xmm0,[rsi+rdx*8+268288]
	paddsw xmm1,xmm0
	movq [rdi],xmm1
	lea rdi,[rdi+8]
	lea rcx,[rcx+4]

	align 16
hf2lop4:
	mov eax,dword [rcx]
	mov ebx,dword [rcx+4]
	movzx rdx,al
	movq xmm1,[rsi+rdx*8+266240]
	movzx edx,bl
	movq xmm2,[rsi+rdx*8+266240]
	movzx edx,ah
	punpcklqdq xmm1,xmm2
	movq xmm0,[rsi+rdx*8+264192]
	movzx edx,bh
	movq xmm2,[rsi+rdx*8+264192]
	shr eax,16
	shr ebx,16
	punpcklqdq xmm0,xmm2
	paddsw xmm1,xmm0
	movzx edx,al
	movq xmm0,[rsi+rdx*8+262144]
	movzx edx,bl
	movq xmm2,[rsi+rdx*8+262144]
	punpcklqdq xmm0,xmm2
	paddsw xmm1,xmm0
	movzx edx,ah
	movq xmm0,[rsi+rdx*8+268288]
	movzx edx,bh
	movq xmm2,[rsi+rdx*8+268288]
	punpcklqdq xmm0,xmm2
	paddsw xmm1,xmm0
	movdqu [rdi],xmm1

	lea rdi,[rdi+16]
	lea rcx,[rcx+8]
	dec rbp
	jnz hf2lop4

	mov rbp,r8 ;dwidth
	mov rsi,r11 ;outPt
	mov rbx,qword [rsp+80] ;index
	mov rdi,qword [rsp+88] ;weight
	mov rcx,qword [rsp+128] ;tmpbuff
	align 16
hf2lop2:
	mov rdx,qword [rsp+72] ;tap
	pxor xmm2,xmm2
	pxor xmm3,xmm3
	align 16
hf2lop3:
	mov rax,qword [rbx]
	movq xmm0,[rcx+rax]
	mov rax,qword [rbx+8]
	movq xmm1,[rcx+rax]
	punpcklwd xmm0,xmm1
	pmaddwd xmm0,[rdi]
	paddd xmm2,xmm0
	
	lea rdi,[rdi+16]
	lea rbx,[rbx+16]
	dec rdx
	jnz hf2lop3

	mov rdx,qword [rsp+72] ;tap
	align 16
hf2lop3b:
	mov rax,qword [rbx]
	movq xmm0,[rcx+rax]
	mov rax,qword [rbx+8]
	movq xmm1,[rcx+rax]
	punpcklwd xmm0,xmm1
	pmaddwd xmm0,[rdi]
	paddd xmm3,xmm0
	
	lea rdi,[rdi+16]
	lea rbx,[rbx+16]
	dec rdx
	jnz hf2lop3b

	psrad xmm2,15
	psrad xmm3,15
	packssdw xmm2,xmm3
	movdqu [rsi],xmm2
	lea rsi,[rsi+16]
	dec rbp
	jnz hf2lop2

	add r10,qword [rsp+96] ;sstep
	add r11,qword [rsp+104] ;dstep

	dec r9 ;currHeight
	jnz hf2lop

	align 16
hfexit:
	pop rdi
	pop rsi
	pop rbx
	pop rbp
	ret

;void LanczosResizer8_C8_vertical_filter(UInt8 *inPt, UInt8 *outPt, OSInt dwidth, OSInt height, OSInt tap, OSInt *index, Int64 *weight, OSInt sstep, OSInt dstep, UInt8 *rgbTable)
;-32 xmm6
;-16 xmm7
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
LanczosResizer8_C8_vertical_filter:
	push rbp
	push rbx
	push rsi
	push rdi
	movdqu [rsp-32],xmm6
	movdqu [rsp-16],xmm7
	mov r10,rcx
	mov r11,rdx
	test r8,1 ;dwidth
	jnz vf1start
	cmp qword [rsp+72],6 ;tap
	jz vf6start
	jmp vfstart
	
	align 16
vf1start:					; if (dwidth & 1)
	mov qword [rsp-40],r12
	mov qword [rsp-48],r13
	mov qword [rsp-56],r14
	mov qword [rsp-64],r15
	
	mov r12,qword [rsp+72] ;tap
	shr r12,1
	mov rax,r8 ;dwidth
	shl rax,2
	mov rcx,r11 ;outPt
	sub qword [rsp+104],rax ;dstep
	pxor xmm3,xmm3
	
	mov r13,qword [rsp+80] ;index
	mov r14,qword [rsp+88] ;weight
	mov rdi,qword [rsp+112] ;rgbTable
	align 16
vf1lop:
	mov rsi,r10 ;inPt
	mov rbp,r8 ;dwidth
	align 16
vf1lop2:
	mov rbx,r13 ;index
	mov r15,r14 ;weight
	mov rdx,r12 ;tap
	
	pxor xmm2,xmm2
	ALIGN 16
vf1lop3:
	mov eax,dword [rbx]
	movq xmm0,[rsi+rax]
	mov eax,dword [rbx+8]
	movq xmm1,[rsi+rax]
	punpcklwd xmm0, xmm1
	pmaddwd xmm0,[r15]
	paddd xmm2,xmm0
	lea r15,[r15+16]
	lea rbx,[rbx+16]

	dec rdx
	jnz vf1lop3

	psrad xmm2,15
	packssdw xmm2,xmm3
	pextrw rbx,xmm2,2
	pextrw rdx,xmm2,3
	mov al,byte [rdi+rbx+131072]
	mov ah,byte [rdi+rdx+196608]
	shl eax,16
	pextrw rbx,xmm2,0
	pextrw rdx,xmm2,1
	mov al,byte [rdi+rbx]
	mov ah,byte [rdi+rdx+65536]

	movnti dword [rcx],eax
	lea rsi,[rsi+8]
	lea rcx,[rcx+4]
	dec rbp
	jnz vf1lop2

	mov rax,r12 ;tap
	shl rax,4
	add r13,rax ;index
	add r14,rax ;weight

	add rcx,qword [rsp+104] ;dstep

	dec r9 ;currHeight
	jnz vf1lop
	mov r12,qword [rsp-40]
	mov r13,qword [rsp-48]
	mov r14,qword [rsp-56]
	mov r15,qword [rsp-64]
	jmp vfexit

	align 16
vf6start:				;else if (tap == 6)
	mov rax,r8 ;dwidth
	shr qword [rsp+72],1 ;tap
	lea rdx,[rax*4]
	shr rax,1
	sub qword [rsp+104],rdx ;dstep
	mov r8,rax ;dwidth

	mov rcx,r11 ;outPt
	mov rdi,qword [rsp+80] ;index
	mov rbp,qword [rsp+112] ;rgbTable
	pxor xmm3,xmm3
	align 16
vf6lop4:
	mov rbx,qword [rsp+88] ;weight
	mov rsi,r10 ;inPt

	movdqa xmm5,[rbx]
	movdqa xmm6,[rbx+16]
	movdqa xmm7,[rbx+32]

	mov rbx,r8 ;dwidth

	align 16
vf6lop5:

	mov rax,qword [rdi]
	pxor xmm3,xmm3
	mov rdx,qword [rdi+8]
	pxor xmm4,xmm4
	movdqu xmm0,[rsi+rax]
	movdqu xmm2,xmm0
	movdqu xmm1,[rsi+rdx]
	punpcklwd xmm0, xmm1
	punpckhwd xmm2, xmm1
	pmaddwd xmm0,xmm5
	pmaddwd xmm2,xmm5
	mov rax,qword [rdi+16]
	paddd xmm3,xmm0
	mov rdx,qword [rdi+24]
	paddd xmm4,xmm2
	movdqu xmm0,[rsi+rax]
	movdqu xmm2,xmm0
	movdqu xmm1,[rsi+rdx]
	punpcklwd xmm0, xmm1
	punpckhwd xmm2, xmm1
	pmaddwd xmm0,xmm6
	pmaddwd xmm2,xmm6
	mov rax,qword [rdi+32]
	paddd xmm3,xmm0
	mov rdx,qword [rdi+40]
	paddd xmm4,xmm2
	movdqu xmm0,[rsi+rax]
	movdqu xmm2,xmm0
	movdqu xmm1,[rsi+rdx]
	punpcklwd xmm0, xmm1
	punpckhwd xmm2, xmm1
	pmaddwd xmm0,xmm7
	pmaddwd xmm2,xmm7
	paddd xmm3,xmm0
	paddd xmm4,xmm2

	psrad xmm3,15
	psrad xmm4,15
	packssdw xmm3,xmm4
	pextrw rdx,xmm3,2
	mov al,byte [rbp+rdx+131072]
	pextrw rdx,xmm3,3
	mov ah,byte [rbp+rdx+196608]
	shl eax,16
	pextrw rdx,xmm3,0
	mov al,byte [rbp+rdx]
	pextrw rdx,xmm3,1
	mov ah,byte [rbp+rdx+65536]
	movd xmm0,eax

	pextrw rdx,xmm3,6
	mov al,byte [rbp+rdx+131072]
	pextrw rdx,xmm3,7
	mov ah,byte [rbp+rdx+196608]
	shl eax,16
	pextrw rdx,xmm3,4
	mov al,byte [rbp+rdx]
	pextrw rdx,xmm3,5
	mov ah,byte [rbp+rdx+65536]
	movd xmm1,eax
	punpckldq xmm0,xmm1

	movq [rcx],xmm0
	lea rcx,[rcx+8]
	lea rsi,[rsi+16]
	dec rbx
	jnz vf6lop5

	add rdi,48 ;index
	add qword [rsp+88],48 ;weight

	add rcx,qword [rsp+104] ;dstep

	dec r9 ;currHeight
	jnz vf6lop4
	jmp vfexit

	align 16
vfstart:
	mov qword [rsp-40],r12
	mov qword [rsp-48],r13
	mov qword [rsp-56],r14
	mov qword [rsp-64],r15
	
	mov r12,qword [rsp+72] ;tap
	shr r8,1 ;dwidth
	shr r12,1
	pxor xmm3,xmm3

	mov r13,qword [rsp+80] ;index
	mov r14,qword [rsp+88] ;weight
	mov rdi,qword [rsp+112] ;rgbTable
	align 16
vflop4:

	mov rsi,r10 ;inPt
	mov rcx,r11 ;outPt
	mov rbp,r8 ;dwidth
	
	align 16
vflop5:

	mov rdx,r12 ;tap
	mov rbx,r13 ;index
	mov r15,r14 ;weight
	pxor xmm2,xmm2
	pxor xmm6,xmm6
	ALIGN 16
vflop6:
	mov rax,qword [rbx]
	movdqu xmm0, [rsi+rax]
	mov rax,qword [rbx+8]
	movdqu xmm1, [rsi+rax]
	movdqu xmm4,xmm0
	punpcklwd xmm0, xmm1
	punpckhwd xmm4, xmm1
	movdqa xmm5,[r15]
	pmaddwd xmm0,xmm5
	pmaddwd xmm4,xmm5
	paddd xmm2,xmm0
	paddd xmm6,xmm4
	lea r15,[r15+16]
	lea rbx,[rbx+16]

	dec rdx
	jnz vflop6


	psrad xmm2,15
	psrad xmm6,15
	packssdw xmm2,xmm6
	pextrw rbx,xmm2,2
	pextrw rdx,xmm2,3
	mov al,byte [rdi+rbx+131072]
	mov ah,byte [rdi+rdx+196608]
	shl eax,16
	pextrw rbx,xmm2,0
	pextrw rdx,xmm2,1
	mov al,byte [rdi+rbx]
	mov ah,byte [rdi+rdx+65536]
	movd xmm0,eax

	pextrw rbx,xmm2,6
	pextrw rdx,xmm2,7
	mov al,byte [rdi+rbx+131072]
	mov ah,byte [rdi+rdx+196608]
	shl eax,16
	pextrw rbx,xmm2,4
	pextrw rdx,xmm2,5
	mov al,byte [rdi+rbx]
	mov ah,byte [rdi+rdx+65536]
	movd xmm1,eax
	punpckldq xmm0,xmm1

	movq [rcx],xmm0
	lea rcx,[rcx+8]
	lea rsi,[rsi+16]
	dec rbp
	jnz vflop5

	mov rax,r12 ;tap
	shl rax,4
	add r13,rax ;index
	add r14,rax ;weight

	add r11,qword [rsp+104] ;dstep
	
	dec r9 ;currHeight
	jnz vflop4

	mov r12,qword [rsp-40]
	mov r13,qword [rsp-48]
	mov r14,qword [rsp-56]
	mov r15,qword [rsp-64]
	align 16
vfexit:
	movdqu [rsp-32],xmm6
	movdqu xmm7,[rsp-16]
	pop rdi
	pop rsi
	pop rbx
	pop rbp
	ret

;void LanczosResizer8_C8_expand(UInt8 *inPt, UInt8 *outPt, OSInt width, OSInt height, OSInt sstep, OSInt dstep, UInt8 *rgbTable)
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
LanczosResizer8_C8_expand:
	push rbp
	push rbx
	push rsi
	push rdi
	
;	mov rcx,qword [rsp+40] ;inPt
	mov rsi,rdx ;outPt
	lea rdx,[r8*4] ;width
	lea rax,[r8*8] ;width
	sub qword [rsp+72],rdx ;sstep
	sub qword [rsp+80],rax ;dstep
	mov rbx,qword [rsp+88] ;rgbTable
	xor rdx,rdx
	align 16
explop:
	mov rax,r8 ;width

	ALIGN 16
explop2:
	movzx edx,byte [rcx+3]
	movq xmm1,[rbx+rdx*8+268288]
	movzx edx,byte [rcx+2]
	movq xmm0,[rbx+rdx*8+262144]
	paddsw xmm1,xmm0
	movzx edx,byte [rcx+1]
	movq xmm0,[rbx+rdx*8+264192]
	paddsw xmm1,xmm0
	movzx edx,byte [rcx]
	movq xmm0,[rbx+rdx*8+266240]
	paddsw xmm1,xmm0
	movq [rsi],xmm1

	lea rsi,[rsi+8]
	lea rcx,[rcx+4]
	dec rax
	jnz explop2

	add rcx,qword [rsp+72] ;sstep
	add rsi,qword [rsp+80] ;dstep

	dec r9
	jnz explop
	pop rdi
	pop rsi
	pop rbx
	pop rbp
	ret

;void LanczosResizer8_C8_expand_pa(UInt8 *inPt, UInt8 *outPt, OSInt width, OSInt height, OSInt sstep, OSInt dstep, UInt8 *rgbTable)
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
LanczosResizer8_C8_expand_pa:
	push rbp
	push rbx
	push rsi
	push rdi
;	mov rcx,rcx ;inPt
	mov rsi,rdx ;outPt
	lea rdx,[r8*4] ;width
	lea rax,[r8*8] ;width
	sub qword [rsp+72],rdx ;sstep
	sub qword [rsp+80],rax ;dstep
	mov rbx,qword [rsp+88] ;rgbTable
	xor rdx,rdx
	align 16
exppalop:
	mov rdi,r8 ;width
	ALIGN 16
exppalop2:
	mov eax,dword [rcx]
	movzx edx,al
	movq xmm1,[rbx+rdx*8+266240]
	movzx edx,ah
	movq xmm0,[rbx+rdx*8+264192]
	shr eax,16
	paddsw xmm1,xmm0
	movzx edx,al
	movq xmm0,[rbx+rdx*8+262144]
	paddsw xmm1,xmm0
	movzx edx,ah

	movd xmm0,edx
	punpcklbw xmm0, xmm0
	punpcklbw xmm0, xmm0
	punpcklbw xmm0, xmm0
	psrlw xmm0,1
	pmulhw xmm0, xmm1
	psllw xmm0,1
	movq xmm1,[rbx+rdx*8+268288]
	paddsw xmm0,xmm1
	movq [rsi],xmm0

	lea rcx,[rcx+4]
	lea rsi,[rsi+8]
	dec rdi
	jnz exppalop2

	add rcx,qword [rsp+72] ;sstep
	add rsi,qword [rsp+80] ;dstep

	dec r9
	jnz exppalop
	pop rdi
	pop rsi
	pop rbx
	pop rbp
	ret

;void LanczosResizer8_C8_collapse(UInt8 *inPt, UInt8 *outPt, OSInt width, OSInt height, OSInt sstep, OSInt dstep, UInt8 *rgbTable)
;	Int64 toAdd = 0xff80ff80ff80ff80;
;	Int64 toAdd2 = 0x80808080;
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
LanczosResizer8_C8_collapse
	push rbp
	push rbx
	push rsi
	push rdi
	;mov rcx,rcx ;inPt
	mov rsi,rdx ;outPt
	lea rdx,[r8*8] ;width
	lea rax,[r8*4] ;width
	sub qword [rsp+72],rdx ;sstep
	sub qword [rsp+80],rax ;dstep
	mov rbx,qword [rsp+88] ;rgbTable
	xor edx,edx
	align 16
collop:
	mov rdi,r8 ;width
	
	ALIGN 16
collop2:
	movzx edx,word [rcx+6]
	mov ah,byte [rbx+rdx+196608]
	movzx edx,word [rcx+4]
	mov al,byte [rbx+rdx+131072]
	shl eax,16
	movzx edx,word [rcx+2]
	mov ah,byte [rbx+rdx+65536]
	movzx edx,word [rcx]
	mov al,byte [rbx+rdx]

	movnti dword [rsi],eax
	lea rsi,[rsi+4]
	lea rcx,[rcx+8]
	dec rdi
	jnz collop2

	add rcx,qword [rsp+72] ;sstep
	add rsi,qword [rsp+80] ;dstep

	dec r9
	jnz collop
	pop rdi
	pop rsi
	pop rbx
	pop rbp
	ret

;void LanczosResizer8_C8_imgcopy(UInt8 *inPt, UInt8 *outPt, OSInt width, OSInt height, OSInt sstep, OSInt dstep, UInt8 *rgbTable)
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
LanczosResizer8_C8_imgcopy:
	push rbp
	push rbx
	push rsi
	push rdi
	mov rsi,rcx ;inPt
	mov rdi,rdx ;outPt
	mov rbx,qword [rsp+88] ;rgbTable
	lea rax,[r8*4] ;width
	sub qword [rsp+72],rax ;sstep
	sub qword [rsp+80],rax ;dstep
	align 16
iclop:
	mov rdx,r8 ;width
	ALIGN 16
iclop2:
	movzx rax,byte [rsi+2]
	movq xmm1,[rbx+rax*8+262144]
	movzx rax,byte [rsi+1]
	movq xmm0,[rbx+rax*8+264192]
	paddsw xmm1,xmm0
	movzx rax,byte [rsi]
	movq xmm0,[rbx+rax*8+266240]
	paddsw xmm1,xmm0
	pextrw rcx,xmm1,2
	mov al,byte [rbx+rcx+131072]
	mov ah,byte [rsi+3]
	shl eax,16
	pextrw rcx,xmm1,1
	mov ah,byte [rbx+rcx+65536]
	pextrw rcx,xmm1,0
	mov al,byte [rbx+rcx]
	movnti [rdi],rax
	lea rsi,[rsi+4]
	lea rdi,[rdi+4]
	dec rdx
	jnz iclop2

	add rsi,qword [rsp+72] ;sstep
	add rdi,qword [rsp+80] ;dstep
	dec r9
	jnz iclop
	pop rdi
	pop rsi
	pop rbx
	pop rbp
	ret

;void LanczosResizer8_C8_imgcopy_pa(UInt8 *inPt, UInt8 *outPt, OSInt width, OSInt height, OSInt sstep, OSInt dstep, UInt8 *rgbTable)
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
LanczosResizer8_C8_imgcopy_pa:
	push rbp
	push rbx
	push rsi
	push rdi
	mov rsi,rcx ;inPt
	mov rdi,rdx ;outPt
	lea rdx,[r8*4] ;width
	shr r8,1 ;width = width >> 1;
	sub qword [rsp+72],rdx ;sstep
	sub qword [rsp+80],rdx ;dstep

	mov rbx,qword [rsp+88] ;rgbTable
	align 16
icpalop:
	mov rbp,r8 ;width
	xor rdx,rdx
	ALIGN 16
icpalop2:
	mov eax,dword [rsi]
	movzx rdx,al
	movq xmm1,[rbx+rdx*8+266240]
	movzx edx,ah
	movq xmm0,[rbx+rdx*8+264192]
	shr eax,16
	paddsw xmm1,xmm0
	movzx edx,al
	movq xmm0,[rbx+rdx*8+262144]
	paddsw xmm1,xmm0
	movzx edx,ah
	movd xmm3,edx

	mov eax,dword [rsi+4]
	movzx rdx,al
	movq xmm2,[rbx+rdx*8+266240]
	movzx edx,ah
	movq xmm0,[rbx+rdx*8+264192]
	shr eax,16
	paddsw xmm2,xmm0
	movzx edx,al
	movq xmm0,[rbx+rdx*8+262144]
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

	pextrw rax,xmm1,2
	movzx edx,byte [rbx+rax+131072]
	mov dh,byte [rsi+3]
	pextrw rax,xmm1,1
	shl edx,16
	mov dh,byte [rbx+rax+65536]
	pextrw rax,xmm1,0
	mov dl,byte [rbx+rax]
	movnti dword [rdi],edx

	pextrw rax,xmm1,6
	movzx edx,byte [rbx+rax+131072]
	mov dh,byte [rsi+7]
	pextrw rax,xmm1,5
	shl edx,16
	mov dh,byte [rbx+rax+65536]
	pextrw rax,xmm1,4
	mov dl,byte [rbx+rax]
	movnti dword [rdi+4],edx

	lea rsi,[rsi+8]
	lea rdi,[rdi+8]
	dec rbp
	jnz icpalop2

	add rsi,qword [rsp+72] ;sstep
	add rdi,qword [rsp+80] ;dstep
	dec r9
	jnz icpalop
	
	pop rdi
	pop rsi
	pop rbx
	pop rbp
	ret
