section .text

global CSNV12_RGB8_do_nv12rgb8
global CSNV12_RGB8_do_nv12rgb2

;void CSNV12_RGB8_do_nv12rgb8(UInt8 *yPtr, UInt8 *uvPtr, UInt8 *dest, OSInt width, OSInt height, OSInt dbpl, OSInt isFirst, OSInt isLast, UInt8 *csLineBuff, UInt8 *csLineBuff2, Int64 *yuv2rgb, UInt8 *rgbGammaCorr);
;-32 cofst
;-24 cWidth4
;-16 cSub
;-8 cSize
;0 rdi
;8 rsi
;16 rbx
;24 rbp
;32 retAddr
;rcx yPtr
;rdx uvPtr
;r8 dest
;r9 width
;72 height
;80 dbpl
;88 isFirst
;96 isLast
;104 csLineBuff
;112 csLineBuff2
;120 yuv2rgb
;128 rgbGammaCorr

	align 16
CSNV12_RGB8_do_nv12rgb8:
	push rbp
	push rbx
	push rsi
	push rdi
	
	mov r10,rcx
	mov r11,rdx
	lea rdx,[r9 * 8] ;width
	mov qword [rsp-8],rdx ;OSInt cSize = width << 3;
	mov rcx,r9
	lea rdx,[r9-4]
	shr rcx,2
	shr rdx,1
	mov qword [rsp-16],rdx ;OSInt cSub = (width >> 1) - 2;
	mov qword [rsp-24],rcx ;OSInt cWidth4 = width >> 2;
	mov qword [rsp-32],0 ;Int32 cofst = 0;//this->cofst;

	mov rcx,qword [rsp+72] ;height
	shr rcx,1
	mov rbx,qword [rsp+96] ;isLast
	and rbx,1
	shl rbx,1
	sub rcx,rbx
	mov qword [rsp+72],rcx ;heightLeft

	mov rcx,r9 ;width
	shr rcx,2
	mov rdi,qword [rsp+104] ;csLineBuff
	mov rbx,qword [rsp+120] ;yuv2rgb
	mov rsi,r10 ;yPtr

	align 16
n2r8lop2a:
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
	jnz n2r8lop2a
	mov r10,rsi ;yPtr

	mov rbp,qword [rsp-16] ;cSub
	mov rdx,qword [rsp-8] ;cSize
	mov rsi,r11 ;uvPtr
	mov rdi,qword [rsp+104] ;csLineBuff
	shr rbp,1 ;widthLeft

	movzx rax,byte [rsi]
	movq xmm0,[rbx+rax*8 + 2048]
	movzx rax,byte [rsi+1]
	movq xmm1,[rbx+rax*8 + 4096]
	paddsw xmm0,xmm1

	pxor xmm1,xmm1
	punpcklqdq xmm1,xmm0
	psraw xmm1,1
	por xmm0,xmm1

	movdqa xmm2,[rdi]
	movdqa xmm3,[rdi+rdx]
	paddsw xmm2,xmm0
	psraw xmm0,1
	movdqa [rdi],xmm2
	paddsw xmm3,xmm0
	movdqa [rdi+rdx],xmm3

	lea rdi,[rdi+16]
	lea rsi,[rsi+2]

	align 16
