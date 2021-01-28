section .text

global LanczosResizerLR_C32_CPU_horizontal_filter
global _LanczosResizerLR_C32_CPU_horizontal_filter
global LanczosResizerLR_C32_CPU_vertical_filter
global _LanczosResizerLR_C32_CPU_vertical_filter
global LanczosResizerLR_C32_CPU_vertical_filter_na
global _LanczosResizerLR_C32_CPU_vertical_filter_na
global LanczosResizerLR_C32_CPU_hv_filter
global _LanczosResizerLR_C32_CPU_hv_filter
global LanczosResizerLR_C32_CPU_collapse
global _LanczosResizerLR_C32_CPU_collapse
global LanczosResizerLR_C32_CPU_collapse_na
global _LanczosResizerLR_C32_CPU_collapse_na

extern _CPUBrand
extern _UseAVX

;void LanczosResizerLR_C32_CPU_horizontal_filter(UInt8 *inPt, UInt8 *outPt, OSInt width, OSInt height, OSInt tap, OSInt *index, Int64 *weight, OSInt sstep, OSInt dstep, UInt8 *rgbTable)
;xmm7 tmpV
;0 rbx
;8 rbp
;16 retAddr
;rdi inPt
;rsi outPt
;rdx width
;rcx height / currHeight
;r8 tap
;r9 index
;24 weight
;32 sstep
;40 dstep / dAdd
;48 rgbTable

	align 16
test16:
	mov rax,0x4000
	mov r10,rax
	shl r10,16
	or r10,rax
	shl r10,16
	or r10,rax
	shl r10,16
	or r10,rax
	lea rax,[rdx*8]	; OSInt dAdd = dstep - width * 8;
	sub qword [rsp+24],rax ;dAdd
	align 16
test16_1:
	mov r8,rdx
	align 16
test16_2:
	mov qword [rsi],r10
	lea rsi,[rsi+8]
	dec r8
	jnz test16_2
	add rsi,qword [rsp+24]
	dec rcx
	jnz test16_1
	ret

	align 16
LanczosResizerLR_C32_CPU_horizontal_filter:
_LanczosResizerLR_C32_CPU_horizontal_filter:
;	jmp test16
	push rbp
	push rbx
	mov eax,0x4000
	movd xmm7,eax
	punpckldq xmm7,xmm7				; Int32 tmpV[4] = {16384, 16384, 16384, 16384};
	punpckldq xmm7,xmm7
	lea rax,[rdx*8]				; OSInt dAdd = dstep - width * 8;
	sub qword [rsp+40],rax ;dAdd
	test rdx,1 ;width
	jnz hfstart
	cmp r8,6 ;tap
	jz hf6nstart
	cmp r8,8 ;tap
	jz hf8nstart
	cmp r8,16 ;tap
	jz hf16nstart
	jmp hfstart2
	
	align 16
hfstart:						; if (width & 1)
	shr r8,1 ;tap
	pxor xmm3,xmm3
	align 16
hflop:
	mov rbp,rdx ;width
	mov r11,r9 ;index
	mov rbx,qword [rsp+24] ;weight
	align 16
hflop2:
	mov r10,r8 ;tap
	movdqa xmm1,xmm7
	ALIGN 16
hflop3:
	mov rax,qword [r11]
	movq xmm0,[rdi+rax]
	mov rax,qword [r11+8]
	movq xmm2,[rdi+rax]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0, [rbx]
	paddd xmm1,xmm0
	lea rbx,[rbx+16]
	lea r11,[r11+16]
	dec r10
	jnz hflop3

	psrad xmm1,15
	packssdw xmm1,xmm3
	movq [rsi],xmm1
	lea rsi,[rsi+8]
	dec rbp
	jnz hflop2

	add rdi,qword [rsp+32] ;sstep
	add rsi,qword [rsp+40] ;dAdd

	dec rcx ;currHeight
	jnz hflop
	jmp hfexit

	align 16
hf6nstart:						; else if (tap == 6)
	test rsi,15 ;outPt
	jnz hf6start
	test qword [rsp+40],15 ;dstep
	jnz hf6start
								; if ((((OSInt)outPt) & 15) == 0 && (dstep & 15) == 0)
	shr rdx,1 ;width
	mov r8,qword [rsp+24] ;weight
	ALIGN 16
hf6nlop4:

	mov rbp,rdx ;width

	mov r10,r9 ;index
	mov r11,r8 ;weight
	ALIGN 16
hf6nlop5:
	mov rax,qword [r10]
	movdqa xmm1,xmm7
	movdqu xmm0,[rdi+rax]
	mov rax,qword [r10+8]
	movdqa xmm4,xmm0
	movdqu xmm2,[rdi+rax]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[r11]
	punpckhwd xmm4,xmm2
	pmaddwd xmm4,[r11+16]
	paddd xmm1,xmm0
	mov rax,qword [r10+16]
	paddd xmm1,xmm4
	movdqu xmm0,[rdi+rax]
	pshufd xmm2,xmm0,14
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[r11+32]
	paddd xmm1,xmm0

	mov rax,qword [r10+24]
	movdqa xmm3,xmm7
	movdqu xmm0,[rdi+rax]
	mov rax,qword [r10+32]
	movdqa xmm4,xmm0
	movdqu xmm2,[rdi+rax]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[r11+48]
	punpckhwd xmm4,xmm2
	pmaddwd xmm4,[r11+64]
	paddd xmm3,xmm0
	mov rax,qword [r10+40]
	paddd xmm3,xmm4
	movdqu xmm0,[rdi+rax]
	pshufd xmm2,xmm0,14
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[r11+80]
	paddd xmm3,xmm0

	lea r11,[r11+96]
	lea r10,[r10+48]

	psrad xmm1,15
	psrad xmm3,15
	packssdw xmm1,xmm3
	movntdq [rsi],xmm1

	lea rsi,[rsi+16]
	dec rbp
	jnz hf6nlop5

	add rdi,qword [rsp+32] ;sstep
	add rsi,qword [rsp+40] ;dAdd

	dec rcx ;height
	jnz hf6nlop4
	jmp hfexit

	align 16
hf6start:
	shr rdx,1 ;width
	mov r8,qword [rsp+24] ;weight
	ALIGN 16
hf6lop4:
	mov rbp,rdx ;width
	mov rbx,r9 ;index
	mov r11,r8 ;weight
	ALIGN 16
hf6lop5:

	mov rax,qword [rbx]
	movdqa xmm1,xmm7
	movdqu xmm0,[rdi+rax]
	mov rax,qword [rbx+8]
	movdqa xmm4,xmm0
	movdqu xmm2,[rdi+rax]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[r11]
	punpckhwd xmm4,xmm2
	pmaddwd xmm4,[r11+16]
	paddd xmm1,xmm0
	mov rax,qword [rbx+16]
	paddd xmm1,xmm4
	movq xmm0,[rdi+rax]
	movq xmm2,[rdi+rax+8]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[r11+32]
	paddd xmm1,xmm0

	mov rax,qword [rbx+24]
	movdqa xmm3,xmm7
	movdqu xmm0,[rdi+rax]
	mov rax,qword [rbx+32]
	movdqa xmm4,xmm0
	movdqu xmm2,[rdi+rax]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[r11+48]
	punpckhwd xmm4,xmm2
	pmaddwd xmm4,[r11+64]
	paddd xmm3,xmm0
	mov rax,qword [rbx+40]
	paddd xmm3,xmm4
	movq xmm0,[rdi+rax]
	movq xmm2,[rdi+rax+8]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[r11+80]
	paddd xmm3,xmm0
	lea r11,[r11+96]
	lea rbx,[rbx+48]

	psrad xmm1,15
	psrad xmm3,15
	packssdw xmm1,xmm3
	movdqu [rsi],xmm1

	lea rsi,[rsi+16]
	dec rbp
	jnz hf6lop5

	add rdi,qword [rsp+32] ;sstep
	add rsi,qword [rsp+40] ;dAdd

	dec rcx ;height
	jnz hf6lop4
	jmp hfexit

	align 16
hf8nstart:						; else if (tap == 8)
	test rsi,15 ;outPt
	jnz hf8start
	test qword [rsp+40],15 ;dstep
	jnz hf8start
	
	cmp dword [rel _UseAVX],0
	jnz hf8navxstart
								; if ((((OSInt)outPt) & 15) == 0 && (dstep & 15) == 0)
	mov rbx,qword [rsp+24] ;weight
	shr rdx,1 ;width
	mov r8,qword [rsp+32] ;sstep
	mov r10,qword [rsp+40] ;dAdd

	ALIGN 16
hf8nlop4:
	mov rbp,rdx ;width
	mov r11,rbx ;weight
	
	ALIGN 16
