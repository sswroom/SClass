section .text

global LanczosResizer8_C8_horizontal_filter_pa
global _LanczosResizer8_C8_horizontal_filter_pa
global LanczosResizer8_C8_horizontal_filter
global _LanczosResizer8_C8_horizontal_filter
global LanczosResizer8_C8_vertical_filter
global _LanczosResizer8_C8_vertical_filter
global LanczosResizer8_C8_horizontal_filter_pac
global _LanczosResizer8_C8_horizontal_filter_pac
global LanczosResizer8_C8_horizontal_filter_c
global _LanczosResizer8_C8_horizontal_filter_c
global LanczosResizer8_C8_expand
global _LanczosResizer8_C8_expand
global LanczosResizer8_C8_expand_pa
global _LanczosResizer8_C8_expand_pa
global LanczosResizer8_C8_collapse
global _LanczosResizer8_C8_collapse
global LanczosResizer8_C8_imgcopy
global _LanczosResizer8_C8_imgcopy
global LanczosResizer8_C8_imgcopy_pa
global _LanczosResizer8_C8_imgcopy_pa

;void LanczosResizer8_C8_horizontal_filter_pa(UInt8 *inPt, UInt8 *outPt, OSInt dwidth, OSInt height, OSInt tap, OSInt *index, Int64 *weight, OSInt sstep, OSInt dstep, UInt8 *rgbTable, OSInt swidth, UInt8 *tmpbuff)
;0 index
;8 tap
;16 rbx
;24 rbp
;32 retAddr
;rdi inpt   r10
;rsi outPt  r11
;rdx dwidth r8
;rcx height r9
;r8 tap     8
;r9 index   0
;40 weight
;48 sstep
;56 dstep
;64 rgbTable
;72 swidth
;80 tmpbuff

	align 16
LanczosResizer8_C8_horizontal_filter_pa:
_LanczosResizer8_C8_horizontal_filter_pa:
LanczosResizer8_C8_horizontal_filter_pac:
_LanczosResizer8_C8_horizontal_filter_pac:
	push rbp
	push rbx
	push r8
	push r9
	mov r10,rdi
	mov r11,rsi
	mov r8,rdx
	mov r9,rcx
	test r8,1 ;dwidth
	jnz hfpastart
	cmp qword [rsp+8],6 ;tap
	jz hfpa6start
	jmp hfpa2start

	align 16
hfpastart:							;if (dwidth & 1)
	shr qword [rsp+8],1 ;tap
	pxor xmm3,xmm3
	align 16
hfpalop:
	mov rcx,r10 ;inPt
	mov rdi,qword [rsp+80] ;tmpbuff
	mov rsi,qword [rsp+64] ;rgbTable

	mov rbp,qword [rsp+72] ;swidth
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
	pmulhuw xmm0, xmm1
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
	movhps xmm1, [rsi+rdx*8+266240]
	movzx edx,ah
	movq xmm0, [rsi+rdx*8+264192]
	movzx edx,bh
	movhps xmm0, [rsi+rdx*8+264192]
	shr eax,16
	shr ebx,16
	paddsw xmm1,xmm0
	movzx edx,al
	movq xmm0, [rsi+rdx*8+262144]
	movzx edx,bl
	movhps xmm0, [rsi+rdx*8+262144]
	movzx edx,ah
	mov dh,bh
	paddsw xmm1,xmm0

	movd xmm0,edx
	punpcklbw xmm0, xmm0
	punpcklbw xmm0, xmm0
	punpcklbw xmm0, xmm0
	pmulhuw xmm0, xmm1
	movzx edx,ah
	movq xmm1, [rsi+rdx*8+268288]
	movzx edx,bh
	movhps xmm1, [rsi+rdx*8+268288]
	paddsw xmm0,xmm1
	movdqu [rdi],xmm0

	lea rdi,[rdi+16]
	lea rcx,[rcx+8]
	dec rbp
	jnz hfpalop4

	mov rsi,r11 ;outPt
	mov rbp,r8 ;dwidth
	mov rbx,qword [rsp+0] ;index
	mov rdi,qword [rsp+40] ;weight
	mov rcx,qword [rsp+80] ;tmpbuff
	align 16
