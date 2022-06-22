section .text

global CSYUV420_RGB32C_VerticalFilterLRGB
global _CSYUV420_RGB32C_VerticalFilterLRGB
global CSYUV420_RGB32C_do_yv12rgb8
global _CSYUV420_RGB32C_do_yv12rgb8
global CSYUV420_RGB32C_do_yv12rgb2
global _CSYUV420_RGB32C_do_yv12rgb2

;void CSYUV420_LRGBC_VerticalFilterLRGB(UInt8 *inYPt, UInt8 *inUPt, UInt8 *inVPt, UInt8 *outPt, OSInt width, OSInt height, OSInt tap, OSInt *index, Int64 *weight, Int32 isFirst, Int32 isLast, UInt8 *csLineBuff, UInt8 *csLineBuff2, OSInt ystep, OSInt dstep, Int64 *yuv2rgb, Int64 *rgbGammaCorr)
; xmm8 tmpV
; 0 xmm8
; 16 cSub
; 24 sWidth
; 32 width
; 40 height
; 48 rbx
; 56 rbp
; 64 retAddr
; rdi inYPt r10
; rsi inUPt r11
; rdx inVPt r8
; rcx outPt r9
; r8 width
; r9 height / currHeight
; 72 tap
; 80 index
; 88 weight
; 96 isFirst
; 104 isLast
; 112 csLineBuff
; 120 csLineBuff2
; 128 ystep / yAdd
; 136 dstep
; 144 yuv2rgb
; 152 rgbGammaCorr

	align 16
CSYUV420_RGB32C_VerticalFilterLRGB:
_CSYUV420_RGB32C_VerticalFilterLRGB:
	mov eax,32768
	push rbp
	push rbx
	sub rsp,48
	movdqu [rsp+0],xmm8
	mov qword [rsp+32],r8 ;width
	mov qword [rsp+40],r9 ;height
	
	movd xmm0,eax
	punpckldq xmm0,xmm0
	punpckldq xmm0,xmm0
	movdqa xmm8,xmm0						;Int32 tmpV[4] = {32768, 32768, 32768, 32768};
	mov r10,rdi ;inYPt
	mov r11,rsi ;inUPt
	mov r8,rdx ;inVPt
	mov r9,rcx ;outPt
	mov rdx,qword [rsp+32] ;width
	mov rax,rdx
	mov rcx,rdx
	sub qword [rsp+128],rdx			;OSInt yAdd = ystep - width;
	shr rax,1
	shr rcx,3
	sub rax,2
	mov qword [rsp+24],rcx ;sWidth	OSInt sWidth = width >> 3;
	mov qword [rsp+16],rax ;cSub	OSInt cSub = (width >> 1) - 2;
	mov rcx,qword [rsp+72] ;tap
	cmp rcx,4
	jnz vflexit						; if (tap == 4)
	and rdx,7
	jz vflstart
									; if (width & 7)
	shr rdx,2
	mov qword [rsp+96],rdx			; OSInt widthLeft = (width & 7) >> 2;

	align 16
vf7lop:
	mov rbx,qword [rsp+88] ;weight
	mov rsi,qword [rsp+120] ;csLineBuff2
	mov rcx,r11 ;inUPt
	mov rdx,r8 ;inVPt

	movdqa xmm5,[rbx+16]
	movdqa xmm6,[rbx+32]

	mov rbp,qword [rsp+24] ;sWidth
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
	movd xmm0,[rdx+rax]
	punpcklbw xmm4,xmm0
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

	mov rbp,qword [rsp+96] ;widthLeft
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

	align 16
vf7lop3b:
	mov rbp,qword [rsp+32] ;width
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
	mov rsi,r10 ;inYPt
	mov rbp,qword [rsp+16] ;cSub
	mov rdi,qword [rsp+112] ;csLineBuff
	mov rcx,qword [rsp+120] ;csLineBuff2
	mov rbx,qword [rsp+144] ;yuv2rgb

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
	add rsi,qword [rsp+128] ;yAdd
	mov r10,rsi ;inYPt

	mov rdi,r9 ;outPt
	mov rcx,qword [rsp+32] ;width
	mov rsi,qword [rsp+112] ;csLineBuff
	mov rbx,qword [rsp+152] ;rgbGammaCorr
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
	pextrw rax,xmm5,0
	mov edx,dword [rbx+rax*4+1572864]
	pextrw rax,xmm5,1
	or edx,dword [rbx+rax*4+1835008]
	pextrw rax,xmm5,2
	or edx,dword [rbx+rax*4+2097152]
	movnti dword [rdi],edx
	lea rsi,[rsi+8]
	lea rdi,[rdi+4]
	dec rcx
	jnz vf7lop5

	add qword [rsp+88],48 ;weight
	add r9,qword [rsp+136] ;dstep

	dec	qword [rsp+40] ;currHeight
	jnz vf7lop
	jmp vflexit

	align 16
vflstart:
	sub rsp,32
	mov qword [rsp],r12
	mov qword [rsp+8],r13
	mov qword [rsp+16],r14
	mov qword [rsp+24],r15
	xor r12,r12
	xor r13,r13
	xor r14,r14
	xor r15,r15

	ALIGN 16
