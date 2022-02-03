section .text

global CSYUV420_LRGBC_VerticalFilterLRGB
global CSYUV420_LRGBC_do_yv12rgb8
global CSYUV420_LRGBC_do_yv12rgb2

;void CSYUV420_LRGBC_VerticalFilterLRGB(UInt8 *inYPt, UInt8 *inUPt, UInt8 *inVPt, UInt8 *outPt, OSInt width, OSInt height, OSInt tap, OSInt *index, Int64 *weight, Int32 isFirst, Int32 isLast, UInt8 *csLineBuff, UInt8 *csLineBuff2, OSInt ystep, OSInt dstep, Int64 *yuv2rgb, Int64 *rgbGammaCorr)
; xmm8 tmpV
; 48 cSub
; 56 sWidth
; 64 rdi
; 72 rsi
; 80 rbx
; 88 rbp
; 96 retAddr
; rcx inYPt r10
; rdx inUPt r11
; r8 inVPt
; r9 outPt
; 136 width
; 144 height / currHeight
; 152 tap
; 160 index
; 168 weight
; 176 isFirst
; 184 isLast
; 192 csLineBuff
; 200 csLineBuff2
; 208 ystep / yAdd
; 216 dstep
; 224 yuv2rgb
; 232 rgbGammaCorr

	align 16
CSYUV420_LRGBC_VerticalFilterLRGB:
	mov eax,32768
	push rbp
	push rbx
	push rsi
	push rdi
	sub rsp,64
	mov r10,rcx
	mov r11,rdx
	movd xmm0,eax
	punpckldq xmm0,xmm0
	punpckldq xmm0,xmm0
	movdqa xmm8,xmm0						;Int32 tmpV[4] = {32768, 32768, 32768, 32768};
	mov rdx,qword [rsp+136] ;width
	mov rax,rdx
	mov rcx,rdx
	sub qword [rsp+208],rdx ;yAdd				;OSInt yAdd = ystep - width;
	shr rax,1
	shr rcx,3
	sub rax,2
	mov qword [rsp+56],rcx ;sWidth				OSInt sWidth = width >> 3;
	mov qword [rsp+48],rax ;cSub				OSInt cSub = (width >> 1) - 2;
	mov rcx,qword [rsp+152] ;tap
	cmp rcx,4
	jnz vflexit									; if (tap == 4)
	and rdx,7
	jz vflstart
												; if (width & 7)
	shr rdx,2
	mov qword [rsp+24],rdx						; OSInt widthLeft = (width & 7) >> 2;

	align 16
vf7lop:
	mov rcx,r11 ;inUPt
	mov rdx,r8 ;inVPt
	mov rbx,qword [rsp+168] ;weight
	mov rsi,qword [rsp+200] ;csLineBuff2

	movdqa xmm5,[rbx+16]
	movdqa xmm6,[rbx+32]

	mov ebp,dword [rsp+56] ;sWidth
	xor rax,rax
	ALIGN 16
vf7lop2:
	mov eax,dword [rbx]
	movdqa xmm1,xmm8
	movd xmm4,[rcx+rax]
	pxor xmm7,xmm7
	movd xmm0,[rdx+rax]
	punpcklbw xmm4,xmm0
	mov eax,dword [rbx+4]
	punpcklbw xmm7,xmm4
	movd xmm4,[rcx+rax]
	movdqa xmm3,xmm8
	movd xmm2,[rdx+rax]
	punpcklbw xmm4,xmm2
	pxor xmm2,xmm2
	punpcklbw xmm2,xmm4
	movdqa xmm4,xmm7
	punpcklwd xmm7,xmm2
	punpckhwd xmm4,xmm2
	psrlw xmm7,1
	psrlw xmm4,1
	pmaddwd xmm7, xmm5
	pmaddwd xmm4, xmm5
	mov eax,dword [rbx+8]
	paddd xmm1,xmm7
	paddd xmm3,xmm4

	movd xmm0,[rdx+rax]
	pxor xmm7,xmm7
	movd xmm4,[rcx+rax]
	punpcklbw xmm4,xmm0
	mov eax,dword [rbx+12]
	punpcklbw xmm7,xmm4
	movd xmm4,[rcx+rax]
	pxor xmm2,xmm2
	movd xmm9,[rdx+rax]
	punpcklbw xmm4,xmm9
	punpcklbw xmm2,xmm4
	movdqa xmm4,xmm7
	punpcklwd xmm7,xmm2
	punpckhwd xmm4,xmm2
	psrlw xmm7,1
	psrlw xmm4,1
	pmaddwd xmm7, xmm6
	pmaddwd xmm4, xmm6
	paddd xmm1,xmm7
	paddd xmm3,xmm4

	psrad xmm1,16
	psrad xmm3,16
	packssdw xmm1,xmm3

	movdqu [rsi],xmm1
	add rsi,16
	add rcx,4
	add rdx,4
	dec rbp
	jnz vf7lop2

	mov rbp,qword [rsp+24] ;widthLeft
	test rbp,rbp
	jz vf7lop3b
	ALIGN 16
vf7lop3:
	mov eax,dword [rbx]
	movdqa xmm1,xmm8
	movd xmm4,[rcx+rax]
	movdqa xmm3,xmm8
	movd xmm0,[rdx+rax]
	punpcklbw xmm4,xmm0
	mov eax,dword [rbx+4]
	pxor xmm0,xmm0
	punpcklbw xmm0,xmm4
	movd xmm4,[rcx+rax]
	pxor xmm7,xmm7
	movd xmm2,[rdx+rax]
	punpcklbw xmm4,xmm2
	punpcklbw xmm7,xmm4
	movdqa xmm4,xmm0
	punpcklwd xmm0,xmm7
	punpckhwd xmm4,xmm7
	psrlw xmm0,1
	psrlw xmm4,1
	pmaddwd xmm0, xmm5
	pmaddwd xmm4, xmm5
	paddd xmm1,xmm0
	mov eax,dword [rbx+8]
	paddd xmm3,xmm4

	movd xmm7,[rdx+rax]
	pxor xmm0,xmm0
	movd xmm4,[rcx+rax]
	punpcklbw xmm4,xmm7
	mov eax,dword [rbx+12]
	punpcklbw xmm0,xmm4
	movd xmm4,[rcx+rax]
	pxor xmm7,xmm7
	movd xmm2,[rdx+rax]
	punpcklbw xmm4,xmm2
	punpcklbw xmm7,xmm4
	movdqa xmm4,xmm0
	punpcklwd xmm0,xmm7
	punpckhwd xmm4,xmm7
	psrlw xmm0,1
	psrlw xmm4,1
	pmaddwd xmm0, xmm6
	pmaddwd xmm4, xmm6
	paddd xmm1,xmm0
	paddd xmm3,xmm4

	psrad xmm1,16
	psrad xmm3,16
	packssdw xmm1,xmm3
	movdqu [rsi],xmm1
	lea rcx,[rcx+4]
	lea rsi,[rsi+16]
	lea rdx,[rdx+4]
;	dec rbp
;	jnz vf7lop3

	align 16
vf7lop3b:
	mov rbp,qword [rsp+136] ;width
	and rbp,3
	jz vf7lop3c
	mov eax,dword [rbx]
	movdqa xmm1,xmm8
	movdqa xmm3,xmm8
	movd xmm4,[rcx+rax]
	movd xmm0,[rdx+rax]
	punpcklbw xmm4,xmm0
	pxor xmm0,xmm0
	mov eax,dword [rbx+4]
	punpcklbw xmm0,xmm4
	movd xmm4,[rcx+rax]
	movd xmm2,[rdx+rax]
	punpcklbw xmm4,xmm2
	pxor xmm2,xmm2
	punpcklbw xmm2,xmm4
	movdqa xmm4,xmm0
	punpcklwd xmm0,xmm2
	punpckhwd xmm4,xmm2
	psrlw xmm0,1
	psrlw xmm4,1
	pmaddwd xmm0, xmm5
	pmaddwd xmm4, xmm5
	paddd xmm1,xmm0
	paddd xmm3,xmm4

	mov eax,dword [rbx+8]
	movd xmm4,[rcx+rax]
	movd xmm0,[rdx+rax]
	punpcklbw xmm4,xmm0
	pxor xmm0,xmm0
	mov eax,dword [rbx+12]
	punpcklbw xmm0,xmm4
	movd xmm4,[rcx+rax]
	movd xmm2,[rdx+rax]
	punpcklbw xmm4,xmm2
	pxor xmm2,xmm2
	punpcklbw xmm2,xmm4
	movdqa xmm4,xmm0
	punpcklwd xmm0,xmm2
	punpckhwd xmm4,xmm2
	psrlw xmm0,1
	psrlw xmm4,1
	pmaddwd xmm0, xmm6
	pmaddwd xmm4, xmm6
	paddd xmm1,xmm0
	paddd xmm3,xmm4

	psrad xmm1,16
	psrad xmm3,16
	packssdw xmm1,xmm3
	movd [rsi],xmm1
	lea rsi,[rsi+4]

	align 16