n2r8lop3a:
	movzx rax,byte [rsi]
	movq xmm0,[rbx+rax*8 + 2048]
	movzx rax,byte [rsi+1]
	movq xmm6,[rbx+rax*8 + 4096]
	movzx rax,byte [rsi+2]
	movq xmm1,[rbx+rax*8 + 2048]
	movzx rax,byte [rsi+3]
	movq xmm7,[rbx+rax*8 + 4096]
	paddsw xmm0,xmm6
	paddsw xmm1,xmm7

	pxor xmm4,xmm4
	pxor xmm5,xmm5
	punpcklqdq xmm4,xmm0
	punpcklqdq xmm5,xmm1
	psraw xmm4,1
	psraw xmm5,1
	por xmm0,xmm4
	por xmm1,xmm5
	paddsw xmm0,xmm5

	movdqa xmm2,[rdi+16]
	movdqa xmm3,[rdi+rdx+16]
	paddsw xmm2,xmm1
	psraw xmm1,1
	movdqa [rdi+16],xmm2
	paddsw xmm3,xmm1
	movdqa [rdi+rdx+16],xmm3

	movdqa xmm2,[rdi]
	movdqa xmm3,[rdi+rdx]
	paddsw xmm2,xmm0
	psraw xmm0,1
	movdqa [rdi],xmm2
	paddsw xmm3,xmm0
	movdqa [rdi+rdx],xmm3

	movdqa xmm2,[rdi-16]
	movdqa xmm3,[rdi+rdx-16]
	paddsw xmm2,xmm4
	psraw xmm4,1
	movdqa [rdi-16],xmm2
	paddsw xmm3,xmm4
	movdqa [rdi+rdx-16],xmm3

	lea rdi,[rdi+32]
	lea rsi,[rsi+4]
	dec rbp
	jnz n2r8lop3a

	pxor xmm4,xmm4
	movzx rax,byte [rsi]
	movq xmm0,[rbx+rax*8 + 2048]
	movzx rax,byte [rsi+1]
	movq xmm1,[rbx+rax*8 + 4096]
	paddsw xmm0,xmm1

	punpcklqdq xmm4,xmm0
	punpcklqdq xmm0,xmm0

	movdqa xmm2,[rdi]
	movdqa xmm3,[rdi+rdx]
	paddsw xmm2,xmm0
	psraw xmm0,1
	movdqa [rdi],xmm2
	paddsw xmm3,xmm0
	movdqa [rdi+rdx],xmm3

	psraw xmm4,1
	movdqa xmm2,[rdi-16]
	paddsw xmm2,xmm4
	movdqa xmm3,[rdi+rdx-16]
	psraw xmm4,1
	paddsw xmm2,xmm4
	movdqa [rdi-16],xmm2
	movdqa [rdi+rdx-16],xmm3

	lea rdi,[rdi+16]
	lea rsi,[rsi+2]
	mov r11,rsi ;uvPtr

	align 16
n2r8lop:
	mov rcx,r9 ;width
	mov rdi,qword [rsp+112] ;csLineBuff2
	mov rbx,qword [rsp+120] ;yuv2rgb
	mov rsi,r10 ;yPtr

	align 16
n2r8lop2:
	movzx rax,byte [rsi]
	movq xmm0,[rbx+rax*8]
	movzx rax,byte [rsi+1]
	movq xmm1,[rbx+rax*8]
	punpcklqdq xmm0,xmm1
	movdqa [rdi],xmm0
	lea rsi,[rsi+2]
	lea rdi,[rdi+16]
	dec rcx
	jnz n2r8lop2
	mov r10,rsi ;yPtr

	mov rbp,qword [rsp-16] ;cSub
	mov rcx,qword [rsp-8] ;cSize
	mov rdx,r11 ;uvPtr
	mov rsi,qword [rsp+104] ;csLineBuff
	mov rdi,qword [rsp+112] ;csLineBuff2
	shr rbp,1 ;widthLeft

	pxor xmm4,xmm4
	movzx rax,byte [rdx]
	movq xmm0,[rbx+rax*8 + 2048]
	movzx rax,byte [rdx+1]
	movq xmm1,[rbx+rax*8 + 4096]
	paddsw xmm0,xmm1

	punpcklqdq xmm4,xmm0
	psraw xmm4,1
	por xmm0,xmm4

	movdqa xmm2,[rdi]
	movdqa xmm3,[rdi+rcx]
	paddsw xmm2,xmm0
	psraw xmm0,1
	movdqa [rdi],xmm2
	paddsw xmm3,xmm0
	movdqa xmm2,[rsi+rcx]
	paddsw xmm2,xmm0
	movdqa [rdi+rcx],xmm3
	movdqa [rsi+rcx],xmm2

	lea rsi,[rsi+16]
	lea rdi,[rdi+16]
	lea rdx,[rdx+2]

	ALIGN 16
