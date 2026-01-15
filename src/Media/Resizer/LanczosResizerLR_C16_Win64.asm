section .text

global LanczosResizerLR_C16_horizontal_filter
global LanczosResizerLR_C16_vertical_filter
global LanczosResizerLR_C16_collapse

;void LanczosResizerLR_C16_horizontal_filter(UInt8 *inPt, UInt8 *outPt, IntOS width, IntOS height, Int32 tap, Int32 *index, Int64 *weight, IntOS sstep, IntOS dstep, UInt8 *rgbTable)
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
;112 rgbTable

	align 16
LanczosResizerLR_C16_horizontal_filter:
	push rbp
	push rbx
	push rsi
	push rdi
	mov r10,rcx
	mov r11,rdx
	test r8,1 ;width
	jnz hfstart
	mov rax,qword [rsp+72] ;tap
	cmp rax,6
	jz hf6start
	jmp hfstart2
	
	align 16
hfstart:							;if (width & 1)
	shr qword [rsp+72],1 ;tap
	pxor xmm3,xmm3
	align 16
hflop:
	mov rsi,r11 ;outPt
	mov rbp,r8 ;width
	mov rdi,qword [rsp+88] ;weight
	mov rbx,qword [rsp+80] ;index
	align 16
hflop2:
	mov rcx,r10 ;inPt
	mov rdx,qword [rsp+72] ;tap
	pxor xmm1,xmm1
	ALIGN 16
hflop3:
	mov rax,qword [rbx]
	movq xmm0,[rcx+rax]
	mov rax,qword [rbx+8]
	movq xmm2,[rcx+rax]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[rdi]
	paddd xmm1,xmm0
	lea rdi,[rdi+16]
	lea rbx,[rbx+16]
	dec rdx
	jnz hflop3

	psrad xmm1,15
	packssdw xmm1,xmm3
	movq [rsi],xmm1
	lea rsi,[rsi+8]
	dec rbp
	jnz hflop2

	add r10,qword [rsp+96] ;sstep inPt
	add r11,qword [rsp+104] ;dstep outPt

	dec r9 ;currHeight
	jnz hflop
	jmp hfexit

	align 16
hf6start:								;else if (tap == 6)
	test r11,15 ;outPt
	jnz hf6nstart
	test qword [rsp+104],15 ;dstep
	jnz hf6nstart
	jmp hf6start2
	
	align 16
hf6nstart:								;if ((((IntOS)outPt) & 15) == 0 && (dstep & 15) == 0)
	shr r8,1 ;width
	align 16
hf6nlop4:
	mov rcx,r10 ;inPt
	mov rsi,r11 ;outPt

	mov rbp,r8 ;width
	mov rbx,qword [rsp+80] ;index
	mov rdi,qword [rsp+88] ;weight
	ALIGN 16
hf6nlop5:
	pxor xmm1,xmm1
	mov rax,qword [rbx]
	mov rdx,qword [rbx+8]
	movq xmm0,[rcx+rax]
	movq xmm2,[rcx+rdx]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[rdi]
	paddd xmm1,xmm0
	mov rax,qword [rbx+16]
	mov rdx,qword [rbx+24]
	movq xmm0,[rcx+rax]
	movq xmm2,[rcx+rdx]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[rdi+16]
	paddd xmm1,xmm0
	mov rax,qword [rbx+32]
	mov rdx,qword [rbx+40]
	movq xmm0,[rcx+rax]
	movq xmm2,[rcx+rdx]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[rdi+32]
	paddd xmm1,xmm0
	lea rdi,[rdi+48]
	lea rbx,[rbx+48]

	pxor xmm3,xmm3
	mov rax,qword [rbx]
	mov rdx,qword [rbx+8]
	movq xmm0,[rcx+rax]
	movq xmm2,[rcx+rdx]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[rdi]
	paddd xmm3,xmm0
	mov rax,qword [rbx+16]
	mov rdx,qword [rbx+24]
	movq xmm0,[rcx+rax]
	movq xmm2,[rcx+rdx]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[rdi+16]
	paddd xmm3,xmm0
	mov rax,qword [rbx+32]
	mov rdx,qword [rbx+40]
	movq xmm0,[rcx+rax]
	movq xmm2,[rcx+rdx]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[rdi+32]
	paddd xmm3,xmm0
	lea rbx,[rbx+48]
	lea rdi,[rdi+48]

	psrad xmm1,15
	psrad xmm3,15
	packssdw xmm1,xmm3
	movntdq [rsi],xmm1

	lea rsi,[rsi+16]
	dec rbp
	jnz hf6nlop5

	add r10,qword [rsp+96] ;sstep inPt
	add r11,qword [rsp+104] ;dstep outPt

	dec r9 ;currHeight
	jnz hf6nlop4
	jmp hfexit
	
	align 16