vflop:
	mov rbx,qword [rsp+120] ;weight
	mov rsi,qword [rsp+152] ;csLineBuff2
	mov rcx,r11 ;inUPt
	mov rdx,r8 ;inVPt

	mov r12d,dword [rbx]
	mov r13d,dword [rbx+4]
	mov r14d,dword [rbx+8]
	mov r15d,dword [rbx+12]
	movdqa xmm5,[rbx+16]
	movdqa xmm6,[rbx+32]

	add qword [rsp+120],48 ;weight

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
	mov rdi,qword [rsp+144] ;csLineBuff
	mov rcx,qword [rsp+152] ;csLineBuff2
	mov rbx,qword [rsp+176] ;yuv2rgb

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
	movdqa [rdi],xmm2

	movzx rdx,word [rsi]
	movzx eax,dl
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

	movzx rdx,word [rsi]
	movzx rax,dl
	movq xmm2,[rbx+rax * 8]
	shr rdx,8
	movq xmm4,[rbx+rdx * 8]
	punpcklqdq xmm2,xmm4
	paddsw xmm2,xmm0
	movdqa [rdi+16],xmm2

	add rsi,qword [rsp+160] ;yAdd
	lea r10,[rsi+2] ;inYPt

	mov rdi,r9 ;outPt
	mov rbp,qword [rsp+64] ;width
	mov rsi,qword [rsp+144] ;csLineBuff
	mov rbx,qword [rsp+184] ;rgbGammaCorr
	shr rbp,1
	ALIGN 16
y2rllop5:
	movdqa xmm1,[rsi]
	pextrw rax,xmm1,2
	movq xmm4,[rbx+rax*8]
	pextrw rax,xmm1,6
	movq xmm0,[rbx+rax*8]

	pextrw rax,xmm1,1
	movq xmm6,[rbx+rax*8+524288]
	pextrw rax,xmm1,5
	movq xmm5,[rbx+rax*8+524288]
	paddsw xmm4,xmm6
	paddsw xmm0,xmm5

	pextrw rax,xmm1,0
	movq xmm6,[rbx+rax*8+1048576]
	pextrw rax,xmm1,4
	movq xmm5,[rbx+rax*8+1048576]
	paddsw xmm4,xmm6
	paddsw xmm0,xmm5

	pextrw rax,xmm4,0
	mov edx,dword [rbx+rax*4+1572864]
	pextrw rax,xmm0,0
	mov ecx,dword [rbx+rax*4+1572864]
	pextrw rax,xmm4,1
	or edx,dword [rbx+rax*4+1835008]
	pextrw rax,xmm0,1
	or ecx,dword [rbx+rax*4+1835008]
	pextrw rax,xmm4,2
	or edx,dword [rbx+rax*4+2097152]
	pextrw rax,xmm0,2
	or ecx,dword [rbx+rax*4+2097152]
	movnti dword [rdi],edx
	movnti dword [rdi+4],ecx

	lea rsi,[rsi+16]
	lea rdi,[rdi+8]
	dec rbp
	jnz y2rllop5

	add r9,qword [rsp+168] ;dstep

	dec qword [rsp+72] ;currHeight
	jnz vflop
	mov r12,qword [rsp]
	mov r13,qword [rsp+8]
	mov r14,qword [rsp+16]
	mov r15,qword [rsp+24]
	add rsp,32

	align 16
vflexit:
	movdqu xmm8,[rsp+0]
	add rsp,48
	pop rbx
	pop rbp
	ret

;void CSYUV420_LRGBC_do_yv12rgb8(UInt8 *yPtr, UInt8 *uPtr, UInt8 *vPtr, UInt8 *dest, OSInt width, OSInt height, OSInt dbpl, OSInt isFirst, OSInt isLast, UInt8 *csLineBuff, UInt8 *csLineBuff2, OSInt yBpl, OSInt uvBpl, Int64 *yuv2rgb, Int64 *rgbGammaCorr)
;0 width
;8 heightLeft
;16 widthLeft
;24 cSub
;32 cSize
;40 rbx
;48 rbp
;56 retAddr
;rdi yPtr r10
;rsi uPtr r11
;rdx r8 vPtr
;rcx r9 dest
;r8 width
;r9 height
;64 dbpl
;72 isFirst
;80 isLast
;88 csLineBuff
;96 csLineBuff2
;104 yBpl / yAdd
;112 uvBpl / uvAdd
;120 yuv2rgb
;128 rgbGammaCorr

	align 16
