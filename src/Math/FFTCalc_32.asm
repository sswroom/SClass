section .text

global _FFTCalc_ApplyWindowI16
global _FFTCalc_ApplyWindowI24
global _FFTCalc_FFT2Freq
global _FFTCalc_Rearrange
global _FFTCalc_ForwardCalc
global _FFTCalc_ForwardCalcR
global _FFTCalc_Forward

extern _Math_PI

;void FFTCalc_ApplyWindowI16(Double *complexOut, UInt8 *sampleIn, Double *sampleWindow, IntOS sampleCnt, IntOS sampleAdd, Double sampleMul)
;0 ebx
;4 edi
;8 esi
;12 retaddr
;16 complexOut
;20 sampleIn
;24 sampleWindow
;28 sampleCnt
;32 sampleAdd
;36 sampleMul

	align 16
_FFTCalc_ApplyWindowI16:
	push esi
	push edi
	push ebx
	mov ecx,dword [esp+16] ;complexOut
	mov edx,dword [esp+20] ;sampleIn
	mov esi,dword [esp+24] ;sampleWindow
	mov edi,dword [esp+28] ;sampleCnt
	mov ebx,dword [esp+32] ;sampleAdd
	movsd xmm0,[esp+36] ;sampleMul
	pxor xmm3,xmm3
	unpcklpd xmm0,xmm0
	shr edi,1 ;sampleCnt
awi16lop:
	movsx eax,word [edx]
	movd xmm1,eax
	movsx eax,word [edx+ebx]
	movd xmm2,eax
	punpckldq xmm1,xmm2
	cvtdq2pd xmm2,xmm1
	mulpd xmm2,xmm0
	mulpd xmm2,[esi]
	movapd xmm1,xmm2
	unpcklpd xmm1,xmm3
	unpckhpd xmm2,xmm3
	movntpd [ecx],xmm1
	movntpd [ecx+16],xmm2
	lea edx,[edx+ebx*2]
	lea esi,[esi+16]
	lea ecx,[ecx+32]
	dec edi ;sampleCnt
	jnz awi16lop

	pop ebx
	pop edi
	pop esi
	ret

;void FFTCalc_ApplyWindowI24(Double *complexOut, UInt8 *sampleIn, Double *sampleWindow, IntOS sampleCnt, IntOS sampleAdd, Double sampleMul)
;0 ebx
;4 edi
;8 esi
;12 retaddr
;16 complexOut
;20 sampleIn
;24 sampleWindow
;28 sampleCnt
;32 sampleAdd
;36 sampleMul

	align 16
_FFTCalc_ApplyWindowI24:
	push esi
	push edi
	push ebx
	mov ecx,dword [esp+16] ;complexOut
	mov edx,dword [esp+20] ;sampleIn
	mov esi,dword [esp+24] ;sampleWindow
	mov edi,dword [esp+28] ;sampleCnt
	mov ebx,dword [esp+32] ;sampleAdd
	movsd xmm0,[esp+36] ;sampleMul
	pxor xmm3,xmm3
	unpcklpd xmm0,xmm0
	shr edi,1 ;sampleCnt
awi24lop:
	movsx eax,byte [edx+2]
	shl eax,16
	mov ax,word [edx]
	movd xmm1,eax
	movsx eax,byte [edx+ebx+2]
	shl eax,16
	mov ax,word [edx+ebx]
	movd xmm2,eax
	punpckldq xmm1,xmm2
	cvtdq2pd xmm2,xmm1
	mulpd xmm2,xmm0
	mulpd xmm2,[esi]
	movapd xmm1,xmm2
	unpcklpd xmm1,xmm3
	unpckhpd xmm2,xmm3
	movntpd [ecx],xmm1
	movntpd [ecx+16],xmm2
	lea edx,[edx+ebx*2]
	lea esi,[esi+16]
	lea ecx,[ecx+32]
	dec edi ;sampleCnt
	jnz awi24lop

	pop ebx
	pop edi
	pop esi
	ret