hfpalop2:
	mov rdx,qword [rsp+8] ;tap
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

	add r10,qword [rsp+48] ;sstep
	add r11,qword [rsp+56] ;dstep
	dec r9 ;currHeight
	jnz hfpalop
	jmp hfpaexit

	align 16
hfpa6start:								;else if (tap == 6)
	shr r8,1 ;dwidth
	shr qword [rsp+8],1 ;tap
	align 16
hfpa6lop:
	mov rcx,r10 ;inPt
	mov rdi,qword [rsp+80] ;tmpbuff
	mov rsi,qword [rsp+64] ;rgbTable

	mov rbp,qword [rsp+72] ;swidth
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
	pmulhuw xmm0, xmm1
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
	movhps xmm1,[rsi+rdx*8+266240]
	movzx edx,ah
	movq xmm0,[rsi+rdx*8+264192]
	movzx edx,bh
	movhps xmm0,[rsi+rdx*8+264192]
	shr eax,16
	shr ebx,16
	paddsw xmm1,xmm0
	movzx edx,al
	movq xmm0,[rsi+rdx*8+262144]
	movzx edx,bl
	movhps xmm0,[rsi+rdx*8+262144]
	movzx edx,ah
	mov dh,bh
	paddsw xmm1,xmm0

	movd xmm0,edx
	punpcklbw xmm0, xmm0
	punpcklbw xmm0, xmm0
	punpcklbw xmm0, xmm0
	pmulhuw xmm0, xmm1
	movzx edx,ah
	movq xmm1,[rsi+rdx*8+268288]
	movzx edx,bh
	movhps xmm1,[rsi+rdx*8+268288]
	paddsw xmm0,xmm1
	movdqu [rdi],xmm0

	lea rdi,[rdi+16]
	lea rcx,[rcx+8]
	dec rbp
	jnz hfpa6lop4

	mov rsi,r11 ;outPt
	mov rbp,r8 ;dwidth
	mov rbx,qword [rsp+0] ;index
	mov rdi,qword [rsp+40] ;weight
	mov rcx,qword [rsp+80] ;tmpbuff
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

	add rdi,96
	add rbx,48

	psrad xmm1,15
	psrad xmm3,15
	packssdw xmm1,xmm3
	movdqu [rsi],xmm1
	add rsi,16
	dec rbp
	jnz hfpa6lop2

	add r10,qword [rsp+48] ;sstep
	add r11,qword [rsp+56] ;dstep

	dec r9 ;currHeight
	jnz hfpa6lop
	jmp hfpaexit

	align 16
hfpa2start:
	shr r8,1 ;dwidth
	shr qword [rsp+8],1 ;tap
	align 16
hfpa2lop:
	mov rcx,r10 ;inPt
	mov rsi,qword [rsp+64] ;rgbTable
	mov rbp,qword [rsp+72] ;swidth
	mov rdi,qword [rsp+80] ;tmpbuff

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
	pmulhuw xmm0, xmm1
	movq xmm1,[rsi+rdx*8+268288]
	paddsw xmm0,xmm1
	movq [rdi],xmm0

	lea rdi,[rdi+8]
	lea rcx,[rcx+4]

	align 16
