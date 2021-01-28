#ifndef _SM_MANAGE_BENCHMARK
#define _SM_MANAGE_BENCHMARK

extern "C"
{
	void Benchmark_MemCopyTest(UInt8 *buff1, UInt8 *buff2, OSInt buffSize, OSInt loopCnt);
	void Benchmark_MemWriteTest(UInt8 *buff1, UInt8 *buff2, OSInt buffSize, OSInt loopCnt);
	void Benchmark_MemReadTest(UInt8 *buff1, UInt8 *buff2, OSInt buffSize, OSInt loopCnt);
}

#endif
