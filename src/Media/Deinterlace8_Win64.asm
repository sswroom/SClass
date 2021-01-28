section .text

global Deinterlace8_VerticalFilter
global Deinterlace8_VerticalFilterOdd
global Deinterlace8_VerticalFilterEven

;void Deinterlace8_VerticalFilter(UInt8 *inPt, UInt8 *outPt, OSInt width, OSInt height, OSInt tap, OSInt *index, Int64 *weight, OSInt sstep, OSInt dstep);
;0 rdi
;8 rsi
;16 rbx
;24 rbp
;32 retAddr
;rcx inPt
;rdx outPt
;r8 width
;r9 height
;72 tap
;80 index
;88 weight
;96 sstep
;104 dstep

	align 16
Deinterlace8_VerticalFilter:
	push rbp
	push rbx
	push rsi
	push rdi
	movdqu [rsp-32],xmm6
	movdqu [rsp-16],xmm7
	mov r10,rcx
	mov r11,rdx
	shr r8,1 ;width
	pxor xmm6,xmm6

	align 16
vflop:
	mov rcx,r10 ;inPt
	mov rsi,r11 ;outPt
	mov rbp,r8 ;width
	
	align 16
vflop2:
	mov rbx,qword [rsp+80] ;index
	mov rdi,qword [rsp+88] ;weight
	mov rdx,qword [rsp+72] ;tap
	pxor xmm1,xmm1
	pxor xmm3,xmm3
	ALIGN 16
vflop3:
	mov rax,qword [rbx]
	movq xmm0,[rcx+rax]
	mov rax,qword [rbx+8]
	movq xmm2,[rcx+rax]
	movdqa xmm5,[rdi]
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
	add rbx,16
	add rdi,16
	sub rdx,2
	jnz vflop3

	psrad xmm1,22
	psrad xmm3,22
	packssdw xmm1,xmm3
	packuswb xmm1,xmm6
	movq [rsi],xmm1
	add rsi,8
	add rcx,8
	dec rbp
	jnz vflop2

	mov rax,qword [rsp+72] ;tap
	add r11,qword [rsp+104] ;dstep outPt
	shl rax,3
	add qword [rsp+80],rax ;index
	add qword [rsp+88],rax ;weight

	dec r9 ;currHeight
	jnz vflop
	
	movdqu xmm6,[rsp-32]
	movdqu xmm7,[rsp-16]
	pop rdi
	pop rsi
	pop rbx
	pop rbp
	ret

;void Deinterlace8_VerticalFilterOdd(UInt8 *inPt, UInt8 *inPtCurr, UInt8 *outPt, OSInt width, OSInt height, OSInt tap, OSInt *index, Int64 *weight, OSInt sstep, OSInt dstep);
;0 rdi
;8 rsi
;16 rbx
;24 rbp
;32 retAddr
;rcx inPt
;rdx inptCurr
;r8 outPt
;r9 width
;72 height
;80 tap
;88 index
;96 weight
;104 sstep
;112 dstep

	align 16
Deinterlace8_VerticalFilterOdd:
	push rbp
	push rbx
	push rsi
	push rdi
	movdqu [rsp-32],xmm6
	movdqu [rsp-16],xmm7
	mov r10,rcx
	mov r11,rdx
	test r9,3 ;width
	jnz vfosstart
	test r8,15 ;outPt
	jnz vfosstart
	test qword [rsp+112],15 ;dstep
	jnz vfosstart
	jmp vfostart
	
	align 16
vfosstart:							;if ((width & 3) != 0 || (((OSInt)outPt) & 15) != 0 || (dstep & 15) != 0)
	mov rax,qword [rsp+72] ;height
	shr r9,1 ;width
	shr rax,1
	mov qword [rsp+72],rax ;currHeight
	pxor xmm6,xmm6

	align 16
vfoslop:
	mov rsi,r8 ;outPt
	mov rcx,r11 ;inPtCurr

	mov rax,r9 ;width
	shr rax,1
	ALIGN 16
vfoslop4:
	movdqu xmm1,[rcx]
	movdqu [rsi],xmm1
	add rcx,16
	add rsi,16
	dec rax
	jnz vfoslop4
	add r11,qword [rsp+104] ;sstep inPtCurr
	add r8,qword [rsp+112] ;dstep outPt

	mov rax,qword [rsp+80] ;tap
	lea rbx,[rax*8]
	add qword [rsp+88],rbx ;index
	add qword [rsp+96],rbx ;weight

	mov rsi,r8 ;outPt
	mov rcx,r10 ;inPt
	mov rbp,r9 ;width

	align 16
