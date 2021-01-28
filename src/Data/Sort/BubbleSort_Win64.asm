section .text

global BubbleSort_SortInt32

;void BubbleSort_SortInt32(Int32 *arr, OSInt firstIndex, OSInt lastIndex)
;0 retAddr
;rcx arr
;rdx firstIndex
;r8 lastIndex
	align 16
BubbleSort_SortInt32:
	push rbx
	cmp r8,rdx
	jle bubbleexit
	
	align 16
bubblelop3:
	mov r11,r8
	dec r8
	mov r9,1 ;finished
	lea r10,[rcx+rdx*4] ;b
	sub r11,rdx
	
	align 16
bubblelop:
	mov eax,dword [r10]
	mov ebx,dword [r10+4]
	cmp eax,ebx
	jle bubblelop2
	xor r9,r9
	mov dword [r10+4],eax
	mov dword [r10],ebx
	align 16
bubblelop2:
	lea r10,[r10+4]
	dec r11
	jnz bubblelop
	
	test r9,r9
	jnz bubbleexit
	cmp r8,rdx
	jg bubblelop3

	align 16
bubbleexit:
	pop rbx
	ret