hfpa2lop4:
	mov eax,dword [rcx]
	mov ebx,dword [rcx+4]
	movzx rdx,al
	movq xmm1, [rsi+rdx*8+266240]
	movzx rdx,bl
	movhps xmm1, [rsi+rdx*8+266240]
	movzx edx,ah
	movq xmm0, [rsi+rdx*8+264192]
	movzx edx,bh
	movhps xmm0, [rsi+rdx*8+264192]
	shr eax,16
	shr ebx,16
	paddsw xmm1,xmm0
	movzx edx,al
	movq xmm0, [rsi+rdx*8+262144]
	movzx edx,bl
	movhps xmm0, [rsi+rdx*8+262144]
	movzx edx,ah
	mov dh,bh
	paddsw xmm1,xmm0

	movd xmm0,edx
	punpcklbw xmm0, xmm0
	punpcklbw xmm0, xmm0
	punpcklbw xmm0, xmm0
	pmulhuw xmm0, xmm1
	movzx edx,ah
	movq xmm1,[rsi+rdx*8+268288]
	movzx edx,bh
	movhps xmm1,[rsi+rdx*8+268288]
	paddsw xmm0,xmm1
	movdqu [rdi],xmm0

	lea rdi,[rdi+16]
	lea rcx,[rcx+8]
	dec rbp
	jnz hfpa2lop4

	mov rsi,r11 ;outPt
	mov rbp,r8 ;dwidth
	mov rbx,qword [rsp+0] ;index
	mov rdi,qword [rsp+40] ;weight
	mov rcx,qword [rsp+80] ;tmpbuff
	align 16
hfpa2lop2:
	mov rdx,qword [rsp+8] ;tap
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

	mov rdx,qword [rsp+8] ;tap
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

	add r10,qword [rsp+48] ;sstep
	add r11,qword [rsp+56] ;dstep
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
;0 index
;8 tap
;16 rbx
;24 rbp
;32 retAddr
;rdi inpt r10
;rsi outPt r11
;rdx dwidth r8
;rcx height r9
;r8 tap     8
;r9 index   0
;40 weight
;48 sstep
;56 dstep
;64 rgbTable
;72 swidth
;80 tmpbuff

	align 16
LanczosResizer8_C8_horizontal_filter:
_LanczosResizer8_C8_horizontal_filter:
LanczosResizer8_C8_horizontal_filter_c:
_LanczosResizer8_C8_horizontal_filter_c:
	push rbp
	push rbx
	push r8
	push r9
	mov r10,rdi
	mov r11,rsi
	mov r8,rdx
	mov r9,rcx
	test r8,1 ;dwidth
	jnz hfstart
	cmp qword [rsp+8],6 ;tap
	jz hf6start
	jmp hf2start
	
	align 16
hfstart:					;if (dwidth & 1)
	shr qword [rsp+8],1 ;tap
	pxor xmm3,xmm3
	align 16
hflop:
	mov rcx,r10 ;inPt
	mov rsi,qword [rsp+64] ;rgbTable
	mov rbp,qword [rsp+72] ;swidth
	mov rdi,qword [rsp+80] ;tmpbuff
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
;	lea rdi,[rdi+8]
;	lea rcx,[rcx+4]
	add rdi,8
	add rcx,4

	align 16
hflop4:
	mov eax,dword [rcx]
	mov ebx,dword [rcx+4]
	movzx rdx,al
	movq xmm1,[rsi+rdx*8+266240]
	movzx edx,bl
	movhps xmm1,[rsi+rdx*8+266240]
	movzx edx,ah
	movq xmm0,[rsi+rdx*8+264192]
	movzx edx,bh
	movhps xmm0,[rsi+rdx*8+264192]
	shr eax,16
	shr ebx,16
	paddsw xmm1,xmm0
	movzx edx,al
	movq xmm0,[rsi+rdx*8+262144]
	movzx edx,bl
	movhps xmm0,[rsi+rdx*8+262144]
	paddsw xmm1,xmm0
	movzx edx,ah
	movq xmm0,[rsi+rdx*8+268288]
	movzx edx,bh
	movhps xmm0,[rsi+rdx*8+268288]
	paddsw xmm1,xmm0
	movdqu [rdi],xmm1

;	lea rdi,[rdi+16]
;	lea rcx,[rcx+8]
	add rdi,16
	add rcx,8
	dec rbp
	jnz hflop4

	mov rbp,r8 ;dwidth
	mov rsi,r11 ;outPt
	mov rbx,qword [rsp+0] ;index
	mov rdi,qword [rsp+40] ;weight
	mov rcx,qword [rsp+80] ;tmpbuff
	align 16
hflop2:
	mov rdx,qword [rsp+8] ;tap
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
	
