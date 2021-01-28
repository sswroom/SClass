section .text

global DeinterlaceLR_VerticalFilter
global DeinterlaceLR_VerticalFilterOdd
global DeinterlaceLR_VerticalFilterEven

;void DeinterlaceLR_VerticalFilter(UInt8 *inPt, UInt8 *outPt, OSInt width, OSInt height, OSInt tap, OSInt *index, Int64 *weight, OSInt sstep, OSInt dstep)
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
DeinterlaceLR_VerticalFilter:
	push rbp
	push rbx
	push rsi
	push rdi
	movdqu [rsp-32],xmm6
	movdqu [rsp-16],xmm7
	mov r10,rcx
	mov r11,rdx
	
	mov rax,qword [rsp+72] ;tap
	cmp rax,6
	jz vf6start
	cmp rax,4
	jz vf4start
	jmp vfstart
	
	align 16
vf6start:	
	mov rsi,r11 ;outPt
	pxor xmm3,xmm3
	lea rax,[r8*8] ;width
	sub qword [rsp+104],rax ;dstep
	mov rbx,qword [rsp+80] ;index
	mov rdi,qword [rsp+88] ;weight

	align 16
vf6lop:
	mov rcx,r10 ;inPt
	mov rbp,r8 ;width
	
	ALIGN 16
vf6lop2:

	pxor xmm1,xmm1
	mov rax,qword [rbx]
	movq xmm0,[rcx+rax]
	mov rax,qword [rbx+8]
	movq xmm2,[rcx+rax]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[rdi]
	paddd xmm1,xmm0
	mov rax,qword [rbx+16]
	movq xmm0,[rcx+rax]
	mov rax,qword [rbx+24]
	movq xmm2,[rcx+rax]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[rdi+16]
	paddd xmm1,xmm0
	mov rax,qword [rbx+32]
	movq xmm0,[rcx+rax]
	mov rax,qword [rbx+40]
	movq xmm2,[rcx+rax]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[rdi+32]
	paddd xmm1,xmm0

	psrad xmm1,15
	packssdw xmm1,xmm3
	movq [rsi],xmm1
	lea rsi,[rsi+8]
	lea rcx,[rcx+8]
	dec rbp
	jnz vf6lop2

	lea rbx,[rbx+48] ;index
	lea rdi,[rdi+48] ;weight
	add rsi,qword [rsp+104] ;dstep

	dec r9 ;currHeight
	jnz vf6lop
	jmp vfexit
	
	align 16
vf4start:
	mov rbx,qword [rsp+80] ;index
	mov rdi,qword [rsp+88] ;weight
	lea rax,[r8*8] ;width
	mov rsi,r11 ;outPt
	sub qword [rsp+104],rax ;dstep
	pxor xmm3,xmm3

	align 16
vf4lop:
	mov rcx,r10 ;inPt

	movdqa xmm6,[rdi]
	movdqa xmm7,[rdi+16]

	mov rbp,r8 ;width
	ALIGN 16
vf4lop2:
	pxor xmm1,xmm1
	mov rax,qword [rbx]
	movq xmm0,[rcx+rax]
	mov rax,qword [rbx+8]
	movq xmm2,[rcx+rax]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,xmm6
	paddd xmm1,xmm0
	mov rax,qword [rbx+16]
	movq xmm0,[rcx+rax]
	mov rax,qword [rbx+24]
	movq xmm2,[rcx+rax]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,xmm7
	paddd xmm1,xmm0

	psrad xmm1,15
	packssdw xmm1,xmm3
	movq [rsi],xmm1
	lea rsi,[rsi+8]
	lea rcx,[rcx+8]
	dec rbp
	jnz vf4lop2

	lea rbx,[rbx+32]
	lea rdi,[rdi+32]
	add rsi,qword [rsp+104] ;dstep

	dec r9 ;currHeight
	jnz vf4lop
	jmp vfexit

	align 16
vfstart:
	pxor xmm3,xmm3

	align 16
vflop:
	mov rsi,r11 ;outPt
	mov rcx,r10 ;inPt

	mov rbp,r8 ;width
	align 16
vflop2:
	mov rbx,qword [rsp+80] ;index
	mov rdi,qword [rsp+88] ;weight

	mov rdx,qword [rsp+72] ;tap
	pxor xmm1,xmm1
	ALIGN 16