hf8nlop5:
	mov rax,qword [r11]
	add rax,rdi
	movdqa xmm1,xmm7
	movdqu xmm0,[rax]
	movdqa xmm4,xmm0
	movdqu xmm2,[rax+16]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[r11+16]
	punpckhwd xmm4,xmm2
	pmaddwd xmm4,[r11+32]
	paddd xmm1,xmm0
	movdqu xmm0,[rax+32]
	paddd xmm1,xmm4
	movdqu xmm2,[rax+48]
	movdqa xmm4,xmm0
	punpcklwd xmm0,xmm2
	punpckhwd xmm4,xmm2
	pmaddwd xmm0,[r11+48]
	paddd xmm1,xmm0
	pmaddwd xmm4,[r11+64]
	paddd xmm1,xmm4

	mov rax,qword [r11+8]
	add rax,rdi
	movdqa xmm3,xmm7
	movdqu xmm0,[rax]
	movdqa xmm4,xmm0
	movdqu xmm2,[rax+16]
	punpcklwd xmm0,xmm2
	punpckhwd xmm4,xmm2
	pmaddwd xmm0,[r11+80]
	paddd xmm3,xmm0
	pmaddwd xmm4,[r11+96]
	paddd xmm3,xmm4
	movdqu xmm0,[rax+32]
	movdqa xmm4,xmm0
	movdqu xmm2,[rax+48]
	punpcklwd xmm0,xmm2
	punpckhwd xmm4,xmm2
	pmaddwd xmm0,[r11+112]
	paddd xmm3,xmm0
	pmaddwd xmm4,[r11+128]
	paddd xmm3,xmm4

	lea r11,[r11+144]

	psrad xmm1,15
	psrad xmm3,15
	packssdw xmm1,xmm3
	movdqa [rsi],xmm1

	lea rsi,[rsi+16]
	dec rbp
	jnz hf8nlop5

	add rdi,r8 ;sstep
	add rsi,r10 ;dAdd

	dec rcx ;height
	jnz hf8nlop4
	jmp hfexit

	align 16
hf8navxstart:
	mov rbx,qword [rsp+24] ;weight
	shr rdx,1 ;width
	mov r8,qword [rsp+32] ;sstep
	mov r10,qword [rsp+40] ;dAdd

	ALIGN 16
hf8navxlop4:
	mov rbp,rdx ;width
	mov r11,rbx ;weight
	
	ALIGN 16
hf8navxlop5:
	mov rax,qword [r11]
	add rax,rdi
	movdqa xmm1,xmm7
	movdqu xmm0,[rax]
	movdqu xmm2,[rax+16]
	vpunpckhwd xmm4,xmm0,xmm2
	pmaddwd xmm4,[r11+32]
	vpunpcklwd xmm0,xmm0,xmm2
	pmaddwd xmm0,[r11+16]
	paddd xmm1,xmm0
	movdqu xmm0,[rax+32]
	paddd xmm1,xmm4
	movdqu xmm2,[rax+48]
	vpunpckhwd xmm4,xmm0,xmm2
	vpunpcklwd xmm0,xmm0,xmm2
	pmaddwd xmm0,[r11+48]
	paddd xmm1,xmm0
	pmaddwd xmm4,[r11+64]
	paddd xmm1,xmm4

	mov rax,qword [r11+8]
	add rax,rdi
	movdqa xmm3,xmm7
	movdqu xmm0,[rax]
	movdqu xmm2,[rax+16]
	vpunpckhwd xmm4,xmm0,xmm2
	vpunpcklwd xmm0,xmm0,xmm2
	pmaddwd xmm0,[r11+80]
	paddd xmm3,xmm0
	pmaddwd xmm4,[r11+96]
	paddd xmm3,xmm4
	movdqu xmm0,[rax+32]
	movdqu xmm2,[rax+48]
	vpunpckhwd xmm4,xmm0,xmm2
	vpunpcklwd xmm0,xmm0,xmm2
	pmaddwd xmm0,[r11+112]
	paddd xmm3,xmm0
	pmaddwd xmm4,[r11+128]
	paddd xmm3,xmm4

	lea r11,[r11+144]

	psrad xmm1,15
	psrad xmm3,15
	packssdw xmm1,xmm3
	movdqa [rsi],xmm1

	lea rsi,[rsi+16]
	dec rbp
	jnz hf8navxlop5

	add rdi,r8 ;sstep
	add rsi,r10 ;dAdd

	dec rcx ;height
	jnz hf8navxlop4
	vzeroupper
	jmp hfexit

	align 16
hf8start:
	shr rdx,1 ;width
	mov rbx,qword [rsp+24] ;weight
	mov r8,qword [rsp+32] ;sstep
	mov r10,qword [rsp+40] ;dstep
	ALIGN 16
hf8lop4:
	mov rbp,rdx ;width
	mov r11,rbx ;weight
	ALIGN 16
hf8lop5:
	mov rax,qword [r11]
	prefetcht0 [rdi+rax+128]
	movdqa xmm1,xmm7
	movdqu xmm0,[rdi+rax]
	movdqa xmm4,xmm0
	movdqu xmm2,[rdi+rax+16]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[r11+16]
	punpckhwd xmm4,xmm2
	pmaddwd xmm4,[r11+32]
	paddd xmm1,xmm0
	paddd xmm1,xmm4
	movdqu xmm0,[rdi+rax+32]
	movdqa xmm4,xmm0
	movdqu xmm2,[rdi+rax+48]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[r11+48]
	punpckhwd xmm4,xmm2
	pmaddwd xmm4,[r11+64]
	paddd xmm1,xmm0
	paddd xmm1,xmm4

	mov rax,qword [r11+8]
	movdqa xmm3,xmm7
	movdqu xmm0,[rdi+rax]
	movdqa xmm4,xmm0
	movdqu xmm2,[rdi+rax+16]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[r11+80]
	punpckhwd xmm4,xmm2
	pmaddwd xmm4,[r11+96]
	paddd xmm3,xmm0
	paddd xmm3,xmm4
	movdqu xmm0,[rdi+rax+32]
	movdqa xmm4,xmm0
	movdqu xmm2,[rdi+rax+48]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[r11+112]
	punpckhwd xmm4,xmm2
	pmaddwd xmm4,[r11+128]
	paddd xmm3,xmm0
	paddd xmm3,xmm4
	lea r11,[r11+144]

	psrad xmm1,15
	psrad xmm3,15
	packssdw xmm1,xmm3
	movdqu [rsi],xmm1

	lea rsi,[rsi+16]
	dec rbp
	jnz hf8lop5

	add rdi,r8 ;sstep
	add rsi,r10 ;dAdd

	dec rcx ;height
	jnz hf8lop4
	jmp hfexit

	align 16
hf16nstart:						; else if (tap == 16)
	test rsi,15 ;outPt
	jnz hf16start
	test qword [rsp+40],15 ;dstep
	jnz hf16start
								; if ((((OSInt)outPt) & 15) == 0 && (dstep & 15) == 0)
	shr rdx,1 ;width
	mov rbx,qword [rsp+24] ;weight
	mov r8,qword [rsp+32] ;sstep
	mov r10,qword [rsp+40] ;dstep
	ALIGN 16
hf16nlop4:
	mov rbp,rdx ;width
	mov r11,rbx ;weight
	ALIGN 16
hf16nlop5:
	mov rax,qword [r11]
	prefetcht0 [rdi+rax+128]
	movdqa xmm1,xmm7
	movdqu xmm0,[rdi+rax]
	movdqa xmm4,xmm0
	movdqu xmm2,[rdi+rax+16]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[r11+16]
	punpckhwd xmm4,xmm2
	pmaddwd xmm4,[r11+32]
	paddd xmm1,xmm0
	paddd xmm1,xmm4
	movdqu xmm0,[rdi+rax+32]
	movdqa xmm4,xmm0
	movdqu xmm2,[rdi+rax+48]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[r11+48]
	punpckhwd xmm4,xmm2
	pmaddwd xmm4,[r11+64]
	paddd xmm1,xmm0
	paddd xmm1,xmm4
	movdqu xmm0,[rdi+rax+64]
	movdqa xmm4,xmm0
	movdqu xmm2,[rdi+rax+80]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[r11+80]
	punpckhwd xmm4,xmm2
	pmaddwd xmm4,[r11+96]
	paddd xmm1,xmm0
	paddd xmm1,xmm4
	movdqu xmm0,[rdi+rax+96]
	movdqa xmm4,xmm0
	movdqu xmm2,[rdi+rax+112]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[r11+112]
	punpckhwd xmm4,xmm2
	pmaddwd xmm4,[r11+128]
	paddd xmm1,xmm0
	paddd xmm1,xmm4

	mov rax,qword [r11+8]
	movdqa xmm3,xmm7
	movdqu xmm0,[rdi+rax]
	movdqa xmm4,xmm0
	movdqu xmm2,[rdi+rax+16]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[r11+144]
	punpckhwd xmm4,xmm2
	pmaddwd xmm4,[r11+160]
	paddd xmm3,xmm0
	paddd xmm3,xmm4
	movdqu xmm0,[rdi+rax+32]
	movdqa xmm4,xmm0
	movdqu xmm2,[rdi+rax+48]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[r11+176]
	punpckhwd xmm4,xmm2
	pmaddwd xmm4,[r11+192]
	paddd xmm3,xmm0
	paddd xmm3,xmm4
	movdqu xmm0,[rdi+rax+64]
	movdqa xmm4,xmm0
	movdqu xmm2,[rdi+rax+80]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[r11+208]
	punpckhwd xmm4,xmm2
	pmaddwd xmm4,[r11+224]
	paddd xmm3,xmm0
	paddd xmm3,xmm4
	movdqu xmm0,[rdi+rax+96]
	movdqa xmm4,xmm0
	movdqu xmm2,[rdi+rax+112]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[r11+240]
	punpckhwd xmm4,xmm2
	pmaddwd xmm4,[r11+256]
	paddd xmm3,xmm0
	paddd xmm3,xmm4

	lea r11,[r11+272]

	psrad xmm1,15
	psrad xmm3,15
	packssdw xmm1,xmm3
	movntdq [rsi],xmm1

	lea rsi,[rsi+16]
	dec rbp
	jnz hf16nlop5

	add rdi,r8 ;sstep
	add rsi,r10 ;dAdd

	dec rcx ;height
	jnz hf16nlop4
	jmp hfexit

	align 16	