CSYUV420_RGB32C_do_yv12rgb8:
_CSYUV420_RGB32C_do_yv12rgb8:
	push rbp
	push rbx
	sub rsp,40
	mov r10,rdi ;yPtr
	mov r11,rsi ;uPtr
	lea rax,[r8*8] ;width
	sub qword [rsp+104],r8 ;yAdd					Int32 yAdd = yBpl - width;
	mov qword [rsp+32],rax ;cSize					Int32 cSize = width << 3;
	mov rax,r8
	shr rax,1
	sub qword [rsp+112],rax ;uvAdd					Int32 uvAdd = uvBpl - (width >> 1);
	sub rax,2
	mov qword [rsp+24],rax ;cSub						Int32 cSub = (width >> 1) - 2;

	shr r9,1 ;height
	mov rbx,qword [rsp+80] ;isLast
	and rbx,1
	shl rbx,1
	sub r9,rbx
	mov qword [rsp+8],r9 ;heightLeft
	mov qword [rsp+0],r8 ;width
	mov r8,rdx ;vPtr
	mov r9,rcx ;dest

	mov rdi,qword [rsp+88] ;csLineBuff
	mov rbx,qword [rsp+120] ;yuv2rgb
	mov rsi,r10 ;yPtr

	mov rcx,qword [rsp+0] ;width
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

	mov rcx,qword [rsp+0] ;width
	add rsi,qword [rsp+104] ;yAdd
	shr rcx,3
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
	add rsi,qword [rsp+104] ;yAdd
	mov r10,rsi ;yPtr

	mov rcx,qword [rsp+24] ;cSub
	mov rdx,qword [rsp+32] ;cSize
	mov rdi,qword [rsp+88] ;csLineBuff
	mov rsi,r11 ;uPtr
	shr rcx,1
	mov qword [rsp+16],rcx ;widthLeft
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
	dec qword [rsp+16] ;widthLeft
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
	mov rbp,qword [rsp+112] ;uvAdd
	lea r11,[rsi+rbp] ;uPtr
	lea r8,[rcx+rbp] ;vPtr

	align 16
y2r8flop:

	mov rdi,qword [rsp+96] ;csLineBuff2
	mov rbx,qword [rsp+120] ;yuv2rgb
	mov rsi,r10 ;yPtr

	mov rcx,qword [rsp+0] ;width
	shr rcx,1
	align 16
y2r8flop2:
	movzx rax,byte [rsi]
	movq xmm0,[rbx+rax*8]
	movzx rax,byte [rsi+1]
	movq xmm1,[rbx+rax*8]
	punpcklqdq xmm0,xmm1
	movdqa [rdi],xmm0
	lea rsi,[rsi+2]
	lea rdi,[rdi+16]
	dec rcx
	jnz y2r8flop2

	add rsi,qword [rsp+104] ;yAdd

	mov rcx,qword [rsp+0] ;width
	shr rcx,1
	align 16