vf7lop3c:

	mov rbp,qword [rsp+48] ;cSub
	mov rsi,r10 ;inYPt
	mov rdi,qword [rsp+192] ;csLineBuff
	mov rcx,qword [rsp+200] ;csLineBuff2
	mov rbx,qword [rsp+224] ;yuv2rgb

	movzx rax,byte [rsi]
	movq xmm2,[rbx+rax * 8]
	movzx rax,byte [rsi+1]
	movq xmm4,[rbx+rax * 8]
	punpcklqdq xmm2,xmm4

	movzx rax,word [rcx]
	movq xmm0,[rbx+rax*8 + 2048]
	movzx rax,word [rcx+2]
	movq xmm1,[rbx+rax*8 + 526336]
	paddsw xmm0,xmm1

	pxor xmm3,xmm3
	punpcklqdq xmm3,xmm0
	psraw xmm3,1
	paddsw xmm2,xmm0
	paddsw xmm2,xmm3

	lea rsi,[rsi+2]
	lea rcx,[rcx+4]
	ALIGN 16
vf7lop2b:
	movzx rax,word [rcx]
	movq xmm0,[rbx+rax*8 + 2048]
	movzx rax,word [rcx+2]
	movq xmm1,[rbx+rax*8 + 526336]
	paddsw xmm0,xmm1

	pxor xmm3,xmm3
	punpcklqdq xmm3,xmm0
	psraw xmm3,1
	paddsw xmm2,xmm3
	movdqa [rdi],xmm2

	movzx rax,byte [rsi]
	movq xmm2,[rbx+rax * 8]
	movzx rax,byte [rsi+1]
	movq xmm4,[rbx+rax * 8]
	punpcklqdq xmm2,xmm4

	paddsw xmm2,xmm0
	paddsw xmm2,xmm3
	lea rdi,[rdi+16]
	lea rsi,[rsi+2]
	lea rcx,[rcx+4]
	dec rbp
	jnz vf7lop2b

	movzx rax,word [rcx]
	movq xmm0,[rbx+rax*8 + 2048]
	movzx rax,word [rcx+2]
	movq xmm1,[rbx+rax*8 + 526336]
	paddsw xmm0,xmm1

	pxor xmm3,xmm3
	punpcklqdq xmm3,xmm0
	por xmm0,xmm3
	psraw xmm3,1
	paddsw xmm2,xmm3
	movdqa [rdi],xmm2

	movzx rax,byte [rsi]
	movq xmm2,[rbx+rax * 8]
	movzx rax,byte [rsi+1]
	movq xmm4,[rbx+rax * 8]
	punpcklqdq xmm2,xmm4
	paddsw xmm2,xmm0
	movdqa [rdi+16],xmm2
	lea rsi,[rsi+2]
	lea rcx,[rcx+4]
	add rsi,qword [rsp+208] ;yAdd
	mov r10,rsi ;inYPt

	mov rdi,r9 ;outPt
	mov rcx,qword [rsp+136] ;width
	mov rsi,qword [rsp+192] ;csLineBuff
	mov rbx,qword [rsp+232] ;rgbGammaCorr
	ALIGN 16
vf7lop5:
	movzx rax,word [rsi+4]
	movq xmm5,[rbx+rax*8]
	movzx rax,word [rsi+2]
	movq xmm6,[rbx+rax*8+524288]
	paddsw xmm5,xmm6
	movzx rax,word [rsi]
	movq xmm6,[rbx+rax*8+1048576]
	paddsw xmm5,xmm6
	movq [rdi],xmm5
	lea rsi,[rsi+8]
	lea rdi,[rdi+8]
	dec ecx
	jnz vf7lop5

	add qword [rsp+168],48 ;weight
	add r9,qword [rsp+216] ;dstep outPt

	dec qword [rsp+144] ;currHeight
	jnz vf7lop
	jmp vflexit

	align 16
vflstart:
	mov qword [rsp+16],r12
	mov qword [rsp+24],r13
	mov qword [rsp+32],r14
	mov qword [rsp+40],r15
	xor r12,r12
	xor r13,r13
	xor r14,r14
	xor r15,r15
	test r9,15
	jnz vfnalop
	test qword [rsp+216],15
	jnz vfnalop
	
	align 16
vflop:
	mov rbx,qword [rsp+168] ;weight
	mov rsi,qword [rsp+200] ;csLineBuff2
	mov rcx,r11 ;inUPt
	mov rdx,r8 ;inVPt

	mov r12d,dword [rbx]
	mov r13d,dword [rbx+4]
	mov r14d,dword [rbx+8]
	mov r15d,dword [rbx+12]
	movdqa xmm5,[rbx+16]
	movdqa xmm6,[rbx+32]

	add qword [rsp+168],48 ;weight

	mov rbp,qword [rsp+56] ;sWidth
	xor rax,rax
	ALIGN 16
vflop2:
	pxor xmm1,xmm1
	movd xmm2,[rcx+r12]
	pxor xmm7,xmm7
	movd xmm4,[rdx+r12]
	punpcklbw xmm2,xmm4
	punpcklbw xmm1,xmm2
	movd xmm2,[rcx+r13]
	movdqa xmm3,xmm1
	movd xmm4,[rdx+r13]
	punpcklbw xmm2,xmm4
	punpcklbw xmm7,xmm2
	punpcklwd xmm1,xmm7
	punpckhwd xmm3,xmm7
	psrlw xmm1,1
	psrlw xmm3,1

	pmaddwd xmm1, xmm5
	movd xmm2,[rcx+r14]
	pmaddwd xmm3,xmm5
	movd xmm4,[rdx+r14]
	pxor xmm0,xmm0
	punpcklbw xmm2,xmm4
	punpcklbw xmm0,xmm2
	movd xmm4,[rcx+r15]
	pxor xmm7,xmm7
	movd xmm2,[rdx+r15]
	punpcklbw xmm4,xmm2
	punpcklbw xmm7,xmm4
	movdqa xmm4,xmm0
	punpcklwd xmm0,xmm7
	punpckhwd xmm4,xmm7
	psrlw xmm0,1
	psrlw xmm4,1
	pmaddwd xmm0,xmm6
	pmaddwd xmm4,xmm6
	paddd xmm1,xmm0
	paddd xmm3,xmm4
	paddd xmm1,xmm8
	paddd xmm3,xmm8

	psrad xmm1,16
	psrad xmm3,16
	packssdw xmm1,xmm3
	movdqa [rsi],xmm1
	lea rcx,[rcx+4]
	lea rdx,[rdx+4]
	lea rsi,[rsi+16]
	dec rbp
	jnz vflop2

	mov rbp,qword [rsp+48] ;cSub
	mov rsi,r10 ;inYPt
	mov rdi,r9 ;outPt
	mov rcx,qword [rsp+200] ;csLineBuff2
	mov rbx,qword [rsp+224] ;yuv2rgb
	mov r12,qword [rsp+232] ;rgbGammaCorr

	movzx rdx,word [rsi]
	movzx rax,dl
	movq xmm2,[rbx+rax * 8]
	shr rdx,8
	movq xmm4,[rbx+rdx * 8]
	punpcklqdq xmm2,xmm4

	mov edx,dword [rcx]
	movzx rax,dx
	movq xmm0,[rbx+rax*8 + 2048]
	shr rdx,16
	movq xmm1,[rbx+rdx*8 + 526336]
	paddsw xmm0,xmm1

	pxor xmm3,xmm3
	punpcklqdq xmm3,xmm0
	psraw xmm3,1
	paddsw xmm2,xmm0
	paddsw xmm2,xmm3

	lea rsi,[rsi+2]
	lea rcx,[rcx+4]
	ALIGN 16
y2rllop2b:
	mov edx,dword [rcx]
	movzx rax,dx
	shr rdx,16
	movq xmm0,[rbx+rax*8 + 2048]
	pxor xmm3,xmm3
	movq xmm1,[rbx+rdx*8 + 526336]
	paddsw xmm0,xmm1

	punpcklqdq xmm3,xmm0
	psraw xmm3,1
	paddsw xmm2,xmm3
	
	pextrw rax,xmm2,0
	movq xmm1,[r12+rax*8+1048576]
	pextrw rax,xmm2,4
	movq xmm7,[r12+rax*8+1048576]

	pextrw rax,xmm2,1
	movq xmm6,[r12+rax*8+524288]
	pextrw rax,xmm2,5
	movq xmm5,[r12+rax*8+524288]
	paddsw xmm1,xmm6
	paddsw xmm7,xmm5

	pextrw rax,xmm2,2
	movq xmm6,[r12+rax*8]
	pextrw rax,xmm2,6
	movq xmm5,[r12+rax*8]
	paddsw xmm1,xmm6
	paddsw xmm7,xmm5

	punpcklqdq xmm1,xmm7
	movntdq [rdi],xmm1

	movzx rdx,word [rsi]
	movzx rax,dl
	movq xmm2,[rbx+rax * 8]
	shr rdx,8
	movq xmm4,[rbx+rdx * 8]
	punpcklqdq xmm2,xmm4

	paddsw xmm2,xmm0
	paddsw xmm2,xmm3
	lea rdi,[rdi+16]
	lea rsi,[rsi+2]
	lea rcx,[rcx+4]
	dec rbp
	jnz y2rllop2b

	mov edx,dword [rcx]
	movzx rax,dx
	movq xmm0,[rbx+rax*8 + 2048]
	shr rdx,16
	movq xmm1,[rbx+rdx*8 + 526336]
	paddsw xmm0,xmm1

	pxor xmm3,xmm3
	punpcklqdq xmm3,xmm0
	por xmm0,xmm3
	psraw xmm3,1
	paddsw xmm2,xmm3

	pextrw rax,xmm2,0
	movq xmm1,[r12+rax*8+1048576]
	pextrw rax,xmm2,4
	movq xmm7,[r12+rax*8+1048576]

	pextrw rax,xmm2,1
	movq xmm6,[r12+rax*8+524288]
	pextrw rax,xmm2,5
	movq xmm5,[r12+rax*8+524288]
	paddsw xmm1,xmm6
	paddsw xmm7,xmm5

	pextrw rax,xmm2,2
	movq xmm6,[r12+rax*8]
	pextrw rax,xmm2,6
	movq xmm5,[r12+rax*8]
	paddsw xmm1,xmm6
	paddsw xmm7,xmm5

	punpcklqdq xmm1,xmm7
	movntdq [rdi],xmm1

	movzx rdx,word [rsi]
	movzx rax,dl
	movq xmm2,[rbx+rax * 8]
	shr rdx,8
	movq xmm4,[rbx+rdx * 8]
	punpcklqdq xmm2,xmm4
	paddsw xmm2,xmm0

	pextrw rax,xmm2,0
	movq xmm1,[r12+rax*8+1048576]
	pextrw rax,xmm2,4
	movq xmm7,[r12+rax*8+1048576]

	pextrw rax,xmm2,1
	movq xmm6,[r12+rax*8+524288]
	pextrw rax,xmm2,5
	movq xmm5,[r12+rax*8+524288]
	paddsw xmm1,xmm6
	paddsw xmm7,xmm5

	pextrw rax,xmm2,2
	movq xmm6,[r12+rax*8]
	pextrw rax,xmm2,6
	movq xmm5,[r12+rax*8]
	paddsw xmm1,xmm6
	paddsw xmm7,xmm5

	punpcklqdq xmm1,xmm7
	movntdq [rdi+16],xmm1

	add rsi,qword [rsp+208] ;yAdd
	lea rsi,[rsi+2]
	mov r10,rsi ;inYPt

	add r9,qword [rsp+216] ;dstep outPt
	xor r12,r12

	dec qword [rsp+144] ;currHeight
	jnz vflop
	mov r12,qword [rsp+16]
	mov r13,qword [rsp+24]
	mov r14,qword [rsp+32]
	mov r15,qword [rsp+40]
	jmp vflexit

	align 16
