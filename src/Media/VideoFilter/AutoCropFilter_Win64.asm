section .text

global AutoCropFilter_CropCalc

;void AutoCropFilter_CropCalc(UInt8 *yptr, OSInt w, OSInt h, OSInt ySplit, OSInt *crops)
;0 rdi
;8 rsi
;16 rbx
;24 rbp
;32 retAddr
;rcx yptr r10
;rdx w r11
;r8 h
;r9 ySplit
;72 crops

	align 16
AutoCropFilter_CropCalc:
	push rbp
	push rbx
	push rsi
	push rdi
	
	mov r10,rcx
	mov r11,rdx
	mov rdi,qword [rsp+72] ;crops
	mov rsi,r10 ;yptr
	xor rdx,rdx
	mov rbp,qword [rdi+8] ;cropTop
	test rbp,rbp
	jz cblop0
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
	cmp rdx,rbp
	jb ctlop
	align 16
ctlop3:
	mov qword [rdi+8],rdx ;cropTop

	align 16
cblop0:
	mov rsi,r10 ;yptr
	mov rax,r11 ;w
	mul r9
	mov rbp,rax
	mul r8 ;h
	add rsi,rax

	xor rdx,rdx
	cmp qword [rdi+24],0 ;cropBottom
	jz cllop
	align 16
cblop:
	sub rsi,rbp
	mov rcx,r11 ;w
	mov rax,rsi
	align 16
cblop2:
	cmp byte [rax],24
	jnb cblop3
	add rax,r9
	dec rcx
	jnz cblop2
	inc rdx
	cmp rdx,qword [rdi+24] ;cropBottom
	jb cblop
	jmp cllop
	align 16
cblop3:
	mov qword [rdi+24],rdx ;cropBottom

	align 16
cllop:
	xor rdx,rdx
	cmp qword [rdi],0 ;cropLeft
	jz crlop0
	mov rax,r11 ;w
	mul r9
	align 16
cllop2:
	mov rsi,r10 ;yptr
	add rsi,rdx
	mov rcx,r8 ;h
	align 16
cllop3:
	cmp byte [rsi],24
	jnb cllop4
	add rsi,rax
	dec rcx
	jnz cllop3
	inc rdx
	cmp rdx,qword [rdi] ;cropLeft
	jb cllop2
	align 16
cllop4:
	mov qword [rdi],rdx ;cropLeft

	align 16
crlop0:
	mov rax,r11 ;w
	mov rsi,r10 ;yptr
	mul r9
	add rsi,rax
	xor rdx,rdx
	cmp qword [rdi+16],0 ;cropRight
	jz crExit
	align 16
crlop:
	sub rsi,r9 ;ySplit
	mov rcx,r8 ;h
	mov rbx,rsi
	align 16
crlop2:
	cmp byte [rbx],24
	jnb crlop3
	add rbx,rax
	dec rcx
	jnz crlop2
	inc rdx
	cmp rdx,qword [rdi+16] ;cropRight
	jb crlop
	jmp crExit
	align 16
crlop3:
	mov qword [rdi+16],rdx ;cropRight
	
	align 16
crExit:

	pop rdi
	pop rsi
	pop rbx
	pop rbp
	ret
