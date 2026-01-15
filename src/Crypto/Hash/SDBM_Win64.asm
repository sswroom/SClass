section .text

global SDBM_Calc

;UInt32 SDBM_Calc(const UInt8 *buff, UIntOS buffSize, UInt32 currVal)
;0 retAddr
;rcx buff
;rdx buffSize
;r8 currVal

	align 16
SDBM_Calc:
	mov rax,r8
	mov r9,rcx
	mov r8,rdx
	mov edx,65599
	align 16
calclop:
	movzx ecx,byte [r9]
	imul eax,edx
	lea r9,[r9+1]
	add eax,ecx
	dec r8
	jnz calclop
	
	ret