hf6start2:
	shr r8,1 ;width
	align 16
hf6lop4:
	mov rcx,r10 ;inPt
	mov rsi,r11 ;outPt
	mov rbp,r8 ;width
	mov rbx,qword [rsp+80] ;index
	mov rdi,qword [rsp+88] ;weight
	ALIGN 16
hf6lop5:
	pxor xmm1,xmm1
	mov rax,qword [rbx]
	mov rdx,qword [rbx+8]
	movq xmm0,[rcx+rax]
	movq xmm2,[rcx+rdx]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[rdi]
	paddd xmm1,xmm0
	mov rax,qword [rbx+16]
	mov rdx,qword [rbx+24]
	movq xmm0,[rcx+rax]
	movq xmm2,[rcx+rdx]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[rdi+16]
	paddd xmm1,xmm0
	mov rax,qword [rbx+32]
	mov rdx,qword [rbx+40]
	movq xmm0,[rcx+rax]
	movq xmm2,[rcx+rdx]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[rdi+32]
	paddd xmm1,xmm0
	lea rdi,[rdi+48]
	lea rbx,[rbx+48]

	pxor xmm3,xmm3
	mov rax,qword [rbx]
	mov rdx,qword [rbx+8]
	movq xmm0,[rcx+rax]
	movq xmm2,[rcx+rdx]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[rdi]
	paddd xmm3,xmm0
	mov rax,qword [rbx+16]
	mov rdx,qword [rbx+24]
	movq xmm0,[rcx+rax]
	movq xmm2,[rcx+rdx]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[rdi+16]
	paddd xmm3,xmm0
	mov rax,qword [rbx+32]
	mov rdx,qword [rbx+40]
	movq xmm0,[rcx+rax]
	movq xmm2,[rcx+rdx]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[rdi+32]
	paddd xmm3,xmm0
	lea rdi,[rdi+48]
	lea rbx,[rbx+48]

	psrad xmm1,15
	psrad xmm3,15
	packssdw xmm1,xmm3
	movdqu [rsi],xmm1

	lea rsi,[rsi+16]
	dec rbp
	jnz hf6lop5

	add r10,qword [rsp+96] ;sstep inPt
	add r11,qword [rsp+104] ;dstep outPt

	dec r9 ;currHeight
	jnz hf6lop4
	jmp hfexit

	align 16
hfstart2:
	shr qword [rsp+72],1 ;tap
	shr r8,1 ;width
	align 16
hflop4:
	mov rcx,r10 ;inPt
	mov rsi,r11 ;outPt
	mov rbp,r8 ;width
	mov rbx,qword [rsp+80] ;index
	mov rdi,qword [rsp+88] ;weight
	align 16
hflop5:
	mov rdx,qword [rsp+72] ;tap
	pxor xmm1,xmm1
	ALIGN 16
hflop6:
	mov rax,qword [rbx]
	movq xmm0,[rcx+rax]
	mov rax,qword [rbx+8]
	movq xmm2,[rcx+rax]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[rdi]
	paddd xmm1,xmm0
	lea rdi,[rdi+16]
	lea rbx,[rbx+16]
	dec rdx
	jnz hflop6

	mov rdx,qword [rsp+72] ;tap
	pxor xmm3,xmm3
	ALIGN 16
hflop7:
	mov rax,qword [rbx]
	movq xmm0,[rcx+rax]
	mov rax,qword [rbx+8]
	movq xmm2,[rcx+rax]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[rdi]
	paddd xmm3,xmm0
	lea rdi,[rdi+16]
	lea rbx,[rbx+16]
	dec rdx
	jnz hflop7

	psrad xmm1,15
	psrad xmm3,15
	packssdw xmm1,xmm3
	movdqu [rsi],xmm1

	lea rsi,[rsi+16]
	dec rbp
	jnz hflop5

	add r10,qword [rsp+96] ;sstep inPt
	add r11,qword [rsp+104] ;dstep outPt

	dec r9 ;currHeight
	jnz hflop4

	align 16
hfexit:
	pop rdi
	pop rsi
	pop rbx
	pop rbp
	ret	

