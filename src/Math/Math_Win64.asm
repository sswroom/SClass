section .text

global Math_Int32Arr2DblArr

;void Math_Int32Arr2DblArr(Double *dblArr, Int32 *intArr, OSInt arrCnt)
;rcx dblArr
;rdx intArr
;r8 arrCnt;
	align 16
Math_Int32Arr2DblArr:
	mov rax,r8
	shr rax,1
	jz i32a2dalop2
	align 16
i32a2dalop:
	movq xmm0,[rdx]
	cvtdq2pd xmm0,xmm0
	movdqu [rcx],xmm0
	lea rdx,[rdx+8]
	lea rcx,[rcx+16]
	dec rax
	jnz i32a2dalop
	test r8,1
	jz i32a2daexit
	align 16
i32a2dalop2:
	cvtsi2sd xmm0,dword [rdx]
	movq [rcx],xmm0
	align 16
i32a2daexit:
	ret