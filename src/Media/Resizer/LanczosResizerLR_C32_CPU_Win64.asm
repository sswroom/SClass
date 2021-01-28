section .text

global LanczosResizerLR_C32_CPU_horizontal_filter
global LanczosResizerLR_C32_CPU_vertical_filter
global LanczosResizerLR_C32_CPU_vertical_filter_na
global LanczosResizerLR_C32_CPU_hv_filter
global LanczosResizerLR_C32_CPU_collapse
global LanczosResizerLR_C32_CPU_collapse_na

extern CPUBrand
extern UseAVX

;void LanczosResizerLR_C32_CPU_horizontal_filter(UInt8 *inPt, UInt8 *outPt, OSInt width, OSInt height, OSInt tap, OSInt *index, Int64 *weight, OSInt sstep, OSInt dstep, UInt8 *rgbTable)
;xmm7 tmpV
;0 rdi
;8 rsi
;16 rbx
;24 rbp
;32 retAddr
;rcx inPt r10
;rdx outPt r11
;r8 width
;r9 height / currHeight
;72 tap
;80 index
;88 weight
;96 sstep
;104 dstep / dAdd
;112 rgbTable

	align 16
LanczosResizerLR_C32_CPU_horizontal_filter:
	push rbp
	push rbx
	push rsi
	push rdi
	mov eax,0x4000
	mov r10,rcx
	mov r11,rdx
	movd xmm7,eax
	punpckldq xmm7,xmm7				; Int32 tmpV[4] = {16384, 16384, 16384, 16384};
	punpckldq xmm7,xmm7
	mov rdx,r8 ;width
	lea rcx,[rdx*8]				; OSInt dAdd = dstep - width * 8;
	sub qword [rsp+104],rcx ;dAdd
	test rdx,1
	jnz hfstart
	mov rcx,qword [rsp+72] ;tap
	cmp rcx,6
	jz hf6nstart
	cmp rcx,8
	jz hf8nstart
	cmp rcx,16
	jz hf16nstart
	jmp hfstart2
	
	align 16
hfstart:						; if (width & 1)
	shr qword [rsp+72],1 ;tap
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
	movdqa xmm1,xmm7
	ALIGN 16
hflop3:
	mov rax,qword [rbx]
	movq xmm0,[rcx+rax]
	mov rax,qword [rbx+8]
	movq xmm2,[rcx+rax]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0, [rdi]
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

	add rcx,qword [rsp+96] ;sstep
	add rsi,qword [rsp+104] ;dAdd

	dec r9 ;currHeight
	jnz hflop
	jmp hfexit

	align 16
hf6nstart:						; else if (tap == 6)
	test r11,15 ;outPt
	jnz hf6start
	test qword [rsp+104],15 ;dstep
	jnz hf6start
								; if ((((OSInt)outPt) & 15) == 0 && (dstep & 15) == 0)
	shr r8,1 ;width
	mov rcx,r10 ;inPt
	mov rsi,r11 ;outPt

	mov r10,qword [rsp+80] ;index
	mov r11,qword [rsp+88] ;weight
	ALIGN 16
hf6nlop4:

	mov rbp,r8 ;width

	mov rbx,r10 ;index
	mov rdi,r11 ;weight
	ALIGN 16
hf6nlop5:
	mov rax,qword [rbx]
	movdqa xmm1,xmm7
	movdqu xmm0,[rcx+rax]
	mov rax,qword [rbx+8]
	movdqa xmm4,xmm0
	movdqu xmm2,[rcx+rax]
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
	pmaddwd xmm0,[rdi+32]
	paddd xmm1,xmm0

	mov rax,qword [rbx+24]
	movdqa xmm3,xmm7
	movdqu xmm0,[rcx+rax]
	mov rax,qword [rbx+32]
	movdqa xmm4,xmm0
	movdqu xmm2,[rcx+rax]
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
	movntdq [rsi],xmm1

	lea rsi,[rsi+16]
	dec rbp
	jnz hf6nlop5

	add rcx,qword [rsp+96] ;sstep
	add rsi,qword [rsp+104] ;dAdd

	dec r9 ;height
	jnz hf6nlop4
	jmp hfexit

	align 16
hf6start:
	shr r8,1 ;width
	mov rcx,r10 ;inPt
	mov rsi,r11 ;outPt
	ALIGN 16
hf6lop4:

	mov rbp,r8 ;width

	mov rbx,qword [rsp+80] ;index
	mov rdi,qword [rsp+88] ;weight
	ALIGN 16
hf6lop5:

	mov rax,qword [rbx]
	movdqa xmm1,xmm7
	movdqu xmm0,[rcx+rax]
	mov rax,qword [rbx+8]
	movdqa xmm4,xmm0
	movdqu xmm2,[rcx+rax]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[rdi]
	punpckhwd xmm4,xmm2
	pmaddwd xmm4,[rdi+16]
	paddd xmm1,xmm0
	mov rax,qword [rbx+16]
	paddd xmm1,xmm4
	movq xmm0,[rcx+rax]
	movq xmm2,[rcx+rax+8]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[rdi+32]
	paddd xmm1,xmm0

	mov rax,qword [rbx+24]
	movdqa xmm3,xmm7
	movdqu xmm0,[rcx+rax]
	mov rax,qword [rbx+32]
	movdqa xmm4,xmm0
	movdqu xmm2,[rcx+rax]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[rdi+48]
	punpckhwd xmm4,xmm2
	pmaddwd xmm4,[rdi+64]
	paddd xmm3,xmm0
	mov rax,qword [rbx+40]
	paddd xmm3,xmm4
	movq xmm0,[rcx+rax]
	movq xmm2,[rcx+rax+8]
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
	jnz hf6lop5

	add rcx,qword [rsp+96] ;sstep
	add rsi,qword [rsp+104] ;dAdd

	dec r9 ;height
	jnz hf6lop4
	jmp hfexit

	align 16
hf8nstart:						; else if (tap == 8)
	test r11,15 ;outPt
	jnz hf8start
	test qword [rsp+104],15 ;dstep
	jnz hf8start
	
	cmp dword [rel UseAVX],0
	jnz hf8navxstart
								; if ((((OSInt)outPt) & 15) == 0 && (dstep & 15) == 0)
	mov rcx,r10 ;inPt
	mov rbx,qword [rsp+88] ;weight
	mov rsi,r11 ;outPt
	mov r10,qword [rsp+96] ;sstep
	shr r8,1 ;width
	mov r11,qword [rsp+104] ;dAdd

	ALIGN 16
hf8nlop4:
	mov rbp,r8 ;width
	mov rdi,rbx ;weight
	
	ALIGN 16
hf8nlop5:
	mov rax,qword [rdi]
	add rax,rcx
	movdqa xmm1,xmm7
	movdqu xmm0,[rax]
	movdqa xmm4,xmm0
	movdqu xmm2,[rax+16]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[rdi+16]
	punpckhwd xmm4,xmm2
	pmaddwd xmm4,[rdi+32]
	paddd xmm1,xmm0
	movdqu xmm0,[rax+32]
	paddd xmm1,xmm4
	movdqu xmm2,[rax+48]
	movdqa xmm4,xmm0
	punpcklwd xmm0,xmm2
	punpckhwd xmm4,xmm2
	pmaddwd xmm0,[rdi+48]
	paddd xmm1,xmm0
	pmaddwd xmm4,[rdi+64]
	paddd xmm1,xmm4

	mov rax,qword [rdi+8]
	add rax,rcx
	movdqa xmm3,xmm7
	movdqu xmm0,[rax]
	movdqa xmm4,xmm0
	movdqu xmm2,[rax+16]
	punpcklwd xmm0,xmm2
	punpckhwd xmm4,xmm2
	pmaddwd xmm0,[rdi+80]
	paddd xmm3,xmm0
	pmaddwd xmm4,[rdi+96]
	paddd xmm3,xmm4
	movdqu xmm0,[rax+32]
	movdqa xmm4,xmm0
	movdqu xmm2,[rax+48]
	punpcklwd xmm0,xmm2
	punpckhwd xmm4,xmm2
	pmaddwd xmm0,[rdi+112]
	paddd xmm3,xmm0
	pmaddwd xmm4,[rdi+128]
	paddd xmm3,xmm4

	lea rdi,[rdi+144]

	psrad xmm1,15
	psrad xmm3,15
	packssdw xmm1,xmm3
	movdqa [rsi],xmm1

	lea rsi,[rsi+16]
	dec rbp
	jnz hf8nlop5

	add rcx,r10 ;sstep
	add rsi,r11 ;dAdd

	dec r9 ;height
	jnz hf8nlop4
	jmp hfexit

	align 16
hf8navxstart:
	mov rcx,r10 ;inPt
	mov rbx,qword [rsp+88] ;weight
	mov rsi,r11 ;outPt
	mov r10,qword [rsp+96] ;sstep
	shr r8,1 ;width
	mov r11,qword [rsp+104] ;dAdd

	ALIGN 16
hf8navxlop4:
	mov rbp,r8 ;width
	mov rdi,rbx ;weight
	
	ALIGN 16
