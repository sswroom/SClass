section .text

global _X86Util_ncpuid

;void X86Util_ncpuid(Int32 *cpuInfo, Int32 func);
;0 esi
;4 ebx
;8 retAddr
;12 cpuInfo
;16 func
	align 16
_X86Util_ncpuid:
	push ebx
	push esi
	mov eax,dword [esp+16]
	mov esi,dword [esp+12]
	cpuid
	mov dword [esi],eax
	mov dword [esi+4],ebx
	mov dword [esi+8],ecx
	mov dword [esi+12],edx
	pop esi
	pop ebx
	ret