vfnalop:
	mov rbx,qword [rsp+168] ;weight
	mov rsi,qword [rsp+200] ;csLineBuff2
	mov rcx,r11 ;inUPt
	mov rdx,r8 ;inVPt

	mov r12d,dword [rbx]
	mov r13d,dword [rbx+4]
	mov r14d,dword [rbx+8]
	mov r15d,dword [rbx+12]
	movdqa xmm5,[rbx+16]
	movdqa xmm6,[rbx+32]

	add qword [rsp+168],48 ;weight

	mov rbp,qword [rsp+56] ;sWidth
	xor rax,rax
	ALIGN 16
vfnalop2:
	pxor xmm1,xmm1
	movd xmm2,[rcx+r12]
	pxor xmm7,xmm7
	movd xmm4,[rdx+r12]
	punpcklbw xmm2,xmm4
	punpcklbw xmm1,xmm2
	movd xmm2,[rcx+r13]
	movdqa xmm3,xmm1
	movd xmm4,[rdx+r13]
	punpcklbw xmm2,xmm4
	punpcklbw xmm7,xmm2
	punpcklwd xmm1,xmm7
	punpckhwd xmm3,xmm7
	psrlw xmm1,1
	psrlw xmm3,1

	pmaddwd xmm1, xmm5
	movd xmm2,[rcx+r14]
	pmaddwd xmm3,xmm5
	movd xmm4,[rdx+r14]
	pxor xmm0,xmm0
	punpcklbw xmm2,xmm4
	punpcklbw xmm0,xmm2
	movd xmm4,[rcx+r15]
	pxor xmm7,xmm7
	movd xmm2,[rdx+r15]
	punpcklbw xmm4,xmm2
	punpcklbw xmm7,xmm4
	movdqa xmm4,xmm0
	punpcklwd xmm0,xmm7
	punpckhwd xmm4,xmm7
	psrlw xmm0,1
	psrlw xmm4,1
	pmaddwd xmm0,xmm6
	pmaddwd xmm4,xmm6
	paddd xmm1,xmm0
	paddd xmm3,xmm4
	paddd xmm1,xmm8
	paddd xmm3,xmm8

	psrad xmm1,16
	psrad xmm3,16
	packssdw xmm1,xmm3
	movdqa [rsi],xmm1
	lea rcx,[rcx+4]
	lea rdx,[rdx+4]
	lea rsi,[rsi+16]
	dec rbp
	jnz vfnalop2

	mov rbp,qword [rsp+48] ;cSub
	mov rsi,r10 ;inYPt
	mov rdi,r9 ;outPt
	mov rcx,qword [rsp+200] ;csLineBuff2
	mov rbx,qword [rsp+224] ;yuv2rgb
	mov r12,qword [rsp+232] ;rgbGammaCorr

	movzx rdx,word [rsi]
	movzx rax,dl
	movq xmm2,[rbx+rax * 8]
	shr rdx,8
	movq xmm4,[rbx+rdx * 8]
	punpcklqdq xmm2,xmm4

	mov edx,dword [rcx]
	movzx rax,dx
	movq xmm0,[rbx+rax*8 + 2048]
	shr rdx,16
	movq xmm1,[rbx+rdx*8 + 526336]
	paddsw xmm0,xmm1

	pxor xmm3,xmm3
	punpcklqdq xmm3,xmm0
	psraw xmm3,1
	paddsw xmm2,xmm0
	paddsw xmm2,xmm3

	lea rsi,[rsi+2]
	lea rcx,[rcx+4]
	ALIGN 16
y2rlnalop2b:
	mov edx,dword [rcx]
	movzx rax,dx
	shr rdx,16
	movq xmm0,[rbx+rax*8 + 2048]
	pxor xmm3,xmm3
	movq xmm1,[rbx+rdx*8 + 526336]
	paddsw xmm0,xmm1

	punpcklqdq xmm3,xmm0
	psraw xmm3,1
	paddsw xmm2,xmm3
	
	pextrw rax,xmm2,0
	movq xmm1,[r12+rax*8+1048576]
	pextrw rax,xmm2,4
	movq xmm7,[r12+rax*8+1048576]

	pextrw rax,xmm2,1
	movq xmm6,[r12+rax*8+524288]
	pextrw rax,xmm2,5
	movq xmm5,[r12+rax*8+524288]
	paddsw xmm1,xmm6
	paddsw xmm7,xmm5

	pextrw rax,xmm2,2
	movq xmm6,[r12+rax*8]
	pextrw rax,xmm2,6
	movq xmm5,[r12+rax*8]
	paddsw xmm1,xmm6
	paddsw xmm7,xmm5

	punpcklqdq xmm1,xmm7
	movdqu [rdi],xmm1

	movzx rdx,word [rsi]
	movzx rax,dl
	movq xmm2,[rbx+rax * 8]
	shr rdx,8
	movq xmm4,[rbx+rdx * 8]
	punpcklqdq xmm2,xmm4

	paddsw xmm2,xmm0
	paddsw xmm2,xmm3
	lea rdi,[rdi+16]
	lea rsi,[rsi+2]
	lea rcx,[rcx+4]
	dec rbp
	jnz y2rlnalop2b

	mov edx,dword [rcx]
	movzx rax,dx
	movq xmm0,[rbx+rax*8 + 2048]
	shr rdx,16
	movq xmm1,[rbx+rdx*8 + 526336]
	paddsw xmm0,xmm1

	pxor xmm3,xmm3
	punpcklqdq xmm3,xmm0
	por xmm0,xmm3
	psraw xmm3,1
	paddsw xmm2,xmm3

	pextrw rax,xmm2,0
	movq xmm1,[r12+rax*8+1048576]
	pextrw rax,xmm2,4
	movq xmm7,[r12+rax*8+1048576]

	pextrw rax,xmm2,1
	movq xmm6,[r12+rax*8+524288]
	pextrw rax,xmm2,5
	movq xmm5,[r12+rax*8+524288]
	paddsw xmm1,xmm6
	paddsw xmm7,xmm5

	pextrw rax,xmm2,2
	movq xmm6,[r12+rax*8]
	pextrw rax,xmm2,6
	movq xmm5,[r12+rax*8]
	paddsw xmm1,xmm6
	paddsw xmm7,xmm5

	punpcklqdq xmm1,xmm7
	movdqu [rdi],xmm1

	movzx rdx,word [rsi]
	movzx rax,dl
	movq xmm2,[rbx+rax * 8]
	shr rdx,8
	movq xmm4,[rbx+rdx * 8]
	punpcklqdq xmm2,xmm4
	paddsw xmm2,xmm0

	pextrw rax,xmm2,0
	movq xmm1,[r12+rax*8+1048576]
	pextrw rax,xmm2,4
	movq xmm7,[r12+rax*8+1048576]

	pextrw rax,xmm2,1
	movq xmm6,[r12+rax*8+524288]
	pextrw rax,xmm2,5
	movq xmm5,[r12+rax*8+524288]
	paddsw xmm1,xmm6
	paddsw xmm7,xmm5

	pextrw rax,xmm2,2
	movq xmm6,[r12+rax*8]
	pextrw rax,xmm2,6
	movq xmm5,[r12+rax*8]
	paddsw xmm1,xmm6
	paddsw xmm7,xmm5

	punpcklqdq xmm1,xmm7
	movdqu [rdi+16],xmm1

	add rsi,qword [rsp+208] ;yAdd
	lea rsi,[rsi+2]
	mov r10,rsi ;inYPt

	add r9,qword [rsp+216] ;dstep outPt
	xor r12,r12

	dec qword [rsp+144] ;currHeight
	jnz vfnalop
	mov r12,qword [rsp+16]
	mov r13,qword [rsp+24]
	mov r14,qword [rsp+32]
	mov r15,qword [rsp+40]

	align 16
