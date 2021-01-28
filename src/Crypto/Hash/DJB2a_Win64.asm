section .text

global DJB2a_Calc

;UInt32 DJB2a_Calc(const UInt8 *buff, UOSInt buffSize, UInt32 currVal)
;0 retAddr
;rcx buff
;rdx buffSize
;r8 currVal

	align 16
DJB2a_Calc:
	mov rax,r8
	mov r9,rcx
	mov r8,rdx
	align 16
calclop:
	mov edx,eax
	shl eax,5
	add eax,edx
	xor al,byte [r9]
	lea r9,[r9+1]
	
	dec r8
	jnz calclop
	
	ret
