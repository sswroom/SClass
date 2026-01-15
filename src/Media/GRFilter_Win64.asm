section .text

global GRFilter_ProcessLayer32H
global GRFilter_ProcessLayer32V
global GRFilter_ProcessLayer32HV
global GRFilter_ProcessLayer64H
global GRFilter_ProcessLayer64V
global GRFilter_ProcessLayer64HV

;void GRFilter_ProcessLayer32H(UInt8 *srcPtr, UInt8 *destPtr, IntOS width, IntOS height, IntOS sbpl, IntOS dbpl, IntOS level, IntOS hOfst, IntOS vOfst);
;0 rdi
;8 rsi
;16 rbx
;24 rbp
;32 retAddr
;rcx srcPtr
;rdx destPtr
;r8 width
;r9 height
;72 sbpl / sAdd
;800 dbpl / dAdd
;88 level
;96 hOfst
;104 vOfst

	align 16
GRFilter_ProcessLayer32H:
	push rbp
	push rbx
	push rsi
	push rdi
	
	; targetOfst rbp
	; loopX rcx
	; loopY ebx
	xor rbp,rbp
	mov rsi,rcx ;srcPtr
	mov rdi,rdx ;destPtr
	mov rcx,r8 ;width
	mov rbx,r9 ;height
	add rsi,qword [rsp+72] ;sbpl
	add rdi,qword [rsp+80] ;dbpl
	dec rcx
	dec rbx
	add rsi,4
	add rdi,4
	mov rax,qword [rsp+96] ;hOfst
	cmp rax,0
	jge pl32hloph
	lea rbp,[rbp+rax*4]
	lea rcx,[rcx+rax+1]
	neg rax
	lea rsi,[rsi+rax*4-4]
	sub rdi,4
	jmp pl32hloph2
	
	align 16
pl32hloph:
	lea rbp,[rbp+rax*4]
	sub rcx,rax
	lea rdi,[rdi+rax*4]
	
	align 16
pl32hloph2:
	mov rax,qword [rsp+104] ;vOfst
	cmp rax,0
	jge pl32hlopv

	lea rbx,[rbx+rax+1]
	mov rdx,qword [rsp+80] ;dbpl
	sub rdi,rdx
	mul rdx
	add rbp,rax
	mov rax,1
	add rax,qword [rsp+104] ;vOfst
	mul qword [rsp+72] ;sbpl
	sub rsi,rax
	jmp pl32hlopv2
	
	align 16
pl32hlopv:	
	sub rbx,rax
	mul qword [rsp+80] ;dbpl
	add rbp,rax
	add rdi,rax

	align 16
pl32hlopv2:
	lea rax,[rcx*4]
	sub qword [rsp+72],rax ;sAdd
	sub qword [rsp+80],rax ;dAdd

	movd xmm4,dword [rsp+88] ;level
	punpcklwd xmm4,xmm4
	punpcklwd xmm4,xmm4
	pxor xmm5,xmm5
	psllw xmm4,8
	align 16
pl32hlop:
	mov rax,rcx
	align 16
pl32hlop2:
	movd xmm0,[rsi]
	movd xmm2,[rsi-4]
	movd xmm3,[rdi]
	punpcklbw xmm0,xmm5
	punpcklbw xmm2,xmm5
	punpcklbw xmm3,xmm5
	psubsw xmm0,xmm2
	pmulhw xmm0,xmm4
	paddsw xmm3,xmm0
	packuswb xmm3,xmm5
	movd [rdi],xmm3
	lea rsi,[rsi+4]
	lea rdi,[rdi+4]
	dec rax
	jnz pl32hlop2
	add rsi,qword [rsp+72] ;sAdd
	add rdi,qword [rsp+80] ;dAdd
	dec rbx
	jnz pl32hlop

	pop rdi
	pop rsi
	pop rbx
	pop rbp
	ret

;void GRFilter_ProcessLayer32V(UInt8 *srcPtr, UInt8 *destPtr, IntOS width, IntOS height, IntOS sbpl, IntOS dbpl, IntOS level, IntOS hOfst, IntOS vOfst);
;0 rdi
;8 rsi
;16 rbx
;24 rbp
;32 retAddr
;rcx srcPtr
;rdx destPtr
;r8 width
;r9 height
;72 sbpl
;80 dbpl
;88 level
;96 hOfst
;104 vOfst

	align 16