vflexit:
	add rsp,64
	pop rdi
	pop rsi
	pop rbx
	pop rbp
	ret


;void CSYUV420_LRGBC_do_yv12rgb8(UInt8 *yPtr, UInt8 *uPtr, UInt8 *vPtr, UInt8 *dest, OSInt width, OSInt height, OSInt dbpl, OSInt isFirst, OSInt isLast, UInt8 *csLineBuff, UInt8 *csLineBuff2, OSInt yBpl, OSInt uvBpl, Int64 *yuv2rgb, Int64 *rgbGammaCorr)
;8 widthLeft
;16 cSub
;24 cSize
;32 rdi
;40 rsi
;48 rbx
;56 rbp
;64 retAddr
;rcx yPtr r10
;rdx uPtr r11
;r8 vPtr
;r9 dest
;104 width
;112 height
;120 dbpl
;128 isFirst
;136 isLast
;144 csLineBuff
;152 csLineBuff2
;160 yBpl / yAdd
;168 uvBpl / uvAdd
;176 yuv2rgb
;184 rgbGammaCorr

	align 16
CSYUV420_LRGBC_do_yv12rgb8:
	push rbp
	push rbx
	push rsi
	push rdi
	sub rsp,32
	mov r10,rcx
	mov r11,rdx
	mov rdx,qword [rsp+104] ;width
	lea rax,[rdx*8]
	sub qword [rsp+160],rdx ;yAdd					Int32 yAdd = yBpl - width;
	shr rdx,1
	mov qword [rsp+24],rax ;cSize					Int32 cSize = width << 3;
	sub qword [rsp+168],rdx ;uvAdd					Int32 uvAdd = uvBpl - (width >> 1);
	sub rdx,2
	mov qword [rsp+16],rdx ;cSub						Int32 cSub = (width >> 1) - 2;

	mov rcx,qword [rsp+112] ;height
	shr rcx,1
	mov rbx,qword [rsp+136] ;isLast
	and rbx,1
	shl rbx,1
	sub rcx,rbx
	mov qword [rsp+112],rcx ;heightLeft

	mov rsi,r10 ;yPtr
	mov rcx,qword [rsp+104] ;width
	mov rdi,qword [rsp+144] ;csLineBuff
	mov rbx,qword [rsp+176] ;yuv2rgb
	
	shr rcx,3
	align 16
y2r8flop2a:
	movzx rax,byte [rsi]
	movq xmm0,[rbx+rax * 8]
	movzx rax,byte [rsi+1]
	movq xmm1,[rbx+rax * 8]
	punpcklqdq xmm0,xmm1
	movdqa [rdi],xmm0

	movzx rax,byte [rsi+2]
	movq xmm0,[rbx+rax * 8]
	movzx rax,byte [rsi+3]
	movq xmm1,[rbx+rax * 8]
	punpcklqdq xmm0,xmm1
	movdqa [rdi+16],xmm0

	movzx rax,byte [rsi+4]
	movq xmm0,[rbx+rax * 8]
	movzx rax,byte [rsi+5]
	movq xmm1,[rbx+rax * 8]
	punpcklqdq xmm0,xmm1
	movdqa [rdi+32],xmm0

	movzx rax,byte [rsi+6]
	movq xmm0,[rbx+rax * 8]
	movzx rax,byte [rsi+7]
	movq xmm1,[rbx+rax * 8]
	punpcklqdq xmm0,xmm1
	movdqa [rdi+48],xmm0

	add rsi,8
	add rdi,64
	dec rcx
	jnz y2r8flop2a

	mov rcx,qword [rsp+104] ;width
	add rsi,qword [rsp+160] ;yAdd
	shr ecx,3
	ALIGN 16
y2r8flop2a2:
	movzx rax,byte [rsi]
	movq xmm0,[rbx+rax * 8]
	movzx rax,byte [rsi+1]
	movq xmm1,[rbx+rax * 8]
	punpcklqdq xmm0,xmm1
	movdqa [rdi],xmm0

	movzx rax,byte [rsi+2]
	movq xmm0,[rbx+rax * 8]
	movzx rax,byte [rsi+3]
	movq xmm1,[rbx+rax * 8]
	punpcklqdq xmm0,xmm1
	movdqa [rdi+16],xmm0

	movzx rax,byte [rsi+4]
	movq xmm0,[rbx+rax * 8]
	movzx rax,byte [rsi+5]
	movq xmm1,[rbx+rax * 8]
	punpcklqdq xmm0,xmm1
	movdqa [rdi+32],xmm0

	movzx rax,byte [rsi+6]
	movq xmm0,[rbx+rax * 8]
	movzx rax,byte [rsi+7]
	movq xmm1,[rbx+rax * 8]
	punpcklqdq xmm0,xmm1
	movdqa [rdi+48],xmm0

	lea rsi,[rsi+8]
	lea rdi,[rdi+64]
	dec rcx
	jnz y2r8flop2a2
	add rsi,qword [rsp+160] ;yAdd
	mov r10,rsi ;yPtr

	mov rcx,qword [rsp+16] ;cSub
	mov rdx,qword [rsp+24] ;cSize
	mov rsi,r11 ;uPtr
	mov rdi,qword [rsp+144] ;csLineBuff
	shr rcx,1
	mov qword [rsp+8],rcx ;widthLeft
	mov rcx,r8 ;vPtr

	movzx rax,byte [rsi]
	movq xmm0,[rbx+rax*8 + 2048]
	movzx rax,byte [rcx]
	movq xmm1,[rbx+rax*8 + 4096]
	paddsw xmm0,xmm1

	pxor xmm1,xmm1
	punpcklqdq xmm1,xmm0
	psraw xmm1,1
	por xmm0,xmm1

	movdqa xmm6,[rdi]
	movdqa xmm7,[rdi+rdx]
	paddsw xmm6,xmm0
	psraw xmm0,1
	paddsw xmm7,xmm0

	lea rdi,[rdi+16]
	inc rsi
	inc rcx

	align 16
y2r8flop3a:
	movzx rax,byte [rsi]
	movq xmm0,[rbx+rax*8 + 2048]
	movzx rax,byte [rsi+1]
	movq xmm1,[rbx+rax*8 + 2048]
	movzx rax,byte [rcx]
	movq xmm4,[rbx+rax*8 + 4096]
	movzx rax,byte [rcx+1]
	movq xmm5,[rbx+rax*8 + 4096]
	paddsw xmm0,xmm4
	paddsw xmm1,xmm5

	pxor xmm4,xmm4
	pxor xmm5,xmm5
	punpcklqdq xmm4,xmm0
	punpcklqdq xmm5,xmm1
	psraw xmm4,1
	psraw xmm5,1
	por xmm0,xmm4
	por xmm1,xmm5
	paddsw xmm0,xmm5

	paddsw xmm6,xmm4
	psraw xmm4,1
	movdqa [rdi-16],xmm6
	paddsw xmm7,xmm4
	movdqa [rdi+rdx-16],xmm7

	movdqa xmm2,[rdi]
	movdqa xmm3,[rdi+rdx]
	paddsw xmm2,xmm0
	psraw xmm0,1
	movdqa [rdi],xmm2
	paddsw xmm3,xmm0
	movdqa [rdi+rdx],xmm3

	movdqa xmm6,[rdi+16]
	movdqa xmm7,[rdi+rdx+16]
	paddsw xmm6,xmm1
	psraw xmm1,1
	paddsw xmm7,xmm1

	lea rdi,[rdi+32]
	lea rsi,[rsi+2]
	lea rcx,[rcx+2]
	dec qword [rsp+8] ;widthLeft
	jnz y2r8flop3a

	pxor xmm4,xmm4
	movzx rax,byte [rsi]
	movq xmm0,[rbx+rax*8 + 2048]
	movzx rax,byte [rcx]
	movq xmm1,[rbx+rax*8 + 4096]
	paddsw xmm0,xmm1

	punpcklqdq xmm4,xmm0
	punpcklqdq xmm0,xmm0

	psraw xmm4,1
	paddsw xmm6,xmm4
	psraw xmm4,1
	paddsw xmm7,xmm4
	movdqa [rdi-16],xmm6
	movdqa [rdi+rdx-16],xmm7

	movdqa xmm2,[rdi]
	movdqa xmm3,[rdi+rdx]
	paddsw xmm2,xmm0
	psraw xmm0,1
	movdqa [rdi],xmm2
	paddsw xmm3,xmm0
	movdqa [rdi+rdx],xmm3

	lea rdi,[rdi+16]
	inc rsi
	inc rcx
	mov rbp,qword [rsp+168] ;uvAdd
	add rsi,rbp
	add rcx,rbp
	mov r11,rsi ;uPtr
	mov r8,rcx ;vPtr

	align 16
y2r8flop:

	mov rsi,r10 ;yPtr
	mov rcx,qword [rsp+104] ;width
	mov rdi,qword [rsp+152] ;csLineBuff2
	mov rbx,qword [rsp+176] ;yuv2rgb
	shr rcx,1
	align 16
y2r8flop2:
	mov dx,word [rsi]
	movzx rax,dl
	movq xmm0,[rbx+rax*8]
	movzx eax,dh
	movq xmm1,[rbx+rax*8]
	punpcklqdq xmm0,xmm1
	movdqa [rdi],xmm0
	lea esi,[rsi+2]
	lea edi,[rdi+16]
	dec rcx
	jnz y2r8flop2

	mov rcx,qword [rsp+104] ;width
	add rsi,qword [rsp+160] ;yAdd
	shr rcx,1
	align 16
