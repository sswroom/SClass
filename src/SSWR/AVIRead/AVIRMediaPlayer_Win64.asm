section .text

global AVIRMediaPlayer_VideoCropImageY

;AVIRMediaPlayer_VideoCropImageY(UInt8 *yptr, OSInt w, OSInt h, OSInt ySplit, OSInt *crops)

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
AVIRMediaPlayer_VideoCropImageY:
	push rbp
	push rsi
	push rbx
	push rdi
	mov r10,rcx
	mov r11,rdx
	mov rsi,r10 ;yptr
	mov rbp,qword [rsp+72] ;crops
	xor rdx,rdx
	align 16
ctlop:
	mov rcx,r11 ;w
	align 16
ctlop2:
	cmp byte [rsi],24
	jnb ctlop3
	add rsi,r9
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
	mul r9 ; ySplit
	mov rdi,rax
	mul r8 ; h
	add rsi,rax
	xor rdx,rdx
	align 16
cblop:
	sub rsi,rdi
	mov rcx,r11 ;w
	mov rbx,rsi
	align 16
cblop2:
	cmp byte [rsi],24
	jnb cblop3
	add rsi,r9
	dec rcx
	jnz cblop2
	mov rsi,rbx
	inc rdx
	jmp cblop
	align 16
cblop3:
	mov rsi,rbx
	mov qword [rbp+24],rdx ;cropBottom

	xor rdx,rdx
	align 16
cblop4b:
	mov rsi,r10 ;yptr
	add rsi,rdx
	mov rcx,r8 ;h
	align 16
cblop4:
	cmp byte [rsi],24
	jnb cblop5
	add rsi,r11 ;w
	dec rcx
	jnz cblop4
	inc rdx
	cmp rdx,r11 ;w
	jb cblop4b
	align 16
cblop5:
	mov qword [rbp],rdx ;cropLeft
	
	mov rdx,r11 ;w
	dec rdx
	align 16
cblop6b:
	mov rsi,r10 ;yptr
	add rsi,rdx
	mov rcx,r8 ;h
	align 16
cblop6:
	cmp byte [rsi],24
	jnb cblop7
	add rsi,r11 ;w
	dec rcx
	jnz cblop6
	dec rdx
	cmp rdx,qword [rbp] ;cropLeft
	ja cblop6b
	align 16
cblop7:
	mov rax,r11 ;w
	sub rax,rdx
	dec rax
	mov qword [rbp+16],rax ;cropRight

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

