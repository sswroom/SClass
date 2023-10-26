section .text

global Benchmark_MemCopyTest
global Benchmark_MemWriteTest
global Benchmark_MemReadTest

%ifdef __CYGWIN__
extern _CPUBrand
extern _UseAVX

%define UseAVX _UseAVX
%define CPUBrand _CPUBrand
%else
extern _CPUBrand
extern _UseAVX
%endif

;Benchmark_MemCopyTest(UInt8 *buff1, UInt8 *buff2, OSInt buffSize, OSInt loopCnt)

;0 rdi
;8 rsi
;16 retAddr
;rcx buff1
;rdx buff2
;r8 buffSize
;r9 loopCnt
	align 16
Benchmark_MemCopyTest:
	cmp dword [rel CPUBrand],2
	jz Benchmark_MemCopyTest_AMD
	cmp r8,256
	jb Benchmark_MemCopyTest_SSE
	cmp dword [rel UseAVX],0
	jnz Benchmark_MemCopyTest_AVX
	align 16
Benchmark_MemCopyTest_SSE:
	push rsi
	push rdi
	mov r10,rcx
	align 16
mcopylop:
	mov rsi,r10 ;buff1
	mov rdi,rdx ;buff2
	mov rcx,r8 ;buffSize
	shr rcx,7
	align 16
mcopylop3b:
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
	dec rcx
	jnz mcopylop3b
	
	align 16
mcopylop4:
	dec r9 ;loopCnt
	jnz mcopylop
	
	pop rdi
	pop rsi
	ret

	align 16
Benchmark_MemCopyTest_AVX:
	push rsi
	push rdi
	mov r10,rcx
	align 16
mcopyavxlop:
	mov rsi,r10 ;buff1
	mov rdi,rdx ;buff2
	mov rcx,r8 ;buffSize
	shr rcx,8
	align 16
mcopyavxlop3b:
	vmovaps ymm0,[rsi]
	vmovaps ymm1,[rsi+32]
	vmovaps ymm2,[rsi+64]
	vmovaps ymm3,[rsi+96]
	vmovaps ymm4,[rsi+128]
	vmovaps ymm5,[rsi+160]
	vmovaps ymm6,[rsi+192]
	vmovaps ymm7,[rsi+224]
	vmovntps [rdi],ymm0
	vmovntps [rdi+32],ymm1
	vmovntps [rdi+64],ymm2
	vmovntps [rdi+96],ymm3
	vmovntps [rdi+128],ymm4
	vmovntps [rdi+160],ymm5
	vmovntps [rdi+192],ymm6
	vmovntps [rdi+224],ymm7
	lea rsi,[rsi+256]
	lea rdi,[rdi+256]
	dec rcx
	jnz mcopyavxlop3b
	
	align 16
mcopyavxlop4:
	dec r9 ;loopCnt
	jnz mcopyavxlop
	
	vzeroupper
	pop rdi
	pop rsi
	ret

	align 16
Benchmark_MemCopyTest_AMD:
	cmp r8,256
	jb Benchmark_MemCopyTest_AMDSSE
	cmp dword [rel UseAVX],0
	jnz Benchmark_MemCopyTest_AMDAVX
	align 16
Benchmark_MemCopyTest_AMDSSE:
	push rsi
	push rdi
	mov r10,rcx
	align 16
mcopyamdlop:
	mov rsi,r10 ;buff1
	mov rdi,rdx ;buff2
	mov rcx,r8 ;buffSize
	shr rcx,7
	align 16
mcopyamdlop3b:
	movdqa xmm0,[rsi]
	movdqa xmm1,[rsi+16]
	movdqa xmm2,[rsi+32]
	movdqa xmm3,[rsi+48]
	movdqa xmm4,[rsi+64]
	movdqa xmm5,[rsi+80]
	movdqa xmm6,[rsi+96]
	movdqa xmm7,[rsi+112]
	movntdq [rdi],xmm0
	movntdq [rdi+16],xmm1
	movntdq [rdi+32],xmm2
	movntdq [rdi+48],xmm3
	movntdq [rdi+64],xmm4
	movntdq [rdi+80],xmm5
	movntdq [rdi+96],xmm6
	movntdq [rdi+112],xmm7
	lea rsi,[rsi+128]
	lea rdi,[rdi+128]
	dec rcx
	jnz mcopyamdlop3b
	
	align 16
mcopyamdlop4:
	dec r9 ;loopCnt
	jnz mcopyamdlop
	
	pop rdi
	pop rsi
	ret

	align 16
Benchmark_MemCopyTest_AMDAVX:
	push rsi
	push rdi
	mov r10,rcx
	align 16
mcopyamdavxlop:
	mov rsi,r10 ;buff1
	mov rdi,rdx ;buff2
	mov rcx,r8 ;buffSize
	shr rcx,8
	align 16