vflop3:
	mov rax,qword [rbx]
	movq xmm0,[rcx+rax]
	mov rax,qword [rbx+8]
	movq xmm2,[rcx+rax]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[rdi]
	paddd xmm1,xmm0
	add rbx,16
	add rdi,16
	sub rdx,2
	jnz vflop3

	psrad xmm1,15
	packssdw xmm1,xmm3
	movq [rsi],xmm1
	lea rsi,[rsi+8]
	lea rcx,[rcx+8]
	dec rbp
	jnz vflop2

	mov rax,qword [rsp+72] ;tap
	lea rdx,[rax*8]
	add qword [rsp+80],rdx ;index
	add qword [rsp+88],rdx ;weight

	add r11,qword [rsp+104] ;dstep outPt

	dec r9 ;currHeight
	jnz vflop

	align 16
vfexit:
	movdqu xmm6,[rsp-32]
	movdqu xmm7,[rsp-16]
	pop rdi
	pop rsi
	pop rbx
	pop rbp
	ret

;void DeinterlaceLR_VerticalFilterOdd(UInt8 *inPt, UInt8 *inPtCurr, UInt8 *outPt, OSInt width, OSInt height, OSInt tap, OSInt *index, Int64 *weight, OSInt sstep, OSInt dstep)
;0 edi
;8 esi
;16 ebx
;24 ebp
;32 retAddr
;rcx inPt
;rdx inPtCurr
;r8 outPt
;r9 width
;72 height
;80 tap
;88 index
;96 weight
;104 sstep
;112 dstep

	align 16
DeinterlaceLR_VerticalFilterOdd:
	push rbp
	push rbx
	push rsi
	push rdi
	movdqu [rsp-32],xmm6
	movdqu [rsp-16],xmm7
	mov r10,rcx
	mov r11,rdx
	mov rax,qword [rsp+80] ;tap
	cmp rax,6
	jz vfo6start
	cmp rax,4
	jz vfo4start
	jmp vfostart

	align 16
vfo6start:
	mov rax,qword [rsp+72] ;height
	lea rdx,[r9*8] ;width
	shr rax,1
	mov qword [rsp+72],rax ;currHeight
	sub qword [rsp+112],rdx ;dstep
	pxor xmm3,xmm3

	add qword [rsp+88],48 ;index 8 * 6
	add qword [rsp+96],48 ;weight 8 * 6
	mov rdi,r8 ;outPt
	mov rbx,qword [rsp+88] ;index

	align 16
vfo6lop:
	mov rsi,r11 ;inPtCurr

	mov rbp,r9 ;width
	lea rcx,[rbp*2]
	rep movsd
	add r11,qword [rsp+104] ;sstep inPtCurr
	add rdi,qword [rsp+112] ;dstep

	mov rcx,qword [rsp+96] ;weight
	movdqa xmm4,[rcx]
	movdqa xmm5,[rcx+16]
	movdqa xmm6,[rcx+32]

	mov rsi,r10 ;inPt
	ALIGN 16
vfo6lop2:
	pxor xmm1,xmm1
	mov rax,qword [rbx]
	mov rdx,qword [rbx+8]
	movq xmm0,[rsi+rax]
	movq xmm2,[rsi+rdx]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,xmm4
	paddd xmm1,xmm0
	mov rax,qword [rbx+16]
	mov rdx,qword [rbx+24]
	movq xmm0,[rsi+rax]
	movq xmm2,[rsi+rdx]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,xmm5
	paddd xmm1,xmm0
	mov rax,qword [rbx+32]
	mov rdx,qword [rbx+40]
	movq xmm0,[rsi+rax]
	movq xmm2,[rsi+rdx]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,xmm6
	paddd xmm1,xmm0

	psrad xmm1,15
	packssdw xmm1,xmm3
	movq [rdi],xmm1
	lea rsi,[rsi+8]
	lea rdi,[rdi+8]
	dec rbp
	jnz vfo6lop2

	add rbx,96 ;8 * 6 * 2
	add qword [rsp+96],96 ;weight 8 * 6 * 2
	add rdi,[rsp+112] ;dstep

	dec qword [rsp+72] ;currHeight
	jnz vfo6lop
	jmp vfoexit

	align 16