hf16start:
	shr rdx,1 ;width
	mov rbx,qword [rsp+24] ;weight
	mov r8,qword [rsp+32] ;sstep
	mov r10,qword [rsp+40] ;dstep
	ALIGN 16
hf16lop4:
	mov rbp,rdx ;width
	mov rdi,rbx ;weight
	ALIGN 16
hf16lop5:
	mov rax,qword [r11]
	prefetcht0 [rdi+rax+128]
	movdqa xmm1,xmm7
	movdqu xmm0,[rdi+rax]
	movdqa xmm4,xmm0
	movdqu xmm2,[rdi+rax+16]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[r11+16]
	punpckhwd xmm4,xmm2
	pmaddwd xmm4,[r11+32]
	paddd xmm1,xmm0
	paddd xmm1,xmm4
	movdqu xmm0,[rdi+rax+32]
	movdqa xmm4,xmm0
	movdqu xmm2,[rdi+rax+48]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[r11+48]
	punpckhwd xmm4,xmm2
	pmaddwd xmm4,[r11+64]
	paddd xmm1,xmm0
	paddd xmm1,xmm4
	movdqu xmm0,[rdi+rax+64]
	movdqa xmm4,xmm0
	movdqu xmm2,[rdi+rax+80]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[r11+80]
	punpckhwd xmm4,xmm2
	pmaddwd xmm4,[r11+96]
	paddd xmm1,xmm0
	paddd xmm1,xmm4
	movdqu xmm0,[rdi+rax+96]
	movdqa xmm4,xmm0
	movdqu xmm2,[rdi+rax+112]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[r11+112]
	punpckhwd xmm4,xmm2
	pmaddwd xmm4,[r11+128]
	paddd xmm1,xmm0
	paddd xmm1,xmm4

	mov rax,qword [r11+8]
	movdqa xmm3,xmm7
	movdqu xmm0,[rdi+rax]
	movdqa xmm4,xmm0
	movdqu xmm2,[rdi+rax+16]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[r11+144]
	punpckhwd xmm4,xmm2
	pmaddwd xmm4,[r11+160]
	paddd xmm3,xmm0
	paddd xmm3,xmm4
	movdqu xmm0,[rdi+rax+32]
	movdqa xmm4,xmm0
	movdqu xmm2,[rdi+rax+48]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[r11+176]
	punpckhwd xmm4,xmm2
	pmaddwd xmm4,[r11+192]
	paddd xmm3,xmm0
	paddd xmm3,xmm4
	movdqu xmm0,[rdi+rax+64]
	movdqa xmm4,xmm0
	movdqu xmm2,[rdi+rax+80]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[r11+208]
	punpckhwd xmm4,xmm2
	pmaddwd xmm4,[r11+224]
	paddd xmm3,xmm0
	paddd xmm3,xmm4
	movdqu xmm0,[rdi+rax+96]
	movdqa xmm4,xmm0
	movdqu xmm2,[rdi+rax+112]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[r11+240]
	punpckhwd xmm4,xmm2
	pmaddwd xmm4,[r11+256]
	paddd xmm3,xmm0
	paddd xmm3,xmm4

	lea r11,[r11+272]

	psrad xmm1,15
	psrad xmm3,15
	packssdw xmm1,xmm3
	movdqu [rsi],xmm1

	lea rsi,[rsi+16]
	dec rbp
	jnz hf16lop5

	add rdi,r8 ;sstep
	add rsi,r10 ;dAdd

	dec rcx ;height
	jnz hf16lop4
	jmp hfexit

;xmm7 tmpV
;0 rbx
;8 rbp
;16 retAddr
;rdi inPt
;rsi outPt
;rdx width
;rcx height / currHeight
;r8 tap
;r9 index
;24 weight
;32 sstep
;40 dstep / dAdd
;48 rgbTable
	align 16
hfstart2:
	shr r8,1 ;tap
	shr rdx,1 ;width
	align 16
hflop4:
	mov rbp,rdx ;width
	mov rbx,r9 ;index
	mov r11,qword [rsp+24] ;weight	
	align 16
hflop5:
	mov r10,r8 ;tap
	movdqa xmm1,xmm7
	ALIGN 16
hflop6:
	mov rax,qword [rbx]
	movq xmm0,[rdi+rax]
	mov rax,qword [rbx+8]
	movq xmm2,[rdi+rax]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[r11]
	paddd xmm1,xmm0
	lea r11,[r11+16]
	lea rbx,[rbx+16]
	dec r10
	jnz hflop6

	mov r10,r8 ;tap
	movdqa xmm3,xmm7
	ALIGN 16
hflop7:
	mov rax,qword [rbx]
	movq xmm0,[rdi+rax]
	mov rax,qword [rbx+8]
	movq xmm2,[rdi+rax]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[r11]
	paddd xmm3,xmm0
	lea r11,[r11+16]
	lea rbx,[rbx+16]
	dec r10
	jnz hflop7

	psrad xmm1,15
	psrad xmm3,15
	packssdw xmm1,xmm3
	movdqu [rsi],xmm1

	lea rsi,[rsi+16]
	dec rbp
	jnz hflop5

	add rdi,qword [rsp+32] ;sstep
	add rsi,qword [rsp+40] ;dAdd

	dec rcx ;currHeight
	jnz hflop4

	align 16
hfexit:
	pop rbx
	pop rbp
	ret

;void LanczosResizerLR_C32_CPU_vertical_filter(UInt8 *inPt, UInt8 *outPt, OSInt width, OSInt height, OSInt tap, OSInt *index, Int64 *weight, OSInt sstep, OSInt dstep, UInt8 *rgbTable)
;0 xmm8
;16 rbx
;24 rbp
;32 retAddr
;rdi inPt
;rsi outPt
;rdx width
;rcx height / currHeight
;r8 tap
;r9 index
;40 weight
;48 sstep
;56 dstep / dAdd
;64 rgbTable

	align 16
LanczosResizerLR_C32_CPU_vertical_filter:
_LanczosResizerLR_C32_CPU_vertical_filter:
	mov eax,0x4000
	push rbp
	push rbx
	sub rsp,16
	movdqu [rsp],xmm8
	movd xmm0,eax					;Int32 tmpV[4] = {16384, 16384, 16384, 16384};
	punpckldq xmm0,xmm0
	punpckldq xmm0,xmm0
	movdqu xmm8,xmm0
	lea rax,[rdx*4]
	sub qword [rsp+56],rax ;dAdd
	test rdx,1						;if (width & 1)
	jz vfstart2
	cmp r8,6 ;tap
	jz vf6_1start
	jmp vf_1start
	
	align 16
vf6_1start:							; if (tap == 6)
	shr rdx,1 ;width
	mov rbp,qword [rsp+64] ;rgbTable
	mov rbx,qword [rsp+40] ;weight
	mov r10,rdx ;width
	xor rax,rax
	ALIGN 16
vf6_1lop4:
	movdqa xmm5,[rbx+32]
	movdqa xmm6,[rbx+48]
	movdqa xmm7,[rbx+64]

	mov r11,r10 ;width
	ALIGN 16