vfoslop2:
	mov rbx,qword [rsp+88] ;index
	mov rdi,qword [rsp+96] ;weight

	mov rdx,qword [rsp+80] ;tap
	pxor xmm1,xmm1
	pxor xmm3,xmm3
	ALIGN 16
vfoslop3:
	mov rax,qword [rbx]
	movq xmm0,[rcx+rax]
	mov rax,qword [rbx+8]
	movq xmm2,[rcx+rax]
	movdqa xmm5,[rdi]
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
	add rbx,16
	add rdi,16
	sub rdx,2
	jnz vfoslop3

	psrad xmm1,22
	psrad xmm3,22
	packssdw xmm1,xmm3
	packuswb xmm1,xmm6
	movq [rsi],xmm1
	add rsi,8
	add rcx,8
	dec rbp
	jnz vfoslop2

	mov rax,qword [rsp+80] ;tap
	add r8,qword [rsp+112] ;dstep outPt
	lea rbx,[rax*8]
	add qword [rsp+88],rbx ;index
	add qword [rsp+96],rbx ;weight

	dec qword [rsp+72] ;currHeight
	jnz vfoslop
	jmp vfoexit
			
	align 16
vfostart:
	mov rax,qword [rsp+72] ;height
	shr r9,2 ;width
	shr rax,1
	mov qword [rsp+72],rax ;currHeight
	pxor xmm6,xmm6

	align 16
vfolop:
	mov rsi,r8 ;outPt
	mov rcx,r11 ;inPtCurr

	mov rax,r9 ;width
	ALIGN 16
vfolop4:
	movdqu xmm1,[rcx]
	movntdq [rsi],xmm1
	add rcx,16
	add rsi,16
	dec rax
	jnz vfolop4
	add r11,qword [rsp+104] ;sstep inPtCurr
	add r8,qword [rsp+112] ;dstep outPt

	mov rax,qword [rsp+80] ;tap
	lea rbx,[rax*8]
	add qword [rsp+88],rbx ;index
	add qword [rsp+96],rbx ;weight

	mov rsi,r8 ;outPt
	mov rcx,r11 ;inPt

	mov rbp,r9 ;width
	align 16
vfolop2:
	mov rbx,qword [rsp+88] ;index
	mov rdi,qword [rsp+96] ;weight

	mov rdx,qword [rsp+80] ;tap
	pxor xmm4,xmm4
	pxor xmm5,xmm5
	pxor xmm6,xmm6
	pxor xmm7,xmm7
	ALIGN 16
vfolop3:
	mov rax,qword [rbx]
	movq xmm2,[rcx+rax]
	mov rax,qword [rbx+8]
	movq xmm3,[rcx+rax]
	movdqa xmm0,[rdi]
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

	mov rax,qword [rbx]
	movq xmm2,[rcx+rax+8]
	mov rax,qword [rbx+8]
	movq xmm3,[rcx+rax+8]
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

	add rbx,16
	add rdi,16
	sub rdx,2
	jnz vfolop3

	psrad xmm4,22
	psrad xmm5,22
	psrad xmm6,22
	psrad xmm7,22
	packssdw xmm4,xmm5
	packssdw xmm6,xmm7
	packuswb xmm4,xmm6
	movntdq [rsi],xmm4
	add rsi,16
	add rcx,16
	dec rbp
	jnz vfolop2

	mov rax,qword [rsp+80] ;tap
	add r8,qword [rsp+112] ;dstep outPt
	lea rbx,[rax*8]
	add qword [rsp+88],rbx ;index
	add qword [rsp+96],rbx ;weight

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

;void Deinterlace8_VerticalFilterEven(UInt8 *inPt, UInt8 *inPtCurr, UInt8 *outPt, OSInt width, OSInt height, OSInt tap, OSInt *index, Int64 *weight, OSInt sstep, OSInt dstep);
;0 rdi
;8 rsi
;16 rbx
;24 rbp
;32 retAddr
;rcx inPt r10
;rdx inptCurr r11
;r8 outPt
;r9 width
;72 height
;80 tap
;88 index
;96 weight
;104 sstep
;112 dstep

	align 16
Deinterlace8_VerticalFilterEven:
	push rbp
	push rbx
	push rsi
	push rdi
	movdqu [rsp-32],xmm6
	movdqu [rsp-16],xmm7
	mov r10,rcx
	mov r11,rdx
	test r9,3 ;width
	jnz vfesstart
	test r8,15 ;outPt
	jnz vfesstart
	test qword [rsp+112],15 ;dstep
	jnz vfesstart
	jmp vfestart

	align 16