vfo4start:
	test r9,7 ;width
	jz vfo4start2			;if (width & 7)
	
	mov rax,qword [rsp+72] ;height
	shr rax,1
	mov qword [rsp+72],rax ;currHeight
	shr r9,1 ;width

	mov rbx,qword [rsp+88] ;index
	add qword [rsp+96],32 ;weight 8 * 4
	add rbx,32 ;8 * 4

	align 16
vfo4_7lop:
	mov rdi,r8 ;outPt
	mov rsi,r11 ;inPtCurr

	mov rax,r9 ;width
	mov rcx,rax
	shr rax,2
	jz vfo4_7lop4
	ALIGN 16
vfo4_7lop3:
	movdqu xmm0,[rsi]
	movdqu xmm1,[rsi+16]
	movdqu xmm2,[rsi+32]
	movdqu xmm3,[rsi+48]
	movntdq [rdi],xmm0
	movntdq [rdi+16],xmm1
	movntdq [rdi+32],xmm2
	movntdq [rdi+48],xmm3
	lea rsi,[rsi+64]
	lea rdi,[rdi+64]
	dec rax
	jnz vfo4_7lop3
	align 16
vfo4_7lop4:
	and rcx,3
	jz vfo4_7lop6
	ALIGN 16
vfo4_7lop5:
	movdqu xmm0,[rsi]
	movntdq [rdi],xmm0
	lea rsi,[rsi+16]
	lea rdi,[rdi+16]
	dec rcx
	jnz vfo4_7lop5
	align 16
vfo4_7lop6:
	mov rdi,r8 ;outPt
	add r11,qword [rsp+104] ;sstep inPtCurr
	add rdi,qword [rsp+112] ;dstep
	mov rcx,qword [rsp+96] ;weight

	movdqa xmm4,[rcx]
	movdqa xmm5,[rcx+16]

	mov rsi,r10 ;inPt
	mov rcx,r9 ;width
	ALIGN 16
vfo4_7lop2:
	pxor xmm1,xmm1
	pxor xmm6,xmm6

	mov rax,qword [rbx]
	mov rdx,qword [rbx+8]
	movdqa xmm0,[rsi+rax]
	movdqa xmm2,[rsi+rdx]
	movdqa xmm3,xmm0
	punpcklwd xmm0,xmm2
	punpckhwd xmm3,xmm2
	pmaddwd xmm0, xmm4
	pmaddwd xmm3, xmm4
	paddd xmm1,xmm0
	paddd xmm6,xmm3
	mov rax,qword [rbx+16]
	mov rdx,qword [rbx+24]
	movdqa xmm0,[rsi+rax]
	movdqa xmm2,[rsi+rdx]
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
	movntdq [rdi],xmm1
	add rsi,16
	add rdi,16
	dec rcx
	jnz vfo4_7lop2

	add rbx,64 ;8 * 4 * 2
	add qword [rsp+96],64 ;weight 8 * 4 * 2

	mov rdx,qword [rsp+112] ;dstep
	shl rdx,1
	add r8,rdx ;outPt

	dec qword [rsp+72] ;currHeight
	jnz vfo4_7lop
	jmp vfoexit

	align 16
vfo4start2:
	mov rax,qword [rsp+72] ;height
	mov rdx,r9 ;width
	shr rax,1
	mov qword [rsp+72],rax ;currHeight
	lea rcx,[rdx*8]
	shr rdx,1
	sub qword [rsp+112],rcx ;dstep
	sub qword [rsp+104],rcx ;sstep
	mov r9,rdx ;width

	mov rdi,r8 ;outPt
	mov rbp,qword [rsp+96] ;weight
	mov rbx,qword [rsp+88] ;index
	mov rdx,r11 ;inPtCurr
	lea rbp,[rbp+32] ;8 * 4
	lea rbx,[rbx+32] ;8 * 4

	ALIGN 16
vfo4lopa:
	mov rax,r9 ;width
	shr rax,2
	ALIGN 16
vfo4lopa3:
	movdqu xmm0,[rdx]
	movdqu xmm1,[rdx+16]
	movdqu xmm2,[rdx+32]
	movdqu xmm3,[rdx+48]
	movntdq [rdi],xmm0
	movntdq [rdi+16],xmm1
	movntdq [rdi+32],xmm2
	movntdq [rdi+48],xmm3
	lea rdx,[rdx+64]
	lea rdi,[rdi+64]
	dec rax
	jnz vfo4lopa3

	add rdx,qword [rsp+104] ;sstep
	add rdi,qword [rsp+112] ;dstep

	movdqa xmm4,[rbp]
	movdqa xmm5,[rbp+16]

	mov rsi,r10 ;inPt
	mov rcx,r9 ;width
	ALIGN 16