hf8navxlop5:
	mov rax,qword [rdi]
	add rax,rcx
	movdqa xmm1,xmm7
	movdqu xmm0,[rax]
	movdqu xmm2,[rax+16]
	vpunpckhwd xmm4,xmm0,xmm2
	pmaddwd xmm4,[rdi+32]
	vpunpcklwd xmm0,xmm0,xmm2
	pmaddwd xmm0,[rdi+16]
	paddd xmm1,xmm0
	movdqu xmm0,[rax+32]
	paddd xmm1,xmm4
	movdqu xmm2,[rax+48]
	vpunpckhwd xmm4,xmm0,xmm2
	vpunpcklwd xmm0,xmm0,xmm2
	pmaddwd xmm0,[rdi+48]
	paddd xmm1,xmm0
	pmaddwd xmm4,[rdi+64]
	paddd xmm1,xmm4

	mov rax,qword [rdi+8]
	add rax,rcx
	movdqa xmm3,xmm7
	movdqu xmm0,[rax]
	movdqu xmm2,[rax+16]
	vpunpckhwd xmm4,xmm0,xmm2
	vpunpcklwd xmm0,xmm0,xmm2
	pmaddwd xmm0,[rdi+80]
	paddd xmm3,xmm0
	pmaddwd xmm4,[rdi+96]
	paddd xmm3,xmm4
	movdqu xmm0,[rax+32]
	movdqu xmm2,[rax+48]
	vpunpckhwd xmm4,xmm0,xmm2
	vpunpcklwd xmm0,xmm0,xmm2
	pmaddwd xmm0,[rdi+112]
	paddd xmm3,xmm0
	pmaddwd xmm4,[rdi+128]
	paddd xmm3,xmm4

	lea rdi,[rdi+144]

	psrad xmm1,15
	psrad xmm3,15
	packssdw xmm1,xmm3
	movdqa [rsi],xmm1

	lea rsi,[rsi+16]
	dec rbp
	jnz hf8navxlop5

	add rcx,r10 ;sstep
	add rsi,r11 ;dAdd

	dec r9 ;height
	jnz hf8navxlop4
	vzeroupper
	jmp hfexit

	align 16
hf8start:
	shr r8,1 ;width
	mov rcx,r10 ;inPt
	mov rsi,r11 ;outPt
	mov rbx,qword [rsp+88] ;weight
	ALIGN 16
hf8lop4:

	mov rbp,r8 ;width

	mov rdi,rbx
	ALIGN 16
hf8lop5:
	mov rax,qword [rdi]
	prefetcht0 [rcx+rax+128]
	movdqa xmm1,xmm7
	movdqu xmm0,[rcx+rax]
	movdqa xmm4,xmm0
	movdqu xmm2,[rcx+rax+16]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[rdi+16]
	punpckhwd xmm4,xmm2
	pmaddwd xmm4,[rdi+32]
	paddd xmm1,xmm0
	paddd xmm1,xmm4
	movdqu xmm0,[rcx+rax+32]
	movdqa xmm4,xmm0
	movdqu xmm2,[rcx+rax+48]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[rdi+48]
	punpckhwd xmm4,xmm2
	pmaddwd xmm4,[rdi+64]
	paddd xmm1,xmm0
	paddd xmm1,xmm4

	mov rax,qword [rdi+8]
	movdqa xmm3,xmm7
	movdqu xmm0,[rcx+rax]
	movdqa xmm4,xmm0
	movdqu xmm2,[rcx+rax+16]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[rdi+80]
	punpckhwd xmm4,xmm2
	pmaddwd xmm4,[rdi+96]
	paddd xmm3,xmm0
	paddd xmm3,xmm4
	movdqu xmm0,[rcx+rax+32]
	movdqa xmm4,xmm0
	movdqu xmm2,[rcx+rax+48]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[rdi+112]
	punpckhwd xmm4,xmm2
	pmaddwd xmm4,[rdi+128]
	paddd xmm3,xmm0
	paddd xmm3,xmm4
	lea rdi,[rdi+144]

	psrad xmm1,15
	psrad xmm3,15
	packssdw xmm1,xmm3
	movdqu [rsi],xmm1

	lea rsi,[rsi+16]
	dec rbp
	jnz hf8lop5

	add rcx,qword [rsp+96] ;sstep
	add rsi,qword [rsp+104] ;dAdd

	dec r9 ;height
	jnz hf8lop4
	jmp hfexit

	align 16
hf16nstart:						; else if (tap == 16)
	test r11,15 ;outPt
	jnz hf16start
	test qword [rsp+104],15 ;dstep
	jnz hf16start
								; if ((((OSInt)outPt) & 15) == 0 && (dstep & 15) == 0)
	shr r8,1 ;width
	mov rcx,r10 ;inPt
	mov rsi,r11 ;outPt
	mov rbx,qword [rsp+88] ;weight
	ALIGN 16
hf16nlop4:

	mov rbp,r8 ;width

	mov rdi,rbx
	ALIGN 16
hf16nlop5:
	mov rax,qword [rdi]
	prefetcht0 [rcx+rax+128]
	movdqa xmm1,xmm7
	movdqu xmm0,[rcx+rax]
	movdqa xmm4,xmm0
	movdqu xmm2,[rcx+rax+16]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[rdi+16]
	punpckhwd xmm4,xmm2
	pmaddwd xmm4,[rdi+32]
	paddd xmm1,xmm0
	paddd xmm1,xmm4
	movdqu xmm0,[rcx+rax+32]
	movdqa xmm4,xmm0
	movdqu xmm2,[rcx+rax+48]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[rdi+48]
	punpckhwd xmm4,xmm2
	pmaddwd xmm4,[rdi+64]
	paddd xmm1,xmm0
	paddd xmm1,xmm4
	movdqu xmm0,[rcx+rax+64]
	movdqa xmm4,xmm0
	movdqu xmm2,[rcx+rax+80]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[rdi+80]
	punpckhwd xmm4,xmm2
	pmaddwd xmm4,[rdi+96]
	paddd xmm1,xmm0
	paddd xmm1,xmm4
	movdqu xmm0,[rcx+rax+96]
	movdqa xmm4,xmm0
	movdqu xmm2,[rcx+rax+112]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[rdi+112]
	punpckhwd xmm4,xmm2
	pmaddwd xmm4,[rdi+128]
	paddd xmm1,xmm0
	paddd xmm1,xmm4

	mov rax,qword [rdi+8]
	movdqa xmm3,xmm7
	movdqu xmm0,[rcx+rax]
	movdqa xmm4,xmm0
	movdqu xmm2,[rcx+rax+16]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[rdi+144]
	punpckhwd xmm4,xmm2
	pmaddwd xmm4,[rdi+160]
	paddd xmm3,xmm0
	paddd xmm3,xmm4
	movdqu xmm0,[rcx+rax+32]
	movdqa xmm4,xmm0
	movdqu xmm2,[rcx+rax+48]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[rdi+176]
	punpckhwd xmm4,xmm2
	pmaddwd xmm4,[rdi+192]
	paddd xmm3,xmm0
	paddd xmm3,xmm4
	movdqu xmm0,[rcx+rax+64]
	movdqa xmm4,xmm0
	movdqu xmm2,[rcx+rax+80]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[rdi+208]
	punpckhwd xmm4,xmm2
	pmaddwd xmm4,[rdi+224]
	paddd xmm3,xmm0
	paddd xmm3,xmm4
	movdqu xmm0,[rcx+rax+96]
	movdqa xmm4,xmm0
	movdqu xmm2,[rcx+rax+112]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[rdi+240]
	punpckhwd xmm4,xmm2
	pmaddwd xmm4,[rdi+256]
	paddd xmm3,xmm0
	paddd xmm3,xmm4

	lea rdi,[rdi+272]

	psrad xmm1,15
	psrad xmm3,15
	packssdw xmm1,xmm3
	movntdq [rsi],xmm1

	lea rsi,[rsi+16]
	dec rbp
	jnz hf16nlop5

	add rcx,qword [rsp+96] ;sstep
	add rsi,qword [rsp+104] ;dAdd

	dec r9 ;height
	jnz hf16nlop4
	jmp hfexit

	align 16	
hf16start:
	shr r8,1 ;width
	mov rcx,r10 ;inPt
	mov rsi,r11 ;outPt
	mov rbx,qword [rsp+88] ;weight
	ALIGN 16
hf16lop4:

	mov rbp,r8 ;width

	mov rdi,rbx
	ALIGN 16
hf16lop5:
	mov rax,qword [rdi]
	prefetcht0 [rcx+rax+128]
	movdqa xmm1,xmm7
	movdqu xmm0,[rcx+rax]
	movdqa xmm4,xmm0
	movdqu xmm2,[rcx+rax+16]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[rdi+16]
	punpckhwd xmm4,xmm2
	pmaddwd xmm4,[rdi+32]
	paddd xmm1,xmm0
	paddd xmm1,xmm4
	movdqu xmm0,[rcx+rax+32]
	movdqa xmm4,xmm0
	movdqu xmm2,[rcx+rax+48]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[rdi+48]
	punpckhwd xmm4,xmm2
	pmaddwd xmm4,[rdi+64]
	paddd xmm1,xmm0
	paddd xmm1,xmm4
	movdqu xmm0,[rcx+rax+64]
	movdqa xmm4,xmm0
	movdqu xmm2,[rcx+rax+80]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[rdi+80]
	punpckhwd xmm4,xmm2
	pmaddwd xmm4,[rdi+96]
	paddd xmm1,xmm0
	paddd xmm1,xmm4
	movdqu xmm0,[rcx+rax+96]
	movdqa xmm4,xmm0
	movdqu xmm2,[rcx+rax+112]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[rdi+112]
	punpckhwd xmm4,xmm2
	pmaddwd xmm4,[rdi+128]
	paddd xmm1,xmm0
	paddd xmm1,xmm4

	mov rax,qword [rdi+8]
	movdqa xmm3,xmm7
	movdqu xmm0,[rcx+rax]
	movdqa xmm4,xmm0
	movdqu xmm2,[rcx+rax+16]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[rdi+144]
	punpckhwd xmm4,xmm2
	pmaddwd xmm4,[rdi+160]
	paddd xmm3,xmm0
	paddd xmm3,xmm4
	movdqu xmm0,[rcx+rax+32]
	movdqa xmm4,xmm0
	movdqu xmm2,[rcx+rax+48]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[rdi+176]
	punpckhwd xmm4,xmm2
	pmaddwd xmm4,[rdi+192]
	paddd xmm3,xmm0
	paddd xmm3,xmm4
	movdqu xmm0,[rcx+rax+64]
	movdqa xmm4,xmm0
	movdqu xmm2,[rcx+rax+80]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[rdi+208]
	punpckhwd xmm4,xmm2
	pmaddwd xmm4,[rdi+224]
	paddd xmm3,xmm0
	paddd xmm3,xmm4
	movdqu xmm0,[rcx+rax+96]
	movdqa xmm4,xmm0
	movdqu xmm2,[rcx+rax+112]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[rdi+240]
	punpckhwd xmm4,xmm2
	pmaddwd xmm4,[rdi+256]
	paddd xmm3,xmm0
	paddd xmm3,xmm4

	lea rdi,[rdi+272]

	psrad xmm1,15
	psrad xmm3,15
	packssdw xmm1,xmm3
	movdqu [rsi],xmm1

	lea rsi,[rsi+16]
	dec rbp
	jnz hf16lop5

	add rcx,qword [rsp+96] ;sstep
	add rsi,qword [rsp+104] ;dAdd

	dec r9 ;height
	jnz hf16lop4
	jmp hfexit

	align 16