;	lea rdi,[rdi+16]
;	lea rbx,[rbx+16]
	add rdi,16
	add rbx,16
	dec rdx
	jnz hflop3

	psrad xmm2,15
	packssdw xmm2,xmm3
	movq [rsi],xmm2
;	lea rsi,[rsi+8]
	add rsi,8
	dec rbp
	jnz hflop2

	add r10,qword [rsp+48] ;sstep
	add r11,qword [rsp+56] ;dstep
	dec r9 ;currHeight
	jnz hflop
	jmp hfexit

	align 16
hf6start:								;else if (tap == 6)
	shr r8,1 ;dwidth
	shr qword [rsp+8],1 ;tap
	pxor xmm3,xmm3
	align 16
hf6lop:
	mov rcx,r10 ;inPt
	mov rsi,qword [rsp+64] ;rgbTable
	mov rbp,qword [rsp+72] ;swidth
	mov rdi,qword [rsp+80] ;tmpbuff

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
;	lea rdi,[rdi+8]
;	lea rcx,[rcx+4]
	add rdi,8
	add rcx,4

	align 16
hf6lop4:
	mov eax,dword [rcx]
	mov ebx,dword [rcx+4]
	movzx rdx,al
	movq xmm1,[rsi+rdx*8+266240]
	movzx edx,bl
	movhps xmm1,[rsi+rdx*8+266240]
	movzx edx,ah
	movq xmm0,[rsi+rdx*8+264192]
	movzx edx,bh
	movhps xmm0,[rsi+rdx*8+264192]
	shr eax,16
	shr ebx,16
	paddsw xmm1,xmm0
	movzx edx,al
	movq xmm0,[rsi+rdx*8+262144]
	movzx edx,bl
	movhps xmm0,[rsi+rdx*8+262144]
	paddsw xmm1,xmm0
	movzx edx,ah
	movq xmm0,[rsi+rdx*8+268288]
	movzx edx,bh
	movhps xmm0,[rsi+rdx*8+268288]
	paddsw xmm1,xmm0
	movdqu [rdi],xmm1

;	lea rdi,[rdi+16]
;	lea rcx,[rcx+8]
	add rdi,16
	add rcx,8
	dec rbp
	jnz hf6lop4

	mov rbp,r8 ;dwidth
	mov rsi,r11 ;outPt
	mov rbx,qword [rsp+0] ;index
	mov rdi,qword [rsp+40] ;weight
	mov rcx,qword [rsp+80] ;tmpbuff

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

;	lea rdi,[rdi+96]
;	lea rbx,[rbx+48]
	add rdi,96
	add rbx,48

	psrad xmm1,15
	psrad xmm3,15
	packssdw xmm1,xmm3
	movdqu [rsi],xmm1
;	lea rsi,[rsi+16]
	add rsi,16
	dec rbp
	jnz hf6lop2

	add r10,qword [rsp+48] ;sstep
	add r11,qword [rsp+56] ;dstep

	dec r9 ;currHeight
	jnz hf6lop
	jmp hfexit

	align 16
hf2start:
	shr r8,1 ;dwidth
	shr qword [rsp+8],1 ;tap
	pxor xmm3,xmm3
	align 16
hf2lop:
	mov rcx,r10 ;inPt
	mov rsi,qword [rsp+64] ;rgbTable
	mov rbp,qword [rsp+72] ;swidth
	mov rdi,qword [rsp+80] ;tmpbuff

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
;	lea rdi,[rdi+8]
;	lea rcx,[rcx+4]
	add rdi,8
	add rcx,4

	align 16
hf2lop4:
	mov eax,dword [rcx]
	mov ebx,dword [rcx+4]
	movzx rdx,al
	movq xmm1,[rsi+rdx*8+266240]
	movzx edx,bl
	movhpd xmm1,[rsi+rdx*8+266240]
	movzx edx,ah
	movq xmm0,[rsi+rdx*8+264192]
	movzx edx,bh
	movhpd xmm0,[rsi+rdx*8+264192]
	shr eax,16
	shr ebx,16
	paddsw xmm1,xmm0
	movzx edx,al
	movq xmm0,[rsi+rdx*8+262144]
	movzx edx,bl
	movhpd xmm0,[rsi+rdx*8+262144]
	paddsw xmm1,xmm0
	movzx edx,ah
	movq xmm0,[rsi+rdx*8+268288]
	movzx edx,bh
	movhpd xmm0,[rsi+rdx*8+268288]
	paddsw xmm1,xmm0
	movdqu [rdi],xmm1