;void FFTCalc_FFT2Freq(Double *freq, Double *complexIn, IntOS sampleCnt)
;0 retaddr
;4 freq
;8 complexIn
;12 sampleCnt

	align 16
_FFTCalc_FFT2Freq:
	mov eax,dword [esp+12]
	mov ecx,dword [esp+4]
	mov edx,dword [esp+8]
	shr eax,1
	test ecx,15
	jz f2freqlop

	align 16
f2freqlop2:
	movapd xmm0,[edx]
	mulpd xmm0,xmm0
	movapd xmm1,[edx+16]
	mulpd xmm1,xmm1
	movapd xmm2,xmm0
	unpcklpd xmm0,xmm1
	unpckhpd xmm2,xmm1
	addpd xmm0,xmm2
	sqrtpd xmm0,xmm0
	movupd [ecx], xmm0
	lea edx,[edx+32]
	lea ecx,[ecx+16]
	dec eax
	jnz f2freqlop2
	ret

	align 16
f2freqlop:
	movapd xmm0,[edx]
	mulpd xmm0,xmm0
	movapd xmm1,[edx+16]
	mulpd xmm1,xmm1
	movapd xmm2,xmm0
	unpcklpd xmm0,xmm1
	unpckhpd xmm2,xmm1
	addpd xmm0,xmm2
	sqrtpd xmm0,xmm0
	movntpd [ecx], xmm0
	lea edx,[edx+32]
	lea ecx,[ecx+16]
	dec eax
	jnz f2freqlop
f2freqexit:
	ret
	
;void FFTCalc_Rearrange(Double *complexData, IntOS sampleCount)
;0 ebx
;4 esi
;8 edi
;12 retaddr
;16 complexData
;20 sampleCount
	align 16
_FFTCalc_Rearrange:
	push edi
	push esi
	push ebx
	
	mov ecx,dword [esp+16]
	mov edx,dword [esp+20]
	lea ebx,[edx*2]	;n = sampleCount << 1;
	xor esi,esi		;i = 0;
	xor edi,edi		;j = 0
	align 16		;while (i < n)
fftrlop:			;{
	cmp edi,esi		;	if (j > i)
	jbe fftrlop2	;	{
	movdqa xmm0,[ecx+esi*8]	;tmpVal = complexData[j];
	movdqa xmm1,[ecx+edi*8]	;complexData[j] = complexData[i];
	movdqa [ecx+edi*8],xmm0	;complexData[i] = tmpVal;
	movdqa [ecx+esi*8],xmm1	;tmpVal = complexData[j + 1];
							;complexData[j + 1] = complexData[i + 1];
							;complexData[i + 1] = tmpVal;
	align 16		;	}
fftrlop2:
	mov eax,edx		;	m = sampleCount;
	test eax,1		;	while (m >= 2 && j >= m)
	jnz fftrlop3
	cmp edi,eax
	jb fftrlop3
	align 16
fftrlop4:			;	{
	sub edi,eax		;		j -= m;
	shr eax,1		;		m >>= 1;
	test eax,1
	jnz fftrlop3
	cmp edi,eax
	jge fftrlop4
	
	align 16
fftrlop3:			;	}
	add edi,eax		;	j += m;
	add esi,2		;	i += 2;
	cmp esi,ebx
	jb fftrlop		;	}

	align 16
fftrexit:
	pop ebx
	pop esi
	pop edi
	ret
	
;void FFTCalc_ForwardCalc(Double *complexData, IntOS sampleCount)
;0 nextSin
;8 kthMul
;16 0.5
;24 ebx
;28 esi
;32 edi
;36 retaddr
;40 complexData
;44 sampleCount
	align 16
