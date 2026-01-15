section .text

global NearestNeighbourResizer64_64_Resize

;void NearestNeighbourResizer64_64_Resize(UInt8 *inPt, UInt8 *outPt, IntOS dwidth, IntOS dheight, IntOS dbpl, IntOS *xindex, IntOS *yindex);
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
NearestNeighbourResizer64_64_Resize:
	push rbp
	push rbx
	push rsi
	push rdi
	lea rax,[r8*8]
	mov r10,qword [rsp+72]
	mov rdi,qword [rsp+88]
	sub r10,rax
	test rdx,15
	jnz nnr64rlop
	test r8,1
	jz nnr64rlop3
	
	align 16
nnr64rlop:
	mov rbx,qword [rdi]
	mov rsi,qword [rsp+80]
	lea rbx,[rbx+rcx]
	mov rbp,r8
	
	align 16
nnr64rlop2:
	mov rax,qword [rsi]
	lea rsi,[rsi+8]
	mov rax,qword [rbx+rax]
	movnti qword [rdx],rax
	dec rbp
	lea rdx,[rdx+8]
	jnz nnr64rlop2
	
	add rdx,r10
	lea rdi,[rdi+8]
	dec r9
	jnz nnr64rlop
	jmp nnr64rexit
	
	align 16
nnr64rlop3:
	shr r8,1

	align 16
nnr64rlop5:
	mov rbx,qword [rdi]
	mov rsi,qword [rsp+80]
	lea rbx,[rbx+rcx]
	mov rbp,r8
	
	align 16
nnr64rlop6:
	mov rax,qword [rsi]
	movq xmm0,[rbx+rax]
	mov rax,qword [rsi+8]
	lea rsi,[rsi+16]
	movq xmm1,qword [rbx+rax]
	punpcklqdq xmm0,xmm1
	movntdq [rdx],xmm0
	dec rbp
	lea rdx,[rdx+16]
	jnz nnr64rlop6
	
	add rdx,r10
	lea rdi,[rdi+8]
	dec r9
	jnz nnr64rlop5
	
	align 16
nnr64rexit:
	pop rdi
	pop rsi
	pop rbx
	pop rbp
	ret