mcopyamdavxlop3b:
	vmovdqa ymm0,[rsi]
	vmovdqa ymm1,[rsi+32]
	vmovdqa ymm2,[rsi+64]
	vmovdqa ymm3,[rsi+96]
	vmovdqa ymm4,[rsi+128]
	vmovdqa ymm5,[rsi+160]
	vmovdqa ymm6,[rsi+192]
	vmovdqa ymm7,[rsi+224]
	vmovntdq [rdi],ymm0
	vmovntdq [rdi+32],ymm1
	vmovntdq [rdi+64],ymm2
	vmovntdq [rdi+96],ymm3
	vmovntdq [rdi+128],ymm4
	vmovntdq [rdi+160],ymm5
	vmovntdq [rdi+192],ymm6
	vmovntdq [rdi+224],ymm7
	lea rsi,[rsi+256]
	lea rdi,[rdi+256]
	dec rcx
	jnz mcopyamdavxlop3b
	
	align 16
mcopyamdavxlop4:
	dec r9 ;loopCnt
	jnz mcopyamdavxlop
	
	vzeroupper
	pop rdi
	pop rsi
	ret

;Benchmark_MemWriteTest(UInt8 *buff1, UInt8 *buff2, OSInt buffSize, OSInt loopCnt)

;0 rdi
;8 rsi
;16 retAddr
;rcx buff1
;rdx buff2
;r8 buffSize
;r9 loopCnt
	align 16
Benchmark_MemWriteTest:
	cmp dword [rel CPUBrand],2
	jz Benchmark_MemWriteTest_AMD
	cmp r8,128
	jb Benchmark_MemWriteTest_SSE
	cmp dword [rel UseAVX],0
	jnz Benchmark_MemWriteTest_AVX
	align 16
Benchmark_MemWriteTest_SSE:
	push rsi
	push rdi
	mov r10,rcx
	pxor xmm0,xmm0
	pxor xmm1,xmm1
	pxor xmm2,xmm2
	pxor xmm3,xmm3
	align 16
mwritelop:
	mov rsi,r10 ;buff1
	mov rdi,rdx ;buff2
	mov rcx,r8 ;buffSize
	xor rax,rax
	shr rcx,6
	align 16
mwritelop2:
	movntps [rdi],xmm0
	movntps [rdi+16],xmm1
	movntps [rdi+32],xmm2
	movntps [rdi+48],xmm3
	lea rdi,[rdi+64]
	dec rcx
	jnz mwritelop2
	
	dec r9 ;loopCnt
	jnz mwritelop
	
	pop rdi
	pop rsi
	ret

	align 16
Benchmark_MemWriteTest_AVX:
	push rsi
	push rdi
	mov r10,rcx
	vpxor ymm0,ymm0,ymm0
	vpxor ymm1,ymm1,ymm1
	vpxor ymm2,ymm2,ymm2
	vpxor ymm3,ymm3,ymm3
	align 16
mwriteavxlop:
	mov rsi,r10 ;buff1
	mov rdi,rdx ;buff2
	mov rcx,r8 ;buffSize
	xor rax,rax
	shr rcx,7
	align 16
mwriteavxlop2:
	vmovntps [rdi],ymm0
	vmovntps [rdi+32],ymm1
	vmovntps [rdi+64],ymm2
	vmovntps [rdi+96],ymm3
	lea rdi,[rdi+128]
	dec rcx
	jnz mwriteavxlop2
	
	dec r9 ;loopCnt
	jnz mwriteavxlop
	
	vzeroupper
	pop rdi
	pop rsi
	ret

	align 16
Benchmark_MemWriteTest_AMD:
	cmp r8,128
	jb Benchmark_MemWriteTest_AMDSSE
	cmp dword [rel UseAVX],0
	jnz Benchmark_MemWriteTest_AMDAVX
	align 16
Benchmark_MemWriteTest_AMDSSE:
	push rsi
	push rdi
	mov r10,rcx
	pxor xmm0,xmm0
	pxor xmm1,xmm1
	pxor xmm2,xmm2
	pxor xmm3,xmm3
	align 16
mwriteamdlop:
	mov rsi,r10 ;buff1
	mov rdi,rdx ;buff2
	mov rcx,r8 ;buffSize
	xor rax,rax
	shr rcx,6
	align 16
mwriteamdlop2:
	movntdq [rdi],xmm0
	movntdq [rdi+16],xmm1
	movntdq [rdi+32],xmm2
	movntdq [rdi+48],xmm3
	lea rdi,[rdi+64]
	dec rcx
	jnz mwriteamdlop2
	
	dec r9 ;loopCnt
	jnz mwriteamdlop
	
	pop rdi
	pop rsi
	ret

	align 16