n2r8lop3:
	pxor xmm4,xmm4
	pxor xmm5,xmm5
	movzx rax,byte [rdx]
	movq xmm0,[rbx+rax*8 + 2048]
	movzx rax,byte [rdx+1]
	movq xmm6,[rbx+rax*8 + 4096]
	movzx rax,byte [rdx+2]
	movq xmm1,[rbx+rax*8 + 2048]
	movzx rax,byte [rdx+3]
	movq xmm7,[rbx+rax*8 + 4096]
	paddsw xmm0,xmm6
	paddsw xmm1,xmm7

	punpcklqdq xmm4,xmm0
	punpcklqdq xmm5,xmm1
	psraw xmm4,1
	psraw xmm5,1
	por xmm0,xmm4
	por xmm1,xmm5
	paddsw xmm0,xmm5

	movdqa xmm2,[rdi+16]
	movdqa xmm3,[rdi+rcx+16]
	paddsw xmm2,xmm1
	psraw xmm1,1
	movdqa [rdi+16],xmm2
	paddsw xmm3,xmm1
	movdqa xmm2,[rsi+rcx+16]
	paddsw xmm2,xmm1
	movdqa [rdi+rcx+16],xmm3
	movdqa [rsi+rcx+16],xmm2

	movdqa xmm2,[rdi]
	movdqa xmm3,[rdi+rcx]
	paddsw xmm2,xmm0
	psraw xmm0,1
	movdqa [rdi],xmm2
	paddsw xmm3,xmm0
	movdqa xmm2,[rsi+rcx]
	paddsw xmm2,xmm0
	movdqa [rdi+rcx],xmm3
	movdqa [rsi+rcx],xmm2

	movdqa xmm2,[rdi-16]
	movdqa xmm3,[rdi+rcx-16]
	paddsw xmm2,xmm4
	psraw xmm4,1
	movdqa [rdi-16],xmm2
	paddsw xmm3,xmm4
	movdqa xmm2,[rsi+rcx-16]
	paddsw xmm2,xmm4
	movdqa [rdi+rcx-16],xmm3
	movdqa [rsi+rcx-16],xmm2

	lea rsi,[rsi+32]

	lea rdi,[rdi+32]
	lea rdx,[rdx+4]
	dec rbp ;widthLeft
	jnz n2r8lop3

	movzx rax,byte [rdx]
	movq xmm0,[rbx+rax*8 + 2048]
	movzx rax,byte [rdx+1]
	movq xmm1,[rbx+rax*8 + 4096]
	paddsw xmm0,xmm1

	pxor xmm4,xmm4
	punpcklqdq xmm4,xmm0
	psraw xmm4,1
	por xmm0,xmm4

	movdqa xmm2,[rdi]
	movdqa xmm3,[rdi+rcx]
	paddsw xmm2,xmm0
	paddsw xmm3,xmm0
	movdqa [rdi],xmm2
	movdqa [rdi+rcx],xmm3
	movdqa xmm2,[rdi-16]
	movdqa xmm3,[rdi+rcx-16]
	paddsw xmm2,xmm4
	paddsw xmm3,xmm4
	movdqa xmm2,[rdi-16]
	movdqa xmm3,[rdi+rcx-16]
	psraw xmm4,1
	psraw xmm0,1
	movdqa xmm2,[rsi+rcx]
	movdqa xmm3,[rsi+rcx-16]
	paddsw xmm2,xmm0
	paddsw xmm3,xmm4
	movdqa [rsi+rcx],xmm2
	movdqa [rsi+rcx-16],xmm3

	lea rsi,[rsi+16]
	lea rdi,[rdi+16]
	lea rdx,[rdx+2]
	mov r11,rdx ;uvPtr

	mov rcx,r9 ;width
	mov rsi,qword [rsp+104] ;csLineBuff
	mov rdi,r8 ;dest
	mov rbx,qword [rsp+128] ;rgbGammaCorr
	
	align 16