hfstart2:
	shr qword [rsp+72],1 ;tap
	shr r8,1 ;width
	mov rcx,r10 ;inPt
	mov rsi,r11 ;outPt
	align 16
hflop4:
	mov rbp,r8 ;width

	mov rbx,qword [rsp+80] ;index
	mov rdi,qword [rsp+88] ;weight
	align 16
hflop5:
	mov rdx,qword [rsp+72] ;tap
	movdqa xmm1,xmm7
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
	movdqa xmm3,xmm7
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

	add rcx,qword [rsp+96] ;sstep
	add rsi,qword [rsp+104] ;dAdd

	dec r9 ;currHeight
	jnz hflop4

	align 16
hfexit:
	pop rdi
	pop rsi
	pop rbx
	pop rbp
	ret

;void LanczosResizerLR_C32_CPU_vertical_filter(UInt8 *inPt, UInt8 *outPt, OSInt width, OSInt height, OSInt tap, OSInt *index, Int64 *weight, OSInt sstep, OSInt dstep, UInt8 *rgbTable)
;-16 tmpV
;0 rdi
;8 rsi
;16 rbx
;24 rbp
;32 retAddr
;rcx inPt r10
;rdx outPt r11
;r8 width
;r9 height / currHeight
;72 tap
;80 index
;88 weight
;96 sstep
;104 dstep / dAdd
;112 rgbTable

	align 16
LanczosResizerLR_C32_CPU_vertical_filter:
	mov eax,0x4000
	push rbp
	push rbx
	push rsi
	push rdi
	movd xmm0,eax					;Int32 tmpV[4] = {16384, 16384, 16384, 16384};
	punpckldq xmm0,xmm0
	punpckldq xmm0,xmm0
	movdqu xmm8,xmm0
	mov r10,rcx
	mov r11,rdx
	mov rdx,r8 ;width
	lea rcx,[rdx*4]
	sub qword [rsp+104],rcx ;dAdd
	mov rax,qword [rsp+72] ;tap
	test rdx,1						;if (width & 1)
	jz vfstart2
	cmp rax,6
	jz vf6_1start
	jmp vf_1start
	
	align 16
vf6_1start:							; if (tap == 6)
	shr r8,1 ;width
	mov rdi,qword [rsp+112] ;rgbTable
	mov rbx,qword [rsp+88] ;weight
	mov rcx,r11 ;outPt
	ALIGN 16
vf6_1lop4:
	movdqa xmm5,[rbx+32]
	movdqa xmm6,[rbx+48]
	movdqa xmm7,[rbx+64]

	mov rsi,r10 ;inPt
	mov rbp,r8 ;width
	ALIGN 16
vf6_1lop5:
	mov eax,dword [rbx]
	movdqu xmm2,xmm8 ;tmpV
	mov edx,dword [rbx+4]
	movdqa xmm3,xmm2
	movdqu xmm0,[rsi+rax]
	movdqa xmm4,xmm0
	movdqu xmm1,[rsi+rdx]
	punpcklwd xmm0, xmm1
	punpckhwd xmm4, xmm1
	mov eax,dword [rbx+8]
	pmaddwd xmm0,xmm5
	pmaddwd xmm4,xmm5
	mov edx,dword [rbx+12]
	paddd xmm2,xmm0
	paddd xmm3,xmm4
	movdqu xmm0,[rsi+rax]
	movdqa xmm4,xmm0
	movdqu xmm1,[rsi+rdx]
	punpcklwd xmm0, xmm1
	punpckhwd xmm4, xmm1
	mov eax,dword [rbx+16]
	pmaddwd xmm0,xmm6
	pmaddwd xmm4,xmm6
	mov edx,dword [rbx+20]
	paddd xmm2,xmm0
	paddd xmm3,xmm4
	movdqu xmm0,[rsi+rax]
	movdqa xmm4,xmm0
	movdqu xmm1,[rsi+rdx]
	punpcklwd xmm0, xmm1
	punpckhwd xmm4, xmm1
	pmaddwd xmm0,xmm7
	pmaddwd xmm4,xmm7
	paddd xmm2,xmm0
	paddd xmm3,xmm4

	psrad xmm3,15
	psrad xmm2,15
	packssdw xmm2,xmm3
	pextrw rdx,xmm2,0
	mov eax,dword [rdi+rdx*4]
	pextrw rdx,xmm2,1
	or eax,dword [rdi+rdx*4+262144]
	pextrw rdx,xmm2,2
	or eax,dword [rdi+rdx*4+524288]
	pextrw rdx,xmm2,3
	or eax,dword [rdi+rdx*4+786432]
	movnti dword [rcx],eax

	pextrw rdx,xmm2,4
	mov eax,dword [rdi+rdx*4]
	pextrw rdx,xmm2,5
	or eax,dword [rdi+rdx*4+262144]
	pextrw rdx,xmm2,6
	or eax,dword [rdi+rdx*4+524288]
	pextrw rdx,xmm2,7
	or eax,dword [rdi+rdx*4+786432]
	movnti dword [rcx+4],eax
	lea rsi,[rsi+16]
	lea rcx,[rcx+8]
	dec rbp
	jnz vf6_1lop5

	mov eax,dword [rbx]
	movdqu xmm2,xmm8 ;tmpV
	mov edx,dword [rbx+4]
	movq xmm0,[rsi+rax]
	movq xmm1,[rsi+rdx]
	punpcklwd xmm0, xmm1
	mov eax,dword [rbx+8]
	pmaddwd xmm0,xmm5
	mov edx,dword [rbx+12]
	paddd xmm2,xmm0
	movdqu xmm0,[rsi+rax]
	movdqu xmm1,[rsi+rdx]
	punpcklwd xmm0, xmm1
	mov eax,dword [rbx+16]
	pmaddwd xmm0,xmm6
	mov edx,dword [rbx+20]
	paddd xmm2,xmm0
	movdqu xmm0,[rsi+rax]
	movdqu xmm1,[rsi+rdx]
	punpcklwd xmm0, xmm1
	pmaddwd xmm0,xmm7
	paddd xmm2,xmm0

	psrad xmm2,15
	packssdw xmm2,xmm2
	pextrw rdx,xmm2,0
	mov eax,dword [rdi+rdx*4]
	pextrw rdx,xmm2,1
	or eax,dword [rdi+rdx*4+262144]
	pextrw rdx,xmm2,2
	or eax,dword [rdi+rdx*4+524288]
	pextrw rdx,xmm2,3
	or eax,dword [rdi+rdx*4+786432]
	movnti dword [rcx],eax
	lea rcx,[rcx+4]

	add rbx,80
	add rcx,qword [rsp+104] ;dAdd

	dec r9 ;currHeight
	jnz vf6_1lop4
	jmp vfexit

	align 16
vf_1start:
	shr qword [rsp+72],1 ;tap
	movdqu xmm7,xmm8 ;tmpV
	pxor xmm3,xmm3
	mov rcx,r11 ;outPt
	align 16
vflop:
	mov rsi,r10 ;inPt
	mov rbp,r8 ;width

	align 16
vflop2:
	mov rdi,qword [rsp+88] ;weight
	mov rbx,qword [rsp+80] ;index
	mov rdx,qword [rsp+72] ;tap

	movdqa xmm1,xmm7
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

	mov rdi,qword [rsp+112] ;rgbTable
	psrad xmm1,15
	packssdw xmm1,xmm3
	pextrw rdx,xmm1,0
	mov eax,dword [rdi+rdx*4]
	pextrw rdx,xmm1,1
	or eax,dword [rdi+rdx*4+262144]
	pextrw rdx,xmm1,2
	or eax,dword [rdi+rdx*4+524288]
	pextrw rdx,xmm1,3
	or eax,dword [rdi+rdx*4+786432]

	mov dword [rcx],eax
	lea rcx,[rcx+4]
	lea rsi,[rsi+8]
	dec rbp
	jnz vflop2

	mov rdx,qword [rsp+72] ;tap
	shl rdx,4
	add qword [rsp+80],rdx ;index
	add qword [rsp+88],rdx ;weight
	add rcx,qword [rsp+104] ;dAdd

	dec r9 ;currHeight
	jnz vflop
	jmp vfexit

	align 16
vfstart2:
	cmp rax,6
	jz vf6start
	cmp rax,8
	jz vf8start
	jmp vfstart

	align 16
