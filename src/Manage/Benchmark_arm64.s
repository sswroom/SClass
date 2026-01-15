	.global	Benchmark_MemCopyTest
	.global	Benchmark_MemWriteTest
	.global	Benchmark_MemReadTest

//void Benchmark_MemCopyTest(UInt8 *buff1, UInt8 *buff2, IntOS buffSize, IntOS loopCnt);
	.align	2
Benchmark_MemCopyTest:
	stmfd	sp!, {x4, x5, lr}
	add	x2,x2,x0
	b	mctlop3
mctlop1:
	mov	x4, x0
	mov	x5, x1
mctlop2:
	vld2.16	{d0,d1}, [x4]
	vst2.16	{d0,d1}, [x5]
	add	x4, x4, #16
	add	x5, x5, #16
	cmp	x4, x2
	bne	mctlop2
	sub	x3, x3, #1
mctlop3:
	cmp	x3, #0
	bgt	mctlop1
	ldmfd	sp!, {x4, x5, pc}

//void Benchmark_MemWriteTest(UInt8 *buff1, UInt8 *buff2, IntOS buffSize, IntOS loopCnt);
	.align	2
Benchmark_MemWriteTest:
	stmfd	sp!, {x4, lr}
	add	x2, x2, x1
	vsub.u32 q0,q0,q0
	b	mwtlop3
mwtlop1:
	mov	x0, x1
mwtlop2:
	vst2.16	{d0,d1}, [x0]
	add	x0, x0, #16
	cmp	x0, x2
	bne	mwtlop2
	sub	x3, x3, #1
mwtlop3:
	cmp	x3, #0
	bgt	mwtlop1
	ldmfd	sp!, {x4, pc}


//void Benchmark_MemReadTest(UInt8 *buff1, UInt8 *buff2, IntOS buffSize, IntOS loopCnt);
	.align	2
Benchmark_MemReadTest:
	stmfd	sp!, {x4, lr}
	add	x2, x2, x0
	b	mrtlop3
mrtlop1:
	mov	x1, x0
mrtlop2:
	vld2.16	{d0,d1}, [x1]
	add	x1, x1, #16
	cmp	x1, x2
	bne	mrtlop2
	sub	x3, x3, #1
mrtlop3:
	cmp	x3, #0
	bgt	mrtlop1
	ldmfd	sp!, {x4, pc}