;void LanczosResizerLR_C16_vertical_filter(UInt8 *inPt, UInt8 *outPt, IntOS width, IntOS height, Int32 tap, Int32 *index, Int64 *weight, IntOS sstep, IntOS dstep, UInt8 *rgbTable)
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
;120 rgbTable

	align 16
LanczosResizerLR_C16_vertical_filter:
	push rbp
	push rbx
	push rsi
	push rdi
	push rax
	mov r10,rcx
	mov r11,rdx
	test r8,1 ;width
	jnz vfstart
	mov rax,qword [rsp+80] ;tap
	cmp rax,6
	jz vf6start
	jmp vfstart2

	align 16
vfstart:					;if (width & 1)
	shr qword [rsp+80],1 ;tap
	pxor xmm3,xmm3
	align 16
vflop:
	mov rsi,r10 ;inPt
	mov rcx,r11 ;outPt

	mov rbp,qword [rsp+120] ;rgbTable
	mov qword [rsp+0],r8 ;currWidth = width
	align 16
vflop2:
	mov rbx,qword [rsp+88] ;index
	mov rdi,qword [rsp+96] ;weight

	mov rdx,qword [rsp+80] ;tap
	pxor xmm1,xmm1
	ALIGN 16
vflop3:
	mov rax,qword [rbx]
	movq xmm0,[rsi+rax]
	mov rax,qword [rbx+8]
	movq xmm2,[rsi+rax]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[rdi]
	paddd xmm1,xmm0
	lea rbx,[rbx+16]
	lea rdi,[rdi+16]
	dec rdx
	jnz vflop3

	psrad xmm1,15
	packssdw xmm1,xmm3
	pextrw rdx,xmm1,0
	mov ax,word [rbp+rdx*2]
	pextrw rdx,xmm1,1
	or ax,word [rbp+rdx*2+131072]
	pextrw rdx,xmm1,2
	or ax,word [rbp+rdx*2+262144]

	mov word [rcx],ax
	add rcx,2
	add rsi,8
	dec qword [rsp+0] ;currWidth
	jnz vflop2

	mov rax,qword [rsp+80] ;tap
	lea rdx,[rax*8]
	shl rax,4
	add qword [rsp+88],rdx ;index
	add qword [rsp+96],rax ;weight

	add r11,qword [rsp+112] ;dstep outPt
	
	dec r9 ;currHeight
	jnz vflop
	jmp vfexit

	align 16
vf6start:									;else if (tap == 6)
	shr r8,1 ;width
	mov rbx,qword [rsp+88] ;index
	mov rdi,qword [rsp+96] ;weight
	mov rbp,qword [rsp+120] ;rgbTable
	align 16
vf6lop4:
	mov rsi,r10 ;inPt
	mov rcx,r11 ;outPt
	movdqa xmm5,[rdi]
	movdqa xmm6,[rdi+16]
	movdqa xmm7,[rdi+32]

	mov qword [rsp+0],r8 ;currWidth = width
	ALIGN 16
vf6lop5:
	pxor xmm2,xmm2
	pxor xmm3,xmm3
	mov rax,qword [rbx]
	mov rdx,qword [rbx+8]
	movdqu xmm0,[rsi+rax]
	movdqu xmm1,[rsi+rdx]
	movdqu xmm4,xmm0
	punpcklwd xmm0, xmm1
	punpckhwd xmm4, xmm1
	pmaddwd xmm0,xmm5
	pmaddwd xmm4,xmm5
	paddd xmm2,xmm0
	paddd xmm3,xmm4
	mov rax,qword [rbx+16]
	mov rdx,qword [rbx+24]
	movdqu xmm0,[rsi+rax]
	movdqu xmm1,[rsi+rdx]
	movdqu xmm4,xmm0
	punpcklwd xmm0, xmm1
	punpckhwd xmm4, xmm1
	pmaddwd xmm0,xmm6
	pmaddwd xmm4,xmm6
	paddd xmm2,xmm0
	paddd xmm3,xmm4
	mov rax,qword [rbx+32]
	mov rdx,qword [rbx+40]
	movdqu xmm0,[rsi+rax]
	movdqu xmm1,[rsi+rdx]
	movdqu xmm4,xmm0
	punpcklwd xmm0, xmm1
	punpckhwd xmm4, xmm1
	pmaddwd xmm0,xmm7
	pmaddwd xmm4,xmm7
	paddd xmm2,xmm0
	paddd xmm3,xmm4

	pxor xmm4,xmm4

	psrad xmm3,15
	packssdw xmm3,xmm4
	pextrw rdx,xmm3,0
	mov ax,word [rbp+rdx*2]
	pextrw rdx,xmm3,1
	or ax,word [rbp+rdx*2+131072]
	pextrw rdx,xmm3,2
	or ax,word [rbp+rdx*2+262144]

	psrad xmm2,15
	shl eax,16
	packssdw xmm2,xmm4
	pextrw rdx,xmm3,0
	mov ax,word [rbp+rdx*2]
	pextrw rdx,xmm3,1
	or ax,word [rbp+rdx*2+131072]
	pextrw rdx,xmm3,2
	or ax,word  [rbp+rdx*2+262144]
	movnti dword [rcx],eax
	
	add rcx,4
	add rsi,16
	dec qword [rsp+0] ;currWidth
	jnz vf6lop5

	add rbx,48
	add rdi,48

	add r11,qword [rsp+112] ;dstep outPt
	
	dec r9 ;currHeight
	jnz vf6lop4
	jmp vfexit

	align 16