y2r8flop2_2:
	movzx rax,byte [rsi]
	movq xmm0,[rbx+rax*8]
	movzx rax,byte [rsi+1]
	movq xmm1,[rbx+rax*8]
	punpcklqdq xmm0,xmm1
	movdqa [rdi],xmm0
	lea rsi,[rsi+2]
	lea rdi,[rdi+16]
	dec rcx
	jnz y2r8flop2_2

	add rsi,qword [rsp+104] ;yAdd
	mov r10,rsi ;yPtr

	mov rcx,qword [rsp+24] ;cSub
	mov rbp,qword [rsp+88] ;csLineBuff
	mov rdi,qword [rsp+96] ;csLineBuff2
	mov rdx,r8 ;vPtr
	mov rsi,r11 ;uPtr
	shr rcx,1
	mov qword [rsp+16],rcx ;widthLeft
	mov rcx,qword [rsp+32] ;cSize

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
	dec qword [rsp+16] ;widthLeft
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
	add rsi,qword [rsp+112] ;uvAdd
	add rdx,qword [rsp+112] ;uvAdd
	mov r11,rsi ;uPtr
	mov r8,rdx ;vPtr

	mov rcx,qword [rsp+0] ;width
	mov rsi,qword [rsp+88] ;csLineBuff
	mov rdi,r9 ;dest
	mov rbx,qword [rsp+128] ;rgbGammaCorr
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
	pextrw rax,xmm7,0
	mov edx,dword [rbx+rax*4+1572864]
	pextrw rax,xmm7,1
	or edx,dword [rbx+rax*4+1835008]
	pextrw rax,xmm7,2
	or edx,dword [rbx+rax*4+2097152]
	movnti dword [rdi],edx
	lea rsi,[rsi+8]
	lea rdi,[rdi+4]
	dec rcx
	jnz y2r8flop5

	add r9,qword [rsp+64] ;dbpl

	mov rcx,qword [rsp+0] ;width
	mov rdi,r9 ;dest
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
	pextrw rax,xmm7,0
	mov edx,dword [rbx+rax*4+1572864]
	pextrw rax,xmm7,1
	or edx,dword [rbx+rax*4+1835008]
	pextrw rax,xmm7,2
	or edx,dword [rbx+rax*4+2097152]
	movnti dword [rdi],edx
	lea rsi,[rsi+8]
	lea rdi,[rdi+4]
	dec rcx
	jnz y2r8flop6

	add r9,qword [rsp+64] ;dbpl
	mov rax,qword [rsp+88] ;csLineBuff
	xchg rax,qword [rsp+96] ;csLineBuff2
	mov qword [rsp+88],rax ;;csLineBuff

	dec qword [rsp+8] ;heightLeft
	jnz y2r8flop

	test qword [rsp+80],1 ;isLast
	jz yv2r8flopexit

	mov rdi,qword [rsp+96] ;csLineBuff2
	mov rbx,qword [rsp+120] ;yuv2rgb
	mov rsi,r10 ;yPtr

	mov rcx,qword [rsp+0] ;width
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
	add rsi,qword [rsp+104] ;yAdd

	mov rcx,qword [rsp+0] ;width
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
	add r10,qword [rsp+104] ;yAdd

	mov rbp,qword [rsp+88] ;csLineBuff
	mov rcx,qword [rsp+24] ;cSub
	mov rdi,qword [rsp+96] ;csLineBuff2
	mov rdx,r8 ;vPtr
	mov rsi,r11 ;uPtr
	shr rcx,1
	mov qword [rsp+16],rcx ;widthLeft
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
	dec qword [rsp+16] ;widthLeft
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
	add rsi,qword [rsp+112] ;uvAdd
	add rdx,qword [rsp+112] ;uvAdd
	mov r11,rsi ;uPtr
	mov r8,rdx ;vPtr

	mov rcx,qword [rsp+0] ;width
	mov rsi,qword [rsp+88] ;csLineBuff
	mov rbx,qword [rsp+128] ;rgbGammaCorr
	mov rdi,r9 ;dest
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
	pextrw rax,xmm7,0
	mov edx,dword [rbx+rax*4+1572864]
	pextrw rax,xmm7,1
	or edx,dword [rbx+rax*4+1835008]
	pextrw rax,xmm7,2
	or edx,dword [rbx+rax*4+2097152]
	movnti dword [rdi],edx
	lea rsi,[rsi+8]
	lea rdi,[rdi+4]
	dec rcx
	jnz y2r8flop5b

	add r9,qword [rsp+64] ;dbpl

	mov rcx,qword [rsp+0] ;width
	mov rdi,r9 ;dest
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
	pextrw rax,xmm7,0
	mov edx,dword [rbx+rax*4+1572864]
	pextrw rax,xmm7,1
	or edx,dword [rbx+rax*4+1835008]
	pextrw rax,xmm7,2
	or edx,dword [rbx+rax*4+2097152]
	movnti dword [rdi],edx
	lea rsi,[rsi+8]
	lea rdi,[rdi+4]
	dec rcx
	jnz y2r8flop6b

	add r9,qword [rsp+64] ;dbpl

	mov rax,qword [rsp+88] ;csLineBuff
	xchg rax,qword [rsp+96] ;csLineBuff2
	mov qword [rsp+88],rax ;csLineBuff

	mov rdi,r9 ;dest
	mov rcx,qword [rsp+0] ;width
	mov rsi,qword [rsp+88] ;csLineBuff
	mov rbx,qword [rsp+128] ;rgbGammaCorr
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
	pextrw rax,xmm7,0
	mov edx,dword [rbx+rax*4+1572864]
	pextrw rax,xmm7,1
	or edx,dword [rbx+rax*4+1835008]
	pextrw rax,xmm7,2
	or edx,dword [rbx+rax*4+2097152]
	movnti dword [rdi],edx
	lea rsi,[rsi+8]
	lea rdi,[rdi+4]
	dec rcx
	jnz y2r8flop5c

	add r9,qword [rsp+64] ;dbpl

	mov rcx,qword [rsp+0] ;width
	mov rdi,r9 ;dest
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
	pextrw rax,xmm7,0
	mov edx,dword [rbx+rax*4+1572864]
	pextrw rax,xmm7,1
	or edx,dword [rbx+rax*4+1835008]
	pextrw rax,xmm7,2
	or edx,dword [rbx+rax*4+2097152]
	movnti dword [rdi],edx
	lea rsi,[rsi+8]
	lea rdi,[rdi+4]
	dec rcx
	jnz y2r8flop6c

	add r9,qword [rsp+64] ;dbpl

	mov rax,qword [rsp+88] ;csLineBuff
	xchg rax,qword [rsp+96] ;csLineBuff2
	mov qword [rsp+88],rax ;csLineBuff

	align 16
yv2r8flopexit:
	add rsp,40
	pop rbx
	pop rbp
	ret