y2r8flop2_2:
	mov dx,word [rsi]
	movzx rax,dl
	movq xmm0,[rbx+rax*8]
	movzx eax,dh
	movq xmm1,[rbx+rax*8]
	punpcklqdq xmm0,xmm1
	movdqa [rdi],xmm0
	lea rsi,[rsi+2]
	lea rdi,[rdi+16]
	dec rcx
	jnz y2r8flop2_2

	add rsi,qword [rsp+160] ;yAdd
	mov r10,rsi ;yPtr

	mov rcx,qword [rsp+16] ;cSub
	mov rsi,r11 ;uPtr
	mov rdx,r8 ;vPtr
	mov rbp,qword [rsp+144] ;csLineBuff
	mov rdi,qword [rsp+152] ;csLineBuff2
	shr rcx,1
	mov qword [rsp+8],rcx ;widthLeft
	mov rcx,qword [rsp+24] ;cSize

	pxor xmm4,xmm4
	movzx rax,byte [rsi]
	movq xmm0,[rbx+rax*8 + 2048]
	movzx rax,byte [rdx]
	movq xmm1,[rbx+rax*8 + 4096]
	paddsw xmm0,xmm1

	punpcklqdq xmm4,xmm0
	psraw xmm4,1
	por xmm0,xmm4

	movdqa xmm2,[rdi]
	movdqa xmm3,[rdi+rcx]
	paddsw xmm2,xmm0
	psraw xmm0,1
	paddsw xmm3,xmm0
	movdqa xmm6,[rbp+rcx]
	paddsw xmm6,xmm0


	add rbp,16
	add rdi,16
	inc rsi
	inc rdx

	ALIGN 16
y2r8flop3:
	movzx rax,byte [rsi]
	movq xmm0,[rbx+rax*8 + 2048]
	movzx rax,byte [rsi+1]
	movq xmm1,[rbx+rax*8 + 2048]
	movzx rax,byte [rdx]
	movq xmm4,[rbx+rax*8 + 4096]
	movzx rax,byte [rdx+1]
	movq xmm5,[rbx+rax*8 + 4096]
	paddsw xmm0,xmm4
	paddsw xmm1,xmm5

	pxor xmm4,xmm4
	pxor xmm5,xmm5

	punpcklqdq xmm4,xmm0
	punpcklqdq xmm5,xmm1
	psraw xmm4,1
	psraw xmm5,1
	por xmm0,xmm4
	por xmm1,xmm5
	paddsw xmm0,xmm5

	paddsw xmm2,xmm4
	psraw xmm4,1
	movdqa [rdi-16],xmm2
	paddsw xmm3,xmm4
	paddsw xmm6,xmm4
	movdqa [rdi+rcx-16],xmm3
	movdqa [rbp+rcx-16],xmm6

	movdqa xmm2,[rdi]
	movdqa xmm3,[rdi+rcx]
	paddsw xmm2,xmm0
	psraw xmm0,1
	movdqa [rdi],xmm2
	paddsw xmm3,xmm0
	movdqa xmm6,[rbp+rcx]
	paddsw xmm6,xmm0
	movdqa [rdi+rcx],xmm3
	movdqa [rbp+rcx],xmm6

	movdqa xmm2,[rdi+16]
	movdqa xmm3,[rdi+rcx+16]
	paddsw xmm2,xmm1
	psraw xmm1,1
	paddsw xmm3,xmm1
	movdqa xmm6,[rbp+rcx+16]
	paddsw xmm6,xmm1

	lea rbp,[rbp+32]
	lea rdi,[rdi+32]
	lea rsi,[rsi+2]
	lea rdx,[rdx+2]
	dec qword [rsp+8] ;widthLeft
	jnz y2r8flop3

	movzx rax,byte [rsi]
	movq xmm0,[rbx+rax*8 + 2048]
	movzx rax,byte [rdx]
	movq xmm1,[rbx+rax*8 + 4096]
	paddsw xmm0,xmm1

	pxor xmm4,xmm4
	punpcklqdq xmm4,xmm0
	por xmm0,xmm4
	psraw xmm4,1

	paddsw xmm2,xmm4
	movdqa [rdi-16],xmm2
	psraw xmm4,1
	paddsw xmm3,xmm4
	paddsw xmm6,xmm4
	movdqa [rdi+rcx-16],xmm3
	movdqa [rbp+rcx-16],xmm6

	movdqa xmm2,[rdi]
	movdqa xmm3,[rdi+rcx]
	movdqa xmm6,[rbp+rcx]
	paddsw xmm2,xmm0
	movdqa [rdi],xmm2
	psraw xmm0,1
	paddsw xmm3,xmm0
	paddsw xmm6,xmm0
	movdqa [rdi+rcx],xmm3
	movdqa [rbp+rcx],xmm6

	lea rbp,[rbp+16]
	lea rdi,[rdi+16]
	inc rsi
	inc rdx
	add rsi,qword [rsp+168] ;uvAdd
	add rdx,qword [rsp+168] ;uvAdd
	mov r11,rsi ;uPtr
	mov r8,rdx ;vPtr

	mov rdi,r9 ;dest
	mov rcx,qword [rsp+104] ;width
	mov rsi,qword [rsp+144] ;csLineBuff
	mov rbx,qword [rsp+184] ;rgbGammaCorr
	ALIGN 16
y2r8flop5:
	movzx rax,word [rsi+4]
	movq xmm7,[rbx+rax*8]
	movzx rax,word [rsi+2]
	movq xmm6,[rbx+rax*8+524288]
	paddsw xmm7,xmm6
	movzx rax,word [rsi]
	movq xmm6,[rbx+rax*8+1048576]
	paddsw xmm7,xmm6
	movq [rdi],xmm7
	lea rsi,[rsi+8]
	lea rdi,[rdi+8]
	dec rcx
	jnz y2r8flop5

	add r9,qword [rsp+120] ;dbpl dest

	mov rdi,r9 ;dest
	mov rcx,qword [rsp+104] ;width
	ALIGN 16
y2r8flop6:
	movzx rax,word [rsi+4]
	movq xmm7,[rbx+rax*8]
	movzx rax,word [rsi+2]
	movq xmm6,[rbx+rax*8+524288]
	paddsw xmm7,xmm6
	movzx rax,word [rsi]
	movq xmm6,[rbx+rax*8+1048576]
	paddsw xmm7,xmm6
	movq [rdi],xmm7
	lea rsi,[rsi+8]
	lea rdi,[rdi+8]
	dec rcx
	jnz y2r8flop6

	add r9,qword [rsp+120] ;dbpl dest
	mov rax,qword [rsp+144] ;csLineBuff
	xchg rax,qword [rsp+152] ;csLineBuff2
	mov qword [rsp+144],rax ;;csLineBuff

	dec qword [rsp+112] ;heightLeft
	jnz y2r8flop

	test qword [rsp+136],1 ;isLast
	jz yv2r8flopexit

	mov rsi,r10 ;yPtr
	mov rcx,qword [rsp+104] ;width
	mov rdi,qword [rsp+152] ;csLineBuff2
	mov rbx,qword [rsp+176] ;yuv2rgb
	shr rcx,1
	ALIGN 16
y2r8flop2b:
	movzx rax,byte [rsi]
	movq xmm0,[rbx+rax*8]
	movzx rax,byte [rsi+1]
	movq xmm1,[rbx+rax*8]
	punpcklqdq xmm0,xmm1
	movdqa [rdi],xmm0
	lea rsi,[rsi+2]
	lea rdi,[rdi+16]
	dec rcx
	jnz y2r8flop2b
	add rsi,qword [rsp+160] ;yAdd

	mov rcx,qword [rsp+104] ;width
	shr rcx,1
	ALIGN 16
y2r8flop2b2:
	movzx rax,byte [rsi]
	movq xmm0,[rbx+rax*8]
	movzx rax,byte [rsi+1]
	movq xmm1,[rbx+rax*8]
	punpcklqdq xmm0,xmm1
	movdqa [rdi],xmm0
	lea rsi,[rsi+2]
	lea rdi,[rdi+16]
	dec rcx
	jnz y2r8flop2b2
	add rsi,qword [rsp+160] ;yAdd
	mov r10,rsi ;yPtr

	mov rcx,qword [rsp+16] ;cSub
	mov rsi,r11 ;uPtr
	mov rdx,r8 ;vPtr
	mov rbp,qword [rsp+144] ;csLineBuff
	mov rdi,qword [rsp+152] ;csLineBuff2
	shr rcx,1
	mov qword [rsp+8],rcx ;widthLeft
	mov rcx,qword [rsp+24] ;cSize

	pxor xmm1,xmm1
	movzx rax,byte [rsi]
	movq xmm0,[rbx+rax*8 + 2048]
	movzx rax,byte [rdx]
	movq xmm4,[rbx+rax*8 + 4096]
	paddsw xmm0,xmm4

	punpcklqdq xmm1,xmm0
	psraw xmm1,1
	por xmm0,xmm1

	movdqa xmm2,[rdi]
	movdqa xmm3,[rdi+rcx]
	movdqa xmm6,[rbp+rcx]
	paddsw xmm2,xmm0
	paddsw xmm3,xmm0
	psraw xmm0,1
	paddsw xmm6,xmm0

	lea rbp,[rbp+16]
	lea rdi,[rdi+16]
	inc rsi
	inc rdx

	align 16