vf6start:							; else if (tap == 6)
	shr r8,1 ;width
	mov rdi,qword [rsp+112] ;rgbTable
	mov rbx,qword [rsp+88] ;weight
	mov rcx,r11 ;outPt
	ALIGN 16
vf6lop4:

	movdqa xmm5,[rbx+32]
	movdqa xmm6,[rbx+48]
	movdqa xmm7,[rbx+64]
	mov rsi,r10 ;inPt
	mov rbp,r8 ;width
	ALIGN 16
vf6lop5:
	mov eax,dword [rbx]
	movdqu xmm2,xmm8 ;tmpV
	mov edx,dword [rbx+4]
	movdqa xmm3,xmm2
	movdqu xmm0,[rsi+rax]
	movdqa xmm4,xmm0
	movdqu xmm1,[rsi+rdx]
	punpcklwd xmm0, xmm1
	punpckhwd xmm4, xmm1
	mov eax,dword [rbx+8]
	pmaddwd xmm0,xmm5
	pmaddwd xmm4,xmm5
	mov edx,dword [rbx+12]
	paddd xmm2,xmm0
	paddd xmm3,xmm4
	movdqu xmm0,[rsi+rax]
	movdqa xmm4,xmm0
	movdqu xmm1,[rsi+rdx]
	punpcklwd xmm0,xmm1
	punpckhwd xmm4,xmm1
	mov eax,dword [rbx+16]
	pmaddwd xmm0,xmm6
	pmaddwd xmm4,xmm6
	mov edx,dword [rbx+20]
	paddd xmm2,xmm0
	paddd xmm3,xmm4
	movdqu xmm0,[rsi+rax]
	movdqa xmm4,xmm0
	movdqu xmm1,[rsi+rdx]
	punpcklwd xmm0,xmm1
	punpckhwd xmm4,xmm1
	pmaddwd xmm0,xmm7
	pmaddwd xmm4,xmm7
	paddd xmm2,xmm0
	paddd xmm3,xmm4

	psrad xmm3,15
	psrad xmm2,15
	packssdw xmm2,xmm3
	pextrw rdx,xmm2,0
	mov eax,dword [rdi+rdx*4]
	pextrw rdx,xmm2,1
	or eax,dword [rdi+rdx*4+262144]
	pextrw rdx,xmm2,2
	or eax,dword [rdi+rdx*4+524288]
	pextrw rdx,xmm2,3
	or eax,dword [rdi+rdx*4+786432]
	movnti dword [rcx],eax

	pextrw rdx,xmm2,4
	mov eax,dword [rdi+rdx*4]
	pextrw rdx,xmm2,5
	or eax,dword [rdi+rdx*4+262144]
	pextrw rdx,xmm2,6
	or eax,dword [rdi+rdx*4+524288]
	pextrw rdx,xmm2,7
	or eax,dword [rdi+rdx*4+786432]
	movnti dword [rcx+4],eax
	lea rsi,[rsi+16]
	lea rcx,[rcx+8]
	dec rbp
	jnz vf6lop5

	add rbx,80
	add rcx,qword [rsp+104] ;dAdd

	dec r9 ;currHeight
	jnz vf6lop4
	jmp vfexit
	
	align 16
vf8start:							;else if (tap == 8)
	shr r8,1 ;width
	mov rbx,qword [rsp+80] ;index
	mov rcx,r11 ;outPt
	ALIGN 16
vf8lop4:

	mov rdi,qword [rsp+88] ;weight
	movdqa xmm5,[rdi]
	punpcklqdq xmm5,[rdi+16]
	movdqa xmm6,[rdi+32]
	movdqa xmm7,[rdi+48]
	add rdi,64
	mov qword [rsp+88],rdi ;weight

	mov rsi,r10 ;inPt
	mov rdi,qword [rsp+112] ;rgbTable
	mov rbp,r8 ;width
	ALIGN 16
vf8lop5:
	mov rax,qword [rbx]
	mov rdx,qword [rbx+8]
	movdqu xmm2,xmm8 ;tmpV
	movdqa xmm3,xmm2
	movdqu xmm0,[rsi+rax]
	movdqu xmm1,[rsi+rdx]
	movdqa xmm4,xmm0
	punpcklwd xmm0, xmm1
	punpckhwd xmm4, xmm1
	movdqa xmm1,xmm5
	punpckldq xmm1,xmm5
	pmaddwd xmm0,xmm1
	pmaddwd xmm4,xmm1
	mov rax,qword [rbx+16]
	mov rdx,qword [rbx+24]
	paddd xmm2,xmm0
	paddd xmm3,xmm4
	movdqu xmm0,[rsi+rax]
	movdqu xmm1,[rsi+rdx]
	movdqa xmm4,xmm0
	punpcklwd xmm0, xmm1
	punpckhwd xmm4, xmm1
	movdqa xmm1,xmm5
	punpckhdq xmm1,xmm5
	pmaddwd xmm0,xmm1
	pmaddwd xmm4,xmm1
	mov rax,qword [rbx+32]
	mov rdx,qword [rbx+40]
	paddd xmm2,xmm0
	paddd xmm3,xmm4
	movdqu xmm0,[rsi+rax]
	movdqu xmm1,[rsi+rdx]
	movdqa xmm4,xmm0
	punpcklwd xmm0, xmm1
	punpckhwd xmm4, xmm1
	pmaddwd xmm0,xmm6
	pmaddwd xmm4,xmm6
	mov rax,qword [rbx+48]
	mov rdx,qword [rbx+56]
	paddd xmm2,xmm0
	paddd xmm3,xmm4
	movdqu xmm0,[rsi+rax]
	movdqu xmm1,[rsi+rdx]
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
	pextrw rdx,xmm2,0
	mov eax,dword [rdi+rdx*4]
	pextrw rdx,xmm2,1
	or eax,dword [rdi+rdx*4+262144]
	pextrw rdx,xmm2,2
	or eax,dword [rdi+rdx*4+524288]
	pextrw rdx,xmm2,3
	or eax,dword [rdi+rdx*4+786432]
	movnti dword [rcx],eax

	pextrw rdx,xmm2,4
	mov eax,dword [rdi+rdx*4]
	pextrw rdx,xmm2,5
	or eax,dword [rdi+rdx*4+262144]
	pextrw rdx,xmm2,6
	or eax,dword [rdi+rdx*4+524288]
	pextrw rdx,xmm2,7
	or eax,dword [rdi+rdx*4+786432]
	movnti dword [rcx+4],eax
	lea rsi,[rsi+16]
	lea rcx,[rcx+8]
	dec rbp
	jnz vf8lop5

	add rbx,64
	add rcx,qword [rsp+104] ;dAdd

	dec r9 ;currHeight
	jnz vf8lop4
	jmp vfexit
	
	align 16
vfstart:
	shr qword [rsp+72],1 ;tap
	shr r8,1 ;width
	mov rbx,qword [rsp+80] ;index
	mov rcx,r11 ;outPt
	movdqu xmm7,xmm8 ;tmpV
	pxor xmm3,xmm3
	align 16
vflop4:
	mov rsi,r10 ;inPt
	mov rbp,r8 ;width

	align 16
vflop5:
	mov rdi,qword [rsp+88] ;weight

	mov r11,rbx
	mov rdx,qword [rsp+72] ;tap
	movdqa xmm2,xmm7
	movdqa xmm6,xmm7
	ALIGN 16
vflop6:
	mov rax,qword [r11]
	movdqu xmm0,[rsi+rax]
	mov rax,qword [r11+8]
	movdqu xmm4,xmm0
	movdqu xmm1,[rsi+rax]
	punpcklwd xmm0, xmm1
	punpckhwd xmm4, xmm1
	movdqa xmm5,[rdi]
	pmaddwd xmm0,xmm5
	pmaddwd xmm4,xmm5
	paddd xmm2,xmm0
	paddd xmm6,xmm4
	lea rdi,[rdi+16]
	lea r11,[r11+16]

	dec rdx
	jnz vflop6

	mov rdi,qword [rsp+112] ;rgbTable

	psrad xmm6,15
	packssdw xmm6,xmm3
	pextrw rdx,xmm6,0
	mov eax,dword [rdi+rdx*4]
	pextrw rdx,xmm6,1
	or eax,dword [rdi+rdx*4+262144]
	pextrw rdx,xmm6,2
	or eax,dword [rdi+rdx*4+524288]
	pextrw rdx,xmm6,3
	or eax,dword [rdi+rdx*4+786432]
	movd xmm0,eax

	psrad xmm2,15
	packssdw xmm2,xmm3
	pextrw rdx,xmm2,0
	mov eax,dword [rdi+rdx*4]
	pextrw rdx,xmm2,1
	or eax,dword [rdi+rdx*4+262144]
	pextrw rdx,xmm2,2
	or eax,dword [rdi+rdx*4+524288]
	pextrw rdx,xmm2,3
	or eax,dword [rdi+rdx*4+786432]

	mov dword [rcx],eax
	movd dword [rcx+4],xmm0
	lea rcx,[rcx+8]
	lea rsi,[rsi+16]
	dec rbp
	jnz vflop5

	mov rax,qword [rsp+72] ;tap
	shl rax,4
	add rbx,rax ;index
	add qword [rsp+88],rax ;weight
	add rcx,qword [rsp+104] ;dAdd

	dec r9 ;currHeight
	jnz vflop4

	align 16
vfexit:
	pop rdi
	pop rsi
	pop rbx
	pop rbp
	ret