;	lea rdi,[rdi+16]
;	lea rcx,[rcx+8]
	add rdi,16
	add rcx,8
	dec rbp
	jnz hf2lop4

	mov rbp,r8 ;dwidth
	mov rsi,r11 ;outPt
	mov rbx,qword [rsp+0] ;index
	mov rdi,qword [rsp+40] ;weight
	mov rcx,qword [rsp+80] ;tmpbuff
	align 16
hf2lop2:
	mov rdx,qword [rsp+8] ;tap
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
	
;	lea rdi,[rdi+16]
;	lea rbx,[rbx+16]
	add rdi,16
	add rbx,16
	dec rdx
	jnz hf2lop3

	mov rdx,qword [rsp+8] ;tap
	align 16
hf2lop3b:
	mov rax,qword [rbx]
	movq xmm0,[rcx+rax]
	mov rax,qword [rbx+8]
	movq xmm1,[rcx+rax]
	punpcklwd xmm0,xmm1
	pmaddwd xmm0,[rdi]
	paddd xmm3,xmm0
	
;	lea rdi,[rdi+16]
;	lea rbx,[rbx+16]
	add rdi,16
	add rbx,16
	dec rdx
	jnz hf2lop3b

	psrad xmm2,15
	psrad xmm3,15
	packssdw xmm2,xmm3
	movdqu [rsi],xmm2
;	lea rsi,[rsi+16]
	add rsi,16
	dec rbp
	jnz hf2lop2

	add r10,qword [rsp+48] ;sstep
	add r11,qword [rsp+56] ;dstep

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
;0 r15
;8 r14
;16 r13
;24 r12
;32 index
;40 tap
;48 rbx
;56 rbp
;64 retAddr
;rdi inPt r10
;rsi outPt r11
;rdx dwidth r8
;rcx height r9
;r8 tap     8
;r9 index   0
;72 weight
;80 sstep
;88 dstep
;96 rgbTable

	align 16
LanczosResizer8_C8_vertical_filter:
_LanczosResizer8_C8_vertical_filter:
	push rbp
	push rbx
	push r8
	push r9
	push r12
	push r13
	push r14
	push r15
	mov r10,rdi
	mov r11,rsi
	mov r8,rdx
	mov r9,rcx
	test r8,1 ;dwidth
	jnz vf1start
	cmp qword [rsp+40],6 ;tap
	jz vf6start
	jmp vfstart
	
	align 16
vf1start:					; if (dwidth & 1)
	mov r12,qword [rsp+40] ;tap
	shr r12,1
	mov rax,r8 ;dwidth
	shl rax,2
	mov rcx,r11 ;outPt
	sub qword [rsp+88],rax ;dstep
	pxor xmm3,xmm3
	
	mov r13,qword [rsp+32] ;index
	mov r14,qword [rsp+72] ;weight
	mov rdi,qword [rsp+96] ;rgbTable
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

	mov dword [rcx],eax
	lea rsi,[rsi+8]
	lea rcx,[rcx+4]
	dec rbp
	jnz vf1lop2

	mov rax,r12 ;tap
	shl rax,4
	add r13,rax ;index
	add r14,rax ;weight

	add rcx,qword [rsp+88] ;dstep

	dec r9 ;currHeight
	jnz vf1lop
	jmp vfexit

	align 16
vf6start:				;else if (tap == 6)
	mov rax,r8 ;dwidth
	shr qword [rsp+40],1 ;tap
	lea rdx,[rax*4]
	shr rax,1
	sub qword [rsp+88],rdx ;dstep
	mov r8,rax ;dwidth

	mov rcx,r11 ;outPt
	mov rdi,qword [rsp+32] ;index
	mov rbp,qword [rsp+96] ;rgbTable
	pxor xmm3,xmm3

	align 16