vfo4lopa2:
	pxor xmm1,xmm1
	pxor xmm6,xmm6

	mov rax,qword [rbx]
	movdqa xmm0,[rsi+rax]
	mov rax,qword [rbx+8]
	movdqa xmm2,[rsi+rax]
	movdqa xmm3,xmm0
	punpcklwd xmm0,xmm2
	punpckhwd xmm3,xmm2
	pmaddwd xmm0, xmm4
	pmaddwd xmm3, xmm4
	paddd xmm1,xmm0
	paddd xmm6,xmm3
	mov rax,qword [rbx+16]
	movdqa xmm0,[rsi+rax]
	mov rax,qword [rbx+24]
	movdqa xmm2,[rsi+rax]
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
	movntdq [rdi],xmm1
	lea rsi,[rsi+16]
	lea rdi,[rdi+16]
	dec rcx
	jnz vfo4lopa2

	lea rbx,[rbx+64] ;8 * 4 * 2
	lea rbp,[rbp+64] ;8 * 4 * 2

	add rdi,qword [rsp+112] ;dstep

	dec qword [rsp+72] ;currHeight
	jnz vfo4lopa
	jmp vfoexit

	align 16
vfostart:
	pxor xmm3,xmm3

	align 16
vfolop:
	mov rsi,r8 ;outPt
	mov rcx,r10 ;inPt

	mov rbp,r9 ;width

	align 16
vfolop2:
	mov rbx,qword [rsp+88] ;index
	mov rdi,qword [rsp+96] ;weight

	mov rdx,qword [rsp+80] ;tap
	pxor xmm1,xmm1
	ALIGN 16
vfolop3:
	mov rax,qword [rbx]
	movq xmm0,[rcx+rax]
	mov rax,qword [rbx+8]
	movq xmm2,[rcx+rax]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[rdi]
	paddd xmm1,xmm0
	add rbx,16
	add rdi,16
	sub rdx,2
	jnz vfolop3

	psrad xmm1,15
	packssdw xmm1,xmm3
	movq [rsi],xmm1
	add rsi,8
	add rcx,8
	dec rbp
	jnz vfolop2

	mov rax,qword [rsp+80] ;tap
	lea rdx,[rax*8]
	add qword [rsp+88],rdx ;index
	add qword [rsp+96],rdx ;weight

	add r8,qword [rsp+112] ;dstep outPt
	dec qword [rsp+72] ;currHeight
	jnz vfolop

	align 16
vfoexit:
	movdqu xmm6,[rsp-32]
	movdqu xmm7,[rsp-16]
	pop rdi
	pop rsi
	pop rbx
	pop rbp
	ret


;void DeinterlaceLR_VerticalFilterEven(UInt8 *inPt, UInt8 *inPtCurr, UInt8 *outPt, OSInt width, OSInt height, OSInt tap, OSInt *index, Int64 *weight, OSInt sstep, OSInt dstep)
;0 rdi
;8 rsi
;16 rbx
;24 rbp
;32 retAddr
;rcx inPt r10
;rdx inPtCurr r11
;r8 outPt
;r9 width
;72 height
;80 tap
;88 index
;96 weight
;104 sstep
;112 dstep

	align 16
DeinterlaceLR_VerticalFilterEven:
	push rbp
	push rbx
	push rsi
	push rdi
	movdqu [rsp-32],xmm6
	movdqu [rsp-16],xmm7
	mov r10,rcx
	mov r11,rdx
	mov rax,qword [rsp+80] ;tap
	cmp rax,6
	jz vfe6start
	cmp rax,4
	jz vfe4start
	jmp vfestart

	align 16
vfe6start:	
	mov rax,qword [rsp+72] ;height
	shr rax,1
	mov qword [rsp+72],rax ;currHeight
	pxor xmm3,xmm3
	mov rbx,qword [rsp+88] ;index
	mov rdx,qword [rsp+96] ;weight

	align 16
