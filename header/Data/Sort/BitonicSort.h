#ifndef _SM_DATA_SORT_BITONICSORT
#define _SM_DATA_SORT_BITONICSORT
#include "AnyType.h"
#include "Data/Comparable.h"
#include "Data/SyncArrayListNN.hpp"
#include "Sync/Event.h"
#include "Sync/Mutex.h"

namespace Data
{
	namespace Sort
	{
		class BitonicSort
		{
		private:
			struct ThreadStat
			{
				NN<BitonicSort> me;
				Bool toStop;
				Int32 state; // 0 = not running, 1 = running, 2 = waiting
				NN<Sync::Event> evt;
				Optional<ThreadStat> nextThread;
			};

			typedef enum
			{
				AT_INT32,
				AT_UINT32,
				AT_STRC
			} ArrayType;

			typedef struct
			{
				AnyType arr; // arr = 0 when end
				IntOS arrLen;
				Bool dir;
				IntOS m;
				ArrayType arrType;
				NN<Sync::Event> notifyEvt;
			} TaskInfo;
		private:
			UIntOS threadCnt;
			UnsafeArray<ThreadStat> threads;
			ThreadStat mainThread;
			Data::SyncArrayListNN<TaskInfo> tasks;

			static void DoMergeInt32(NN<ThreadStat> stat, UnsafeArray<Int32> arr, IntOS n, Bool dir, IntOS m);
			static void DoMergeUInt32(NN<ThreadStat> stat, UnsafeArray<UInt32> arr, IntOS n, Bool dir, IntOS m);
			Bool DoTask(NN<ThreadStat> stat);
			void SortInnerInt32(UnsafeArray<Int32> arr, IntOS n, Bool dir, IntOS pw2);
			void SortInnerUInt32(UnsafeArray<UInt32> arr, IntOS n, Bool dir, IntOS pw2);
			static UInt32 __stdcall ProcessThread(AnyType userObj);
		public:
			BitonicSort();
			~BitonicSort();

			void SortInt32(UnsafeArray<Int32> arr, IntOS firstIndex, IntOS lastIndex);
			void SortUInt32(UnsafeArray<UInt32> arr, IntOS firstIndex, IntOS lastIndex);
			void SortStr(UnsafeArray<UnsafeArray<UTF8Char>> arr, IntOS firstIndex, IntOS lastIndex);
		};
	}
}
#endif