;void CSYUV420_LRGBC_do_yv12rgb2(UInt8 *yPtr, UInt8 *uPtr, UInt8 *vPtr, UInt8 *dest, OSInt width, OSInt height, OSInt dbpl, OSInt isFirst, OSInt isLast, UInt8 *csLineBuff, UInt8 *csLineBuff2, OSInt yBpl, OSInt uvBpl, Int64 *yuv2rgb, Int64 *rgbGammaCorr)
;0 widthLeft
;4 cSub
;8 cSize
;12 edi
;16 esi
;20 ebx
;24 ebp
;28 retAddr
;32 yPtr
;36 uPtr
;40 vPtr
;44 dest
;48 width
;52 height
;56 dbpl
;60 isFirst
;64 isLast
;68 csLineBuff
;72 csLineBuff2
;76 yBpl / yAdd
;80 uvBpl / uvAdd
;84 yuv2rgb
;88 rgbGammaCorr

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	align 16
CSYUV420_RGB32C_do_yv12rgb2:
_CSYUV420_RGB32C_do_yv12rgb2:
	ret
	push rbp
	push rbx
	push rsi
	push rdi
	mov edx,dword [esp+48] ;width
	lea ecx,[edx-2]
	lea eax,[edx*8]
	sub dword [esp+76],ecx ;yAdd					Int32 yAdd = yBpl - width - 2;
	shr edx,1
	inc edx
	mov dword [esp+8],eax ;cSize					Int32 cSize = width << 3;
	sub dword [esp+80],edx ;uvAdd					Int32 uvAdd = uvBpl - ((width + 2) >> 1);
	sub edx,2
	mov dword [esp+4],edx ;cSub						Int32 cSub = (width >> 1) - 2;

	mov ecx,dword [esp+52] ;height
	shr ecx,1
	mov ebx,dword [esp+64] ;isLast
	and ebx,1
	shl ebx,1
	sub ecx,ebx
	mov dword [esp+52],ecx ;heightLeft

	mov ecx,dword [esp+48] ;width
	shr ecx,2
	mov edi,dword [esp+68] ;csLineBuff
	mov ebx,dword [esp+84] ;yuv2rgb
	mov esi,dword [esp+32] ;yPtr

	mov ebp,ecx
	align 16
y2rflop2a:
	movzx eax,byte [esi]
	movq xmm0,[ebx+eax * 8]
	movq [edi],xmm0
	movzx eax,byte [esi+1]
	movq xmm0,[ebx+eax * 8]
	movq [edi+8],xmm0
	movzx eax,byte [esi+2]
	movq xmm0,[ebx+eax * 8]
	movq [edi+16],xmm0
	movzx eax,byte [esi+3]
	movq xmm0,[ebx+eax * 8]
	movq [edi+24],xmm0

	add esi,4
	add edi,32
	dec ecx
	jnz y2rflop2a
	movzx eax,byte [esi]
	movq xmm0,[ebx+eax * 8]
	movq [edi],xmm0
	movzx eax,byte [esi+1]
	movq xmm0,[ebx+eax * 8]
	movq [edi+8],xmm0
	add esi,4
	add edi,16
	add esi,dword [esp+76] ;yAdd
	mov ecx,ebp

	align 16
