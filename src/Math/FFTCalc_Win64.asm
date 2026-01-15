section .text

global FFTCalc_ApplyWindowI16
global FFTCalc_ApplyWindowI24
global FFTCalc_FFT2Freq
global FFTCalc_Rearrange
global FFTCalc_ForwardCalc
global FFTCalc_ForwardCalcR
global FFTCalc_Forward

extern cos
extern sin
extern Math_PI

;void FFTCalc_ApplyWindowI16(Double *complexOut, UInt8 *sampleIn, Double *sampleWindow, IntOS sampleCnt, IntOS sampleAdd, Double sampleMul)
;0 retaddr
;rcx complexOut
;rdx sampleIn
;r8 sampleWindow
;r9 sampleCnt
;40 sampleAdd
;xmm0 sampleMul

	align 16
FFTCalc_ApplyWindowI16:
	mov r10,rbx
	xchg rsi,r8 ;sampleWindow
	mov rbx,qword [rsp+40] ;sampleAdd
	
	pxor xmm3,xmm3
	unpcklpd xmm0,xmm0 ;sampleMul
	shr r9,1 ;sampleCnt
awi16lop:
	movsx eax,word [rdx]
	movd xmm1,eax
	movsx eax,word [rdx+rbx]
	movd xmm2,eax
	punpckldq xmm1,xmm2
	cvtdq2pd xmm2,xmm1
	mulpd xmm2,xmm0
	mulpd xmm2,[rsi]
	movapd xmm1,xmm2
	unpcklpd xmm1,xmm3
	unpckhpd xmm2,xmm3
	movntpd [rcx],xmm1
	movntpd [rcx+16],xmm2
	lea rdx,[rdx+rbx*2]
	lea rsi,[rsi+16]
	lea rcx,[rcx+32]
	dec r9 ;sampleCnt
	jnz awi16lop

	mov rbx,r10
	mov rsi,r8
	ret

;void FFTCalc_ApplyWindowI24(Double *complexOut, UInt8 *sampleIn, Double *sampleWindow, IntOS sampleCnt, IntOS sampleAdd, Double sampleMul)
;0 retaddr
;rcx complexOut
;rdx sampleIn
;r8 sampleWindow
;r9 sampleCnt
;40 sampleAdd
;xmm0 sampleMul

	align 16
FFTCalc_ApplyWindowI24:
	mov r10,rbx
	xchg rsi,r8 ;sampleWindow
	mov rbx,qword [rsp+40] ;sampleAdd
	
	pxor xmm3,xmm3
	unpcklpd xmm0,xmm0 ;sampleMul
	shr r9,1 ;sampleCnt
awi24lop:
	movsx eax,byte [rdx+2]
	shl eax,16
	mov ax,word [rdx]
	movd xmm1,eax
	movsx eax,byte [rdx+rbx+2]
	shl eax,16
	mov ax,word [rdx+rbx]
	movd xmm2,eax
	punpckldq xmm1,xmm2
	cvtdq2pd xmm2,xmm1
	mulpd xmm2,xmm0
	mulpd xmm2,[rsi]
	movapd xmm1,xmm2
	unpcklpd xmm1,xmm3
	unpckhpd xmm2,xmm3
	movntpd [rcx],xmm1
	movntpd [rcx+16],xmm2
	lea rdx,[rdx+rbx*2]
	lea rsi,[rsi+16]
	lea rcx,[rcx+32]
	dec r9 ;sampleCnt
	jnz awi24lop

	mov rbx,r10
	mov rsi,r8
	ret

;void FFTCalc_FFT2Freq(Double *freq, Double *complexIn, IntOS sampleCnt)
;0 retaddr
;rcx freq
;rdx complexIn
;r8 sampleCnt

	align 16
FFTCalc_FFT2Freq:
	shr r8,1
	test rcx,15
	jz f2freqlop

	align 16
f2freqlop2:
	movapd xmm0,[rdx]
	mulpd xmm0,xmm0
	movapd xmm1,[rdx+16]
	mulpd xmm1,xmm1
	movapd xmm2,xmm0
	unpcklpd xmm0,xmm1
	unpckhpd xmm2,xmm1
	addpd xmm0,xmm2
	sqrtpd xmm0,xmm0
	movupd [rcx], xmm0
	lea rdx,[rdx+32]
	lea rcx,[rcx+16]
	dec r8
	jnz f2freqlop2
	ret

	align 16