_FFTCalc_ForwardCalc:
	sub esp,36
	mov dword [esp+24],ebx
	mov dword [esp+28],esi
	mov dword [esp+32],edi
	
								;// thisSampleCount = 2
								;csin1[0] = 1.0;
								;csin1[1] = 0.0;
								;csin[0] = 0.0;
								;csin[1] = 0.0;
								;groupEnd = 1;
	mov edx,dword [esp+44]		;sampleCount
	xor eax,eax					;i = 0;
	lea edx,[edx*2]
	align 16					;while (i < sampleCount)
fftfclop:						;{
	movapd xmm0,[ecx+eax*8]		;	even[0] = complexData[i * 2];
	movapd xmm2,xmm0			;	even[1] = complexData[i * 2 + 1];
	movapd xmm1,[ecx+eax*8+16]	;	odd[0] = complexData[(i + groupEnd) * 2];
								;	odd[1] = complexData[(i + groupEnd) * 2 + 1];
								;	wk[0] = csin1[0] * odd[0] - csin1[1] * odd[1];
								;	wk[1] = csin1[0] * odd[1] + csin1[1] * odd[0];
	addpd xmm0,xmm1				;	complexData[i * 2] = even[0] + wk[0];
	movapd [ecx+eax*8],xmm0		;	complexData[i * 2 + 1] = even[1] + wk[1];
	subpd xmm2,xmm1				;	complexData[(i + groupEnd) * 2] = even[0] - wk[0];
	movapd [ecx+eax*8+16],xmm2	;	complexData[(i + groupEnd) * 2 + 1] = even[1] - wk[1];
	add eax,4					;	i += 2;
	cmp eax,edx
	jb fftfclop					;}
	

	pxor xmm7,xmm7
	mov eax,0x80000000
	movd xmm0,eax
	punpckldq xmm7,xmm0			;Int32 negv[4] = {0, 0x80000000, 0, 0};
	
	mov dword [esp+0],2
	mov esi,8					;IntOS thisSampleCount = 4;
	fld1
	fidiv dword [esp+0]
	fst qword [esp+16]			; = 0.5
	fldpi
	fchs
	fmulp
	fst qword [esp+8]			;Double kthMul = -2 * Math::PI / 4.0;
	fsin
	fstp qword [esp+0]			;nextSin = Math_Sin(kthMul);
	mov edi,dword [esp+44]		;sampleCount << 1
	shl edi,1
	
	cmp esi,edi
	ja fftfcexit
	align 16					;while (thisSampleCount <= sampleCount)