vfstart2:
	shr qword [rsp+80],1 ;tap
	shr r8,1 ;width
	mov rbp,qword [rsp+120] ;rgbTable

	pxor xmm3,xmm3
	align 16
vflop4:
	mov rsi,r10 ;inPt
	mov rcx,r11 ;outPt
	mov qword [rsp+0],r8 ;currWidth = width

	align 16
vflop5:
	mov rbx,qword [rsp+88] ;index
	mov rdi,qword [rsp+96] ;weight
	mov rdx,qword [rsp+80] ;tap
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
	movdqa xmm5,[rdi]
	pmaddwd xmm0,xmm5
	pmaddwd xmm4,xmm5
	paddd xmm2,xmm0
	paddd xmm6,xmm4
	add rdi,16
	add rbx,16

	dec rdx
	jnz vflop6

	psrad xmm6,15
	packssdw xmm6,xmm3
	pextrw rdx,xmm6,0
	mov ax,word [rdi+rdx*2]
	pextrw rdx,xmm6,1
	or ax,word [rdi+rdx*2+131072]
	pextrw rdx,xmm6,2
	or ax,word [rdi+rdx*2+262144]
	shl eax,16

	psrad xmm2,15
	packssdw xmm2,xmm3
	pextrw rdx,xmm2,0
	mov ax,word [rdi+rdx*2]
	pextrw rdx,xmm2,1
	or ax,word [rdi+rdx*2+131072]
	pextrw rdx,xmm2,2
	or ax,word [rdi+rdx*2+262144]

	movnti dword [rcx],eax
	add rcx,4
	add rsi,16
	dec qword [rsp+0] ;currWidth
	jnz vflop5

	mov rax,qword [rsp+80] ;tap
	shl rax,4
	add qword [rsp+88],rax ;index
	add qword [rsp+96],rax ;weight

	add r11,qword [rsp+112] ;dstep outPt

	dec r9 ;currHeight
	jnz vflop4
		
	align 16
vfexit:
	pop rax
	pop rdi
	pop rsi
	pop rbx
	pop rbp
	ret

;void LanczosResizerLR_C16_collapse(UInt8 *inPt, UInt8 *outPt, Int32 width, Int32 height, Int32 sstep, Int32 dstep, UInt8 *rgbTable)
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
LanczosResizerLR_C16_collapse:
	push rbp
	push rbx
	push rsi
	push rdi
	mov r10,rcx
	mov r11,rdx
	
	mov rax,r8 ;width
	lea rdx,[rax*8]
	shl rax,1
	sub qword [rsp+72],rdx ;sstep
	sub qword [rsp+80],rax ;dstep
	mov rbx,qword [rsp+88] ;rgbTable
	mov rcx,r10 ;inPt
	mov rsi,r11 ;outPt
	mov rbp,r9 ;height
	align 16
collop:

	mov rdi,r8 ;width

	ALIGN 16
collop2:
	movzx rdx,word [rcx+4]
	mov ax,word [rbx+rdx*2+262144]
	movzx edx,word [rcx+2]
	or ax,word [rbx+rdx*2+131072]
	movzx edx,word [rcx]
	or ax,word [rbx+rdx*2]

	mov word [rsi],ax
	lea rsi,[rsi+2]
	lea rcx,[rcx+8]
	dec rdi
	jnz collop2

	add rcx,qword [rsp+72] ;sstep
	add rsi,qword [rsp+80] ;dstep

	dec rbp
	jnz collop
	pop rdi
	pop rsi
	pop rbx
	pop rbp
	ret