vf6_1lop5:
	mov eax,dword [rbx]
	movdqu xmm2,xmm8 ;tmpV
	mov edx,dword [rbx+4]
	movdqa xmm3,xmm2
	movdqu xmm0,[rdi+rax]
	movdqa xmm4,xmm0
	movdqu xmm1,[rdi+rdx]
	punpcklwd xmm0, xmm1
	punpckhwd xmm4, xmm1
	mov eax,dword [rbx+8]
	pmaddwd xmm0,xmm5
	pmaddwd xmm4,xmm5
	mov edx,dword [rbx+12]
	paddd xmm2,xmm0
	paddd xmm3,xmm4
	movdqu xmm0,[rdi+rax]
	movdqa xmm4,xmm0
	movdqu xmm1,[rdi+rdx]
	punpcklwd xmm0, xmm1
	punpckhwd xmm4, xmm1
	mov eax,dword [rbx+16]
	pmaddwd xmm0,xmm6
	pmaddwd xmm4,xmm6
	mov edx,dword [rbx+20]
	paddd xmm2,xmm0
	paddd xmm3,xmm4
	movdqu xmm0,[rdi+rax]
	movdqa xmm4,xmm0
	movdqu xmm1,[rdi+rdx]
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
	mov eax,dword [rbp+rdx*4]
	pextrw rdx,xmm2,1
	or eax,dword [rbp+rdx*4+262144]
	pextrw rdx,xmm2,2
	or eax,dword [rbp+rdx*4+524288]
	pextrw rdx,xmm2,3
	or eax,dword [rbp+rdx*4+786432]
	movnti dword [rsi],eax

	pextrw rdx,xmm2,4
	mov eax,dword [rbp+rdx*4]
	pextrw rdx,xmm2,5
	or eax,dword [rbp+rdx*4+262144]
	pextrw rdx,xmm2,6
	or eax,dword [rbp+rdx*4+524288]
	pextrw rdx,xmm2,7
	or eax,dword [rbp+rdx*4+786432]
	movnti dword [rsi+4],eax
	lea rdi,[rdi+16]
	lea rsi,[rsi+8]
	dec r11
	jnz vf6_1lop5

	mov eax,dword [rbx]
	movdqu xmm2,xmm8 ;tmpV
	mov edx,dword [rbx+4]
	movq xmm0,[rdi+rax]
	movq xmm1,[rdi+rdx]
	punpcklwd xmm0, xmm1
	mov eax,dword [rbx+8]
	pmaddwd xmm0,xmm5
	mov edx,dword [rbx+12]
	paddd xmm2,xmm0
	movdqu xmm0,[rdi+rax]
	movdqu xmm1,[rdi+rdx]
	punpcklwd xmm0, xmm1
	mov eax,dword [rbx+16]
	pmaddwd xmm0,xmm6
	mov edx,dword [rbx+20]
	paddd xmm2,xmm0
	movdqu xmm0,[rdi+rax]
	movdqu xmm1,[rdi+rdx]
	punpcklwd xmm0, xmm1
	pmaddwd xmm0,xmm7
	paddd xmm2,xmm0

	psrad xmm2,15
	packssdw xmm2,xmm2
	pextrw rdx,xmm2,0
	mov eax,dword [rbp+rdx*4]
	pextrw rdx,xmm2,1
	or eax,dword [rbp+rdx*4+262144]
	pextrw rdx,xmm2,2
	or eax,dword [rbp+rdx*4+524288]
	pextrw rdx,xmm2,3
	or eax,dword [rbp+rdx*4+786432]
	movnti dword [rsi],eax
	lea rsi,[rsi+4]

	add rbx,80
	add rsi,qword [rsp+56] ;dAdd

	dec rcx ;currHeight
	jnz vf6_1lop4
	jmp vfexit

	align 16
vf_1start:
	shr r8,1 ;tap
	movdqu xmm7,xmm8 ;tmpV
	pxor xmm3,xmm3
	mov r10,rdx ;width
	align 16
vflop:
	mov r11,r10 ;width

	align 16
vflop2:
	mov rbp,qword [rsp+40] ;weight
	mov rbx,r9 ;index
	mov rdx,r8 ;tap

	movdqa xmm1,xmm7
	ALIGN 16
vflop3:
	mov rax,qword [rbx]
	movq xmm0,[rdi+rax]
	mov rax,qword [rbx+8]
	movq xmm2,[rdi+rax]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[rbp]
	paddd xmm1,xmm0
	lea rbx,[rbx+16]
	lea rbp,[rbp+16]
	dec rdx
	jnz vflop3

	mov rbp,qword [rsp+64] ;rgbTable
	psrad xmm1,15
	packssdw xmm1,xmm3
	pextrw rdx,xmm1,0
	mov eax,dword [rbp+rdx*4]
	pextrw rdx,xmm1,1
	or eax,dword [rbp+rdx*4+262144]
	pextrw rdx,xmm1,2
	or eax,dword [rbp+rdx*4+524288]
	pextrw rdx,xmm1,3
	or eax,dword [rbp+rdx*4+786432]

	mov dword [rsi],eax
	lea rdi,[rdi+8]
	lea rsi,[rsi+4]
	dec r11
	jnz vflop2

	mov rdx,r8 ;tap
	shl rdx,4
	add r9,rdx ;index
	add qword [rsp+40],rdx ;weight
	add rsi,qword [rsp+56] ;dAdd

	dec rcx ;currHeight
	jnz vflop
	jmp vfexit

	align 16
vfstart2:
	cmp r8,6 ;tap
	jz vf6start
	cmp r8,8 ;tap
	jz vf8start
	jmp vfstart

	align 16
vf6start:							; else if (tap == 6)
	shr rdx,1 ;width
	mov rbp,qword [rsp+64] ;rgbTable
	mov rbx,qword [rsp+40] ;weight
	mov r10,rdx ;width
	xor rax,rax
	ALIGN 16
vf6lop4:

	movdqa xmm5,[rbx+32]
	movdqa xmm6,[rbx+48]
	movdqa xmm7,[rbx+64]
	mov r11,r10 ;width
	ALIGN 16
vf6lop5:
	mov eax,dword [rbx]
	movdqu xmm2,xmm8 ;tmpV
	mov edx,dword [rbx+4]
	movdqa xmm3,xmm2
	movdqu xmm0,[rdi+rax]
	movdqa xmm4,xmm0
	movdqu xmm1,[rdi+rdx]
	punpcklwd xmm0, xmm1
	punpckhwd xmm4, xmm1
	mov eax,dword [rbx+8]
	pmaddwd xmm0,xmm5
	pmaddwd xmm4,xmm5
	mov edx,dword [rbx+12]
	paddd xmm2,xmm0
	paddd xmm3,xmm4
	movdqu xmm0,[rdi+rax]
	movdqa xmm4,xmm0
	movdqu xmm1,[rdi+rdx]
	punpcklwd xmm0,xmm1
	punpckhwd xmm4,xmm1
	mov eax,dword [rbx+16]
	pmaddwd xmm0,xmm6
	pmaddwd xmm4,xmm6
	mov edx,dword [rbx+20]
	paddd xmm2,xmm0
	paddd xmm3,xmm4
	movdqu xmm0,[rdi+rax]
	movdqa xmm4,xmm0
	movdqu xmm1,[rdi+rdx]
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
	mov eax,dword [rbp+rdx*4]
	pextrw rdx,xmm2,1
	or eax,dword [rbp+rdx*4+262144]
	pextrw rdx,xmm2,2
	or eax,dword [rbp+rdx*4+524288]
	pextrw rdx,xmm2,3
	or eax,dword [rbp+rdx*4+786432]
	movnti dword [rsi],eax

	pextrw rdx,xmm2,4
	mov eax,dword [rbp+rdx*4]
	pextrw rdx,xmm2,5
	or eax,dword [rbp+rdx*4+262144]
	pextrw rdx,xmm2,6
	or eax,dword [rbp+rdx*4+524288]
	pextrw rdx,xmm2,7
	or eax,dword [rbp+rdx*4+786432]
	movnti dword [rsi+4],eax
	lea rdi,[rdi+16]
	lea rsi,[rsi+8]
	dec r11
	jnz vf6lop5

	add rbx,80
	add rsi,qword [rsp+56] ;dAdd

	dec rcx ;currHeight
	jnz vf6lop4
	jmp vfexit
	
	align 16
vf8start:							;else if (tap == 8)
	shr rdx,1 ;width
	mov rbx,r9 ;index
	mov r10,rdx ;width
	mov r8,qword [rsp+40] ;weight
	mov rbp,qword [rsp+64] ;rgbTable
	ALIGN 16
vf8lop4:
	movdqa xmm5,[r8]
	punpcklqdq xmm5,[r8+16]
	movdqa xmm6,[r8+32]
	movdqa xmm7,[r8+48]

	mov r11,r10 ;width
	ALIGN 16
vf8lop5:
	mov rax,qword [rbx]
	mov rdx,qword [rbx+8]
	movdqu xmm2,xmm8 ;tmpV
	movdqa xmm3,xmm2
	movdqu xmm0,[rdi+rax]
	movdqu xmm1,[rdi+rdx]
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
	movdqu xmm0,[rdi+rax]
	movdqu xmm1,[rdi+rdx]
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
	movdqu xmm0,[rdi+rax]
	movdqu xmm1,[rdi+rdx]
	movdqa xmm4,xmm0
	punpcklwd xmm0, xmm1
	punpckhwd xmm4, xmm1
	pmaddwd xmm0,xmm6
	pmaddwd xmm4,xmm6
	mov rax,qword [rbx+48]
	mov rdx,qword [rbx+56]
	paddd xmm2,xmm0
	paddd xmm3,xmm4
	movdqu xmm0,[rdi+rax]
	movdqu xmm1,[rdi+rdx]
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
	mov eax,dword [rbp+rdx*4]
	pextrw rdx,xmm2,1
	or eax,dword [rbp+rdx*4+262144]
	pextrw rdx,xmm2,2
	or eax,dword [rbp+rdx*4+524288]
	pextrw rdx,xmm2,3
	or eax,dword [rbp+rdx*4+786432]
	movnti dword [rsi],eax

	pextrw rdx,xmm2,4
	mov eax,dword [rbp+rdx*4]
	pextrw rdx,xmm2,5
	or eax,dword [rbp+rdx*4+262144]
	pextrw rdx,xmm2,6
	or eax,dword [rbp+rdx*4+524288]
	pextrw rdx,xmm2,7
	or eax,dword [rbp+rdx*4+786432]
	movnti dword [rsi+4],eax
	lea rdi,[rdi+16]
	lea rsi,[rsi+8]
	dec r11
	jnz vf8lop5

	add rbx,64 ;index
	add r8,64 ;weight
	add rsi,qword [rsp+56] ;dAdd

	dec rcx ;currHeight
	jnz vf8lop4
	jmp vfexit
	
	align 16
vfstart:
	shr r8,1 ;tap
	shr rdx,1 ;width
	mov qword [rsp+48],rdi
	movdqu xmm7,xmm8 ;tmpV
	pxor xmm3,xmm3
	mov r10,rdx ;width
	align 16
