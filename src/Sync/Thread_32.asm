section .text

global _Thread_NanoSleep

;void Thread_NanoSleep(Int64 clk)
;0 ebx
;4 retAddr
;8 clk
;12 clk[4]
	align 16
_Thread_NanoSleep:
	push ebx
	mov ebx,dword [esp+8] ;clk
	mov ecx,dword [esp+12] ;clk
	rdtsc
	add ebx,eax
	adc ecx,edx
	
	align 16
nslop:
	pause
	pause
	pause
	pause
	rdtsc
	cmp edx,ecx
	jb nslop
	ja nslop2
	cmp eax,ebx
	jb nslop
nslop2
	pop ebx
	ret
