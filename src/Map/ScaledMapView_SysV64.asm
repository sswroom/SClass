%ifidn __OUTPUT_FORMAT__,elf
section .note.GNU-stack noalloc noexec nowrite progbits
%endif
%ifidn __OUTPUT_FORMAT__,elf32
section .note.GNU-stack noalloc noexec nowrite progbits
%endif
%ifidn __OUTPUT_FORMAT__,elf64
section .note.GNU-stack noalloc noexec nowrite progbits
%endif

section .text

global ScaledMapView_IMapXYToScnXY
global _ScaledMapView_IMapXYToScnXY

;Bool ScaledMapView_IMapXYToScnXY(const Int32 *srcArr, Int32 *destArr, OSInt nPoints, Double rRate, Double dleft, Double dbottom, Double xmul, Double ymul, Int32 ofstX, Int32 ofstY, OSInt scnWidth, OSInt scnHeight);
;0 retAddr
;rdi srcArr
;rsi destArr
;rdx nPoints
;xmm0 rRate
;xmm1 dleft
;xmm2 dbottom
;xmm3 xmul
;xmm4 ymul
;rcx ofstX
;r8 ofstY
;r9 scnWidth
;8 scnHeight
	align 16
ScaledMapView_IMapXYToScnXY:
_ScaledMapView_IMapXYToScnXY:
	mov r10d,dword [rsp+8]
	movapd xmm5,xmm3
	unpcklpd xmm5,xmm4 ;xmul ymul

	pxor xmm3,xmm3
	subsd xmm3,xmm1 ;dleft
	unpcklpd xmm3,xmm2 ;dbottom

	pxor xmm4,xmm4
	subsd xmm4,xmm0
	unpcklpd xmm0,xmm4 ;rRate -rRate

	cvtsi2sd xmm4,rcx ;ofstX
	cvtsi2sd xmm1,r8 ;ofstY
	unpcklpd xmm4,xmm1

	mov eax,0xffffffff
	movd xmm7,r9d ;scnWidth
	movd xmm2,r10d ;scnHeight
	movd xmm6,eax
	punpckldq xmm6,xmm6
	punpckldq xmm7,xmm2
	cvtdq2pd xmm6,xmm6 ;maxVals
	cvtdq2pd xmm7,xmm7 ;minVals

	align 16
imxytsxylop:
	cvtpi2pd xmm2,[rdi]
	mulpd xmm2,xmm0 ;rRate -rRate
	addpd xmm2,xmm3 ;-dleft dbottom
	mulpd xmm2,xmm5 ;xmul ymul
	addpd xmm2,xmm4 ;ofstX ofstY
	cvtpd2dq xmm1,xmm2
	maxpd xmm6,xmm2
	minpd xmm7,xmm2
	movq [rsi],xmm1
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
	cmp edx,r9d ;scnWidth
	jge imxytsxylop2
	pshufd xmm6,xmm6,1
	pshufd xmm7,xmm7,1
	movd eax,xmm6
	movd edx,xmm7
	cmp eax,0
	jl imxytsxylop2
	cmp edx,r10d ;scnHeight
	jge imxytsxylop2
	mov rax,1
	jmp imxytsxyexit
	
	align 16
imxytsxylop2:
	mov rax,0
	align 16
imxytsxyexit:	
	ret