vf6lop4:
	mov rbx,qword [rsp+72] ;weight
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
	add qword [rsp+72],48 ;weight

	add rcx,qword [rsp+88] ;dstep

	dec r9 ;currHeight
	jnz vf6lop4
	jmp vfexit

	align 16
vfstart:
	mov r12,qword [rsp+40] ;tap
	shr r8,1 ;dwidth
	shr r12,1
	pxor xmm3,xmm3

	mov r13,qword [rsp+32] ;index
	mov r14,qword [rsp+72] ;weight
	mov rdi,qword [rsp+96] ;rgbTable
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

	pextrw rbx,xmm2,6
	pextrw rdx,xmm2,7
	mov al,byte [rdi+rbx+131072]
	mov ah,byte [rdi+rdx+196608]
	shl rax,16
	pextrw rbx,xmm2,4
	pextrw rdx,xmm2,5
	mov al,byte [rdi+rbx]
	mov ah,byte [rdi+rdx+65536]
	shl rax,16

	pextrw rbx,xmm2,2
	pextrw rdx,xmm2,3
	mov al,byte [rdi+rbx+131072]
	mov ah,byte [rdi+rdx+196608]
	shl rax,16
	pextrw rbx,xmm2,0
	pextrw rdx,xmm2,1
	mov al,byte [rdi+rbx]
	mov ah,byte [rdi+rdx+65536]

	mov qword [rcx],rax
	lea rcx,[rcx+8]
	lea rsi,[rsi+16]
	dec rbp
	jnz vflop5

	mov rax,r12 ;tap
	shl rax,4
	add r13,rax ;index
	add r14,rax ;weight

	add r11,qword [rsp+88] ;dstep
	
	dec r9 ;currHeight
	jnz vflop4

	align 16
vfexit:
	pop r15
	pop r14
	pop r13
	pop r12
	pop rdi
	pop rsi
	pop rbx
	pop rbp
	ret

;void LanczosResizer8_C8_expand(UInt8 *inPt, UInt8 *outPt, OSInt width, OSInt height, OSInt sstep, OSInt dstep, UInt8 *rgbTable)
;0 rbx
;8 rbp
;16 retAddr
;rdi inPt
;rsi outPt
;rdx width
;rcx height
;r8 sstep
;r9 dstep
;24 rgbTable

	align 16
LanczosResizer8_C8_expand:
_LanczosResizer8_C8_expand:
	push rbp
	push rbx
	
	lea rbx,[rdx*4] ;width
	lea rax,[rdx*8] ;width
	sub r8,rbx ;sstep
	sub r9,rax ;dstep
	mov rbp,qword [rsp+24] ;rgbTable
	xor rbx,rbx
	align 16
explop:
	mov rax,rdx ;width

	ALIGN 16
explop2:
	movzx ebx,byte [rdi+3]
	movq xmm1,[rbp+rbx*8+268288]
	movzx ebx,byte [rdi+2]
	movq xmm0,[rbp+rbx*8+262144]
	paddsw xmm1,xmm0
	movzx ebx,byte [rdi+1]
	movq xmm0,[rbp+rbx*8+264192]
	paddsw xmm1,xmm0
	movzx ebx,byte [rdi]
	movq xmm0,[rbp+rbx*8+266240]
	paddsw xmm1,xmm0
	movq [rsi],xmm1

	lea rsi,[rsi+8]
	lea rdi,[rdi+4]
	dec rax
	jnz explop2

	add rdi,r8 ;sstep
	add rsi,r9 ;dstep

	dec rcx
	jnz explop
	pop rbx
	pop rbp
	ret

;void LanczosResizer8_C8_expand_pa(UInt8 *inPt, UInt8 *outPt, OSInt width, OSInt height, OSInt sstep, OSInt dstep, UInt8 *rgbTable)
;0 rbx
;8 rbp
;16 retAddr
;rdi inPt
;rsi outPt
;rdx width
;rcx height
;r8 sstep
;r9 dstep
;24 rgbTable

	align 16