;void LanczosResizerLR_C32_CPU_vertical_filter_na(UInt8 *inPt, UInt8 *outPt, OSInt width, OSInt height, OSInt tap, OSInt *index, Int64 *weight, OSInt sstep, OSInt dstep, UInt8 *rgbTable)
;xmm8 tmpV
;0 rdi
;8 rsi
;16 rbx
;24 rbp
;32 retAddr
;rcx inPt r10
;rdx outPt r11
;r8 width
;r9 height / currHeight
;72 tap
;80 index
;88 weight
;96 sstep
;104 dstep / dAdd
;112 rgbTable

	align 16
LanczosResizerLR_C32_CPU_vertical_filter_na:
	mov eax,0x4000
	push rbp
	push rbx
	push rsi
	push rdi
	movd xmm0,eax
	punpckldq xmm0,xmm0
	punpckldq xmm0,xmm0
	movdqa xmm8,xmm0						;Int32 tmpV[4] = {16384, 16384, 16384, 16384};
	mov r10,rcx
	mov r11,rdx
	mov rdx,r8 ;width
	mov rax,qword [rsp+72] ;tap
	lea rcx,[rdx*4]
	sub qword [rsp+104],rcx ;dAdd			;OSInt dAdd = dstep - width * 4;
	test rdx,1						;if (width & 1)
	jz vfnastart2
	cmp rax,6
	jz vf6na_1start
	jmp vfna_1start
	
	align 16
vf6na_1start:						;if (tap == 6)
	shr r8,1 ;width
	mov rdi,qword [rsp+112] ;rgbTable
	mov rbx,qword [rsp+88] ;weight
	mov rcx,r11 ;outPt
	ALIGN 16
vf6na_1lop4:

	movdqa xmm5,[rbx+32]
	movdqa xmm6,[rbx+48]
	movdqa xmm7,[rbx+64]

	mov rsi,r10 ;inPt
	mov rbp,r8 ;width
	ALIGN 16
vf6na_1lop5:
	mov eax,dword [rbx]
	movdqu xmm2,xmm8 ;tmpV
	mov edx,dword [rbx+4]
	movdqa xmm3,xmm2
	movdqu xmm0,[rsi+rax]
	movdqa xmm4,xmm0
	movdqu xmm1,[rsi+rdx]
	punpcklwd xmm0, xmm1
	punpckhwd xmm4, xmm1
	mov eax,dword [rbx+8]
	pmaddwd xmm0,xmm5
	pmaddwd xmm4,xmm5
	mov edx,dword [rbx+12]
	paddd xmm2,xmm0
	paddd xmm3,xmm4
	movdqu xmm0,[rsi+rax]
	movdqa xmm4,xmm0
	movdqu xmm1,[rsi+rdx]
	punpcklwd xmm0, xmm1
	punpckhwd xmm4, xmm1
	mov eax,dword [rbx+16]
	pmaddwd xmm0,xmm6
	pmaddwd xmm4,xmm6
	mov edx,dword [rbx+20]
	paddd xmm2,xmm0
	paddd xmm3,xmm4
	movdqu xmm0,[rsi+rax]
	movdqa xmm4,xmm0
	movdqu xmm1,[rsi+rdx]
	punpcklwd xmm0, xmm1
	punpckhwd xmm4, xmm1
	pmaddwd xmm0,xmm7
	pmaddwd xmm4,xmm7
	paddd xmm2,xmm0
	paddd xmm3,xmm4

	psrad xmm3,15
	psrad xmm2,15
	packssdw xmm2,xmm3
	pextrw rdx,xmm2,0
	mov eax,dword [rdi+rdx*4]
	pextrw rdx,xmm2,1
	or eax,dword [rdi+rdx*4+262144]
	pextrw rdx,xmm2,2
	or eax,dword [rdi+rdx*4+524288]
	movnti dword [rcx],eax

	pextrw rdx,xmm2,4
	mov eax,dword [rdi+rdx*4]
	pextrw rdx,xmm2,5
	or eax,dword [rdi+rdx*4+262144]
	pextrw rdx,xmm2,6
	or eax,dword [rdi+rdx*4+524288]
	movnti dword [rcx+4],eax
	lea rsi,[rsi+16]
	lea rcx,[rcx+8]
	dec rbp
	jnz vf6na_1lop5

	mov eax,dword [rbx]
	movdqu xmm2,xmm8 ;tmpV
	mov edx,dword [rbx+4]
	movq xmm0,[rsi+rax]
	movq xmm1,[rsi+rdx]
	punpcklwd xmm0, xmm1
	mov eax,dword [rbx+8]
	pmaddwd xmm0,xmm5
	mov edx,dword [rbx+12]
	paddd xmm2,xmm0
	movdqu xmm0,[rsi+rax]
	movdqu xmm1,[rsi+rdx]
	punpcklwd xmm0, xmm1
	mov eax,dword [rbx+16]
	pmaddwd xmm0,xmm6
	mov edx,dword [rbx+20]
	paddd xmm2,xmm0
	movdqu xmm0,[rsi+rax]
	movdqu xmm1,[rsi+rdx]
	punpcklwd xmm0, xmm1
	pmaddwd xmm0,xmm7
	paddd xmm2,xmm0

	psrad xmm2,15
	packssdw xmm2,xmm2
	pextrw rdx,xmm2,0
	mov eax,dword [rdi+rdx*4]
	pextrw rdx,xmm2,1
	or eax,dword [rdi+rdx*4+262144]
	pextrw rdx,xmm2,2
	or eax,dword [rdi+rdx*4+524288]
	movnti dword [rcx],eax
	lea rcx,[rcx+4]

	add rbx,80
	add rcx,qword [rsp+104] ;dAdd

	dec r9 ;currHeight
	jnz vf6na_1lop4
	jmp vfnaexit

	align 16
vfna_1start:
	shr qword [rsp+72],1 ;tap
	pxor xmm3,xmm3
	mov rcx,r11 ;outPt
	align 16
vfnalop:
	mov rsi,r10 ;inPt
	mov rbp,r8 ;width

	align 16
vfnalop2:
	mov rbx,qword [rsp+80] ;index
	mov rdi,qword [rsp+88] ;weight

	mov rdx,qword [rsp+72] ;tap
	pxor xmm1,xmm1
	ALIGN 16
vfnalop3:
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
	jnz vfnalop3

	psrad xmm1,15
	packssdw xmm1,xmm3
	mov rdi,qword [rsp+112] ;rgbTable

	pextrw rdx,xmm1,0
	mov eax,dword [rdi+rdx*4]
	pextrw rdx,xmm1,1
	or eax,dword [rdi+rdx*4+262144]
	pextrw rdx,xmm1,2
	or eax,dword [rdi+rdx*4+524288]

	mov dword [rcx],eax
	lea rcx,[rcx+4]
	lea rsi,[rsi+8]
	dec rbp
	jnz vfnalop2

	mov rax,qword [rsp+72] ;tap
	shl rax,4
	add qword [rsp+80],rax ;index
	add qword [rsp+88],rax ;weight

	add rcx,qword [rsp+104] ;dAdd

	dec r9 ;currHeight
	jnz vfnalop
	jmp vfnaexit

	align 16
vfnastart2:
	cmp rax,6
	jz vf6nastart
	cmp rax,8
	jz vf8nastart
	cmp rax,12
	jz vf12nastart
	jmp vfnastart
	
	align 16
vf6nastart:						; else if (tap == 6)
	test r8,3
	jnz vf6nastart2
	cmp dword [rel UseAVX],0
	jnz vf6nastartavx
	align 16
vf6nastart2:
	mov qword [rsp-8],r12
	mov qword [rsp-16],r13
	mov qword [rsp-24],r14
	mov qword [rsp-32],r15
	xor r12,r12
	xor r13,r13
	xor r14,r14
	xor r15,r15
	mov qword [rsp+64],r9 ;height
	shr r8,1 ;width
	mov rcx,r11 ;outPt
	mov rbx,qword [rsp+88] ;weight
	mov rdi,qword [rsp+112] ;rgbTable
	xor r11,r11
	xor r9,r9
	ALIGN 16
vf6nalop4:
	movdqa xmm5,[rbx+32]
	movdqa xmm6,[rbx+48]
	movdqa xmm7,[rbx+64]
	mov r12d,dword [rbx]
	mov r13d,dword [rbx+4]
	mov r14d,dword [rbx+8]
	mov r15d,dword [rbx+12]
	mov r11d,dword [rbx+16]
	mov r9d,dword [rbx+20]

	mov rsi,r10 ;inPt
	mov rbp,r8 ;width
	ALIGN 16
vf6nalop5:
	movdqa xmm2,xmm8 ;tmpV
	pxor xmm3,xmm3
	movdqu xmm0,[rsi+r12]
	movdqa xmm4,xmm0
	movdqu xmm1,[rsi+r13]
	punpcklwd xmm0, xmm1
	punpckhwd xmm4, xmm1
	pmaddwd xmm0,xmm5
	pmaddwd xmm4,xmm5
	paddd xmm2,xmm0
	movdqu xmm0,[rsi+r14]
	paddd xmm3,xmm4
	movdqu xmm1,[rsi+r15]
	movdqa xmm4,xmm0
	punpcklwd xmm0, xmm1
	punpckhwd xmm4, xmm1
	pmaddwd xmm0,xmm6
	pmaddwd xmm4,xmm6
	paddd xmm2,xmm0
	movdqu xmm0,[rsi+r11]
	paddd xmm3,xmm4
	movdqu xmm1,[rsi+r9]
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
	pextrw rdx,xmm2,0
	mov eax,dword [rdi+rdx*4]
	pextrw rdx,xmm2,1
	or eax,dword [rdi+rdx*4+262144]
	pextrw rdx,xmm2,2
	or eax,dword [rdi+rdx*4+524288]
	mov dword [rcx],eax

	pextrw rdx,xmm2,4
	mov eax,dword [rdi+rdx*4]
	pextrw rdx,xmm2,5
	or eax,dword [rdi+rdx*4+262144]
	pextrw rdx,xmm2,6
	or eax,dword [rdi+rdx*4+524288]
	mov dword [rcx+4],eax

	lea rcx,[rcx+8]
	lea rsi,[rsi+16]
	dec rbp
	jnz vf6nalop5

	lea rbx,[rbx+80]
	add rcx,qword [rsp+104] ;dAdd

	dec qword [rsp+64] ;r9 ;currHeight
	jnz vf6nalop4
	mov r12,qword [rsp-8]
	mov r13,qword [rsp-16]
	mov r14,qword [rsp-24]
	mov r15,qword [rsp-32]
	jmp vfnaexit

	align 16