Benchmark_MemWriteTest_AMDAVX:
	push rsi
	push rdi
	mov r10,rcx
	vpxor ymm0,ymm0,ymm0
	vpxor ymm1,ymm1,ymm1
	vpxor ymm2,ymm2,ymm2
	vpxor ymm3,ymm3,ymm3
	align 16
mwriteamdavxlop:
	mov rsi,r10 ;buff1
	mov rdi,rdx ;buff2
	mov rcx,r8 ;buffSize
	xor rax,rax
	shr rcx,7
	align 16
mwriteamdavxlop2:
	vmovntdq [rdi],ymm0
	vmovntdq [rdi+32],ymm1
	vmovntdq [rdi+64],ymm2
	vmovntdq [rdi+96],ymm3
	lea rdi,[rdi+128]
	dec rcx
	jnz mwriteavxlop2
	
	dec r9 ;loopCnt
	jnz mwriteamdavxlop
	
	vzeroupper
	pop rdi
	pop rsi
	ret

;Benchmark_MemReadTest(UInt8 *buff1, UInt8 *buff2, OSInt buffSize, OSInt loopCnt)

;0 rdi
;8 rsi
;16 retAddr
;rcx buff1
;rdx buff2
;r8 buffSize
;r9 loopCnt
	align 16
Benchmark_MemReadTest:
	cmp dword [rel CPUBrand],2
	jz Benchmark_MemReadTest_AMD
	cmp r8,128
	jb Benchmark_MemReadTest_SSE
	cmp dword [rel UseAVX],0
	jnz Benchmark_MemReadTest_AVX
	align 16
Benchmark_MemReadTest_SSE:
	push rsi
	push rdi
	mov r10,rcx
	align 16
mreadlop:
	mov rsi,r10 ;buff1
	mov rdi,rdx ;buff2
	mov rcx,r8 ;buffSize
	xor rax,rax
	shr rcx,6
	align 16
mreadlop2:
	movaps xmm0,[rsi]
	movaps xmm1,[rsi+16]
	movaps xmm2,[rsi+32]
	movaps xmm3,[rsi+48]
	lea rsi,[rsi+64]
	dec rcx
	jnz mreadlop2
	
	dec r9 ;loopCnt
	jnz mreadlop
	
	pop rdi
	pop rsi
	ret

	align 16
Benchmark_MemReadTest_AVX:
	push rsi
	push rdi
	mov r10,rcx
	align 16
mreadavxlop:
	mov rsi,r10 ;buff1
	mov rdi,rdx ;buff2
	mov rcx,r8 ;buffSize
	xor rax,rax
	shr rcx,7
	align 16
mreadavxlop2:
	vmovaps ymm0,[rsi]
	vmovaps ymm1,[rsi+32]
	vmovaps ymm2,[rsi+64]
	vmovaps ymm3,[rsi+96]
	lea rsi,[rsi+128]
	dec rcx
	jnz mreadavxlop2
	
	dec r9 ;loopCnt
	jnz mreadavxlop
	
	vzeroupper
	pop rdi
	pop rsi
	ret

	align 16
Benchmark_MemReadTest_AMD:
	cmp r8,128
	jb Benchmark_MemReadTest_AMDSSE
	cmp dword [rel UseAVX],0
	jnz Benchmark_MemReadTest_AMDAVX
	align 16
Benchmark_MemReadTest_AMDSSE:
	push rsi
	push rdi
	mov r10,rcx
	align 16
mreadamdlop:
	mov rsi,r10 ;buff1
	mov rdi,rdx ;buff2
	mov rcx,r8 ;buffSize
	xor rax,rax
	shr rcx,6
	align 16
mreadamdlop2:
	movdqa xmm0,[rsi]
	movdqa xmm1,[rsi+16]
	movdqa xmm2,[rsi+32]
	movdqa xmm3,[rsi+48]
	lea rsi,[rsi+64]
	dec rcx
	jnz mreadamdlop2
	
	dec r9 ;loopCnt
	jnz mreadamdlop
	
	pop rdi
	pop rsi
	ret

	align 16
Benchmark_MemReadTest_AMDAVX:
	push rsi
	push rdi
	mov r10,rcx
	align 16
mreadamdavxlop:
	mov rsi,r10 ;buff1
	mov rdi,rdx ;buff2
	mov rcx,r8 ;buffSize
	xor rax,rax
	shr rcx,7
	align 16
mreadamdavxlop2:
	vmovdqa ymm0,[rsi]
	vmovdqa ymm1,[rsi+32]
	vmovdqa ymm2,[rsi+64]
	vmovdqa ymm3,[rsi+96]
	lea rsi,[rsi+128]
	dec rcx
	jnz mreadamdavxlop2
	
	dec r9 ;loopCnt
	jnz mreadamdavxlop
	
	vzeroupper
	pop rdi
	pop rsi
	ret

