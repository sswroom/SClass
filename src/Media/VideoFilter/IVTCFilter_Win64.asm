section .text

global IVTCFilter_CalcField
global IVTCFilter_CalcFieldP

;void IVTCFilter_CalcField(UInt8 *oddPtr, UInt8 *evenPtr, OSInt w, OSInt h, Int32 *fieldStats)
;0 rdi
;8 rsi
;16 rbx
;24 rbp
;32 retAddr
;rcx oddPtr
;rdx evenPtr
;r8 w
;r9 h
;72 fieldStats

	align 16
IVTCFilter_CalcField:
	push rbp
	push rbx
	push rsi
	push rdi
	mov rax,r9 ;h
	sub rax,2
	shr rax,1
	mov rsi,rcx ;oddPtr
	mov rdx,rdx ;evenPtr
	mov r9,rax ;hLeft
	pxor xmm7,xmm7
	pxor xmm5,xmm5 ;fieldMDiff
	pxor xmm6,xmm6 ;field2MDiff
	align 16
cfslop:
	mov rbp,r8 ;w
	shr rbp,4
	align 16
cfslop2:
	mov rdi,r8 ;w
	movdqu xmm0,[rsi]
	movdqu xmm1,[rsi+rdi*2]
	movdqu xmm3,[rdx+rdi]
	movdqu xmm2,xmm0
	pmaxub xmm0,xmm1
	pminub xmm1,xmm2
	movdqu xmm2,xmm3
	pmaxub xmm3,xmm0
	pminub xmm2,xmm1
	psubusb xmm3,xmm0
	movdqu xmm4,xmm1
	psubusb xmm4,xmm2
	paddb xmm3,xmm4 ;v4
	movdqu xmm2,xmm3
	psubusb xmm0,xmm1
	psubusb xmm2,xmm0 ;v5
	pmaxub xmm5,xmm3
	pmaxub xmm6,xmm2

	xor ecx,ecx ;fieldDiff
	xor ebx,ebx ;fieldCnt
	pextrw eax,xmm3,0
	movzx edi,al
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,ah
	add ecx,edi
	pextrw eax,xmm3,1
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,al
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,ah
	add ecx,edi
	pextrw eax,xmm3,2
	shr eax,16
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,al
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,ah
	add ecx,edi
	pextrw eax,xmm3,3
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,al
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,ah
	add ecx,edi
	pextrw eax,xmm3,4
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,al
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,ah
	add ecx,edi
	pextrw eax,xmm3,5
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,al
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,ah
	add ecx,edi
	pextrw eax,xmm3,6
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,al
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,ah
	add ecx,edi
	pextrw eax,xmm3,7
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,al
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,ah
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0

	movd xmm4,ecx
	movd xmm1,ebx
	punpckldq xmm4,xmm1

	xor ecx,ecx ;field2Diff
	xor ebx,ebx ;field2Cnt

	pextrw eax,xmm2,0
	movzx edi,al
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,ah
	add ecx,edi
	pextrw eax,xmm2,1
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,al
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,ah
	add ecx,edi
	pextrw eax,xmm2,2
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,al
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,ah
	add ecx,edi
	pextrw eax,xmm2,3
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,al
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,ah
	add ecx,edi
	pextrw eax,xmm2,4
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,al
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,ah
	add ecx,edi
	pextrw eax,xmm2,5
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,al
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,ah
	add ecx,edi
	pextrw eax,xmm2,6
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,al
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,ah
	add ecx,edi
	pextrw eax,xmm2,7
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,al
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,ah
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0

	movd xmm0,ecx
	movd xmm1,ebx
	punpckldq xmm0,xmm1
	punpcklqdq xmm4,xmm0
	paddd xmm7,xmm4

	mov rdi,r8 ;w
	lea rax,[rdi*2+rdi]
	movdqu xmm0,[rdx+rdi]
	movdqu xmm1,[rdx+rax]
	movdqu xmm3,[rsi+rdi*2]
	movdqu xmm2,xmm0
	pmaxub xmm0,xmm1
	pminub xmm1,xmm2
	movdqu xmm2,xmm3
	pmaxub xmm3,xmm0
	pminub xmm2,xmm1
	psubusb xmm3,xmm0
	movdqu xmm4,xmm1
	psubusb xmm4,xmm2
	paddb xmm3,xmm4 ;v4
	movdqu xmm2,xmm3
	psubusb xmm0,xmm1
	psubusb xmm2,xmm0 ;v5
	pmaxub xmm5,xmm3
	pmaxub xmm6,xmm2

	xor ecx,ecx ;fieldDiff
	xor ebx,ebx ;fieldCnt
	pextrw eax,xmm3,0
	movzx edi,al
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,ah
	add ecx,edi
	pextrw eax,xmm3,1
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,al
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,ah
	add ecx,edi
	pextrw eax,xmm3,2
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,al
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,ah
	add ecx,edi
	pextrw eax,xmm3,3
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,al
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,ah
	add ecx,edi
	pextrw eax,xmm3,4
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,al
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,ah
	add ecx,edi
	pextrw eax,xmm3,5
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,al
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,ah
	add ecx,edi
	pextrw eax,xmm3,6
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,al
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,ah
	add ecx,edi
	pextrw eax,xmm3,7
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,al
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,ah
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0

	movd xmm4,ecx
	movd xmm1,ebx
	punpckldq xmm4,xmm1

	xor ecx,ecx ;field2Diff
	xor ebx,ebx ;field2Cnt

	pextrw eax,xmm2,0
	movzx edi,al
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,ah
	add ecx,edi
	pextrw eax,xmm2,1
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,al
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,ah
	add ecx,edi
	pextrw eax,xmm2,2
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,al
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,ah
	add ecx,edi
	pextrw eax,xmm2,3
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,al
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,ah
	add ecx,edi
	pextrw eax,xmm2,4
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,al
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,ah
	add ecx,edi
	pextrw eax,xmm2,5
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,al
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,ah
	add ecx,edi
	pextrw eax,xmm2,6
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,al
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,ah
	add ecx,edi
	pextrw eax,xmm2,7
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,al
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,ah
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0

	movd xmm0,ecx
	movd xmm1,ebx
	punpckldq xmm0,xmm1
	punpcklqdq xmm4,xmm0
	paddd xmm7,xmm4

	add rsi,16
	add rdx,16
	dec rbp
	jnz cfslop2

	mov rdi,r8 ;w
	add rsi,rdi
	add rdx,rdi
	dec r9 ;hLeft
	jnz cfslop

	mov rbp,qword [rsp+72] ;fieldStats
	movdqu [rbp],xmm7
	pextrw eax,xmm5,0
	movzx edx,ah
	movzx ebx,al
	cmp edx,ebx
	cmovb edx,ebx
	pextrw eax,xmm5,1
	movzx ebx,ah
	movzx eax,al
	cmp edx,ebx
	cmovb edx,ebx
	cmp edx,eax
	cmovb edx,eax
	pextrw eax,xmm5,2
	movzx ebx,ah
	movzx eax,al
	cmp edx,ebx
	cmovb edx,ebx
	cmp edx,eax
	cmovb edx,eax
	pextrw eax,xmm5,3
	movzx ebx,ah
	movzx eax,al
	cmp edx,ebx
	cmovb edx,ebx
	cmp edx,eax
	cmovb edx,eax
	pextrw eax,xmm5,4
	movzx ebx,ah
	movzx eax,al
	cmp edx,ebx
	cmovb edx,ebx
	cmp edx,eax
	cmovb edx,eax
	pextrw eax,xmm5,5
	movzx ebx,ah
	movzx eax,al
	cmp edx,ebx
	cmovb edx,ebx
	cmp edx,eax
	cmovb edx,eax
	pextrw eax,xmm5,6
	movzx ebx,ah
	movzx eax,al
	cmp edx,ebx
	cmovb edx,ebx
	cmp edx,eax
	cmovb edx,eax
	pextrw eax,xmm5,7
	movzx ebx,ah
	movzx eax,al
	cmp edx,ebx
	cmovb edx,ebx
	cmp edx,eax
	cmovb edx,eax
	mov dword [rbp+16],edx ;fieldMDiff

	pextrw eax,xmm6,0
	movzx edx,ah
	movzx ebx,al
	cmp edx,ebx
	cmovb edx,ebx
	pextrw eax,xmm6,1
	movzx ebx,ah
	movzx eax,al
	cmp edx,ebx
	cmovb edx,ebx
	cmp edx,eax
	cmovb edx,eax
	pextrw eax,xmm6,2
	movzx ebx,ah
	movzx eax,al
	cmp edx,ebx
	cmovb edx,ebx
	cmp edx,eax
	cmovb edx,eax
	pextrw eax,xmm6,3
	movzx ebx,ah
	movzx eax,al
	cmp edx,ebx
	cmovb edx,ebx
	cmp edx,eax
	cmovb edx,eax
	pextrw eax,xmm6,4
	movzx ebx,ah
	movzx eax,al
	cmp edx,ebx
	cmovb edx,ebx
	cmp edx,eax
	cmovb edx,eax
	pextrw eax,xmm6,5
	movzx ebx,ah
	movzx eax,al
	cmp edx,ebx
	cmovb edx,ebx
	cmp edx,eax
	cmovb edx,eax
	pextrw eax,xmm6,6
	movzx ebx,ah
	movzx eax,al
	cmp edx,ebx
	cmovb edx,ebx
	cmp edx,eax
	cmovb edx,eax
	pextrw eax,xmm6,7
	movzx ebx,ah
	movzx eax,al
	cmp edx,ebx
	cmovb edx,ebx
	cmp edx,eax
	cmovb edx,eax
	mov dword [rbp+20],edx ;field2MDiff
	pop rdi
	pop rsi
	pop rbx
	pop rbp
	ret