GRFilter_ProcessLayer32V:
	push rbp
	push rbx
	push rsi
	push rdi

	; targetOfst ebp
	; loopX ecx
	; loopY ebx
	xor rbp,rbp
	mov rsi,rcx ;srcPtr
	mov rdi,rdx ;destPtr
	mov rcx,r8 ;width
	mov rbx,r9 ;height
	add rsi,qword [rsp+72] ;sbpl
	add rdi,qword [rsp+80] ;dbpl
	dec rcx
	dec rbx
	add rsi,4
	add rdi,4
	mov rax,qword [rsp+96] ;hOfst
	cmp rax,0
	jge pl32vloph
	lea rbp,[rbp+rax*4]
	lea rcx,[rcx+rax+1]
	neg rax
	lea rsi,[rsi+rax*4-4]
	sub rdi,4
	jmp pl32vloph2
	
	align 16
pl32vloph:
	lea rbp,[rbp+rax*4]
	sub rcx,rax
	lea rdi,[rdi+rax*4]
	
	align 16
pl32vloph2:
	mov rax,qword [rsp+104] ;vOfst
	cmp rax,0
	jge pl32vlopv

	lea rbx,[rbx+rax+1]
	mov rdx,qword [rsp+80] ;dbpl
	sub rdi,rdx
	mul rdx
	add rbp,rax
	mov rax,1
	add rax,qword [rsp+104] ;vOfst
	mul qword [rsp+72] ;sbpl
	sub rsi,rax
	jmp pl32vlopv2
	
	align 16
pl32vlopv:	
	sub rbx,rax
	mul qword [rsp+80] ;dbpl
	add rbp,rax
	add rdi,rax

	align 16
pl32vlopv2:
	lea rax,[rcx*4]
	mov rdx,qword [rsp+72] ;sbpl
	sub qword [rsp+72],rax ;sAdd
	sub qword [rsp+80],rax ;dAdd

	neg rdx
	movd xmm4,dword [rsp+88] ;level
	punpcklwd xmm4,xmm4
	punpcklwd xmm4,xmm4
	pxor xmm5,xmm5
	psllw xmm4,8
	align 16
pl32vlop:
	mov rax,rcx
	align 16
pl32vlop2:
	movd xmm0,[rsi]
	movd xmm1,[rsi+rdx]
	movd xmm3,[rdi]
	punpcklbw xmm0,xmm5
	punpcklbw xmm1,xmm5
	punpcklbw xmm3,xmm5
	psubsw xmm0,xmm1
	pmulhw xmm0,xmm4
	paddsw xmm3,xmm0
	packuswb xmm3,xmm5
	movd [rdi],xmm3
	lea esi,[rsi+4]
	lea edi,[rdi+4]
	dec rax
	jnz pl32vlop2
	add esi,dword [rsp+72] ;sAdd
	add edi,dword [rsp+80] ;dAdd
	dec rbx
	jnz pl32vlop

	pop rdi
	pop rsi
	pop rbx
	pop rbp
	ret

;void GRFilter_ProcessLayer32HV(UInt8 *srcPtr, UInt8 *destPtr, IntOS width, IntOS height, IntOS sbpl, IntOS dbpl, IntOS level, IntOS hOfst, IntOS vOfst);
;0 rdi
;8 rsi
;16 rbx
;24 rbp
;32 retAddr
;rcx srcPtr
;rdx destPtr
;r8 width
;r9 height
;72 sbpl
;80 dbpl
;88 level
;96 hOfst
;104 vOfst

	align 16
GRFilter_ProcessLayer32HV:
	push rbp
	push rbx
	push rsi
	push rdi

	; targetOfst ebp
	; loopX ecx
	; loopY ebx
	xor rbp,rbp
	mov rsi,rcx ;srcPtr
	mov rdi,rdx ;destPtr
	mov rcx,r8 ;width
	mov rbx,r9 ;height
	add rsi,qword [rsp+72] ;sbpl
	add rdi,qword [rsp+80] ;dbpl
	dec rcx
	dec rbx
	add rsi,4
	add rdi,4
	mov rax,qword [rsp+96] ;hOfst
	cmp rax,0
	jge pl32hvloph
	lea rbp,[rbp+rax*4]
	lea rcx,[rcx+rax+1]
	neg rax
	lea rsi,[rsi+rax*4-4]
	sub rdi,4
	jmp pl32hvloph2
	
	align 16
