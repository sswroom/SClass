section .text

global AlphaBlend8_8_DoBlend
global AlphaBlend8_8_DoBlendPA

;void AlphaBlend8_8_DoBlend(UInt8 *dest, IntOS dbpl, UInt8 *src, IntOS sbpl, IntOS width, IntOS height)

;0 rdi
;8 rsi
;16 rbx
;24 rbp
;321 retAddr
;rcx dest r10
;rdx dbpl / dAdd r11
;r8 src
;r9 sbpl / sAdd
;72 width
;80 height

	align 16
AlphaBlend8_8_DoBlend:
	push rbp
	push rbx
	push rsi
	push rdi
	mov rax,qword [rsp+72] ;width
	shl rax,2
	sub r9,rax ;sbpl
	sub rdx,rax ;dbpl
	mov r10,rcx
	mov r11,rdx
	test rax,15
	jnz iadbldstart
	test rcx,15 ;dest
	jnz iadbld4start
	test rdx,15 ;dbpl
	jnz iadbld4start
	jmp iadbld4astart
	
	align 16
iadbldstart:
	mov rbx,0x10101010
	pxor xmm2,xmm2
	movq xmm4,rbx
	punpcklbw xmm4, xmm2

	mov rsi,r8 ;src
	mov rdi,r10 ;dest
	mov rdx,qword [rsp+80] ;height
	ALIGN 16
iadbldlop:

	mov rcx,qword [rsp+72] ;width
	align 16
iadbldlop2:
	mov bl,byte [rsi+3]
	mov bh,bl
	movzx rax,bx
	shl ebx,16
	or rax,rbx

	movq xmm0,rax
	mov rbx,0xffffffff
	sub rbx,rax
	or ebx,0xff000000
	movq xmm3,rbx

	movd xmm1,dword [rsi]
	punpcklbw xmm0, xmm2
	punpcklbw xmm1, xmm2
	pmullw xmm0,xmm1
	movdqa xmm1,xmm0

	movd xmm0,dword [rdi]
	punpcklbw xmm3, xmm2
	punpcklbw xmm0, xmm2
	pmullw xmm0,xmm3
	paddusw xmm0,xmm1

	movdqa xmm1,xmm0
	psrlw xmm1,8
	paddusw xmm0,xmm1
	paddusw xmm0,xmm4

	psrlw xmm0,8
	packuswb xmm0,xmm2
	movd dword [rdi],xmm0

	lea rsi,[rsi+4]
	lea rdi,[rdi+4]
	dec rcx
	jnz iadbldlop2

	add rsi,r9 ;sAdd
	add rdi,r11 ;dAdd
	dec rdx
	jnz iadbldlop
	jmp iadbldexit

	align 16
iadbld4start:
	shr qword [rsp+72],2 ;width
	mov rbx,0x10101010
	pxor xmm2,xmm2
	movq xmm4,rbx
	punpckldq xmm4, xmm4
	punpcklbw xmm4, xmm2

	mov rbx,0xffffffff
	movq xmm6,rbx
	mov rbx,0xff000000
	movq xmm7,rbx
	punpckldq xmm6,xmm6
	punpckldq xmm7,xmm7

	mov rdi,r10 ;dest
	mov rsi,r8 ;src
	mov rdx,qword [rsp+80] ;height
	ALIGN 16
iadbld4lop:
	mov rcx,qword [rsp+72] ;width
	align 16
iadbld4lop2:
	movzx rax,byte [rsi+3]
	mov ah,byte [rsi+7]

	movd xmm0,eax
	punpcklbw xmm0,xmm0
	punpcklwd xmm0,xmm0

	movdqa xmm3,xmm6
	psubd xmm3,xmm0
	por xmm3,xmm7

	movq xmm1,[rsi]
	punpcklbw xmm0, xmm2
	punpcklbw xmm1, xmm2
	pmullw xmm0,xmm1
	movdqa xmm1,xmm0

	movq xmm0,[rdi]
	punpcklbw xmm3, xmm2
	punpcklbw xmm0, xmm2
	pmullw xmm0,xmm3
	paddusw xmm0,xmm1

	movdqa xmm1,xmm0
	psrlw xmm1,8
	paddusw xmm0,xmm1
	paddusw xmm0,xmm4

	movzx rax,byte [rsi+11]
	mov ah,byte [rsi+15]

	movd xmm5,eax
	punpcklbw xmm5,xmm5
	punpcklwd xmm5,xmm5

	movdqa xmm3,xmm6
	psubd xmm3,xmm5
	por xmm3,xmm7

	movq xmm1,[rsi+8]
	punpcklbw xmm5,xmm2
	punpcklbw xmm1,xmm2
	pmullw xmm5,xmm1
	movdqa xmm1,xmm5

	movq xmm5,[rdi+8]
	punpcklbw xmm3,xmm2
	punpcklbw xmm5,xmm2
	pmullw xmm5,xmm3
	paddusw xmm5,xmm1

	movdqa xmm1,xmm5
	psrlw xmm1,8
	paddusw xmm5,xmm1
	paddusw xmm5,xmm4

	psrlw xmm0,8
	psrlw xmm5,8
	packuswb xmm0,xmm5
	movdqu [rdi],xmm0

	lea rsi,[rsi+16]
	lea rdi,[rdi+16]
	dec rcx
	jnz iadbld4lop2

	add rsi,r9 ;sAdd
	add rdi,r11 ;dAdd
	dec rdx
	jnz iadbld4lop
	jmp iadbldexit
	
	align 16
