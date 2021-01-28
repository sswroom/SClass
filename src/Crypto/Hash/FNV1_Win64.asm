section .text

global FNV1_Calc

;UInt32 FNV1_Calc(const UInt8 *buff, UOSInt buffSize, UInt32 currVal)
;0 retAddr
;rcx buff
;rdx buffSize
;r8 currVal

	align 16
FNV1_Calc:
	mov rax,r8
	mov r9,rcx
	mov r8,rdx
	mov edx,16777619
	align 16
calclop:
	imul eax,edx
	xor al,byte [r9]
	lea r9,[r9+1]
	dec r8
	jnz calclop
	
	ret
