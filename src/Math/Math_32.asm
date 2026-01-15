section .text

global _Math_Int32Arr2DblArr

;void Math_Int32Arr2DblArr(Double *dblArr, Int32 *intArr, IntOS arrCnt)
;0 retAddr
;4 dblArr
;8 intArr
;12 arrCnt;
	align 16
_Math_Int32Arr2DblArr:
	mov ecx,dword [esp+4]
	mov edx,dword [esp+8]
	mov eax,dword [esp+12]
	shr eax,1
	jz i32a2dalop2
	align 16
i32a2dalop:
	movq xmm0,[edx]
	cvtdq2pd xmm0,xmm0
	movdqu [ecx],xmm0
	lea edx,[edx+8]
	lea ecx,[ecx+16]
	dec eax
	jnz i32a2dalop
	test dword [esp+12],1
	jz i32a2daexit
	align 16
i32a2dalop2:
	cvtsi2sd xmm0,dword [edx]
	movq [ecx],xmm0
	align 16
i32a2daexit:
	ret