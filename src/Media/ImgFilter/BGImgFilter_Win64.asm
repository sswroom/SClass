section .text

global BGImgFilter_DiffA8_YUY2
global BGImgFilter_DiffYUV8

;void BGImgFilter_DiffA8_YUY2(UInt8 *destImg, UInt8 *bgImg, OSInt pxCnt)
;0 retAddr
;rcx destImg
;rdx bgImg
;r8 pxCnt
	align 16
BGImgFilter_DiffA8_YUY2:
	mov eax,0x00FF00FF
	movd xmm0,eax
	mov eax,0x00800080
	movd xmm1,eax
	
	mov rax,r8
	shr rax,3
	jz bgifa8_yuy2lop2
	
	punpckldq xmm0,xmm0
	punpckldq xmm1,xmm1
	punpckldq xmm0,xmm0
	punpckldq xmm1,xmm1
	pxor xmm7,xmm7
	
	align 16
bgifa8_yuy2lop1:
	movdqa xmm2,[rcx]
	movdqa xmm3,[rdx]
	movdqa xmm4,xmm2
	movdqa xmm5,xmm3
	pand xmm2,xmm0
	pand xmm3,xmm0
	psrlw xmm4,8
	psrlw xmm5,8
	pand xmm4,xmm0
	pand xmm5,xmm0
	movdqa xmm6,xmm3
	pminsw xmm6,xmm2
	pmaxsw xmm2,xmm3
	psubw xmm2,xmm6
	psubw xmm4,xmm5
	paddw xmm4,xmm1
	packuswb xmm2,xmm7
	packuswb xmm4,xmm7
	punpcklbw xmm2,xmm4
	movntdq [rcx],xmm2
	add rcx,16
	add rdx,16
	dec rax
	jnz bgifa8_yuy2lop1
	
	align 16
bgifa8_yuy2lop2:
	ret
	
;void BGImgFilter_DiffYUV8(UInt8 *destImg, UInt8 *bgImg, OSInt byteCnt)
;0 retAddr
;rcx destImg
;rdx bgImg
;r8 byteCnt
	align 16
BGImgFilter_DiffYUV8:
	mov eax,0x00800080
	movd xmm0,eax
	
	mov rax,r8
	shr rax,4
	jz bgifyuv8lop2
	
	punpckldq xmm0,xmm0
	pxor xmm1,xmm1
	punpckldq xmm0,xmm0
	
	align 16
bgifyuv8lop1:
	movdqa xmm2,[rcx]
	movdqa xmm3,[rdx]
	movdqa xmm4,xmm2
	movdqa xmm5,xmm3
	punpcklbw xmm2,xmm1
	punpcklbw xmm3,xmm1
	punpckhbw xmm4,xmm1
	punpckhbw xmm5,xmm1
	psubw xmm2,xmm3
	psubw xmm4,xmm5
	paddw xmm2,xmm0
	paddw xmm4,xmm0
	packuswb xmm2,xmm4
	movntdq [rcx],xmm2
	add rcx,16
	add rdx,16
	dec rax
	jnz bgifyuv8lop1
	
	align 16
bgifyuv8lop2:
	ret