f2freqlop:
	movapd xmm0,[rdx]
	mulpd xmm0,xmm0
	movapd xmm1,[rdx+16]
	mulpd xmm1,xmm1
	movapd xmm2,xmm0
	unpcklpd xmm0,xmm1
	unpckhpd xmm2,xmm1
	addpd xmm0,xmm2
	sqrtpd xmm0,xmm0
	movntpd [rcx], xmm0
	lea rdx,[rdx+32]
	lea rcx,[rcx+16]
	dec r8
	jnz f2freqlop
f2freqexit:
	ret
	
;void FFTCalc_Rearrange(Double *complexData, IntOS sampleCount)
;0 rsi
;8 rdi
;16 retaddr
;rcx complexData
;rdx sampleCount
	align 16
FFTCalc_Rearrange:
	push rdi
	push rsi
	
	lea r8,[rdx*2]	;n = sampleCount << 1;
	xor rsi,rsi		;i = 0;
	xor rdi,rdi		;j = 0
	align 16		;while (i < n)
fftrlop:			;{
	cmp rdi,rsi		;	if (j > i)
	jbe fftrlop2	;	{
	movdqa xmm0,[rcx+rsi*8]	;tmpVal = complexData[j];
	movdqa xmm1,[rcx+rdi*8]	;complexData[j] = complexData[i];
	movdqa [rcx+rdi*8],xmm0	;complexData[i] = tmpVal;
	movdqa [rcx+rsi*8],xmm1	;tmpVal = complexData[j + 1];
							;complexData[j + 1] = complexData[i + 1];
							;complexData[i + 1] = tmpVal;
	align 16		;	}
fftrlop2:
	mov r9,rdx		;	m = sampleCount;
	test r9,1		;	while (m >= 2 && j >= m)
	jnz fftrlop3
	cmp rdi,r9
	jb fftrlop3
	align 16
fftrlop4:			;	{
	sub rdi,r9		;		j -= m;
	shr r9,1		;		m >>= 1;
	test r9,1
	jnz fftrlop3
	cmp rdi,r9
	jge fftrlop4
	
	align 16
fftrlop3:			;	}
	add rdi,r9		;	j += m;
	add rsi,2		;	i += 2;
	cmp rsi,r8
	jb fftrlop		;	}

	align 16
fftrexit:
	pop rsi
	pop rdi
	ret

	
;void FFTCalc_ForwardCalc(Double *complexData, IntOS sampleCount)
;0 xmm6
;16 xmm7
;32 xmm8
;48 xmm9
;64 rsi
;72 rdi
;80 retaddr
;88 rcx complexData
;96 rdx sampleCount
	align 16
FFTCalc_ForwardCalc:
	sub rsp,80
	movups [rsp+0],xmm6
	movups [rsp+16],xmm7
	movups [rsp+32],xmm8
	movups [rsp+48],xmm9
	mov qword [rsp+64],rsi
	mov qword [rsp+72],rdi
	mov qword [rsp+88],rcx
	mov qword [rsp+96],rdx
	
								;// thisSampleCount = 2
								;csin1[0] = 1.0;
								;csin1[1] = 0.0;
								;csin[0] = 0.0;
								;csin[1] = 0.0;
								;groupEnd = 1;
	lea r8,[rdx*2]
	xor rax,rax					;i = 0;
	align 16					;while (i < sampleCount)