vflop4:
	mov rdi,qword [rsp+48]
	mov r11,r10 ;width

	align 16
vflop5:
	mov rbp,qword [rsp+40] ;weight
	mov rbx,r9 ;index
	mov rdx,r8 ;tap
	movdqa xmm2,xmm7
	movdqa xmm6,xmm7
	ALIGN 16
vflop6:
	mov rax,qword [rbx]
	movdqu xmm0,[rdi+rax]
	mov rax,qword [rbx+8]
	movdqu xmm4,xmm0
	movdqu xmm1,[rdi+rax]
	punpcklwd xmm0, xmm1
	punpckhwd xmm4, xmm1
	movdqa xmm5,[rbp]
	pmaddwd xmm0,xmm5
	pmaddwd xmm4,xmm5
	paddd xmm2,xmm0
	paddd xmm6,xmm4
	lea rbp,[rbp+16]
	lea rbx,[rbx+16]

	dec rdx
	jnz vflop6

	mov rbp,qword [rsp+64] ;rgbTable

	psrad xmm6,15
	packssdw xmm6,xmm3
	pextrw rdx,xmm6,0
	mov eax,dword [rbp+rdx*4]
	pextrw rdx,xmm6,1
	or eax,dword [rbp+rdx*4+262144]
	pextrw rdx,xmm6,2
	or eax,dword [rbp+rdx*4+524288]
	pextrw rdx,xmm6,3
	or eax,dword [rbp+rdx*4+786432]
	movd xmm0,eax

	psrad xmm2,15
	packssdw xmm2,xmm3
	pextrw rdx,xmm2,0
	mov eax,dword [rbp+rdx*4]
	pextrw rdx,xmm2,1
	or eax,dword [rbp+rdx*4+262144]
	pextrw rdx,xmm2,2
	or eax,dword [rbp+rdx*4+524288]
	pextrw rdx,xmm2,3
	or eax,dword [rbp+rdx*4+786432]

	mov dword [rsi],eax
	movd dword [rsi+4],xmm0
	lea rdi,[rdi+16]
	lea rsi,[rsi+8]
	dec r11
	jnz vflop5

	mov rax,r8 ;tap
	shl rax,4
	add r9,rax ;index
	add qword [rsp+40],rax ;weight
	add rsi,qword [rsp+56] ;dAdd

	dec rcx ;currHeight
	jnz vflop4

	align 16
vfexit:
	movdqu xmm8,[rsp]
	add rsp,16
	pop rbx
	pop rbp
	ret

;void LanczosResizerLR_C32_CPU_vertical_filter_na(UInt8 *inPt, UInt8 *outPt, OSInt width, OSInt height, OSInt tap, OSInt *index, Int64 *weight, OSInt sstep, OSInt dstep, UInt8 *rgbTable)
;0 r15
;8 r14
;16 r13
;24 r12
;32 xmm8
;48 rbx
;26 rbp
;64 retAddr
;rdi inPt
;rsi outPt
;rdx width
;rcx height / currHeight
;r8 tap
;r9 index
;72 weight
;80 sstep
;88 dstep / dAdd
;96 rgbTable

	align 16
LanczosResizerLR_C32_CPU_vertical_filter_na:
_LanczosResizerLR_C32_CPU_vertical_filter_na:
	mov eax,0x4000
	push rbp
	push rbx
	sub rsp,16
	movdqu [rsp],xmm8
	push r12
	push r13
	push r14
	push r15
	movd xmm0,eax
	punpckldq xmm0,xmm0
	punpckldq xmm0,xmm0
	movdqa xmm8,xmm0						;Int32 tmpV[4] = {16384, 16384, 16384, 16384};
	lea rax,[rdx*4] ;width
	sub qword [rsp+88],rax ;dAdd			;OSInt dAdd = dstep - width * 4;
	test rdx,1						;if (width & 1)
	jz vfnastart2
	cmp r8,6 ;tap
	jz vf6na_1start
	jmp vfna_1start

	align 16
vf6na_1start:						;if (tap == 6)
	shr rdx,1 ;width
	mov r8,qword [rsp+96] ;rgbTable
	mov rbx,qword [rsp+72] ;weight
	ALIGN 16
vf6na_1lop4:

	movdqa xmm5,[rbx+32]
	movdqa xmm6,[rbx+48]
	movdqa xmm7,[rbx+64]

	mov r10,rdi ;inPt
	mov r11,rdx ;width

	xor rax,rax
	xor rbp,rbp
	ALIGN 16
vf6na_1lop5:
	mov eax,dword [rbx]
	movdqu xmm2,xmm8 ;tmpV
	mov ebp,dword [rbx+4]
	movdqa xmm3,xmm2
	movdqu xmm0,[r10+rax]
	movdqa xmm4,xmm0
	movdqu xmm1,[r10+rbp]
	punpcklwd xmm0, xmm1
	punpckhwd xmm4, xmm1
	mov eax,dword [rbx+8]
	pmaddwd xmm0,xmm5
	pmaddwd xmm4,xmm5
	mov ebp,dword [rbx+12]
	paddd xmm2,xmm0
	paddd xmm3,xmm4
	movdqu xmm0,[r10+rax]
	movdqa xmm4,xmm0
	movdqu xmm1,[r10+rbp]
	punpcklwd xmm0, xmm1
	punpckhwd xmm4, xmm1
	mov eax,dword [rbx+16]
	pmaddwd xmm0,xmm6
	pmaddwd xmm4,xmm6
	mov ebp,dword [rbx+20]
	paddd xmm2,xmm0
	paddd xmm3,xmm4
	movdqu xmm0,[r10+rax]
	movdqa xmm4,xmm0
	movdqu xmm1,[r10+rbp]
	punpcklwd xmm0, xmm1
	punpckhwd xmm4, xmm1
	pmaddwd xmm0,xmm7
	pmaddwd xmm4,xmm7
	paddd xmm2,xmm0
	paddd xmm3,xmm4

	psrad xmm3,15
	psrad xmm2,15
	packssdw xmm2,xmm3
	pextrw rbp,xmm2,0
	mov eax,dword [r8+rbp*4]
	pextrw rbp,xmm2,1
	or eax,dword [r8+rbp*4+262144]
	pextrw rbp,xmm2,2
	or eax,dword [r8+rbp*4+524288]
	mov dword [rsi],eax

	pextrw rbp,xmm2,4
	mov eax,dword [r8+rbp*4]
	pextrw rbp,xmm2,5
	or eax,dword [r8+rbp*4+262144]
	pextrw rbp,xmm2,6
	or eax,dword [r8+rbp*4+524288]
	mov dword [rsi+4],eax
	lea r10,[r10+16]
	lea rsi,[rsi+8]
	dec r11
	jnz vf6na_1lop5

	mov eax,dword [rbx]
	movdqu xmm2,xmm8 ;tmpV
	mov ebp,dword [rbx+4]
	movq xmm0,[r10+rax]
	movq xmm1,[r10+rbp]
	punpcklwd xmm0, xmm1
	mov eax,dword [rbx+8]
	pmaddwd xmm0,xmm5
	mov ebp,dword [rbx+12]
	paddd xmm2,xmm0
	movdqu xmm0,[r10+rax]
	movdqu xmm1,[r10+rbp]
	punpcklwd xmm0, xmm1
	mov eax,dword [rbx+16]
	pmaddwd xmm0,xmm6
	mov ebp,dword [rbx+20]
	paddd xmm2,xmm0
	movdqu xmm0,[r10+rax]
	movdqu xmm1,[r10+rbp]
	punpcklwd xmm0, xmm1
	pmaddwd xmm0,xmm7
	paddd xmm2,xmm0

	psrad xmm2,15
	packssdw xmm2,xmm2
	pextrw rbp,xmm2,0
	mov eax,dword [r8+rbp*4]
	pextrw rbp,xmm2,1
	or eax,dword [r8+rbp*4+262144]
	pextrw rbp,xmm2,2
	or eax,dword [r8+rbp*4+524288]
	mov dword [rsi],eax
	lea rsi,[rsi+4]

	add rbx,80
	add rsi,qword [rsp+88] ;dAdd

	dec rcx ;currHeight
	jnz vf6na_1lop4
	jmp vfnaexit

	align 16
vfna_1start:
	shr r8,1 ;tap
	pxor xmm3,xmm3
	mov qword [rsp+80],rcx
	align 16
vfnalop:
	mov rcx,rdi ;inPt
	mov rbp,rdx ;width

	align 16
vfnalop2:
	mov rbx,r9 ;index
	mov r11,qword [rsp+72] ;weight
	mov r10,r8 ;tap

	pxor xmm1,xmm1
	ALIGN 16
