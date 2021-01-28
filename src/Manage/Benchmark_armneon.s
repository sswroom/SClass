	.fpu neon
	.global	Benchmark_MemCopyTest
	.global	Benchmark_MemWriteTest
	.global	Benchmark_MemReadTest

//void Benchmark_MemCopyTest(UInt8 *buff1, UInt8 *buff2, OSInt buffSize, OSInt loopCnt);
	.align	2
Benchmark_MemCopyTest:
	stmfd	sp!, {r4, r5, lr}
	add	r2,r2,r0
	b	mctlop3
mctlop1:
	mov	r4, r0
	mov	r5,r1
mctlop2:
	vld2.16	{d0,d1},[r4]
	vst2.16	{d0,d1}, [r5]
	add	r4, r4, #16
	add	r5, r5, #16
	cmp	r4,r2
	bne	mctlop2
	sub	r3, r3, #1
mctlop3:
	cmp	r3, #0
	bgt	mctlop1
	ldmfd	sp!, {r4, r5, pc}

//void Benchmark_MemWriteTest(UInt8 *buff1, UInt8 *buff2, OSInt buffSize, OSInt loopCnt);
	.align	2
Benchmark_MemWriteTest:
	stmfd	sp!, {r4, lr}
	add	r2, r2, r1
	vsub.u32 q0,q0,q0
	b	mwtlop3
mwtlop1:
	mov	r0, r1
mwtlop2:
	vst2.16	{d0,d1}, [r0]
	add	r0, r0, #16
	cmp	r0, r2
	bne	mwtlop2
	sub	r3, r3, #1
mwtlop3:
	cmp	r3, #0
	bgt	mwtlop1
	ldmfd	sp!, {r4, pc}


//void Benchmark_MemReadTest(UInt8 *buff1, UInt8 *buff2, OSInt buffSize, OSInt loopCnt);
	.align	2
Benchmark_MemReadTest:
	stmfd	sp!, {r4, lr}
	add	r2, r2, r0
	b	mrtlop3
mrtlop1:
	mov	r1, r0
mrtlop2:
	vld2.16	{d0,d1}, [r1]
	add	r1, r1, #16
	cmp	r1,r2
	bne	mrtlop2
	sub	r3, r3, #1
mrtlop3:
	cmp	r3, #0
	bgt	mrtlop1
	ldmfd	sp!, {r4, pc}