n2r8lop5:
	movzx rax,word [rsi+4]
	movzx rdx,byte [rbx+rax]
	shl rdx,16
	movzx rax,word [rsi]
	mov dl,byte [rbx+rax+131072]
	movzx rax,word [rsi+2]
	mov dh,byte [rbx+rax+65536]
	mov dword [rdi],edx
	lea rsi,[rsi+8]
	lea rdi,[rdi+4]
	dec rcx
	jnz n2r8lop5

	add r8,qword [rsp+80] ;dest += dbpl

	mov rcx,r9 ;width
	mov rdi,r8 ;dest
	align 16
n2r8lop6:
	movzx rax,word [rsi+4]
	movzx rdx,byte [rbx+rax]
	shl rdx,16
	movzx rax,word [rsi]
	mov dl,byte [rbx+rax+131072]
	movzx rax,word [rsi+2]
	mov dh,byte [rbx+rax+65536]
	mov dword [rdi],edx
	lea rsi,[rsi+8]
	lea rdi,[rdi+4]
	dec rcx
	jnz n2r8lop6

	add r8,qword [rsp+80] ;dest += dbpl

	mov rsi,qword [rsp+104] ;csLineBuff
	mov rdi,qword [rsp+112] ;csLineBuff2
	mov qword [rsp+112],rsi ;csLineBuff2
	mov qword [rsp+104],rdi ;csLineBuff

	dec qword [rsp+72] ;heightLeft
	jnz n2r8lop

	test qword [rsp+96],1 ;isLast
	jz n2r8lopexit

	mov rsi,r10 ;yPtr
	mov rcx,r9 ;width
	mov rdi,qword [rsp+112] ;csLineBuff2
	mov rbx,qword [rsp+120] ;yuv2rgb

	align 16
n2r8lop2b:
	movzx rax,byte [rsi]
	movq xmm0,[rbx+rax*8]
	movzx rax,byte [rsi+1]
	movq xmm1,[rbx+rax*8]
	punpcklqdq xmm0,xmm1
	movdqa [rdi],xmm0
	lea rsi,[rsi+2]
	lea rdi,[rdi+16]
	dec rcx
	jnz n2r8lop2b
	mov r10,rsi ;yPtr

	mov rbp,qword [rsp-16] ;cSub
	mov rcx,qword [rsp-8] ;cSize
	mov rdx,r11 ;uvPtr
	mov rsi,qword [rsp+104] ;csLineBuff
	mov rdi,qword [rsp+112] ;csLineBuff2
	shr rbp,1 ;widthLeft

	pxor xmm1,xmm1
	movzx rax,byte [rdx]
	movq xmm0,[rbx+rax*8 + 2048]
	movzx rax,byte [rdx+1]
	movq xmm4,[rbx+rax*8 + 4096]
	paddsw xmm0,xmm4

	punpcklqdq xmm1,xmm0
	psraw xmm1,1
	por xmm0,xmm1

	movdqa xmm2,[rdi]
	movdqa xmm3,[rdi+rcx]
	paddsw xmm2,xmm0
	paddsw xmm3,xmm0
	movdqa [rdi],xmm2
	movdqa [rdi+rcx],xmm3
	psraw xmm0,1
	movdqa xmm2,[rsi+rcx]
	paddsw xmm2,xmm0
	movdqa [rsi+rcx],xmm2

	lea rsi,[rsi+16]
	lea rdi,[rdi+16]
	lea rdx,[rdx+2]

	align 16