vfnalop3:
	mov rax,qword [rbx]
	movq xmm0,[rcx+rax]
	mov rax,qword [rbx+8]
	movq xmm2,[rcx+rax]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[r11]
	paddd xmm1,xmm0
	lea rbx,[rbx+16]
	lea r11,[r11+16]
	dec r10
	jnz vfnalop3

	psrad xmm1,15
	packssdw xmm1,xmm3
	mov r11,qword [rsp+96] ;rgbTable

	pextrw rbx,xmm1,0
	mov eax,dword [r11+rbx*4]
	pextrw rbx,xmm1,1
	or eax,dword [r11+rbx*4+262144]
	pextrw rbx,xmm1,2
	or eax,dword [r11+rbx*4+524288]

	mov dword [rsi],eax
	lea rcx,[rcx+8]
	lea rsi,[rsi+4]
	dec rbp
	jnz vfnalop2

	mov rax,r8 ;tap
	shl rax,4
	add r9,rax ;index
	add qword [rsp+72],rax ;weight

	add rsi,qword [rsp+88] ;dAdd

	dec qword [rsp+80] ;currHeight
	jnz vfnalop
	jmp vfnaexit


	align 16
vfnastart2:
	cmp r8,6 ;tap
	jz vf6nastart
	cmp r8,8 ;tap
	jz vf8nastart
	cmp r8,12 ;tap
	jz vf12nastart
	jmp vfnastart

	align 16
vf6nastart:						; else if (tap == 6)
	test rdx,3 ;width
	jnz vf6nastart2
	cmp dword [rel _UseAVX],0
	jnz vf6nastartavx
	align 16
vf6nastart2:
	xor r12,r12
	xor r13,r13
	xor r14,r14
	xor r15,r15
	mov qword [rsp+80],rcx ;height
	shr rdx,1 ;width
	mov rbx,qword [rsp+72] ;weight
	mov r10,qword [rsp+96] ;rgbTable
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

	mov r8,rdi ;inPt
	mov rbp,rdx ;width
	ALIGN 16
vf6nalop5:
	movdqa xmm2,xmm8 ;tmpV
	pxor xmm3,xmm3
	movdqu xmm0,[r8+r12]
	movdqa xmm4,xmm0
	movdqu xmm1,[r8+r13]
	punpcklwd xmm0, xmm1
	punpckhwd xmm4, xmm1
	pmaddwd xmm0,xmm5
	pmaddwd xmm4,xmm5
	paddd xmm2,xmm0
	movdqu xmm0,[r8+r14]
	paddd xmm3,xmm4
	movdqu xmm1,[r8+r15]
	movdqa xmm4,xmm0
	punpcklwd xmm0, xmm1
	punpckhwd xmm4, xmm1
	pmaddwd xmm0,xmm6
	pmaddwd xmm4,xmm6
	paddd xmm2,xmm0
	movdqu xmm0,[r8+r11]
	paddd xmm3,xmm4
	movdqu xmm1,[r8+r9]
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
	pextrw rcx,xmm2,0
	mov eax,dword [r10+rcx*4]
	pextrw rcx,xmm2,1
	or eax,dword [r10+rcx*4+262144]
	pextrw rcx,xmm2,2
	or eax,dword [r10+rcx*4+524288]
	mov dword [rsi],eax

	pextrw rcx,xmm2,4
	mov eax,dword [r10+rcx*4]
	pextrw rcx,xmm2,5
	or eax,dword [r10+rcx*4+262144]
	pextrw rcx,xmm2,6
	or eax,dword [r10+rcx*4+524288]
	mov dword [rsi+4],eax

	lea rsi,[rsi+8]
	lea r8,[r8+16]
	dec rbp
	jnz vf6nalop5

	lea rbx,[rbx+80]
	add rsi,qword [rsp+88] ;dAdd

	dec qword [rsp+80] ;r9 ;currHeight
	jnz vf6nalop4
	jmp vfnaexit

	align 16
vf6nastartavx:
	xor r12,r12
	xor r13,r13
	xor r14,r14
	xor r15,r15
	mov qword [rsp+80],rcx ;height
	shr rdx,2 ;width
	mov rbx,qword [rsp+72] ;weight
	mov r10,qword [rsp+96] ;rgbTable
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

	mov r8,rdi ;inPt
	mov rbp,rdx ;width
	ALIGN 16
vf6naavxlop5:
	vmovdqa ymm2,ymm8 ;tmpV
	vpxor ymm3,ymm3,ymm3
	vmovdqu ymm0,[r8+r12]
	vmovdqa ymm4,ymm0
	vmovdqu ymm1,[r8+r13]
	vpunpcklwd ymm0,ymm0,ymm1
	vpunpckhwd ymm4,ymm4,ymm1
	vpmaddwd ymm0,ymm0,ymm5
	vpmaddwd ymm4,ymm4,ymm5
	vpaddd ymm2,ymm2,ymm0
	vmovdqu ymm0,[r8+r14]
	vpaddd ymm3,ymm3,ymm4
	vmovdqu ymm1,[r8+r15]
	vmovdqa ymm4,ymm0
	vpunpcklwd ymm0,ymm0,ymm1
	vpunpckhwd ymm4,ymm4,ymm1
	vpmaddwd ymm0,ymm0,ymm6
	vpmaddwd ymm4,ymm4,ymm6
	vpaddd ymm2,ymm2,ymm0
	vmovdqu ymm0,[r8+r11]
	vpaddd ymm3,ymm3,ymm4
	vmovdqu ymm1,[r8+r9]
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
	vpextrw rcx,xmm2,0
	vextractf128 xmm0,ymm2,1
	mov eax,dword [r10+rcx*4]
	vpextrw rcx,xmm2,1
	or eax,dword [r10+rcx*4+262144]
	vpextrw rcx,xmm2,2
	or eax,dword [r10+rcx*4+524288]
	mov dword [rsi],eax

	vpextrw rcx,xmm2,4
	mov eax,dword [r10+rcx*4]
	vpextrw rcx,xmm2,5
	or eax,dword [r10+rcx*4+262144]
	vpextrw rcx,xmm2,6
	or eax,dword [r10+rcx*4+524288]
	mov dword [rsi+4],eax

	vpextrw rcx,xmm0,0
	mov eax,dword [r10+rcx*4]
	vpextrw rcx,xmm0,1
	or eax,dword [r10+rcx*4+262144]
	vpextrw rcx,xmm0,2
	or eax,dword [r10+rcx*4+524288]
	mov dword [rsi+8],eax

	vpextrw rcx,xmm0,4
	mov eax,dword [r10+rcx*4]
	vpextrw rcx,xmm0,5
	or eax,dword [r10+rcx*4+262144]
	vpextrw rcx,xmm0,6
	or eax,dword [r10+rcx*4+524288]
	mov dword [rsi+12],eax

	lea rsi,[rsi+16]
	lea r8,[r8+32]
	dec rbp
	jnz vf6naavxlop5

	lea rbx,[rbx+80]
	add rsi,qword [rsp+88] ;dAdd

	dec qword [rsp+80] ;r9 ;currHeight
	jnz vf6naavxlop4
	vzeroupper
	jmp vfnaexit

	align 16
vf8nastart:						;else if (tap == 8)
	shr rdx,1 ;width
	push rdi ;inPt
	push rdx ;width
	mov qword [rsp+80+16],rcx ;height
	mov rcx,rsi ;outPt
	mov rbx,r9 ;index
	mov rdi,qword [rsp+96+16] ;rgbTable
	ALIGN 16
vf8nalop4:

	mov rbp,qword [rsp+72+16] ;weight
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
	mov qword [rsp+72+16],rbp ;weight

	mov rsi,qword [rsp+8] ;inPt
	mov rbp,qword [rsp] ;width
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
	add rcx,qword [rsp+88+16] ;dAdd

	dec qword [rsp+80+16] ;currHeight
	jnz vf8nalop4
	pop rdx
	pop rdi
	jmp vfnaexit
	
	align 16
vf12nastart:						;else if (tap == 12)
	shr rdx,1 ;width
	mov rbx,qword [rsp+96] ;rgbTable
	mov r8,rdx
	ALIGN 16
vf12nalop4:

	mov rbp,qword [rsp+72] ;weight
	movdqa xmm5,[rbp]
	movdqa xmm6,[rbp+16]
	movdqa xmm7,[rbp+32]
	movdqa xmm9,[rbp+48]
	movdqa xmm10,[rbp+64]
	movdqa xmm11,[rbp+80]
	add rbp,96
	mov qword [rsp+72],rbp ;weight

	mov r10,rdi ;inPt
	mov rbp,r8 ;width
	ALIGN 16
