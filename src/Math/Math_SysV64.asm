%ifidn __OUTPUT_FORMAT__,elf
section .note.GNU-stack noalloc noexec nowrite progbits
%endif
%ifidn __OUTPUT_FORMAT__,elf32
section .note.GNU-stack noalloc noexec nowrite progbits
%endif
%ifidn __OUTPUT_FORMAT__,elf64
section .note.GNU-stack noalloc noexec nowrite progbits
%endif

section .text

global Math_Int32Arr2DblArr
global _Math_Int32Arr2DblArr

;void Math_Int32Arr2DblArr(Double *dblArr, Int32 *intArr, OSInt arrCnt)
;rdi dblArr
;rsi intArr
;rcx arrCnt;
	align 16
Math_Int32Arr2DblArr:
_Math_Int32Arr2DblArr:
	mov rax,rcx
	shr rax,1
	jz i32a2dalop2
	align 16
i32a2dalop:
	movq xmm0,[rsi]
	cvtdq2pd xmm0,xmm0
	movdqu [rdi],xmm0
	lea rsi,[rsi+8]
	lea rdi,[rdi+16]
	dec rax
	jnz i32a2dalop
	test rcx,1
	jz i32a2daexit
	align 16
i32a2dalop2:
	cvtsi2sd xmm0,dword [rsi]
	movq [rdi],xmm0
	align 16
i32a2daexit:
	ret
