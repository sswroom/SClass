section .text

global _Benchmark_MemCopyTest
global Benchmark_MemCopyTest
global _Benchmark_MemWriteTest
global Benchmark_MemWriteTest
global _Benchmark_MemReadTest
global Benchmark_MemReadTest

;Benchmark_MemCopyTest(UInt8 *buff1, UInt8 *buff2, Int32 buffSize, Int32 loopCnt)

;0 edi
;4 ebx
;8 esi
;12 ebp
;16 retAddr
;20 buff1
;24 buff2
;28 buffSize
;32 loopCnt
	align 16
_Benchmark_MemCopyTest:
Benchmark_MemCopyTest:
	push ebp
	push esi
	push ebx
	push edi
	mov edx,dword [esp+32]
	align 16
t12lop:
	mov ecx,dword [esp+28]
	mov esi,dword [esp+20]
	mov edi,dword [esp+24]
	shr ecx,6
	align 16
t12lop2:
	movaps xmm0,[esi]
	movaps xmm1,[esi+16]
	movaps xmm2,[esi+32]
	movaps xmm3,[esi+48]
	movntps [edi],xmm0
	movntps [edi+16],xmm1
	movntps [edi+32],xmm2
	movntps [edi+48],xmm3
	add esi,64
	add edi,64
	dec ecx
	jnz t12lop2
	
	dec edx
	jnz t12lop
	
	pop edi
	pop ebx
	pop esi
	pop ebp
	ret

;Benchmark_MemWriteTest(UInt8 *buff1, UInt8 *buff2, Int32 buffSize, Int32 loopCnt)

;0 edi
;4 ebx
;8 esi
;12 ebp
;16 retAddr
;20 buff1
;24 buff2
;28 buffSize
;32 loopCnt

	align 16
_Benchmark_MemWriteTest:
Benchmark_MemWriteTest:
	push ebp
	push esi
	push ebx
	push edi
	mov edx,dword [esp+32]
	pxor xmm0,xmm0
	pxor xmm1,xmm1
	pxor xmm2,xmm2
	pxor xmm3,xmm3
	align 16
t14lop:
	mov ecx,dword [esp+28]
	mov esi,dword [esp+20]
	mov edi,dword [esp+24]
	xor eax,eax
	shr ecx,6
	align 16
t14lop2:
	movntps [edi],xmm0
	movntps [edi+16],xmm1
	movntps [edi+32],xmm2
	movntps [edi+48],xmm3
	add edi,64
	dec ecx
	jnz t14lop2
	
	dec edx
	jnz t14lop
	
	pop edi
	pop ebx
	pop esi
	pop ebp
	ret

;Benchmark_MemReadTest(UInt8 *buff1, UInt8 *buff2, Int32 buffSize, Int32 loopCnt)

;0 edi
;4 ebx
;8 esi
;12 ebp
;16 retAddr
;20 buff1
;24 buff2
;28 buffSize
;32 loopCnt

	align 16
_Benchmark_MemReadTest:
Benchmark_MemReadTest:
	push ebp
	push esi
	push ebx
	push edi
	mov edx,dword [esp+32]
	align 16
t15lop:
	mov ecx,dword [esp+28]
	mov esi,dword [esp+20]
	mov edi,dword [esp+24]
	xor eax,eax
	shr ecx,6
	align 16
t15lop2:
	prefetcht0 [esi+512]
	movaps xmm0,[esi]
	movaps xmm1,[esi+16]
	movaps xmm2,[esi+32]
	movaps xmm3,[esi+48]
	lea esi,[esi+64]
	dec ecx
	jnz t15lop2
	
	dec edx
	jnz t15lop
	
	pop edi
	pop ebx
	pop esi
	pop ebp
	ret

