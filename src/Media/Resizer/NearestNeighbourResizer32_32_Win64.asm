section .text

global NearestNeighbourResizer32_32_Resize

;void NearestNeighbourResizer32_32_Resize(UInt8 *inPt, UInt8 *outPt, OSInt dwidth, OSInt dheight, OSInt dbpl, OSInt *xindex, OSInt *yindex);
;0 rdi
;8 rsi
;16 rbx
;24 rbp
;32 retAddr
;rcx inPt
;rdx outPt
;r8 dwidth
;r9 dheight
;72 dbpl r10
;80 xindex
;88 yindex rdi

	align 16
NearestNeighbourResizer32_32_Resize:
	push rbp
	push rbx
	push rsi
	push rdi
	lea rax,[r8*4]
	mov r10,qword [rsp+72]
	mov rdi,qword [rsp+88]
	sub r10,rax
	test rdx,15
	jnz nnr32rlop
	test r8,3
	jz nnr32rlop3
	
	align 16
nnr32rlop:
	mov rbx,qword [rdi]
	mov rsi,qword [rsp+80]
	lea rbx,[rbx+rcx]
	mov rbp,r8
	
	align 16
nnr32rlop2:
	mov rax,qword [rsi]
	lea rsi,[rsi+8]
	mov eax,dword [rbx+rax]
	movnti dword [rdx],eax
	dec rbp
	lea rdx,[rdx+4]
	jnz nnr32rlop2
	
	add rdx,r10
	lea rdi,[rdi+8]
	dec r9
	jnz nnr32rlop
	jmp nnr32rexit
	
	align 16
nnr32rlop3:
	shr r8,2

	align 16
nnr32rlop5:
	mov rbx,qword [rdi]
	mov rsi,qword [rsp+80]
	lea rbx,[rbx+rcx]
	mov rbp,r8
	
	align 16
nnr32rlop6:
	mov rax,qword [rsi]
	movd xmm0,dword [rbx+rax]
	mov rax,qword [rsi+16]
	movd xmm2,dword [rbx+rax]
	mov rax,qword [rsi+8]
	punpckldq xmm0,xmm2
	movd xmm1,dword [rbx+rax]
	mov rax,qword [rsi+24]
	lea rsi,[rsi+32]
	movd xmm3,dword [rbx+rax]
	punpckldq xmm1,xmm3
	punpckldq xmm0,xmm1
	movntdq [rdx],xmm0
	dec rbp
	lea rdx,[rdx+16]
	jnz nnr32rlop6
	
	add rdx,r10
	lea rdi,[rdi+8]
	dec r9
	jnz nnr32rlop5
	
	align 16
nnr32rexit:
	pop rdi
	pop rsi
	pop rbx
	pop rbp
	ret
