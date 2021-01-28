section .text

global Benchmark_MemCopyTest
global _Benchmark_MemCopyTest
global Benchmark_MemWriteTest
global _Benchmark_MemWriteTest
global Benchmark_MemReadTest
global _Benchmark_MemReadTest

extern _CPUBrand
extern _UseAVX

;Benchmark_MemCopyTest(UInt8 *buff1, UInt8 *buff2, OSInt buffSize, OSInt loopCnt)

;0 retAddr
;rdi buff1
;rsi buff2
;rdx buffSize
;rcx loopCnt
	align 16
Benchmark_MemCopyTest:
_Benchmark_MemCopyTest:
	mov r8,rdi
	mov r9,rsi
	mov r10,rdx
	align 16
t12lop:
	mov rsi,r8 ;buff1
	mov rdi,r9 ;buff2
	mov rdx,r10 ;buffSize
	shr rdx,7
	align 16
t12lop2:
	movaps xmm0,[rsi]
	movaps xmm1,[rsi+16]
	movaps xmm2,[rsi+32]
	movaps xmm3,[rsi+48]
	movaps xmm4,[rsi+64]
	movaps xmm5,[rsi+80]
	movaps xmm6,[rsi+96]
	movaps xmm7,[rsi+112]
	movntps [rdi],xmm0
	movntps [rdi+16],xmm1
	movntps [rdi+32],xmm2
	movntps [rdi+48],xmm3
	movntps [rdi+64],xmm4
	movntps [rdi+80],xmm5
	movntps [rdi+96],xmm6
	movntps [rdi+112],xmm7
	lea rsi,[rsi+128]
	lea rdi,[rdi+128]
	dec rdx
	jnz t12lop2
	
	dec rcx ;loopCnt
	jnz t12lop
	
	ret

;Benchmark_MemWriteTest(UInt8 *buff1, UInt8 *buff2, OSInt buffSize, OSInt loopCnt)
;0 retAddr
;rdi buff1
;rsi buff2
;rdx buffSize
;rcx loopCnt
	align 16
Benchmark_MemWriteTest:
_Benchmark_MemWriteTest:
	cmp rdx,256
	jb Benchmark_MemWriteTest_SSE
	cmp dword [rel _UseAVX],0
	jnz Benchmark_MemWriteTest_AVX

	align 16
Benchmark_MemWriteTest_SSE:
	mov r8,rdi
	mov r9,rsi
	mov r10,rdx
	pxor xmm0,xmm0
	pxor xmm1,xmm1
	pxor xmm2,xmm2
	pxor xmm3,xmm3
	pxor xmm4,xmm4
	pxor xmm5,xmm5
	pxor xmm6,xmm6
	pxor xmm7,xmm7
	align 16
mwritelop:
	mov rsi,r8 ;buff1
	mov rdi,r9 ;buff2
	mov rdx,r10 ;buffSize
	xor rax,rax
	shr rdx,7
	align 16
mwritelop2:
	movntps [rdi],xmm0
	movntps [rdi+16],xmm1
	movntps [rdi+32],xmm2
	movntps [rdi+48],xmm3
	movntps [rdi+64],xmm4
	movntps [rdi+80],xmm5
	movntps [rdi+96],xmm6
	movntps [rdi+112],xmm7
	lea rdi,[rdi+128]
	dec rdx
	jnz mwritelop2
	
	dec rcx ;loopCnt
	jnz mwritelop

	ret

	align 16
Benchmark_MemWriteTest_AVX:
	mov r8,rdi
	mov r9,rsi
	mov r10,rdx
	vpxor ymm0,ymm0
	vpxor ymm1,ymm1
	vpxor ymm2,ymm2
	vpxor ymm3,ymm3
	vpxor ymm4,ymm4
	vpxor ymm5,ymm5
	vpxor ymm6,ymm6
	vpxor ymm7,ymm7
	align 16
mwriteavxlop:
	mov rsi,r8 ;buff1
	mov rdi,r9 ;buff2
	mov rdx,r10 ;buffSize
	xor rax,rax
	shr rdx,8
	align 16
mwriteavxlop2:
	vmovntps [rdi],ymm0
	vmovntps [rdi+32],ymm1
	vmovntps [rdi+64],ymm2
	vmovntps [rdi+96],ymm3
	vmovntps [rdi+128],ymm4
	vmovntps [rdi+160],ymm5
	vmovntps [rdi+192],ymm6
	vmovntps [rdi+224],ymm7
	lea rdi,[rdi+256]
	dec rdx
	jnz mwriteavxlop2
	
	dec rcx ;loopCnt
	jnz mwriteavxlop
	vzeroupper

	ret

;Benchmark_MemReadTest(UInt8 *buff1, UInt8 *buff2, OSInt buffSize, OSInt loopCnt)
;0 retAddr
;rdi buff1
;rsi buff2
;rdx buffSize
;rcx loopCnt
	align 16
Benchmark_MemReadTest:
_Benchmark_MemReadTest:
	cmp rdx,256
	jb Benchmark_MemReadTest_SSE
	cmp dword [rel _UseAVX],0
	jnz Benchmark_MemReadTest_AVX

	align 16
Benchmark_MemReadTest_SSE:
	mov r8,rdi
	mov r9,rsi
	mov r10,rdx
	align 16
mreadlop:
	mov rsi,r8 ;buff1
	mov rdi,r9 ;buff2
	mov rdx,r10 ;buffSize
	xor rax,rax
	shr rdx,7
	align 16
mreadlop2:
	prefetcht0 [rsi+512]
	movaps xmm0,[rsi]
	movaps xmm1,[rsi+16]
	movaps xmm2,[rsi+32]
	movaps xmm3,[rsi+48]
	movaps xmm4,[rsi+64]
	movaps xmm5,[rsi+80]
	movaps xmm6,[rsi+96]
	movaps xmm7,[rsi+112]
	lea rsi,[rsi+128]
	dec rdx
	jnz mreadlop2
	
	dec rcx ;loopCnt
	jnz mreadlop
	
	ret

	align 16
Benchmark_MemReadTest_AVX:
	mov r8,rdi
	mov r9,rsi
	mov r10,rdx
	align 16
mreadavxlop:
	mov rsi,r8 ;buff1
	mov rdi,r9 ;buff2
	mov rdx,r10 ;buffSize
	xor rax,rax
	shr rdx,8
	align 16
mreadavxlop2:
	vmovaps ymm0,[rsi]
	vmovaps ymm1,[rsi+32]
	vmovaps ymm2,[rsi+64]
	vmovaps ymm3,[rsi+96]
	vmovaps ymm4,[rsi+128]
	vmovaps ymm5,[rsi+160]
	vmovaps ymm6,[rsi+192]
	vmovaps ymm7,[rsi+224]
	lea rsi,[rsi+256]
	dec rdx
	jnz mreadavxlop2
	
	dec rcx ;loopCnt
	jnz mreadavxlop
	vzeroupper

	ret