vf6nastartavx:
	mov qword [rsp-8],r12
	mov qword [rsp-16],r13
	mov qword [rsp-24],r14
	mov qword [rsp-32],r15
	xor r12,r12
	xor r13,r13
	xor r14,r14
	xor r15,r15
	mov qword [rsp+64],r9 ;height
	shr r8,2 ;width
	mov rcx,r11 ;outPt
	mov rbx,qword [rsp+88] ;weight
	mov rdi,qword [rsp+112] ;rgbTable
	xor r11,r11
	xor r9,r9
	ALIGN 16
vf6naavxlop4:
	vmovdqa xmm5,[rbx+32]
	vmovdqa xmm6,[rbx+48]
	vmovdqa xmm7,[rbx+64]
	mov r12d,dword [rbx]
	mov r13d,dword [rbx+4]
	mov r14d,dword [rbx+8]
	mov r15d,dword [rbx+12]
	mov r11d,dword [rbx+16]
	mov r9d,dword [rbx+20]
	vinsertf128 ymm5,ymm5,xmm5,1
	vinsertf128 ymm6,ymm6,xmm6,1
	vinsertf128 ymm7,ymm7,xmm7,1
	vinsertf128 ymm8,ymm8,xmm8,1 ;tmpV

	mov rsi,r10 ;inPt
	mov rbp,r8 ;width
	ALIGN 16
vf6naavxlop5:
	vmovdqa ymm2,ymm8 ;tmpV
	vpxor ymm3,ymm3,ymm3
	vmovdqu ymm0,[rsi+r12]
	vmovdqa ymm4,ymm0
	vmovdqu ymm1,[rsi+r13]
	vpunpcklwd ymm0,ymm0,ymm1
	vpunpckhwd ymm4,ymm4,ymm1
	vpmaddwd ymm0,ymm0,ymm5
	vpmaddwd ymm4,ymm4,ymm5
	vpaddd ymm2,ymm2,ymm0
	vmovdqu ymm0,[rsi+r14]
	vpaddd ymm3,ymm3,ymm4
	vmovdqu ymm1,[rsi+r15]
	vmovdqa ymm4,ymm0
	vpunpcklwd ymm0,ymm0,ymm1
	vpunpckhwd ymm4,ymm4,ymm1
	vpmaddwd ymm0,ymm0,ymm6
	vpmaddwd ymm4,ymm4,ymm6
	vpaddd ymm2,ymm2,ymm0
	vmovdqu ymm0,[rsi+r11]
	vpaddd ymm3,ymm3,ymm4
	vmovdqu ymm1,[rsi+r9]
	vmovdqa ymm4,ymm0
	vpunpcklwd ymm0,ymm0,ymm1
	vpunpckhwd ymm4,ymm4,ymm1
	vpmaddwd ymm0,ymm0,ymm7
	vpmaddwd ymm4,ymm4,ymm7
	vpaddd ymm2,ymm2,ymm0
	vpaddd ymm3,ymm3,ymm4

	vpsrad ymm3,ymm3,15
	vpsrad ymm2,ymm2,15
	vpackssdw ymm2,ymm2,ymm3
	vpextrw rdx,xmm2,0
	vextractf128 xmm0,ymm2,1
	mov eax,dword [rdi+rdx*4]
	vpextrw rdx,xmm2,1
	or eax,dword [rdi+rdx*4+262144]
	vpextrw rdx,xmm2,2
	or eax,dword [rdi+rdx*4+524288]
	mov dword [rcx],eax

	vpextrw rdx,xmm2,4
	mov eax,dword [rdi+rdx*4]
	vpextrw rdx,xmm2,5
	or eax,dword [rdi+rdx*4+262144]
	vpextrw rdx,xmm2,6
	or eax,dword [rdi+rdx*4+524288]
	mov dword [rcx+4],eax

	vpextrw rdx,xmm0,0
	mov eax,dword [rdi+rdx*4]
	vpextrw rdx,xmm0,1
	or eax,dword [rdi+rdx*4+262144]
	vpextrw rdx,xmm0,2
	or eax,dword [rdi+rdx*4+524288]
	mov dword [rcx+8],eax

	vpextrw rdx,xmm0,4
	mov eax,dword [rdi+rdx*4]
	vpextrw rdx,xmm0,5
	or eax,dword [rdi+rdx*4+262144]
	vpextrw rdx,xmm0,6
	or eax,dword [rdi+rdx*4+524288]
	mov dword [rcx+12],eax

	lea rcx,[rcx+16]
	lea rsi,[rsi+32]
	dec rbp
	jnz vf6naavxlop5

	lea rbx,[rbx+80]
	add rcx,qword [rsp+104] ;dAdd

	dec qword [rsp+64] ;r9 ;currHeight
	jnz vf6naavxlop4
	vzeroupper
	mov r12,qword [rsp-8]
	mov r13,qword [rsp-16]
	mov r14,qword [rsp-24]
	mov r15,qword [rsp-32]
	jmp vfnaexit

	align 16
vf8nastart:						;else if (tap == 8)
	mov qword [rsp-8],r12
	mov qword [rsp-16],r13
	mov qword [rsp-24],r14
	mov qword [rsp-32],r15
	
	shr r8,1 ;width
	mov rcx,r11 ;outPt
	mov rbx,qword [rsp+80] ;index
	mov rdi,qword [rsp+112] ;rgbTable
	mov qword [rsp+40],r10 ;inPt
	mov qword [rsp+56],r8 ;width
	mov qword [rsp+64],r9 ;height
	ALIGN 16
vf8nalop4:

	mov rbp,qword [rsp+88] ;weight
	movdqa xmm5,[rbp]
	movdqa xmm9,[rbp+16]
	movdqa xmm6,[rbp+32]
	movdqa xmm7,[rbp+48]
	add rbp,64
	mov r8,qword [rbx]
	mov r9,qword [rbx+8]
	mov r10,qword [rbx+16]
	mov r11,qword [rbx+24]
	mov r12,qword [rbx+32]
	mov r13,qword [rbx+40]
	mov r14,qword [rbx+48]
	mov r15,qword [rbx+56]
	mov qword [rsp+88],rbp ;weight

	mov rsi,qword [rsp+40] ;r10 ;inPt
	mov rbp,qword [rsp+56] ;r8 ;width
	ALIGN 16
vf8nalop5:
	movdqa xmm2,xmm8 ;tmpV
	pxor xmm3,xmm3
	movdqu xmm0,[rsi+r8]
	movdqa xmm4,xmm0
	movdqu xmm1,[rsi+r9]
	punpcklwd xmm0, xmm1
	punpckhwd xmm4, xmm1
	pmaddwd xmm0,xmm5
	pmaddwd xmm4,xmm5
	paddd xmm2,xmm0
	movdqu xmm0,[rsi+r10]
	paddd xmm3,xmm4
	movdqu xmm1,[rsi+r11]
	movdqa xmm4,xmm0
	punpcklwd xmm0, xmm1
	punpckhwd xmm4, xmm1
	pmaddwd xmm0,xmm9
	pmaddwd xmm4,xmm9
	paddd xmm2,xmm0
	movdqu xmm0,[rsi+r12]
	paddd xmm3,xmm4
	movdqu xmm1,[rsi+r13]
	movdqa xmm4,xmm0
	punpcklwd xmm0, xmm1
	punpckhwd xmm4, xmm1
	pmaddwd xmm0,xmm6
	pmaddwd xmm4,xmm6
	paddd xmm2,xmm0
	movdqu xmm0,[rsi+r14]
	paddd xmm3,xmm4
	movdqu xmm1,[rsi+r15]
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
	pextrw rdx,xmm2,0
	mov eax,dword [rdi+rdx*4]
	pextrw rdx,xmm2,1
	or eax,dword [rdi+rdx*4+262144]
	pextrw rdx,xmm2,2
	or eax,dword [rdi+rdx*4+524288]
	movnti dword [rcx],eax

	pextrw rdx,xmm2,4
	mov eax,dword [rdi+rdx*4]
	pextrw rdx,xmm2,5
	or eax,dword [rdi+rdx*4+262144]
	pextrw rdx,xmm2,6
	or eax,dword [rdi+rdx*4+524288]
	movnti dword [rcx+4],eax

	lea rcx,[rcx+8]
	lea rsi,[rsi+16]
	dec rbp
	jnz vf8nalop5

	lea rbx,[rbx+64]
	add rcx,qword [rsp+104] ;dAdd

	dec qword [rsp+64] ;r9 ;currHeight
	jnz vf8nalop4
	mov r12,qword [rsp-8]
	mov r13,qword [rsp-16]
	mov r14,qword [rsp-24]
	mov r15,qword [rsp-32]
	jmp vfnaexit
	
	align 16
vf12nastart:						;else if (tap == 12)
	shr r8,1 ;width
	mov rcx,r11 ;outPt
	mov rbx,qword [rsp+80] ;index
	mov rdi,qword [rsp+112] ;rgbTable
	ALIGN 16