y2r8flop3b:
	movzx rax,byte [rsi]
	movq xmm0,[rbx+rax*8 + 2048]
	movzx rax,byte [rsi+1]
	movq xmm1,[rbx+rax*8 + 2048]
	movzx rax,byte [rdx]
	movq xmm4,[rbx+rax*8 + 4096]
	movzx rax,byte [rdx+1]
	movq xmm5,[rbx+rax*8 + 4096]
	paddsw xmm0,xmm4
	paddsw xmm1,xmm5

	pxor xmm4,xmm4
	pxor xmm5,xmm5

	punpcklqdq xmm4,xmm0
	punpcklqdq xmm5,xmm1
	psraw xmm4,1
	psraw xmm5,1
	por xmm0,xmm4
	por xmm1,xmm5
	paddsw xmm0,xmm5

	paddsw xmm2,xmm4
	paddsw xmm3,xmm4
	movdqa [rdi-16],xmm2
	psraw xmm4,1
	movdqa [rdi+rcx-16],xmm3
	paddsw xmm6,xmm4
	movdqa [rbp+rcx-16],xmm6

	movdqa xmm2,[rdi]
	movdqa xmm3,[rdi+rcx]
	movdqa xmm6,[rbp+rcx]
	paddsw xmm2,xmm0
	paddsw xmm3,xmm0
	movdqa [rdi],xmm2
	psraw xmm0,1
	movdqa [rdi+rcx],xmm3
	paddsw xmm6,xmm0
	movdqa [rbp+rcx],xmm6
	movdqa xmm2,[rdi+16]
	movdqa xmm3,[rdi+rcx+16]
	movdqa xmm6,[rbp+rcx+16]
	paddsw xmm2,xmm1
	paddsw xmm3,xmm1
	psraw xmm1,1
	paddsw xmm6,xmm1

	lea rbp,[rbp+32]
	lea rdi,[rdi+32]
	lea rsi,[rsi+2]
	lea rdx,[rdx+2]
	dec qword [rsp+8] ;widthLeft
	jnz y2r8flop3b

	pxor xmm1,xmm1
	movzx rax,byte [rsi]
	movq xmm0,[rbx+rax*8+2048]
	movzx rax,byte [rdx]
	movq xmm4,[rbx+rax*8+4096]
	paddsw xmm0,xmm4

	punpcklqdq xmm1,xmm0
	punpcklqdq xmm0,xmm0
	psraw xmm1,1

	paddsw xmm2,xmm1
	paddsw xmm3,xmm1
	movdqa [rdi-16],xmm2
	psraw xmm1,1
	movdqa [rdi+rcx-16],xmm3
	paddsw xmm6,xmm1
	movdqa [rbp+rcx-16],xmm6
	movdqa xmm2,[rdi]
	movdqa xmm3,[rdi+rcx]
	movdqa xmm6,[rbp+rcx]
	paddsw xmm2,xmm0
	paddsw xmm3,xmm0
	movdqa [rdi],xmm2
	psraw xmm0,1
	movdqa [rdi+rcx],xmm3
	paddsw xmm6,xmm0
	movdqa [rbp+rcx],xmm6

	lea rbp,[rbp+16]
	lea rdi,[rbp+16]
	inc rsi
	inc rdx
	add rsi,qword [rsp+168] ;uvAdd
	add rdx,qword [rsp+168] ;uvAdd
	mov r11,rsi ;uPtr
	mov r8,rdx ;vPtr

	mov rdi,r9 ;dest
	mov rcx,qword [rsp+104] ;width
	mov rsi,qword [rsp+144] ;csLineBuff
	mov rbx,qword [rsp+184] ;rgbGammaCorr
	ALIGN 16
y2r8flop5b:
	movzx rax,word [rsi+4]
	movq xmm7,[rbx+rax*8]
	movzx rax,word [rsi+2]
	movq xmm6,[rbx+rax*8+524288]
	paddsw xmm7,xmm6
	movzx rax,word [rsi]
	movq xmm6,[rbx+rax*8+1048576]
	paddsw xmm7,xmm6
	movq [rdi],xmm7
	lea rsi,[rsi+8]
	lea rdi,[rdi+8]
	dec rcx
	jnz y2r8flop5b

	add r9,qword [rsp+120] ;dbpl dest

	mov rdi,r9 ;dest
	mov rcx,qword [rsp+104] ;width
	ALIGN 16
y2r8flop6b:
	movzx rax,word [rsi+4]
	movq xmm7,[rbx+rax*8]
	movzx rax,word [rsi+2]
	movq xmm6,[rbx+rax*8+524288]
	paddsw xmm7,xmm6
	movzx rax,word [rsi]
	movq xmm6,[rbx+rax*8+1048576]
	paddsw xmm7,xmm6
	movq [rdi],xmm7
	lea rsi,[rsi+8]
	lea rdi,[rdi+8]
	dec rcx
	jnz y2r8flop6b

	add r9,qword [rsp+120] ;dbpl dest

	mov rax,qword [rsp+144] ;csLineBuff
	xchg rax,qword [rsp+152] ;csLineBuff2
	mov qword [rsp+144],rax ;csLineBuff

	mov rdi,r9 ;dest
	mov rcx,qword [rsp+104] ;width
	mov rsi,qword [rsp+144] ;csLineBuff
	mov rbx,qword [rsp+184] ;rgbGammaCorr
	ALIGN 16
y2r8flop5c:
	movzx rax,word [rsi+4]
	movq xmm7,[rbx+rax*8]
	movzx rax,word [rsi+2]
	movq xmm6,[rbx+rax*8+524288]
	paddsw xmm7,xmm6
	movzx rax,word [rsi]
	movq xmm6,[rbx+rax*8+1048576]
	paddsw xmm7,xmm6
	movq [rdi],xmm7
	lea rsi,[rsi+8]
	lea rdi,[rdi+8]
	dec rcx
	jnz y2r8flop5c

	add r9,qword [rsp+120] ;dbpl dest

	mov rdi,r9 ;dest
	mov rcx,qword [rsp+104] ;width
	ALIGN 16
y2r8flop6c:
	movzx rax,word [rsi+4]
	movq xmm7,[rbx+rax*8]
	movzx rax,word [rsi+2]
	movq xmm6,[rbx+rax*8+524288]
	paddsw xmm7,xmm6
	movzx rax,word [rsi]
	movq xmm6,[rbx+rax*8+1048576]
	paddsw xmm7,xmm6
	movq [rdi],xmm7
	lea rsi,[rsi+8]
	lea rdi,[rdi+8]
	dec rcx
	jnz y2r8flop6c

	add r9,qword [rsp+120] ;dbpl dest

	mov rax,qword [rsp+144] ;csLineBuff
	xchg rax,qword [rsp+152] ;csLineBuff2
	mov qword [rsp+144],rax ;csLineBuff

	align 16
yv2r8flopexit:
	add rsp,32
	pop rdi
	pop rsi
	pop rbx
	pop rbp
	ret

;void CSYUV420_LRGBC_do_yv12rgb2(UInt8 *yPtr, UInt8 *uPtr, UInt8 *vPtr, UInt8 *dest, OSInt width, OSInt height, OSInt dbpl, OSInt isFirst, OSInt isLast, UInt8 *csLineBuff, UInt8 *csLineBuff2, OSInt yBpl, OSInt uvBpl, Int64 *yuv2rgb, Int64 *rgbGammaCorr)
;8 widthLeft
;16 cSub
;24 cSize
;32 rdi
;40 rsi
;48 rbx
;56 rbp
;64 retAddr
;rcx yPtr r10
;rdx uPtr r11
;r8 vPtr
;r9 dest
;104 width
;112 height
;120 dbpl
;128 isFirst
;136 isLast
;144 csLineBuff
;152 csLineBuff2
;160 yBpl / yAdd
;168 uvBpl / uvAdd
;176 yuv2rgb
;184 rgbGammaCorr

	align 16
CSYUV420_LRGBC_do_yv12rgb2:
	push rbp
	push rbx
	push rsi
	push rdi
	sub rsp,32
	mov r10,rcx
	mov r11,rdx
	mov rdx,qword [rsp+104] ;width
	lea rcx,[rdx-2]
	lea rax,[rdx*8]
	sub qword [rsp+160],rcx ;yAdd					Int32 yAdd = yBpl - width - 2;
	shr rdx,1
	inc rdx
	mov qword [rsp+24],rax ;cSize					Int32 cSize = width << 3;
	sub qword [rsp+168],rdx ;uvAdd					Int32 uvAdd = uvBpl - ((width + 2) >> 1);
	sub rdx,2
	mov qword [rsp+16],rdx ;cSub						Int32 cSub = (width >> 1) - 2;

	mov rcx,qword [rsp+112] ;height
	shr rcx,1
	mov rbx,qword [rsp+136] ;isLast
	and rbx,1
	shl rbx,1
	sub rcx,rbx
	mov qword [rsp+112],rcx ;heightLeft

	mov rcx,qword [rsp+104] ;width
	shr rcx,2
	mov rsi,r10 ;yPtr
	mov rdi,qword [rsp+144] ;csLineBuff
	mov rbx,qword [rsp+176] ;yuv2rgb

	mov rbp,rcx
	align 16
