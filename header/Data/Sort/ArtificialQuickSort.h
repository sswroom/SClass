#ifndef _SM_DATA_SORT_ARTIFICIALQUICKSORT
#define _SM_DATA_SORT_ARTIFICIALQUICKSORT
#include "Data/Sort/ArtificialQuickSortC.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"

//http://www.critticall.com/ArtificialSort.html

namespace Data
{
	namespace Sort
	{
		class ArtificialQuickSort
		{
		private:
			typedef struct
			{
				ArtificialQuickSort *me;
				UOSInt threadId;
				Bool toStop;
				Int32 state; // 0 = not running, 1 = idle, 2 = processing
				Sync::Event *evt;
			} ThreadStat;

			typedef enum
			{
				AT_INT32,
				AT_UINT32,
				AT_STRUTF8
			} ArrayType;
		private:
			UOSInt threadCnt;
			ThreadStat *threads;
			Sync::Event *mainEvt;

			Sync::Mutex *mut;
			void *arr;
			ArrayType arrType;
			OSInt *tasks;
			UOSInt taskCnt;

			void DoSortInt32(ThreadStat *stat, Int32 *arr, OSInt firstIndex, OSInt lastIndex);
			void DoSortUInt32(ThreadStat *stat, UInt32 *arr, OSInt firstIndex, OSInt lastIndex);
			void DoSortStr(ThreadStat *stat, UTF8Char **arr, OSInt firstIndex, OSInt lastIndex);
			static UInt32 __stdcall ProcessThread(void *userObj);
		public:
			ArtificialQuickSort();
			~ArtificialQuickSort();

			void SortInt32(Int32 *arr, OSInt firstIndex, OSInt lastIndex);
			void SortUInt32(UInt32 *arr, OSInt firstIndex, OSInt lastIndex);
			void SortStr(UTF8Char **arr, OSInt firstIndex, OSInt lastIndex);
		};
	}
}
#endif