y2rflop2c:
	movzx eax,byte [esi]
	movq xmm0,[ebx+eax * 8]
	movq [edi],xmm0
	movzx eax,byte [esi+1]
	movq xmm0,[ebx+eax * 8]
	movq [edi+8],xmm0
	movzx eax,byte [esi+2]
	movq xmm0,[ebx+eax * 8]
	movq [edi+16],xmm0
	movzx eax,byte [esi+3]
	movq xmm0,[ebx+eax * 8]
	movq [edi+24],xmm0

	add esi,4
	add edi,32
	dec ecx
	jnz y2rflop2c
	movzx eax,byte [esi]
	movq xmm0,[ebx+eax * 8]
	movq [edi],xmm0
	movzx eax,byte [esi+1]
	movq xmm0,[ebx+eax * 8]
	movq [edi+8],xmm0
	add esi,4
	add edi,16
	add esi,dword [esp+76] ;yAdd
	mov dword [esp+32],esi ;yPtr

	mov ecx,dword [esp+4] ;cSub
	mov edi,dword [esp+68] ;csLineBuff
	mov edx,dword [esp+8] ;cSize
	mov esi,dword [esp+36] ;uPtr
	mov dword [esp+0],ecx ;widthLeft
	mov ecx,dword [esp+40] ;vPtr


	movzx eax,byte [esi]
	movq xmm0,[ebx+eax*8 + 2048]
	movzx eax,byte [ecx]
	movq xmm1,[ebx+eax*8 + 4096]

	movq xmm2,[edi]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [edi],xmm2
	psraw xmm0,1
	psraw xmm1,1
	movq xmm2,[edi+edx]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [edi+edx],xmm2
	movq xmm2,[edi+8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [edi+8],xmm2
	psraw xmm0,1
	psraw xmm1,1
	movq xmm2,[edi+edx+8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [edi+edx+8],xmm2

	add edi,16
	inc esi
	inc ecx

	align 16
y2rflop3a:
	movzx eax,byte [esi]
	movq xmm0,[ebx+eax*8 + 2048]
	movzx eax,byte [ecx]
	movq xmm1,[ebx+eax*8 + 4096]

	movq xmm2,[edi]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [edi],xmm2
	psraw xmm0,1
	psraw xmm1,1
	movq xmm2,[edi+edx]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [edi+edx],xmm2
	movq xmm2,[edi+8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [edi+8],xmm2
	movq xmm2,[edi-8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [edi-8],xmm2
	psraw xmm0,1
	psraw xmm1,1
	movq xmm2,[edi+edx+8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [edi+edx+8],xmm2
	movq xmm2,[edi+edx-8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [edi+edx-8],xmm2

	add edi,16
	inc esi
	inc ecx
	dec dword [esp+0] ;widthLeft
	jnz y2rflop3a


	movzx eax,byte [esi]
	movq xmm0,[ebx+eax*8 + 2048]
	movzx eax,byte [ecx]
	movq xmm1,[ebx+eax*8 + 4096]

	movq xmm2,[edi]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [edi],xmm2
	movq xmm2,[edi+8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [edi+8],xmm2
	psraw xmm0,1
	psraw xmm1,1
	movq xmm2,[edi+edx]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [edi+edx],xmm2
	movq xmm2,[edi-8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [edi-8],xmm2
	movq xmm2,[edi+edx+8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [edi+edx+8],xmm2
	psraw xmm0,1
	psraw xmm1,1
	movq xmm2,[edi+edx-8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [edi+edx-8],xmm2

	add edi,16
	add esi,2
	add ecx,2
	add esi,dword [esp+80] ;uvAdd
	add ecx,dword [esp+80] ;uvAdd
	mov dword [esp+36],esi ;uPtr
	mov dword [esp+40],ecx ;vPtr
	align 16
y2rflop:

	mov ecx,dword [esp+48] ;width
	mov edi,dword [esp+72] ;csLineBuff2
	mov ebx,dword [esp+84] ;yuv2rgb
	mov esi,dword [esp+32] ;yPtr

	align 16
y2rflop2:
	movzx eax,byte [esi]
	movq xmm0,[ebx+eax*8]
	movq [edi],xmm0
	inc esi
	add edi,8
	dec ecx
	jnz y2rflop2
	add esi,2
	add esi,dword [esp+76] ;yAdd

	mov ecx,dword [esp+48] ;width
	align 16
y2rflop2d:
	movzx eax,byte [esi]
	movq xmm0,[ebx+eax*8]
	movq [edi],xmm0
	inc esi
	add edi,8
	dec ecx
	jnz y2rflop2d
	add esi,2
	add esi,dword [esp+76] ;yAdd

	mov dword [esp+32],esi ;yPtr

	mov ebp,dword [esp+68] ;csLineBuff
	mov ecx,dword [esp+4] ;cSub
	mov edi,dword [esp+72] ;csLineBuff2
	mov edx,dword [esp+40] ;vPtr
	mov esi,dword [esp+36] ;uPtr
	mov dword [esp+0],ecx ;widthLeft
	mov ecx,dword [esp+8] ;cSize

	movzx eax,byte [esi]
	movq xmm0,[ebx+eax*8 + 2048]
	movzx eax,byte [edx]
	movq xmm1,[ebx+eax*8 + 4096]

	movq xmm2,[edi]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [edi],xmm2
	psraw xmm0,1
	psraw xmm1,1
	movq xmm2,[edi+ecx]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [edi+ecx],xmm2
	movq xmm2,[ebp+ecx]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [ebp+ecx],xmm2
	movq xmm2,[edi+8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [edi+8],xmm2
	psraw xmm0,1
	psraw xmm1,1
	movq xmm2,[edi+ecx+8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [edi+ecx+8],xmm2
	movq xmm2,[ebp+ecx+8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [ebp+ecx+8],xmm2

	lea ebp,[ebp+16]
	lea edi,[edi+16]
	inc esi
	inc edx

	align 16
y2rflop3:
	movzx eax,byte [esi]
	movq xmm0,[ebx+eax*8 + 2048]
	movzx eax,byte [edx]
	movq xmm1,[ebx+eax*8 + 4096]

	movq xmm2,[edi]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [edi],xmm2
	psraw xmm0,1
	psraw xmm1,1
	movq xmm2,[edi+ecx]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [edi+ecx],xmm2
	movq xmm2,[ebp+ecx]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [ebp+ecx],xmm2
	movq xmm2,[edi+8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [edi+8],xmm2
	movq xmm2,[edi-8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [edi-8],xmm2
	psraw xmm0,1
	psraw xmm1,1
	movq xmm2,[edi+ecx+8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [edi+ecx+8],xmm2
	movq xmm2,[ebp+ecx+8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [ebp+ecx+8],xmm2
	movq xmm2,[edi+ecx-8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [edi+ecx-8],xmm2
	movq xmm2,[ebp+ecx-8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [ebp+ecx-8],xmm2

	lea ebp,[ebp+16]
	lea edi,[edi+16]
	inc esi
	inc edx
	dec dword [esp+0] ;widthLeft
	jnz y2rflop3

	movzx eax,byte [esi]
	movq xmm0,[ebx+eax*8 + 2048]
	movzx eax,byte [edx]
	movq xmm1,[ebx+eax*8 + 4096]

	movq xmm2,[edi]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [edi],xmm2
	movq xmm2,[edi+8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [edi+8],xmm2
	psraw xmm0,1
	psraw xmm1,1
	movq xmm2,[edi+ecx]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [edi+ecx],xmm2
	movq xmm2,[ebp+ecx]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [ebp+ecx],xmm2
	movq xmm2,[edi+ecx+8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [edi+ecx+8],xmm2
	movq xmm2,[ebp+ecx+8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [ebp+ecx+8],xmm2
	movq xmm2,[edi-8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [edi-8],xmm2
	psraw xmm0,1
	psraw xmm1,1
	movq xmm2,[edi+ecx-8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [edi+ecx-8],xmm2
	movq xmm2,[ebp+ecx-8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [ebp+ecx-8],xmm2

	lea ebp,[ebp+16]
	add edi,16
	add esi,2
	add edx,2
	add esi,dword [esp+80] ;uvAdd
	add edi,dword [esp+80] ;uvAdd
	mov dword [esp+36],esi ;uPtr
	mov dword [esp+40],edx ;vPtr

	mov ecx,dword [esp+48] ;width
	mov esi,dword [esp+68] ;csLineBuff
	mov edi,dword [esp+44] ;dest
	mov ebx,dword [esp+88] ;rgbGammaCorr
	align 16
y2rflop5:
	movzx eax,word [esi+4]
	movq xmm7,[ebx+eax*8]
	movzx eax,word [esi+2]
	movq xmm6,[ebx+eax*8+524288]
	paddsw xmm7,xmm6
	movzx eax,word [esi]
	movq xmm6,[ebx+eax*8+1048576]
	paddsw xmm7,xmm6
	movq [edi],xmm7
	add esi,8
	add edi,8
	dec ecx
	jnz y2rflop5

	mov eax,dword [esp+56] ;dbpl
	add dword [esp+44],eax ;dest

	mov ecx,dword [esp+48] ;width
	mov edi,dword [esp+44] ;dest
	align 16
y2rflop6:
	movzx eax,word [esi+4]
	movq xmm7,[ebx+eax*8]
	movzx eax,word [esi+2]
	movq xmm6,[ebx+eax*8+524288]
	paddsw xmm7,xmm6
	movzx eax,word [esi]
	movq xmm6,[ebx+eax*8+1048576]
	paddsw xmm7,xmm6
	movq [edi],xmm7
	add esi,8
	add edi,8
	dec ecx
	jnz y2rflop6

	mov eax,dword [esp+56] ;dbpl
	add dword [esp+44],eax ;dest

	mov eax,dword [esp+68] ;csLineBuff
	xchg eax,dword [esp+72] ;csLineBuff2
	mov dword [esp+68],eax ;csLineBuff

	dec dword [esp+52] ;heightLeft
	jnz y2rflop

	test dword [esp+64],1 ;isLast
	jz yv2rflopexit

	mov ecx,dword [esp+48] ;width
	mov edi,dword [esp+72] ;csLineBuff2
	mov ebx,dword [esp+84] ;yuv2rgb
	mov esi,dword [esp+32] ;yPtr

	align 16
y2rflop2b:
	movzx eax,byte [esi]
	movq xmm0,[ebx+eax*8]
	movq [edi],xmm0
	inc esi
	add edi,8
	dec ecx
	jnz y2rflop2b
	inc esi

	mov ecx,dword [esp+48] ;width
	align 16
y2rflop2e:
	movzx eax,byte [esi]
	movq xmm0,[ebx+eax*8]
	movq [edi],xmm0
	inc esi
	add edi,8
	dec ecx
	jnz y2rflop2e
	inc esi
	add esi,dword [esp+76] ;yAdd
	mov dword [esp+32],esi ;yPtr

	mov ebp,dword [esp+68] ;csLineBuff
	mov ecx,dword [esp+4] ;cSub
	mov edi,dword [esp+72] ;csLineBuff2
	mov edx,dword [esp+40] ;vPtr
	mov esi,dword [esp+36] ;uPtr
	mov dword [esp+0],ecx ;widthLeft
	mov ecx,dword [esp+8] ;cSize

	movzx eax,byte [esi]
	movq xmm0,[ebx+eax*8 + 2048]
	movzx eax,byte [edx]
	movq xmm1,[ebx+eax*8 + 4096]

	movq xmm2,[edi]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [edi],xmm2
	movq xmm2,[edi+ecx]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [edi+ecx],xmm2
	psraw xmm0,1
	psraw xmm1,1
	movq xmm2,[ebp+ecx]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [ebp+ecx],xmm2
	movq xmm2,[edi+8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [edi+8],xmm2
	movq xmm2,[edi+ecx+8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [edi+ecx+8],xmm2
	psraw xmm0,1
	psraw xmm1,1
	movq xmm2,[ebp+ecx+8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [ebp+ecx+8],xmm2

	lea ebp,[ebp+16]
	lea edi,[edi+16]
	inc esi
	inc edx

	align 16
y2rflop3b:
	movzx eax,byte [esi]
	movq xmm0,[ebx+eax*8 + 2048]
	movzx eax,byte [edx]
	movq xmm1,[ebx+eax*8 + 4096]

	movq xmm2,[edi]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [edi],xmm2
	movq xmm2,[edi+ecx]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [edi+ecx],xmm2
	psraw xmm0,1
	psraw xmm1,1
	movq xmm2,[ebp+ecx]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [ebp+ecx],xmm2
	movq xmm2,[edi+8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [edi+8],xmm2
	movq xmm2,[edi-8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [edi-8],xmm2
	movq xmm2,[edi+ecx+8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [edi+ecx+8],xmm2
	movq xmm2,[edi+ecx-8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [edi+ecx-8],xmm2
	psraw xmm0,1
	psraw xmm1,1
	movq xmm2,[ebp+ecx+8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [ebp+ecx+8],xmm2
	movq xmm2,[ebp+ecx-8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [ebp+ecx-8],xmm2

	lea ebp,[ebp+16]
	lea edi,[edi+16]
	inc esi
	inc edx
	dec dword [esp+0] ;widthLeft
	jnz y2rflop3b

	movzx eax,byte [esi]
	movq xmm0,[ebx+eax*8 + 2048]
	movzx eax,byte [edx]
	movq xmm1,[ebx+eax*8 + 4096]

	movq xmm2,[edi]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [edi],xmm2
	movq xmm2,[edi+8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [edi+8],xmm2
	movq xmm2,[edi+ecx]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [edi+ecx],xmm2
	movq xmm2,[edi+ecx+8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [edi+ecx+8],xmm2
	psraw xmm0,1
	psraw xmm1,1
	movq xmm2,[ebp+ecx]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [ebp+ecx],xmm2
	movq xmm2,[ebp+ecx+8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [ebp+ecx+8],xmm2
	movq xmm2,[edi-8]
	movq [edi-8],xmm2
	psraw xmm0,1
	psraw xmm1,1
	movq xmm2,[edi+ecx-8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [edi+ecx-8],xmm2
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq xmm2,[ebp+ecx-8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [ebp+ecx-8],xmm2

	lea ebp,[ebp+16]
	lea edi,[edi+16]
	add esi,2
	add edx,2
	add esi,dword [esp+80] ;uvAdd
	add edx,dword [esp+80] ;uvAdd
	mov dword [esp+36],esi ;uPtr
	mov dword [esp+40],edx ;vPtr

	mov ecx,dword [esp+48] ;width
	mov esi,dword [esp+68] ;csLineBuff
	mov edi,dword [esp+44] ;dest
	mov ebx,dword [esp+88] ;rgbGammaCorr
	align 16
y2rflop5b:
	movzx eax,word [esi+4]
	movq xmm7,[ebx+eax*8]
	movzx eax,word [esi+2]
	movq xmm6,[ebx+eax*8+524288]
	paddsw xmm7,xmm6
	movzx eax,word [esi]
	movq xmm6,[ebx+eax*8+1048576]
	paddsw xmm7,xmm6
	movq [edi],xmm7
	add esi,8
	add edi,8
	dec ecx
	jnz y2rflop5b

	mov eax,dword [esp+56] ;dbpl
	add dword [esp+44],eax ;dest

	mov ecx,dword [esp+48] ;width
	mov edi,dword [esp+44] ;dest
	align 16
y2rflop6b:
	movzx eax,word [esi+4]
	movq xmm7,[ebx+eax*8]
	movzx eax,word [esi+2]
	movq xmm6,[ebx+eax*8+524288]
	paddsw xmm7,xmm6
	movzx eax,word [esi]
	movq xmm6,[ebx+eax*8+1048576]
	paddsw xmm7,xmm6
	movq [edi],xmm7
	add esi,8
	add edi,8
	dec ecx
	jnz y2rflop6b

	mov eax,dword [esp+56] ;dbpl
	add dword [esp+44],eax ;dest

	mov esi,dword [esp+68] ;csLineBuff
	mov edi,dword [esp+72] ;csLineBuff2
	mov dword [esp+72],esi ;csLineBuff2
	mov dword [esp+68],edi ;csLineBuff

	mov ecx,dword [esp+48] ;width
	mov esi,dword [esp+68] ;csLineBuff
	mov edi,dword [esp+44] ;dest
	mov ebx,dword [esp+88] ;rgbGammaCorr
	align 16
y2rflop5c:
	movzx eax,word [esi+4]
	movq xmm7,[ebx+eax*8]
	movzx eax,word [esi+2]
	movq xmm6,[ebx+eax*8+524288]
	paddsw xmm7,xmm6
	movzx eax,word [esi]
	movq xmm6,[ebx+eax*8+1048576]
	paddsw xmm7,xmm6
	movq [edi],xmm7
	add esi,8
	add edi,8
	dec ecx
	jnz y2rflop5c

	mov eax,dword [esp+56] ;dbpl
	add dword [esp+44],eax ;dest

	mov ecx,dword [esp+48] ;width
	mov edi,dword [esp+44] ;dest
	align 16
y2rflop6c:
	movzx eax,word [esi+4]
	movq xmm7,[ebx+eax*8]
	movzx eax,word [esi+2]
	movq xmm6,[ebx+eax*8+524288]
	paddsw xmm7,xmm6
	movzx eax,word [esi]
	movq xmm6,[ebx+eax*8+1048576]
	paddsw xmm7,xmm6
	movq [edi],xmm7
	add esi,8
	add edi,8
	dec ecx
	jnz y2rflop6c

	mov eax,dword [esp+56] ;dbpl
	add dword [esp+44],eax ;dest

	mov esi,dword [esp+68] ;csLineBuff
	mov edi,dword [esp+72] ;csLineBuff2
	mov dword [esp+72],esi ;csLineBuff2
	mov dword [esp+68],edi ;csLineBuff

	align 16
yv2rflopexit:
	pop rdi
	pop rsi
	pop rbx
	pop rbp
	ret