pl32hvloph:
	lea rbp,[rbp+rax*4]
	sub rcx,rax
	lea rdi,[rdi+rax*4]
	
	align 16
pl32hvloph2:
	mov rax,qword [rsp+104] ;vOfst
	cmp rax,0
	jge pl32hvlopv

	lea rbx,[rbx+rax+1]
	mov rdx,qword [rsp+80] ;dbpl
	sub rdi,rdx
	mul rdx
	add rbp,rax
	mov rax,1
	add rax,qword [rsp+104] ;vOfst
	mul qword [rsp+72] ;sbpl
	sub rsi,rax
	jmp pl32hvlopv2
	
	align 16
pl32hvlopv:	
	sub rbx,rax
	mul qword [rsp+80] ;dbpl
	add rbp,rax
	add rdi,rax

	align 16
pl32hvlopv2:
	lea rax,[rcx*4]
	mov rdx,qword [rsp+72] ;sbpl
	sub qword [rsp+72],rax ;sAdd
	sub qword [rsp+80],rax ;dAdd

	neg rdx
	movd xmm4,dword [rsp+88] ;level
	punpcklwd xmm4,xmm4
	punpcklwd xmm4,xmm4
	pxor xmm5,xmm5
	psllw xmm4,8
	align 16
pl32hvlop:
	mov rax,rcx
	align 16
pl32hvlop2:
	movd xmm0,[rsi]
	movd xmm1,[rsi+rdx]
	movd xmm2,[rsi-4]
	movd xmm3,[rdi]
	punpcklbw xmm0,xmm5
	punpcklbw xmm1,xmm5
	punpcklbw xmm2,xmm5
	punpcklbw xmm3,xmm5
	paddsw xmm0,xmm0
	psubsw xmm0,xmm1
	psubsw xmm0,xmm2
	pmulhw xmm0,xmm4
	paddsw xmm3,xmm0
	packuswb xmm3,xmm5
	movd [rdi],xmm3
	lea rsi,[rsi+4]
	lea rdi,[rdi+4]
	dec rax
	jnz pl32hvlop2
	add rsi,qword [rsp+72] ;sAdd
	add rdi,qword [rsp+80] ;dAdd
	dec rbx
	jnz pl32hvlop
	
	pop rdi
	pop rsi
	pop rbx
	pop rbp
	ret

;void GRFilter_ProcessLayer64H(UInt8 *srcPtr, UInt8 *destPtr, IntOS width, IntOS height, IntOS sbpl, IntOS dbpl, IntOS level, IntOS hOfst, IntOS vOfst);
;0 edi
;8 esi
;16 ebx
;24 ebp
;32 retAddr
;rcx srcPtr
;rdx destPtr
;r8 width
;r9 height
;72 sbpl
;80 dbpl
;88 level
;96 hOfst
;104 vOfst

	align 16
GRFilter_ProcessLayer64H:
	ret

;void GRFilter_ProcessLayer64V(UInt8 *srcPtr, UInt8 *destPtr, IntOS width, IntOS height, IntOS sbpl, IntOS dbpl, IntOS level, IntOS hOfst, IntOS vOfst);
;0 edi
;8 esi
;16 ebx
;24 ebp
;32 retAddr
;rcx srcPtr
;rdx destPtr
;r8 width
;r9 height
;72 sbpl
;80 dbpl
;88 level
;96 hOfst
;104 vOfst

	align 16
GRFilter_ProcessLayer64V:
	ret

;void GRFilter_ProcessLayer64HV(UInt8 *srcPtr, UInt8 *destPtr, IntOS width, IntOS height, IntOS sbpl, IntOS dbpl, IntOS level, IntOS hOfst, IntOS vOfst);
;0 edi
;8 esi
;16 ebx
;24 ebp
;32 retAddr
;rcx srcPtr
;rdx destPtr
;r8 width
;r9 height
;72 sbpl
;80 dbpl
;88 level
;96 hOfst
;104 vOfst

	align 16
GRFilter_ProcessLayer64HV:
	ret