vf12nalop5:
	mov rax,qword [r9]
	movdqa xmm2,xmm8 ;tmpV
	mov rdx,qword [r9+8]
	pxor xmm3,xmm3
	movdqu xmm0,[r10+rax]
	movdqa xmm4,xmm0
	movdqu xmm1,[r10+rdx]
	punpcklwd xmm0, xmm1
	punpckhwd xmm4, xmm1
	mov rax,qword [r9+16]
	pmaddwd xmm0,xmm5
	pmaddwd xmm4,xmm5
	mov rdx,qword [r9+24]
	paddd xmm2,xmm0
	movdqu xmm0,[r10+rax]
	paddd xmm3,xmm4
	movdqu xmm1,[r10+rdx]
	movdqa xmm4,xmm0
	punpcklwd xmm0, xmm1
	punpckhwd xmm4, xmm1
	mov rax,qword [r9+32]
	pmaddwd xmm0,xmm6
	pmaddwd xmm4,xmm6
	mov rdx,qword [r9+40]
	paddd xmm2,xmm0
	movdqu xmm0,[r10+rax]
	paddd xmm3,xmm4
	movdqu xmm1,[r10+rdx]
	movdqa xmm4,xmm0
	punpcklwd xmm0, xmm1
	punpckhwd xmm4, xmm1
	mov rax,qword [r9+48]
	pmaddwd xmm0,xmm7
	pmaddwd xmm4,xmm7
	mov rdx,qword [r9+56]
	paddd xmm2,xmm0
	movdqu xmm0,[r10+rax]
	paddd xmm3,xmm4
	movdqu xmm1,[r10+rdx]
	movdqa xmm4,xmm0
	punpcklwd xmm0, xmm1
	punpckhwd xmm4, xmm1
	mov rax,qword [r9+64]
	pmaddwd xmm0,xmm9
	pmaddwd xmm4,xmm9
	mov rdx,qword [r9+72]
	paddd xmm2,xmm0
	movdqu xmm0,[r10+rax]
	paddd xmm3,xmm4
	movdqu xmm1,[r10+rdx]
	movdqa xmm4,xmm0
	punpcklwd xmm0, xmm1
	punpckhwd xmm4, xmm1
	mov rax,qword [r9+80]
	pmaddwd xmm0,xmm10
	pmaddwd xmm4,xmm10
	mov rdx,qword [r9+88]
	paddd xmm2,xmm0
	movdqu xmm0,[r10+rax]
	paddd xmm3,xmm4
	movdqu xmm1,[r10+rdx]
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
	mov eax,dword [rbx+rdx*4]
	pextrw rdx,xmm2,1
	or eax,dword [rbx+rdx*4+262144]
	pextrw rdx,xmm2,2
	or eax,dword [rbx+rdx*4+524288]
	movnti dword [rsi],eax

	pextrw rdx,xmm2,4
	mov eax,dword [rbx+rdx*4]
	pextrw rdx,xmm2,5
	or eax,dword [rbx+rdx*4+262144]
	pextrw rdx,xmm2,6
	or eax,dword [rbx+rdx*4+524288]
	movnti dword [rsi+4],eax

	lea rsi,[rsi+8]
	lea r10,[r10+16]
	dec rbp
	jnz vf12nalop5

	lea r9,[r9+96]
	add rsi,qword [rsp+88] ;dAdd

	dec rcx ;currHeight
	jnz vf12nalop4
	jmp vfnaexit

	align 16
vfnastart:
	shr r8,1 ;tap
	shr rdx,1 ;width
	pxor xmm3,xmm3
	mov qword [rsp+80],rcx ;height

	align 16
vfnalop4:
	mov r10,rdi ;inPt
	mov rbp,rdx ;width

	align 16
vfnalop5:
	mov r11,qword [rsp+72] ;weight
	mov rbx,r9 ;index
	mov rcx,r8 ;tap

	pxor xmm2,xmm2
	pxor xmm6,xmm6
	ALIGN 16
vfnalop6:
	mov rax,qword [rbx]
	movdqu xmm0,[r10+rax]
	mov rax,qword [rbx+8]
	movdqa xmm4,xmm0
	movdqu xmm1,[r10+rax]
	punpcklwd xmm0, xmm1
	punpckhwd xmm4, xmm1
	movdqa xmm5,[r11]
	pmaddwd xmm0,xmm5
	pmaddwd xmm4,xmm5
	paddd xmm2,xmm0
	paddd xmm6,xmm4
	lea r11,[r11+16]
	lea rbx,[rbx+16]

	dec rcx
	jnz vfnalop6

	mov r11,qword [rsp+96] ;rgbTable

	psrad xmm6,15
	packssdw xmm6,xmm3
	pextrw rcx,xmm6,0
	mov eax,dword [r11+rcx*4]
	pextrw rcx,xmm6,1
	or eax,dword [r11+rcx*4+262144]
	pextrw rcx,xmm6,2
	or eax,dword [r11+rcx*4+524288]
	movd xmm0,eax

	psrad xmm2,15
	packssdw xmm2,xmm3
	pextrw rcx,xmm2,0
	mov eax,dword [r11+rcx*4]
	pextrw rcx,xmm2,1
	or eax,dword [r11+rcx*4+262144]
	pextrw rcx,xmm2,2
	or eax,dword [r11+rcx*4+524288]

	mov dword [rsi],eax
	movd dword [rsi+4],xmm0
	lea r10,[r10+16]
	lea rsi,[rsi+8]
	dec rbp
	jnz vfnalop5

	mov rax,r8 ;tap
	shl rax,4
	add r9,rax ;index
	add qword [rsp+72],rax ;weight

	add rsi,qword [rsp+88] ;dAdd

	dec qword [rsp+80] ;currHeight
	jnz vfnalop4

	align 16
vfnaexit:
	pop r15
	pop r14
	pop r13
	pop r12
	movdqu xmm8,[rsp]
	add rsp,16
	pop rbx
	pop rbp
	ret

;void LanczosResizerLR_C32_CPU_hv_filter(UInt8 *inPt, UInt8 *outPt, OSInt dwidth, OSInt dheight, OSInt swidth, OSInt htap, OSInt *hindex, Int64 *hweight, OSInt vtap, OSInt *vindex, Int64 *vweight, OSInt sstep, OSInt dstep, UInt8 *rgbTable, UInt8 *buffPt)
;0 ebx
;8 ebp
;16 retAddr
;rdi inPt
;rsi outPt
;rdx dwidth
;rcx dheight / currHeight
;r8 swidth
;r9 htap
;24 hindex
;32 hweight
;40 vtap
;48 vindex
;56 vweight
;64 sstep
;72 dstep / dAdd
;80 rgbTable
;88 buffPt

	align 16
LanczosResizerLR_C32_CPU_hv_filter:
_LanczosResizerLR_C32_CPU_hv_filter:
	push rbp
	push rbx
	cmp r9,6 ;htap
	jnz hvfexit
	cmp qword [rsp+40],6 ;vtap
	jnz hvfexit

	shr r8,1 ;swidth
	shr rdx,1 ;dwidth
	mov qword [rsp+40],rdx
	align 16
hvf6lop1:
	mov rbp,qword [rsp+56] ;vweight
	movdqa xmm5,[rbp]
	movdqa xmm6,[rbp+16]
	movdqa xmm7,[rbp+32]

	mov rbx,qword [rsp+48] ;vindex
	mov r9,rsi ;inPt
	mov r10,qword [rsp+88] ;buffPt

	mov r11,r8 ;swidth
	ALIGN 16
hvf6lop2:
	pxor xmm2,xmm2
	pxor xmm3,xmm3
	mov rax,qword [rbx]
	mov rbp,qword [rbx+8]
	movdqu xmm0,[r9+rax]
	movdqu xmm1,[r9+rbp]
	movdqu xmm4,xmm0
	punpcklwd xmm0, xmm1
	punpckhwd xmm4, xmm1
	pmaddwd xmm0,xmm5
	pmaddwd xmm4,xmm5
	paddd xmm2,xmm0
	paddd xmm3,xmm4
	mov rax,qword [rbx+16]
	mov rbp,qword [rbx+24]
	movdqu xmm0,[r9+rax]
	movdqu xmm1,[r9+rbp]
	movdqu xmm4,xmm0
	punpcklwd xmm0, xmm1
	punpckhwd xmm4, xmm1
	pmaddwd xmm0,xmm6
	pmaddwd xmm4,xmm6
	paddd xmm2,xmm0
	paddd xmm3,xmm4
	mov rax,qword [rbx+32]
	mov rbp,qword [rbx+40]
	movdqu xmm0,[r9+rax]
	movdqu xmm1,[r9+rbp]
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
	movdqa [r10],xmm2

	add r10,16
	add r9,16
	dec r11
	jnz hvf6lop2

	add qword [rsp+48],48 ;vindex
	add qword [rsp+56],48 ;vweight

	mov r9,qword [rsp+40] ;dwidth
	mov r10,rsi ;outPt

	mov r11,qword [rsp+32] ;hweight
	mov rbx,qword [rsp+24] ;hindex
	ALIGN 16
hvf6lop3:
	mov rbp,qword [rsp+88] ;buffPt
	pxor xmm1,xmm1
	mov rax,qword [rbx]
	movq xmm0,[rbp+rax]
	mov rax,qword [rbx+8]
	movq xmm2,[rbp+rax]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[r11]
	paddd xmm1,xmm0
	mov rax,qword [rbx+16]
	movq xmm0,[rbp+rax]
	mov rax,qword [rbx+24]
	movq xmm2,[rbp+rax]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[r11+16]
	paddd xmm1,xmm0
	mov rax,qword [rbx+32]
	movq xmm0,[rbp+rax]
	mov rax,qword [rbx+40]
	movq xmm2,[rbp+rax]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[r11+32]
	paddd xmm1,xmm0
	add r11,48
	add rbx,48

	pxor xmm3,xmm3
	mov rax,qword [rbx]
	movq xmm0,[rbp+rax]
	mov rax,qword [rbx+8]
	movq xmm2,[rbp+rax]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[r11]
	paddd xmm3,xmm0
	mov rax,qword [rbx+16]
	movq xmm0,[rbp+rax]
	mov rax,qword [rbx+24]
	movq xmm2,[rbp+rax]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[r11+16]
	paddd xmm3,xmm0
	mov rax,qword [rbx+32]
	movq xmm0,[rbp+rax]
	mov rax,qword [rbx+40]
	movq xmm2,[rbp+rax]
	punpcklwd xmm0,xmm2
	pmaddwd xmm0,[r11+32]
	paddd xmm3,xmm0
	add r11,48
	add rbx,48

	psrad xmm1,15
	psrad xmm3,15
	packssdw xmm1,xmm3

	mov rbp,qword [rsp+80] ;rgbTable
	pextrw rdx,xmm1,0
	mov eax,dword [rbp+rdx*4]
	pextrw rdx,xmm1,1
	or eax,dword [rbp+rdx*4+262144]
	pextrw rdx,xmm1,2
	or eax,dword [rbp+rdx*4+524288]
	mov dword [r10],eax

	pextrw rdx,xmm1,4
	mov eax,dword [rbp+rdx*4]
	pextrw rdx,xmm1,5
	or eax,dword [rbp+rdx*4+262144]
	pextrw rdx,xmm1,6
	or eax,dword [rbp+rdx*4+524288]
	mov dword [r10+4],eax

	add r10,8
	dec r9 ;currWidth
	jnz hvf6lop3

	add r11,qword [rsp+144] ;dstep outPt

	dec rcx ;currHeight
	jnz hvf6lop1
	
	align 16