vfesstart:								;if ((width & 3) != 0 || (((OSInt)outPt) & 15) != 0 || (dstep & 15) != 0)
	mov rax,qword [rsp+72] ;height
	shr r9,1 ;width
	shr rax,1
	mov qword [rsp+72],rax ;currHeight
	pxor xmm6,xmm6

	align 16
vfeslop:
	mov rsi,r8 ;outPt
	mov rcx,r10 ;inPt

	mov rbp,r9 ;width

	align 16
vfeslop2:
	mov rbx,qword [rsp+88] ;index
	mov rdi,qword [rsp+96] ;weight

	mov rdx,qword [rsp+80] ;tap
	pxor xmm1,xmm1
	pxor xmm3,xmm3
	ALIGN 16
vfeslop3:
	mov rax,qword [rbx]
	movq xmm0,[rcx+rax]
	mov rax,qword [rbx+8]
	movq xmm2,[rcx+rax]
	movdqa xmm5,[rdi]
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
	add rbx,16
	add rdi,16
	sub rdx,2
	jnz vfeslop3

	psrad xmm1,22
	psrad xmm3,22
	packssdw xmm1,xmm3
	packuswb xmm1,xmm6
	movq [rsi],xmm1
	add rsi,8
	add rcx,8
	dec rbp
	jnz vfeslop2

	mov rax,qword [rsp+80] ;tap
	add r8,qword [rsp+112] ;dstep outPt
	lea rbx,[rax*8]
	add qword [rsp+88],rbx ;index
	add qword [rsp+96],rbx ;weight

	mov rcx,r11 ;inPtCurr
	mov rax,r9 ;width
	shr rax,1
	ALIGN 16
vfeslop4:
	movdqu xmm1,[rcx]
	movdqu [rsi],xmm1
	add rcx,16
	add rsi,16
	dec rax
	jnz vfeslop4
	add r11,qword [rsp+104] ;sstep inPtCurr
	add r8,qword [rsp+112] ;dstep outPt

	mov rax,qword [rsp+80] ;tap
	lea rbx,[rax*8]
	add qword [rsp+88],rbx ;index
	add qword [rsp+96],rbx ;weight

	dec qword [rsp+72] ;currHeight
	jnz vfeslop
	jmp vfeexit

	align 16
vfestart:
	mov rax,qword [rsp+72] ;height
	shr r9,2 ;width
	shr rax,1
	mov qword [rsp+72],rax ;currHeight
	pxor xmm6,xmm6

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
	pxor xmm4,xmm4
	pxor xmm5,xmm5
	pxor xmm6,xmm6
	pxor xmm7,xmm7
	ALIGN 16
vfelop3:
	mov rax,qword [rbx]
	movq xmm2,[rcx+rax]
	mov rax,qword [rbx+8]
	movq xmm3,[rcx+rax]
	movdqa xmm0,[rdi]
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

	mov rax,qword [rbx]
	movq xmm2,[rcx+rax+8]
	mov rax,qword [rbx+8]
	movq xmm3,[rcx+rax+8]
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

	add rbx,16
	add rdi,16
	sub rdx,2
	jnz vfelop3

	psrad xmm4,22
	psrad xmm5,22
	psrad xmm6,22
	psrad xmm7,22
	packssdw xmm4,xmm5
	packssdw xmm6,xmm7
	packuswb xmm4,xmm6
	movntdq [rsi],xmm4
	add rsi,16
	add rcx,16
	dec rbp
	jnz vfelop2

	mov rax,qword [rsp+80] ;tap
	add r8,qword [rsp+112] ;dstep outPt
	lea rbx,[rax*8]
	add qword [rsp+88],rbx ;index
	add qword [rsp+96],rbx ;weight

	mov rcx,r11 ;inPtCurr
	mov rsi,r8 ;outPt
	mov rax,r9 ;width
	ALIGN 16
vfelop4:
	movdqu xmm1,[rcx]
	movntdq [rsi],xmm1
	add rcx,16
	add rsi,16
	dec rax
	jnz vfelop4
	add r11,qword [rsp+104] ;sstep inPtCurr
	add r8,qword [rsp+112] ;dstep outPt

	mov rax,qword [rsp+80] ;tap
	lea rbx,[rax*8]
	add qword [rsp+88],rbx ;index
	add qword [rsp+96],rbx ;weight

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
	