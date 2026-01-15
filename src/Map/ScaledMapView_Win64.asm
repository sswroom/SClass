section .text

global ScaledMapView_IMapXYToScnXY

;Bool ScaledMapView_IMapXYToScnXY(const Int32 *srcArr, Int32 *destArr, IntOS nPoints, Double rRate, Double dleft, Double dbottom, Double xmul, Double ymul, Int32 ofstX, Int32 ofstY, IntOS scnWidth, IntOS scnHeight);
;0 xmm6
;16 xmm7
;32 rdi
;40 rsi
;48 retAddr
;rcx srcArr
;rdx destArr
;r8 nPoints
;xmm3 rRate
;88 dleft
;96 dbottom
;104 xmul
;112 ymul
;120 ofstX
;128 ofstY
;136 scnWidth
;144 scnHeight
	align 16
ScaledMapView_IMapXYToScnXY:
	push rsi
	push rdi
	sub rsp,32
	movdqu [rsp],xmm6
	movdqu [rsp+16],xmm7
	mov rsi,rcx
	mov rdi,rdx
	pxor xmm2,xmm2
	pxor xmm4,xmm4
	subsd xmm2,[rsp+88] ;dleft
	movsd xmm1,[rsp+96] ;dbottom
	movupd xmm5,[rsp+104] ;xmul ymul
	unpcklpd xmm2,xmm1
	subsd xmm4,xmm3
	unpcklpd xmm3,xmm4
	cvtsi2sd xmm4,dword [rsp+120] ;ofstX
	cvtsi2sd xmm1,dword [rsp+128] ;ofstY
	unpcklpd xmm4,xmm1
	mov eax,0xffffffff
	mov ecx,dword [rsp+136] ;scnWidth
	mov edx,dword [rsp+144] ;scnHeight
	movd xmm6,eax
	movd xmm7,ecx
	movd xmm0,edx
	punpckldq xmm6,xmm6
	punpckldq xmm7,xmm0
	cvtdq2pd xmm6,xmm6 ;maxVals
	cvtdq2pd xmm7,xmm7 ;minVals
	align 16
imxytsxylop:
	cvtpi2pd xmm0,[rsi]
	mulpd xmm0,xmm3
	addpd xmm0,xmm2
	mulpd xmm0,xmm5
	addpd xmm0,xmm4
	cvtpd2dq xmm1,xmm0
	maxpd xmm6,xmm0
	minpd xmm7,xmm0
	movq [rdi],xmm1
	lea rsi,[rsi+8]
	lea rdi,[rdi+8]
	dec r8
	jnz imxytsxylop
	cvtpd2dq xmm6,xmm6
	cvtpd2dq xmm7,xmm7
	movd eax,xmm6
	movd edx,xmm7
	cmp eax,0
	jl imxytsxylop2
	cmp edx,dword [rsp+136] ;scnWidth
	jge imxytsxylop2
	pshufd xmm6,xmm6,1
	pshufd xmm7,xmm7,1
	movd eax,xmm6
	movd edx,xmm7
	cmp eax,0
	jl imxytsxylop2
	cmp edx,dword [rsp+144] ;scnHeight
	jge imxytsxylop2
	mov rax,1
	jmp imxytsxyexit
	
	align 16
imxytsxylop2:
	mov rax,0
	align 16
imxytsxyexit:	
	movdqu xmm6,[rsp]
	movdqu xmm7,[rsp+16]
	add rsp,32
	pop rdi
	pop rsi
	ret