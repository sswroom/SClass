section .text

global _ScaledMapView_IMapXYToScnXY
global ScaledMapView_IMapXYToScnXY

;Bool ScaledMapView_IMapXYToScnXY(const Int32 *srcArr, Int32 *destArr, OSInt nPoints, Double rRate, Double dleft, Double dbottom, Double xmul, Double ymul, Int32 ofstX, Int32 ofstY, OSInt scnWidth, OSInt scnHeight);
;0 edi
;4 esi
;8 retAddr
;12 srcArr
;16 destArr
;20 nPoints
;24 rRate
;32 dleft
;40 dbottom
;48 xmul
;56 ymul
;64 ofstX
;68 ofstY
;72 scnWidth
;76 scnHeight
	align 16
_ScaledMapView_IMapXYToScnXY:
ScaledMapView_IMapXYToScnXY:
	push esi
	push edi
	mov esi,dword [esp+12] ;srcArr
	mov edi,dword [esp+16] ;destArr
	pxor xmm2,xmm2
	pxor xmm4,xmm4
	movsd xmm3,[esp+24] ;rRate
	subsd xmm2,[esp+32] ;dleft
	movsd xmm1,[esp+40] ;dbottom
	movupd xmm5,[esp+48] ;xmul ymul
	unpcklpd xmm2,xmm1
	subsd xmm4,xmm3
	unpcklpd xmm3,xmm4
	cvtsi2sd xmm4,dword [esp+64] ;ofstX
	cvtsi2sd xmm1,dword [esp+68] ;ofstY
	unpcklpd xmm4,xmm1
	mov eax,0xffffffff
	mov ecx,dword [esp+72] ;scnWidth
	mov edx,dword [esp+76] ;scnHeight
	movd xmm6,eax
	movd xmm7,ecx
	movd xmm0,edx
	punpckldq xmm6,xmm6
	punpckldq xmm7,xmm0
	cvtdq2pd xmm6,xmm6 ;maxVals
	cvtdq2pd xmm7,xmm7 ;minVals
	mov ecx,dword [esp+20] ;nPoints
	align 16
imxytsxylop:
	cvtpi2pd xmm0,[esi]
	mulpd xmm0,xmm3
	addpd xmm0,xmm2
	mulpd xmm0,xmm5
	addpd xmm0,xmm4
	cvtpd2dq xmm1,xmm0
	maxpd xmm6,xmm0
	minpd xmm7,xmm0
	movq [edi],xmm1
	lea esi,[esi+8]
	lea edi,[edi+8]
	dec ecx
	jnz imxytsxylop
	cvtpd2dq xmm6,xmm6
	cvtpd2dq xmm7,xmm7
	movd eax,xmm6
	movd edx,xmm7
	cmp eax,0
	jl imxytsxylop2
	cmp edx,dword [esp+72] ;scnWidth
	jge imxytsxylop2
	pshufd xmm6,xmm6,1
	pshufd xmm7,xmm7,1
	movd eax,xmm6
	movd edx,xmm7
	cmp eax,0
	jl imxytsxylop2
	cmp edx,dword [esp+76] ;scnHeight
	jge imxytsxylop2
	mov eax,1
	jmp imxytsxyexit
	
	align 16
imxytsxylop2:
	mov eax,0
	align 16
imxytsxyexit:	
	pop edi
	pop esi
	ret