fftfclop:						;{
	movapd xmm0,[rcx+rax*8]		;	even[0] = complexData[i * 2];
	movapd xmm2,xmm0			;	even[1] = complexData[i * 2 + 1];
	movapd xmm1,[rcx+rax*8+16]	;	odd[0] = complexData[(i + groupEnd) * 2];
								;	odd[1] = complexData[(i + groupEnd) * 2 + 1];
								;	wk[0] = csin1[0] * odd[0] - csin1[1] * odd[1];
								;	wk[1] = csin1[0] * odd[1] + csin1[1] * odd[0];
	addpd xmm0,xmm1				;	complexData[i * 2] = even[0] + wk[0];
	movapd [rcx+rax*8],xmm0		;	complexData[i * 2 + 1] = even[1] + wk[1];
	subpd xmm2,xmm1				;	complexData[(i + groupEnd) * 2] = even[0] - wk[0];
	movapd [rcx+rax*8+16],xmm2	;	complexData[(i + groupEnd) * 2 + 1] = even[1] - wk[1];
	add rax,4					;	i += 2;
	cmp rax,r8
	jb fftfclop					;}
	

	pxor xmm7,xmm7
	mov eax,0x80000000
	movd xmm0,eax
	punpckldq xmm7,xmm0			;Int32 negv[4] = {0, 0x80000000, 0, 0};
	
	mov rsi,8					;IntOS thisSampleCount = 4;
	mov eax,1
	mov edx,2
	cvtsi2sd xmm9,eax
	cvtsi2sd xmm1,edx
	divsd xmm9,xmm1				; = 0.5
	movapd xmm6,xmm9
	xorpd xmm6,xmm7
	mulsd xmm6,[rel Math_PI]	;Double kthMul = -2 * Math::PI / 4.0;
	movapd xmm0,xmm6
	call sin
	movapd xmm8,xmm0			;nextSin = Math_Sin(kthMul);
	mov rdi,qword [rsp+96]		;sampleCount << 1
	shl rdi,1
	
	cmp rsi,rdi
	ja fftfcexit
	align 16					;while (thisSampleCount <= sampleCount)
fftfclop2:						;{
	mulsd xmm6,xmm9				;	kthMul = kthMul * 0.5;
	movapd xmm0,xmm6
	call sin
	mov eax,-2
	cvtsi2sd xmm5,eax
	mulsd xmm5,xmm0
	mulsd xmm5,xmm0				;	csin[0] = -2.0 * Math_Sin(kthMul) * Math_Sin(kthMul);
	unpcklpd xmm5,xmm8			;	csin[1] = nextSin;
	movapd xmm8,xmm0			;	nextSin = Math_Sin(kthMul);

	mov eax,1
	pxor xmm4,xmm4				;	csin1[1] = 0.0;
	cvtsi2sd xmm4,eax			;	csin1[0] = 1.0;

	mov rcx,qword [rsp+88]		;complexData
								;	groupEnd = thisSampleCount >> 1;
	xor r8,r8					;	groupCurr = 0;
	align 16					;	while (groupCurr < groupEnd)
fftfclop3:						;	{
	mov rdx,r8					;		i = groupCurr;
	lea rax,[rdx+rsi]			; (i + groupEnd) * 2
	shr rdx,1					; i * 2
	shr rax,1
	
	align 16					;		while (i < sampleCount)
fftfclop4:						;		{
	movapd xmm2,xmm4
	movapd xmm0,[rcx+rdx*8]		;			even[0] = complexData[i * 2];
								;			even[1] = complexData[i * 2 + 1];
	movapd xmm3,xmm4
	movapd xmm1,[rcx+rax*8]		;			odd[0] = complexData[(i + groupEnd) * 2];
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
	movapd [rcx+rdx*8],xmm1		;			complexData[i * 2 + 1] = even[1] + wk[1];
	subpd xmm0,xmm2				;			complexData[(i + groupEnd) * 2] = even[0] - wk[0];
	movapd [rcx+rax*8],xmm0		;			complexData[(i + groupEnd) * 2 + 1] = even[1] - wk[1];
								;
	lea rdx,[rdx+rsi]			;			i += thisSampleCount;
	lea rax,[rax+rsi]
	cmp rdx,rdi					;		}
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
								
	add r8,4					;		groupCurr++;
	cmp r8,rsi					;	}
	jb fftfclop3

	shl rsi,1					;	thisSampleCount = thisSampleCount << 1;
	cmp rsi,rdi
	jbe fftfclop2				;}
	
	align 16
fftfcexit:
	movups xmm6,[rsp+0]
	movups xmm7,[rsp+16]
	movups xmm8,[rsp+32]
	movups xmm9,[rsp+48]
	mov rsi,qword [rsp+64]
	mov rdi,qword [rsp+72]
	add rsp,80
	ret

