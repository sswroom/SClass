section .text

global X86Util_ncpuid

;void X86Util_ncpuid(Int32 *cpuInfo, Int32 func, Int32 subfunc);
;0 retAddr
;rcx cpuInfo
;rdx func
;r8 subfunc
	align 16
X86Util_ncpuid:
	mov r9,rbx
	mov r10,rcx
	mov eax,edx
	mov ecx,r8d
	cpuid
	mov dword [r10],eax
	mov dword [r10+4],ebx
	mov dword [r10+8],ecx
	mov dword [r10+12],edx
	mov rbx,r9
	ret