n2r8lop3b:
	pxor xmm4,xmm4
	pxor xmm5,xmm5
	movzx rax,byte [rdx]
	movq xmm0,[rbx+rax*8 + 2048]
	movzx rax,byte [rdx+1]
	movq xmm6,[rbx+rax*8 + 4096]
	movzx rax,byte [rdx+2]
	movq xmm1,[rbx+rax*8 + 2048]
	movzx rax,byte [rdx+3]
	movq xmm7,[rbx+rax*8 + 4096]
	paddsw xmm0,xmm6
	paddsw xmm1,xmm7

	punpcklqdq xmm4,xmm0
	punpcklqdq xmm5,xmm1
	psraw xmm4,1
	psraw xmm5,1
	por xmm0,xmm4
	por xmm1,xmm5
	paddsw xmm0,xmm5

	movdqa xmm2,[rdi]
	movdqa xmm3,[rdi+16]
	movdqa xmm6,[rdi+rcx]
	movdqa xmm7,[rdi+rcx+16]
	paddsw xmm2,xmm0
	paddsw xmm3,xmm1
	paddsw xmm6,xmm0
	paddsw xmm7,xmm1
	movdqa [rdi],xmm2
	movdqa [rdi+16],xmm3
	movdqa [rdi+rcx],xmm6
	movdqa [rdi+rcx+16],xmm7
	psraw xmm0,1
	psraw xmm1,1
	movdqa xmm2,[rsi+rcx]
	movdqa xmm3,[rsi+rcx+16]
	paddsw xmm2,xmm0
	paddsw xmm3,xmm1
	movdqa [rsi+rcx],xmm2
	movdqa [rsi+rcx+16],xmm3
	movdqa xmm2,[rdi-16]
	movdqa xmm3,[rdi+rcx-16]
	paddsw xmm2,xmm4
	paddsw xmm3,xmm4
	movdqa [rdi-16],xmm2
	movdqa [rdi+rcx-16],xmm3
	psraw xmm4,1
	movdqa xmm2,[rsi+rcx-16]
	paddsw xmm2,xmm4
	movdqa [rsi+rcx-16],xmm2

	lea rsi,[rsi+32]
	lea rdi,[rdi+32]
	lea rdx,[rdx+4]
	dec rbp ;widthLeft//ecx
	jnz n2r8lop3b

	pxor xmm1,xmm1
	movzx rax,byte [rdx]
	movq xmm0,[rbx+rax*8+2048]
	movzx rax,byte [rdx+1]
	movq xmm4,[rbx+rax*8+4096]
	paddsw xmm0,xmm4

	punpcklqdq xmm1,xmm0
	punpcklqdq xmm0,xmm0
	psraw xmm1,1

	movdqa xmm2,[rdi]
	movdqa xmm3,[rdi+rcx]
	movdqa xmm4,[rdi-16]
	movdqa xmm5,[rdi+rcx-16]
	paddsw xmm2,xmm0
	paddsw xmm3,xmm0
	paddsw xmm4,xmm1
	paddsw xmm5,xmm1
	movdqa [rdi],xmm2
	movdqa [rdi+rcx],xmm3
	movdqa [rdi-16],xmm4
	movdqa [rdi+rcx-16],xmm5
	psraw xmm0,1
	psraw xmm1,1
	movdqa xmm2,[rsi+rcx]
	movdqa xmm3,[rsi+rcx-16]
	paddsw xmm2,xmm0
	paddsw xmm3,xmm1
	movdqa [rsi+rcx],xmm2
	movdqa [rsi+rcx-16],xmm3

	lea rsi,[rsi+16]
	lea rdi,[rdi+16]
	lea rdx,[rdx+2]
	mov r11,rdx ;uvPtr

	mov rdi,r8 ;dest
	mov rcx,r9 ;width
	mov rsi,qword [rsp+104] ;csLineBuff
	mov rbx,qword [rsp+128] ;rgbGammaCorr

	align 16
