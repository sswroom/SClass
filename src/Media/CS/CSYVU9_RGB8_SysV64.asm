section .text

global CSYVU9_RGB8_do_yvu9rgb8
global _CSYVU9_RGB8_do_yvu9rgb8

;void CSYVU9_RGB8_do_yvu9rgb8(UInt8 *yPtr, UInt8 *uPtr, UInt8 *vPtr, UInt8 *dest, OSInt width, OSInt height, OSInt dbpl, OSInt isFirst, OSInt isLast, UInt8 *csLineBuff, UInt8 *csLineBuff2, Int64 *yuv2rgb, UInt8 *rgbGammaCorr)
;-24 cSub
;-16 cSize
;-8 cWidth4
;0 rdi
;8 rsi
;16 rbx
;24 rbp
;32 retAddr
;rcx yPtr r10
;rdx uPtr r11
;r8 vptr
;r9 dest
;72 width
;80 height
;88 dbpl
;96 isFirst
;104 isLast
;112 csLineBuff
;120 csLineBuff2
;128 yuv2rgb
;136 rgbGammaCorr

	align 16
CSYVU9_RGB8_do_yvu9rgb8:
_CSYVU9_RGB8_do_yvu9rgb8:
	push rbp
	push rbx
	push rsi
	push rdi
	mov r10,rcx
	mov r11,rdx
	
	lea rax,qword [rsp+72] ;width
	lea rdx,[rax*8]
	mov rcx,rax
	mov rbx,rax
	shr rcx,1
	shr rbx,2
	sub rcx,2
	mov qword [rsp-24],rcx ;cSub					Int32 cSub = (width >> 1) - 2;
	mov qword [rsp-16],rdx ;cSize					Int32 cSize = width << 3;
	mov qword [rsp-8],rbx ;cWidth4					Int32 cWidth4 = width >> 2;

	mov rcx,qword [rsp+80] ;height
	mov rbx,qword [rsp+104] ;isLast
	shr rcx,1
	and rbx,1
	shl rbx,1
	sub rcx,rbx
	mov qword [rsp+80],rcx ;heightLeft
	
	mov rcx,qword [rsp+72] ;width
	shr rcx,2
	mov rdi,qword [rsp+112] ;csLineBuff
	mov rbx,qword [rsp+128] ;yuv2rgb

	align 16
y2rflop2a:
	movzx rax,byte [r10]
	movq xmm0,[rbx+rax * 8]
	movzx rax,byte [r10+1]
	movq xmm1,[rbx+rax * 8]
	punpcklqdq xmm0,xmm1
	movdqa [rdi],xmm0

	movzx rax,byte [r10+2]
	movq xmm0,[rbx+rax * 8]
	movzx rax,byte [r10+3]
	movq xmm1,[rbx+rax * 8]
	punpcklqdq xmm0,xmm1
	movdqa [rdi+16],xmm0

	movzx rax,byte [r10+4]
	movq xmm0,[rbx+rax * 8]
	movzx rax,byte [r10+5]
	movq xmm1,[rbx+rax * 8]
	punpcklqdq xmm0,xmm1
	movdqa [rdi+32],xmm0

	movzx rax,byte [r10+6]
	movq xmm0,[rbx+rax * 8]
	movzx rax,byte [r10+7]
	movq xmm1,[rbx+rax * 8]
	punpcklqdq xmm0,xmm1
	movdqa [rdi+48],xmm0

	lea r10,[r10+8]
	lea rdi,[rdi+64]
	dec rcx
	jnz y2rflop2a

	mov rbp,qword [rsp-24] ;cSub
	mov rdx,qword [rsp-16] ;cSize
	mov rsi,r11 ;uPtr
	mov rcx,r8 ;vPtr
	mov rdi,qword [rsp+112] ;csLineBuff
	shr rbp,1


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

	align 16
y2rflop3a:
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
	inc rsi
	inc rcx
	dec rbp
	jnz y2rflop3a

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

	align 16
y2rflop:
	mov rcx,qword [rsp+72] ;width
	mov rdi,qword [rsp+120] ;csLineBuff2
	mov rbx,qword [rsp+128] ;yuv2rgb

	align 16
