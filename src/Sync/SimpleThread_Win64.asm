section .text

global SimpleThread_NanoSleep

;void SimpleThread_NanoSleep(Int64 clk)
	align 16
SimpleThread_NanoSleep:
	rdtsc
	shl rdx,32
	or rax,rdx
	add rcx,rax
	
	align 16
nslop:
	pause
	pause
	pause
	pause
	rdtsc
	shl rdx,32
	or rax,rdx
	cmp rax,rcx
	jb nslop
	ret