y2rflop2a:
	movzx rax,byte [rsi]
	movq xmm0,[rbx+rax * 8]
	movq [rdi],xmm0
	movzx rax,byte [rsi+1]
	movq xmm0,[rbx+rax * 8]
	movq [rdi+8],xmm0
	movzx rax,byte [rsi+2]
	movq xmm0,[rbx+rax * 8]
	movq [rdi+16],xmm0
	movzx rax,byte [rsi+3]
	movq xmm0,[rbx+rax * 8]
	movq [rdi+24],xmm0

	add rsi,4
	add rdi,32
	dec rcx
	jnz y2rflop2a
	movzx rax,byte [rsi]
	movq xmm0,[rbx+rax * 8]
	movq [rdi],xmm0
	movzx eax,byte [rsi+1]
	movq xmm0,[rbx+rax * 8]
	movq [rdi+8],xmm0
	add rsi,4
	add rdi,16
	add rsi,qword [rsp+160] ;yAdd
	mov rcx,rbp

	align 16
y2rflop2c:
	movzx rax,byte [rsi]
	movq xmm0,[rbx+rax * 8]
	movq [rdi],xmm0
	movzx rax,byte [rsi+1]
	movq xmm0,[rbx+rax * 8]
	movq [rdi+8],xmm0
	movzx rax,byte [rsi+2]
	movq xmm0,[rbx+rax * 8]
	movq [rdi+16],xmm0
	movzx rax,byte [rsi+3]
	movq xmm0,[rbx+rax * 8]
	movq [rdi+24],xmm0

	add rsi,4
	add rdi,32
	dec rcx
	jnz y2rflop2c
	movzx rax,byte [rsi]
	movq xmm0,[rbx+rax * 8]
	movq [rdi],xmm0
	movzx rax,byte [rsi+1]
	movq xmm0,[rbx+rax * 8]
	movq [rdi+8],xmm0
	add rsi,4
	add rdi,16
	add rsi,qword [rsp+160] ;yAdd
	mov r10,rsi ;yPtr

	mov rcx,qword [rsp+16] ;cSub
	mov rdx,qword [rsp+24] ;cSize
	mov rsi,r11 ;uPtr
	mov rdi,qword [rsp+144] ;csLineBuff
	mov qword [rsp+8],rcx ;widthLeft
	mov rcx,r8 ;vPtr

	movzx rax,byte [rsi]
	movq xmm0,[rbx+rax*8 + 2048]
	movzx rax,byte [rcx]
	movq xmm1,[rbx+rax*8 + 4096]

	movq xmm2,[rdi]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [rdi],xmm2
	psraw xmm0,1
	psraw xmm1,1
	movq xmm2,[rdi+rdx]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [rdi+rdx],xmm2
	movq xmm2,[rdi+8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [rdi+8],xmm2
	psraw xmm0,1
	psraw xmm1,1
	movq xmm2,[rdi+rdx+8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [rdi+rdx+8],xmm2

	add rdi,16
	inc rsi
	inc rcx

	align 16
y2rflop3a:
	movzx rax,byte [rsi]
	movq xmm0,[rbx+rax*8 + 2048]
	movzx rax,byte [rcx]
	movq xmm1,[rbx+rax*8 + 4096]

	movq xmm2,[rdi]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [rdi],xmm2
	psraw xmm0,1
	psraw xmm1,1
	movq xmm2,[rdi+rdx]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [rdi+rdx],xmm2
	movq xmm2,[rdi+8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [rdi+8],xmm2
	movq xmm2,[rdi-8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [rdi-8],xmm2
	psraw xmm0,1
	psraw xmm1,1
	movq xmm2,[rdi+rdx+8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [rdi+rdx+8],xmm2
	movq xmm2,[rdi+rdx-8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [rdi+rdx-8],xmm2

	add rdi,16
	inc rsi
	inc rcx
	dec qword [rsp+8] ;widthLeft
	jnz y2rflop3a


	movzx rax,byte [rsi]
	movq xmm0,[rbx+rax*8 + 2048]
	movzx rax,byte [rcx]
	movq xmm1,[rbx+rax*8 + 4096]

	movq xmm2,[rdi]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [rdi],xmm2
	movq xmm2,[rdi+8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [rdi+8],xmm2
	psraw xmm0,1
	psraw xmm1,1
	movq xmm2,[rdi+rdx]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [rdi+rdx],xmm2
	movq xmm2,[rdi-8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [rdi-8],xmm2
	movq xmm2,[rdi+rdx+8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [rdi+rdx+8],xmm2
	psraw xmm0,1
	psraw xmm1,1
	movq xmm2,[rdi+rdx-8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [rdi+rdx-8],xmm2

	add rdi,16
	add rsi,2
	add rcx,2
	add rsi,qword [rsp+168] ;uvAdd
	add rcx,qword [rsp+168] ;uvAdd
	mov r11,rsi ;uPtr
	mov r8,rcx ;vPtr
	align 16
y2rflop:

	mov rsi,r10 ;yPtr
	mov rcx,qword [rsp+104] ;width
	mov rdi,qword [rsp+152] ;csLineBuff2
	mov rbx,qword [rsp+176] ;yuv2rgb

	align 16
y2rflop2:
	movzx rax,byte [rsi]
	movq xmm0,[rbx+rax*8]
	movq [rdi],xmm0
	inc rsi
	add rdi,8
	dec rcx
	jnz y2rflop2
	add rsi,2
	add rsi,qword [rsp+160] ;yAdd

	mov rcx,qword [rsp+104] ;width
	align 16
y2rflop2d:
	movzx rax,byte [rsi]
	movq xmm0,[rbx+rax*8]
	movq [rdi],xmm0
	inc rsi
	add rdi,8
	dec rcx
	jnz y2rflop2d
	add rsi,2
	add rsi,qword [rsp+160] ;yAdd

	mov r10,rsi ;yPtr

	mov rcx,qword [rsp+16] ;cSub
	mov rsi,r11 ;uPtr
	mov rdx,r8 ;vPtr
	mov rbp,qword [rsp+144] ;csLineBuff
	mov rdi,qword [rsp+152] ;csLineBuff2
	mov qword [rsp+8],rcx ;widthLeft
	mov rcx,qword [rsp+24] ;cSize

	movzx rax,byte [rsi]
	movq xmm0,[rbx+rax*8 + 2048]
	movzx rax,byte [rdx]
	movq xmm1,[rbx+rax*8 + 4096]

	movq xmm2,[rdi]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [rdi],xmm2
	psraw xmm0,1
	psraw xmm1,1
	movq xmm2,[rdi+rcx]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [rdi+rcx],xmm2
	movq xmm2,[rbp+rcx]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [rbp+rcx],xmm2
	movq xmm2,[rdi+8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [rdi+8],xmm2
	psraw xmm0,1
	psraw xmm1,1
	movq xmm2,[rdi+rcx+8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [rdi+rcx+8],xmm2
	movq xmm2,[rbp+rcx+8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [rbp+rcx+8],xmm2

	lea rbp,[rbp+16]
	lea rdi,[rdi+16]
	inc rsi
	inc rdx

	align 16
y2rflop3:
	movzx rax,byte [rsi]
	movq xmm0,[rbx+rax*8 + 2048]
	movzx rax,byte [rdx]
	movq xmm1,[rbx+rax*8 + 4096]

	movq xmm2,[rdi]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [rdi],xmm2
	psraw xmm0,1
	psraw xmm1,1
	movq xmm2,[rdi+rcx]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [rdi+rcx],xmm2
	movq xmm2,[rbp+rcx]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [rbp+rcx],xmm2
	movq xmm2,[rdi+8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [rdi+8],xmm2
	movq xmm2,[rdi-8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [rdi-8],xmm2
	psraw xmm0,1
	psraw xmm1,1
	movq xmm2,[rdi+rcx+8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [rdi+rcx+8],xmm2
	movq xmm2,[rbp+rcx+8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [rbp+rcx+8],xmm2
	movq xmm2,[rdi+rcx-8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [rdi+rcx-8],xmm2
	movq xmm2,[rbp+rcx-8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [rbp+rcx-8],xmm2

	lea rbp,[rbp+16]
	lea rdi,[rdi+16]
	inc rsi
	inc rdx
	dec qword [rsp+8] ;widthLeft
	jnz y2rflop3

	movzx rax,byte [rsi]
	movq xmm0,[rbx+rax*8 + 2048]
	movzx rax,byte [rdx]
	movq xmm1,[rbx+rax*8 + 4096]

	movq xmm2,[rdi]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [rdi],xmm2
	movq xmm2,[rdi+8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [rdi+8],xmm2
	psraw xmm0,1
	psraw xmm1,1
	movq xmm2,[rdi+rcx]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [rdi+rcx],xmm2
	movq xmm2,[rbp+rcx]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [rbp+rcx],xmm2
	movq xmm2,[rdi+rcx+8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [rdi+rcx+8],xmm2
	movq xmm2,[rbp+rcx+8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [rbp+rcx+8],xmm2
	movq xmm2,[rdi-8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [rdi-8],xmm2
	psraw xmm0,1
	psraw xmm1,1
	movq xmm2,[rdi+rcx-8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [rdi+rcx-8],xmm2
	movq xmm2,[rbp+rcx-8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [rbp+rcx-8],xmm2

	lea rbp,[rbp+16]
	add rdi,16
	add rsi,2
	add rdx,2
	add rsi,qword [rsp+168] ;uvAdd
	add rdi,qword [rsp+168] ;uvAdd
	mov r11,rsi ;uPtr
	mov r8,rdx ;vPtr

	mov rdi,r9 ;dest
	mov rcx,qword [rsp+104] ;width
	mov rsi,qword [rsp+144] ;csLineBuff
	mov rbx,qword [rsp+184] ;rgbGammaCorr
	align 16
y2rflop5:
	movzx rax,word [rsi+4]
	movq xmm7,[rbx+rax*8]
	movzx rax,word [rsi+2]
	movq xmm6,[rbx+rax*8+524288]
	paddsw xmm7,xmm6
	movzx rax,word [rsi]
	movq xmm6,[rbx+rax*8+1048576]
	paddsw xmm7,xmm6
	movq [rdi],xmm7
	add rsi,8
	add rdi,8
	dec rcx
	jnz y2rflop5

	add r9,qword [rsp+120] ;dbpl dest

	mov rdi,r9 ;dest
	mov rcx,qword [rsp+104] ;width
	align 16
y2rflop6:
	movzx rax,word [rsi+4]
	movq xmm7,[rbx+rax*8]
	movzx rax,word [rsi+2]
	movq xmm6,[rbx+rax*8+524288]
	paddsw xmm7,xmm6
	movzx rax,word [rsi]
	movq xmm6,[rbx+rax*8+1048576]
	paddsw xmm7,xmm6
	movq [rdi],xmm7
	add rsi,8
	add rdi,8
	dec rcx
	jnz y2rflop6

	add r9,qword [rsp+120] ;dbpl dest

	mov rax,qword [rsp+144] ;csLineBuff
	xchg rax,qword [rsp+152] ;csLineBuff2
	mov qword [rsp+144],rax ;csLineBuff

	dec qword [rsp+112] ;heightLeft
	jnz y2rflop

	test qword [rsp+136],1 ;isLast
	jz yv2rflopexit

	mov rsi,r10 ;yPtr
	mov rcx,qword [rsp+104] ;width
	mov rdi,qword [rsp+152] ;csLineBuff2
	mov rbx,qword [rsp+176] ;yuv2rgb

	align 16
y2rflop2b:
	movzx rax,byte [rsi]
	movq xmm0,[rbx+rax*8]
	movq [rdi],xmm0
	inc rsi
	add rdi,8
	dec rcx
	jnz y2rflop2b
	inc rsi

	mov rcx,qword [rsp+104] ;width
	align 16
y2rflop2e:
	movzx rax,byte [rsi]
	movq xmm0,[rbx+rax*8]
	movq [rdi],xmm0
	inc rsi
	add rdi,8
	dec rcx
	jnz y2rflop2e
	inc rsi
	add rsi,qword [rsp+160] ;yAdd
	mov r10,rsi ;yPtr

	mov rcx,qword [rsp+16] ;cSub
	mov rsi,r11 ;uPtr
	mov rdx,r8 ;vPtr
	mov rbp,qword [rsp+144] ;csLineBuff
	mov rdi,qword [rsp+152] ;csLineBuff2
	mov qword [rsp+8],rcx ;widthLeft
	mov rcx,qword [rsp+24] ;cSize

	movzx rax,byte [rsi]
	movq xmm0,[rbx+rax*8 + 2048]
	movzx rax,byte [rdx]
	movq xmm1,[rbx+rax*8 + 4096]

	movq xmm2,[rdi]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [rdi],xmm2
	movq xmm2,[rdi+rcx]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [rdi+rcx],xmm2
	psraw xmm0,1
	psraw xmm1,1
	movq xmm2,[rbp+rcx]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [rbp+rcx],xmm2
	movq xmm2,[rdi+8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [rdi+8],xmm2
	movq xmm2,[rdi+rcx+8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [rdi+rcx+8],xmm2
	psraw xmm0,1
	psraw xmm1,1
	movq xmm2,[rbp+rcx+8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [rbp+rcx+8],xmm2

	lea rbp,[rbp+16]
	lea rdi,[rdi+16]
	inc rsi
	inc rdx

	align 16
y2rflop3b:
	movzx rax,byte [rsi]
	movq xmm0,[rbx+rax*8 + 2048]
	movzx rax,byte [rdx]
	movq xmm1,[rbx+rax*8 + 4096]

	movq xmm2,[rdi]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [rdi],xmm2
	movq xmm2,[rdi+rcx]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [rdi+rcx],xmm2
	psraw xmm0,1
	psraw xmm1,1
	movq xmm2,[rbp+rcx]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [rbp+rcx],xmm2
	movq xmm2,[rdi+8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [rdi+8],xmm2
	movq xmm2,[rdi-8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [rdi-8],xmm2
	movq xmm2,[rdi+rcx+8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [rdi+rcx+8],xmm2
	movq xmm2,[rdi+rcx-8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [rdi+rcx-8],xmm2
	psraw xmm0,1
	psraw xmm1,1
	movq xmm2,[rbp+rcx+8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [rbp+rcx+8],xmm2
	movq xmm2,[rbp+rcx-8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [rbp+rcx-8],xmm2

	lea rbp,[rbp+16]
	lea rdi,[rdi+16]
	inc rsi
	inc rdx
	dec qword [rsp+8] ;widthLeft
	jnz y2rflop3b

	movzx rax,byte [rsi]
	movq xmm0,[rbx+rax*8 + 2048]
	movzx rax,byte [rdx]
	movq xmm1,[rbx+rax*8 + 4096]

	movq xmm2,[rdi]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [rdi],xmm2
	movq xmm2,[rdi+8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [rdi+8],xmm2
	movq xmm2,[rdi+rcx]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [rdi+rcx],xmm2
	movq xmm2,[rdi+rcx+8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [rdi+rcx+8],xmm2
	psraw xmm0,1
	psraw xmm1,1
	movq xmm2,[rbp+rcx]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [rbp+rcx],xmm2
	movq xmm2,[rbp+rcx+8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [rbp+rcx+8],xmm2
	movq xmm2,[rdi-8]
	movq [rdi-8],xmm2
	psraw xmm0,1
	psraw xmm1,1
	movq xmm2,[rdi+rcx-8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [rdi+rcx-8],xmm2
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq xmm2,[rbp+rcx-8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [rbp+rcx-8],xmm2

	lea rbp,[rbp+16]
	lea rdi,[rdi+16]
	add rsi,2
	add rdx,2
	add rsi,qword [rsp+168] ;uvAdd
	add rdx,qword [rsp+168] ;uvAdd
	mov r11,rsi ;uPtr
	mov r8,rdx ;vPtr

	mov rdi,r9 ;dest
	mov rcx,qword [rsp+104] ;width
	mov rsi,qword [rsp+144] ;csLineBuff
	mov rbx,qword [rsp+184] ;rgbGammaCorr
	align 16
y2rflop5b:
	movzx rax,word [rsi+4]
	movq xmm7,[rbx+rax*8]
	movzx rax,word [rsi+2]
	movq xmm6,[rbx+rax*8+524288]
	paddsw xmm7,xmm6
	movzx rax,word [rsi]
	movq xmm6,[rbx+rax*8+1048576]
	paddsw xmm7,xmm6
	movq [rdi],xmm7
	add rsi,8
	add rdi,8
	dec rcx
	jnz y2rflop5b

	add r9,qword [rsp+120] ;dbpl dest

	mov rdi,r9 ;dest
	mov rcx,qword [rsp+104] ;width
	align 16
y2rflop6b:
	movzx rax,word [rsi+4]
	movq xmm7,[rbx+rax*8]
	movzx rax,word [rsi+2]
	movq xmm6,[rbx+rax*8+524288]
	paddsw xmm7,xmm6
	movzx rax,word [rsi]
	movq xmm6,[rbx+rax*8+1048576]
	paddsw xmm7,xmm6
	movq [rdi],xmm7
	add rsi,8
	add rdi,8
	dec rcx
	jnz y2rflop6b

	add r9,qword [rsp+120] ;dbpl dest

	mov rsi,qword [rsp+144] ;csLineBuff
	mov rdi,qword [rsp+152] ;csLineBuff2
	mov qword [rsp+152],rsi ;csLineBuff2
	mov qword [rsp+144],rdi ;csLineBuff

	mov rdi,r9 ;dest
	mov rcx,qword [rsp+104] ;width
	mov rsi,qword [rsp+144] ;csLineBuff
	mov rbx,qword [rsp+184] ;rgbGammaCorr
	align 16
y2rflop5c:
	movzx rax,word [rsi+4]
	movq xmm7,[rbx+rax*8]
	movzx rax,word [rsi+2]
	movq xmm6,[rbx+rax*8+524288]
	paddsw xmm7,xmm6
	movzx rax,word [rsi]
	movq xmm6,[rbx+rax*8+1048576]
	paddsw xmm7,xmm6
	movq [rdi],xmm7
	add rsi,8
	add rdi,8
	dec rcx
	jnz y2rflop5c

	add r9,qword [rsp+120] ;dbpl dest

	mov rdi,r9 ;dest
	mov rcx,qword [rsp+104] ;width
	align 16
y2rflop6c:
	movzx rax,word [rsi+4]
	movq xmm7,[rbx+rax*8]
	movzx rax,word [rsi+2]
	movq xmm6,[rbx+rax*8+524288]
	paddsw xmm7,xmm6
	movzx rax,word [rsi]
	movq xmm6,[rbx+rax*8+1048576]
	paddsw xmm7,xmm6
	movq [rdi],xmm7
	add rsi,8
	add rdi,8
	dec rcx
	jnz y2rflop6c

	add r9,qword [rsp+120] ;dbpl dest

	mov rsi,qword [rsp+144] ;csLineBuff
	mov rdi,qword [rsp+152] ;csLineBuff2
	mov qword [rsp+152],rsi ;csLineBuff2
	mov qword [rsp+144],rdi ;csLineBuff

	align 16
yv2rflopexit:
	add rsp,32
	pop rdi
	pop rsi
	pop rbx
	pop rbp
	ret