vf12nalop4:

	mov rbp,qword [rsp+88] ;weight
	movdqa xmm5,[rbp]
	movdqa xmm6,[rbp+16]
	movdqa xmm7,[rbp+32]
	movdqa xmm9,[rbp+48]
	movdqa xmm10,[rbp+64]
	movdqa xmm11,[rbp+80]
	add rbp,96
	mov qword [rsp+88],rbp ;weight

	mov rsi,r10 ;inPt
	mov rbp,r8 ;width
	ALIGN 16
vf12nalop5:
	mov rax,qword [rbx]
	movdqa xmm2,xmm8 ;tmpV
	mov rdx,qword [rbx+8]
	pxor xmm3,xmm3
	movdqu xmm0,[rsi+rax]
	movdqa xmm4,xmm0
	movdqu xmm1,[rsi+rdx]
	punpcklwd xmm0, xmm1
	punpckhwd xmm4, xmm1
	mov rax,qword [rbx+16]
	pmaddwd xmm0,xmm5
	pmaddwd xmm4,xmm5
	mov rdx,qword [rbx+24]
	paddd xmm2,xmm0
	movdqu xmm0,[rsi+rax]
	paddd xmm3,xmm4
	movdqu xmm1,[rsi+rdx]
	movdqa xmm4,xmm0
	punpcklwd xmm0, xmm1
	punpckhwd xmm4, xmm1
	mov rax,qword [rbx+32]
	pmaddwd xmm0,xmm6
	pmaddwd xmm4,xmm6
	mov rdx,qword [rbx+40]
	paddd xmm2,xmm0
	movdqu xmm0,[rsi+rax]
	paddd xmm3,xmm4
	movdqu xmm1,[rsi+rdx]
	movdqa xmm4,xmm0
	punpcklwd xmm0, xmm1
	punpckhwd xmm4, xmm1
	mov rax,qword [rbx+48]
	pmaddwd xmm0,xmm7
	pmaddwd xmm4,xmm7
	mov rdx,qword [rbx+56]
	paddd xmm2,xmm0
	movdqu xmm0,[rsi+rax]
	paddd xmm3,xmm4
	movdqu xmm1,[rsi+rdx]
	movdqa xmm4,xmm0
	punpcklwd xmm0, xmm1
	punpckhwd xmm4, xmm1
	mov rax,qword [rbx+64]
	pmaddwd xmm0,xmm9
	pmaddwd xmm4,xmm9
	mov rdx,qword [rbx+72]
	paddd xmm2,xmm0
	movdqu xmm0,[rsi+rax]
	paddd xmm3,xmm4
	movdqu xmm1,[rsi+rdx]
	movdqa xmm4,xmm0
	punpcklwd xmm0, xmm1
	punpckhwd xmm4, xmm1
	mov rax,qword [rbx+80]
	pmaddwd xmm0,xmm10
	pmaddwd xmm4,xmm10
	mov rdx,qword [rbx+88]
	paddd xmm2,xmm0
	movdqu xmm0,[rsi+rax]
	paddd xmm3,xmm4
	movdqu xmm1,[rsi+rdx]
	movdqa xmm4,xmm0
	punpcklwd xmm0, xmm1
	punpckhwd xmm4, xmm1
	pmaddwd xmm0,xmm11
	pmaddwd xmm4,xmm11
	paddd xmm2,xmm0
	paddd xmm3,xmm4

	psrad xmm3,15
	psrad xmm2,15
	packssdw xmm2,xmm3
	pextrw rdx,xmm2,0
	mov eax,dword [rdi+rdx*4]
	pextrw rdx,xmm2,1
	or eax,dword [rdi+rdx*4+262144]
	pextrw rdx,xmm2,2
	or eax,dword [rdi+rdx*4+524288]
	movnti dword [rcx],eax

	pextrw rdx,xmm2,4
	mov eax,dword [rdi+rdx*4]
	pextrw rdx,xmm2,5
	or eax,dword [rdi+rdx*4+262144]
	pextrw rdx,xmm2,6
	or eax,dword [rdi+rdx*4+524288]
	movnti dword [rcx+4],eax

	lea rcx,[rcx+8]
	lea rsi,[rsi+16]
	dec rbp
	jnz vf12nalop5

	lea rbx,[rbx+96]
	add rcx,qword [rsp+104] ;dAdd

	dec r9 ;currHeight
	jnz vf12nalop4
	jmp vfnaexit
	
	align 16
vfnastart:
	shr qword [rsp+72],1 ;tap
	shr r8,1 ;width
	mov rcx,r11 ;outPt
	pxor xmm3,xmm3
	align 16
vfnalop4:
	mov rsi,r10 ;inPt
	mov rbp,r8 ;width

	align 16
vfnalop5:
	mov rdi,qword [rsp+88] ;weight
	mov rbx,qword [rsp+80] ;index

	mov rdx,qword [rsp+72] ;tap
	pxor xmm2,xmm2
	pxor xmm6,xmm6
	ALIGN 16
vfnalop6:
	mov rax,qword [rbx]
	movdqu xmm0,[rsi+rax]
	mov rax,qword [rbx+8]
	movdqa xmm4,xmm0
	movdqu xmm1,[rsi+rax]
	punpcklwd xmm0, xmm1
	punpckhwd xmm4, xmm1
	movdqa xmm5,[rdi]
	pmaddwd xmm0,xmm5
	pmaddwd xmm4,xmm5
	paddd xmm2,xmm0
	paddd xmm6,xmm4
	lea rdi,[rdi+16]
	lea rbx,[rbx+16]

	dec rdx
	jnz vfnalop6

	mov rdi,qword [rsp+112] ;rgbTable

	psrad xmm6,15
	packssdw xmm6,xmm3
	pextrw rdx,xmm6,0
	mov eax,dword [rdi+rdx*4]
	pextrw rdx,xmm6,1
	or eax,dword [rdi+rdx*4+262144]
	pextrw rdx,xmm6,2
	or eax,dword [rdi+rdx*4+524288]
	movd xmm0,eax

	psrad xmm2,15
	packssdw xmm2,xmm3
	pextrw rdx,xmm2,0
	mov eax,dword [rdi+rdx*4]
	pextrw rdx,xmm2,1
	or eax,dword [rdi+rdx*4+262144]
	pextrw rdx,xmm2,2
	or eax,dword [rdi+rdx*4+524288]

	mov dword [rcx],eax
	movd dword [rcx+4],xmm0
	lea ecx,[rcx+8]
	lea esi,[rsi+16]
	dec rbp
	jnz vfnalop5

	mov rax,qword [rsp+72] ;tap
	shl rax,4
	add qword [rsp+80],rax ;index
	add qword [rsp+88],rax ;weight

	add rcx,qword [rsp+104] ;dAdd

	dec r9 ;currHeight
	jnz vfnalop4

	align 16
vfnaexit:
	pop rdi
	pop rsi
	pop rbx
	pop rbp
	ret

;void LanczosResizerLR_C32_CPU_hv_filter(UInt8 *inPt, UInt8 *outPt, OSInt dwidth, OSInt dheight, OSInt swidth, OSInt htap, OSInt *hindex, Int64 *hweight, OSInt vtap, OSInt *vindex, Int64 *vweight, OSInt sstep, OSInt dstep, UInt8 *rgbTable, UInt8 *buffPt)
;0 currWidth
;8 edi
;16 esi
;24 ebx
;32 ebp
;40 retAddr
;rcx inPt r10
;rdx outPt r11
;r8 dwidth
;r9 dheight / currHeight
;80 swidth
;88 htap
;96 hindex
;104 hweight
;112 vtap
;120 vindex
;128 vweight
;136 sstep
;144 dstep / dAdd
;152 rgbTable
;160 buffPt

	align 16
LanczosResizerLR_C32_CPU_hv_filter:
	push rbp
	push rbx
	push rsi
	push rdi
	push rax
	mov r10,rcx
	mov r11,rdx
	cmp qword [rsp+88],6 ;htap
	jnz hvfexit
	cmp qword [rsp+112],6 ;vtap
	jnz hvfexit

	shr qword [rsp+80],1 ;swidth
	shr r8,1 ;dwidth
	align 16
hvf6lop1:
	mov rdi,qword [rsp+128] ;vweight
	movdqa xmm5,[rdi]
	movdqa xmm6,[rdi+16]
	movdqa xmm7,[rdi+32]

	mov rsi,r10 ;inPt
	mov rcx,qword [rsp+160] ;buffPt

	mov rdi,qword [rsp+80] ;swidth
	ALIGN 16
hvf6lop2:
	mov rbx,qword [rsp+120] ;vindex
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

	psrad xmm3,15
	psrad xmm2,15
	packssdw xmm2,xmm3
	movdqa [rcx],xmm2

	add rcx,16
	add rsi,16
	dec rdi
	jnz hvf6lop2

	add qword [rsp+120],48 ;vindex
	add qword [rsp+128],48 ;vweight

	mov rbp,r8 ;dwidth ;currWidth
	mov rsi,r11 ;outPt

	mov rdi,qword [rsp+104] ;hweight
	mov rbx,qword [rsp+96] ;hindex
	ALIGN 16
