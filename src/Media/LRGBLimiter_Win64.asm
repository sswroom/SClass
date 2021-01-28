section .text

global LRGBLimiter_LimitImageLRGB

;void LRGBLimiter_LimitImageLRGB(UInt8 *imgPtr, OSInt w, OSInt h);
;0 rdi
;8 rsi
;16 rbx
;24 rbp
;32 retAddr
;rcx imgPtr
;rdx w
;r8 h

	align 16
LRGBLimiter_LimitImageLRGB:
	push rbp
	push rbx
	push rsi
	push rdi
	mov rax,rdx ;w
	mov rsi,rcx ;imgPtr
	mul r8 ;h
	mov rbp,rax
	ALIGN 16
lilrgblop:
	movsx rax,word [rsi]
	mov rdx,rax ;minV
	mov rbx,rax ;maxV
	movsx rax,word [rsi+2]
	cmp rdx,rax
	cmovg rdx,rax
	cmp rbx,rax
	cmovl rbx,rax
	movsx rax,word [rsi+4]
	cmp rdx,rax
	cmovg rdx,rax
	cmp rbx,rax
	cmovl rbx,rax

	cmp rdx,16384
	jge lilrgblop5
	cmp rbx,16384
	jle lilrgblop4

	mov rcx,rbx
	mov rdi,rdx
	movsx rax,word [rsi]
	mov rdx,16384
	sub rax,rdi
	sub rdx,rdi
	sub rcx,rdi
	imul rdx
	idiv rcx
	add rax,rdi
	mov word [rsi],ax

	movsx rax,word [rsi+2]
	mov rdx,16384
	sub rax,rdi
	sub rdx,rdi
	imul rdx
	idiv rcx
	add rax,rdi
	mov word [rsi+2],ax

	movsx rax,word [rsi+4]
	mov rdx,16384
	sub rax,rdi
	sub rdx,rdi
	imul rdx
	idiv rcx
	add rax,rdi
	mov word [rsi+4],ax
	jmp lilrgblop4
	ALIGN 16
lilrgblop5:
	mov word [rsi],16383
	mov word [rsi+2],16383
	mov word [rsi+4],16383

	ALIGN 16
lilrgblop4:
	add rsi,8
	dec rbp
	jnz lilrgblop
	pop rdi
	pop rsi
	pop rbx
	pop rbp
	ret