n2r8lop5b:
	movzx rax,word [rsi+4]
	movzx rdx,byte [rbx+rax]
	shl rdx,16
	movzx rax,word [rsi]
	mov dl,byte [rbx+rax+131072]
	movzx rax,word [rsi+2]
	mov dh,byte [rbx+rax+65536]
	mov dword [rdi],edx
	lea rsi,[rsi+8]
	lea rdi,[rdi+4]
	dec rcx
	jnz n2r8lop5b

	add r8,qword [rsp+80] ;dest += dbpl

	mov rdi,r8 ;dest
	mov rcx,r9 ;width
	align 16
n2r8lop6b:
	movzx rax,word [rsi+4]
	movzx rdx,byte [rbx+rax]
	shl rdx,16
	movzx rax,word [rsi]
	mov dl,byte [rbx+rax+131072]
	movzx rax,word [rsi+2]
	mov dh,byte [rbx+rax+65536]
	mov dword [rdi],edx
	lea rsi,[rsi+8]
	lea rdi,[rdi+4]
	dec rcx
	jnz n2r8lop6b

	add r8,qword [rsp+80] ;dest += dbpl

	mov rsi,qword [rsp+104] ;csLineBuff
	mov rdi,qword [rsp+112] ;csLineBuff2
	mov qword [rsp+112],rsi ;csLineBuff2
	mov qword [rsp+104],rdi ;csLineBuff

	mov rdi,r8 ;dest
	mov rcx,r9 ;width
	mov rsi,qword [rsp+104] ;csLineBuff
	mov rbx,qword [rsp+128] ;rgbGammaCorr
	align 16
n2r8lop5c:
	movzx rax,word [rsi+4]
	movzx rdx,byte [rbx+rax]
	shl rdx,16
	movzx rax,word [rsi]
	mov dl,byte [rbx+rax+131072]
	movzx rax,word [rsi+2]
	mov dh,byte [rbx+rax+65536]
	mov dword [rdi],edx
	lea rsi,[rsi+8]
	lea rdi,[rdi+4]
	dec rcx
	jnz n2r8lop5c

	add r8,qword [rsp+80] ;dest += dbpl

	mov rdi,r8 ;dest
	mov rcx,r9 ;width

	align 16
n2r8lop6c:
	movzx rax,word [rsi+4]
	movzx rdx,byte [rbx+rax]
	shl rdx,16
	movzx rax,word [rsi]
	mov dl,byte [rbx+rax+131072]
	movzx rax,word [rsi+2]
	mov dh,byte [rbx+rax+65536]
	mov dword [rdi],edx
	lea rsi,[rsi+8]
	lea rdi,[rdi+4]
	dec rcx
	jnz n2r8lop6c

	add r8,qword [rsp+80] ;dest += dbpl

	mov rsi,qword [rsp+104] ;csLineBuff
	mov rdi,qword [rsp+112] ;csLineBuff2
	mov qword [rsp+112],rsi ;csLineBuff2
	mov qword [rsp+104],rdi ;csLineBuff

	align 16
n2r8lopexit:
	pop rdi
	pop rsi
	pop rbx
	pop rbp
	ret

;void CSNV12_RGB8_do_nv12rgb2(UInt8 *yPtr, UInt8 *uvPtr, UInt8 *dest, OSInt width, OSInt height, OSInt dbpl, OSInt isFirst, OSInt isLast, UInt8 *csLineBuff, UInt8 *csLineBuff2, Int64 *yuv2rgb, UInt8 *rgbGammaCorr);
;0 rdi
;8 rsi
;16 rbx
;24 rbp
;32 retAddr
;rcx yPtr
;rdx uvPtr
;r8 dest
;r9 width
;72 height
;80 dbpl
;88 isFirst
;96 isLast
;104 csLineBuff
;112 csLineBuff2
;120 yuv2rgb
;128 rgbGammaCorr

	align 16
CSNV12_RGB8_do_nv12rgb2:
	ret