y2rflop2:
	movzx rax,byte [r10]
	movq xmm0,[rbx+rax*8]
	movzx rax,byte [r10+1]
	movq xmm1,[rbx+rax*8]
	punpcklqdq xmm0,xmm1
	movdqa [rdi],xmm0
	lea r10,[r10+2]
	lea rdi,[rdi+16]
	dec rcx
	jnz y2rflop2

	mov rcx,qword [rsp-24] ;cSub
	mov rbp,qword [rsp+112] ;csLineBuff
	mov rdi,qword [rsp+120] ;csLineBuff2
	mov rsi,r11 ;uPtr
	mov rdx,r8 ;vPtr
	shr rcx,1
	mov qword [rsp-32],rcx ;widthLeft
	mov rcx,qword [rsp-16] ;cSize

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


	add rbp,16 ;buffTemp
	add rdi,16

	ALIGN 16
y2rflop3:
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

	add rbp, 32 ;buffTemp

	lea rdi,[rdi+32]
	inc rsi
	inc rdx
	dec qword [rsp-32] ;widthLeft
	jnz y2rflop3

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
	movdqa xmm3,[rbp+rcx]
	movdqa xmm6,[rbp+rcx]
	paddsw xmm2,xmm0
	psraw xmm0,1
	movdqa [rdi],xmm2
	paddsw xmm3,xmm0
	paddsw xmm6,xmm0
	movdqa [rdi+rcx],xmm3
	movdqa [rbp+rcx],xmm6

	add rbp,16 ;buffTemp
	add rdi,16
	inc rsi
	inc rdx
	test qword [rsp+80], 1 ;heightLeft
	jnz y2rflop3c
	mov r11,rsi ;uPtr
	mov r8,rdx ;vPtr

	align 16
y2rflop3c:
	mov rdi,r9 ;dest
	mov rcx,qword [rsp+72] ;width
	mov rsi,qword [rsp+112] ;csLineBuff
	mov rbx,qword [rsp+136] ;rgbGammaCorr
	ALIGN 16
y2rflop5:
	movzx rax,word [rsi+4]
	movzx edx,byte [rbx+rax]
	shl edx,16
	movzx rax,word [rsi]
	mov dl,byte [rbx+rax+131072]
	movzx rax,word [rsi+2]
	mov dh,byte [rbx+rax+65536]
	mov dword [rdi],edx
	add rsi,8
	add rdi,4
	dec rcx
	jnz y2rflop5

	add r9,qword [rsp+88] ;dbpl
	mov rcx,qword [rsp+72] ;width
	mov rdi,r9 ;dest
	ALIGN 16
y2rflop6:
	movzx rax,word [rsi+4]
	movzx edx,byte [rbx+rax]
	shl edx,16
	movzx rax,word [rsi]
	mov dl,byte [rbx+rax+131072]
	movzx rax,word [rsi+2]
	mov dh,byte [rbx+rax+65536]
	mov dword [rdi],edx
	add rsi,8
	add rdi,4
	dec rcx
	jnz y2rflop6

	add r9,qword [rsp+88] ;dbpl

	mov rax,qword [rsp+112] ;csLineBuff
	xchg rax,qword [rsp+120] ;csLineBuff2
	mov qword [rsp+112],rax ;csLineBuff

	dec qword [rsp+80] ;heightLeft
	jnz y2rflop

	test qword [rsp+104],1 ;isLast
	jz yv2rflopexit

	mov rcx,qword [rsp+72] ;width
	mov rdi,qword [rsp+120] ;csLineBuff2
	mov rbx,qword [rsp+128] ;yuv2rgb

	align 16