vfe6lop:
	mov rdi,r8 ;outPt
	mov rsi,r10 ;inPt

	movdqa xmm4,[rdx]
	movdqa xmm5,[rdx+16]
	movdqa xmm6,[rdx+32]

	mov rbp,r9 ;width
	ALIGN 16
vfe6lop2:
	pxor xmm1,xmm1
	mov rax,qword [rbx]
	movq xmm0,[rsi+rax]
	mov rax,qword [rbx+8]
	movq xmm2,[rsi+rax]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,xmm4
	paddd xmm1,xmm0
	mov rax,qword [rbx+16]
	movq xmm0,[rsi+rax]
	mov rax,qword [rbx+24]
	movq xmm2,[rsi+rax]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,xmm5
	paddd xmm1,xmm0
	mov rax,qword [rbx+32]
	movq xmm0,[rsi+rax]
	mov rax,qword [rbx+40]
	movq xmm2,[rsi+rax]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,xmm6
	paddd xmm1,xmm0

	psrad xmm1,15
	packssdw xmm1,xmm3
	movq [rdi],xmm1
	add rsi,8
	add rdi,8
	dec rbp
	jnz vfe6lop2

	add ebx,96 ;8 * 6 * 2
	add edx,96 ;8 * 6 * 2

	add r8,qword [rsp+112] ;dstep outPt

	mov rsi,r11 ;inPtCurr
	mov rdi,r8 ;outPt
	mov rcx,r9 ;width
	shl rcx,1
	rep movsd
	add r11,qword [rsp+104] ;sstep inPtCurr
	add r8,qword [rsp+112] ;dstep outPt

	dec qword [rsp+72] ;currHeight
	jnz vfe6lop
	jmp vfeexit

	align 16
vfe4start:
	test r9,7 ;width
	jz vfe4start2								;if (width & 7)

	mov rax,qword [rsp+72] ;height
	shr rax,1
	mov qword [rsp+72],rax ;currHeight

	mov rbx,qword [rsp+88] ;index
	mov rdx,qword [rsp+96] ;weight
	ALIGN 16
vfe4_7lop:
	mov rdi,r8 ;outPt
	mov rsi,r10 ;inPt

	movdqa xmm4,[rcx]
	movdqa xmm5,[rcx+16]

	mov rcx,r9 ;width
	shr rcx,1
	ALIGN 16
vfe4_7lop2:
	pxor xmm1,xmm1
	pxor xmm6,xmm6

	mov rax,qword [rbx]
	movdqa xmm0,[rsi+rax]
	mov rax,qword [rbx+8]
	movdqa xmm2,[rsi+rax]
	movdqa xmm3,xmm0
	punpcklwd xmm0,xmm2
	punpckhwd xmm3,xmm2
	pmaddwd xmm0,xmm4
	pmaddwd xmm3,xmm4
	paddd xmm1,xmm0
	paddd xmm6,xmm3
	mov rax,qword [rbx+16]
	movdqa xmm0,[rsi+rax]
	mov rax,qword [rbx+24]
	movdqa xmm2,[rsi+rax]
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
	movdqa [rdi],xmm1
	add rsi,16
	add rdi,16
	dec rcx
	jnz vfe4_7lop2

	add rbx,64 ;4 * 4 * 2
	add rdx,64 ;8 * 4 * 2

	add r8,qword [rsp+112] ;dstep outPt

	mov rsi,r11 ;inPtCurr
	mov rdi,r8 ;outPt
	mov rcx,r9 ;width
	mov rax,rcx
	shr rcx,2
	jz vfe4_7lop4
	ALIGN 16
vfe4_7lop3:
	movdqu xmm0,[rsi]
	movdqu xmm1,[rsi+16]
	movdqu xmm2,[rsi+32]
	movdqu xmm3,[rsi+48]
	movntdq [rdi],xmm0
	movntdq [rdi+16],xmm1
	movntdq [rdi+32],xmm2
	movntdq [rdi+48],xmm3
	add rsi,64
	add rdi,64
	dec rcx
	jnz vfe4_7lop3
	align 16
vfe4_7lop4:
	and rax,3
	jz vfe4_7lop6
	ALIGN 16
vfe4_7lop5:
	movdqu xmm0,[rsi]
	movntdq [rdi],xmm0
	add rsi,16
	add rdi,16
	dec rax
	jnz vfe4_7lop5
	align 16