;void IVTCFilter_CalcFieldP(UInt8 *framePtr, OSInt w, OSInt h, Int32 *fieldStats)
;0 rdi
;8 rsi
;16 rbx
;24 rbp
;32 retAddr
;rcx framePtr
;rdx w r10
;r8 h
;r9 fieldStats

	align 16
IVTCFilter_CalcFieldP:
	push rbp
	push rbx
	push rsi
	push rdi
	mov rsi,rcx ;framePtr
	mov rax,r8 ;h
	sub rax,2
	mul rdx ;w
	shr rax,4
	mov rbp,rax ;OSInt wLeft = w * (h - 2) >> 4;

;	mov rdx,rdx ;w
	pxor xmm7,xmm7
	pxor xmm5,xmm5 ;fieldMDiff
	pxor xmm6,xmm6 ;field2MDiff
	align 16
cfsplop:
	movdqu xmm0,[rsi]
	movdqu xmm1,[rsi+rdx*2]
	movdqu xmm3,[rsi+rdx]
	movdqu xmm2,xmm0
	pmaxub xmm0,xmm1
	pminub xmm1,xmm2
	movdqu xmm2,xmm3
	pmaxub xmm3,xmm0
	pminub xmm2,xmm1
	psubusb xmm3,xmm0
	movdqu xmm4,xmm1
	psubusb xmm4,xmm2
	paddb xmm3,xmm4 ;v4
	movdqu xmm2,xmm3
	psubusb xmm0,xmm1
	psubusb xmm2,xmm0 ;v5
	pmaxub xmm5,xmm3
	pmaxub xmm6,xmm2

	xor ecx,ecx ;fieldDiff
	xor ebx,ebx ;fieldCnt
	pextrw eax,xmm3,0
	movzx edi,al
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,ah
	add ecx,edi
	pextrw eax,xmm3,1
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,al
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,ah
	add ecx,edi
	pextrw eax,xmm3,2
	shr eax,16
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,al
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,ah
	add ecx,edi
	pextrw eax,xmm3,3
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,al
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,ah
	add ecx,edi
	pextrw eax,xmm3,4
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,al
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,ah
	add ecx,edi
	pextrw eax,xmm3,5
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,al
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,ah
	add ecx,edi
	pextrw eax,xmm3,6
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,al
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,ah
	add ecx,edi
	pextrw eax,xmm3,7
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,al
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,ah
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0

	movd xmm4,ecx
	movd xmm1,ebx
	punpckldq xmm4,xmm1

	xor ecx,ecx ;field2Diff
	xor ebx,ebx ;field2Cnt

	pextrw eax,xmm2,0
	movzx edi,al
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,ah
	add ecx,edi
	pextrw eax,xmm2,1
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,al
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,ah
	add ecx,edi
	pextrw eax,xmm2,2
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,al
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,ah
	add ecx,edi
	pextrw eax,xmm2,3
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,al
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,ah
	add ecx,edi
	pextrw eax,xmm2,4
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,al
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,ah
	add ecx,edi
	pextrw eax,xmm2,5
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,al
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,ah
	add ecx,edi
	pextrw eax,xmm2,6
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,al
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,ah
	add ecx,edi
	pextrw eax,xmm2,7
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,al
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,ah
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0

	movd xmm0,ecx
	movd xmm1,ebx
	punpckldq xmm0,xmm1
	punpcklqdq xmm4,xmm0
	paddd xmm7,xmm4

	add rsi,16
	dec rbp
	jnz cfsplop

	movdqu [r9],xmm7  ;fieldStats
	pextrw eax,xmm5,0
	movzx edx,ah
	movzx ebx,al
	cmp edx,ebx
	cmovb edx,ebx
	pextrw eax,xmm5,1
	movzx ebx,ah
	movzx eax,al
	cmp edx,ebx
	cmovb edx,ebx
	cmp edx,eax
	cmovb edx,eax
	pextrw eax,xmm5,2
	movzx ebx,ah
	movzx eax,al
	cmp edx,ebx
	cmovb edx,ebx
	cmp edx,eax
	cmovb edx,eax
	pextrw eax,xmm5,3
	movzx ebx,ah
	movzx eax,al
	cmp edx,ebx
	cmovb edx,ebx
	cmp edx,eax
	cmovb edx,eax
	pextrw eax,xmm5,4
	movzx ebx,ah
	movzx eax,al
	cmp edx,ebx
	cmovb edx,ebx
	cmp edx,eax
	cmovb edx,eax
	pextrw eax,xmm5,5
	movzx ebx,ah
	movzx eax,al
	cmp edx,ebx
	cmovb edx,ebx
	cmp edx,eax
	cmovb edx,eax
	pextrw eax,xmm5,6
	movzx ebx,ah
	movzx eax,al
	cmp edx,ebx
	cmovb edx,ebx
	cmp edx,eax
	cmovb edx,eax
	pextrw eax,xmm5,7
	movzx ebx,ah
	movzx eax,al
	cmp edx,ebx
	cmovb edx,ebx
	cmp edx,eax
	cmovb edx,eax
	mov dword [r9+16],edx ;fieldMDiff

	pextrw eax,xmm6,0
	movzx edx,ah
	movzx ebx,al
	cmp edx,ebx
	cmovb edx,ebx
	pextrw eax,xmm6,1
	movzx ebx,ah
	movzx eax,al
	cmp edx,ebx
	cmovb edx,ebx
	cmp edx,eax
	cmovb edx,eax
	pextrw eax,xmm6,2
	movzx ebx,ah
	movzx eax,al
	cmp edx,ebx
	cmovb edx,ebx
	cmp edx,eax
	cmovb edx,eax
	pextrw eax,xmm6,3
	movzx ebx,ah
	movzx eax,al
	cmp edx,ebx
	cmovb edx,ebx
	cmp edx,eax
	cmovb edx,eax
	pextrw eax,xmm6,4
	movzx ebx,ah
	movzx eax,al
	cmp edx,ebx
	cmovb edx,ebx
	cmp edx,eax
	cmovb edx,eax
	pextrw eax,xmm6,5
	movzx ebx,ah
	movzx eax,al
	cmp edx,ebx
	cmovb edx,ebx
	cmp edx,eax
	cmovb edx,eax
	pextrw eax,xmm6,6
	movzx ebx,ah
	movzx eax,al
	cmp edx,ebx
	cmovb edx,ebx
	cmp edx,eax
	cmovb edx,eax
	pextrw eax,xmm6,7
	movzx ebx,ah
	movzx eax,al
	cmp edx,ebx
	cmovb edx,ebx
	cmp edx,eax
	cmovb edx,eax
	mov dword [r9+20],edx ;field2MDiff
	
	pop rdi
	pop rsi
	pop rbx
	pop rbp
	ret
