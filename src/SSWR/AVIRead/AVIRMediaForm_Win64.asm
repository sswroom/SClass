section .text

global AVIRMediaForm_VideoCropImageY

;AVIRMediaForm_VideoCropImageY(UInt8 *yptr, OSInt w, OSInt h, OSInt ySplit, OSInt *crops)

;0 rdi
;8 rbx
;16 rsi
;24 rbp
;32 retAddr
;rcx yptr r10
;rdx w r11
;r8 h
;r9 ySplit
;72 crops
	align 16
AVIRMediaForm_VideoCropImageY:
	push rbp
	push rsi
	push rbx
	push rdi
	mov r10,rcx
	mov r11,rdx
	mov rbp,qword [rsp+72] ;crops
	mov rsi,rcx ;yptr
	xor rdx,rdx
	mov rbx,r9 ;ySplit
	align 16
ctlop:
	mov rcx,r11 ;w
	align 16
ctlop2:
	cmp byte [rsi],18
	jnb ctlop3
	add rsi,rbx
	dec rcx
	jnz ctlop2
	inc rdx
	cmp rdx,r8 ;h
	jb ctlop
	jmp cropError
	align 16
ctlop3:
	mov qword [rbp+8],rdx ;cropTop
	mov rsi,r10 ;yptr
	mov rax,r11 ;w
	mul r9 ;ySplit
	mov rdi,rax
	mul r8 ;h
	add rsi,rax
	xor rdx,rdx
	align 16
cblop:
	sub rsi,rdi
	mov rcx,r11 ;w
	mov r10,rsi
	align 16
cblop2:
	cmp byte [rsi],18
	jnb cblop3
	add rsi,rbx
	dec rcx
	jnz cblop2
	mov rsi,r10
	inc rdx
	jmp cblop
	align 16
cblop3:
	mov rsi,r10
	mov qword [rbp+24],rdx ;cropBottom
	mov qword [rbp],0 ;cropLeft
	mov qword [rbp+16],0 ;cropRight
	jmp cropExit
	align 16
cropError:
	mov qword [rbp],0 ;cropLeft
	mov qword [rbp+8],0 ;cropTop
	mov qword [rbp+16],0 ;cropRight
	mov qword [rbp+24],0 ;cropBottom
	align 16
cropExit:
	pop rdi
	pop rbx
	pop rsi
	pop rbp
	ret

