section .text

global DJB2_Calc

;UInt32 DJB2_Calc(const UInt8 *buff, UIntOS buffSize, UInt32 currVal)
;0 retAddr
;rcx buff
;rdx buffSize
;r8 currVal

	align 16
DJB2_Calc:
	mov rax,r8
	mov r9,rcx
	mov r8,rdx
	align 16
calclop:
	mov edx,eax
	movzx ecx,byte [r9]
	shl eax,5
	lea r9,[r9+1]
	add eax,edx
	add eax,ecx
	
	dec r8
	jnz calclop
	
	ret