vfe4_7lop6:
	add r11,qword [rsp+104] ;sstep inPtCurr
	add r8,qword [rsp+112] ;dstep outPt

	dec qword [rsp+72] ;currHeight
	jnz vfe4_7lop
	jmp vfeexit

	align 16
vfe4start2:
	mov rax,qword [rsp+72] ;height
	shr rax,1
	mov qword [rsp+72],rax ;currHeight
	shr r9,1 ;width

	mov rdx,r8 ;outPt
	mov rbp,qword [rsp+96] ;weight
	mov rbx,qword [rsp+88] ;index
	ALIGN 16
vfe4lopa:
	mov rdi,rdx
	mov rsi,r10 ;inPt

	movdqa xmm4,[rbp]
	movdqa xmm5,[rbp+16]

	mov rcx,r9 ;width
	ALIGN 16
vfe4lopa2:
	pxor xmm1,xmm1
	pxor xmm6,xmm6

	mov rax,qword [rbx]
	movdqa xmm0,[rsi+rax]
	mov rax,qword [rbx+8]
	movdqa xmm2,[rsi+rax]
	movdqa xmm3,xmm0
	punpcklwd xmm0,xmm2
	punpckhwd xmm3,xmm2
	pmaddwd xmm0,xmm4
	pmaddwd xmm3,xmm4
	paddd xmm1,xmm0
	paddd xmm6,xmm3
	mov rax,qword [rbx+16]
	movdqa xmm0,[rsi+rax]
	mov rax,qword [rbx+24]
	movdqa xmm2,[rsi+rax]
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
	movntdq [rdi],xmm1
	lea rsi,[rsi+16]
	lea rdi,[rdi+16]
	dec rcx
	jnz vfe4lopa2

	lea rbx,[rbx+64] ;8 * 4 * 2
	lea rbp,[rbp+64] ;8 * 4 * 2

	add rdx,qword [rsp+112] ;dstep

	mov rsi,r11 ;inPtCurr
	mov rdi,rdx
	mov rcx,r9 ;width
	shr rcx,2
	ALIGN 16
vfe4lopa3:
	movdqu xmm0,[rsi]
	movdqu xmm1,[rsi+16]
	movdqu xmm2,[rsi+32]
	movdqu xmm3,[rsi+48]
	movntdq [rdi],xmm0
	movntdq [rdi+16],xmm1
	movntdq [rdi+32],xmm2
	movntdq [rdi+48],xmm3
	lea rsi,[rsi+64]
	lea rdi,[rdi+64]
	dec rcx
	jnz vfe4lopa3

	add r11,qword [rsp+104] ;sstep inPtCurr
	add rdx,qword [rsp+112] ;dstep

	dec qword [rsp+72] ;currHeight
	jnz vfe4lopa
	jmp vfeexit

	align 16
vfestart:
	pxor xmm3,xmm3

	align 16
vfelop:
	mov rsi,r8 ;outPt
	mov rcx,r10 ;inPt

	mov rbp,r9 ;width
	align 16
vfelop2:
	mov rbx,qword [rsp+88] ;index
	mov rdi,qword [rsp+96] ;weight

	mov rdx,qword [rsp+80] ;tap
	pxor xmm1,xmm1
	ALIGN 16
vfelop3:
	mov rax,qword [rbx]
	movq xmm0,[rcx+rax]
	mov rax,qword [rbx+8]
	movq xmm2,[rcx+rax]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[rdi]
	paddd xmm1,xmm0
	add rbx,16
	add rdi,16
	sub rdx,2
	jnz vflop3

	psrad xmm1,15
	packssdw xmm1,xmm3
	movq [rsi],xmm1
	add rsi,8
	add rcx,8
	dec rbp
	jnz vfelop2

	mov rax,qword [rsp+80] ;tap
	lea rdx,[rax*8]
	add qword [rsp+88],rdx ;index
	add qword [rsp+96],rdx ;weight

	add r8,qword [rsp+112] ;dstep outPt

	dec qword [rsp+72] ;currHeight
	jnz vfelop
	
	align 16
vfeexit:
	movdqu xmm6,[rsp-32]
	movdqu xmm7,[rsp-16]
	pop rdi
	pop rsi
	pop rbx
	pop rbp
	ret