y2rflop2b:
	movzx rax,byte [r10]
	movq xmm0,[rbx+rax*8]
	movzx rax,byte [r10+1]
	movq xmm1,[rbx+rax*8]
	punpcklqdq xmm0,xmm1
	movdqa [rdi],xmm0
	lea r10,[r10+2]
	lea rdi,[rdi+16]
	dec rcx
	jnz y2rflop2b

	mov rcx,qword [rsp-24] ;cSub
	mov rbp,qword [rsp+112] ;csLineBuff
	mov rdi,qword [rsp+120] ;csLineBuff2
	mov rsi,r11 ;uPtr
	mov rdx,r8 ;vPtr
	shr rcx,1
	mov qword [rsp-32],rcx ;widthLeft
	mov rcx,qword [rsp-16] ;cSize

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

	add rbp,16 ;buffTemp
	add rdi,16

	align 16
y2rflop3b:
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

	add rbp,32
	add rdi,32
	inc rsi
	inc rdx
	dec qword [rsp-32] ;widthLeft
	jnz y2rflop3b

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

	add rbp,16
	add rdi,16
	inc rsi
	inc rdx
	mov r11,rsi ;uPtr
	mov r8,rdx ;vPtr

	mov rdi,r9 ;dest
	mov rcx,qword [rsp+72] ;width
	mov rsi,qword [rsp+112] ;csLineBuff
	mov rbx,qword [rsp+136] ;rgbGammaCorr
	align 16
y2rflop5b:
	movzx rax,word [rsi+4]
	movzx edx,byte [rbx+rax]
	shl edx,16
	movzx eax,word [rsi]
	mov dl,byte [rbx+rax+131072]
	movzx eax,word [rsi+2]
	mov dh,byte [rbx+rax+65536]
	mov dword [rdi],edx
	add rsi,8
	add rdi,4
	dec rcx
	jnz y2rflop5b

	add r9,qword [rsp+88] ;dbpl

	mov rdi,r9 ;dest
	mov rcx,qword [rsp+72] ;width
	align 16
y2rflop6b:
	movzx rax,word [rsi+4]
	movzx edx,byte [rbx+rax]
	shl edx,16
	movzx rax,word [rsi]
	mov dl,byte [rbx+rax+131072]
	movzx rax,word [rsi+2]
	mov dh,byte [rbx+rax+65536]
	mov dword [rdi],edx
	add rsi,8
	add rdi,4
	dec rcx
	jnz y2rflop6b

	add r9,qword [rsp+88] ;dbpl

	mov rsi,qword [rsp+112] ;csLineBuff
	mov rdi,qword [rsp+120] ;csLineBuff2
	mov qword [rsp+120],rsi ;csLineBuff2
	mov qword [rsp+112],rdi ;csLineBuff

	mov rdi,r9 ;dest
	mov rcx,qword [rsp+72] ;width
	mov rsi,qword [rsp+112] ;csLineBuff
	mov rbx,qword [rsp+136] ;rgbGammaCorr
	align 16
y2rflop5c:
	movzx rax,word [rsi+4]
	movzx edx,byte [rbx+rax]
	shl edx,16
	movzx rax,word [rsi]
	mov dl,byte [rbx+rax+131072]
	movzx rax,word [rsi+2]
	mov dh,byte [rbx+rax+65536]
	mov dword [rdi],edx
	add rsi,8
	add rdi,4
	dec rcx
	jnz y2rflop5c

	add r9,qword [rsp+88] ;dbpl

	mov rdi,r9 ;dest
	mov rcx,qword [rsp+72] ;width
	align 16
y2rflop6c:
	movzx rax,word [rsi+4]
	movzx edx,byte [rbx+rax]
	shl edx,16
	movzx rax,word [rsi]
	mov dl,byte [rbx+rax+131072]
	movzx rax,word [rsi+2]
	mov dh,byte [rbx+rax+65536]
	mov dword [rdi],edx
	add rsi,8
	add rdi,4
	dec rcx
	jnz y2rflop6c

	add r9,qword [rsp+88] ;dbpl

	mov rsi,qword [rsp+112] ;csLineBuff
	mov rdi,qword [rsp+120] ;csLineBuff2
	mov qword [rsp+120],rsi ;csLineBuff2
	mov qword [rsp+112],rdi ;csLineBuff

	align 16
yv2rflopexit:
	pop rdi
	pop rsi
	pop rbx
	pop rbp
	ret
