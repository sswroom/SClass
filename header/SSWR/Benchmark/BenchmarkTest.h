#ifndef _SM_SSWR_BENCHMARK_BENCHMARKTEST
#define _SM_SSWR_BENCHMARK_BENCHMARKTEST
#include "Data/ArrayListNN.h"

namespace SSWR
{
	namespace Benchmark
	{
		class BenchmarkTest
		{
		public:
			typedef struct
			{
				const UTF8Char *deviceLocator;
				const UTF8Char *manufacturer;
				const UTF8Char *partNo;
				const UTF8Char *sn;
				Int64 defSpdMHz;
				Int64 confSpdMHz;
				Int32 dataWidth;
				Int32 totalWidth;
				Int64 memorySize;
			} RAMInfo;

			typedef struct
			{
				UTF8Char monitorName[14];
				UTF8Char vendorName[4];
				UInt16 productCode;
				Int32 yearOfManu;
				UInt32 weekOfManu;
				UInt32 dispPhysicalW;
				UInt32 dispPhysicalH;
				Double wx;
				Double wy;
				Double rx;
				Double ry;
				Double gx;
				Double gy;
				Double bx;
				Double by;
			} MonitorInfo;
			
		private:
			const UTF8Char *platformName;
			const UTF8Char *cpuName;
			const UTF8Char *cpuModel;
			const UTF8Char *osDistro;
			const UTF8Char *osVersion;
			Int32 threadCnt;
			Int64 progBuildTime;
			Int64 testTime;
			Int64 totalMemSize;
			Int64 totalUsableMemSize;
			Data::ArrayListNN<RAMInfo> *ramList;
			Data::ArrayListNN<MonitorInfo> *monList;
			Double apiTimeGetTime;
			Double apiTimeGetThreadId;
			Double apiTimeMutexLockUnlock;
			Double apiTimeInterlockedInc;
			Double apiTimeEventSet;
			Double apiTimeEventWait;
			Double apiTimeThreadCreate;
			Double apiTimeThreadEventWake;
			Double apiTimeThreadMutexUnlock;
			Double apiTimeMemAlloc;
			Double apiTimeMemFree;
			Double apiTimeMemAllocA;
			Double apiTimeMemFreeA;
			Double apiTimeStrInt32UTF8;
			Double apiTimeStrInt32UTF16;
			Double apiTimeStrInt32UTF32;
			Double apiTimeStrInt64UTF8;
			Double apiTimeStrInt64UTF16;
			Double apiTimeStrInt64UTF32;
			Double apiTimeStrDoubleUTF8;
			Double apiTimeStrDoubleUTF16;
			Double apiTimeStrDoubleUTF32;
			UInt32 arrayListCount;
			Double apiTimeArrayListAdd;
			Double apiTimeArrayListRemoveLast;
			Double apiTimeArrayListSortedInsert;

			void ReadComputerInfo();
		public:
			BenchmarkTest();
			~BenchmarkTest();
		};
	}
}

#endif