fftfclop2:						;{
	fld qword [esp+8]
	fmul qword [esp+16]
	fst qword [esp+8]			;	kthMul = kthMul * 0.5;
	movsd xmm1,[esp+0]
	fsin
	fstp qword [esp+0]
	mov eax,-2
	movsd xmm0,[esp+0]
	cvtsi2sd xmm5,eax
	mulsd xmm5,xmm0
	mulsd xmm5,xmm0				;	csin[0] = -2.0 * Math_Sin(kthMul) * Math_Sin(kthMul);
	unpcklpd xmm5,xmm1			;	csin[1] = nextSin;
								;	nextSin = Math_Sin(kthMul);

	mov eax,1
	pxor xmm4,xmm4				;	csin1[1] = 0.0;
	cvtsi2sd xmm4,eax			;	csin1[0] = 1.0;

	mov ecx,dword [esp+40]		;complexData
								;	groupEnd = thisSampleCount >> 1;
	xor ebx,ebx					;	groupCurr = 0;
	align 16					;	while (groupCurr < groupEnd)
fftfclop3:						;	{
	mov edx,ebx					;		i = groupCurr;
	lea eax,[edx+esi]			; (i + groupEnd) * 2
	shr edx,1					; i * 2
	shr eax,1
	
	align 16					;		while (i < sampleCount)
fftfclop4:						;		{
	movapd xmm2,xmm4
	movapd xmm0,[ecx+edx*8]		;			even[0] = complexData[i * 2];
								;			even[1] = complexData[i * 2 + 1];
	movapd xmm3,xmm4
	movapd xmm1,[ecx+eax*8]		;			odd[0] = complexData[(i + groupEnd) * 2];
								;			odd[1] = complexData[(i + groupEnd) * 2 + 1];
	unpcklpd xmm2,xmm2
	unpckhpd xmm3,xmm3
	mulpd xmm2,xmm1
	xorpd xmm1,xmm7
	shufpd xmm1,xmm1,1
	mulpd xmm3,xmm1				;			wk[0] = csin1[0] * odd[0] - csin1[1] * odd[1];
	addpd xmm2,xmm3				;			wk[1] = csin1[0] * odd[1] + csin1[1] * odd[0];
	movapd xmm1,xmm0
	addpd xmm1,xmm2				;			complexData[i * 2] = even[0] + wk[0];
	movapd [ecx+edx*8],xmm1		;			complexData[i * 2 + 1] = even[1] + wk[1];
	subpd xmm0,xmm2				;			complexData[(i + groupEnd) * 2] = even[0] - wk[0];
	movapd [ecx+eax*8],xmm0		;			complexData[(i + groupEnd) * 2 + 1] = even[1] - wk[1];
								;
	lea edx,[edx+esi]			;			i += thisSampleCount;
	lea eax,[eax+esi]
	cmp edx,edi					;		}
	jb fftfclop4				;
	
	
	movapd xmm2,xmm4
	movapd xmm0,xmm5
	movapd xmm1,xmm5
	xorpd xmm2,xmm7	
	unpcklpd xmm0,xmm0
	unpckhpd xmm1,xmm1
	shufpd xmm2,xmm2,1
	mulpd xmm0,xmm4
	mulpd xmm1,xmm2				;		wk[0] = csin1[0];
	addpd xmm4,xmm0				;		csin1[0] = csin[0] * csin1[0] - csin[1] * csin1[1] + csin1[0];
	addpd xmm4,xmm1				;		csin1[1] = csin[0] * csin1[1] + csin[1] *    wk[0] + csin1[1];
								
	add ebx,4					;		groupCurr++;
	cmp ebx,esi					;	}
	jb fftfclop3

	shl esi,1					;	thisSampleCount = thisSampleCount << 1;
	cmp esi,edi
	jbe fftfclop2				;}
	
	align 16
fftfcexit:
	mov ebx,dword [esp+24]
	mov esi,dword [esp+28]
	mov edi,dword [esp+32]
	add esp,36
	ret

;void FFTCalc_ForwardCalcR(Double *complexData, IntOS sampleCount)
;0 tempData
;16 esi
;20 edi
;24 retaddr
;28 complexData
;32 sampleCount

	align 16
_FFTCalc_ForwardCalcR:
	sub esp,8
	mov edx,dword [esp+16]
	shr edx,1
	test edx,1
	jnz fftfcrstart
	mov ecx,dword [esp+12]
	mov dword [esp+0],ecx
	mov dword [esp+4],edx
	cmp edx,4096
	jb fftfcrstart0
	call _FFTCalc_ForwardCalcR ;FFTCalc_ForwardCalcR(complexData, j)
	mov edx,dword [esp+16]
	mov ecx,dword [esp+12]
	lea ecx,[ecx+edx*8]
	shr edx,1
	mov dword [esp+0],ecx
	mov dword [esp+4],edx
	call _FFTCalc_ForwardCalcR ;FFTCalc_ForwardCalcR(&complexData[j * 2], j)
	jmp fftfcrstart
	
	align 16
fftfcrstart0:
	call _FFTCalc_ForwardCalc ;FFTCalc_ForwardCalcR(complexData, j)
	mov edx,dword [esp+16]
	mov ecx,dword [esp+12]
	lea ecx,[ecx+edx*8]
	shr edx,1
	mov dword [esp+0],ecx
	mov dword [esp+4],edx
	call _FFTCalc_ForwardCalc ;FFTCalc_ForwardCalcR(&complexData[j * 2], j)

	align 16