LanczosResizer8_C8_expand_pa:
_LanczosResizer8_C8_expand_pa:
	push rbp
	push rbx
	lea rbx,[rdx*4] ;width
	lea rax,[rdx*8] ;width
	sub r8,rbx ;sstep
	sub r9,rax ;dstep
	mov rbp,qword [rsp+24] ;rgbTable
	xor rbx,rbx
	align 16
exppalop:
	mov r10,rdx ;width
	ALIGN 16
exppalop2:
	mov eax,dword [rdi]
	movzx ebx,al
	movq xmm1,[rbp+rbx*8+266240]
	movzx ebx,ah
	movq xmm0,[rbp+rbx*8+264192]
	shr eax,16
	paddsw xmm1,xmm0
	movzx ebx,al
	movq xmm0,[rbp+rbx*8+262144]
	paddsw xmm1,xmm0
	movzx ebx,ah

	movd xmm0,ebx
	punpcklbw xmm0, xmm0
	punpcklbw xmm0, xmm0
	punpcklbw xmm0, xmm0
	pmulhuw xmm0, xmm1
	movq xmm1,[rbp+rbx*8+268288]
	paddsw xmm0,xmm1
	movq [rsi],xmm0

	lea rdi,[rdi+4]
	lea rsi,[rsi+8]
	dec r10
	jnz exppalop2

	add rdi,r8 ;sstep
	add rsi,r9 ;dstep

	dec rcx
	jnz exppalop
	pop rbx
	pop rbp
	ret

;void LanczosResizer8_C8_collapse(UInt8 *inPt, UInt8 *outPt, OSInt width, OSInt height, OSInt sstep, OSInt dstep, UInt8 *rgbTable)
;	Int64 toAdd = 0xff80ff80ff80ff80;
;	Int64 toAdd2 = 0x80808080;
;0 rbx
;8 rbp
;16 retAddr
;rdi inPt
;rsi outPt
;rdx width
;rcx height
;r8 sstep
;r9 dstep
;24 rgbTable

	align 16
LanczosResizer8_C8_collapse:
_LanczosResizer8_C8_collapse:
	push rbp
	push rbx
	lea rbx,[rdx*8] ;width
	lea rax,[rdx*4] ;width
	sub r8,rbx ;sstep
	sub r9,rax ;dstep
	mov rbp,qword [rsp+24] ;rgbTable
	xor ebx,ebx
	align 16
collop:
	mov r10,rdx ;width
	
	ALIGN 16
collop2:
	movzx ebx,word [rdi+6]
	mov ah,byte [rbp+rbx+196608]
	movzx ebx,word [rdi+4]
	mov al,byte [rbp+rbx+131072]
	shl eax,16
	movzx ebx,word [rdi+2]
	mov ah,byte [rbp+rbx+65536]
	movzx ebx,word [rdi]
	mov al,byte [rbp+rbx]

	mov dword [rsi],eax
	lea rsi,[rsi+4]
	lea rdi,[rdi+8]
	dec r10
	jnz collop2

	add rdi,r8 ;sstep
	add rsi,r9 ;dstep

	dec rcx
	jnz collop
	pop rbx
	pop rbp
	ret

;void LanczosResizer8_C8_imgcopy(UInt8 *inPt, UInt8 *outPt, OSInt width, OSInt height, OSInt sstep, OSInt dstep, UInt8 *rgbTable)
;0 rbx
;8 rbp
;16 retAddr
;rdi inPt
;rsi outPt
;rdx width
;rcx height
;r8 sstep
;r9 dstep
;24 rgbTable

	align 16
LanczosResizer8_C8_imgcopy:
_LanczosResizer8_C8_imgcopy:
	push rbp
	push rbx
	mov rbp,qword [rsp+24] ;rgbTable
	lea rax,[rdx*4] ;width
	sub r8,rax ;sstep
	sub r9,rax ;dstep
	align 16
iclop:
	mov r10,rdx ;width
	ALIGN 16