iadbld4astart:
	shr qword [rsp+72],2 ;width
	mov rbx,0x10101010
	pxor xmm2,xmm2
	movd xmm4,ebx
	punpckldq xmm4, xmm4
	punpcklbw xmm4, xmm2

	mov rbx,0xffffffff
	movd xmm6,ebx
	mov ebx,0xff000000
	movd xmm7,ebx
	punpckldq xmm6,xmm6
	punpckldq xmm7,xmm7

	mov rdi,r10 ;dest
	mov rsi,r8 ;src
	mov rdx,qword [rsp+80] ;height
	ALIGN 16
iadbld4alop:

	mov rcx,qword [rsp+72] ;width
	align 16
iadbld4alop2:
	movzx rax,byte [rsi+3]
	mov ah,byte [rsi+7]

	movd xmm0,eax
	punpcklbw xmm0,xmm0
	punpcklwd xmm0,xmm0

	movdqa xmm3,xmm6
	psubd xmm3,xmm0
	por xmm3,xmm7

	movq xmm1,[rsi]
	punpcklbw xmm0, xmm2
	punpcklbw xmm1, xmm2
	pmullw xmm0,xmm1
	movdqa xmm1,xmm0

	movq xmm0,[rdi]
	punpcklbw xmm3, xmm2
	punpcklbw xmm0, xmm2
	pmullw xmm0,xmm3
	paddusw xmm0,xmm1

	movdqa xmm1,xmm0
	psrlw xmm1,8
	paddusw xmm0,xmm1
	paddusw xmm0,xmm4

	movzx rax,byte [rsi+11]
	mov ah,byte [rsi+15]

	movd xmm5,eax
	punpcklbw xmm5,xmm5
	punpcklwd xmm5,xmm5

	movdqa xmm3,xmm6
	psubd xmm3,xmm5
	por xmm3,xmm7

	movq xmm1,[rsi+8]
	punpcklbw xmm5,xmm2
	punpcklbw xmm1,xmm2
	pmullw xmm5,xmm1
	movdqa xmm1,xmm5

	movq xmm5,[rdi+8]
	punpcklbw xmm3,xmm2
	punpcklbw xmm5,xmm2
	pmullw xmm5,xmm3
	paddusw xmm5,xmm1

	movdqa xmm1,xmm5
	psrlw xmm1,8
	paddusw xmm5,xmm1
	paddusw xmm5,xmm4

	psrlw xmm0,8
	psrlw xmm5,8
	packuswb xmm0,xmm5
	movntdq [rdi],xmm0

	lea rsi,[rsi+16]
	lea rdi,[rdi+16]
	dec rcx
	jnz iadbld4alop2

	add rsi,r9 ;sAdd
	add rdi,r11 ;dAdd
	dec rdx
	jnz iadbld4alop

	align 16
iadbldexit:
	pop rdi
	pop rsi
	pop rbx
	pop rbp
	ret
	
;void AlphaBlend8_8_DoBlendPA(UInt8 *dest, IntOS dbpl, UInt8 *src, IntOS sbpl, IntOS width, IntOS height)

;0 rdi
;8 rsi
;16 rbx
;24 rbp
;32 retAddr
;rcx dest r10
;rdx dbpl r11
;r8 src
;r9 sbpl
;72 width
;80 height

	align 16
AlphaBlend8_8_DoBlendPA:
	push rbp
	push rbx
	push rsi
	push rdi
	xor rax,rax
	mov rbx,0x10101010
	
	movd xmm2,eax
	movd xmm4,ebx
	punpcklbw xmm4, xmm2
	mov rax,0xffffffff
	movd xmm5,eax
	punpcklbw xmm5,xmm2
	mov r10,rcx
	mov r11,rdx

	mov rdx,qword [rsp+80] ;height
	ALIGN 16
iabpabldlop:
	mov rsi,r8 ;src
	mov rdi,r10 ;dest

	mov rcx,qword [rsp+72] ;width
	align 16
iadpabldlop2:
	mov bl,byte [rsi+3]
	mov bh,bl
	movzx rax,bx
	shl ebx,16
	or rax,rbx

	movd xmm0,eax
	mov rbx,0xffffffff
	sub rbx,rax
	or ebx,0xff000000
	movd xmm3,ebx

	movd xmm1,dword [rsi]
	punpcklbw xmm1, xmm2
	pmullw xmm1,xmm5

	movd xmm0,dword [rdi]
	punpcklbw xmm3, xmm2
	punpcklbw xmm0, xmm2
	pmullw xmm0,xmm3
	paddusw xmm0,xmm1

	movq xmm1,xmm0
	psrlw xmm1,8
	paddusw xmm0,xmm1
	paddusw xmm0,xmm4

	psrlw xmm0,8
	packuswb xmm0,xmm2
	movd dword [rdi],xmm0

	lea rsi,[rsi+4]
	lea rdi,[rdi+4]
	dec rcx
	jnz iadpabldlop2

	add r8,r9 ;src
	add r10,r11 ;dest
	dec rdx
	jnz iabpabldlop
	
	pop rdi
	pop rsi
	pop rbx
	pop rbp
	ret