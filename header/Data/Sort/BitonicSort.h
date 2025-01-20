#ifndef _SM_DATA_SORT_BITONICSORT
#define _SM_DATA_SORT_BITONICSORT
#include "AnyType.h"
#include "Data/Comparable.h"
#include "Data/SyncArrayList.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"

namespace Data
{
	namespace Sort
	{
		class BitonicSort
		{
		private:
			typedef struct
			{
				BitonicSort *me;
				Bool toStop;
				Int32 state; // 0 = not running, 1 = running, 2 = waiting
				Sync::Event *evt;
				void *nextThread;
			} ThreadStat;

			typedef enum
			{
				AT_INT32,
				AT_UINT32,
				AT_STRC
			} ArrayType;

			typedef struct
			{
				void *arr; // arr = 0 when end
				OSInt arrLen;
				Bool dir;
				OSInt m;
				ArrayType arrType;
				Sync::Event *notifyEvt;
			} TaskInfo;
		private:
			UOSInt threadCnt;
			ThreadStat *threads;
			ThreadStat mainThread;
			Data::SyncArrayList<TaskInfo*> tasks;

			static void DoMergeInt32(NN<ThreadStat> stat, Int32 *arr, OSInt n, Bool dir, OSInt m);
			static void DoMergeUInt32(NN<ThreadStat> stat, UInt32 *arr, OSInt n, Bool dir, OSInt m);
			Bool DoTask(NN<ThreadStat> stat);
			void SortInnerInt32(Int32 *arr, OSInt n, Bool dir, OSInt pw2);
			void SortInnerUInt32(UInt32 *arr, OSInt n, Bool dir, OSInt pw2);
			static UInt32 __stdcall ProcessThread(AnyType userObj);
		public:
			BitonicSort();
			~BitonicSort();

			void SortInt32(Int32 *arr, OSInt firstIndex, OSInt lastIndex);
			void SortUInt32(UInt32 *arr, OSInt firstIndex, OSInt lastIndex);
			void SortStr(UTF8Char **arr, OSInt firstIndex, OSInt lastIndex);
		};
	}
}
#endif