;void FFTCalc_ForwardCalcR(Double *complexData, IntOS sampleCount)
;0 xmm6
;16 xmm7
;32 rsi
;40 rdi
;48 retaddr
;56 rcx complexData
;64 rdx sampleCount

	align 16
FFTCalc_ForwardCalcR:
	mov qword [rsp+8],rcx
	mov qword [rsp+16],rdx
	sub rsp,16
	shr rdx,1
	test rdx,1
	jnz fftfcrstart
	cmp rdx,4096
	jb fftfcrstart0
	call FFTCalc_ForwardCalcR ;FFTCalc_ForwardCalcR(complexData, j)
	mov rdx,qword [rsp+32]
	mov rcx,qword [rsp+24]
	lea rcx,[rcx+rdx*8]
	shr rdx,1
	call FFTCalc_ForwardCalcR ;FFTCalc_ForwardCalcR(&complexData[j * 2], j)
	jmp fftfcrstart
	
	align 16
fftfcrstart0:
	call FFTCalc_ForwardCalc ;FFTCalc_ForwardCalcR(complexData, j)
	mov rdx,qword [rsp+32]
	mov rcx,qword [rsp+24]
	lea rcx,[rcx+rdx*8]
	shr rdx,1
	call FFTCalc_ForwardCalc ;FFTCalc_ForwardCalcR(&complexData[j * 2], j)

	align 16
fftfcrstart:
	sub rsp,32
	movups [rsp+0],xmm6
	movups [rsp+16],xmm7
	mov qword [rsp+32],rsi
	mov qword [rsp+40],rdi

	mov rdx,qword [rsp+64] ;sampleCount
	cmp rdx,4 ;	Double csin[2];
	jb fftfcrlop0a
	mov eax,-2	;if (j > 1)
	cvtsi2sd xmm7,eax	;Double kthMul = -2 * Math::PI / sampleCount;
	cvtsi2sd xmm2,rdx
	movsd xmm3,[rel Math_PI]
	mulsd xmm7,xmm3
	divsd xmm7,xmm2
	movapd xmm0,xmm7
	call cos
	movsd xmm6,xmm0
	movapd xmm0,xmm7
	call sin
	unpcklpd xmm6,xmm0		;csin[0] = Math_Cos(kthMul);
	movapd xmm5,xmm6		;csin[1] = Math_Sin(kthMul);
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

	mov rsi,qword [rsp+56] ;complexData
	mov rcx,qword [rsp+64] ;sampleCount
	lea rdi,[rsi+rcx*8]
	shr rcx,1
	align 16
fftfcrlop:
	movapd xmm0,[rsi]
	movapd xmm2,xmm6
	movapd xmm1,[rdi]
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
	movapd [rsi],xmm0
	subpd xmm1,xmm2
	movapd [rdi],xmm1

	lea rsi,[rsi+16]
	lea rdi,[rdi+16]

	dec rcx
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
	movups xmm6,[rsp+0]
	movups xmm7,[rsp+16]
	mov rsi,qword [rsp+32]
	mov rdi,qword [rsp+40]
	add rsp,48
	ret
	
;IntOS FFTCalc_Forward(Double *complexData, IntOS sampleCount)
;0 retaddr
;8 rcx complexData
;16 rdx sampleCount

	align 16
FFTCalc_Forward:
	cmp rdx,1
	jz fftfsucc
	jl fftffail
	lea rax,[rdx-1]
	test rdx,rax
	jnz fftffail

	mov qword [rsp+8],rcx
	mov qword [rsp+16],rdx
	sub rsp,16
	call FFTCalc_Rearrange
	
	mov rcx,qword [rsp+24]
	mov rdx,qword [rsp+32]
	cmp rdx,4096
	jb fftflop
	call FFTCalc_ForwardCalcR
	add rsp,16
	jmp fftfsucc
	
	align 16
fftflop:
	call FFTCalc_ForwardCalc
	add rsp,16
	
	align 16	
fftfsucc:
	mov rax,1
	ret

	align 16
fftffail:
	xor rax,rax
	ret