hvfexit:
	pop rbx
	pop rbp
	ret

;void LanczosResizerLR_C32_CPU_collapse(UInt8 *inPt, UInt8 *outPt, OSInt width, OSInt height, OSInt sstep, OSInt dstep, UInt8 *rgbTable)
;0 rbx
;8 rbp
;16 retAddr
;rdi inPt
;rsi outPt
;rdx width
;rcx height
;r8 sstep / sAdd
;r9 dstep / dAdd
;24 rgbTable

	align 16
LanczosResizerLR_C32_CPU_collapse:
_LanczosResizerLR_C32_CPU_collapse:
	push rbp
	push rbx
	lea rax,[rdx*8] ;width
	lea rbx,[rdx*4] ;width
	sub r8,rax ;sAdd		OSInt sAdd = sstep - width * 8;
	sub r9,rbx ;dAdd		OSInt dAdd = dstep - width * 4;
	test rdx,3 ;width
	jnz colstart
	test rsi,15 ;outPt
	jnz colstart
	test r9,15 ;dstep
	jz col16start
	align 16
colstart:						;if ((width & 3) || (((OSInt)outPt) & 15) || (dstep & 15))
	mov rbx,qword [rsp+24] ;rgbTable
	align 16
collop:
	mov r10,r8 ;width
	align 16
collop2:
	movzx rbp,word [rdi]
	mov eax,dword [rbx+rbp*4]
	movzx rbp,word [rdi+2]
	or eax,dword [rbx+rbp*4+262144]
	movzx rbp,word [rdi+4]
	or eax,dword [rbx+rbp*4+524288]
	movzx rbp,word [rdi+6]
	or eax,dword [rbx+rbp*4+786432]

	movnti dword [rsi],eax
	lea rdi,[rdi+8]
	lea rsi,[rsi+4]
	dec r10
	jnz collop2

	add rdi,r8 ;sAdd
	add rsi,r9 ;dAdd

	dec rcx ;height
	jnz collop
	jmp colexit

	align 16
col16start:
	shr rdx,2 ;width
	mov rbx,qword [rsp+24] ;rgbTable
	align 16
collop16:
	mov r10,r8 ;width

	align 16
collop16_2:
	movdqu xmm4,[rdi]
	movdqu xmm5,[rdi+16]
	pextrw rbp,xmm4,0
	mov eax,dword [rbx+rbp*4]
	pextrw rbp,xmm4,1
	or eax,dword [rbx+rbp*4+262144]
	pextrw rbp,xmm4,2
	or eax,dword [rbx+rbp*4+524288]
	pextrw rbp,xmm4,3
	or eax,dword [rbx+rbp*4+786432]
	movd xmm0,eax

	pextrw rbp,xmm4,4
	mov eax,dword [rbx+rbp*4]
	pextrw rbp,xmm4,5
	or eax,dword [rbx+rbp*4+262144]
	pextrw rbp,xmm4,6
	or eax,dword [rbx+rbp*4+524288]
	pextrw rbp,xmm4,7
	or eax,dword [rbx+rbp*4+786432]
	movd xmm1,eax
	punpckldq xmm0,xmm1

	pextrw rbp,xmm5,0
	mov eax,dword [rbx+rbp*4]
	pextrw rbp,xmm5,1
	or eax,dword [rbx+rbp*4+262144]
	pextrw rbp,xmm5,2
	or eax,dword [rbx+rbp*4+524288]
	pextrw rbp,xmm5,3
	or eax,dword [rbx+rbp*4+786432]
	movd xmm1,eax

	pextrw rbp,xmm5,4
	mov eax,dword [rbx+rbp*4]
	pextrw rbp,xmm5,5
	or eax,dword [rbx+rbp*4+262144]
	pextrw rbp,xmm5,6
	or eax,dword [rbx+rbp*4+524288]
	pextrw rbp,xmm5,7
	or eax,dword [rbx+rbp*4+786432]
	movd xmm2,eax
	punpckldq xmm1,xmm2
	punpcklqdq xmm0,xmm1

	movntdq [rsi],xmm0
	lea rdi,[rdi+32]
	lea rsi,[rsi+16]
	dec r10
	jnz collop16_2

	add rdi,r8 ;sAdd
	add rsi,r9 ;dAdd

	dec rcx ;height
	jnz collop16

	align 16
colexit:
	pop rbx
	pop rbp
	ret

;void LanczosResizerLR_C32_CPU_collapse_na(UInt8 *inPt, UInt8 *outPt, OSInt width, OSInt height, OSInt sstep, OSInt dstep, UInt8 *rgbTable)
;0 rbx
;8 rbp
;16 retAddr
;rdi inPt
;rsi outPt
;rdx width
;rcx height
;r8 sstep / sAdd
;r9 dstep / dAdd
;24 rgbTable

	align 16
LanczosResizerLR_C32_CPU_collapse_na:
_LanczosResizerLR_C32_CPU_collapse_na:
	push rbp
	push rbx
	lea rax,[rdx*8] ;width
	lea rbx,[rdx*4] ;width
	sub r8,rax ;sAdd		OSInt sAdd = sstep - width * 8;
	sub r9,rbx ;dAdd		OSInt dAdd = dstep - width * 4;
	test rdx,3 ;width
	jnz colnastart
	test rsi,15 ;outPt
	jnz colnastart
	test r9,15 ;dstep
	jz colna16start

	align 16
colnastart:					; if ((width & 3) || (((OSInt)outPt) & 15) || (dstep & 15))
	mov rbx,qword [rsp+24] ;rgbTable

	align 16
colnalop:
	mov r10,rdx ;width
	
	align 16
colnalop2:
	movzx rbp,word [rdi]
	mov eax,dword [rbx+rbp*4]
	movzx rbp,word [rdi+2]
	or eax,dword [rbx+rbp*4+262144]
	movzx rbp,word [rdi+4]
	or eax,dword [rbx+rbp*4+524288]

	mov dword [rsi],eax
	lea rdi,[rdi+8]
	lea rsi,[rsi+4]
	dec r10
	jnz colnalop2

	add rdi,r8 ;sAdd
	add rsi,r9 ;dAdd

	dec rcx ;height
	jnz colnalop
	jmp colnaexit
	
	align 16
colna16start:
	shr rdx,2 ;width
	mov rbx,qword [rsp+24] ;rgbTable
	align 16
colnalop16:
	mov r10,rdx ;width

	align 16
colnalop16_2:
	prefetcht0 [rdi+128]
	movdqu xmm4,[rdi]
	movdqu xmm5,[rdi+16]
	pextrw rbp,xmm4,0
	mov eax,dword [rbx+rbp*4]
	pextrw rbp,xmm4,1
	or eax,dword [rbx+rbp*4+262144]
	pextrw rbp,xmm4,2
	or eax,dword [rbx+rbp*4+524288]
	movd xmm0,eax

	pextrw rbp,xmm4,4
	mov eax,dword [rbx+rbp*4]
	pextrw rbp,xmm4,5
	or eax,dword [rbx+rbp*4+262144]
	pextrw rbp,xmm4,6
	or eax,dword [rbx+rbp*4+524288]
	movd xmm1,eax
	punpckldq xmm0,xmm1

	pextrw rbp,xmm5,0
	mov eax,dword [rbx+rbp*4]
	pextrw rbp,xmm5,1
	or eax,dword [rbx+rbp*4+262144]
	pextrw rbp,xmm5,2
	or eax,dword [rbx+rbp*4+524288]
	movd xmm1,eax

	pextrw rbp,xmm5,4
	mov eax,dword [rbx+rbp*4]
	pextrw rbp,xmm5,5
	or eax,dword [rbx+rbp*4+262144]
	pextrw rbp,xmm5,6
	or eax,dword [rbx+rbp*4+524288]
	movd xmm2,eax
	punpckldq xmm1,xmm2
	punpcklqdq xmm0,xmm1

	movntdq [rsi],xmm0
	lea rdi,[rdi+32]
	lea rsi,[rsi+16]
	dec r10
	jnz colnalop16_2

	add rdi,r8 ;sAdd
	add rsi,r9 ;dAdd

	dec rcx ;height
	jnz colnalop16

	align 16
colnaexit:
	pop rbx
	pop rbp
	ret
