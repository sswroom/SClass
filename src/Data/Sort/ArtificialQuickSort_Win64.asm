section .text

global ArtificialQuickSort_DoPreSortInt32_2

;void ArtificialQuickSort_DoPreSortInt32_2(Int32 *arr, OSInt firstIndex, OSInt lastIndex)
;0 retAddr
;rcx arr
;rdx firstIndex
;r8 lastIndex
	align 16
ArtificialQuickSort_DoPreSortInt32_2:
	cmp rdx,r8
	jge presortexit
	
	align 16
presortlop2:
	mov r10d,dword [rcx+rdx*4]
	mov r9d,dword [rcx+r8*4]
	cmp r10d,r9d
	jle presortlop
	
	mov dword [rcx+r8*4],r10d
	mov dword [rcx+rdx*4],r9d
	
	align 16
presortlop:
	inc rdx
	dec r8
	cmp rdx,rax
	jl presortlop2
	
	align 16
presortexit:
	ret