hvf6lop3:
	mov rcx,qword [rsp+160] ;buffPt
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
	add rdi,48
	add rbx,48

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
	add rdi,48
	add rbx,48

	psrad xmm1,15
	psrad xmm3,15
	packssdw xmm1,xmm3

	mov rcx,qword [rsp+152] ;rgbTable
	pextrw rdx,xmm1,0
	mov eax,dword [rcx+rdx*4]
	pextrw rdx,xmm1,1
	or eax,dword [rcx+rdx*4+262144]
	pextrw rdx,xmm1,2
	or eax,dword [rcx+rdx*4+524288]
	mov dword [rsi],eax

	pextrw rdx,xmm1,4
	mov eax,dword [rcx+rdx*4]
	pextrw rdx,xmm1,5
	or eax,dword [rcx+rdx*4+262144]
	pextrw rdx,xmm1,6
	or eax,dword [rcx+rdx*4+524288]
	mov dword [rsi+4],eax

	add rsi,8
	dec rbp ;currWidth
	jnz hvf6lop3

	add r11,qword [rsp+144] ;dstep outPt

	dec r9 ;currHeight
	jnz hvf6lop1
	
	align 16
hvfexit:
	pop rax
	pop rdi
	pop rsi
	pop rbx
	pop rbp
	ret

;void LanczosResizerLR_C32_CPU_collapse(UInt8 *inPt, UInt8 *outPt, OSInt width, OSInt height, OSInt sstep, OSInt dstep, UInt8 *rgbTable)
;0 rdi
;8 rsi
;16 rbx
;24 rbp
;32 retAddr
;rcx inPt r10
;rdx outPt r11
;r8 width
;r9 height
;72 sstep / sAdd
;80 dstep / dAdd
;88 rgbTable

	align 16
LanczosResizerLR_C32_CPU_collapse:
	push rbp
	push rbx
	push rsi
	push rdi
	mov r10,rcx
	mov r11,rdx
	lea rcx,[r8*8] ;width
	lea rdx,[r8*4] ;width
	sub qword [rsp+72],rcx ;sAdd		OSInt sAdd = sstep - width * 8;
	sub qword [rsp+80],rdx ;dAdd		OSInt dAdd = dstep - width * 4;
	test r8,3
	jnz colstart
	test r11,15 ;outPt
	jnz colstart
	test qword [rsp+80],15 ;dstep
	jz col16start
	align 16
colstart:						;if ((width & 3) || (((OSInt)outPt) & 15) || (dstep & 15))
	mov rbx,qword [rsp+88] ;rgbTable
	mov rcx,r10 ;inPt
	mov rsi,r11 ;outPt
	align 16
collop:
	mov rbp,r8 ;width
	align 16
collop2:
	movzx rdx,word [rcx]
	mov eax,dword [rbx+rdx*4]
	movzx rdx,word [rcx+2]
	or eax,dword [rbx+rdx*4+262144]
	movzx rdx,word [rcx+4]
	or eax,dword [rbx+rdx*4+524288]
	movzx rdx,word [rcx+6]
	or eax,dword [rbx+rdx*4+786432]

	movnti dword [rsi],eax
	lea rsi,[rsi+4]
	lea rcx,[rcx+8]
	dec rbp
	jnz collop2

	add rcx,qword [rsp+72] ;sAdd
	add rsi,qword [rsp+80] ;dAdd

	dec r9 ;height
	jnz collop
	jmp colexit

	align 16
col16start:
	shr r8,2 ;width
	mov rbx,qword [rsp+88] ;rgbTable
	mov rcx,r10 ;inPt
	mov rsi,r11 ;outPt
	align 16
collop16:
	mov rdi,r8 ;width

	align 16
collop16_2:
	movdqu xmm4,[rcx]
	movdqu xmm5,[rcx+16]
	pextrw rdx,xmm4,0
	mov eax,dword [rbx+rdx*4]
	pextrw rdx,xmm4,1
	or eax,dword [rbx+rdx*4+262144]
	pextrw rdx,xmm4,2
	or eax,dword [rbx+rdx*4+524288]
	pextrw rdx,xmm4,3
	or eax,dword [rbx+rdx*4+786432]
	movd xmm0,eax

	pextrw rdx,xmm4,4
	mov eax,dword [rbx+rdx*4]
	pextrw rdx,xmm4,5
	or eax,dword [rbx+rdx*4+262144]
	pextrw rdx,xmm4,6
	or eax,dword [rbx+rdx*4+524288]
	pextrw rdx,xmm4,7
	or eax,dword [rbx+rdx*4+786432]
	movd xmm1,eax
	punpckldq xmm0,xmm1

	pextrw rdx,xmm5,0
	mov eax,dword [rbx+rdx*4]
	pextrw rdx,xmm5,1
	or eax,dword [rbx+rdx*4+262144]
	pextrw rdx,xmm5,2
	or eax,dword [rbx+rdx*4+524288]
	pextrw rdx,xmm5,3
	or eax,dword [rbx+rdx*4+786432]
	movd xmm1,eax

	pextrw rdx,xmm5,4
	mov eax,dword [rbx+rdx*4]
	pextrw rdx,xmm5,5
	or eax,dword [rbx+rdx*4+262144]
	pextrw rdx,xmm5,6
	or eax,dword [rbx+rdx*4+524288]
	pextrw rdx,xmm5,7
	or eax,dword [rbx+rdx*4+786432]
	movd xmm2,eax
	punpckldq xmm1,xmm2
	punpcklqdq xmm0,xmm1

	movntdq [rsi],xmm0
	lea rcx,[rcx+32]
	lea rsi,[rsi+16]
	dec rdi
	jnz collop16_2

	add rcx,qword [rsp+72] ;sAdd
	add rsi,qword [rsp+80] ;dAdd

	dec r9 ;height
	jnz collop16

	align 16
colexit:
	pop rdi
	pop rsi
	pop rbx
	pop rbp
	ret

;void LanczosResizerLR_C32_CPU_collapse_na(UInt8 *inPt, UInt8 *outPt, OSInt width, OSInt height, OSInt sstep, OSInt dstep, UInt8 *rgbTable)
;0 rdi
;8 rsi
;16 rbx
;24 rbp
;32 retAddr
;rcx inPt
;rdx outPt
;r8 width
;r9 height
;72 sstep / sAdd
;80 dstep / dAdd
;88 rgbTable

	align 16
LanczosResizerLR_C32_CPU_collapse_na:
	push rbp
	push rbx
	push rsi
	push rdi
	mov r10,rcx
	mov r11,rdx
	lea rcx,[r8*8] ;width
	lea rdx,[r8*4] ;width
	sub qword [rsp+72],rcx ;sAdd		OSInt sAdd = sstep - width * 8;
	sub qword [rsp+80],rdx ;dAdd		OSInt dAdd = dstep - width * 4;
	test r8,3
	jnz colnastart
	test r11,15 ;outPt
	jnz colnastart
	test qword [rsp+80],15 ;dstep
	jz colna16start

	align 16
colnastart:					; if ((width & 3) || (((OSInt)outPt) & 15) || (dstep & 15))
	mov rbx,qword [rsp+88] ;rgbTable
	mov rcx,r10 ;inPt
	mov rsi,r11 ;outPt

	align 16
colnalop:
	mov rdi,r8 ;width
	
	align 16
colnalop2:
	movzx rdx,word [rcx]
	mov eax,dword [rbx+rdx*4]
	movzx rdx,word [rcx+2]
	or eax,dword [rbx+rdx*4+262144]
	movzx rdx,word [rcx+4]
	or eax,dword [rbx+rdx*4+524288]

	mov dword [rsi],eax
	lea rsi,[rsi+4]
	lea rcx,[rcx+8]
	dec rdi
	jnz colnalop2

	add rcx,qword [rsp+72] ;sAdd
	add rsi,qword [rsp+80] ;dAdd

	dec r9 ;height
	jnz colnalop
	jmp colnaexit
	
	align 16
colna16start:
	shr r8,2 ;width
	mov rbx,qword [rsp+88] ;rgbTable
	mov rcx,r10 ;inPt
	mov rsi,r11 ;outPt
	align 16
colnalop16:
	mov rdi,r8 ;width

	align 16
colnalop16_2:
	prefetcht0 [rcx+128]
	movdqu xmm4,[rcx]
	movdqu xmm5,[rcx+16]
	pextrw rdx,xmm4,0
	mov eax,dword [rbx+rdx*4]
	pextrw rdx,xmm4,1
	or eax,dword [rbx+rdx*4+262144]
	pextrw rdx,xmm4,2
	or eax,dword [rbx+rdx*4+524288]
	movd xmm0,eax

	pextrw rdx,xmm4,4
	mov eax,dword [rbx+rdx*4]
	pextrw rdx,xmm4,5
	or eax,dword [rbx+rdx*4+262144]
	pextrw rdx,xmm4,6
	or eax,dword [rbx+rdx*4+524288]
	movd xmm1,eax
	punpckldq xmm0,xmm1

	pextrw rdx,xmm5,0
	mov eax,dword [rbx+rdx*4]
	pextrw rdx,xmm5,1
	or eax,dword [rbx+rdx*4+262144]
	pextrw rdx,xmm5,2
	or eax,dword [rbx+rdx*4+524288]
	movd xmm1,eax

	pextrw rdx,xmm5,4
	mov eax,dword [rbx+rdx*4]
	pextrw rdx,xmm5,5
	or eax,dword [rbx+rdx*4+262144]
	pextrw rdx,xmm5,6
	or eax,dword [rbx+rdx*4+524288]
	movd xmm2,eax
	punpckldq xmm1,xmm2
	punpcklqdq xmm0,xmm1

	movntdq [rsi],xmm0
	lea rsi,[rsi+16]
	lea rcx,[rcx+32]
	dec rdi
	jnz colnalop16_2

	add rcx,qword [rsp+72] ;sAdd
	add rsi,qword [rsp+80] ;dAdd

	dec r9 ;height
	jnz colnalop16

	align 16
colnaexit:
	pop rdi
	pop rsi
	pop rbx
	pop rbp
	ret