iclop2:
	movzx rax,byte [rdi+2]
	movq xmm1,[rbp+rax*8+262144]
	movzx rax,byte [rdi+1]
	movq xmm0,[rbp+rax*8+264192]
	paddsw xmm1,xmm0
	movzx rax,byte [rdi]
	movq xmm0,[rbp+rax*8+266240]
	paddsw xmm1,xmm0
	pextrw rbx,xmm1,2
	mov al,byte [rbp+rbx+131072]
	mov ah,byte [rdi+3]
	shl eax,16
	pextrw rbx,xmm1,1
	mov ah,byte [rbp+rbx+65536]
	pextrw rbx,xmm1,0
	mov al,byte [rbp+rbx]
	mov [rsi],rax
	lea rsi,[rsi+4]
	lea rdi,[rdi+4]
	dec r10
	jnz iclop2

	add rdi,r8 ;sstep
	add rsi,r9 ;dstep
	dec rcx
	jnz iclop
	pop rbx
	pop rbp
	ret

;void LanczosResizer8_C8_imgcopy_pa(UInt8 *inPt, UInt8 *outPt, OSInt width, OSInt height, OSInt sstep, OSInt dstep, UInt8 *rgbTable)
;0 rbx
;8 rbp
;16 retAddr
;rdi inPt
;rsi outPt
;rdx width
;rcx height
;r8 sstep
;r9 dstep
;24 rgbTable

	align 16
LanczosResizer8_C8_imgcopy_pa:
_LanczosResizer8_C8_imgcopy_pa:
	push rbp
	push rbx
	lea rax,[rdx*4] ;width
	shr rdx,1 ;width = width >> 1;
	sub r8,rax ;sstep
	sub r9,rax ;dstep

	mov rbp,qword [rsp+24] ;rgbTable
	align 16
icpalop:
	mov r10,rdx ;width
	xor rbx,rbx
	ALIGN 16
icpalop2:
	mov eax,dword [rdi]
	movzx rbx,al
	movq xmm1,[rbp+rbx*8+266240]
	movzx ebx,ah
	movq xmm0,[rbp+rbx*8+264192]
	shr eax,16
	paddsw xmm1,xmm0
	movzx ebx,al
	movq xmm0,[rbp+rbx*8+262144]
	paddsw xmm1,xmm0
	movzx ebx,ah
	movd xmm3,ebx

	mov eax,dword [rdi+4]
	movzx rbx,al
	movq xmm2,[rbp+rbx*8+266240]
	movzx ebx,ah
	movq xmm0,[rbp+rbx*8+264192]
	shr eax,16
	paddsw xmm2,xmm0
	movzx ebx,al
	movq xmm0,[rbp+rbx*8+262144]
	paddsw xmm2,xmm0
	punpcklqdq xmm1,xmm2
	movzx ebx,ah
	movd xmm0,ebx

	punpcklbw xmm3, xmm0
	punpcklbw xmm3, xmm3
	punpcklwd xmm3, xmm3
	punpckldq xmm3, xmm3
	pmulhuw xmm1, xmm3

	pextrw rax,xmm1,2
	movzx ebx,byte [rbp+rax+131072]
	mov bh,byte [rdi+3]
	pextrw rax,xmm1,1
	shl ebx,16
	mov bh,byte [rbp+rax+65536]
	pextrw rax,xmm1,0
	mov bl,byte [rbp+rax]
	mov dword [rsi],ebx

	pextrw rax,xmm1,6
	movzx ebx,byte [rbp+rax+131072]
	mov bh,byte [rdi+7]
	pextrw rax,xmm1,5
	shl ebx,16
	mov bh,byte [rbp+rax+65536]
	pextrw rax,xmm1,4
	mov bl,byte [rbp+rax]
	mov dword [rsi+4],ebx

	lea rsi,[rsi+8]
	lea rdi,[rdi+8]
	dec r10
	jnz icpalop2

	add rdi,r8 ;sstep
	add rsi,r9 ;dstep
	dec rcx
	jnz icpalop
	
	pop rbx
	pop rbp
	ret