fftfcrstart:
	sub esp,16
	mov dword [esp+16],esi
	mov dword [esp+20],edi

	mov edx,dword [esp+32] ;sampleCount
	cmp edx,4 ;	Double csin[2];
	jb fftfcrlop0a
	mov dword [esp],-2		;if (j > 1)
	fldpi
	fimul dword [esp]		;-2
	fidiv dword [esp+32];	;sampleCount
	fld st0					;Double kthMul = -2 * Math::PI / sampleCount;
	fcos
	fstp qword [esp]
	fsin
	fstp qword [esp+8]		;csin[0] = Math_Cos(kthMul);
	movupd xmm5,[esp]		;csin[1] = Math_Sin(kthMul);
	jmp fftfcrlop0b
	
	align 16
fftfcrlop0a: ;else
	pxor xmm5,xmm5 ;csin[0] = 0.0;
					;csin[1] = 0.0;

	align 16
fftfcrlop0b:
	pxor xmm7,xmm7
	pxor xmm0,xmm0
	mov eax,1
	cvtsi2sd xmm6,eax
	unpcklpd xmm6,xmm0 ;Double csin1[2] = {1.0, 0.0};
	
	mov eax,0x80000000
	movd xmm0,eax
	punpckldq xmm7,xmm0 ;Int32 negv[4] = {0, 0x80000000, 0, 0};

	mov esi,dword [esp+28] ;complexData
	mov ecx,dword [esp+32] ;sampleCount
	lea edi,[esi+ecx*8]
	shr ecx,1
	align 16
fftfcrlop:
	movapd xmm0,[esi]
	movapd xmm2,xmm6
	movapd xmm1,[edi]
	movapd xmm3,xmm6
	unpcklpd xmm2,xmm2
	unpckhpd xmm3,xmm3
	xorpd xmm3,xmm7
	movapd xmm4,xmm1
	mulpd xmm2,xmm1
	shufpd xmm4,xmm1,1
	mulpd xmm3,xmm4
	movapd xmm1,xmm0
	addpd xmm2,xmm3
	addpd xmm0,xmm2
	movapd [esi],xmm0
	subpd xmm1,xmm2
	movapd [edi],xmm1

	lea esi,[esi+16]
	lea edi,[edi+16]

	dec ecx
	jz fftfcrexit

	movapd xmm2,xmm5
	movapd xmm3,xmm5
	unpcklpd xmm2,xmm2
	unpckhpd xmm3,xmm3
	mulpd xmm2,xmm6
	xorpd xmm3,xmm7
	shufpd xmm6,xmm6,1
	mulpd xmm3,xmm6
	addpd xmm2,xmm3
	movapd xmm6,xmm2
	jmp fftfcrlop

	align 16
fftfcrexit:
	mov esi,dword [esp+16]
	mov edi,dword [esp+20]
	add esp,24
	ret
	
;IntOS FFTCalc_Forward(Double *complexData, IntOS sampleCount)
;0 retaddr
;4 complexData
;8 sampleCount

	align 16
_FFTCalc_Forward:
	mov edx,dword [esp+8] ;sampleCount
	cmp edx,1
	jz fftfsucc
	jl fftffail
	lea eax,[edx-1]
	test edx,eax
	jnz fftffail

	mov ecx,dword [esp+4] ;sampleData
	sub esp,8
	mov dword [esp+0],ecx
	mov dword [esp+4],edx
	call _FFTCalc_Rearrange
	
	mov ecx,dword [esp+12]
	mov edx,dword [esp+16]
	mov dword [esp],ecx
	mov dword [esp+4],edx
	cmp edx,4096
	jb fftflop
	call _FFTCalc_ForwardCalcR
	add esp,8
	jmp fftfsucc
	
	align 16
fftflop:
	call _FFTCalc_ForwardCalc
	add esp,8
	
	align 16	
fftfsucc:
	mov eax,1
	ret

	align 16
fftffail:
	xor eax,eax
	ret
